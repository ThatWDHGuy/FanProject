#include "Config.h"

#ifdef ESP_PLATFORM
#include <SPIFFS.h>
#elif defined(ARDUINO_ESP8266_NODEMCU)
#include <FS.h>
#endif

//above is so it can be easily changed between esp32 and 8266

Config::Config() : historyLength(60), maxPower(255), pIn(2.0), iIn(10.0), dIn(1.0), setPoint(2.0) { //initalisation of config
  SPIFFS.begin(); //open file system read library
  File f = SPIFFS.open("/WiFiConf.txt", "r"); //read file from system
  if (!f) { //if cant open file
    Serial.println("file open failed on read.");
  } else { //if can open file
    while (f.available()) {
      String line =  f.readStringUntil(':'); //syntax used to seperate variables and names
      String value = f.readStringUntil(',');      
      if (line == "APssid") { //name for access point it will host
        APssid = value;
        Serial.print("APssid: "); //print variable to console
        Serial.println(APssid);
      } else if (line == "APpassword") { //above but different variable
        APpassword = value;
        Serial.print("APpassword: ");
        Serial.println(APpassword);
      } else if (line == "WFssid") { //etc
        ssid = value;
        Serial.print("ssid: ");
        Serial.println(ssid);
      } else if (line == "WFpassword") { //etc
        password = value;
        Serial.print("password: ");
        Serial.println(password);
      } else if (line == "history") {
        historyLength = value.toInt();
      } else if (line == "maxPower") {
        maxPower = value.toInt();
      } else if (line == "pIn") {
        pIn = value.toDouble();
      } else if (line == "iIn") {
        iIn = value.toDouble();
      } else if (line == "dIn") {
        dIn = value.toDouble();
      } else if (line == "setPoint") {
	      setPoint = value.toDouble();
      } else {
        Serial.printf("Configuration item \"%s\" is not recognised\n", line.c_str());
      }
    }
  }
  Serial.printf("Config historyLength = %d\n", historyLength);
}

void Config::SaveWifiConf() { //save wifi config when its updated
  File f = SPIFFS.open("/WiFiConf.txt", "w"); //write to file
  if (!f) {
    Serial.println("File open failed on update."); //read top
  } else {
    String WifiInfo = "APssid:" + APssid + ",APpassword:" + APpassword +
                      ",WFssid:" + ssid + ",WFpassword:" + password + ","; //string that will go into the file to be read next time
    f.write((const uint8_t*)WifiInfo.c_str() , WifiInfo.length());
  }
  
}
