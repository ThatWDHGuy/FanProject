#ifndef Fan_H
#define Fan_H

#include <Arduino.h>
#include "LimitedSizeClass.h"


typedef struct { //Defines the structure for multiple fans and their dividers
  char fantype;
  unsigned int fandiv;
} fanspec;

//Definitions of the fans
//This is the varible used to select the fan and it's divider,
//set 1 for unipole hall effect sensor
//and 2 for bipole hall effect sensor

class Fan { //makes a Fan class which has variables for each fan
  private:  //defines variables for the fans
    unsigned int drivepin; //Pin which the PWM output is delivered to
    unsigned int tachpin; //Pin which which the Tachometer input comes from the fan to
    unsigned int value;
    unsigned int power;
    const unsigned int valueMax = 255; //top value of PWM input
    const unsigned int valueMin = 0; //bottom value of PWM input
    unsigned int startPower;
    unsigned int lastSpeed;
    unsigned long oldT; //value used for deteriming fan RPM
    unsigned long currentT; //value used for deteriming fan RPM
    /* ESP32 code
      static int channelCount;
      unsigned int channel;
      const unsigned int timer = 13;
      const unsigned int baseFreq = 1000;*/
    volatile unsigned int isrCount;
    static const unsigned int wait = 50;
    unsigned int spedCount;
      
  public:
    unsigned int lastPower;
    class Timesped {
      public:
        int rectime;
        unsigned int value;
    };
    Fan(String name, unsigned char drivepin, unsigned char tachpin); //function in the fan class whichsets the drive pin and tachometer for each fan
    void setPower(unsigned int value); //function that sets the PWM input
    void getSpeed(); //fuction that gets the RPM of the fan
    void ICACHE_RAM_ATTR isr(); //Function that is called when the fan does one turn
    LimitedSizeList<Timesped> history;
    String fanName;

};

#endif
