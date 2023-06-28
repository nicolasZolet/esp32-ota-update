/*
    Autor: Nicolas Zolet
    Data: 27/06/2023
    Descrição: Atualização do ESP32 atraves de um link no github.
    Fonte: https://github.com/programmer131/ESP8266_ESP32_SelfUpdate
*/

#ifndef OTA_H
#define OTA_H

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#define FIRMWARE_VERSION "2.2"

class Ota {
    public:
        void setup();
        void handleWifiConnection();
        void verifyFirmwareAvailable(String urlFirmware, String urlVersion, int timeToVerify);
        void update(String url);
        void connectWifi(char* _ssid, char* _password);

    private:
        int isFirmwareAvailable(String url);

    private:
        char* ssid;
        char* password;

        bool isWifiConnected = false;
        bool wifiBegin = false;
        int TIME_VERIFY_VERSION;
        unsigned long timerVerifyFirmwareAvailable, timerShowStatus;
};

extern Ota ota;

#endif