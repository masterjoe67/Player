#pragma once
#include "SIM800L_interface.h"
#include "ili9341_graph.h"
#include "ili9341_text.h"
#include "xpt2046.h"
#include "fonts.h"

bool phone_initialized;

void onIncomingCall();
void phoneInit();