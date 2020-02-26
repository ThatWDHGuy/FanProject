#ifndef Pwm_H
#define Pwm_H

//#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include "iicDriver.h"

class Pwm : protected IICDriverMaster {
  public:
    Adafruit_PWMServoDriver pwm;

    Pwm(uint8_t addr = 0x40);
    void setPwm(uint8_t pin, uint16_t power);

};
#endif
