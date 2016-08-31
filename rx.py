import serial, sys, time
curFile = -1
f = None
ser = serial.Serial('/dev/ttyACM0',115200,timeout=1800000)
curPacket = -1
fileTimestamp = ""
logf = open("log_" + time.strftime("%Y%m%d_%H%M%S") + ".txt", 'w')
while True:
    line = ser.readline().decode("utf-8")
    logf.write(line + "\n")
    splitline = line.split("  ")
    if len(splitline) > 4:
        integers = [int(x.strip()) for x in splitline[:-1]]
        packet = bytearray(integers)
        fileno = (packet[0] << 8) | packet[1]
        packetid = (packet[2] << 8) | packet[3]

        print("File " + fileTimestamp + "_" + str(fileno) + ".jpg, packet seq " + str(packetid) + ", len " + str(len(packet)))

        if fileno != curFile:
            if curFile != -1:
                print("All done, saved to " + fileTimestamp + "_" + str(curFile) + ".jpg")
                f.close()
                if fileno != (curFile + 1):
                    if fileno == 0:
                        print("Warning: tx reset")
                    else:
                        print("File number did not increase sequentially - likely bad packet or tx failure")
                        f.write(bytearray(32))
                        #sys.exit(1)
                        continue
            fileTimestamp = time.strftime("%Y%m%d_%H%M%S")
            f = open(fileTimestamp + "_" + str(fileno) + ".jpg","wb")
            curPacket = -1
        curFile = fileno
        if (packetid != curPacket + 1):
            print("Warning: dropped packet, padding with zeros")
            f.write(bytearray(32))
        curPacket = packetid

        f.write(packet[4:])
