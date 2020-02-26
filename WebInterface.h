#ifndef WebInterface_H
#define WebInterface_H
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#ifdef ESP_PLATFORM
#include <WebServer.h>
#include <SPIFFS.h>
#elif defined(ARDUINO_ESP8266_NODEMCU)
#include <ESP8266WebServer.h>
#include <FS.h>
#endif
#include "Config.h"


class WebInterface : public ESP8266WebServer {
  private:
    Config* config;

  public:
    WebInterface(Config* config);
    void getWifiInfo();
    void startAP();
    void connectWifi();
};

#endif
