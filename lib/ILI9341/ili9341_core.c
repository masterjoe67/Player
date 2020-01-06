#include "ili9341_core.h"
#include "tm_stm32f4_delay.h"


static u16 screen_width = LCD_PIXEL_WIDTH, screen_height = LCD_PIXEL_HEIGHT;

static const uint8_t init_commands[] =
{
		// Power control A
		6, LCD_POWERA, 0x39, 0x2C, 0x00, 0x34, 0x02,
		// Power control B
		4, LCD_POWERB, 0x00, 0xC1, 0x30,
		// Driver timing control A
		4, LCD_DTCA, 0x85, 0x00, 0x78,
		// Driver timing control B
		3, LCD_DTCB, 0x00, 0x00,
		// Power on sequence control
		5, LCD_POWER_SEQ, 0x64, 0x03, 0x12, 0x81,
		// Pump ratio control
		2, LCD_PRC, 0x20,
		// Power control 1
		2, LCD_POWER1, 0x23,
		// Power control 2
		2, LCD_POWER2, 0x10,
		// VCOM control 1
		3, LCD_VCOM1, 0x3E, 0x28,
		// VCOM cotnrol 2
		2, LCD_VCOM2, 0x86,
		// Memory access control
		2, LCD_MAC, 0x48,
		// Pixel format set
		2, LCD_PIXEL_FORMAT, 0x55,
		// Frame rate control
		3, LCD_FRMCTR1, 0x00, 0x18,
		// Display function control
		4, LCD_DFC, 0x08, 0x82, 0x27,
		// 3Gamma function disable
		2, LCD_3GAMMA_EN, 0x00,
		// Gamma curve selected
		2, LCD_GAMMA, 0x01,
		// Set positive gamma
		16, LCD_PGAMMA, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37,0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, 
		16, LCD_NGAMMA, 0x00, 0x0E, 0x14,0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
		0 };

void LCD_configure() {
	u8 *address = (u8 *) init_commands;
	u8 count;
	
	/* Reset The Screen */
	LCD_reset();
	ili9341_sendCmd(0x01);
	Delayms(1000);
	dev_spi_start(m_spi);
	count = *(address++);
	/* Power Control A */
	ili9341_sendCmdCont(*(address++));			//0xCB
	ili9341_sendDataCont8((address++), 1);  //0x39
	ili9341_sendDataCont8((address++), 1); 	//0x2c
	ili9341_sendDataCont8((address++), 1); 	//0x00
	ili9341_sendDataCont8((address++), 1); 	//0x34
	ili9341_sendDataCont8((address++), 1); 	//0x02

	count = *(address++);
	/* Power Control B */
	ili9341_sendCmdCont(*(address++)); 			//0xCF
	ili9341_sendDataCont8((address++), 1); 	//0x00
	ili9341_sendDataCont8((address++), 1); 	//0xC1
	ili9341_sendDataCont8((address++), 1); 	//0x30

	count = *(address++);
	/* Driver timing control A */
	ili9341_sendCmdCont(*(address++)); 			//0xE8
	ili9341_sendDataCont8((address++), 1); 	//0x85
	ili9341_sendDataCont8((address++), 1); 	//0x00
	ili9341_sendDataCont8((address++), 1); 	//0x78

	count = *(address++);
	/* Driver timing control B */
	ili9341_sendCmdCont(*(address++)); 			//0xEA
	ili9341_sendDataCont8((address++), 1); 	//0x00
	ili9341_sendDataCont8((address++), 1); 	//0x00

	count = *(address++);
	/* Power on Sequence control */
	ili9341_sendCmdCont(*(address++)); 			//0xED
	ili9341_sendDataCont8((address++), 1); 	//0x64
	ili9341_sendDataCont8((address++), 1); 	//0x03
	ili9341_sendDataCont8((address++), 1); 	//0x12
	ili9341_sendDataCont8((address++), 1); 	//0x81

	count = *(address++);
	/* Pump ratio control */
	ili9341_sendCmdCont(*(address++)); 			//0xF7
	ili9341_sendDataCont8((address++), 1); 	//0x20

	count = *(address++);
	/* Power Control 1 */
	ili9341_sendCmdCont(*(address++)); 			//0xC0
	ili9341_sendDataCont8((address++), 1); 	//0x23

	count = *(address++);
	/* Power Control 2 */
	ili9341_sendCmdCont(*(address++)); 			//0xC1
	ili9341_sendDataCont8((address++), 1); 	//0x10

	count = *(address++);
	/* VCOM Control 1 */
	ili9341_sendCmdCont(*(address++)); 			//0xC5
	ili9341_sendDataCont8((address++), 1); 	//0x3E
	ili9341_sendDataCont8((address++), 1); 	//0x28

	count = *(address++);
	/* VCOM Control 2 */
	ili9341_sendCmdCont(*(address++)); 			//0xC7
	ili9341_sendDataCont8((address++), 1); 	//0x86

	count = *(address++);
	/* VCOM Control 2 */
	ili9341_sendCmdCont(*(address++)); 			//0x36
	ili9341_sendDataCont8((address++), 1); 	//0x48
	
	count = *(address++);
	/* Pixel Format Set */
	ili9341_sendCmdCont(*(address++)); 			//0x3A
	ili9341_sendDataCont8((address++), 1); 	//0x55      //16bit

	count = *(address++);
	ili9341_sendCmdCont(*(address++)); 			//0xB1
	ili9341_sendDataCont8((address++), 1); 	//0x00
	ili9341_sendDataCont8((address++), 1); 	//0x18

	count = *(address++);
	/* Display Function Control */
	ili9341_sendCmdCont(*(address++)); 			//0xB6
	ili9341_sendDataCont8((address++), 1); 	//0x08
	ili9341_sendDataCont8((address++), 1); 	//0x82
	ili9341_sendDataCont8((address++), 1); 	//0x27

	count = *(address++);
	/* 3GAMMA FUNCTION DISABLE */
	ili9341_sendCmdCont(*(address++)); 			//0xF2
	ili9341_sendDataCont8((address++), 1); 	//0x00

	count = *(address++);
	/* GAMMA CURVE SELECTED */
	ili9341_sendCmdCont(*(address++)); 			//0x26   //Gamma set
	ili9341_sendDataCont8((address++), 1);  //0x01  //Gamma Curve (G2.2)

	count = *(address++);
	//Positive Gamma  Correction
	ili9341_sendCmdCont(*(address++)); 			//0xE0
	ili9341_sendDataCont8((address++), 1); 					//0x0F, 1);
	ili9341_sendDataCont8((address++), 1); 				//0x31, 1);
	ili9341_sendDataCont8((address++), 1); 				//0x2B, 1);
	ili9341_sendDataCont8((address++), 1); 				//0x0C, 1);
	ili9341_sendDataCont8((address++), 1); 				//0x0E, 1);
	ili9341_sendDataCont8((address++), 1); 				//0x08, 1);
	ili9341_sendDataCont8((address++), 1); 				//0x4E, 1);
	ili9341_sendDataCont8((address++), 1); 				//0xF1, 1);
	ili9341_sendDataCont8((address++), 1); 			//0x37, 1);
	ili9341_sendDataCont8((address++), 1); 			//0x07, 1);
	ili9341_sendDataCont8((address++), 1); 				//0x10, 1);
	ili9341_sendDataCont8((address++), 1); 				//0x03, 1);
	ili9341_sendDataCont8((address++), 1); 				//0x0E, 1);
	ili9341_sendDataCont8((address++), 1); 				//0x09, 1);
	ili9341_sendDataCont8((address++), 1); 				//)0x00, 1);

	count = *(address++);
	//Negative Gamma  Correction
	ili9341_sendCmdCont(*(address++)); 				//0xE1
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);
	ili9341_sendDataCont8((address++), 1);

	//EXIT SLEEP
	ili9341_sendCmdCont(0x11);
	Delayms(120);
	//TURN ON DISPLAY
	ili9341_sendCmdCont(0x29);

	dev_spi_stop(m_spi);
	LCD_setOrientation(0);
}

