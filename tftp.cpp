#include <ESP8266WiFi.h>

#include "tftp.h"

#define TFTP_ERROR_NOT_DEFINED 0      // Not defined, see error message (if any).
#define TFTP_ERROR_FILE_NOT_FOUND 1   // File not found.
#define TFTP_ERROR_ACCESS_VIOLATION 2 // Access violation.
#define TFTP_ERROR_DISK_FULL 3        // Disk full or allocation exceeded.
#define TFTP_ERROR_ILLEGAL_OP 4       // Illegal TFTP operation.
#define TFTP_ERROR_UNKNOWN_ID 5       // Unknown transfer ID.
#define TFTP_ERROR_ALREAD_EXISTS 6    // File already exists.
#define TFTP_ERROR_NO_SUCH_USER 7     // No such user.

TFTPServer::TFTPServer(unsigned short port) : timeout(0), currentIPAddress((uint32_t)0) {
  udp.begin(port);
  lastData[0] = 0;
  *tempFilename = '\0';
}

bool TFTPServer::poll() {
  static uint8_t incomingPacket[600];
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // receive incoming UDP packets
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
    int len = udp.read(incomingPacket, sizeof(incomingPacket));
    uint16_t opCode = incomingPacket[0] << 8 | incomingPacket[1];
    //Serial.printf("UDP packet op code: %2.2X\n", opCode);
    if ((opCode == 1) || (opCode ==2)) {
      if ((uint32_t)currentIPAddress == 0) {
        currentIPAddress = udp.remoteIP();
        currentPort = udp.remotePort();
        filename = (char *)(incomingPacket+2);
        String mode((char *)incomingPacket+2+filename.length()+1);
        //Serial.printf("%s file \"%s\" mode %s\n", (opCode==1?"RRQ":"WRQ"), filename.c_str(), mode.c_str());
        if (opCode == 1) { // RRQ
          if (SPIFFS.exists(filename)) {
            file = SPIFFS.open(filename, "r");
            block = 1;
            makeData();
            sendPacket(currentIPAddress, currentPort);
          } else { // no file
            error(TFTP_ERROR_FILE_NOT_FOUND, "File not found");
            currentIPAddress = (uint32_t)0;
          }
        } else { // WRQ
          do {
            sprintf(tempFilename, "/.tftp%.6lu", millis());
          } while (SPIFFS.exists(tempFilename));
          file = SPIFFS.open(tempFilename, "w");
          block = 1;
          makeAck(0);
          sendPacket(currentIPAddress, currentPort);
        }
      }
    } else if (opCode == 3) { // Data
      uint16_t rxBlock = incomingPacket[2] << 8 | incomingPacket[3];
      //Serial.printf("data block %hu\n", rxBlock);
      makeAck(rxBlock);
      sendPacket(udp.remoteIP(), udp.remotePort());
      if ((rxBlock == block) && (udp.remoteIP() == currentIPAddress) && (udp.remotePort() == currentPort)) {
        file.write(incomingPacket+4, len-4);
        block++;
        if (len != 516) { 
          file.close();
          if (SPIFFS.exists(filename))
            SPIFFS.remove(filename);
          SPIFFS.rename(tempFilename, filename);
          currentIPAddress = (uint32_t)0;
          timeout = 0;
          //Serial.printf("RX complete file renamed from %s to %s\n", tempFilename, filename.c_str());
          *tempFilename = '\0';
        }
      } else {
        timeout = 0;
        //Serial.println("Ignoring");
      }
    } else if (opCode == 4) { // Ack
      uint16_t rxBlock = incomingPacket[2] << 8 | incomingPacket[3];
      //Serial.printf("ack block %hu\n", rxBlock);
      if ((udp.remoteIP() == currentIPAddress) && (udp.remotePort() == currentPort)) {
        if (rxBlock == block) {
          block++;
        }
        //Serial.printf("ok %d\n", lastSendSize);
        if (lastSendSize == 512) {
          makeData();
          sendPacket(currentIPAddress, currentPort);
        } else {
          file.close();
          currentIPAddress = (uint32_t)0;
          timeout = 0;
          //Serial.println("TX complete");
        }
      }
    }

    // send back a reply, to the IP address and port we got the packet from
  }
  if (timeout) {
    unsigned long now = millis();
    if (now >= timeout) {
      //Serial.printf("Timeout#%d\n", retryCount);
      if (retryCount) {
        retryCount--;
        sendPacket(currentIPAddress, currentPort);
      } else {
        //Serial.println("Giving up");
        file.close();
        currentIPAddress = (uint32_t)0;
        timeout = 0;
        if (*tempFilename) {
          SPIFFS.remove(tempFilename);
          //Serial.print("Removed file ");
          //Serial.println(tempFilename);
          *tempFilename = '\0';
        }
      }
    } 
  }
  return (packetSize != 0);
}

void TFTPServer::error(uint16_t errCode, const char *errString) {
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.write((uint8_t)0);
  udp.write((uint8_t)5);
  udp.write((uint8_t)(errCode>>8));
  udp.write((uint8_t)(errCode & 0xff));
  if (errString == NULL)
    errString = "Error";
  udp.write(errString, strlen(errString)+1);
  udp.endPacket();
}

void TFTPServer::makeData() {
  lastData[1] = 3;
  lastData[2] = block >> 8;
  lastData[3] = block & 0xff;
  lastSendSize = file.read(lastData+4, sizeof(lastData)-4);
  retryCount = 3;
}

void TFTPServer::makeAck(uint16_t b) {
  lastData[1] = 4;
  lastData[2] = b >> 8;
  lastData[3] = b & 0xff;
  lastSendSize = 0;
  retryCount = 2;
}

void TFTPServer::sendPacket(IPAddress addr, uint16_t port) {
  udp.beginPacket(addr, port);
  udp.write(lastData, 4 + lastSendSize);
  udp.endPacket();
  timeout = millis() + tftpTimeout;
}

