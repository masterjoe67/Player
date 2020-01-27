#include "display.h"
#include "cwMP3.h"
#include "icons_included.h"

#include "lcdButton.h"
#include "icon.h"
#include "xpt2046.h"
#include "stm32f4xx_it.h"
#include "ff.h"
#include "tm_stm32f4_fatfs.h"
#include "tm_stm32f4_usb_msc_host.h"
#include "string.h"
#include "tm_stm32f4_delay.h"
#include "jpg_interface.h"
#include "ili9341_text.h"
#include "ili9341_graph.h"
#include "Audio.h"
#include "utility.h"
#include "background.h"
#include "phone.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")

BTN_TypeDef btn_s1, btn_s2, btn_s3, btn_s4, btn_exit, btn_mute;
BTN_TypeDef buttons[12], btn_prev, btn_next, btn_play_stop, btn_incvol, btn_decvol, btn_pgup, btn_pgdown;
BTN_Icon_TypeDef but_icon[8];

DIR dir;
uint8_t mode = 0;
uint32_t lastTouch;

char name[20];

// Parameters for the array of buttons
const int xstartButton[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };                    // x-min for keypads
const int ystartButton[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216 };              // y-min for keypads


// Button position in main menu
const int widthButtonMain = 48;
const int heightButtonMain = 48;
const int xstartButtonMain[] = { 25, 99, 173, 247, 25, 99, 173, 247 };                      // x-min for keypads
const int ystartButtonMain[] = { 50, 50, 50, 50, 162, 162, 162, 162 };                // y-min for keypads


const int widthButton = 16;
const int heightButton = 24;



int getButtonNumber(int xInput, int yInput)
{
	if (mode == modeMain)
	{
		if (BTN_isPressed(&but_icon[0], xInput, yInput)) {
			return 1; 
		}
		else if (BTN_isPressed(&but_icon[1], xInput, yInput)) {
			return 2; 
		}
		else if (BTN_isPressed(&but_icon[2], xInput, yInput)) {
			return 3; 
		}
		else if (BTN_isPressed(&but_icon[3], xInput, yInput)) {
			return 4; 
		}
		else if (BTN_isPressed(&but_icon[4], xInput, yInput)) {
			return 5; 
		}
		else if (BTN_isPressed(&but_icon[5], xInput, yInput)) {
			return 6; 
		}
		else if (BTN_isPressed(&but_icon[6], xInput, yInput)) {
			return 7; 
		}
		else if (BTN_isPressed(&but_icon[7], xInput, yInput)) {
			return 8; 
		}
	}
	else if (mode == modePaint)
	{
		for (int i = 0; i < 10; i++) {
			if (BTN_isPressed(&buttons[i], xInput, yInput)) 
				return i; // Returns the button number that was pressed
		}
		if (BTN_isPressed(&btn_exit, xInput, yInput)) {
			return 11; // Signifies big button was pressed
		}
		//		if (btn_clear.isPressed(xInput, yInput)) {
		//			return 12; // Signifies big button was pressed
		//		}
		//		if (btn_save.isPressed(xInput, yInput)) {
		//			return 13; // Signifies big button was pressed
		//		}
		//		if (btn_load.isPressed(xInput, yInput)) {
		//			return 14; // Signifies big button was pressed
		//		}
	}
	else if (mode == modeFoto)
	{
		if (BTN_isPressed(&btn_exit, xInput, yInput)) {
			return btn_exit_ret; // Signifies big button was pressed
		}
	}
	else if (mode == modeMP3)
	{
		if (BTN_isPressed(&btn_exit, xInput, yInput))
			return btn_exit_ret; 
		if (BTN_isPressed(&btn_play_stop, xInput, yInput))
			return btn_play_ret;
		if (BTN_isPressed(&btn_incvol, xInput, yInput))
			return btn_incvol_ret;
		if (BTN_isPressed(&btn_decvol, xInput, yInput))
			return btn_decvol_ret;
		if (BTN_isPressed(&btn_mute, xInput, yInput))
			return btn_mute_ret;
		if (BTN_isPressed(&btn_prev, xInput, yInput))
			return btn_prev_ret;
		if (BTN_isPressed(&btn_next, xInput, yInput))
			return btn_next_ret;
		
	}
	else if (mode == modeMP3sel)
	{
		if (BTN_isPressed(&btn_pgup, xInput, yInput)) 
			return btn_pgup_ret;
		if (BTN_isPressed(&btn_pgdown, xInput, yInput)) 
			return btn_pgdown_ret;
		if (BTN_isPressed(&btn_exit, xInput, yInput))
			return btn_exit_ret;
		for (int y = 0; y < 6; y++)
		{
			if (BTN_isPressed(&buttons[y + 6], xInput, yInput))
				return y + btn_song_ret_1;
		}

	}
	return -1;
}

