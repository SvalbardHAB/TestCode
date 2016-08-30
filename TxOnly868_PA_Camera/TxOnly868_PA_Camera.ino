#include <Adafruit_VC0706.h>

#include <SPI.h>

#include <RFM98W_library.h>
RFMLib radio = RFMLib(10, 6, 255, 2);
#define nss 10
bool continuous =
    1; // Set to true to transmit almost continuously (for PA testing)
/// Test figures - ignore
// no PA, LoRa to 10dBm, open := SDR=-52dBm
// LoRa to 7dBm, +20dB PA to +27dBm, 54R dummy load :=- SDR=-38dBm
#define PA_EN 23
Adafruit_VC0706 cam = Adafruit_VC0706(&Serial3);

void setup() {
  pinMode(PA_EN, OUTPUT);
  digitalWrite(PA_EN, LOW);
  delay(1000);
  SPI.begin();
  Serial.begin(115200);
  Serial.println("Starting");
  byte my_config[6] = {0x84, 0x84, 0x85, 0xAC, 0xCD, 0x00};
  radio.high_frequency = 1;
  radio.configure(my_config);
  radio.setFrequency(869525000);
  Serial.print("LoRa IC version 0x");
  Serial.println(radio.getVersion(), HEX);

  if (cam.begin()) {
    Serial.println("Camera Found:");
  } else {
    Serial.println("No camera found?");
    return;
  }
  delay(500); // hopefully fix image size setting issue
  cam.setImageSize(VC0706_160x120);
}
int fileCount = 0;
int packetSeq = 0;

uint16_t jpglen = 0;
void loop() {

  if ((radio.rfm_status == 0)) {
    RFMLib::Packet p;
    if (jpglen == 0) {
      fileCount++;
      packetSeq = 0;
      cam.takePicture();
      jpglen = cam.frameLength();
    }
    p.data[0] = (fileCount >> 8) & 0xFF;
    p.data[1] = fileCount & 0xFF;

    p.data[2] = (packetSeq >> 8) & 0xFF;
    p.data[3] = packetSeq & 0xFF;

    uint8_t *buffer;
    uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but
                                           // may not work with all setups!
    buffer = cam.readPicture(bytesToRead);
    digitalWrite(PA_EN, HIGH);

    for (int i = 0; i < bytesToRead; i++) {
      p.data[i + 4] = buffer[i];
    }
    jpglen -= bytesToRead;
    p.len = bytesToRead + 4;

    packetSeq++;

    radio.beginTX(p);
    attachInterrupt(6, RFMISR, RISING);
    delay(5);
  }

  if (radio.rfm_done) {
    Serial.println("Ending");
    radio.endTX();
    digitalWrite(PA_EN, LOW);
  }
}

void RFMISR() {
  Serial.println("interrupt");
  radio.rfm_done = true;
}
