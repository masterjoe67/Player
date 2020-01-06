
#include "calc.h"
#include "lcdButton.h"
#include "xpt2046.h"
#include "ili9341_text.h"
#include "ili9341_graph.h"
#include "tm_stm32f4_delay.h"
#include "fonts.h"


// Button position in main menu
const int widthButtonCalc = 75;
const int heightButtonCalc = 39;
const int xstartButtonCalc[] = { 4, 83, 162, 241, 4, 83, 162, 241, 4, 83, 162, 241, 4, 83, 162, 241 };                        // x-min for keypads
const int ystartButtonCalc[] = { 64, 64, 64, 64, 107, 107, 107, 107,  150, 150, 150, 150, 193, 193, 193, 193};                  // y-min for keypads
const char labelButtonCalc[] = { "789/456+123-.0*=" };

void Display_calc()
{
	LCD_setFont(&FreeMonoBold18pt7b);
	BTN_Label_TypeDef but_icon[16];
	LCD_fillScreen(BLACK);
	char label[2];
	for (int i = 0; i < 16; i++)
	{
		label[0] = labelButtonCalc[i];
		label[1] = 0x00;
		BTN_Label_initUL(&but_icon[i], xstartButtonCalc[i], ystartButtonCalc[i], widthButtonCalc, heightButtonCalc, LGRAY, BLACK, GREEN, label, 1, &FreeMono24pt7b);
		BTN_Label_draw(&but_icon[i]); 
	}
	
	/*i++;
	BTN_initUL(&but_icon[i], xstartButtonCalc[i], ystartButtonCalc[i], widthButtonCalc, heightButtonCalc, LGRAY, WHITE, BLACK, "8", 2, 2);
	BTN_draw(&but_icon[i], 0); 
	i++;
	BTN_initUL(&but_icon[i], xstartButtonCalc[i], ystartButtonCalc[i], widthButtonCalc, heightButtonCalc, LGRAY, WHITE, BLACK, "9", 2, 2);
	BTN_draw(&but_icon[i], 0); 
	i++;
	BTN_initUL(&but_icon[i], xstartButtonCalc[i], ystartButtonCalc[i], widthButtonCalc, heightButtonCalc, LGRAY, WHITE, BLACK, "/", 2, 2);
	BTN_draw(&but_icon[i], 0); 
	i++;
	BTN_initUL(&but_icon[i], xstartButtonCalc[i], ystartButtonCalc[i], widthButtonCalc, heightButtonCalc, LGRAY, WHITE, BLACK, "4", 2, 2);
	BTN_draw(&but_icon[i], 0); 
	i++;
	BTN_initUL(&but_icon[i], xstartButtonCalc[i], ystartButtonCalc[i], widthButtonCalc, heightButtonCalc, LGRAY, WHITE, BLACK, "5", 2, 2);
	BTN_draw(&but_icon[i], 0); 
	i++;
	BTN_initUL(&but_icon[i], xstartButtonCalc[i], ystartButtonCalc[i], widthButtonCalc, heightButtonCalc, LGRAY, WHITE, BLACK, "6", 2, 2);
	BTN_draw(&but_icon[i], 0); 
	i++;
	BTN_initUL(&but_icon[i], xstartButtonCalc[i], ystartButtonCalc[i], widthButtonCalc, heightButtonCalc, LGRAY, WHITE, BLACK, "7", 2, 2);
	BTN_draw(&but_icon[i], 0); 
	i++;
	BTN_initUL(&but_icon[i], xstartButtonCalc[i], ystartButtonCalc[i], widthButtonCalc, heightButtonCalc, LGRAY, WHITE, BLACK, "6", 2, 2);
	BTN_draw(&but_icon[i], 0); 
	i++;*/
	
	/* Infinite loop */
	for (;;)
	{

		if (TouchPressed())
		{
			getDisplayPoint(&display, Read_Value(), &matrix);

			int userInput = -1; 
//			userInput = getButtonNumber(display.x, display.y);
			if (userInput >= 0)
			{
				switch (userInput)
				{
				case 1:
					
					break;
				case 2:
					//return modeGame;
					break;
				case 3:
					//return modePaint;
					return;
					break;
				case 4:
					//return modeMP3;
					break;
				case 5:
					//return modeMP3;
					break;
				case 6:
					//return modeMP3;
					break;
				case 7:
					//return modeMP3;
					break;
				case 8:
					return;
					break;
					
				}
			}
		}
		Delayms(5);
	}
	
	
}
