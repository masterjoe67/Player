#pragma once 
#include "stm32f4xx.h"
#include "defines.h"
#include <stdlib.h>
#include "main.h"
#include "tm_stm32f4_delay.h"

#define  RESET_OFF 		GPIO_SetBits(ILI9341_RST_GPIO_Port,ILI9341_RST_Pin)
#define  RESET_ON 		GPIO_ResetBits(ILI9341_RST_GPIO_Port,ILI9341_RST_Pin)
#define  CS_OFF 		GPIO_SetBits(ILI9341_CS_GPIO_Port,ILI9341_CS_Pin)
#define  CS_ON 			GPIO_ResetBits(ILI9341_CS_GPIO_Port,ILI9341_CS_Pin)
#define  DC_DATA 		GPIO_SetBits(ILI9341_RS_GPIO_Port,ILI9341_RS_Pin)
#define  DC_COMMAND 	GPIO_ResetBits(ILI9341_RS_GPIO_Port,ILI9341_RS_Pin)

#define BLACK       	0x0000
#define NAVY        	0x000F
#define DARKGREEN   	0x03E0
#define DARKCYAN    	0x03EF
#define MAROON      	0x7800
#define PURPLE      	0x780F
#define OLIVE       	0x7BE0
#define LIGHTGREY   	0xC618
#define DARKGREY    	0x7BEF
#define BLUE        	0x001F
#define GREEN       	0x07E0
#define CYAN        	0x07FF
#define RED         	0xF800
#define MAGENTA     	0xF81F
#define YELLOW      	0xFFE0
#define WHITE       	0xFFFF
#define ORANGE      	0xFD20
#define GREENYELLOW 	0xAFE5
#define PINK        	0xF81F

#define swap(a,b) 		{int16_t t=a;a=b;b=t;}
#define BURST_MAX_SIZE 	500

#define ILI9341_DC_SET DC_DATA
#define LCD_GRAM                0x2C   /* GRAM register */

//#define LCD_setAddressWindowToWrite(x1,y1,x2,y2) \
    //ILI9341_setAddrBlock(x1, y1, x2, y2, 0);

//#define LCD_setAddressWindowToRead(x1,y1,x2,y2) \
    //ILI9341_setAddrBlock(x1, y1, x2, y2, 1);





extern unsigned int X_SIZE;
extern unsigned int Y_SIZE;

int16_t cursor_x, cursor_y;
uint16_t textcolor, textbgcolor;
uint8_t textsize_x, textsize_y;

//int LCD_getWidth();
//int LCD_getHeight();

void Show_Menu(void);
void ILI9341_Init(void);
void ILI9341_Reset(void);
void ILI9341_Set_Rotation(unsigned char rotation);
void ILI9341_Fill_Screen(unsigned int color);
void ILI9341_startWrite();

void ILI9341_setCursor(int16_t x, int16_t y);
void ILI9341_setTextColor(uint16_t c);
void ILI9341_setTextColorBg(uint16_t c, uint16_t bg);

void ILI9341_setTextSizeXY(uint8_t s_x, uint8_t s_y);

void ILI9341_Draw_Pixel(int x, int y, unsigned int color);
void ILI9341_Draw_Double_Pixel(int x, int y, unsigned int color1, unsigned int color2);
void ILI9341_Draw_Circle(unsigned int x0, unsigned int y0, int r, unsigned int color, unsigned char flood);
void ILI9341_Draw_Line(unsigned int color, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void ILI9341_Draw_Filled_Rectangle(unsigned int color,unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void ILI9341_Draw_Empty_Rectangle(unsigned int color,unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void ILI9341_Draw_String(unsigned int x, unsigned int y, unsigned int color, unsigned int phone, char *str, unsigned char size);
void ILI9341_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void ILI9341_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color);
void ILI9341_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void ILI9341_drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);
void ILI9341_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void ILI9341_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void ILI9341_drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void ILI9341_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void ILI9341_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size, u16 fc, u16 bc);
void ILI9341_setAddrBlock(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t RW);
void ili9341_sendData16(u16 *data, u32 n);
void ili9341_receiveDataCont8(u8 *data);
void ili9341_fill16(u16 color, u32 n);