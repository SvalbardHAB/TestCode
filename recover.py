import sys, copy
def recursiveTryPermutations(data1, data2, remainingChunks, fileIndex):
    if len(remainingChunks) == 0:
        #check if data1 is a valid jpeg
        outFileName = sys.argv[1] + "_" + str(fileIndex) + ".jpg"
        print("Found valid permutation, saving to " + outFileName)
        with open(outFileName, 'wb') as f:
            f.write(bytearray(data1))
        fileIndex += 1
    else:
        tempData = list(data1)
        fileIndex = recursiveTryPermutations(tempData, data2, remainingChunks[1:], fileIndex)
        rangeStart, rangeEnd = remainingChunks[0]
        tempData[rangeStart:rangeEnd] = data2[rangeStart:rangeEnd]
        fileIndex = recursiveTryPermutations(tempData, data2, remainingChunks[1:], fileIndex)

    return fileIndex

if len(sys.argv) < 3:
    print("Usage: recover.py file1 file2")
    sys.exit(1)
with open(sys.argv[1], 'rb') as f1:
    data1 = f1.read()
with open(sys.argv[2], 'rb') as f2:
    data2 = f2.read()
if len(data1) != len(data2):
    print("Length mismatch; unable to recover")
    sys.exit(1)

#This assumes 32-byte packets and that for each packet it is only corrupted or lost
#in one of the files
mismatchedChunks = []
i = 0
while i < len(data1):
    rangeStart = i
    rangeEnd = min(i+32, len(data1))
    if data1[rangeStart:rangeEnd] != data2[rangeStart:rangeEnd]:
        mismatchedChunks.append((rangeStart, rangeEnd))
    i += 32

if len(mismatchedChunks) == 0:
    print("Files are identical")
else:
    if len(mismatchedChunks) > 12:
        print("Too many mismatched chunks(" + str(len(mismatchedChunks)) + "), trying first 12 only (this should recover header)")
        mismatchedChunks = mismatchedChunks[0:12]
    else:
        print(str(len(mismatchedChunks)) + " mismatched chunks, trying all permutations")
        recursiveTryPermutations(data1, data2, mismatchedChunks, 0)
