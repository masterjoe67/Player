

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "defines.h"
#include "phone.h"
#include "SIM800L_interface.h"

#define SIM800_EXTI_IRQHandler Sim800l_onIncomingCall(); //onIncomingCall();

bool system_initialized;
void MP3_Player();


#endif /* __MAIN_H */


