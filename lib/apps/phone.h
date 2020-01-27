#pragma once
#include "SIM800L_interface.h"
#include "ili9341_graph.h"
#include "ili9341_text.h"
#include "xpt2046.h"
#include "fonts.h"
#include "lcdButton.h"

bool phone_initialized;
uint8_t NetworkStatus;

void onIncomingCall();
void phoneInit();
void phone_call();
void sms();
bool isRinging();