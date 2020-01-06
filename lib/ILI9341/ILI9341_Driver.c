#include "ILI9341_Driver.h"
#include "characters.h"
#include "tm_stm32f4_spi.h"
#include "ili9341_spi.h"
//static void ili9341_spi_init(void);
//static void ILI9341_startWrite(void);
static void Before_Sending_Command(void);
static void ILI9341_Send_Data(unsigned char data);
static void ILI9341_Send_Command(unsigned char command);
static void ILI9341_Send_Burst(unsigned short color, unsigned long len);
//static void ILI9341_Draw_Double_Pixel(int x, int y, unsigned int color1, unsigned int color2);
static void ILI9341_Draw_Char(int x, int y, unsigned int color, unsigned int phone, unsigned char charcode, unsigned char size);

unsigned int X_SIZE = 320;
unsigned int Y_SIZE = 240;

unsigned char hh;
static SPI_TypeDef lcd_spi;

void ILI9341_Init() {
	//ili9341_spi_init();
	//SPI_Cmd(SPI2, ENABLE);
	/* Reset The Screen */
	ILI9341_Reset();
	ili9341_sendCmd(0x01);
	Delayms(1000);

	/* Power Control A */
	ili9341_sendCmd(0xCB);
	ILI9341_Send_Data(0x39);
	ILI9341_Send_Data(0x2C);
	ILI9341_Send_Data(0x00);
	ILI9341_Send_Data(0x34);
	ILI9341_Send_Data(0x02);

	/* Power Control B */
	ili9341_sendCmd(0xCF);
	ILI9341_Send_Data(0x00);
	ILI9341_Send_Data(0xC1);
	ILI9341_Send_Data(0x30);

	/* Driver timing control A */
	ili9341_sendCmd(0xE8);
	ILI9341_Send_Data(0x85);
	ILI9341_Send_Data(0x00);
	ILI9341_Send_Data(0x78);

	/* Driver timing control B */
	ili9341_sendCmd(0xEA);
	ILI9341_Send_Data(0x00);
	ILI9341_Send_Data(0x00);

	/* Power on Sequence control */
	ili9341_sendCmd(0xED);
	ILI9341_Send_Data(0x64);
	ILI9341_Send_Data(0x03);
	ILI9341_Send_Data(0x12);
	ILI9341_Send_Data(0x81);

	/* Pump ratio control */
	ili9341_sendCmd(0xF7);
	ILI9341_Send_Data(0x20);

	/* Power Control 1 */
	ili9341_sendCmd(0xC0);
	ILI9341_Send_Data(0x23);

	/* Power Control 2 */
	ili9341_sendCmd(0xC1);
	ILI9341_Send_Data(0x10);

	/* VCOM Control 1 */
	ili9341_sendCmd(0xC5);
	ILI9341_Send_Data(0x3E);
	ILI9341_Send_Data(0x28);

	/* VCOM Control 2 */
	ili9341_sendCmd(0xC7);
	ILI9341_Send_Data(0x86);

	/* VCOM Control 2 */
	ili9341_sendCmd(0x36);
	ILI9341_Send_Data(0x48);

	/* Pixel Format Set */
	ili9341_sendCmd(0x3A);
	ILI9341_Send_Data(0x55);    //16bit

	ili9341_sendCmd(0xB1);
	ILI9341_Send_Data(0x00);
	ILI9341_Send_Data(0x18);

	/* Display Function Control */
	ili9341_sendCmd(0xB6);
	ILI9341_Send_Data(0x08);
	ILI9341_Send_Data(0x82);
	ILI9341_Send_Data(0x27);

	/* 3GAMMA FUNCTION DISABLE */
	ili9341_sendCmd(0xF2);
	ILI9341_Send_Data(0x00);

	/* GAMMA CURVE SELECTED */
	ili9341_sendCmd(0x26);  //Gamma set
	ILI9341_Send_Data(0x01); 	//Gamma Curve (G2.2)

	//Positive Gamma  Correction
	ili9341_sendCmd(0xE0);
	ILI9341_Send_Data(0x0F);
	ILI9341_Send_Data(0x31);
	ILI9341_Send_Data(0x2B);
	ILI9341_Send_Data(0x0C);
	ILI9341_Send_Data(0x0E);
	ILI9341_Send_Data(0x08);
	ILI9341_Send_Data(0x4E);
	ILI9341_Send_Data(0xF1);
	ILI9341_Send_Data(0x37);
	ILI9341_Send_Data(0x07);
	ILI9341_Send_Data(0x10);
	ILI9341_Send_Data(0x03);
	ILI9341_Send_Data(0x0E);
	ILI9341_Send_Data(0x09);
	ILI9341_Send_Data(0x00);

	//Negative Gamma  Correction
	ili9341_sendCmd(0xE1);
	ILI9341_Send_Data(0x00);
	ILI9341_Send_Data(0x0E);
	ILI9341_Send_Data(0x14);
	ILI9341_Send_Data(0x03);
	ILI9341_Send_Data(0x11);
	ILI9341_Send_Data(0x07);
	ILI9341_Send_Data(0x31);
	ILI9341_Send_Data(0xC1);
	ILI9341_Send_Data(0x48);
	ILI9341_Send_Data(0x08);
	ILI9341_Send_Data(0x0F);
	ILI9341_Send_Data(0x0C);
	ILI9341_Send_Data(0x31);
	ILI9341_Send_Data(0x36);
	ILI9341_Send_Data(0x0F);

	//EXIT SLEEP
	ili9341_sendCmd(0x11);
	Delayms(120);
	//TURN ON DISPLAY
	ili9341_sendCmd(0x29);
	//ILI9341_Send_Data(0x2C);
	//SPI_Cmd(SPI2, DISABLE);
}

