#include "Ota.h"

#include "cert.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>

void Ota::setup() {
  Serial.println("Active firmware version: " + FIRMWARE_VERSION);
}

void Ota::connectWifi(char* _ssid, char* _password){
  ssid = _ssid;
  password = _password;
  handleWifiConnection();
}

void Ota::verifyFirmwareAvailable(String urlFirmware, String urlVersion, int timeToVerify){
  TIME_VERIFY_VERSION = timeToVerify;
  if (millis() - timerVerifyFirmwareAvailable > TIME_VERIFY_VERSION) {
    if (isFirmwareAvailable(urlVersion)) update(urlFirmware);
    timerVerifyFirmwareAvailable = millis();
  }
}

void Ota::handleWifiConnection(){
  if(WiFi.status()!=WL_CONNECTED && !wifiBegin){
    WiFi.begin(ssid, password);
    wifiBegin = true;
    Serial.println("Waiting connection...");
  } else if(WiFi.status()==WL_CONNECTED && !isWifiConnected){ //connected to wifi
    isWifiConnected = true;
    wifiBegin = true;
    Serial.print("Connected - IP address: ");
    Serial.println(WiFi.localIP());
  } else if(WiFi.status()!=WL_CONNECTED && isWifiConnected){ //lostConnection
    Serial.println("Lost connection!");
    wifiBegin = false;
    isWifiConnected = false;
  }
}

void Ota::update(String url) {
  if(WiFi.status()==WL_CONNECTED){
    Serial.println("Update started!");
    WiFiClientSecure client;
    client.setCACert(rootCACertificate);
    t_httpUpdate_return ret = httpUpdate.update(client, url);

    if(ret==HTTP_UPDATE_FAILED) Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
    else if(ret==HTTP_UPDATE_NO_UPDATES) Serial.println("HTTP_UPDATE_NO_UPDATES");
    else if(ret==HTTP_UPDATE_OK) Serial.println("HTTP_UPDATE_OK");
  }
}

int Ota::isFirmwareAvailable(String url) {
  if(WiFi.status()!=WL_CONNECTED) return 0;
  String payload;
  int httpCode;
  Serial.println(url);
  WiFiClientSecure * client = new WiFiClientSecure;

  if (client) {
    client -> setCACert(rootCACertificate);

    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
    HTTPClient https;

    if (https.begin( * client, url)) { // HTTPS      
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK) { //version received
        payload = https.getString(); // save received version
      } else {
        Serial.print("error in downloading version file:");
        Serial.println(httpCode);
      }
      https.end();
    }
    delete client;
  }
      
  if (httpCode == HTTP_CODE_OK) { //version received
    payload.trim();
    if (payload.equals(FirmwareVersion)) {
      Serial.printf("\nDevice already on latest firmware version:%s\n", FirmwareVersion);
      return 0;
    } 
    else {
      Serial.println(payload);
      Serial.println("New firmware detected");
      return 1;
    }
  } 
  return 0;  
}

Ota ota;