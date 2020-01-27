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
#include <stdlib.h>
#include <string>
#include <iostream>
#include "Sim800l.h"

extern "C" {
#include "tm_stm32f4_usart.h"
#include "tm_stm32f4_delay.h"
#include "ili9341_text.h"	
}

#define WAITTIME 2000000

Sim800l _Sim800l;
bool NotificationEnabled;

/********* Stream ********************************************/

int Sim800l::putc(int value) {
	TM_USART_Putc(USART2, (char)value);
	return 1;
}

int Sim800l::getc() {
	__disable_irq();  // Start Critical Section - don't interrupt while changing global buffer variables
    
	// Wait for data if the buffer is empty
	if(isRxBufferEmpty()) {
		__enable_irq();  // End Critical Section - need to allow rx interrupt to get new characters for buffer
        
		while(isRxBufferEmpty());
        
		__disable_irq();  // Start Critical Section - don't interrupt while changing global buffer variables
	}
    
	int data = rxBuffer[rxBufferOutIndex];
	incrementRxBufferOutIndex();
    
	__enable_irq();  // End Critical Section
    
	return data;
}

/********* HELPERS *********************************************/

bool Sim800l::expectReply(const char* reply, uint16_t timeout) {
	readline(timeout);

	//DEBUG_PRINT(F("\t<--- ")); DEBUG_PRINTLN(replybuffer);

	return (prog_char_strcmp(replybuffer, (char*)reply) == 0);
}

int Sim800l::RecvResponse2()
{
	int wait = WAITTIME;
	int recv = 0;
	char c;

	while (wait--) {
		if ((c = TM_USART_Getc(USART2)) != 0) {
			if (c == '\r') {

			}
			else {
				replybuffer[recv++] = c;
			}
		}
	}

	replybuffer[recv] = '\0';
	return recv;
}

/*void Sim800l::setEventListener(EventListener *eventListener) {
	this->eventListener = eventListener;
}*/

void Sim800l::onSerialDataReceived(uint8_t c) {
	while (USART_GetITStatus(USART2, USART_IT_RXNE) && !isRxBufferFull()) {
		int data = USART2->DR;
		rxBuffer[rxBufferInIndex] = data;
        
		//
		// Analyze the received data in order to detect events like RING or NO CARRIER
		//
        
		// Copy the data in the current line
		if(currentReceivedLineSize < RX_BUFFER_SIZE && data != '\r' && data != '\n') {
			currentReceivedLine[currentReceivedLineSize] = (char) data;
			currentReceivedLineSize++;
		}
        
		// Check if the line is complete
		if(data == '\n') {
			currentReceivedLine[currentReceivedLineSize] = 0;
            
				// Check if we have a special event
				if(strcmp(currentReceivedLine, "RING") == 0) {
					_incomingCall = true;
					
				} else if(strcmp(currentReceivedLine, "NO CARRIER") == 0) {
					_noCarrier = true;
				}
			
            
			currentReceivedLineSize = 0;
		}
        
		incrementRxBufferInIndex();
	}
}

uint16_t Sim800l::readRaw(uint16_t b) {
	uint16_t idx = 0;
    
	while (b && (idx < sizeof(replybuffer) - 1)) {
		if (readable()) {
			replybuffer[idx] = getc();
			idx++;
			b--;
		}
	}
	replybuffer[idx] = 0;
    
	return idx;
}

uint8_t Sim800l::readline(uint16_t timeout, bool multiline) {
	uint16_t replyidx = 0;
    
	while (timeout--) {
		if (replyidx >= 254) {
			break;
		}
    
		while (readable()) {
			char c =  getc();
			if (c == '\r') continue;
			if (c == 0xA) {
				if (replyidx == 0)   // the first 0x0A is ignored
				    continue;
                
				if (!multiline) {
					timeout = 0;          // the second 0x0A is the end of the line
					break;
				}
			}
			replybuffer[replyidx] = c;
			replyidx++;
		}
    
		if (timeout == 0) {
			break;
		}
		Delayms(1);
	}
	replybuffer[replyidx] = 0;   // null term
	return replyidx;
}

int Sim800l::readable() {
	return !isRxBufferEmpty();
}

void Sim800l::flushInput() {
	// Read all available serial input to flush pending data.
    uint16_t timeoutloop = 0;
	while (timeoutloop++ < 40) {
		while (readable()) {
			getc();
			timeoutloop = 0;   // If char was received reset the timer
		}
		Delayms(1);
	}
}

