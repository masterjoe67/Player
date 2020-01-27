
#include "SIM800L_interface.h"
#include "Sim800l.h"

#ifdef __cplusplus
extern "C" {
#endif
	wSim800l*   Sim800l_create() {
		//calling the constructor and casting the pointer to wJPEGDecoder
		return reinterpret_cast<wSim800l*>(new Sim800l());
	}
	
	void  Sim800l_destroy(wSim800l* self) {
		//calling the destructor
		delete reinterpret_cast<Sim800l*>(self);
	}
	bool Sim800l_begin() {
		return _Sim800l.begin();
	}
	
	void Sim800l_onSerialDataReceived(uint8_t c) {_Sim800l.onSerialDataReceived(c); }
	
	//IMEI
	uint8_t Sim800l_getIMEI(char *imei) {return _Sim800l.getIMEI(imei);}
	
	// Battery and ADC
    bool Sim800l_getADCVoltage(uint16_t *v){return _Sim800l.getADCVoltage(v);}
	bool Sim800l_getBattVoltage(uint16_t *v) {return _Sim800l.getBattVoltage(v);}
	bool Sim800l_getBattPercent(uint16_t *p) {return _Sim800l.getBattPercent(p);}
	
	// SIM query
	bool Sim800l_unlockSIM(char *pin) {return _Sim800l.unlockSIM(pin);}
	uint8_t Sim800l_getSIMCCID(char *ccid) {return _Sim800l.getSIMCCID(ccid);}
	uint8_t Sim800l_getNetworkStatus(void) {return _Sim800l.getNetworkStatus();}
	uint8_t Sim800l_getRSSI(void) {return _Sim800l.getRSSI();}
	uint8_t Sim800l_getSIMProvider(char *provider) {return _Sim800l.getSIMProvider(provider);}
	
	// set Audio output
    bool Sim800l_setAudio(uint8_t a) {return _Sim800l.setAudio(a);}
	bool Sim800l_setVolume(uint8_t i) {return _Sim800l.setVolume(i);}
	uint8_t Sim800l_getVolume(void) {return _Sim800l.getVolume();}
	bool Sim800l_playToolkitTone(uint8_t t, uint16_t len) {return _Sim800l.playToolkitTone(t, len);}
	bool Sim800l_setMicVolume(uint8_t a, uint8_t level) {return _Sim800l.setMicVolume(a, level);}
	bool Sim800l_playDTMF(char tone) {return _Sim800l.playDTMF(tone);}
	
	// FM radio functions
    bool Sim800l_tuneFMradio(uint16_t station) {return _Sim800l.tuneFMradio(station);}
	bool Sim800l_FMradio(bool onoff, uint8_t a) {return _Sim800l.FMradio(onoff, a);}
	bool Sim800l_FMradioDefault(bool onoff) {return _Sim800l.FMradio(onoff);}
	bool Sim800l_setFMVolume(uint8_t i) {return _Sim800l.setFMVolume(i);}
	int8_t Sim800l_getFMVolume() {return _Sim800l.getFMVolume();}
	int8_t Sim800l_getFMSignalLevel(uint16_t station) {return _Sim800l.getFMSignalLevel(station);}
	
	// PWM (buzzer)
	bool Sim800l_setPWM(uint16_t period, uint8_t duty) {return _Sim800l.setPWM(period, duty);}
	bool Sim800l_setPWMnoDuty(uint16_t period) {return _Sim800l.setPWM(period);}
	
	// Phone calls
    bool Sim800l_callPhone(char *phonenum) {return _Sim800l.callPhone(phonenum);}
	bool Sim800l_hangUp(void) {return _Sim800l.hangUp();}
	bool Sim800l_pickUp(void) {return _Sim800l.pickUp();}
	bool Sim800l_callerIdNotification(bool enable) {return _Sim800l.callerIdNotification(enable);}
	bool Sim800l_incomingCallNumber(char* phonenum) {return _Sim800l.incomingCallNumber(phonenum);}
	void Sim800l_onIncomingCall() {_Sim800l.onIncomingCall(); }
	uint8_t Sim800l_isCallActive() {return _Sim800l.isCallActive(); }
	uint8_t Sim800l_IsEvent() 
	{
		uint8_t ret = 0;
		if (_Sim800l._incomingCall) ret += 1; 
		if (_Sim800l._noCarrier) ret += 2;
		return ret;
	}
	void Sim800l_clearEvent() 
	{
		_Sim800l._incomingCall = false; 
		_Sim800l._noCarrier = false;
	}
	
	// SMS handling
    bool Sim800l_setSMSInterrupt(uint8_t i) {return _Sim800l.setSMSInterrupt(i); }
	uint8_t Sim800l_getSMSInterrupt(void) {return _Sim800l.getSMSInterrupt(); }
	int8_t Sim800l_getNumSMS(void) {return _Sim800l.getNumSMS(); }
	bool Sim800l_readSMS(uint8_t i, char *smsbuff, uint16_t max, uint16_t *readsize) {return _Sim800l.readSMS(i, smsbuff, max, readsize); }
	bool Sim800l_sendSMS(char *smsaddr, char *smsmsg) {return _Sim800l.sendSMS(smsaddr, smsmsg); }
	bool Sim800l_deleteSMS(uint8_t i) {return _Sim800l.deleteSMS(i); }
	bool Sim800l_getSMSSender(uint8_t i, char *sender, int senderlen) {return _Sim800l.getSMSSender(i, sender, senderlen); }
	
	// Time
	bool Sim800l_enableNetworkTimeSync(bool onoff) {return _Sim800l.enableNetworkTimeSync(onoff); }
	bool Sim800l_enableNTPTimeSync(bool onoff, const char* ntpserver) {return _Sim800l.enableNTPTimeSync(onoff, ntpserver); }
	bool Sim800l_enableNTPTimeSyncNS(bool onoff) {return _Sim800l.enableNTPTimeSync(onoff); }
	bool Sim800l_getTime(char* buff, uint16_t maxlen) {return _Sim800l.getTime(buff, maxlen); }
	
	// GPRS handling
    bool Sim800l_enableGPRS(bool onoff) {return _Sim800l.enableGPRS(onoff); }
	uint8_t Sim800l_GPRSstate(void) {return _Sim800l.GPRSstate(); }
	bool Sim800l_getGSMLoc(uint16_t *replycode, char *buff, uint16_t maxlen) {return _Sim800l.getGSMLoc(replycode, buff, maxlen); }
	bool Sim800l_getGSMLocLatLon(float *lat, float *lon) {return _Sim800l.getGSMLoc(lat, lon); }
	void Sim800l_setGPRSNetworkSettings(const char* apn, const char* username, const char* password) {return _Sim800l.setGPRSNetworkSettings(apn, username, password); }
	
	// RTC
    bool Sim800l_enableRTC(uint8_t i) {return _Sim800l.enableRTC(i); }    // i = 0 <=> disable, i = 1 <=> enable
	
	// TCP raw connections
    bool Sim800l_TCPconnect(char *server, uint16_t port) {return _Sim800l.TCPconnect(server, port); }
	bool Sim800l_TCPclose(void) {return _Sim800l.TCPclose(); }
	bool Sim800l_TCPconnected(void) {return _Sim800l.TCPconnected(); }
	bool Sim800l_TCPsend(char *packet, uint8_t len) {return _Sim800l.TCPsend(packet, len); }
	uint16_t Sim800l_TCPavailable(void) {return _Sim800l.TCPavailable(); }
	uint16_t Sim800l_TCPread(uint8_t *buff, uint8_t len) {return _Sim800l.TCPread(buff, len); }
	
	// HTTP low level interface (maps directly to SIM800 commands).
    bool Sim800l_HTTP_init() {return _Sim800l.HTTP_init(); }
	bool Sim800l_HTTP_term() {return _Sim800l.HTTP_term(); }
	void Sim800l_HTTP_para_start(const char* parameter, bool quoted) {return _Sim800l.HTTP_para_start(parameter, quoted); }
	bool Sim800l_HTTP_para_end(bool quoted) {return _Sim800l.HTTP_para_end(quoted); }
	bool Sim800l_HTTP_para(const char* parameter, const char *value) {return _Sim800l.HTTP_para(parameter, value); }
	bool Sim800l_HTTP_paraI(const char* parameter, int32_t value) {return _Sim800l.HTTP_para(parameter, value); }
	bool Sim800l_HTTP_data(uint32_t size, uint32_t maxTime) {return _Sim800l.HTTP_data(size, maxTime); }
	bool Sim800l_HTTP_action(uint8_t method, uint16_t *status, uint16_t *datalen, int32_t timeout) {return _Sim800l.HTTP_action(method, status, datalen, timeout); }
	bool Sim800l_HTTP_readall(uint16_t *datalen) {return _Sim800l.HTTP_readall(datalen); }
	bool Sim800l_HTTP_ssl(bool onoff) {return _Sim800l.HTTP_ssl(onoff); }
	
	// HTTP high level interface (easier to use, less flexible).
    bool Sim800l_HTTP_GET_start(char *url, uint16_t *status, uint16_t *datalen) {return _Sim800l.HTTP_GET_start(url, status, datalen); }
	void Sim800l_HTTP_GET_end(void) {return _Sim800l.HTTP_GET_end(); }
	bool Sim800l_HTTP_POST_start(char *url, const char* contenttype, const uint8_t *postdata, uint16_t postdatalen, uint16_t *status, uint16_t *datalen) {return _Sim800l.HTTP_POST_start(url, contenttype, postdata, postdatalen, status, datalen); }
	void Sim800l_HTTP_POST_end(void) {return _Sim800l.HTTP_POST_end(); }
	void Sim800l_setUserAgent(const char* useragent) {return _Sim800l.setUserAgent(useragent); }
	
	// HTTPS
    void Sim800l_setHTTPSRedirect(bool onoff);
	
	
#ifdef __cplusplus
}
#endif