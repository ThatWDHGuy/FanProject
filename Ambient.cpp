#include "Ambient.h"

Ambient::Ambient(String ambientname, int tempPin) : sensor(tempPin) {
  name = ambientname;
}

void Ambient::getStats(){
  sensor.getTemp(); //get temperature from sensor
}

