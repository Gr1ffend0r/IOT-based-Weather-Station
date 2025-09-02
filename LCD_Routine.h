
//-----------------------LCD Definitions---------------------------//



//-----------------------LCD Variables---------------------------//
#include "define.h"

const int cs       = 50; // cs PE3
const int RS       = 52; // A0 PE5
const int WR_      = 51; // WR PG4
const int reset_   = 53; // reset PE4

#define LCD_Port PORTL
#define LCD_rs_Clr   PORTB &= ~_BV(PB1)    /* LCD RS (Register select) pin PC5 */
#define LCD_cs_Clr   PORTB &= ~_BV(PB3)   /* LCD R/W (Read/ Write)pin PC6    */
#define LCD_wr_Clr   PORTB &= ~_BV(PB2)   /* LCD Enable pin PC7      */
//#define LCD_led_Clr   PORTB &= ~_BV(PB7)
#define LCD_res_Clr   PORTB &= ~_BV(PB0)

#define LCD_rs_Set   PORTB |= _BV(PB1)    /* LCD RS (Register select) pin PC5 */
#define LCD_cs_Set   PORTB |= _BV(PB3)    /* LCD R/W (Read/ Write)pin PC6    */
#define LCD_wr_Set   PORTB |= _BV(PB2)    /* LCD Enable pin PC7      */
//#define LCD_led_Set   PORTB |= _BV(PB7)
#define LCD_res_Set   PORTB |= _BV(PB0)
void LCD_BlockWrite(unsigned char startx, unsigned char starty, unsigned char endx, unsigned char endy);
void WritePixel(unsigned int Xloc, unsigned int Yloc, unsigned int pColor);
void address_set(unsigned char addX1, unsigned char addX2, unsigned char addY1, unsigned char addY2);

//-----------------------LCD Variables END---------------------------//


char invertbits(unsigned char val)
{
  unsigned char tmp = 0;
  unsigned char i;
  for ( i = 0; i < 8; i++)
    if ( val & ( 1 << ( 7 - i ) ) )
      tmp |= 1 << i;
  return (tmp);
}



//-----------------------LCD Data Line Write function---------------------------//

void write_data(byte in_data)
{
  LCD_cs_Clr;
  LCD_rs_Set;
  LCD_Port =(in_data);
  LCD_wr_Clr;
  LCD_wr_Set;
  LCD_cs_Set;
}


//-----------------------LCD Write function---------------------------//

void write_com(byte in_com)
{
  LCD_cs_Clr;
  LCD_rs_Clr;
  LCD_Port =(in_com);
  LCD_wr_Clr;
  LCD_wr_Set;
  LCD_cs_Set;
}

//-----------------------LCD Initializaition function---------------------------//

