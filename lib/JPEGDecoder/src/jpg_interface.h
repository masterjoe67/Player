#pragma once


#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
	
	/* Wrapper struct to hold a pointer to
	   Rectangle object in C  */
	struct      wJPEGDecoder;             
	typedef     struct wJPEGDecoder wJPEGDecoder;
	
	/* Wrapper for the constructor */
	wJPEGDecoder*   JPEGDecoder_create();     
	/* Wrapper for destructor */
	void  JPEGDecoder_destroy(wJPEGDecoder* self);
	
	/* Wrapper for method area */
	int JpegDec_decodeSdFile(const char *pFilename);
	
	int JpegDec_read();
	void JpegDec_abort();
	
	int JpegDec_MCUWidth();
	int JpegDec_MCUHeight();
	int JpegDec_width();
	int JpegDec_height();
	int JpegDec_MCUx();
	int JpegDec_MCUy();
	uint16_t * JpegDec_pImage();
	
#ifdef __cplusplus
}
#endif	