uint8_t battery_level = 0;

void battery_helper()
{
	switch (battery_level)
	{
	case 0: 
		LCD_drawRGBBitmap(250, 2, (uint16_t *)image_data_batt_0, 24, 24);
		break;
	case 1: 
		LCD_drawRGBBitmap(250, 2, (uint16_t *)image_data_batt_1, 24, 24);
		break;
	case 2: 
		LCD_drawRGBBitmap(250, 2, (uint16_t *)image_data_batt_2, 24, 24);
		break;
	case 3: 
		LCD_drawRGBBitmap(250, 2, (uint16_t *)image_data_batt_3, 24, 24);
		break;
	case 4: 
		LCD_drawRGBBitmap(250, 2, (uint16_t *)image_data_batt_4, 24, 24);
		break;
	}
}

void rssi_helper()
{
	RSSI = Sim800l_getRSSI();
	if (RSSI > 30)
		LCD_drawRGBBitmap(200, 0, (uint16_t *)image_data_rssi_5, 40, 32);
	else if (RSSI > 30)
		LCD_drawRGBBitmap(200, 0, (uint16_t *)image_data_rssi_4, 40, 32);
	else if (RSSI > 15)
		LCD_drawRGBBitmap(200, 0, (uint16_t *)image_data_rssi_3, 40, 32);
	else if (RSSI > 5)
		LCD_drawRGBBitmap(200, 0, (uint16_t *)image_data_rssi_2, 40, 32);
	else if (RSSI == 0)
		LCD_drawRGBBitmap(200, 0, (uint16_t *)image_data_rssi_1, 40, 32);
	else if (RSSI == 99)
		LCD_fillRect(200, 2, 40, 32, WHITE);
	
}

void provider_helper(uint8_t status)
{
	char text[32];
	LCD_setTextColor(BLACK);
	
	LCD_setFont(&FreeMono9pt7b);
	LCD_setCursor(20, 16);
	LCD_fillRect(20, 0, 140, 20, WHITE);
	
	if (status == 1)
	{
		if (Sim800l_getSIMProvider(text) > 0)
		{
			char *p, *p2;
			p = strchr(text, 0x22) + 1;    // '"'
			p2 = strrchr(text, 0x22); 
			uint8_t n = p2 - p;
		
			char subbuff[n];
			memcpy(subbuff, p, n);
			subbuff[n] = '\0';
			LCD_fillRect(20, 0, 140, 20, WHITE);
			LCD_writeString((unsigned char*)subbuff);

		}
	}
	else if (status == 2)
	{
		LCD_fillRect(20, 0, 140, 20, WHITE);
		LCD_writeString((unsigned char*)"Ricerca rete");
	}

}

