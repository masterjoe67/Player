#pragma once
#ifdef __cplusplus


extern "C" {
#endif
	
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


	struct wSim800l;
	typedef     struct wSim800l wSim800l;
	/* Wrapper for the constructor */
	wSim800l*   Sim800l_create();     
	/* Wrapper for destructor */
	void  Sim800l_destroy(wSim800l* self);
	
	bool Sim800l_begin();	
	
	void Sim800l_onSerialDataReceived(uint8_t c);

	//get time with the variables by reference
	void Sim800l_RTCtime(int *day, int *month, int *year, int *hour, int *minute, int *second);  
	void Sim800l_dateNet(char* datetime);   //return date,time, of the network
	bool Sim800l_updateRtc(int utc);    //Update the RTC Clock with de Time AND Date of red-.
	

	
	////////////////////////////////////////////////////////////////////////////////////////
	// IMEI
	uint8_t Sim800l_getIMEI(char *imei);
	
	
	// Battery and ADC
    bool Sim800l_getADCVoltage(uint16_t *v);
	bool Sim800l_getBattPercent(uint16_t *p);
	bool Sim800l_getBattVoltage(uint16_t *v);
	
	// SIM query
	bool Sim800l_unlockSIM(char *pin);
	uint8_t Sim800l_getSIMCCID(char *ccid);
	uint8_t Sim800l_getNetworkStatus(void);
	uint8_t Sim800l_getRSSI(void);
	
	// set Audio output
    bool Sim800l_setAudio(uint8_t a);
	bool Sim800l_setVolume(uint8_t i);
	uint8_t Sim800l_getVolume(void);
	bool Sim800l_playToolkitTone(uint8_t t, uint16_t len);
	bool Sim800l_setMicVolume(uint8_t a, uint8_t level);
	bool Sim800l_playDTMF(char tone);
	
	// FM radio functions
    bool Sim800l_tuneFMradio(uint16_t station);
	bool Sim800l_FMradio(bool onoff, uint8_t a);
	bool Sim800l_FMradioDefault(bool onoff);
	bool Sim800l_setFMVolume(uint8_t i);
	int8_t Sim800l_getFMVolume();
	int8_t Sim800l_getFMSignalLevel(uint16_t station);
	
	// PWM (buzzer)
    bool Sim800l_setPWM(uint16_t period, uint8_t duty);
	bool Sim800l_setPWMnoDuty(uint16_t period);

	// Phone calls
    bool Sim800l_callPhone(char *phonenum);
	bool Sim800l_hangUp(void);
	bool Sim800l_pickUp(void);
	bool Sim800l_callerIdNotification(bool enable);
	bool Sim800l_incomingCallNumber(char* phonenum);
	void Sim800l_onIncomingCall();
	
	// SMS handling
    bool Sim800l_setSMSInterrupt(uint8_t i);
	uint8_t Sim800l_getSMSInterrupt(void);
	int8_t Sim800l_getNumSMS(void);
	bool Sim800l_readSMS(uint8_t i, char *smsbuff, uint16_t max, uint16_t *readsize);
	bool Sim800l_sendSMS(char *smsaddr, char *smsmsg);
	bool Sim800l_deleteSMS(uint8_t i);
	bool Sim800l_getSMSSender(uint8_t i, char *sender, int senderlen);
	
	// Time
	bool Sim800l_enableNetworkTimeSync(bool onoff);
	bool Sim800l_enableNTPTimeSync(bool onoff, const char* ntpserver);
	bool Sim800l_enableNTPTimeSyncNS(bool onoff);
	bool Sim800l_getTime(char* buff, uint16_t maxlen);
	
	// GPRS handling
    bool Sim800l_enableGPRS(bool onoff);
	uint8_t Sim800l_GPRSstate(void);
	bool Sim800l_getGSMLoc(uint16_t *replycode, char *buff, uint16_t maxlen);
	bool Sim800l_getGSMLocLatLon(float *lat, float *lon);
	void Sim800l_setGPRSNetworkSettings(const char* apn, const char* username, const char* password);
	
	// RTC
    bool Sim800l_enableRTC(uint8_t i);    // i = 0 <=> disable, i = 1 <=> enable
	
	// TCP raw connections
    bool Sim800l_TCPconnect(char *server, uint16_t port);
	bool Sim800l_TCPclose(void);
	bool Sim800l_TCPconnected(void);
	bool Sim800l_TCPsend(char *packet, uint8_t len);
	uint16_t Sim800l_TCPavailable(void);
	uint16_t Sim800l_TCPread(uint8_t *buff, uint8_t len);
	
	// HTTP low level interface (maps directly to SIM800 commands).
    bool Sim800l_HTTP_init();
	bool Sim800l_HTTP_term();
	void Sim800l_HTTP_para_start(const char* parameter, bool quoted);
	bool Sim800l_HTTP_para_end(bool quoted);
	bool Sim800l_HTTP_para(const char* parameter, const char *value);
	bool Sim800l_HTTP_paraI(const char* parameter, int32_t value);
	bool Sim800l_HTTP_data(uint32_t size, uint32_t maxTime);
	bool Sim800l_HTTP_action(uint8_t method, uint16_t *status, uint16_t *datalen, int32_t timeout);
	bool Sim800l_HTTP_readall(uint16_t *datalen);
	bool Sim800l_HTTP_ssl(bool onoff);
	
	// HTTP high level interface (easier to use, less flexible).
    bool Sim800l_HTTP_GET_start(char *url, uint16_t *status, uint16_t *datalen);
	void Sim800l_HTTP_GET_end(void);
	bool Sim800l_HTTP_POST_start(char *url, const char* contenttype, const uint8_t *postdata, uint16_t postdatalen, uint16_t *status, uint16_t *datalen);
	void Sim800l_HTTP_POST_end(void);
	void Sim800l_setUserAgent(const char* useragent);
	
	// HTTPS
    void Sim800l_setHTTPSRedirect(bool onoff);
	
	
	
#ifdef __cplusplus
}
#endif	
