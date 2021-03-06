#include "Shelf.h"
#include "Ambient.h"
#include "Config.h"
extern Ambient *ambient;

extern Config *config;

//std::list<Shelf *> Shelf::shelvesList;
//std::map<String, Shelf *> Shelf::shelvesByName;

Shelf::Shelf(String fanname, int tempPin): sensor(tempPin) {
  name = fanname;
  shelfPID = new PID(&input, &output, &setpoint, config->pIn, config->iIn, config->dIn, REVERSE); //make pid controller and give it an input output and setpoing 2,5,1
  shelfPID->SetMode(AUTOMATIC); //config for controller
  shelfPID->SetOutputLimits(0, config->maxPower); // only output limits between 0 - 255
  //shelvesByName[name] = this; // misc
  //shelvesList.push_back(this); // misc
  fanNo = 0;
  for (int i = 0; i < fps; i++) { //set all fans to null before they get defined incase some arent defined
    fans[i] = NULL;
  }
  last = 0;
}


void Shelf::addFan(String name , int newdrivepin, int newtachpin) { // process of adding a fan to shelf. it'll first do fan 0, then 1, then 2.
  fans[fanNo] = new Fan(name, newdrivepin, newtachpin);
  //fansList.push_back(fans[fanNo]);
  fans[fanNo]->setPower(0);
  fanNo++;

}


void Shelf::getStats() { //function thats called to get the stats for fan speed, temperature and re-evaluate PID value. 
  sensor.getTemp(); //get temp function for sensor
  Sensor::TimeTemp *z = sensor.history.last(); // get last value of the sensor from the array of data
  if (z == NULL) { // incase there is no value to read
    input = 0;
  } else {
    input = z->value;
  }
  setpoint = (double)(ambient->sensor.history.last()->value) + config->setPoint; //make PID setpoint the temperature of the ambient of + 2 degrees
  //Serial.println(input);
  //Serial.println(setpoint);
  //setpoint = 29;
  shelfPID->Compute(); // compute PID
  if ((int)output != last) { // print new power value
    last = output;
    Serial.println(last);
  }
  
  for (int i = 0; i < fps; i++) {
    if (fans[i] != NULL) {
      fans[i]->getSpeed();
      
      fans[i]->setPower(output);

    }
  }
}