void GSM_helper()
{
	NetworkStatus = Sim800l_getNetworkStatus();
	battery_helper();

		switch (NetworkStatus)
		{
		case 0:		//Not  registered,  MT  is  not  currently  searching  a  new 
					
			break;
		case 1:		//1    Registered, home network 
		case 5 : 		//5   Registered roaming	

				rssi_helper();
				provider_helper(1);	
			
			
			break;
		case 2:		//2    Not  registered,  but  MT  is  currently  searching  a  new
			
				provider_helper(2);	
			
			break;
		case 3:		//3   Registration    denied             
					
			break;
		case 4:		//4   Unknown
					
			break;

		}
		
	
}


void exit_btn_helper()
{
	BTN_initUL(&btn_exit, 303, 2, 16, 16, WHITE, BLUE, WHITE, "", 8, 8);
	LCD_drawRGBBitmap(303, 2, (uint16_t *)image_data_Cancel, 16, 16);
}

void Display_mainMenu_draw()
{
	BG_DisableBGTimer();
	LCD_fillScreen(WHITE);

	int i = 0;
	BTN_icon_initUL(&but_icon[i], xstartButtonMain[i], ystartButtonMain[i], widthButtonMain, heightButtonMain, (uint16_t *)image_data_AppsCalcicon);
	BTN_icon_draw(&but_icon[i]); 
	i++;
	BTN_icon_initUL(&but_icon[i], xstartButtonMain[i], ystartButtonMain[i], widthButtonMain, heightButtonMain, (uint16_t *)image_data_AppsClockicon);
	BTN_icon_draw(&but_icon[i]); 
	i++;
	BTN_icon_initUL(&but_icon[i], xstartButtonMain[i], ystartButtonMain[i], widthButtonMain, heightButtonMain, (uint16_t *)image_data_AppsGalleryicon);
	BTN_icon_draw(&but_icon[i]); 
	i++;
	BTN_icon_initUL(&but_icon[i], xstartButtonMain[i], ystartButtonMain[i], widthButtonMain, heightButtonMain, (uint16_t *)image_data_PreferencesCicon);
	BTN_icon_draw(&but_icon[i]); 
	i++;
	BTN_icon_initUL(&but_icon[i], xstartButtonMain[i], ystartButtonMain[i], widthButtonMain, heightButtonMain, (uint16_t *)image_data_AppsPhoneicon);
	BTN_icon_draw(&but_icon[i]); 
	i++;
	BTN_icon_initUL(&but_icon[i], xstartButtonMain[i], ystartButtonMain[i], widthButtonMain, heightButtonMain, (uint16_t *)image_data_Apps_Rubrica);
	BTN_icon_draw(&but_icon[i]); 
	i++;
	BTN_icon_initUL(&but_icon[i], xstartButtonMain[i], ystartButtonMain[i], widthButtonMain, heightButtonMain, (uint16_t *)image_data_AppsSubtitleeditoricon);
	BTN_icon_draw(&but_icon[i]); 
	i++;
	BTN_icon_initUL(&but_icon[i], xstartButtonMain[i], ystartButtonMain[i], widthButtonMain, heightButtonMain, (uint16_t *)image_data_AppsPlayerAudioCicon);
	BTN_icon_draw(&but_icon[i]); 

	battery_helper();
	
	vbat_refresh = true;
	BG_EnableBGTimer();
}

uint8_t Display_mainMenu()
{
	uint8_t oldlevel = battery_level;
	mode = modeMain;
	vbat_refresh = false;
	Display_mainMenu_draw();
	
	/* Infinite loop */
	for (;;)
	{
		if (isRinging()) Display_mainMenu_draw();
		if (LONG_CYCLE_Flag)
		{
			GSM_helper();
			LONG_CYCLE_Flag = false;
		}
		
		if (TouchPressed())
		{
			getDisplayPoint(&display, Read_Value(), &matrix);

			int userInput = -1; 
			userInput = getButtonNumber(display.x, display.y);
			if (userInput >= 0)
			{
				switch (userInput)
				{
				case 1:
					return modeCalc;
					break;
				case 2:
					//return modeGame;
					break;
				case 3:
					//return modePaint;
					return modeFoto;
					break;
				case 4:
					//return modeMP3;
					break;
				case 5:
					return modeCall;
					break;
				case 6:
					//return modeMP3;
					break;
				case 7:
					sms();
					break;
				case 8:
					return modeMP3;
					break;
					
				}
			}
		}
		if (battery_level != oldlevel)
		{
			oldlevel = battery_level;
			battery_helper();
		}
			
		Delayms(5);
	}
	
	
}

