#include "utility.h"
#include "FreeMonoBold18pt7b.h"
#include "ff.h"
#include "tm_stm32f4_fatfs.h"
#include "tm_stm32f4_usb_msc_host.h"

bool searchUSB() {
	LCD_fillScreen(BLACK);
	LCD_setFont(&FreeMonoBold18pt7b);
	LCD_setTextSize(1);
	LCD_setTextColor(WHITE);
	LCD_setTextBgColor(BLACK);
	LCD_setCursor(50, 100);
	LCD_writeString("ATTENDERE");

	uint32_t curtime = TM_Time + SEARCH_TIMEOUT;
	
	while (curtime > TM_Time)
	{
		if (TM_USB_MSCHOST_Device() != TM_USB_MSCHOST_Result_Connected)
		{
			TM_USB_MSCHOST_Process();
		}
		else return true;
	}
	LCD_fillScreen(BLACK);
	LCD_setCursor(50, 100);
	LCD_writeString("USB NON TROVATO");
	LCD_setCursor(50, 120);
	LCD_writeString("TOCCARE PER USCIRE");

	while (!TouchPressed()) ;
	return false;
	
	
}

const char *GetFilenameExt(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) return "";
	return dot + 1;
}

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}