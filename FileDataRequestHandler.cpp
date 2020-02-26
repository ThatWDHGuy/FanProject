#include <ArduinoJson.h>
#include "FileDataRequestHandler.h"
#include "Shelf.h"
#include "Fan.h"
#include "Sensor.h"
#include "Ambient.h"

extern Shelf *shelfs[];
extern Ambient *ambient;
const int numShelfs = 2;

#ifdef ESP_PLATFORM
#include <SPIFFS.h>
#elif defined(ARDUINO_ESP8266_NODEMCU)
#include <FS.h>
#endif

const char *FileDataRequestHandler::jsonMimeType = "application/json";
const char *FileDataRequestHandler::textMimeType = "text/plain";

FileDataRequestHandler::FileDataRequestHandler(Config* config): config(config) {

}

bool FileDataRequestHandler::canHandle(HTTPMethod method, String uri) {
  Serial.println(uri);
  return (uri == "/getData") || uri.endsWith(".dat");
}

bool FileDataRequestHandler::handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
  if (requestUri == "/wifiConfig.dat") { //if the website asks for this file it'll send the data loaded from config.
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& wifiConfig = jsonBuffer.createObject();
    wifiConfig["ssid"] = config->ssid;
    wifiConfig["password"] = config->password;
    wifiConfig["APssid"] = config->APssid;
    wifiConfig["APpassword"] = config->APpassword;
    char json[128];
    wifiConfig.printTo((char*)json, wifiConfig.measureLength() + 1);
    server.send(200, jsonMimeType, json);
  } else if (requestUri == "/getData") { //when the webserver wants the latest data from the array of times and values in JSON format
    boolean firstShelf = true; //variable to not put a comma in so it can correctly
    char buf[64]; //buffer of text to send
    sprintf(buf, "{"); //write string to buf and send
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send_P(200, jsonMimeType, buf); //send buf
    
    sprintf(buf, "\"%s\":{", "ambient");
    server.sendContent_P(buf); //same as above
    
    sprintf(buf, "\"sensor\":[");
    server.sendContent_P(buf);
    boolean first = true; //following code is to wrire all sensor data into a long string
    for (LimitedSizeList<Sensor::TimeTemp>::iterator iter = ambient->sensor.history.begin(); iter != ambient->sensor.history.end(); iter++) { //goes through list of all values in array of data
      
      if (first == true) { //doesnt print a ',' infront of the values
        
        sprintf(buf, "[%d,%.2f]" , (*iter).rectime - (*ambient->sensor.history.last()).rectime, (*iter).value);
        server.sendContent_P(buf);
        first = false;
      } else {
        sprintf(buf, ",[%d,%.2f]" , (*iter).rectime - (*ambient->sensor.history.last()).rectime, (*iter).value);
        server.sendContent_P(buf);
      }
    }
   
    sprintf(buf, "]}"); //end of ambient write stuff
    server.sendContent_P(buf);
    for (int o = 0; o < numShelfs; o++) { //its pretty much the same as all of the ambient stuff. it just sends temp and fans instead of only temp
      if (shelfs[o] != NULL) {
        boolean firstFan = true;
        /*if (firstShelf) {
          firstShelf = false;
          } else {*/
        sprintf(buf, ",");
        server.sendContent_P(buf);
        //}
        sprintf(buf, "\"%s\":{", shelfs[o]->name.c_str());
        server.sendContent_P(buf);
        sprintf(buf, "\"sensor\":[");
        server.sendContent_P(buf);
        boolean first = true;
        for (LimitedSizeList<Sensor::TimeTemp>::iterator iter = shelfs[o]->sensor.history.begin(); iter != shelfs[o]->sensor.history.end(); iter++) {
          if (first == true) {
            sprintf(buf, "[%d,%.2f]" , (*iter).rectime - (*shelfs[o]->sensor.history.last()).rectime, (*iter).value);
            server.sendContent_P(buf);
            first = false;
          } else {
            sprintf(buf, ",[%d,%.2f]" , (*iter).rectime - (*shelfs[o]->sensor.history.last()).rectime, (*iter).value);
            server.sendContent_P(buf);
          }
        }
        sprintf(buf, "],");
        server.sendContent_P(buf);
        for (int i = 0; i < Shelf::fps; i++) {
          if (shelfs[o]->fans[i] != NULL) {
            if (firstFan) {
              firstFan = false;
            } else {
              sprintf(buf, ",");
              server.sendContent_P(buf);
            }
            String fanName = shelfs[o]->fans[i]->fanName;
            sprintf(buf, "\"%s\":[", shelfs[o]->fans[i]->fanName.c_str());
            server.sendContent_P(buf);
            first = true;
            for (LimitedSizeList<Fan::Timesped>::iterator iter = shelfs[o]->fans[i]->history.begin(); iter != shelfs[o]->fans[i]->history.end(); iter++) {
              if (first == true) {
                sprintf(buf, "[%d,%d]" , (*iter).rectime - (*shelfs[o]->fans[i]->history.last()).rectime, (*iter).value);
                server.sendContent_P(buf);
                first = false;
              } else {
                sprintf(buf, ",[%d,%d]" , (*iter).rectime - (*shelfs[o]->fans[i]->history.last()).rectime, (*iter).value);
                server.sendContent_P(buf);
              }
            }
            sprintf(buf, "]");
            server.sendContent_P(buf);
          }
        }
      }
      sprintf(buf, "}");
      server.sendContent_P(buf);
    }
    sprintf(buf, "}");
    server.sendContent_P(buf);
  } else {
    server.send(404, "text/plain", "NULL");
  }
  return true;
};
