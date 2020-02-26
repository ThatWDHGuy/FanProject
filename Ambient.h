#ifndef Ambient_H
#define Ambient_H

#include "Sensor.h"
#include "Arduino.h"

class Ambient
{
  private:

  public: //variables and functions defined
    Ambient(String name, int tempPin);
    void getStats();
    String name;
    Sensor sensor;
};

#endif