/**
 * @brief SPI2 Initialization Function
 * @param None
 * @retval None
 */


void ILI9341_SPI_Send(unsigned char data)
{
	//SPI_Cmd(SPI2, ENABLE);
	//SPI_I2S_SendData(SPI2, data);
	TM_SPI_Send(SPI2, data);
	//TM_SPI_Write(&lcd_spi, data);
	//SPI_Cmd(SPI2, DISABLE);
}

void ILI9341_SPI_Send_Multiple(unsigned char data, int size)
{
	TM_SPI_WriteMulti(SPI2, &data, size);
}

void ILI9341_Reset() {
	RESET_ON;
	Delayms(50);
	RESET_OFF;
	CS_ON;
	Delayms(50);
	ILI9341_Send_Command(0x01);
	CS_OFF;
}

void ILI9341_Set_Rotation(unsigned char rotation) {
	dev_spi_start(m_spi);
	ili9341_sendCmdCont(0x36);
	u8 d = 0xE8;
	switch (rotation) {
	case 0:
		ili9341_sendData8(0x48, 1);
		X_SIZE = 240;
		Y_SIZE = 320;
		break;
	case 1:
		ILI9341_Send_Data(0x28);
		X_SIZE = 320;
		Y_SIZE = 240;
		break;
	case 2:
		ILI9341_Send_Data(0x88);
		X_SIZE = 240;
		Y_SIZE = 320;
		break;
	case 3:
		
		
		ili9341_sendDataCont8(&d, 1);
		X_SIZE = 320;
		Y_SIZE = 240;
		break;
	}
	dev_spi_stop(m_spi);
}

void ILI9341_SPI_Send_32(unsigned char command, unsigned long data) {
	CS_ON;
	DC_COMMAND;
	ILI9341_SPI_Send(command);

	DC_DATA;
	ILI9341_SPI_Send(data >> 24);
	ILI9341_SPI_Send(data >> 16);
	ILI9341_SPI_Send(data >> 8);
	ILI9341_SPI_Send(data);

}

void ILI9341_Set_Address(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
	unsigned long t;
	t = x1;
	t <<= 16;
	t |= x2;
	ILI9341_SPI_Send_32(0x2A, t); //Column Addres Set
	t = y1;
	t <<= 16;
	t |= y2;
	ILI9341_SPI_Send_32(0x2B, t); //Page Addres Set
}

