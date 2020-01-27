#include "phone.h"
#include "cwMemory.h"
#include "tm_stm32f4_delay.h"
#include "display.h"
#include "background.h"
#include "icons_included.h"
#include "icon.h"
#include <stdio.h>

#pragma GCC push_options
#pragma GCC optimize ("O0")

int day, month, year, hour, minute, second;

uint8_t callStatus = 2;
bool phone_initialized, call_in_progress = false;
char phone_number[20];
uint8_t number_index = 0;



BTN_Label_TypeDef but_key[16], btn_back, btn_call, btn_respond, btn_refuse;
BTN_TypeDef btn_exit;

const int widthButtonPhoneKey = 75;
const int heightButtonPhoneKey = 39;
const int xstartButtonPhoneKey[] = { 4, 83, 162, 4, 83, 162, 4, 83, 162, 4, 83, 162 };                           // x-min for keypads
const int ystartButtonPhoneKey[] = { 64, 64, 64, 107, 107, 107, 150, 150, 150, 193, 193, 193 };                     // y-min for keypads
const char labelButtonPhoneKey[] = { "123456789*0#" };

int getButtonAnswer(int xInput, int yInput)
{
	if (BTN_isPressed(&btn_respond, xInput, yInput))
	{
		return 1; 
	}
	else if (BTN_isPressed(&btn_refuse, xInput, yInput))
	{
		return 2; 
	}
	return -1;
}

int getButtonKey(int xInput, int yInput)
{

	if (BTN_isPressed(&but_key[0], xInput, yInput))
	{
		return 1; 
	}
	else if (BTN_isPressed(&but_key[1], xInput, yInput))
	{
		return 2; 
	}
	else if (BTN_isPressed(&but_key[2], xInput, yInput))
	{
		return 3; 
	}
	else if (BTN_isPressed(&but_key[3], xInput, yInput))
	{
		return 4; 
	}
	else if (BTN_isPressed(&but_key[4], xInput, yInput))
	{
		return 5; 
	}
	else if (BTN_isPressed(&but_key[5], xInput, yInput))
	{
		return 6; 
	}
	else if (BTN_isPressed(&but_key[6], xInput, yInput))
	{
		return 7; 
	}
	else if (BTN_isPressed(&but_key[7], xInput, yInput))
	{
		return 8; 
	}
	else if (BTN_isPressed(&but_key[8], xInput, yInput))
	{
		return 9; 
	}
	else if (BTN_isPressed(&but_key[9], xInput, yInput))
	{
		return 10; 
	}
	else if (BTN_isPressed(&but_key[10], xInput, yInput))
	{
		return 0; 
	}
	else if (BTN_isPressed(&but_key[11], xInput, yInput))
	{
		return 11; 
	}
	else if (BTN_isPressed(&btn_exit, xInput, yInput))
	{
		return 12;
	}
	else if (BTN_isPressed(&btn_back, xInput, yInput))
	{
		return 13;
	}
	else if (BTN_isPressed(&btn_call, xInput, yInput))
	{
		return 14;
	}

	
	return -1;
}

void exit_btn_phone_helper()
{
	BTN_initUL(&btn_exit, 303, 2, 16, 16, WHITE, BLUE, WHITE, "", 8, 8);
	LCD_drawRGBBitmap(303, 2, (uint16_t *)image_data_Cancel, 16, 16);
}

void number_helper()
{
	char* ptr = phone_number;
	LCD_fillRect(0, 32, 319, 24, WHITE);
	LCD_setTextColor(BLACK);
	LCD_setFont(&FreeMonoBold12pt7b);
	LCD_setCursor(10, 50);
	LCD_writeString((unsigned char*)ptr);
}

void call_btn_helper(uint8_t call)
{
	if (call == 1)
		BTN_Label_initUL(&btn_call, 241, 193, widthButtonPhoneKey, heightButtonPhoneKey, LGRAY, RED, WHITE, "H", 1, &FreeMonoBold18pt7b);
	else if (call == 0)
		BTN_Label_initUL(&btn_call, 241, 193, widthButtonPhoneKey, heightButtonPhoneKey, LGRAY, GREEN, WHITE, "C", 1, &FreeMonoBold18pt7b);
	else if (call == 2)
		BTN_Label_initUL(&btn_call, 241, 193, widthButtonPhoneKey, heightButtonPhoneKey, LGRAY, LGRAY, WHITE, "C", 1, &FreeMonoBold18pt7b);
	
	BTN_Label_draw(&btn_call); 
}

