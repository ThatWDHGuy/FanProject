/*
 * CFile1.c
 *
 * Created: 07/07/2015 23:00:19
 *  Author: steve
 */ 
#include "Arduino.h"
#include "Wire.h"
#include "iicDriver.h"

IICDriverMaster *IICDriverMaster::owner;


bool IICDriverMaster::notInitialised = true;
uint8_t IICDriverMaster::sda = D1;
uint8_t IICDriverMaster::scl = D2;

IICDriverMaster::IICDriverMaster(uint8_t addr, unsigned int kilohertz) : speed(kilohertz*1000), address(addr){
  if (notInitialised) {
    Wire.begin(sda, scl);
    notInitialised = false;
  }
  Wire.setClock(speed);
}

/*
IICDriverMaster::IICDriverMaster() {
  static char iicNotInitialised = 1;
  
  if (iicNotInitialised) {
    iicNotInitialised = 0;
    setClock(100);
    //TWBR = 18; // 3 is 400kHz, 18 is 100kHz
    //TWSR = 1; // prescaler = 1
  
    //txHead = txTail = IIC_TX_BUFFER_MASK;
  
    sei();
  }
}*/

void IICDriverMaster::setClock(unsigned int kilohertz) {
  speed = kilohertz*1000;
  Wire.setClock(speed);
}

void IICDriverMaster::start(uint8_t rw) {
  Wire.beginTransmission(address);
}

void IICDriverMaster::end() {
  Wire.endTransmission();
}

void IICDriverMaster::write(uint8_t byte) {
  Wire.write(byte);
}

void IICDriverMaster::write(const void *bytes, size_t count) {
  Wire.write((const uint8_t *)bytes, count);
}

void IICDriverMaster::read(void *bytes, size_t count) {
  Wire.requestFrom(address, count);
  uint8_t *b = (uint8_t *)bytes;
  while (Wire.available()) {   
    *(b++) = Wire.read();
  }
}

void IICDriverMaster::setRegs(unsigned char reg, void *val, size_t count) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write((uint8_t*)val, count);
  Wire.endTransmission();
}

void IICDriverMaster::asyncGetRegs(uint8_t regAddress, void *buf, size_t count) {
  Wire.beginTransmission(address);
  Wire.write(regAddress);
  Wire.endTransmission();
  read(buf, count);
}

int IICDriverMaster::getRegs(uint8_t regAddress, void *buf, size_t count) {
  Wire.beginTransmission(address);
  Wire.write(regAddress);
  Wire.endTransmission();
  read((uint8_t *)buf, count);
  return 0;
}