void ILI9341_setAddrBlock(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t RW) {
	unsigned long t;
	t = x1;
	t <<= 16;
	t |= x2;
	ILI9341_SPI_Send_32(0x2A, t);  //Column Addres Set
	t = y1;
	t <<= 16;
	t |= y2;
	ILI9341_SPI_Send_32(0x2B, t);  //Page Addres Set

	if (RW == 0)
		// write to RAM
		ILI9341_Send_Command(0x2C);     // RAMWR
	else
	{
		// read from RAM
		ILI9341_Send_Command(0x2E);        // RAMRD
		uint8_t dummy = TM_SPI_Send(SPI2, 0);       // dummy read
	}
		
}

void ILI9341_Fill_Screen(unsigned int color)
{
	ILI9341_Set_Address(0, 0, X_SIZE-1, Y_SIZE-1);
	ILI9341_Send_Burst(color, (long)X_SIZE * (long)Y_SIZE);
}

void ILI9341_Draw_Pixel(int x, int y, unsigned int color)
{
	if((x < 0) || (y < 0) || (x >= X_SIZE) || (y >= Y_SIZE))
	{
		return;
	}
	ILI9341_Set_Address(x, y, X_SIZE-1, Y_SIZE-1);
	DC_COMMAND;
	ILI9341_SPI_Send(0x2C);
	DC_DATA;
	ILI9341_SPI_Send(color>>8);
	ILI9341_SPI_Send(color);
}

void ILI9341_Draw_Filled_Rectangle(unsigned int color,unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	ILI9341_Set_Address(x1, y1, x2, y2);
	ILI9341_Send_Burst(color, (long)(x2-x1+1) * (long)(y2-y1+1));
}

void ILI9341_Draw_Empty_Rectangle(unsigned int color,unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	ILI9341_Draw_Line(color, x1, y1, x2, y1);
	ILI9341_Draw_Line(color, x2, y1, x2, y2);
	ILI9341_Draw_Line(color, x1, y1, x1, y2);
	ILI9341_Draw_Line(color, x1, y2, x2, y2);
}

void ILI9341_Draw_Line(unsigned int color, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	int steep = abs(y2-y1) > abs(x2-x1);

	if (steep)
	{
		swap(x1,y1);
		swap(x2,y2);
	}

	if(x1>x2)
	{
		swap(x1,x2);
		swap(y1,y2);
	}

	int dx,dy;
	dx = (x2 - x1);
	dy = abs(y2 - y1);
	int err = dx / 2;
	int ystep;
	if(y1 < y2)
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}
	for (; x1 <= x2; x1++)
	{
		if (steep)
		{
			ILI9341_Draw_Pixel(y1, x1, color);
		}
		else
		{
			ILI9341_Draw_Pixel(x1, y1, color);
		}
		err -= dy;
		if (err < 0)
		{
			y1 += ystep;
			err += dx;
		}
	}
}

