#include "JPEGDecoder.h"
#include "jpg_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

	wJPEGDecoder*   JPEGDecoder_create() {
		//calling the constructor and casting the pointer to WRectangle
		return reinterpret_cast<wJPEGDecoder*>(new JPEGDecoder());
	}
	
	void  JPEGDecoder_destroy(wJPEGDecoder* self) {
		//calling the destructor
		delete reinterpret_cast<JPEGDecoder*>(self);
	}
	int JpegDec_decodeSdFile(const char *pFilename) {
		return JpegDec.decodeSdFile(pFilename);
		//return reinterpret_cast<JPEGDecoder*>(*pFilename)->decodeSdFile(pFilename);
	}
	int JpegDec_read(){
		return JpegDec.read();
		//return reinterpret_cast<JPEGDecoder*>()->read();
	}
	void JpegDec_abort(){
		JpegDec.abort();
		//return reinterpret_cast<JPEGDecoder*>()->abort();
	}
	
	
	int JpegDec_MCUWidth() {return JpegDec.MCUWidth;}
	int JpegDec_MCUHeight() { return JpegDec.MCUHeight;}
	int JpegDec_width(){ return JpegDec.width;}
	int JpegDec_height(){ return JpegDec.height;}
	int JpegDec_MCUx(){ return JpegDec.MCUx;}
	int JpegDec_MCUy(){ return JpegDec.MCUy;}
	
	uint16_t * JpegDec_pImage(){return JpegDec.pImage;}

#ifdef __cplusplus
}
#endif