bool Sim800l::begin(){
	TM_USART_Init(USART2, TM_USART_PinsPack_1, 115200);
	GPIO_InitTypeDef gpioStructure;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	// GPIO speed by default
	gpioStructure.GPIO_Speed = GPIO_Speed_100MHz;

	// GPIO for RESET
	gpioStructure.GPIO_Pin = SIM800L_RST_Pin;
	gpioStructure.GPIO_Mode = GPIO_Mode_OUT;
	gpioStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(SIM800L_RST_GPIO_Port, &gpioStructure);
	
	/* GPIO for  IRQ */
	gpioStructure.GPIO_Mode = GPIO_Mode_IN;
	gpioStructure.GPIO_PuPd = GPIO_PuPd_UP;
	gpioStructure.GPIO_Pin = SIM800L_RING_Pin;
	GPIO_Init(SIM800L_RING_GPIO_Port, &gpioStructure);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource6);
	EXTI_InitStruct.EXTI_Line = EXTI_Line6;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 10;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 6;
	
	NVIC_Init(&NVIC_InitStruct);
	
	
	SIM800L_RST_SET;
	Delayms(10);
	SIM800L_RST_RESET;
	Delayms(100);
	
	while (readable()) getc();
    
	sendCheckReply("AT", "OK");
	Delayms(100);
	sendCheckReply("AT", "OK");
	Delayms(100);
	sendCheckReply("AT", "OK");
	Delayms(100);
	
    _noCarrier = false;
	_incomingCall = false;
	
	// turn off Echo!
	sendCheckReply("ATE0", "OK");
	Delayms(100);
	
    
	if (!sendCheckReply("ATE0", "OK")) {
		return false;
	}
	
	
	return true;
}

bool Sim800l::sendParseReply(const char* tosend, const char* toreply, uint16_t *v, char divider, uint8_t index) {
	getReply(tosend);
    
	if (!parseReply(toreply, v, divider, index)) return false;
    
	readline();  // eat 'OK'
    
	return true;
}

bool Sim800l::parseReply(const char* toreply, uint16_t *v, char divider, uint8_t index) {
	char *p = strstr(replybuffer, toreply);   // get the pointer to the voltage
	if(p == 0) return false;
	p += strlen(toreply);
    
	for (uint8_t i = 0; i < index; i++) {
		// increment dividers
		p = strchr(p, divider);
		if (!p) return false;
		p++;
	}
    
	*v = atoi(p);
    
	return true;
}

bool Sim800l::parseReply(const char* toreply, char *v, char divider, uint8_t index) {
	uint8_t i = 0;
	char *p = strstr(replybuffer, toreply);
	if (p == 0) return false;
	p += strlen(toreply);
    
	for (i = 0; i < index; i++) {
		// increment dividers
		p = strchr(p, divider);
		if (!p) return false;
		p++;
	}
    
	for (i = 0; i < strlen(p); i++) {
		if (p[i] == divider)
			break;
		v[i] = p[i];
	}
    
	v[i] = '\0';
    
	return true;
}

// Parse a quoted string in the response fields and copy its value (without quotes)
// to the specified character array (v).  Only up to maxlen characters are copied
// into the result buffer, so make sure to pass a large enough buffer to handle the
// response.
bool Sim800l::parseReplyQuoted(const char* toreply, char* v, int maxlen, char divider, uint8_t index) {
	uint8_t i = 0, j;
	// Verify response starts with toreply.
	char *p = strstr(replybuffer, toreply);
	if (p == 0) return false;
	p += strlen(toreply);
    
	// Find location of desired response field.
	for(i = 0 ; i < index ; i++) {
		// increment dividers
		p = strchr(p, divider);
		if (!p) return false;
		p++;
	}
    
	// Copy characters from response field into result string.
	for(i = 0, j = 0 ; j < maxlen && i < strlen(p) ; ++i) {
		// Stop if a divier is found.
		if(p[i] == divider)
		    break;
		// Skip any quotation marks.
		else if(p[i] == '"')
		    continue;
		v[j++] = p[i];
	}
    
	// Add a null terminator if result string buffer was not filled.
	if(j < maxlen)
	    v[j] = '\0';
    
	return true;
}

bool Sim800l::sendCheckReply(const char *send, const char *reply, uint16_t timeout) {
	getReply(send, timeout);
    
	return (strcmp(replybuffer, reply) == 0);
}

// Send prefix, suffix, and newline.  Verify FONA response matches reply parameter.
bool Sim800l::sendCheckReply(const char* prefix, char *suffix, const char* reply, uint16_t timeout) {
	getReply(prefix, suffix, timeout);
	return (strcmp(replybuffer, reply) == 0);
}

// Send prefix, suffix, and newline.  Verify FONA response matches reply parameter.
bool Sim800l::sendCheckReply(const char* prefix, int32_t suffix, const char* reply, uint16_t timeout) {
	getReply(prefix, suffix, timeout);
	return (strcmp(replybuffer, reply) == 0);
}

// Send prefix, suffix, suffix2, and newline.  Verify FONA response matches reply parameter.
bool Sim800l::sendCheckReply(const char* prefix, int32_t suffix1, int32_t suffix2, const char* reply, uint16_t timeout) {
	getReply(prefix, suffix1, suffix2, timeout);
	return (strcmp(replybuffer, reply) == 0);
}

uint8_t Sim800l::getReply(const char* send, uint16_t timeout) {
	flushInput();

#ifdef ADAFRUIT_FONA_DEBUG
	printf("\t---> %s\r\n", send);
#endif
	char str[32];
	sprintf(str, "%s\r\n", send);
	
	TM_USART_Puts(USART2, str);
	


	uint8_t l = readline(timeout);
#ifdef ADAFRUIT_FONA_DEBUG
	printf("\t<--- %s\r\n", replybuffer);
#endif
	return l;
}

// Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
uint8_t Sim800l::getReply(const char* prefix, char* suffix, uint16_t timeout) {
	flushInput();
    
#ifdef ADAFRUIT_FONA_DEBUG
	printf("\t---> %s%s\r\n", prefix, suffix);
#endif
	
	char str[32];
	sprintf(str, "%s%s\r\n", prefix, suffix);
	
	TM_USART_Puts(USART2, str);
    
    
	uint8_t l = readline(timeout);
#ifdef ADAFRUIT_FONA_DEBUG
	printf("\t<--- %s\r\n", replybuffer);
#endif
	return l;
}

// Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
uint8_t Sim800l::getReply(const char* prefix, int32_t suffix, uint16_t timeout) {
	flushInput();
    
#ifdef ADAFRUIT_FONA_DEBUG
	printf("\t---> %s%d\r\n", prefix, suffix);
#endif
	
	char str[32];
	sprintf(str, "%s%d\r\n", prefix, suffix);
	
	TM_USART_Puts(USART2, str);
    
    
	uint8_t l = readline(timeout);
#ifdef ADAFRUIT_FONA_DEBUG
	printf("\t<--- %s\r\n", replybuffer);
#endif
	return l;
}

// Send prefix, suffix, suffix2, and newline. Return response (and also set replybuffer with response).
uint8_t Sim800l::getReply(const char* prefix, int32_t suffix1, int32_t suffix2, uint16_t timeout) {
	flushInput();
    
#ifdef ADAFRUIT_FONA_DEBUG
	printf("\t---> %s%d,%d\r\n", prefix, suffix1, suffix2);
#endif
	char str[32];
	sprintf(str, "%s%d,%d\r\n", prefix, suffix1, suffix2);
	
	TM_USART_Puts(USART2, str);
	
    
	uint8_t l = readline(timeout);
#ifdef ADAFRUIT_FONA_DEBUG
	printf("\t<--- %s\r\n", replybuffer);
#endif
	return l;
}

// Send prefix, ", suffix, ", and newline. Return response (and also set replybuffer with response).
uint8_t Sim800l::getReplyQuoted(const char* prefix, const char* suffix, uint16_t timeout) {
	flushInput();
    
#ifdef ADAFRUIT_FONA_DEBUG
	printf("\t---> %s\"%s\"\r\n", prefix, suffix);
#endif
	char str[32];
	sprintf(str, "%s\"%s\"\r\n", prefix, suffix);
	
	TM_USART_Puts(USART2, str);
	
    
	uint8_t l = readline(timeout);
#ifdef ADAFRUIT_FONA_DEBUG
	printf("\t<--- %s\r\n", replybuffer);
#endif
	return l;
}

// Send prefix, ", suffix, ", and newline.  Verify FONA response matches reply parameter.
bool Sim800l::sendCheckReplyQuoted(const char* prefix, const char* suffix, const char* reply, uint16_t timeout) {
	getReplyQuoted(prefix, suffix, timeout);
	return (strcmp(replybuffer, reply) == 0);
}

/********* BATTERY & ADC ********************************************/

/* returns value in mV (uint16_t) */
bool Sim800l::getBattVoltage(uint16_t *v) {
	return sendParseReply("AT+CBC", "+CBC: ", v, ',', 2);
}

/* returns the percentage charge of battery as reported by sim800 */
bool Sim800l::getBattPercent(uint16_t *p) {
	return sendParseReply("AT+CBC", "+CBC: ", p, ',', 1);
}

bool Sim800l::getADCVoltage(uint16_t *v) {
	return sendParseReply("AT+CADC?", "+CADC: 1,", v);
}

/********* Real Time Clock ********************************************/

bool Sim800l::enableRTC(uint8_t i) {
	if (!sendCheckReply("AT+CLTS=", i, "OK")) 
		return false;
	return sendCheckReply("AT&W", "OK");
}

/********* SIM ***********************************************************/

bool Sim800l::unlockSIM(char *pin)
{
	char sendbuff[14] = "AT+CPIN=";
	sendbuff[8] = pin[0];
	sendbuff[9] = pin[1];
	sendbuff[10] = pin[2];
	sendbuff[11] = pin[3];
	sendbuff[12] = NULL;
    
	return sendCheckReply(sendbuff, "OK");
}

uint8_t Sim800l::getSIMProvider(char *provider) {
	getReply("AT+CSPN?"); //CSPN?
	// up to 20 chars
	strncpy(provider, replybuffer, 20);
	provider[20] = 0;
    
	readline();  // eat 'OK'
    
	return strlen(provider);
}

uint8_t Sim800l::getSIMCCID(char *ccid) {
	getReply("AT+CCID");
	// up to 20 chars
	strncpy(ccid, replybuffer, 20);
	ccid[20] = 0;
    
	readline();   // eat 'OK'
    
	return strlen(ccid);
}



/********* IMEI **********************************************************/

