#include "Sensor.h"
#include "Pwm.h"
#include "Config.h"

// **$** There is a Pwm object already defined in Fan.cpp. This might be a problem.
extern Pwm PWM;
extern Config *config;

Sensor::Sensor(int pin) : history(config->historyLength) {
  thermistorPin = pin; //set pin when defined to pin to read from
  PWM.setPwm(thermistorPin, 0); //ensures PWM chip to pwm 0
  tempCount = 0; //reset all values
  reading = 0.0;
  volt = (3.3 - 0.65); //voltage source - voltage used by diode
  TimeTemp newtimetemp; //list for values of temp and 
  newtimetemp.rectime = millis() / 1000; //sets inital value of 0 so there are no null PID errors
  newtimetemp.value = 0; //sets inital value of 0 so there are no null PID errors
  history.push(newtimetemp); //add data to array
  for (LimitedSizeList<TimeTemp>::iterator iter = history.begin(); iter != history.end(); iter++) {
    /*Serial.print("time: ");
      Serial.print((*iter).rectime);
      Serial.print(", value: ");
      Serial.println((*iter).value);*/

    printf("(T: %d, V: %.2f), ", (*iter).rectime, (*iter).value); // print array
  }
  printf("element count : %d\n", history.size());
};

void Sensor::getTemp() {
  PWM.setPwm(thermistorPin, 255); // turn on pin to read (on pwm chip)
  delay(50); //wait so the chip has time to turn on
  int aVal = analogRead(A0); // read the analog pin
  PWM.setPwm(thermistorPin, 0); //turn off pwm for chip
  reading += aVal; //add what was just read to the reading to later be averaged
  tempCount++; // add one to tempcount so it knows when to average
  if (tempCount == wait) { // when its time to average
    tempCount = 0; // reset counter

    reading = reading / wait; // find average reading per recording

    //printf("aVal = %d : reading-av = %.2f\n", aVal, reading);

    // convert the value to resistance
    //reading = (volt * reading) / 1024;
    //reading = seriesResistor * ((volt / reading) - 1);
    /*Serial.print("Thermistor resistance ");
      Serial.println(reading);*/

    float steinhart;
    // 822 = 1024*(1-.6/3.3)
    steinhart =  ( ((837 / reading ) - 1));     // R/Ro = (ADCmax/ADC - 1)
    steinhart = log(steinhart);                  // ln(R/Ro)
    steinhart /= bCoefficient;                   // 1/B * ln(R/Ro)
    steinhart += 1.0 / (temperateNominal + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;                 // Invert
    steinhart -= 273.15;
    reading = 0;
    /*Serial.print("Temp ");
      Serial.print(steinhart);
      Serial.println(" Degrees C");*/
    TimeTemp newtimetemp; // add reading and time to array of data
    newtimetemp.rectime = millis() / 1000;
    newtimetemp.value = steinhart;
    history.push(newtimetemp);
    //for (LimitedSizeList<Timetemp>::iterator iter = history.begin(); iter != history.end(); iter++) {
      /*Serial.print("time: ");
        Serial.print((*iter).rectime);
        Serial.print(", value: ");
        Serial.println((*iter).value);*/

    //  printf("(%d, T:%.2f), ", (*iter).rectime, (*iter).value);
    //}
    //printf("element count : %d\n", history.size());
  }
}