void paintMenu()
{
	
  
	// Sets the coordinates and sizes of the keypad buttons, and sets all the values for the buttons' text
	//const int noRows = 4;
	//const int noColumns = 3;
	LCD_fillScreen(BLACK);
	int y = 0; 
	BTN_initUL(&buttons[y], xstartButton[y], ystartButton[y], widthButton, heightButton, WHITE, BLACK, WHITE, "", 8, 8);
	y++;
	BTN_initUL(&buttons[y], xstartButton[y], ystartButton[y], widthButton, heightButton, WHITE, GREENYELLOW, WHITE, "", 8, 8);
	y++;
	BTN_initUL(&buttons[y], xstartButton[y], ystartButton[y], widthButton, heightButton, WHITE, MAGENTA, WHITE, "", 8, 8);
	y++;
	BTN_initUL(&buttons[y], xstartButton[y], ystartButton[y], widthButton, heightButton, WHITE, CYAN, WHITE, "", 8, 8);
	y++;
	BTN_initUL(&buttons[y], xstartButton[y], ystartButton[y], widthButton, heightButton, WHITE, WHITE, WHITE, "", 8, 8);
	y++;
	BTN_initUL(&buttons[y], xstartButton[y], ystartButton[y], widthButton, heightButton, WHITE, YELLOW, WHITE, "", 8, 8);
	y++;
	BTN_initUL(&buttons[y], xstartButton[y], ystartButton[y], widthButton, heightButton, WHITE, GREEN, WHITE, "", 8, 8);
	y++;
	BTN_initUL(&buttons[y], xstartButton[y], ystartButton[y], widthButton, heightButton, WHITE, MAROON, WHITE, "", 8, 8);
	y++;
	BTN_initUL(&buttons[y], xstartButton[y], ystartButton[y], widthButton, heightButton, WHITE, RED, WHITE, "", 8, 8);
	y++;
	BTN_initUL(&buttons[y], xstartButton[y], ystartButton[y], widthButton, heightButton, WHITE, BLUE, WHITE, "", 8, 8);
	

	
	/// Draw the array of buttons
	for(int i = 0 ; i < 10 ; i++) 
	  BTN_draw(&buttons[i], false);
	
	exit_btn_helper();
	
	//BTN_initUL(&btn_exit, 303, 2, 16, 16, WHITE, BLUE, WHITE, "", 8, 8);
	//btn_clear.initButtonUL(&Tft, 303, 26, 16, 16, WHITE, ILI9341_BLUE, WHITE, "", 8);
	//btn_save.initButtonUL(&Tft, 303, 50, 16, 16, WHITE, ILI9341_BLUE, WHITE, "", 8);
	//btn_load.initButtonUL(&Tft, 303, 74, 16, 16, WHITE, ILI9341_BLUE, WHITE, "", 8);
	//btn_exit.drawButton();
	//LCD_drawRGBBitmap(303, 2, (uint16_t *)image_data_Cancel, 16, 16);
	//Tft.drawRGBBitmap(303, 26, (uint16_t *)image_data_Trash, 16, 16);
	//Tft.drawRGBBitmap(303, 50, (uint16_t *)image_data_Save, 16, 16);
	//Tft.drawRGBBitmap(303, 74, (uint16_t *)image_data_Load, 16, 16);

	
}

