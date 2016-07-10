#include <SPI.h>

#include <RFM98W_library.h>
RFMLib radio =RFMLib(20,7,16,21);
#define nss 20
void setup(){
  SPI.begin();
  Serial.begin(38400);
  byte my_config[6] = {0x84,0x84,0x88,0xAC,0xCD, 0x08};
  radio.high_frequency = true;
  radio.configure(my_config);
  radio.setFrequency(869525000);
  Serial.print("LoRa IC version 0x");
  Serial.println(radio.getVersion(), HEX);
}

void loop(){
  if(radio.rfm_status == 0){
    radio.beginRX(); 
    attachInterrupt(7,RFMISR,RISING);
  }

  if(radio.rfm_done){
        Serial.println("Ending");   
    RFMLib::Packet rx;
    radio.endRX(rx);
   for(byte i = 0;i<rx.len;i++){
     Serial.print(rx.data[i]);
     Serial.print("  ");
   }
   Serial.println();
  }
  
}

void RFMISR(){
  Serial.println("interrupt");
 radio.rfm_done = true; 
}