uint8_t Sim800l::getIMEI(char *imei) {
	getReply("AT+GSN");
	// up to 15 chars  
	strncpy(imei, replybuffer, 15);
	imei[15] = 0;
    
	readline();  // eat 'OK'
    
	return strlen(imei);
}

/********* NETWORK *******************************************************/

uint8_t Sim800l::getNetworkStatus(void) {
	uint16_t status;
    
	if (!sendParseReply("AT+CREG?", "+CREG: ", &status, ',', 1)) return 0;
    
	return status;
}


uint8_t Sim800l::getRSSI(void) {
	uint16_t reply;
    
	if (!sendParseReply("AT+CSQ", "+CSQ: ", &reply)) return 0;
    
	return reply;
}

/********* AUDIO *******************************************************/

bool Sim800l::setAudio(uint8_t a) {
	// 0 is headset, 1 is external audio
	if(a > 1) return false;
    
	return sendCheckReply("AT+CHFA=", a, "OK");
}

uint8_t Sim800l::getVolume(void) {
	uint16_t reply;
    
	if (!sendParseReply("AT+CLVL?", "+CLVL: ", &reply)) return 0;
    
	return reply;
}

bool Sim800l::setVolume(uint8_t i) {
	return sendCheckReply("AT+CLVL=", i, "OK");
}


bool Sim800l::playDTMF(char dtmf) {
	char str[4];
	str[0] = '\"';
	str[1] = dtmf;
	str[2] = '\"';
	str[3] = 0;
	return sendCheckReply("AT+CLDTMF=3,", str, "OK");
}

bool Sim800l::playToolkitTone(uint8_t t, uint16_t len) {
	return sendCheckReply("AT+STTONE=1,", t, len, "OK");
}

bool Sim800l::setMicVolume(uint8_t a, uint8_t level) {
	// 0 is headset, 1 is external audio
	if(a > 1) return false;
    
	return sendCheckReply("AT+CMIC=", a, level, "OK");
}

/********* FM RADIO *******************************************************/


bool Sim800l::FMradio(bool onoff, uint8_t a) {
	if (!onoff) {
		return sendCheckReply("AT+FMCLOSE", "OK");
	}
    
	// 0 is headset, 1 is external audio
	if(a > 1) return false;
    
	return sendCheckReply("AT+FMOPEN=", a, "OK");
}

bool Sim800l::tuneFMradio(uint16_t station) {
	// Fail if FM station is outside allowed range.
	if((station < 870) || (station > 1090))
	    return false;
    
	return sendCheckReply("AT+FMFREQ=", station, "OK");
}

bool Sim800l::setFMVolume(uint8_t i) {
	// Fail if volume is outside allowed range (0-6).
	if(i > 6) {
		return false;
	}
	// Send FM volume command and verify response.
	return sendCheckReply("AT+FMVOLUME=", i, "OK");
}

int8_t Sim800l::getFMVolume() {
	uint16_t level;
    
	if (!sendParseReply("AT+FMVOLUME?", "+FMVOLUME: ", &level)) return 0;
    
	return level;
}

int8_t Sim800l::getFMSignalLevel(uint16_t station) {
	// Fail if FM station is outside allowed range.
	if((station < 875) || (station > 1080)) {
		return -1;
	}   
    
	// Send FM signal level query command.
	// Note, need to explicitly send timeout so right overload is chosen.
	getReply("AT+FMSIGNAL=", station, FONA_DEFAULT_TIMEOUT_MS);
	// Check response starts with expected value.
	char *p = strstr(replybuffer, "+FMSIGNAL: ");
	if (p == 0) return -1;
	p += 11;
	// Find second colon to get start of signal quality.
	p = strchr(p, ':');
	if (p == 0) return -1;
	p += 1;
	// Parse signal quality.
	int8_t level = atoi(p);
	readline();   // eat the "OK"
	return level;
}

/********* PWM/BUZZER **************************************************/

bool Sim800l::setPWM(uint16_t period, uint8_t duty) {
	if (period > 2000) return false;
	if (duty > 100) return false;
    
	return sendCheckReply("AT+SPWM=0,", period, duty, "OK");
}

/********* CALL PHONES **************************************************/
bool Sim800l::callPhone(char *number) {
	char sendbuff[35] = "ATD";
	strncpy(sendbuff + 3, number, MIN((int)30, (int)strlen(number)));
	uint8_t x = strlen(sendbuff);
	sendbuff[x] = ';';
	sendbuff[x + 1] = 0;
    
	return sendCheckReply(sendbuff, "OK");
}

bool Sim800l::hangUp(void) {
	return sendCheckReply("ATH", "OK");
}

bool Sim800l::pickUp(void) {
	return sendCheckReply("ATA", "OK");
}

uint8_t Sim800l::isCallActive()
{
	uint16_t reply;
    
	if (!sendParseReply("AT+CPAS", "+CPAS: ", &reply)) return 2;
    
	/*Result code:
	    0: ready
	    2: unknown
	    3: ringing
	    4: call in progress*/
    

    
	return reply;


}

void Sim800l::onIncomingCall() {
	if (NotificationEnabled)
		_incomingCall = true;
}

