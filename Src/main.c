

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "stdio.h"
#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_usart.h"
#include "tm_stm32f4_lis302dl_lis3dsh.h"
#include "tm_stm32f4_fatfs.h"
#include "tm_stm32f4_usb_msc_host.h"
#include "cwSoundFile.h"
#include "display.h"
#include "ili9341_spi.h"
#include "ili9341_core.h"
#include "ili9341_graph.h"
#include "ili9341_text.h"
#include "xpt2046.h"
#include "calc.h"
#include "player.h"
#include "fonts.h"
#include "background.h"
#include "phone.h"

uint8_t state = 0;
void hwInit();
int x, y;


int main(void) {
	system_initialized = false;
	touch_irq = 0;
	hwInit();

	LCD_init();
	LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
	LCD_fillScreen(BLACK);
	/*LCD_setTextBgColor(BLACK);
	LCD_setTextSize(4);
	LCD_setTextColor(YELLOW);
	LCD_setCursor(0, 0);
	LCD_fillRect(10, 10, 40, 30, WHITE);
	LCD_drawCircle(100, 100, 10, YELLOW);
	LCD_writeString("Ciao");*/
	
	//uint16_t *buf = { RED, RED, RED, RED };
	//LCD_setAddressWindowToWrite(10, 10, 13, 10);
	//dev_spi_set16(m_spi);
	
	//ili9341_sendData16(buf, 4);
	
	//dev_spi_set8(m_spi);
	
	/*LCD_setTextColor(WHITE);
	LCD_setTextSize(1);
	LCD_setTextBgColor(BLACK);
	LCD_setCursor(0, 0);
	LCD_setFont(&FreeMono24pt7b);
	//
	
	LCD_writeString("CIAO come va");*/
	
	phoneInit();
	
	
	//Delayms(5000);

	
	xpt2046_init();

	//ts_calibrate(X_SIZE, Y_SIZE);


	system_initialized = true;
	
	
	while (1)
	{
		
		switch (Display_mainMenu())
		{
		case modeCalc:
			Display_calc();
			break;
		case modeFoto:
			slideshow();
			break;
		case modePaint:
			paint();
			break;
		case modeCall:
			phone_call();
			//__enable_irq();
			BG_EnableBGTimer();
			break;
		case modeMP3:
			mediaplayer();
			__enable_irq();
			break;
		}
	}
  
	/*while (1) {
		TM_USB_MSCHOST_Process();
		if (TM_USB_MSCHOST_Device() == TM_USB_MSCHOST_Result_Connected) {
			while (1) {
				cwSFPlayDirectory("1:", 0);
			}
		}
	}*/
}

void MP3_Player()
{
	if (TM_USB_MSCHOST_Device() != TM_USB_MSCHOST_Result_Connected)
	{
		TM_USB_MSCHOST_Process();
	}	
	else if (TM_USB_MSCHOST_Device() == TM_USB_MSCHOST_Result_Connected) {
			while (1) {
				cwSFPlayDirectory("1:", 0);
			}
		}
	
	
}


void hwInit() {
	// Initialize System
	//SystemInit();
	if(SysTick_Config(SystemCoreClock / 1000)) {
		// Capture error
		while(1) {}
;
	}
	// Initialise leds on board
	TM_DISCO_LedInit();
	// Initialise delay Systick timer
	TM_DELAY_Init();
	// Initialize background timer
	BG_Init();
	//Initialise USART2 at 115200 baud, TX: PA2, RX: PA3
	TM_USART_Init(USART2, TM_USART_PinsPack_1, 115200);
	// Initialize button on board
	TM_DISCO_ButtonInit();
	/* Initialize USB MSC HOST */
	TM_USB_MSCHOST_Init();
	
    
//	/* Detect proper device */
//	if (TM_LIS302DL_LIS3DSH_Detect() == TM_LIS302DL_LIS3DSH_Device_LIS302DL) {
//		/* Turn on GREEN and RED */
//		TM_DISCO_LedOn(LED_GREEN | LED_RED);
//		/* Initialize LIS302DL */
//		TM_LIS302DL_LIS3DSH_Init(TM_LIS302DL_Sensitivity_2_3G, TM_LIS302DL_Filter_2Hz);
//	}
//	else if (TM_LIS302DL_LIS3DSH_Detect() == TM_LIS302DL_LIS3DSH_Device_LIS3DSH) {
//		/* Turn on BLUE and ORANGE */
//		TM_DISCO_LedOn(LED_BLUE | LED_ORANGE);
//		/* Initialize LIS3DSH */
//		TM_LIS302DL_LIS3DSH_Init(TM_LIS3DSH_Sensitivity_2G, TM_LIS3DSH_Filter_800Hz);
//	}
//	else {
//		/* Device is not recognized */      
//		/* Turn on ALL leds */
//		TM_DISCO_LedOn(LED_GREEN | LED_RED | LED_BLUE | LED_ORANGE);
//      
//		/* Infinite loop */
//		while (1) ;
//	}

	Delayms(1000);
}
