/* this library is writing by  Cristian Steib.
 *      steibkhriz@gmail.com
 *  Designed to work with the GSM Sim800l,maybe work with SIM900L
 *  
 *     This library use SoftwareSerial, you can define de RX and TX pin
 *     in the header "Sim800l.h" ,by default the pin is RX=10 TX=11..
 *     be sure that gnd is attached to arduino too. 
 *     You can also change the other preferred RESET_PIN
 *    
 *     Esta libreria usa SoftwareSerial, se pueden cambiar los pines de RX y TX
 *     en el archivo header, "Sim800l.h", por defecto los pines vienen configurado en
 *     RX=10 TX=11.  
 *     Tambien se puede cambiar el RESET_PIN por otro que prefiera
 *     
 *    PINOUT: 
 *        _____________________________
 *       |  ARDUINO UNO >>>   SIM800L  |
 *        -----------------------------
 *            GND      >>>   GND
 *        RX  10       >>>   TX    
 *        TX  11       >>>   RX
 *       RESET 2       >>>   RST 
 *                 
 *   POWER SOURCE 4.2V >>> VCC
 *
 *    Created on: April 20, 2016
 *        Author: Cristian Steib
 *        
 *
*/
#ifndef Sim800l_h
#define Sim800l_h

#include "stm32f4xx.h"
#include "stdbool.h"
#include <stdlib.h>
#include <string>
#include <iostream>

/*#ifdef __cplusplus
extern "C" {
#endif
#define EXTI_IRQHandler &Sim800l::onIncomingCall
	
#ifdef __cplusplus
}
#endif*/

#ifndef prog_char_strcmp
#define prog_char_strcmp(a, b)					strcmp((a), (b))
#endif
	

#define RX_PIN PA3
#define TX_PIN PA2	

#define SIM800L_RST_GPIO_Port GPIOD
#define SIM800L_RST_Pin GPIO_Pin_11

#define SIM800L_RING_GPIO_Port GPIOC
#define SIM800L_RING_Pin GPIO_Pin_6

#define FONA_DEFAULT_TIMEOUT_MS 500
#define FONA_HEADSETAUDIO 0
#define FONA_EXTAUDIO 1

#define FONA_HTTP_GET   0
#define FONA_HTTP_POST  1
#define FONA_HTTP_HEAD  2

#define LED false // used for indicator led, in case that you don want set to false . 
#define LED_PIN 13 //pin to indicate states. 

#define SIM800L_RST_SET      GPIO_SetBits(SIM800L_RST_GPIO_Port,SIM800L_RST_Pin);
#define SIM800L_RST_RESET    GPIO_ResetBits(SIM800L_RST_GPIO_Port,SIM800L_RST_Pin);

#ifndef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif

#define RX_BUFFER_SIZE  255

typedef  std::string String;


class Sim800l		
{				
public:

	
	volatile bool _incomingCall;
	volatile bool _noCarrier;
  private:
	int _timeout;
	String _buffer;
	String _readSerial();
	char replybuffer[255];

	
	
	char* apn;
	char* apnusername;
	char* apnpassword;
	bool httpsredirect;
	char* useragent;
	
	
	
 	void reset(); 
	
	int putc(int value);
	int getc();
	
	
	
	
	uint16_t readRaw(uint16_t b);
	
	uint8_t readline(uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS, bool multiline = false);
	
	int RecvResponse2();
	