void call_progress(char* phone, bool out)
{
	BG_DisableBGTimer();
	BTN_Label_initUL(&btn_refuse, 136, 120, 48, 48, LGRAY, RED, WHITE, "NO", 1, &FreeMonoBold18pt7b);
	LCD_fillRect(32, 32, 256, 176, BLACK);
	LCD_fillRect(34, 34, 252, 172, WHITE);
	LCD_setTextColor(BLACK);
	LCD_setFont(&FreeMonoBold12pt7b);
	LCD_setCursor(80, 50);
	if (out)
		LCD_writeString((unsigned char*)"Chiama\r\n\r\n");
	else
		LCD_writeString((unsigned char*)"Chiamata da\r\n\r\n");
	LCD_setCursorX(80);
	LCD_writeString((unsigned char*)phone);
	BTN_Label_draw(&btn_refuse);
	for (;;)
	{
		
		if (TouchPressed())
		{
			
			getDisplayPoint(&display, Read_Value(), &matrix);

			int userInput = -1; 
			userInput = getButtonAnswer(display.x, display.y);

			if (userInput == 2)
			{
				call_in_progress = false;
				BTN_Label_initUL(&btn_refuse, 136, 120, 48, 48, LGRAY, LGRAY, WHITE, "NO", 1, &FreeMonoBold18pt7b);
				BTN_Label_draw(&btn_refuse);
				Sim800l_hangUp();
				//break;
			}	
			
		}
		if ((Sim800l_IsEvent() & 0x02) || (Sim800l_isCallActive() == 0 && !call_in_progress))
		{
			Sim800l_hangUp();
			break;
		}
		Delayms(10);
		
	}
	
}

bool isRinging()
{
	//bool Sim800l_IsIncomingCall() {return _Sim800l._incomingCall; }
	//void Sim800l_clearIncomingCall() {_Sim800l._incomingCall = false; }
	if(Sim800l_IsEvent() & 0x01)	//Ring
	{
		BG_DisableBGTimer();
		char phone[32] = { 0 };
		if (Sim800l_incomingCallNumber(phone)) {
			BTN_Label_initUL(&btn_respond, 64, 120, 48, 48, LGRAY, GREEN, WHITE, "OK", 1, &FreeMonoBold18pt7b);
			BTN_Label_initUL(&btn_refuse, 200, 120, 48, 48, LGRAY, RED, WHITE, "NO", 1, &FreeMonoBold18pt7b);
	
			 
			LCD_fillRect(32, 32, 256, 176, BLACK);
			LCD_fillRect(34, 34, 252, 172, WHITE);
			LCD_setTextColor(BLACK);
			LCD_setFont(&FreeMonoBold12pt7b);
			LCD_setCursor(80, 50);
			LCD_writeString((unsigned char*)"Chiamata da\r\n\r\n");
			LCD_setCursorX(80);
			LCD_writeString((unsigned char*)phone);
			BTN_Label_draw(&btn_respond);
			BTN_Label_draw(&btn_refuse);
			
			for (;;)
			{
				if ((Sim800l_IsEvent() & 0x02) || Sim800l_isCallActive() == 0)
				{
					Sim800l_hangUp();
					break;
				}
				if (TouchPressed())
				{
			
					getDisplayPoint(&display, Read_Value(), &matrix);

					int userInput = -1; 
					userInput = getButtonAnswer(display.x, display.y);
					if (userInput >= 0)
					{
						if (userInput == 1)
						{
							Sim800l_pickUp();
							call_progress(phone, false);
							break;
						}
						else if (userInput == 2)
						{
							Sim800l_hangUp();
							break;
						}	
					}
				}
				Delayms(5);
			}
			BG_EnableBGTimer();
			Sim800l_clearEvent();
			return true;
		}
		
	}
	Sim800l_clearEvent();
	return false;
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
	if (phone_initialized)
	{
		 
		char text[32];
		Sim800l_setAudio(0);
		Sim800l_setVolume(100);
		Sim800l_setMicVolume(0, 100);
		
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
		if (Sim800l_getSIMProvider(text) > 0)
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
		Sim800l_callerIdNotification(true);
		Delayms(2000);
		
	}
	//phone_initialized = true;
}

void phone_call_draw()
{
	BG_DisableBGTimer();
	LCD_setFont(&FreeMonoBold18pt7b);
	
	LCD_fillScreen(WHITE);
	char label[2];
	for (int i = 0; i < 12; i++)
	{
		label[0] = labelButtonPhoneKey[i];
		label[1] = 0x00;
		BTN_Label_initUL(&but_key[i], xstartButtonPhoneKey[i], ystartButtonPhoneKey[i], widthButtonPhoneKey, heightButtonPhoneKey, LGRAY, WHITE, BLUE, label, 1, &FreeMonoBold18pt7b);
		BTN_Label_draw(&but_key[i]); 
	}
	call_btn_helper(false); 
	BTN_Label_initUL(&btn_back, 241, 64, widthButtonPhoneKey, heightButtonPhoneKey, LGRAY, WHITE, BLUE, "<-", 1, &FreeMonoBold18pt7b); 
	BTN_Label_draw(&btn_back); 
	exit_btn_phone_helper();
		
	battery_helper();
	number_helper();
	BG_EnableBGTimer();
}

