//
//  cwMP3.h
// 
//
//

#ifndef __CW_MP3__
#define __CW_MP3__

#include <stdio.h>
#include <stdint.h>
#include "Audio.h"
#include "ff.h"

#define PAUSE_STATUS		((uint32_t)0x00)
#define PLAY_STATUS			((uint32_t)0x01)

uint8_t player_status;

uint8_t cwMP3PlayFile(char* filename);
void cwMP3AudioCallback(void *context,int buffer);
uint32_t Mp3ReadId3V2Tag(FIL* pInFile, char* pszArtist, uint32_t unArtistSize, char* pszTitle, uint32_t unTitleSize);

#endif /* defined(__CW_MP3__) */
