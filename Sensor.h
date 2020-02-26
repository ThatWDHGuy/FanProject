#ifndef Temp_H
#define Temp_H

//#include <arduino.h>

#include "LimitedSizeClass.h"

#define temperateNominal 25
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define bCoefficient 3950
// the value of the 'other' resistor
//#define SERIESRESISTOR 10000



class Sensor
{
  private: //variables used privatly in program.
    static const unsigned int seriesResistor = 10000;
    static const unsigned int wait = 50;
    int thermistorPin;
    float reading;
    unsigned int tempCount;
    unsigned int volt;
  public:
    class TimeTemp {
      public:
        int rectime;
        float value;
    };
  
    Sensor(int pin);
    void getTemp();
    
    inline int getLastRecordedTime() {
      return history.last()->rectime;
    }
    inline float getLastRecordedTemp() {
      return history.last()->value;
    }
    inline TimeTemp *getLastRecordedData() {
      return history.last();
    }

    LimitedSizeList<TimeTemp> history;

};

#endif
