#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include "iicDriver.h"
#include "ssd1306.h"
#include "oledfont.c"


SSD1306::SSD1306(unsigned char iicAddr) : IICDriverMaster(iicAddr, 400) {
  initialise();
}


void SSD1306::writeCommand(unsigned char IIC_Command) {
  startWrite();
  write(0);
  write(IIC_Command);
  end();
}

void SSD1306::beginData() {
  startWrite();
  write(0x40);  //write data
}

void SSD1306::setPos(unsigned char x, unsigned char y) {
  startWrite();
  write(0x00);  //write command
  
  write(0xb0 + y);    // 0xbY Y=page address (row)
  write((x>>4) | 0x10); // 0x1X X=hi nybble column address
  write(x & 0x0f);    // 0x0X X=lo nybble column address
  
  end();
}

void SSD1306::fillScreen(unsigned char fill_Data) {
  unsigned char m,n;
  for(m=0;m<8;m++) {
    writeCommand(0xb0+m); //page0-page1
    writeCommand(0x00);   //low column start address
    writeCommand(0x10);   //high column start address
    beginData();
    for (n=0;n<128;n++)
      write(fill_Data);
    end();
  }
}

extern const unsigned char font5x8[];

void SSD1306::drawText5x8(unsigned char x, unsigned char y, const char *ch) {
  unsigned int c;
  
    setPos(x,y);
    beginData();
  while((c=*(ch++)) != '\0') {
    //c -= 32;
    c *= 5;
    if(x>123) {
      end();
      x=0;
      y++;
        setPos(x,y);
      beginData();  
    }

      for(unsigned char i=0;i<5;i++)
      write(pgm_read_byte(&font5x8[c++]));
    x += 5;
  }
    end();
}

void SSD1306::drawText6x8(unsigned char x, unsigned char y, const char *ch) {
  unsigned int c;
  
    setPos(x,y);
    beginData();
  while((c=*(ch++)) != '\0') {
    c -= 32;
    c *= 6;
    if(x>122) {
      end();
      x=0;
      y++;
        setPos(x,y);
      beginData();  
    }

      for(unsigned char i=0;i<6;i++)
      write(pgm_read_byte(&font6x8[c++]));
    x += 6;
  }
    end();
}


void SSD1306::drawText8x16(unsigned char x, unsigned char y,const char *ch) {
  unsigned int c;
  while ((c=*(ch++)) != '\0') {
    c-=32;
    c<<=4;
    if (x>120) {
      x=0;
      y++;
      }
    setPos(x,y);
    beginData();
    int i=0;
    for (;(i<8);i++)
      write(pgm_read_byte(&font8X16[c++]));
//    iicBytes(&font8X16[c*16], 8);
    end();
    setPos(x,y+1);
    beginData();
    for (;(i<16);i++)
      write(pgm_read_byte(&font8X16[c++]));
//    iicBytes(&font8X16[c*16+8], 8);
    end();
    x+=8;
  }
}

/*
void IIC_without_ACK::cnFont16x16(unsigned char x, unsigned char y, unsigned char N) {
  unsigned char wm=0;
  unsigned int adder=32*N;
  IIC_SetPos(x , y);
  Begin_IIC_Data();
  for(wm = 0;wm < 16;wm++)
  {
    Write_IIC_Byte(CN16x16[adder]);
    adder += 1;
  }
  IIC_Stop();
  IIC_SetPos(x,y + 1);
  Begin_IIC_Data();
  for(wm = 0;wm < 16;wm++)
  {
    Write_IIC_Byte(CN16x16[adder]);
    adder += 1;
  }
  IIC_Stop();
}
*/

/*
void IIC_without_ACK::drawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1,const char BMP[]) {
  unsigned int j=0;
  unsigned char x,y;
  
  if(y1%8==0)
    y=y1/8;
  else
    y=y1/8+1;
  for(y=y0;y<y1;y++)
  {
    IIC_SetPos(x0,y);
    Begin_IIC_Data();
    for(x=x0;x<x1;x++)
    {
      Write_IIC_Byte(BMP[j++]);
    }
    IIC_Stop();
  }
}
*/

void SSD1306::initialise() {
  writeCommand(0xAE);//display off
  writeCommand(0x00);//Set Memory Addressing Mode
  writeCommand(0x10);//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
  writeCommand(0x40);//Set Page Start Address for Page Addressing Mode,0-7
  writeCommand(0x81);//Set COM Output Scan Direction
  writeCommand(0xCF);//---set low column address
  writeCommand(0xA1);//---set high column address
  writeCommand(0xC8);//--set start line address
  writeCommand(0xA6);//--set contrast control register
  writeCommand(0xA8);
  writeCommand(0x3F);//--set segment re-map 0 to 127
  writeCommand(0xD3);//--set normal display
  writeCommand(0x00);//--set multiplex ratio(1 to 64)
  writeCommand(0xD5);//
  writeCommand(0x80);//0xa4,Output follows RAM content;0xa5,Output ignores RAM content
  writeCommand(0xD9);//-set display offset
  writeCommand(0xF1);//-not offset
  writeCommand(0xDA);//--set display clock divide ratio/oscillator frequency
  writeCommand(0x12);//--set divide ratio
  writeCommand(0xDB);//--set pre-charge period
  writeCommand(0x40);//
  writeCommand(0x20);//--set com pins hardware configuration
  writeCommand(0x02);
  writeCommand(0x8D);//--set vcomh
  writeCommand(0x14);//0x20,0.77xVcc
  writeCommand(0xA4);//--set DC-DC enable
  writeCommand(0xA6);//
  writeCommand(0xAF);//--turn on oled panel 
}

