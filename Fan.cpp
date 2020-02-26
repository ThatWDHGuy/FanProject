//#include <FunctionalInterrupt.h>

#include "Fan.h"

#include "Shelf.h"
#include "Pwm.h"
#include "Config.h"

extern Pwm PWM;
extern Config *config;

typedef void (*voidFuncPtr)(void);
typedef struct {
  Fan *objPtr;
} FunctionInfo;
typedef struct {
  void *interruptInfo;
  FunctionInfo* functionInfo;
} ArgStructure;
extern "C" void ICACHE_RAM_ATTR __attachInterruptArg(uint8_t pin, voidFuncPtr userFunc, void*fp , int mode);
//extern "C" {
  void ICACHE_RAM_ATTR fanInterrupt(ArgStructure* arg) {
    arg->functionInfo->objPtr->isr();
  }
//}

Fan::Fan(String name, unsigned char drivepin, unsigned char tachpin): history(config->historyLength), fanName(name)  { //function in the fan class whichsets the drive pin and tachometer for each fan
  spedCount = 0;
  lastSpeed = 0;
  isrCount = 0;
  startPower = 50;
  Timesped newtimesped;
  newtimesped.rectime = millis() / 1000; //current time when speed recorded
  newtimesped.value = 0;
  history.push(newtimesped); //add new entry into array of sppeds and times
  this->drivepin = drivepin; //sets the drive pin fed into the function to a value thats used later
  this->tachpin = tachpin;  //sets the Tachometer pin fed into the function to a value thats used later
  pinMode(tachpin, INPUT_PULLUP); //tells the computer chip that the Tachometer pin is where it will recieve information
  currentT = 0; //sets the time of the CurrentT (used in getting the rpm) to 0 to prevent using null values later
  //attachInterrupt(tachpin, std::bind(&Fan::isr, this), FALLING); // attatches an interrupt to the tachometer pin to perform an action (std::bind(&Fan::isr, this)) when it recives a peak of a wavelength (every 1 spin)



  FunctionInfo* fi = new FunctionInfo;
  fi->objPtr = this;
  ArgStructure *as = new ArgStructure;
  as->interruptInfo = nullptr;
  as->functionInfo = fi;
  __attachInterruptArg(tachpin, (voidFuncPtr)fanInterrupt, as, FALLING);

  sei(); // enable the signal interrupt
  Serial.printf("max history = %u\n", history.maxSize());
}
   
void Fan::setPower(unsigned int value) { //class to give the fan a specified power
  if (value != 0) {
    if (lastSpeed < 50) {
      if (value < startPower)
        value = startPower;
    }
  }
  PWM.setPwm(drivepin, value);
  lastPower = value;
}

void Fan::getSpeed() { //function called to calculate the speed by getting time to do 1 revolution
  spedCount++;
  if (spedCount % (wait/10) == 0) {
    oldT = currentT;
    currentT = micros();
    lastSpeed = (isrCount * 30000000) / (currentT - oldT);
    //printf("%d\t%ld\t%ld\t%ld\t%d\n", isrCount, currentT, oldT, currentT - oldT, lastSpeed);
    isrCount = 0;
  }
  if (spedCount == wait) {
    spedCount = 0;
    Timesped newtimesped;
    newtimesped.rectime = millis() / 1000; //current time when speed recorded
    newtimesped.value = lastSpeed;
    history.push(newtimesped); //add new entry into array of sppeds and times
    //for (LimitedSizeList<Timesped>::iterator iter = history.begin(); iter != history.end(); iter++) {
      /*Serial.print("time: ");
        Serial.print((*iter).rectime);
        Serial.print(", value: ");
        Serial.println((*iter).value);*/
      //printf("(%d, V:%d), ", (*iter).rectime, (*iter).value);
    //}
    //printf("element count : %d\n", history.size());
  }
}

void ICACHE_RAM_ATTR Fan::isr() { //function called whenever the interrupt detects a signal
  if (digitalRead(tachpin) == LOW) {
    isrCount++;
  }
}
