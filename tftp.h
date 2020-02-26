#ifndef TFTP_H
#define TFTP_H

#include <WiFiUdp.h>
#include <FS.h>

class TFTPServer {
  private:
    WiFiUDP udp;
    File file;
    unsigned long timeout;
    IPAddress currentIPAddress;
    uint16_t currentPort;
    uint16_t block;
    uint16_t lastSendSize;
    uint8_t retryCount;
    uint8_t lastData[516];
    char tempFilename[32];
    String filename;
    const unsigned long tftpTimeout = 2000;
  public:
    TFTPServer(unsigned short port=69);
    bool poll();

  private:
    void error(uint16_t errCode, const char *errString=NULL);
    void makeData();
    void makeAck(uint16_t b);
    void sendPacket(IPAddress addr, uint16_t port);
};
#endif