bool Sim800l::callerIdNotification(bool enable) {
	if (enable) {
		NotificationEnabled = true;
		return sendCheckReply("AT+CLIP=1", "OK");
	}
    
	NotificationEnabled = false;
	return sendCheckReply("AT+CLIP=0", "OK");
}

bool Sim800l::incomingCallNumber(char* phonenum) {
	//+CLIP: "<incoming phone number>",145,"",0,"",0
	//if(!_incomingCall)
	//    return false;
    
	readline();
	while (!strcmp(replybuffer, "RING") == 0) {
		flushInput();
		readline();
	}
    
	readline();  //reads incoming phone number line
    
	parseReply("+CLIP: \"", phonenum, '"');
    
#ifdef ADAFRUIT_FONA_DEBUG
	printf("Phone Number: %s\r\n", replybuffer);
#endif
    
	_incomingCall = false;
	return true;
}

/********* SMS **********************************************************/

uint8_t Sim800l::getSMSInterrupt(void) {
	uint16_t reply;
    
	if (!sendParseReply("AT+CFGRI?", "+CFGRI: ", &reply)) return 0;
    
	return reply;
}

bool Sim800l::setSMSInterrupt(uint8_t i) {
	return sendCheckReply("AT+CFGRI=", i, "OK");
}

int8_t Sim800l::getNumSMS(void) {
	uint16_t numsms;
    
	if (!sendCheckReply("AT+CMGF=1", "OK")) return -1;
	// ask how many sms are stored
    
	if(!sendParseReply("AT+CPMS?", "+CPMS: \"SM_P\",", &numsms)) return - 1;
    
	return numsms;
}

// Reading SMS's is a bit involved so we don't use helpers that may cause delays or debug
// printouts!
bool Sim800l::readSMS(uint8_t i, char *smsbuff, uint16_t maxlen, uint16_t *readlen) {
	// text mode
	if(!sendCheckReply("AT+CMGF=1", "OK")) return false;
    
	// show all text mode parameters
	if(!sendCheckReply("AT+CSDH=1", "OK")) return false;
    
	// parse out the SMS len
	uint16_t thesmslen = 0;
	

    
	getReply("AT+CMGR=", i, 1000);  //  do not print debug!
	//mySerial.printf("AT+CMGR=%d\r\n", i);
	//readline(1000);  // timeout
    
	// parse it out...
	if(!parseReply("+CMGR:", &thesmslen, ',', 11)) {
		*readlen = 0;
		return false;
	}
    
	readRaw(thesmslen);
    
	flushInput();
    
	uint16_t thelen = MIN(maxlen, (uint16_t)strlen(replybuffer));
	strncpy(smsbuff, replybuffer, thelen);
	smsbuff[thelen] = 0;  // end the string
    

	*readlen = thelen;
	return true;
}

// Retrieve the sender of the specified SMS message and copy it as a string to
// the sender buffer.  Up to senderlen characters of the sender will be copied
// and a null terminator will be added if less than senderlen charactesr are
// copied to the result.  Returns true if a result was successfully retrieved,
// otherwise false.
bool Sim800l::getSMSSender(uint8_t i, char *sender, int senderlen) {
	// Ensure text mode and all text mode parameters are sent.
	if(!sendCheckReply("AT+CMGF=1", "OK")) return false;
	if (!sendCheckReply("AT+CSDH=1", "OK")) return false;
	// Send command to retrieve SMS message and parse a line of response.
	char str[32];
	sprintf(str, (const char*) "AT+CMGR=%d\r\n", i);

	TM_USART_Puts(USART2, str);

	readline(1000);
	//return RecvResponse2(); //return l;

	// Parse the second field in the response.
	//bool result = parseReplyQuoted("+CMGR:", sender, senderlen, ',', (uint8_t)0x01);
	bool result = parseReplyQuoted("+CMGR:", sender, senderlen, ',', 1);
	// Drop any remaining data from the response.
	flushInput();
	return result;
}

bool Sim800l::sendSMS(char *smsaddr, char *smsmsg) {
	if (!sendCheckReply("AT+CMGF=1", "OK")) return -1;
    
	char sendcmd[30] = "AT+CMGS=\"";
	strncpy(sendcmd + 9, smsaddr, 30 - 9 - 2);   // 9 bytes beginning, 2 bytes for close quote + null
	sendcmd[strlen(sendcmd)] = '\"';
    
	if (!sendCheckReply(sendcmd, "> ")) return false;
	
	char str[32];
	sprintf(str, (const char*) "%s\r\n\r\n", smsmsg);

	TM_USART_Puts(USART2, str);
	TM_USART_Putc(USART2, 0x1A);

	readline(10000);  // read the +CMGS reply, wait up to 10 seconds!!!
    //Serial.print("* "); Serial.println(replybuffer);
    if(strstr(replybuffer, "+CMGS") == 0) {
		return false;
	}
	readline(1000);  // read OK
	//Serial.print("* "); Serial.println(replybuffer);
    
	if(strcmp(replybuffer, "OK") != 0) {
		return false;
	}
    
	return true;
}