void ILI9341_Draw_Circle(unsigned int x0, unsigned int y0, int r, unsigned int color, unsigned char flood) {
	int f = 1 - r;
	int ddF_x = 1;
	int ddF_y = -2 * r;
	int x = 0;
	int y = r;
	if (flood == 0) {
		ILI9341_Draw_Pixel(x0, y0 + r, color);
		ILI9341_Draw_Pixel(x0, y0 - r, color);
		ILI9341_Draw_Pixel(x0 + r, y0, color);
		ILI9341_Draw_Pixel(x0 - r, y0, color);
		while (x < y) {
			if (f >= 0) {
				y--;
				ddF_y += 2;
				f += ddF_y;
			}
			x++;
			ddF_x += 2;
			f += ddF_x;
			ILI9341_Draw_Pixel(x0 + x, y0 + y, color);
			ILI9341_Draw_Pixel(x0 - x, y0 + y, color);
			ILI9341_Draw_Pixel(x0 + x, y0 - y, color);
			ILI9341_Draw_Pixel(x0 - x, y0 - y, color);
			ILI9341_Draw_Pixel(x0 + y, y0 + x, color);
			ILI9341_Draw_Pixel(x0 - y, y0 + x, color);
			ILI9341_Draw_Pixel(x0 + y, y0 - x, color);
			ILI9341_Draw_Pixel(x0 - y, y0 - x, color);
		}
	} else {
		ILI9341_Draw_Pixel(x0, y0 + r, color);
		ILI9341_Draw_Pixel(x0, y0 - r, color);
		ILI9341_Set_Address(x0 - r, y0, x0 + r, y0);
		DC_COMMAND;
		ILI9341_SPI_Send(0x2C);
		DC_DATA;
		for (uint32_t fff = 0; fff < r * 2 + 1; fff++) {
			ILI9341_SPI_Send(color >> 8);
			ILI9341_SPI_Send(color);
		}
		while (x < y) {
			if (f >= 0) {
				y--;
				ddF_y += 2;
				f += ddF_y;
			}
			x++;
			ddF_x += 2;
			f += ddF_x;
			ILI9341_Set_Address(x0 - x, y0 + y, x0 + x, y0 + y);
			DC_COMMAND;
			ILI9341_SPI_Send(0x2C);
			DC_DATA;
			for (uint32_t fff = 0; fff < x * 2 + 1; fff++) {
				ILI9341_SPI_Send(color >> 8);
				ILI9341_SPI_Send(color);
			}
			ILI9341_Set_Address(x0 - x, y0 - y, x0 + x, y0 - y);
			DC_COMMAND;
			ILI9341_SPI_Send(0x2C);
			DC_DATA;
			for (uint32_t fff = 0; fff < x * 2 + 1; fff++) {
				ILI9341_SPI_Send(color >> 8);
				ILI9341_SPI_Send(color);
			}
			ILI9341_Set_Address(x0 - y, y0 + x, x0 + y, y0 + x);
			DC_COMMAND;
			ILI9341_SPI_Send(0x2C);
			DC_DATA;
			for (uint32_t fff = 0; fff < y * 2 + 1; fff++) {
				ILI9341_SPI_Send(color >> 8);
				ILI9341_SPI_Send(color);
			}
			ILI9341_Set_Address(x0 - y, y0 - x, x0 + y, y0 - x);
			DC_COMMAND;
			ILI9341_SPI_Send(0x2C);
			DC_DATA;
			for (uint32_t fff = 0; fff < y * 2 + 1; fff++) {
				ILI9341_SPI_Send(color >> 8);
				ILI9341_SPI_Send(color);
			}
		}
	}
}

void ILI9341_Draw_String(unsigned int x, unsigned int y, unsigned int color, unsigned int phone, char *str, unsigned char size)
{
	switch (size)
	{
	case 1:
		while (*str)
		{
			if ((x+(size*8))>X_SIZE)
			{
				x = 1;
				y = y + (size*8);
			}
			ILI9341_Draw_Char(x, y, color, phone, *str, size);
			x += size*8-2;
			*str++;
		}
	break;
	case 2:
		hh=1;
		while (*str)
		{
			if ((x+(size*8))>X_SIZE)
			{
				x = 1;
				y = y + (size*8);
			}
			ILI9341_Draw_Char(x,y,color,phone,*str,size);
			x += hh*8;
			*str++;
		}
	break;
	}
}








//int LCD_getWidth(){ return X_SIZE;}
//int LCD_getHeight(){ return Y_SIZE;}










u32 mypow(u8 m, u8 n)
{
	u32 result = 1;	 
	while (n--)result *= m;    
	return result;
}

void ILI9341_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size, u16 fc, u16 bc)
{         	
	u8 t, temp;
	u8 enshow = 0;						   
	for (t = 0; t < len; t++)
	{
		temp = (num / mypow(10, len - t - 1)) % 10;
		if (enshow == 0&&t < (len - 1))
		{
			if (temp == 0)
			{
				//LCD_ShowChar(x + (size / 2)*t, y, fc, bc, ' ', size);
				continue;
			}
			else enshow = 1; 
		 	 
		}
		//LCD_ShowChar(x + (size / 2)*t, y, fc, bc, temp + '0', size); 
	}
} 

