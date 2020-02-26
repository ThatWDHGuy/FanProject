#ifndef Shelf_H
#define Shelf_H

//#include <arduino.h>
#include "Sensor.h"
#include "Fan.h"
#include <PID_v1.h>
#include <list>
#include <map>


class Shelf
{
  private:
    uint8_t fanNo;
    double setpoint;
    double input;
    double output;
    PID* shelfPID;

  public:
    static const int fps = 3;
    Sensor sensor;
    Fan* fans[fps];
    //std::list <Fan *> fansList;
    String name;
    uint8_t last;

    Shelf(String name, int tempPin);
    void addFan(String name, int newdrivepin, int newtachpin);
    void getStats();
    
    //static std::map<String, Shelf *> shelvesByName;
    //static std::list<Shelf *> shelvesList;
};

#endif