bool Sim800l::deleteSMS(uint8_t i) {
	if (!sendCheckReply("AT+CMGF=1", "OK")) return -1;
	// read an sms
	char sendbuff[12] = "AT+CMGD=000";
	sendbuff[8] = (i / 100) + '0';
	i %= 100;
	sendbuff[9] = (i / 10) + '0';
	i %= 10;
	sendbuff[10] = i + '0';
    
	return sendCheckReply(sendbuff, "OK", 2000);
}

/********* TIME **********************************************************/

bool Sim800l::enableNetworkTimeSync(bool onoff) {
	if (onoff) {
		if (!sendCheckReply("AT+CLTS=1", "OK"))
			return false;
	}
	else {
		if (!sendCheckReply("AT+CLTS=0", "OK"))
			return false;
	}
    
	flushInput();  // eat any 'Unsolicted Result Code'
    
	return true;
}

bool Sim800l::enableNTPTimeSync(bool onoff, const char* ntpserver) {
	if (onoff) {
		if (!sendCheckReply("AT+CNTPCID=1", "OK"))
			return false;
        
		TM_USART_Puts(USART2, (char*)"AT+CNTP=\"");
		//mySerial.printf("AT+CNTP=\"");
		if (ntpserver != 0) {
			TM_USART_Puts(USART2, (char*)ntpserver);
			//mySerial.printf(ntpserver);
		}
		else {
			TM_USART_Puts(USART2, (char*)"pool.ntp.org");
		}
		TM_USART_Puts(USART2, (char*)"\",0\r\n");
		RecvResponse2();
		//readline(FONA_DEFAULT_TIMEOUT_MS);
		if (strcmp(replybuffer, "OK") != 0)
			return false;
        
		if (!sendCheckReply("AT+CNTP", "OK", 10000))
			return false;
        
		uint16_t status;
		//readline(10000);
		RecvResponse2();
		if (!parseReply("+CNTP:", &status))
			return false;
	}
	else {
		if (!sendCheckReply("AT+CNTPCID=0", "OK"))
			return false;
	}
    
	return true;
}

bool Sim800l::getTime(char* buff, uint16_t maxlen) {
	getReply("AT+CCLK?", (uint16_t) 10000);
	if (strncmp(replybuffer, "+CCLK: ", 7) != 0)
		return false;
    
	char *p = replybuffer + 7;
	uint16_t lentocopy = MIN((uint16_t)(maxlen - 1), (uint16_t)strlen(p));
	strncpy(buff, p, lentocopy + 1);
	buff[lentocopy] = 0;
    
	readline();  // eat OK
    
	return true;
}

/********* GPRS **********************************************************/

bool Sim800l::enableGPRS(bool onoff) {
	if (onoff) {
		// disconnect all sockets
		sendCheckReply("AT+CIPSHUT", "SHUT OK", 5000);
        
		if (!sendCheckReply("AT+CGATT=1", "OK", 10000))
			return false;
        
		// set bearer profile! connection type GPRS
		if(!sendCheckReply("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", "OK", 10000))
		    return false;
        
		// set bearer profile access point name
		if(apn) {
			// Send command AT+SAPBR=3,1,"APN","<apn value>" where <apn value> is the configured APN value.
			if(!sendCheckReplyQuoted("AT+SAPBR=3,1,\"APN\",", apn, "OK", 10000))
			    return false;
            
			// set username/password
			if(apnusername) {
				// Send command AT+SAPBR=3,1,"USER","<user>" where <user> is the configured APN username.
				if(!sendCheckReplyQuoted("AT+SAPBR=3,1,\"USER\",", apnusername, "OK", 10000))
				    return false;
			}
			if (apnpassword) {
				// Send command AT+SAPBR=3,1,"PWD","<password>" where <password> is the configured APN password.
				if(!sendCheckReplyQuoted("AT+SAPBR=3,1,\"PWD\",", apnpassword, "OK", 10000))
				    return false;
			}
		}
        
		// open GPRS context
		if(!sendCheckReply("AT+SAPBR=1,1", "OK", 10000))
		    return false;
	}
	else {
		// disconnect all sockets
		if(!sendCheckReply("AT+CIPSHUT", "SHUT OK", 5000))
		    return false;
        
		// close GPRS context
		if(!sendCheckReply("AT+SAPBR=0,1", "OK", 10000))
		    return false;
        
		if (!sendCheckReply("AT+CGATT=0", "OK", 10000))
			return false;
	}
	return true;
}

uint8_t Sim800l::GPRSstate(void) {
	uint16_t state;
    
	if (!sendParseReply("AT+CGATT?", "+CGATT: ", &state))
		return -1;
    
	return state;
}

void Sim800l::setGPRSNetworkSettings(const char* apn, const char* ausername, const char* apassword) {
	this->apn = (char*) apn;
	this->apnusername = (char*) ausername;
	this->apnpassword = (char*) apassword;
}

