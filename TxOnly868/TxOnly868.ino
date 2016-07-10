#include <SPI.h>

#include <RFM98W_library.h>
RFMLib radio =RFMLib(10,6,255,2);
#define nss 10
  bool continuous = 1; //Set to true to transmit almost continuously (for PA testing)


void setup(){
  delay(1000);
  SPI.begin();
  Serial.begin(38400);
  Serial.println("Starting");
  byte my_config[6] = {0x84,0x84,0x88,0xAC,0xCD, 0x08};
  radio.high_frequency = 1;
  radio.configure(my_config);
  radio.setFrequency(869525000);
  Serial.print("LoRa IC version 0x");
  Serial.println(radio.getVersion(), HEX);
}
long lastTx = 0;
void loop(){
  if((radio.rfm_status ==0) && (((millis() - lastTx) > 1000) || continuous) ){
    RFMLib::Packet p;
    lastTx = millis();
    p.data[0]=255;
    p.data[1]=243;
    p.len = 2;
    radio.beginTX(p); 
    attachInterrupt(6,RFMISR,RISING);
  }

  if(radio.rfm_done){
        Serial.println("Ending");   
    radio.endTX();
  }
 
}

void RFMISR(){
  Serial.println("interrupt");
 radio.rfm_done = true; 
}