	uint8_t getReply(const char* send, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
	uint8_t getReply(const char* prefix, char *suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
	uint8_t getReply(const char* prefix, int32_t suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
	uint8_t getReply(const char* prefix, int32_t suffix1, int32_t suffix2, uint16_t timeout);  // Don't set default value or else function call is ambiguous.
	uint8_t getReplyQuoted(const char* prefix, const char* suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
	
	
	bool sendParseReply(const char* tosend, const char* toreply, uint16_t *v, char divider = ',', uint8_t index = 0);
	
	bool sendCheckReply(const char* send, const char* reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
	bool sendCheckReply(const char* prefix, char *suffix, const char* reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
	bool sendCheckReply(const char* prefix, int32_t suffix, const char* reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
	bool sendCheckReply(const char* prefix, int32_t suffix, int32_t suffix2, const char* reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
	bool sendCheckReplyQuoted(const char* prefix, const char* suffix, const char* reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
	
	bool parseReply(const char* toreply, uint16_t *v, char divider  = ',', uint8_t index = 0);
	bool parseReply(const char* toreply, char *v, char divider  = ',', uint8_t index = 0);
	bool parseReplyQuoted(const char* toreply, char* v, int maxlen, char divider, uint8_t index);
	int readable();
	void flushInput();
	
	// Circular buffer used to receive serial data from an interruption
    int rxBuffer[RX_BUFFER_SIZE + 1];
	volatile int rxBufferInIndex;  // Index where new data is added to the buffer
	volatile int rxBufferOutIndex;  // Index where data is removed from the buffer
	char currentReceivedLine[RX_BUFFER_SIZE];  // Array containing the current received line
	int currentReceivedLineSize;
	
	inline bool isRxBufferFull() {
		return ((rxBufferInIndex + 1) % RX_BUFFER_SIZE) == rxBufferOutIndex;
	}
	inline void incrementRxBufferInIndex() {
		rxBufferInIndex = (rxBufferInIndex + 1) % RX_BUFFER_SIZE;
	}
	
	inline bool isRxBufferEmpty() {
		return rxBufferInIndex == rxBufferOutIndex;
	}
	
	inline void incrementRxBufferOutIndex() {
		rxBufferOutIndex = (rxBufferOutIndex + 1) % RX_BUFFER_SIZE;
	}
	
  public:
	void onSerialDataReceived(uint8_t c);

	bool begin();
	//void setEventListener(EventListener *eventListener);
	
	void Product_Identification_Information(char* ret);
	void setPhoneFunctionality();
	void activateBearerProfile();
	void deactivateBearerProfile();


	//IMEI
	uint8_t getIMEI(char *imei);
	
	// Helper functions to verify responses.
	bool expectReply(const char* reply, uint16_t timeout = 10000);
	
	// HTTP helpers
	bool HTTP_setup(char *url);
	
	// Battery and ADC
    bool getADCVoltage(uint16_t *v);
	bool getBattPercent(uint16_t *p);
	bool getBattVoltage(uint16_t *v);
	
	// SIM query
    bool unlockSIM(char *pin);
	uint8_t getSIMCCID(char *ccid);
	uint8_t getNetworkStatus(void);
	uint8_t getRSSI(void);
	uint8_t getSIMProvider(char *provider);
	
	// set Audio output
    bool setAudio(uint8_t a);
	bool setVolume(uint8_t i);
	uint8_t getVolume(void);
	bool playToolkitTone(uint8_t t, uint16_t len);
	bool setMicVolume(uint8_t a, uint8_t level);
	bool playDTMF(char tone);
	
	// FM radio functions
    bool tuneFMradio(uint16_t station);
	bool FMradio(bool onoff, uint8_t a = FONA_HEADSETAUDIO);
	bool setFMVolume(uint8_t i);
	int8_t getFMVolume();
	int8_t getFMSignalLevel(uint16_t station);
	
	// PWM (buzzer)
    bool setPWM(uint16_t period, uint8_t duty = 50);

	// Phone calls
    bool callPhone(char *phonenum);
	bool hangUp(void);
	bool pickUp(void);
	bool callerIdNotification(bool enable);
	bool incomingCallNumber(char* phonenum);
	void onIncomingCall();
	uint8_t isCallActive();
	
	// SMS handling
    bool setSMSInterrupt(uint8_t i);
	uint8_t getSMSInterrupt(void);
	int8_t getNumSMS(void);
	bool readSMS(uint8_t i, char *smsbuff, uint16_t max, uint16_t *readsize);
	bool sendSMS(char *smsaddr, char *smsmsg);
	bool deleteSMS(uint8_t i);
	bool getSMSSender(uint8_t i, char *sender, int senderlen);
	
	// Time
    bool enableNetworkTimeSync(bool onoff);
	bool enableNTPTimeSync(bool onoff, const char* ntpserver = 0);
	bool getTime(char* buff, uint16_t maxlen);
	
	// GPRS handling
    bool enableGPRS(bool onoff);
	uint8_t GPRSstate(void);
	bool getGSMLoc(uint16_t *replycode, char *buff, uint16_t maxlen);
	bool getGSMLoc(float *lat, float *lon);
	void setGPRSNetworkSettings(const char* apn, const char* username = 0, const char* password = 0);
	
	// RTC
    bool enableRTC(uint8_t i);  // i = 0 <=> disable, i = 1 <=> enable
	void RTCtime(int *day, int *month, int *year, int *hour, int *minute, int *second);  
	bool updateRtc(int utc);
	String dateNet();
	
	// TCP raw connections
    bool TCPconnect(char *server, uint16_t port);
	bool TCPclose(void);
	bool TCPconnected(void);
	bool TCPsend(char *packet, uint8_t len);
	uint16_t TCPavailable(void);
	uint16_t TCPread(uint8_t *buff, uint8_t len);
	
	// HTTP low level interface (maps directly to SIM800 commands).
    bool HTTP_init();
	bool HTTP_term();
	void HTTP_para_start(const char* parameter, bool quoted = true);
	bool HTTP_para_end(bool quoted = true);
	bool HTTP_para(const char* parameter, const char *value);
	bool HTTP_para(const char* parameter, int32_t value);
	bool HTTP_data(uint32_t size, uint32_t maxTime = 10000);
	bool HTTP_action(uint8_t method, uint16_t *status, uint16_t *datalen, int32_t timeout = 10000);
	bool HTTP_readall(uint16_t *datalen);
	bool HTTP_ssl(bool onoff);
	
	// HTTP high level interface (easier to use, less flexible).
    bool HTTP_GET_start(char *url, uint16_t *status, uint16_t *datalen);
	void HTTP_GET_end(void);
	bool HTTP_POST_start(char *url, const char* contenttype, const uint8_t *postdata, uint16_t postdatalen, uint16_t *status, uint16_t *datalen);
	void HTTP_POST_end(void);
	void setUserAgent(const char* useragent);
	
	// HTTPS
    void setHTTPSRedirect(bool onoff);
	
	
	
};
extern Sim800l _Sim800l;

#endif 