void LCD_Init()
{
  /******************Software Reset****/
  //digitalWrite(reset_ , LOW);
  LCD_res_Clr;
  delayMicroseconds(100);
  //digitalWrite(reset_ , HIGH);
  LCD_res_Set;
  delayMicroseconds(100);
  //digitalWrite(reset_ , LOW);
  LCD_res_Clr;
  delayMicroseconds(100);
  //digitalWrite(reset_ , HIGH);
  LCD_res_Set;
  delayMicroseconds(100);
  write_com(0x11);

  write_com(0xb1);
  write_data(0x01);
  write_data(0x2c);
  write_data(0x2d);
  write_com(0xb2);
  write_data(0x01);
  write_data(0x2c);
  write_data(0x2d);
  write_com(0xb3);
  write_data(0x01);
  write_data(0x2c);
  write_data(0x2d);
  write_data(0x01);
  write_data(0x2d);  //2d
  write_data(0x2d);

  write_com(0xb4);
  write_data(0x02);  //02

  write_com(0xb6);
  write_data(0xb4);
  write_data(0xf0);

  write_com(0xc0);
  write_data(0xa2);
  write_data(0x02);
  write_data(0x84);

  write_com(0xc1);
  write_data(0xc5);

  write_com(0xc2);
  write_data(0x0a);
  write_data(0x00);

  write_com(0xc3);
  write_data(0x8a);
  write_data(0x2a);
  write_com(0xc4);
  write_data(0x8a);
  write_data(0xee);

  write_com(0xc5);
  write_data(0x0e);

  write_com(0x36);   //ÐÐÉ¨ÃèË³Ðò£¬ÁÐÉ¨ÃèË³Ðò£¬ºá·Å/Êú·Å
  write_data(0x68);   //160*128 //MX=0£¨ÐÐµØÖ·Ë³Ðò£º´Ó×óµ½ÓÒ£©£¬MY=1£¨ÁÐµØÖ·Ë³Ðò£º´ÓÉÏµ½ÏÂ£©£¬MV=1Êú·Å£©£¬ML=0(×ÝÏòË¢ÐÂ£º´ÓÉÏµ½ÏÂ£©£¬RGB=1(ÒÀ´ÎÎªRGB£©£¬MH=0£¨ºáÏòË¢ÐÂË³Ðò£º´Ó×óµ½ÓÒ£©
  //write_data(0xC8);     //128*160//MX=0£¨ÐÐµØÖ·Ë³Ðò£º´Ó×óµ½ÓÒ£©£¬MY=1£¨ÁÐµØÖ·Ë³Ðò£º´ÓÉÏµ½ÏÂ£©£¬MV=1Êú·Å£©£¬ML=0(×ÝÏòË¢ÐÂ£º´ÓÉÏµ½ÏÂ£©£¬RGB=1(ÒÀ´ÎÎªRGB£©£¬MH=0£¨ºáÏòË¢ÐÂË³Ðò£º´Ó×óµ½ÓÒ£©

  write_com(0xff);
  write_data(0x40);
  write_data(0x03);
  write_data(0x1a);

  write_com(0xfc);
  write_data(0x11);
  write_data(0x17);

  write_com(0xf0);
  write_data(0x01);

  write_com(0x3a);
  write_data(0x05);

  write_com(0xf6);
  write_data(0x00);

  write_com(0xe0);
  write_data(0x02);
  write_data(0x1c);
  write_data(0x07);
  write_data(0x12);
  write_data(0x37);
  write_data(0x32);
  write_data(0x29);
  write_data(0x2d);
  write_data(0x29);
  write_data(0x25);
  write_data(0x2b);
  write_data(0x39);
  write_data(0x00);
  write_data(0x01);
  write_data(0x03);
  write_data(0x10);
  write_com(0xe1);
  write_data(0x0b);
  write_data(0x14);
  write_data(0x09);
  write_data(0x26);
  write_data(0x27);
  write_data(0x22);
  write_data(0x1c);
  write_data(0x20);
  write_data(0x1d);
  write_data(0x1a);
  write_data(0x25);
  write_data(0x2d);
  write_data(0x06);
  write_data(0x06);
  write_data(0x02);
  write_data(0x0f);

  write_com(0x2a);    //¶¨ÒåXµØÖ·µÄ¿ªÊ¼¼°½áÊøÎ»ÖÃ
  write_data(0x00);
  write_data(0x00);
  write_data(0x00);
  write_data(0x9F);   //160*128
  //write_data(0x7F);   //128*160

  write_com(0x2b);     //¶¨ÒåYµØÖ·µÄ¿ªÊ¼¼°½áÊøÎ»ÖÃ
  write_data(0x00);
  write_data(0x00);
  write_data(0x00);
  write_data(0x7F);   //160*128
  //write_data(0x9F);     //128*160

  write_com(0x29);    //¿ªÏÔÊ¾
  //_delay_ms(10);
  // DrawLCD2(0,128,0,159,Yellow);
}


void DrawLCD2(unsigned char yStart, unsigned char yEnd, unsigned char xStart, unsigned char xEnd, unsigned long ulValue)
{
  unsigned int icnt, jcnt;

  LCD_BlockWrite(xStart, yStart, xEnd, yEnd);

  for (jcnt = yStart; jcnt < yEnd + 1; jcnt++)
  {
    for (icnt = xStart; icnt < xEnd + 1; icnt++)
    {
      write_data((ulValue & 0xFF00) >> 8);
      write_data(ulValue & 0x00FF);
    }
  }
}


