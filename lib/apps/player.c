#include "ili9341_text.h"
#include "ili9341_graph.h"
#include "Audio.h"
#include "icons_included.h"
#include "icon.h"
#include "lcdButton.h"
#include "cwMP3.h"
#include "xpt2046.h"
#include "tm_stm32f4_delay.h"
#include "player.h"
#include "tm_stm32f4_fatfs.h"
#include "tm_stm32f4_usb_msc_host.h"
#include "utility.h"
#include "FreeMono9pt7b.h"
#include "FreeMono12pt7b.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")

uint8_t muteStatus = 0;
int vol_cursor_last = -1;
uint32_t lastTouch;
uint16_t currentSong = 0;
uint8_t player_mode = modeMP3sel;
char  nameFiles[128][20];
char name[20];

int filePOS = 0;
int fileIndex = 0;
int listStart = 1;
int maxmidicount;

DIR dir;

BTN_TypeDef btn_exit, btn_mute;
BTN_TypeDef btn_prev, btn_next, btn_play_stop, btn_incvol, btn_decvol, btn_pgup, btn_pgdown;
BTN_Icon_TypeDef but_icon[8];
BTN_Label_TypeDef buttons[12];

// Button position in MP3 list
const int xstartButtonList[] = { 0, 0, 0, 0, 0, 0 };                      // x-min for keypads
const int ystartButtonList[] = { 32, 64, 96, 128, 160, 192 };                // y-min for keypads



