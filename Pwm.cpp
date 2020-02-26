#include "Pwm.h"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Pwm::Pwm(uint8_t addr) : IICDriverMaster(addr, 400), pwm(&Wire, addr) { 
  //Wire.begin(D1, D2); //defines I2C pins used for pwm chip
  pwm.begin(); //starts pwm chip
  pwm.setPWMFreq(1000); //sets pwm freqency to frequency of ESP 8266
  //Wire.setClock(400000); // set clock
  for (int i = 0; i <= 15; i++) { //set all pins to off
    setPwm(i, 0);
  }
}

void Pwm::setPwm(uint8_t pin, uint16_t power) { //sets pin with power
  uint16_t startt;
  uint16_t endt;

  if (power == 0) { //special case for off
    startt = 0;
    endt = 4096;
  } else if (power >= 255) { //special case for on
    startt = 4096;
    endt = 0;
  } else { //case for all other power values
    startt = 800 * pin;
    endt = startt + (16 * power);
    if (endt >= 4096) {
      endt = endt - 4096;
    }
  }
  //printf("Pin: %d, Start:%d, End:%d \n", pin, startt, endt);
  pwm.setPWM(pin, startt, endt); //tells pwm chip to write power to pin
}