void paint()
{
	mode = modePaint;
	uint16_t currentColor = WHITE;
	paintMenu();
	int x, y;
	Delayms(1000);
	/* Infinite loop */
	for (;;)
	{
		if (TouchPressed())
		{
			getDisplayPoint(&display, Read_Value(), &matrix);
			x = display.x;
			y = display.y;
			
			if (x > 0 && x<320 && y>0 && y < 240)
			{
				// Determine which button was pressed
				   int userInput = -1;         // -1 means no button is pressed
				   int w = widthButton;
			
				if ((x < w) || (x > 320 - w))
				{
					userInput = getButtonNumber(x, y);
				}
			
				if (userInput >= 0)
				{
					switch (userInput)
					{
					case 9:
						currentColor = BLUE;
						break;
					case 8:
						currentColor = RED;
						break;
					case 7:
						currentColor = MAROON;
						break;
					case 6:
						currentColor = GREEN;
						break;
					case 5:
						currentColor = YELLOW;
						break;
					case 4:
						currentColor = WHITE;
						break;
					case 3:
						currentColor = CYAN;
						break;
					case 2:
						currentColor = MAGENTA;
						break;
					case 1:
						currentColor = GREENYELLOW;
						break;
					case 0:
						currentColor = BLACK;
						break;
					case 11:
						return;
						break;
					case 12:
						//clearPad(currentColor);
						break;
					case 13:
				
						//save();
						break;
					case 14:
				
						//load();
						break;
					}
				}
				if (x > (widthButton + 2) && x < (320 - widthButton - 3))
				{
					if (x >= 0 && y >= 0)
						LCD_fillCircle(x, y, 2, currentColor);
					
				}
			}

		}	
		//osDelay(1);
	}
	
}

int slideshowMenu()
{
	int x, y;
	int userInput = -1;
	LCD_drawRGBBitmap(303, 2, (uint16_t *)image_data_Cancel, 16, 16);
	uint32_t curTime = TM_Time;
	uint32_t endTime = curTime + 5000;
	while (TM_Time < endTime)
	{
		
	
		if (TouchPressed())
		{
			getDisplayPoint(&display, Read_Value(), &matrix);
			//userInput = -1;
			userInput = getButtonNumber(display.x, display.y);
			if (userInput == 11) return userInput;
		}
	}
	return userInput;
}

void renderJPEG(int xpos, int ypos) {

	// retrieve infomration about the image
	uint16_t *pImg;
	uint16_t mcu_w = JpegDec_MCUWidth();
	uint16_t mcu_h = JpegDec_MCUHeight();
	uint32_t max_x = JpegDec_width();
	uint32_t max_y = JpegDec_height();
	uint32_t ofs_x = (LCD_getWidth() - max_x) / 2; 
	uint32_t ofs_y = (LCD_getHeight() - max_y) / 2; 
	if (ofs_x < 0) ofs_x = 0;
	if (ofs_y < 0) ofs_y = 0;
	xpos += ofs_x;
	ypos += ofs_y;

	// Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
	// Typically these MCUs are 16x16 pixel blocks
	// Determine the width and height of the right and bottom edge image blocks
	uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
	uint32_t min_h = minimum(mcu_h, max_y % mcu_h);

	// save the current image block size
	uint32_t win_w = mcu_w;
	uint32_t win_h = mcu_h;

	// record the current time so we can measure how long it takes to draw an image
	uint32_t drawTime = TM_Time;

	// save the coordinate of the right and bottom edges to assist image cropping
	// to the screen size
	max_x += xpos;
	max_y += ypos;
	//ILI9341_startWrite();
	// read each MCU block until there are no more
	while(JpegDec_read()) {

		// save a pointer to the image block
		pImg = JpegDec_pImage();

		// calculate where the image block should be drawn on the screen
		int mcu_x = JpegDec_MCUx() * mcu_w + xpos;
		int mcu_y = JpegDec_MCUy() * mcu_h + ypos;

		// check if the image block size needs to be changed for the right and bottom edges
		if(mcu_x + mcu_w <= max_x) win_w = mcu_w;
		else win_w = min_w;
		if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
		else win_h = min_h;

		// calculate how many pixels must be drawn
		uint32_t mcu_pixels = win_w * win_h;

		// draw image block if it will fit on the screen
		
		if((mcu_x + win_w) <= LCD_getWidth() && (mcu_y + win_h) <= LCD_getHeight()) {
			// open a window onto the screen to paint the pixels into
			
			//ILI9341_setAddrBlock(mcu_x, mcu_y, mcu_x + win_w - 1, mcu_y + win_h - 1, 0);
			LCD_setAddressWindowToWrite(mcu_x, mcu_y, mcu_x + win_w - 1, mcu_y + win_h - 1);

			ili9341_sendData8((uint8_t*)(pImg), mcu_pixels * 2);

		}

		// stop drawing blocks if the bottom of the screen has been reached
		// the abort function will close the file
		else if((mcu_y + win_h) >= LCD_getWidth()) JpegDec_abort();

	}
	



}

