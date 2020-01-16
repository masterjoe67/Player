#include "phone.h"
#include "cwMemory.h"

#include <stdio.h>
int day, month, year, hour, minute, second;

bool phone_initialized = false;


void onIncomingCall()
{
	cwMemoryPlayFile("");
}

void phoneInit()
{
	phone_initialized = false;
	LCD_fillScreen(BLACK);
	LCD_setTextBgColor(BLACK);
	LCD_setTextSize(1);
	LCD_setTextColor(WHITE);
	LCD_setCursor(0, 0);
	LCD_setFont(&FreeMono9pt7b);
	LCD_writeString((unsigned char*)"Inizializzazione GSM....\r\n");
	if (Sim800l_begin())
	{
		phone_initialized = true;
		LCD_setTextColor(GREEN);
		LCD_writeString((unsigned char*)"GSM ok\n\r");
	}
	else
	{
		phone_initialized = false;
		LCD_setTextColor(RED);
		LCD_writeString((unsigned char*)"GSM fault\n\r");
	}
	char text[32];
	
	LCD_setTextColor(WHITE);
	LCD_writeString((unsigned char*)"IMEI: ");	
	
	if (Sim800l_getIMEI(text) > 0)
	{
		LCD_setTextColor(YELLOW);
		LCD_writeString((unsigned char*)text);
		LCD_writeString((unsigned char*)"\n\r");
	}
	else
	{
		LCD_setTextColor(RED);
		LCD_writeString((unsigned char*)"non disponibile\n\r");
	}
	LCD_setTextColor(WHITE);
	LCD_writeString((unsigned char*)"RSSI: ");
	int RSSI = Sim800l_getRSSI();
	if (RSSI > 0)
	{
		sprintf(text, "%d", RSSI);
		LCD_setTextColor(YELLOW);
		LCD_writeString((unsigned char*)text);
		LCD_writeString((unsigned char*)"\n\r");
	}
	else
	{
		LCD_setTextColor(RED);
		LCD_writeString((unsigned char*)"non disponibile\n\r");
	}
	Sim800l_enableRTC(1);
	Sim800l_getTime(text, 32);
	LCD_writeString((unsigned char*)text);
	uint16_t bat_percentage;
	Sim800l_getBattPercent(&bat_percentage);
	sprintf(text, "%d", bat_percentage);
	LCD_setTextColor(YELLOW);
	LCD_writeString((unsigned char*)text);

	
	phone_initialized = true;
}