static void ILI9341_Send_Command(unsigned char command) {
	Before_Sending_Command();
	ILI9341_SPI_Send(command);
	//SPI_Cmd(SPI2, DISABLE);
}

static void ILI9341_Send_Data(unsigned char data) {
	ILI9341_startWrite();
	ILI9341_SPI_Send(data);
	//SPI_Cmd(SPI2, DISABLE);
}

static void ILI9341_Send_Burst(unsigned short color, unsigned long len) {
	unsigned short blocks;
	unsigned char i, high_bit = color >> 8, low_bit = color;
	CS_ON;
	DC_COMMAND;
	ILI9341_SPI_Send(0x2C);
	DC_DATA;
	ILI9341_SPI_Send(high_bit);
	ILI9341_SPI_Send(low_bit);
	len--;
	blocks = (unsigned short) (len / 64); //64 pixels/block
	if (high_bit == low_bit)
	{
		while (blocks--) {
			i = 16;
			do {
				ILI9341_SPI_Send(low_bit);
				ILI9341_SPI_Send(low_bit);
				ILI9341_SPI_Send(low_bit);
				ILI9341_SPI_Send(low_bit);
				ILI9341_SPI_Send(low_bit);
				ILI9341_SPI_Send(low_bit);
				ILI9341_SPI_Send(low_bit);
				ILI9341_SPI_Send(low_bit);
			} while (--i);
		}
		//Fill any remaining pixels(1 to 64)
		for (i = (unsigned char) len & 63; i--;) {
			ILI9341_SPI_Send(low_bit);
			ILI9341_SPI_Send(low_bit);
		}
	}
	else
	{
		while (blocks--) {
			i = 16;
			do {
				ILI9341_SPI_Send(high_bit);
				ILI9341_SPI_Send(low_bit);
				ILI9341_SPI_Send(high_bit);
				ILI9341_SPI_Send(low_bit);
				ILI9341_SPI_Send(high_bit);
				ILI9341_SPI_Send(low_bit);
				ILI9341_SPI_Send(high_bit);
				ILI9341_SPI_Send(low_bit);
			} while (--i);
		}
		//Fill any remaining pixels(1 to 64)
		for (i = (unsigned char) len & 63; i--;) {
			ILI9341_SPI_Send(high_bit);
			ILI9341_SPI_Send(low_bit);
		}
	}
	CS_OFF;
}

void ILI9341_Draw_Double_Pixel(int x, int y, unsigned int color1, unsigned int color2)
{
	if((x < 0) || (y < 0) || (x >= X_SIZE) || (y >= Y_SIZE))
	{
		return;
	}
	ILI9341_Set_Address(x, y, X_SIZE-1, Y_SIZE-1);
	DC_COMMAND;
	ILI9341_SPI_Send(0x2C);
	DC_DATA;
	ILI9341_SPI_Send(color1);
	ILI9341_SPI_Send(color2);
}

static void ILI9341_Draw_Char(int x, int y, unsigned int color, unsigned int phone, unsigned char charcode, unsigned char size)
{
	int i,h;
	switch(size)
	{
		case 1:
			ILI9341_Set_Address(x, y, x+5, y+8);
			DC_COMMAND;
			ILI9341_SPI_Send(0x2C);
			DC_DATA;
			for(h=0;h<8;h++)
			{
				for(i=2;i<8;i++)
				{
					if ((chars8[charcode-0x20][h]>>(7-i))&0x01)
					{
						ILI9341_SPI_Send(color>>8);
						ILI9341_SPI_Send(color);
					}
					else
					{
						ILI9341_SPI_Send(phone>>8);
						ILI9341_SPI_Send(phone);
					}
				}
			}
			break;
		case 2:
			ILI9341_Set_Address(x, y, x+7, y+16);
			DC_COMMAND;
			ILI9341_SPI_Send(0x2C);
			DC_DATA;
			for(h=0;h<16;h++)
			{
				for(i=0;i<8;i++)
				{
					if ((chars16[charcode-0x20][h]>>(7-i))&0x01)
					{
						ILI9341_SPI_Send(color>>8);
						ILI9341_SPI_Send(color);
					}
					else
					{
						ILI9341_SPI_Send(phone>>8);
						ILI9341_SPI_Send(phone);
					}
				}
			}
		break;
	}
}

