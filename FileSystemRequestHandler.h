#ifndef FileSystemRequestHandler_h
#define FileSystemRequestHandler_h

#ifdef ESP_PLATFORM
#include <WebServer.h>
#include <SPIFFS.h>
#elif defined(ARDUINO_ESP8266_NODEMCU)
#include <ESP8266WebServer.h>
#endif

class FileSystemRequestHandler : public RequestHandler {
  public:
    virtual bool canHandle(HTTPMethod method, String uri);
    virtual bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri);
};

#endif
