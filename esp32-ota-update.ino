#include "src/OTA/Ota.h"

#define PIN_LED 2

#define TIME_BLINK_LED 1000


#define SSID "DIMELTHOZ OI"
#define PASSWORD "cesna194"

#define URL_FMW_bin "https://raw.githubusercontent.com/nicolasZolet/recicle-bem-fmw-update/main/fmw.bin"
#define URL_FMW_version "https://raw.githubusercontent.com/nicolasZolet/recicle-bem-fmw-update/main/version.txt"

unsigned long int timerBlinkLed;

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_LED,OUTPUT);
  digitalWrite(PIN_LED,LOW);
  
  ota.setup();
  ota.connectWifi(SSID, PASSWORD);
}

void loop() {
  blinkLed();
  ota.handleWifiConnection();

  ota.verifyFirmwareAvailable(URL_FMW_bin, URL_FMW_version, 10000);

  if(isUpdateRequested()){
    ota.update(URL_FMW_bin);
  }
}

void blinkLed(){
  if(millis() - timerBlinkLed > TIME_BLINK_LED){
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    timerBlinkLed = millis();
  }
}

String command="";
boolean startUpdate=false;
boolean isUpdateRequested(){
  if(!startUpdate && Serial.available()>0){
    char character = Serial.read();
    if(character=='{'){
      command="";
    } else if(character=='}'){
      if(command == "UPDATE"){
        return true;
      }
    } else {
      command += character;
    }
  }
  return false;
}
