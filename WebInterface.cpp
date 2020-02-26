#include "FileSystemRequestHandler.h"
#include "FileDataRequestHandler.h"
#include "WebInterface.h"

WebInterface::WebInterface(Config* config): ESP8266WebServer(80), config(config) { //inital setup params

}

void WebInterface::startAP() {

  WiFi.softAP(config->APssid.c_str(), config->APpassword.c_str()); //creates a access point so you can connect directly to the chip to configure what wif it connects to. etc

  Serial.println();
  Serial.print("Server IP address: "); //print what its connected to
  Serial.println(WiFi.softAPIP());
  Serial.print("Server MAC address: "); //print what to go to if connected to it as AP
  Serial.println(WiFi.softAPmacAddress());
  SPIFFS.begin();
  addHandler(new FileSystemRequestHandler()); //adds handler to take requests for .dat files, webpages, js files. etc
  addHandler(new FileDataRequestHandler(config)); //used to do same as line above. but loads the wifi and access point names and passwords

  begin();
  Serial.println("Config page started");//self explanatory
}

void WebInterface::connectWifi() {

  WiFi.begin(config->ssid.c_str(), config->password.c_str()); //begin WiFi connection
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) { //keep waiting until wifi connects
    delay(250);
    Serial.print("."); //print . many times
  }
  Serial.println("");
  Serial.print("Connected to "); //print in console ssid that chip has connected to
  Serial.println(config->ssid);
  Serial.print("IP address: "); //print ip address to put into hotbar to connect to server
  Serial.println(WiFi.localIP());
  

  begin();//self explanatory
  Serial.println("Web server started!"); //self explanatory
}

