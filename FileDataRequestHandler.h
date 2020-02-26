#ifndef FileDataRequestHandler_h
#define FileDataRequestHandler_h

#ifdef ESP_PLATFORM
#include <WebServer.h>
#include <SPIFFS.h>
#elif defined(ARDUINO_ESP8266_NODEMCU)
#include <ESP8266WebServer.h>
#endif

#include "Config.h"

class FileDataRequestHandler : public RequestHandler {
  private:
    Config* config;
    static const char *jsonMimeType;
    static const char *textMimeType;
  public:
    FileDataRequestHandler(Config* config);
    virtual bool canHandle(HTTPMethod method, String uri);
    virtual bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri);
    // ESP32 --> virtual bool handle(WebServer& server, HTTPMethod requestMethod, String requestUri);
};

#endif
