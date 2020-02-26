#ifndef Config_H
#define Config_H

#include <Arduino.h>

class Config {
  private:

  public: //variables defined
    String ssid;
    String password;
    String APssid;
    String APpassword;
    unsigned int historyLength;
    unsigned int maxPower;
    double pIn, iIn, dIn;
    double setPoint;
    Config();
    void SaveWifiConf();
};

#endif