void phone_call()
{
	phone_number[0] = '\0';
	number_index = 0;
	
	phone_call_draw();
	
	
	uint8_t oldlevel = battery_level;
	vbat_refresh = true;
	
	uint32_t last_time;
	uint8_t call_btn_status = 0;

#define key_delay 400
	Delayms(100);
	
	/* Infinite loop */
	for (;;)
	{
		if (isRinging()) phone_call_draw();
		
		if (LONG_CYCLE_Flag)
		{
			BG_DisableBGTimer();
			GSM_helper();
			
			if (battery_level != oldlevel)
			{
				oldlevel = battery_level;
				
				battery_helper();
				
			}
			
			
			if (call_in_progress) 
			{
				callStatus = Sim800l_isCallActive();
				if (callStatus == 0)
				{
					//BG_DisableBGTimer();
					call_btn_helper(0);
					call_in_progress = false;
					//BG_EnableBGTimer();
				}
			}
			else
			{
				callStatus = Sim800l_isCallActive();
				if (callStatus != 0)
				{
					Sim800l_hangUp();
				}
				else if (call_btn_status == 2)
				{
					call_btn_status = 0;
					call_btn_helper(0);
				}
			}
			LONG_CYCLE_Flag = false;
			BG_EnableBGTimer();
			
		}
		
		if (TouchPressed())
		{
			
			getDisplayPoint(&display, Read_Value(), &matrix);

			int userInput = -1; 
			userInput = getButtonKey(display.x, display.y);
			if (userInput >= 0  && (TM_Time - last_time) > key_delay)
			{
				BG_DisableBGTimer();
				last_time = TM_Time;
				switch (userInput)
				{
				case 1:
					if (number_index < 20)
					{
						phone_number[number_index] = 0x31;
						number_index++;
						phone_number[number_index] = '\0';
						
						number_helper();
					}
					break;
				case 2:
					if (number_index < 20)
					{
						phone_number[number_index] = 0x32;
						number_index++;
						phone_number[number_index] = '\0';
						number_helper();
					}
					break;
				case 3:
					if (number_index < 20)
					{
						phone_number[number_index] = 0x33;
						number_index++;
						phone_number[number_index] = '\0';
						number_helper();
					}
					break;
				case 4:
					if (number_index < 20)
					{
						phone_number[number_index] = 0x34;
						number_index++;
						phone_number[number_index] = '\0';
						number_helper();
					}
					break;
				case 5:
					if (number_index < 20)
					{
						phone_number[number_index] = 0x35;
						number_index++;
						phone_number[number_index] = '\0';
						number_helper();
					}
					break;
				case 6:
					if (number_index < 20)
					{
						phone_number[number_index] = 0x36;
						number_index++;
						phone_number[number_index] = '\0';
						number_helper();
					}
					break;
				case 7:
					if (number_index < 20)
					{
						phone_number[number_index] = 0x37;
						number_index++;
						phone_number[number_index] = '\0';
						number_helper();
					}
					break;
				case 8:
					if (number_index < 20)
					{
						phone_number[number_index] = 0x38;
						number_index++;
						phone_number[number_index] = '\0';
						number_helper();
					}
					break;
				case 9:
					if (number_index < 20)
					{
						phone_number[number_index] = 0x39;
						number_index++;
						phone_number[number_index] = '\0';
						number_helper();
					}
					break;
				case 10:
					if (number_index < 20)
					{
						phone_number[number_index] = 0x2A;
						number_index++;
						phone_number[number_index] = '\0';
						number_helper();
					}
					break;
				case 0:
					if (number_index < 20)
					{
						phone_number[number_index] = 0x30;
						number_index++;
						phone_number[number_index] = '\0';
						number_helper();
					}
					break;
				case 11:
					if (number_index < 20)
					{
						phone_number[number_index] = 0x23;
						number_index++;
						phone_number[number_index] = '\0';
						number_helper();
					}
					break;
				case 12:
					//__disable_irq();
					//BG_DisableBGTimer();
					return;
					break;
				case 13:
					if (number_index > 0)
					{
						number_index--;
						phone_number[number_index] = '\0';
						number_helper();
					}
					break;
				case 14:
					if (number_index > 0 && !call_in_progress && Sim800l_isCallActive() == 0)
					{
						//BG_DisableBGTimer();
						call_in_progress = true;
						//call_btn_status = 1;
						//call_btn_helper(1);
						Sim800l_callPhone(phone_number);
						call_progress(phone_number, true);
						phone_call_draw();
						//BG_EnableBGTimer();
					}
					else if (call_in_progress)
					{
						//BG_DisableBGTimer();
						
						call_btn_status = 2;
						call_btn_helper(2);
						call_in_progress = false;
						//BG_EnableBGTimer();
					}
					break;
				}
				
			}
			BG_EnableBGTimer();
		}
		/*if (battery_level != oldlevel)
		{
			oldlevel = battery_level;
			battery_helper();
		}*/
		//Delayms(10);
	}
	
	
}

void call()
{
	Sim800l_callPhone("3468783682");
}

void sms()
{
	Sim800l_sendSMS("3391382648", "TDK funziona??"); 
}

#pragma GCC pop_options