void ILI9341_startWrite() {
	//SPI_Cmd(SPI2, ENABLE);
	GPIO_SetBits(ILI9341_RS_GPIO_Port, ILI9341_RS_Pin);
	GPIO_ResetBits(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin);
}

static void Before_Sending_Command() {
	//SPI_Cmd(SPI2, ENABLE);
	GPIO_ResetBits(ILI9341_RS_GPIO_Port, ILI9341_RS_Pin);
	GPIO_ResetBits(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin);
}

void ILI9341_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
	int16_t max_radius = ((w < h) ? w : h) / 2;  // 1/2 minor axis
	if(r > max_radius) r = max_radius;
	// smarter version
	ILI9341_Draw_Filled_Rectangle((unsigned int)color, (unsigned int)x + r, (unsigned int)y, (unsigned int)x + w - 2*r, (unsigned int)y + h);
	//writeFillRect(x + r, y, w - 2*r, h, color);
	// draw four corners
	ILI9341_fillCircleHelper((uint16_t)(x + w - r - 1), (uint16_t)(y + r), (uint16_t)r, 1, (uint16_t)(h - 2*r - 1), color);
	ILI9341_fillCircleHelper(x + r, y + r, r, 2, h - 2*r - 1, color);
	
}

/**************************************************************************/
/*!
    @brief  Quarter-circle drawer with fill, used for circles and roundrects
    @param  x0       Center-point x coordinate
    @param  y0       Center-point y coordinate
    @param  r        Radius of circle
    @param  corners  Mask bits indicating which quarters we're doing
    @param  delta    Offset from center-point, used for round-rects
    @param  color    16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void ILI9341_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color) {

	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;
	int16_t px    = x;
	int16_t py    = y;

	delta++;  // Avoid some +1's in the loop

	while(x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		// These checks avoid double-drawing certain lines, important
		// for the SSD1306 library which has an INVERT drawing mode.
		if(x < (y + 1)) {
			if (corners & 1) ILI9341_drawFastVLine(x0 + x, y0 - y, 2*y + delta, color);
			
			if (corners & 2) ILI9341_drawFastVLine(x0 - x, y0 - y, 2*y + delta, color);
		}
		if (y != py) {
			if (corners & 1) ILI9341_drawFastVLine(x0 + py, y0 - px, 2*px + delta, color);
			if (corners & 2) ILI9341_drawFastVLine(x0 - py, y0 - px, 2*px + delta, color);
			py = y;
		}
		px = x;
	}
}

void ILI9341_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
	ILI9341_Draw_Line(color, x, y, x, y + h - 1);
	//writeLine(x, y, x, y + h - 1, color);
}

/**************************************************************************/
/*!
   @brief    Draw a perfectly horizontal line (this is often optimized in a subclass!)
    @param    x   Left-most x coordinate
    @param    y   Left-most y coordinate
    @param    w   Width in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void ILI9341_drawFastHLine(int16_t x,
	int16_t y,
	int16_t w,
	uint16_t color) {

		ILI9341_Draw_Line(color, x, y, x + w - 1, y);

}

/**************************************************************************/
/*!
    @brief   Set text 'magnification' size. Each increase in s makes 1 pixel that much bigger.
    @param  s_x  Desired text width magnification level in X-axis. 1 is default
    @param  s_y  Desired text width magnification level in Y-axis. 1 is default
*/
/**************************************************************************/
void ILI9341_setTextSizeXY(uint8_t s_x, uint8_t s_y) {
	textsize_x = (s_x > 0) ? s_x : 1;
	textsize_y = (s_y > 0) ? s_y : 1;
}

/**********************************************************************/
  /*!
    @brief  Set text cursor location
    @param  x    X coordinate in pixels
    @param  y    Y coordinate in pixels
  */
  /**********************************************************************/
