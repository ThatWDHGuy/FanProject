#include <stdio.h>
#include <ESP8266WiFi.h>
#include "Ambient.h"
#include "Sensor.h"
#include "Shelf.h"
#include "Fan.h"
#include "WebInterface.h"
//#include "TimeStuff.h"
#include "tftp.h"
#include "ssd1306.h"
#include "Pwm.h"

const int numShelfs = 2;

Ambient* ambient; //define ambient object
Shelf *shelfs[numShelfs]; //define 2 shelf objects
WebInterface* webInterface; //define webinterface onj
Config* config; //load config
TFTPServer* tftpServer;
SSD1306 *display;
Pwm PWM;

void setup(void) {
  Serial.begin(115200);  //start serial port for console
  IICDriverMaster::setPins(D1, D2);
  display = new SSD1306();
  display->fillScreen(0);
  display->drawText6x8(42, 0, "Temps     Fan");
  display->drawText6x8(0, 1, "Amb");
  display->drawText6x8(0, 2, "Top");
  display->drawText6x8(0, 5, "Bottom");
  config = new Config(); //initiate config
  webInterface = new WebInterface(config); //start webinterface with config
  webInterface->startAP(); //start acess point
  webInterface->connectWifi(); //connect to wifi network
  ambient = new Ambient("Ambient", 5); //fill ambient 'shelf' with name and sensor pin
  shelfs[0] = new Shelf("shelf1",6); //new shelf with name and sensor pin
  shelfs[0]->addFan("fan1a",0, D3); //new fan with name, PWM chip pin and tach in pin
  shelfs[0]->addFan("fan1b",1, D4);
  shelfs[0]->addFan("fan1c",2, D5);
  shelfs[1] = new Shelf("shelf2",7);
  shelfs[1]->addFan("fan2a",3, D6);
  shelfs[1]->addFan("fan2b",4, D7);
  tftpServer = new TFTPServer();

}

void loop(void) {
  static unsigned long statCheck = 0; //var used to do statistic check
  static const Sensor::TimeTemp *lastData = NULL;
  static uint16_t pwmVal = 0;
  unsigned long now = millis() / 10;  //set now to a number
  static uint8_t displayedPower[numShelfs];
  
  if (now >= statCheck) { //will make sure it only stat checks once every 10 milli seconds
    statCheck = now; //update stat check
    
    ambient->getStats(); //get stats for ambient temp sensor
    for (int i = 0; i < numShelfs; i++) { // go through both shelves
      if (shelfs[i]) { //if shelf isnt null
        shelfs[i]->getStats(); //get fan rpms and temp for shelf
      }
    }

    const Sensor::TimeTemp *currentData = ambient->sensor.getLastRecordedData();
    if (currentData != lastData) {
      lastData = currentData;

      // flash the LED
      pwmVal ^= 0xff;
      PWM.setPwm(8, pwmVal);
      
      char buf[24];
      sprintf(buf, "%.1f\174", currentData->value);
      display->drawText6x8(42, 1, buf);
      Serial.printf("Ta %.2f", currentData->value);
      for (int i = 0; i < numShelfs; i++) { // go through both shelves
        if (shelfs[i]) { //if shelf isnt null
          sprintf(buf, "%.1f\174", shelfs[i]->sensor.getLastRecordedTemp());
          display->drawText6x8(42, 2+i*3, buf);
          Serial.printf(" T%d %.1f", i+1, shelfs[i]->sensor.getLastRecordedTemp()); 
        }
      }
      for (int i = 0; i < numShelfs; i++) { // go through both shelves
        if (shelfs[i]) { //if shelf isnt null
          Fan **f = shelfs[i]->fans;
          //sprintf(buf, "%3u", shelfs[i]->last);
          //display->drawText6x8(42, i*3+3, buf);
          for (int j = 0; j < Shelf::fps; j++) {
            if (*f != NULL) {
               sprintf(buf, "%4u %3u", (*f)->history.last()->value, (*f)->lastPower);
               display->drawText6x8(78, i*3+j+2, buf);
               Serial.printf(" F%d%c %u", i+1, 'a'+j, (*f)->history.last()->value);
            }
            f++;
          }
        }
      }
      Serial.print('\n');
    }
    for (int i = 0; i < numShelfs; i++) { 
      if (shelfs[i]->last != displayedPower[i]) {
        displayedPower[i] = shelfs[i]->last;
        char buf[4];
        sprintf(buf, "%3u", shelfs[i]->last);
        display->drawText6x8(42, i*3+3, buf);
      }
    }
  }
  webInterface->handleClient(); //check for webserver request
  tftpServer->poll();
}