void LCD_BlockWrite(unsigned char startx, unsigned char starty, unsigned char endx, unsigned char endy)
{
  write_com(0x2A);  //X Address Setup
  write_data(0x00);
  write_data(startx);
  write_data(0x00);
  write_data(endx);

  write_com(0x2B);  //Y Address Setup
  write_data(0x00);
  write_data(starty);
  write_data(0x00);
  write_data(endy);

  write_com(0x2C);  //Write Display Data
}


/********* Display 16*16 Charector *********/
void FontDisp16(signed char offset, unsigned char x, unsigned char y, char* textptr, unsigned int Bcolor, unsigned int Tcolor, unsigned char res)
{
  unsigned int  xx;
  unsigned char read_Data, readData1;
  char kk , tt;
  unsigned int i, mn;               // Loop counters
  unsigned char pixelData[32];
  xx = (x * 16) + offset;
  for (i = 0; textptr[i] != '\0'; i++ ) // Loop through the passed string
  {
    for (mn = 0; mn < 32; mn++)
    {
      pixelData[mn] = Font16[textptr[i] - ' '][mn];
    }
    tt = 0;
    for (kk = 0; kk < 32;)
    {
      read_Data = pixelData[kk];
      readData1 = pixelData[kk + 1];

      for (mn = 0; mn < 8; mn++)
      {
        if (read_Data & 0x80)
        {
          WritePixel((xx + mn), ((y * 16) + tt), Tcolor);
        }
        else
        {
          WritePixel((xx + mn), ((y * 16) + tt), Bcolor);
        }
        read_Data = read_Data << 1;
      }

      for (mn = 8; mn < 16; mn++)
      {
        if (readData1 & 0x80)
        {
          WritePixel((xx + mn), ((y * 16) + tt), Tcolor);
        }
        else
        {
          WritePixel((xx + mn), ((y * 16) + tt), Bcolor);
        }
        readData1 = readData1 << 1;
      }

      kk = kk + 2;
      tt = tt + 1;
    }
    xx = xx + res; //y++;
    if (x + res >= Resolution_X)  // Performs character wrapping
    {
      x = 0;                     // Set x at far left position
      y += 15 + 1;               // +Set y at next position down
    }
  }
}
/*************Font 8x8 **********************************/
void FontDisp8x8(unsigned char x, unsigned char y, char* textptr, unsigned int Bcolor, unsigned int Tcolor, unsigned char res)
{
  unsigned int  xx, i, mn;
  unsigned char read_Data;
  char kk , tt;
  unsigned char pixelData[8];

  xx = x * 8;
  for (i = 0; textptr[i] != '\0'; i++ ) // Loop through the passed string
  {
    for (mn = 0; mn < 8; mn++)
    {
      pixelData[mn] = Font8[textptr[i] - ' '][mn];
    }

    tt = 0;
    for (kk = 0; kk < 8;)
    {
      read_Data = pixelData[kk];

      for (mn = 0; mn < 8; mn++)
      {
        if (read_Data & 0x80)
        {
          WritePixel((xx + mn), ((y * 8) + kk), Tcolor);
        }
        else
        {
          WritePixel((xx + mn), ((y * 8) + kk), Bcolor);
        }
        read_Data = read_Data << 1;
      }

      kk = kk + 1;
      tt = tt + 1;
    }
    xx = xx + res; //y++;
    if (x + res >= Resolution_X)  // Performs character wrapping
    {
      x = 0;                     // Set x at far left position
      y += 7 + 1;               // +Set y at next position down
    }
  }
}


void WritePixel(unsigned int Xloc, unsigned int Yloc, unsigned int pColor)
{
  address_set(Yloc, Yloc, Xloc, (Xloc + 1));
  write_com(0x2c);
  write_data((pColor & 0xFF00) >> 8);
  write_data(pColor & 0x00FF);
}
void WritePixelImage(unsigned int Xloc, unsigned int Yloc, unsigned int pColor)
{
  address_set(Yloc, Yloc, Xloc, (Xloc + 1));
  write_com(0x2c);
  write_data((pColor & 0xFF00) >> 8);
  write_data(pColor & 0x00FF);
}

void address_set(unsigned char addX1, unsigned char addX2, unsigned char addY1, unsigned char addY2)
{
  write_com(0x2b);  //y axis
  write_data(addX1);
  write_data(addX2);
  write_com(0x2a);     //x axis
  write_data(addY1);
  write_data(addY2);
}