bool Sim800l::getGSMLoc(uint16_t *errorcode, char *buff, uint16_t maxlen) {
	getReply("AT+CIPGSMLOC=1,1", (uint16_t)10000);
    
	if (!parseReply("+CIPGSMLOC: ", errorcode))
		return false;
    
	char *p = replybuffer + 14;
	uint16_t lentocopy = MIN((uint16_t)(maxlen - 1), (uint16_t)strlen(p));
	strncpy(buff, p, lentocopy + 1);
    
	RecvResponse2();   // eat OK
    
	return true;
}

bool Sim800l::getGSMLoc(float *lat, float *lon) {
	uint16_t returncode;
	char gpsbuffer[120];
    
	// make sure we could get a response
	if(!getGSMLoc(&returncode, gpsbuffer, 120))
	    return false;
    
	// make sure we have a valid return code
	if(returncode != 0)
	    return false;
    
	// tokenize the gps buffer to locate the lat & long
	char *latp = strtok(gpsbuffer, ",");
	if (!latp) return false;
    
	char *longp = strtok(NULL, ",");
	if (!longp) return false;
    
	*lat = atof(latp);
	*lon = atof(longp);
    
	return true;
}

/********* TCP FUNCTIONS  ************************************/

bool Sim800l::TCPconnect(char *server, uint16_t port) {
	flushInput();
    
	// close all old connections
	if(!sendCheckReply("AT+CIPSHUT", "SHUT OK", 5000)) return false;
    
	// single connection at a time
	if(!sendCheckReply("AT+CIPMUX=0", "OK")) return false;
    
	// manually read data
	if(!sendCheckReply("AT+CIPRXGET=1", "OK")) return false;
    
#ifdef ADAFRUIT_FONA_DEBUG
	printf("AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n", server, port);
#endif
	
	char str[32];
	sprintf(str, (const char*) "AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n", server, port);

	TM_USART_Puts(USART2, str);


	//return RecvResponse2(); //return l;
    

    
	if (!expectReply("OK")) return false;
	if (!expectReply("CONNECT OK")) return false;
	return true;
}

bool Sim800l::TCPclose(void) {
	return sendCheckReply("AT+CIPCLOSE", "OK");
}

bool Sim800l::TCPconnected(void) {
	if (!sendCheckReply("AT+CIPSTATUS", "OK", 100)) return false;
	RecvResponse2();
#ifdef ADAFRUIT_FONA_DEBUG
	printf("\t<--- %s\r\n", replybuffer);
#endif
	return (strcmp(replybuffer, "STATE: CONNECT OK") == 0);
}

bool Sim800l::TCPsend(char *packet, uint8_t len) {
#ifdef ADAFRUIT_FONA_DEBUG
	printf("AT+CIPSEND=%d\r\n", len);
    
	for (uint16_t i = 0; i < len; i++) {
		printf(" 0x%#02x", packet[i]);
	}
	printf("\r\n");
#endif
	char str[32];
	sprintf(str, (const char*) "AT+CIPSEND=%d\r\n", len);

	TM_USART_Puts(USART2, str);
    

	RecvResponse2();

	if (replybuffer[0] != '>') return false;
    
	for (uint16_t i = 0; i < len; i++) {
		TM_USART_Putc(USART2, packet[i]);
	}
	RecvResponse2();   // wait up to 3 seconds to send the data

    
	return (strcmp(replybuffer, "SEND OK") == 0);
}

uint16_t Sim800l::TCPavailable(void) {
	uint16_t avail;
    
	if (!sendParseReply("AT+CIPRXGET=4", "+CIPRXGET: 4,", &avail, ',', 0)) return false;
    
#ifdef ADAFRUIT_FONA_DEBUG
	printf("%d bytes available\r\n", avail);
#endif
    
	return avail;
}

uint16_t Sim800l::TCPread(uint8_t *buff, uint8_t len) {
	uint16_t avail;
	
	char str[32];
	sprintf(str, (const char*)"AT+CIPRXGET=2,%d\r\n", len);

	TM_USART_Puts(USART2, str);
    

	RecvResponse2();
	if (!parseReply("+CIPRXGET: 2,", &avail, ',', 0)) return false;
    
	readRaw(avail);
    
#ifdef ADAFRUIT_FONA_DEBUG
	printf("%d bytes read\r\n", avail);
	for (uint8_t i = 0; i < avail; i++) {
		printf(" 0x%#02x", replybuffer[i]);
	}
	printf("\r\n");
#endif
    
	memcpy(buff, replybuffer, avail);
    
	return avail;
}

/********* HTTP LOW LEVEL FUNCTIONS  ************************************/

bool Sim800l::HTTP_init() {
	return sendCheckReply("AT+HTTPINIT", "OK");
}

bool Sim800l::HTTP_term() {
	return sendCheckReply("AT+HTTPTERM", "OK");
}

void Sim800l::HTTP_para_start(const char* parameter, bool quoted) {
	flushInput();
	
	char str[32];
	sprintf(str, (const char*) "AT+HTTPPARA=\"%s", parameter);

	TM_USART_Puts(USART2, str);
    
	if (quoted)
		TM_USART_Puts(USART2, (char*) "\",\"");
		
	else
		TM_USART_Puts(USART2, (char*) "\",");

}

