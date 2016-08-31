#include <SPI.h>

#include <RFM98W_library.h>
RFMLib radio = RFMLib(PB12, PC6, 16, 21);
#define nss PB12
void setup() {
  Serial.begin(115200);

  SPI.setModule(2);
  SPI.begin();
  byte my_config[6] = {0x88, 0x84, 0x85, 0xAC, 0xCD, 0x00};
  radio.high_frequency = true;
  radio.configure(my_config);
  radio.setFrequency(869525000);
  Serial.print("LoRa IC version 0x");
  Serial.println(radio.getVersion(), HEX);
}

void RFMISR() {
  // Serial.println("interrupt");
  radio.rfm_done = true;
}

void loop() {
  if (radio.rfm_status == 0) {
    radio.beginRX();
    attachInterrupt(PC6, RFMISR, RISING);
  }

  if (radio.rfm_done) {
    // Serial.println("Ending");
    RFMLib::Packet rx;
    radio.endRX(rx);
    Serial.println("=packet=");
    Serial.print("RSSI:");
    Serial.println(rx.rssi);
    Serial.print("SNR:");
    Serial.println(rx.snr);
    for (byte i = 0; i < rx.len; i++) {
      Serial.print(rx.data[i]);
      Serial.print("  ");
    }

    Serial.println();
  }
}