int getButtonNumberPlayer(int xInput, int yInput)
{
	
	if (player_mode == modeMP3)
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
	else if (player_mode == modeMP3sel)
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

void list_helper(u16 pos, u32 numFiles)
{
	char filename[20];
	unsigned char *label;
	u8 textsize = 1;
	const GFXfont *font = &FreeMono9pt7b;
	
	LCD_fillRect(0, 32, 296, 207, BLACK);
	
	for (int y = 0; y < numFiles; y++)
	{
		BTN_Label_initUL(&buttons[y], xstartButtonList[y], ystartButtonList[y], 32, 32, WHITE, BLACK, WHITE, "A", 1, font);
		BTN_Label_draw(&buttons[y]);
		BTN_Label_initUL(&buttons[y + 6], 32, ystartButtonList[y], 264, 32, WHITE, BLACK, WHITE, "", 1, font);
		BTN_Label_draw(&buttons[y + 6]);
		
		memcpy(filename, nameFiles[y + 1 + pos], 20);
		label = filename;
		label += 3;
		uint8_t xof = font->glyph->xAdvance;
		uint8_t yof = font->yOffset;
		LCD_setFont(font);
		LCD_setCursor(32 + 152 - (strlen(filename) * xof / 2), ystartButtonList[y] + 16 + yof);
		LCD_setTextColor(GREENYELLOW);
		LCD_setTextSize(textsize);
		LCD_writeString(label);
	
		
	} 
}

void scollBar_helper(u16 pos, u32 numFiles)
{
	if (pos > 0)
		BTN_initUL(&btn_pgup, 297, 32, 22, 32, WHITE, BLACK, WHITE, "A", 2, 2);	
	else
		BTN_initUL(&btn_pgup, 297, 32, 22, 32, WHITE, BLACK, DGRAY, "A", 2, 2);
	if (numFiles > 5 && (maxmidicount - pos) > 6)
		BTN_initUL(&btn_pgdown, 297, 192, 22, 32, WHITE, BLACK, WHITE, "V", 2, 2);		
	else
		BTN_initUL(&btn_pgdown, 297, 192, 22, 32, WHITE, BLACK, DGRAY, "V", 2, 2);
	BTN_draw(&btn_pgup, 0);
	BTN_draw(&btn_pgdown, 0);
}


void btn_play_stop_helper(uint8_t status)
{
	if (status == PLAY_STATUS)
	{
		LCD_drawRGBBitmap(125, 110, (uint16_t *)image_data_pause, 48, 48);
	}
	else
	{
		LCD_drawRGBBitmap(125, 110, (uint16_t *)image_data_play, 48, 48);
		
	}
	
}

void btn_mute_helper()
{
	if (muteStatus == 0)
	{
		LCD_drawRGBBitmap(216, 64, (uint16_t *)image_data_mute, 48, 48);
	}
	else
	{
		LCD_drawRGBBitmap(216, 64, (uint16_t *)image_data_music, 48, 48);
		
	}
	
}

void exit_btn_player_helper()
{
	BTN_initUL(&btn_exit, 303, 2, 16, 16, WHITE, BLUE, WHITE, "", 8, 8);
	LCD_drawRGBBitmap(303, 2, (uint16_t *)image_data_Cancel, 16, 16);
}

void volumeBar_helper(u16 pos)
{
	int y = map(pos, 255, 0, 64, 182);
	if (vol_cursor_last > 0)
		LCD_fillRect(295, vol_cursor_last, 22, 8, BLACK);
	LCD_fillRect(295, y, 22, 8, MAGENTA);
	vol_cursor_last = y;
}

uint8_t playerControl()
{
	int userInput = -1;
	if (TouchPressed())
	{
		getDisplayPoint(&display, Read_Value(), &matrix);
		if (display.x > 0 && display.x<320 && display.y>0 && display.y < 240 && (TM_Time - lastTouch) > 800) 
		{
			lastTouch = TM_Time;
			userInput = getButtonNumberPlayer(display.x, display.y);
			if (userInput == btn_play_ret && player_status == PLAY_STATUS)
			{
				Audio_Pause();
				player_status = PAUSE_STATUS;
				btn_play_stop_helper(player_status);
				return 0;
			}
			else if (userInput == btn_play_ret && player_status == PAUSE_STATUS) {
				
				player_status = PLAY_STATUS;
				btn_play_stop_helper(player_status);
				muteStatus = 0;
				btn_mute_helper();
				Audio_Resume();
				return 0;
			}
			else if (userInput == btn_incvol_ret && player_status == PLAY_STATUS) {
				if (volume < 0xff)
				{
					volume += 10;
					if (volume > 0xff) volume = 0xff;
					SetAudioVolume(volume);
					volumeBar_helper(volume);
				}
				return 0;
			}
			else if (userInput == btn_decvol_ret && player_status == PLAY_STATUS) {
				if (volume > 0x00)
				{
					volume -= 0x0a;
					if (volume < 0) volume = 0x00;
					SetAudioVolume(volume);
					volumeBar_helper(volume);
				}
				return 0;
			}
			else if (userInput == btn_mute_ret && player_status == PLAY_STATUS) {
				if (muteStatus == 0)
				{
					Audio_SetMute(1);
					muteStatus = 1;
					btn_mute_helper();
				}
				else
				{
					Audio_SetMute(0);
					muteStatus = 0;
					btn_mute_helper();
				}
				return 0;
			}
			else if (userInput == btn_prev_ret && currentSong > 1) {
				
				currentSong--;
				return 2;
			}
			else if (userInput == btn_next_ret && currentSong < maxmidicount) {
				
				currentSong++;
				return 3;
			}
			else if (userInput == btn_exit_ret) {
				
				return 1;
			}
		}
	}
	return 0;
}

uint8_t Display_playerMenu(char* filename)
{
	char name[20];
	unsigned char *label;
	const GFXfont *font = &FreeMono12pt7b;
	
	player_mode = modeMP3;
	LCD_fillScreen(BLACK);
	LCD_drawFastHLine(0, 28, 319, MAGENTA);
	LCD_drawFastHLine(0, 29, 319, MAGENTA);
	
	BTN_initUL(&btn_exit, 32, 64, 48, 48, WHITE, BLUE, WHITE, "", 8, 8);	
	BTN_initUL(&btn_play_stop, 127, 87, 48, 48, WHITE, GREEN, WHITE, "", 4, 4);
	BTN_initUL(&btn_mute, 216, 64, 48, 48, WHITE, YELLOW, WHITE, "", 8, 8);
	BTN_initUL(&btn_incvol, 293, 32, 26, 26, MAGENTA, BLACK, WHITE, "", 2, 2);	
	BTN_initUL(&btn_decvol, 293, 198, 26, 26, MAGENTA, BLACK, WHITE, "", 2, 2);
	BTN_initUL(&btn_prev, 32, 154, 48, 48, MAGENTA, BLACK, WHITE, "", 2, 2);	
	BTN_initUL(&btn_next, 216, 154, 48, 48, MAGENTA, BLACK, WHITE, "", 2, 2);

	LCD_drawRGBBitmap(125, 110, (uint16_t *)image_data_pause, 48, 48);
	LCD_drawRGBBitmap(32, 64, (uint16_t *)image_data_circ_menu, 48, 48);
	LCD_drawRGBBitmap(216, 64, (uint16_t *)image_data_mute, 48, 48);
	LCD_drawRGBBitmap(32, 154, (uint16_t *)image_data_prev, 48, 48);
	LCD_drawRGBBitmap(216, 154, (uint16_t *)image_data_next, 48, 48);
	
	
	//  Titolo	
	memcpy(name, filename, 20);
	label = (unsigned char)name;
	label += 3;
	
	LCD_setFont(font);
	LCD_setTextSize(1);
	LCD_setCursor((340 / 2) - (strlen(filename) * font->glyph->xAdvance / 2), font->yAdvance - font->yOffset);
	LCD_setTextColor(RED);
	LCD_writeString((unsigned char*)label);
	
	//  Volume
		LCD_drawFastVLine(293, 56, 143, MAGENTA);
	LCD_drawFastVLine(319, 56, 143, MAGENTA);
	LCD_drawRGBBitmap(292, 33, (uint16_t *)image_data_audio100, 24, 24);
	LCD_drawRGBBitmap(292, 199, (uint16_t *)image_data_audio0, 24, 24);
	volumeBar_helper(volume);
	muteStatus = 0;
	
	
	return cwMP3PlayFile(filename);
	
}

void Display_list_of_songs() {
	u16 numFiles;
	u32 minFile = 0;
	FATFS USB_Fs;
	FRESULT res;
	FILINFO fno;
	//char label[20];
	const char* path = "1:";
	//FIL pInFile;
	char filename[20];
	//uint8_t result = 0;
	
	
#if _USE_LFN
	static char lfn[_MAX_LFN + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
#endif
redraw:	
	player_mode = modeMP3sel;
	res = f_mount(&USB_Fs, "1:", 1);
	if (res != FR_OK)
		return;
	res = f_opendir(&dir, path); /* Open the directory */
	
	if ((maxmidicount - minFile) < 6) 
		numFiles = maxmidicount - minFile;
	else
		numFiles = 6;
	

	LCD_fillScreen(BLACK);
	
	LCD_drawFastHLine(0, 28, 319, YELLOW);
	LCD_drawFastHLine(0, 29, 319, YELLOW);
	
	
	
	list_helper(minFile, numFiles);
	
	
	LCD_drawRoundRect(297, 32, 22, 192, 8, WHITE);
		
	scollBar_helper(minFile, numFiles);
	exit_btn_player_helper();
		
	/* Infinite loop */
	for (;;)
	{

		if (TouchPressed())
		{
			getDisplayPoint(&display, Read_Value(), &matrix);

			
			
			int userInput = -1; 
			userInput = getButtonNumberPlayer(display.x, display.y);
			if (userInput >= 0)
			{
				switch (userInput)
				{
				case btn_exit_ret:
					if ((TM_Time - lastTouch) > 1000)
						return;
					lastTouch = TM_Time;
					break;
				case btn_pgup_ret:
					if (minFile > 0)
					{
						minFile--;
						numFiles = maxmidicount - minFile;
						if (numFiles > 6) numFiles = 6;
						list_helper(minFile, numFiles);
						scollBar_helper(minFile, numFiles);
					}
					break;
				case btn_pgdown_ret:
					if (minFile < maxmidicount && (maxmidicount - minFile) > 6)
					{
						minFile++;
						numFiles = maxmidicount - minFile;
						if (numFiles > 6) numFiles = 6;
						list_helper(minFile, numFiles);
						scollBar_helper(minFile, numFiles);
					}
					break;
				case btn_song_ret_1:
				case btn_song_ret_2:
				case btn_song_ret_3:
				case btn_song_ret_4:
				case btn_song_ret_5:
				case btn_song_ret_6:
					currentSong = (userInput & 0x0f) + 1 + minFile;
					memcpy(filename, nameFiles[currentSong], 20);

					
play_next_prev:					
					res = Display_playerMenu(filename);
					if (res == ret_toList)
						goto redraw;
					if (res == ret_prev || res == ret_next)
					{
						memcpy(filename, nameFiles[currentSong], 20);
						goto play_next_prev;
					}
				
					break;
				}
			}
		}
		Delayms(5);
	}
}

FRESULT IndexFileNames() {
	FATFS USB_Fs;
	FRESULT res;
	FILINFO fno;
	
	const char* path = "1:";
	unsigned char seek = 0;
	char *fn; /* This function is assuming non-Unicode cfg. */
	char buffer[200];
	
#if _USE_LFN
	static char lfn[_MAX_LFN + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
#endif
	
	res = f_mount(&USB_Fs, "1:", 1);
	if (res != FR_OK)
		return 0;
	
	//SD.vwd()->rewind();
	res = f_opendir(&dir, path); /* Open the directory */
	
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
			if(strcasecmp("mp3", GetFilenameExt(buffer)) == 0) {
				fileIndex++;          
				for (int i = 0; i < 20; i++) {

					if (buffer[i] != 0x00) {
						nameFiles[fileIndex][i] = buffer[i];

					}
				}  		
			}
			
		}
	}
	maxmidicount = fileIndex;
	return 0;
}
	
FRESULT mediaplayer() {
	
	
	if (!searchUSB()) return 1;
	IndexFileNames();
	
	Display_list_of_songs();
	
	
	return 0;
}

#pragma GCC pop_options
