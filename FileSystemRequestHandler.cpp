#include "FileSystemRequestHandler.h"

#ifdef ESP_PLATFORM
#include <WebServer.h>
#include <SPIFFS.h>
#elif defined(ARDUINO_ESP8266_NODEMCU)
#include <ESP8266WebServer.h>
#include <FS.h>
#endif


bool FileSystemRequestHandler::canHandle(HTTPMethod method, String uri) { //method to check if this request can be handled
  if (uri == "/") {
    uri = "/index.html";
  }
  Serial.println(uri);
  return SPIFFS.exists(uri);
}

bool FileSystemRequestHandler::handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
  if (requestUri == "/") { //if at the homepage
    requestUri = "/index.html"; //send index file
  }
  File f = SPIFFS.open(requestUri, "r");//open requested file
  if (!f) { //if no file is found
    printf("%s open failed\n", requestUri.c_str());
    return false;
  } else {
    const char *type; //dictates what kind of file to send.
    if (requestUri.endsWith(".txt")) { //send plain text
      type = "text/plain";
    } else if (requestUri.endsWith(".html")) { //send html file
      type = "text/html";
    } else if (requestUri.endsWith(".jpg")) { //send image
      type = "image/jpeg";
    } else if (requestUri.endsWith(".js")) { //send javascript
      type = "text/javascript";
   // } else if (requestUri.endsWith(".css")) { //send css
   //   type = "text/css";
    } else {
      type = "text/plain"; //if not applicable with above send plain
    }
    server.streamFile( f, type); //send requested file and specifies how to send it
    f.close(); //close file reader
    return true;
  }
};