FRESULT slideshow()
{
	BTN_initUL(&btn_exit, 303, 2, 16, 16, WHITE, BLUE, WHITE, "", 8, 8);
	mode = modeFoto;
	FATFS USB_Fs;
	FRESULT res;
	FILINFO fno;
	DIR dir;
	const char* path = "1:";
	unsigned char seek = 0;
	char *fn; /* This function is assuming non-Unicode cfg. */
	char buffer[200];
	
#if _USE_LFN
	static char lfn[_MAX_LFN + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
#endif
	
	LCD_fillScreen(BLACK);
	
	if (TM_USB_MSCHOST_Device() != TM_USB_MSCHOST_Result_Connected)
	{
		while (1) {
			TM_USB_MSCHOST_Process();
			if (TM_USB_MSCHOST_Device() == TM_USB_MSCHOST_Result_Connected) {
				goto USB_OK;
			}
		}
	}
USB_OK: 
	//if (TM_USB_MSCHOST_Device() != TM_USB_MSCHOST_Result_Connected)
	//	return 0;
	res = f_mount(&USB_Fs, "1:", 1);
	if (res != FR_OK)
		return 0;
	
	res = f_opendir(&dir, path); /* Open the directory */
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno); /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0) break; /* Break on error or end of dir */
			if (fno.fname[0] == '.') continue; /* Ignore dot entry */
#if _USE_LFN
			fn = *fno.lfname ? fno.lfname : fno.fname;
#else
			fn = fno.fname;
#endif
			if (fno.fattrib & AM_DIR) {
				/* It is a directory */
        
			}
			else {
				/* It is a file. */
				sprintf(buffer, "%s/%s", path, fn);
				// Skip "seek" number of mp3 files and ignore file with a starting char '.'
				if(seek || !strncasecmp("1:/.", buffer, 4)) {
					//printf("Skip file: %s\r\n", buffer);
					seek--;
					continue;
				}
				//printf("Play file: %s\r\n", buffer);
        
				// Check if it is an mp3 file
				if(strcasecmp("jpg", GetFilenameExt(buffer)) == 0) {
					JpegDec_decodeSdFile(buffer);
					//JpegDec.decodeSdFile(buffer);
					renderJPEG(0, 0);
					if (slideshowMenu() == 11) return res;
					
				}
				else if(strcasecmp("wav", GetFilenameExt(buffer)) == 0) {
					//cwWavePlayFile(buffer);
				}
				else if(strcasecmp("mem", GetFilenameExt(buffer)) == 0) {
					//cwMemoryPlayFile(NULL);
				}
				LCD_fillScreen(BLACK);
			}
		}
	}
	f_closedir(&dir);
	f_mount(0, "", 0);
	return res;
	
	
	//dir.rewindDirectory();
	
	
	
}


	
#pragma GCC pop_options

	
	
	