bool Sim800l::HTTP_para_end(bool quoted) {
	if (quoted)
		TM_USART_Puts(USART2, (char*)"\"\r\n");
	else
		TM_USART_Puts(USART2, (char*)"\r\n");
    
	return expectReply("OK");
}

bool Sim800l::HTTP_para(const char* parameter, const char* value) {
	HTTP_para_start(parameter, true);
	
	TM_USART_Puts(USART2, (char*) value);
	
	return HTTP_para_end(true);
}

bool Sim800l::HTTP_para(const char* parameter, int32_t value) {
	HTTP_para_start(parameter, false);
	char str[32];
	int i = 1;
	sprintf(str, "%d", value);
	TM_USART_Puts(USART2, str);
	return HTTP_para_end(false);
}

bool Sim800l::HTTP_data(uint32_t size, uint32_t maxTime) {
	flushInput();
    
	char str[32];
	int i = 1;
	sprintf(str, "AT+HTTPDATA=%d,%d\r\n", size, maxTime);
	TM_USART_Puts(USART2, str);
    
	return expectReply("DOWNLOAD");
}

bool Sim800l::HTTP_action(uint8_t method, uint16_t *status, uint16_t *datalen, int32_t timeout) {
	// Send request.
	if(!sendCheckReply("AT+HTTPACTION=", method, "OK"))
	    return false;
    
	// Parse response status and size.
	readline(timeout);
	if (!parseReply("+HTTPACTION:", status, ',', 1))
		return false;
	if (!parseReply("+HTTPACTION:", datalen, ',', 2))
		return false;
    
	return true;
}

bool Sim800l::HTTP_readall(uint16_t *datalen) {
	getReply("AT+HTTPREAD");
	if (!parseReply("+HTTPREAD:", datalen, ',', 0))
		return false;
    
	return true;
}

bool Sim800l::HTTP_ssl(bool onoff) {
	return sendCheckReply("AT+HTTPSSL=", onoff ? 1 : 0, "OK");
}

/********* HTTP HIGH LEVEL FUNCTIONS ***************************/

bool Sim800l::HTTP_GET_start(char *url, uint16_t *status, uint16_t *datalen) {
	if (!HTTP_setup(url))
		return false;
    
	// HTTP GET
	if(!HTTP_action(FONA_HTTP_GET, status, datalen))
	    return false;
    
#ifdef ADAFRUIT_FONA_DEBUG
	printf("Status: %d\r\n", *status);
	printf("Len: %d\r\n", *datalen);
#endif
    
	// HTTP response data
	if(!HTTP_readall(datalen))
	    return false;
    
	return true;
}

void Sim800l::HTTP_GET_end(void) {
	HTTP_term();
}

bool Sim800l::HTTP_POST_start(char *url, const char* contenttype, const uint8_t *postdata, uint16_t postdatalen, uint16_t *status, uint16_t *datalen) {
	if (!HTTP_setup(url))
		return false;
    
	if (!HTTP_para("CONTENT", contenttype)) {
		return false;
	}
    
	// HTTP POST data
	if(!HTTP_data(postdatalen, 10000))
	    return false;
	for (uint16_t i = 0; i < postdatalen; i++) {
		TM_USART_Putc(USART2, postdata[i]);
		
	}
	if (!expectReply("OK"))
		return false;
    
	// HTTP POST
	if(!HTTP_action(FONA_HTTP_POST, status, datalen))
	    return false;
    
#ifdef ADAFRUIT_FONA_DEBUG
	printf("Status: %d\r\n", *status);
	printf("Len: %d\r\n", *datalen);
#endif
    
	// HTTP response data
	if(!HTTP_readall(datalen))
	    return false;
    
	return true;
}

void Sim800l::HTTP_POST_end(void) {
	HTTP_term();
}

void Sim800l::setUserAgent(const char* useragent) {
	this->useragent = (char*) useragent;
}

void Sim800l::setHTTPSRedirect(bool onoff) {
	httpsredirect = onoff;
}

/********* HTTP HELPERS ****************************************/

bool Sim800l::HTTP_setup(char *url) {
	// Handle any pending
	HTTP_term();
    
	// Initialize and set parameters
	if(!HTTP_init())
	    return false;
	if (!HTTP_para("CID", 1))
		return false;
	if (!HTTP_para("UA", useragent))
		return false;
	if (!HTTP_para("URL", url))
		return false;
    
	// HTTPS redirect
	if(httpsredirect) {
		if (!HTTP_para("REDIR", 1))
			return false;
        
		if (!HTTP_ssl(true))
			return false;
	}
    
	return true;
}

//
//PRIVATE METHODS
//
//String Sim800l::_readSerial() {
//	_timeout = 0;
//	char* buf;
//	
//	while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET && _timeout < 3000) 
//	{
//		Delayms(1);
//		_timeout++;
//
//
//	}
//	if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET) {
//		TM_USART_Gets(USART2, buf, 255);
//		return buf;
//	}
//	return "";
//
//}


//
//PUBLIC METHODS
//





