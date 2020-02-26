#ifndef _IIC_without_ACK_H
#define _IIC_without_ACK_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "iicDriver.h"

//#include "Adafruit_GFX.h"

#define BLACK 0
#define WHITE 1

//#define DISPLAY0 0x78
//#define DISPLAY1 0x7a

class SSD1306 :  IICDriverMaster {
private:

public:
  static const uint8_t DISPLAY_0 = 0x3c;
  static const uint8_t DISPLAY_1 = 0x3d;
 
  SSD1306(unsigned char iicAddr = DISPLAY_0);

  void setPos(unsigned char x, unsigned char y);
  void fillScreen(unsigned char fill_Data);
  void drawText5x8(unsigned char x, unsigned char y, const char *ch);
  void drawText6x8(unsigned char x, unsigned char y, const char *ch);
  void drawText8x16(unsigned char x, unsigned char y,const char *ch);
  //void cnFont16x16(unsigned char x, unsigned char y, unsigned char N);
  //void drawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1,const char BMP[]);

  void beginData();
  inline void write(unsigned char byte) { IICDriverMaster::write(byte); }
  inline void end() { IICDriverMaster::end(); }
  
private:
  void initialise();
  void writeCommand(unsigned char IIC_Command);

};
#endif