void ILI9341_setCursor(int16_t x, int16_t y) { cursor_x = x; cursor_y = y; }

/**********************************************************************/
  /*!
    @brief   Set text font color with transparant background
    @param   c   16-bit 5-6-5 Color to draw text with
    @note    For 'transparent' background, background and foreground
             are set to same color rather than using a separate flag.
  */
  /**********************************************************************/
void ILI9341_setTextColor(uint16_t c) { textcolor = textbgcolor = c; }

/**********************************************************************/
/*!
  @brief   Set text font color with custom background color
  @param   c   16-bit 5-6-5 Color to draw text with
  @param   bg  16-bit 5-6-5 Color to draw background/fill with
*/
/**********************************************************************/
void ILI9341_setTextColorBg(uint16_t c, uint16_t bg) {
	textcolor   = c;
	textbgcolor = bg;
}

/**************************************************************************/
/*!
   @brief   Draw a PROGMEM-resident 16-bit image (RGB 5/6/5) at the specified (x,y) position.
   For 16-bit display devices; no color reduction performed.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with 16-bit color bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
*/
/**************************************************************************/
void ILI9341_drawRGBBitmap(int16_t x,
	int16_t y,
	const uint16_t bitmap[],
	int16_t w,
	int16_t h) {

	for (int16_t j = 0; j < h; j++, y++) {
		for (int16_t i = 0; i < w; i++) {
			ILI9341_Draw_Pixel(x + i, y, bitmap[j * w + i]);
			//writePixel(x + i, y, pgm_read_word(&bitmap[j * w + i]));
		}
	}

}

/**************************************************************************/
/*!
   @brief   Draw a rounded rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    r   Radius of corner rounding
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void ILI9341_drawRoundRect(int16_t x,
	int16_t y,
	int16_t w,
	int16_t h,
	int16_t r,
	uint16_t color) {
	int16_t max_radius = ((w < h) ? w : h) / 2;  // 1/2 minor axis
	if(r > max_radius) r = max_radius;
	// smarter version
	ILI9341_drawFastHLine(x + r, y, w - 2*r, color);   // Top
	ILI9341_drawFastHLine(x + r, y + h - 1, w - 2*r, color);   // Bottom
	ILI9341_drawFastVLine(x, y + r, h - 2*r, color);   // Left
	ILI9341_drawFastVLine(x + w - 1, y + r, h - 2*r, color);   // Right
	// draw four corners
	ILI9341_drawCircleHelper(x + r, y + r, r, 1, color);
	ILI9341_drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
	ILI9341_drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
	ILI9341_drawCircleHelper(x + r, y + h - r - 1, r, 8, color);

}

/**************************************************************************/
/*!
    @brief    Quarter-circle drawer, used to do circles and roundrects
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    cornername  Mask bit #1 or bit #2 to indicate which quarters of the circle we're doing
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void ILI9341_drawCircleHelper( int16_t x0,
	int16_t y0,
	int16_t r,
	uint8_t cornername,
	uint16_t color) {
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x4) {
			ILI9341_Draw_Pixel(x0 + x, y0 + y, color);
			ILI9341_Draw_Pixel(x0 + y, y0 + x, color);
		}
		if (cornername & 0x2) {
			ILI9341_Draw_Pixel(x0 + x, y0 - y, color);
			ILI9341_Draw_Pixel(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8) {
			ILI9341_Draw_Pixel(x0 - y, y0 + x, color);
			ILI9341_Draw_Pixel(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1) {
			ILI9341_Draw_Pixel(x0 - y, y0 - x, color);
			ILI9341_Draw_Pixel(x0 - x, y0 - y, color);
		}
	}
}

/**************************************************************************/
/*!
   @brief    Draw a circle with filled color
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void ILI9341_fillCircle(int16_t x0,
	int16_t y0,
	int16_t r,
	uint16_t color) {

		ILI9341_drawFastVLine(x0, y0 - r, 2*r + 1, color);
		ILI9341_fillCircleHelper(x0, y0, r, 3, 0, color);

}