void LCD_reset()
{
	ILI9341_RST_RESET;
	Delayms(50);
	ILI9341_RST_SET;
	//Delayms(50);

	Delayms(50);
	ili9341_sendCmd(0x01);

}

void LCD_exitStandby()
{
	ili9341_sendCmd(LCD_SLEEP_OUT);
	Delayms(150);
	ili9341_sendCmd(LCD_DISPLAY_ON);
}


void LCD_init()
{
	ili9341_spi_init();
	LCD_reset();
	LCD_configure();

}

void LCD_setOrientation(u8 o)
{
	if (o == ORIENTATION_LANDSCAPE || o == ORIENTATION_LANDSCAPE_MIRROR)
	{
		screen_height = LCD_PIXEL_WIDTH;
		screen_width = LCD_PIXEL_HEIGHT;
	}
	else
	{
		screen_height = LCD_PIXEL_HEIGHT;
		screen_width = LCD_PIXEL_WIDTH;
	}
	dev_spi_start(m_spi);
	ili9341_sendCmdCont(LCD_MAC);
	ili9341_sendDataCont8(&o, 1);
	dev_spi_stop(m_spi);
}


inline void LCD_setAddressWindow(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 pointData[2];

	dev_spi_start(m_spi);
	ili9341_sendCmdCont(LCD_COLUMN_ADDR);
	pointData[0] = x1;
	pointData[1] = x2;
	dev_spi_set16(m_spi);
	ili9341_sendDataCont16(pointData, 2);
	dev_spi_set8(m_spi);

	ili9341_sendCmdCont(LCD_PAGE_ADDR);
	pointData[0] = y1;
	pointData[1] = y2;
	dev_spi_set16(m_spi);
	ili9341_sendDataCont16(pointData, 2);
	dev_spi_set8(m_spi);
	dev_spi_stop(m_spi);
}

inline u16 LCD_getWidth()
{
	return screen_width;
}

inline u16 LCD_getHeight()
{
	return screen_height;
}
