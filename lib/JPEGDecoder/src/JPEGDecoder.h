/*
JPEGDecoder.h

JPEG Decoder for Arduino
Public domain, Makoto Kurauchi <http://yushakobo.jp>

Adapted by Bodmer for use with a TFT screen

Latest version here:
https://github.com/Bodmer/JPEGDecoder

*/

#ifndef JPEGDECODER_H
#define JPEGDECODER_H

#include "User_Config.h"
#include "ff.h"	
#include "picojpeg.h"
#include "stdint.h"
#include <string.h>
#include <stddef.h>


enum {
	JPEG_ARRAY = 0,
	JPEG_FS_FILE,
	JPEG_SD_FILE
};



//#define DEBUG

//------------------------------------------------------------------------------
#ifndef jpg_min
	#define jpg_min(a,b)     (((a) < (b)) ? (a) : (b))
#endif

#ifndef min
  #define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

//------------------------------------------------------------------------------
typedef unsigned char uint8;
typedef unsigned int uint;
//------------------------------------------------------------------------------

class JPEGDecoder {

private:
#if defined (LOAD_SD_LIBRARY) || defined (LOAD_SDFAT_LIBRARY)
	FIL g_pInFileSd;
#endif
#ifdef LOAD_SPIFFS
	fs::File g_pInFileFs;
#endif
	pjpeg_scan_type_t scan_type;
	pjpeg_image_info_t image_info;
	
	int is_available;
	int mcu_x;
	int mcu_y;
	uint g_nInFileSize;
	uint g_nInFileOfs;
	uint row_pitch;
	uint decoded_width, decoded_height;
	uint row_blocks_per_mcu, col_blocks_per_mcu;
	uint8 status;
	uint8 jpg_source = 0;
	uint8_t* jpg_data;
	
	static uint8 pjpeg_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);
	uint8 pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);
	int decode_mcu(void);
	int decodeCommon(void);
public:

	uint16_t *pImage;
	JPEGDecoder *thisPtr;

	int width;
	int height;
	int comps;
	int MCUSPerRow;
	int MCUSPerCol;
	pjpeg_scan_type_t scanType;
	int MCUWidth;
	int MCUHeight;
	int MCUx;
	int MCUy;
	
	JPEGDecoder();
	~JPEGDecoder();

	int available(void);
	int read(void);
	int readSwappedBytes(void);
	
	int decodeFile (const char *pFilename);
	//int decodeFile (const String& pFilename);
	
#if defined (LOAD_SD_LIBRARY) || defined (LOAD_SDFAT_LIBRARY)
	int decodeSdFile (const char *pFilename);
	//int decodeSdFile (const str& pFilename);
	int decodeSdFile (FIL g_pInFile);
#endif

#ifdef LOAD_SPIFFS
	int decodeFsFile (const char *pFilename);
	int decodeFsFile (const String& pFilename);
	int decodeFsFile (fs::File g_pInFile);
#endif

	int decodeArray(const uint8_t array[], uint32_t  array_size);
	void abort(void);

};

extern JPEGDecoder JpegDec;

#endif // JPEGDECODER_H
