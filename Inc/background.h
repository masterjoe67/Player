#pragma once
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "tm_stm32f4_timer_properties.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_adc.h"
#include <stdbool.h>

uint16_t VBAT;
uint8_t battery_level;

bool vbat_refresh;
void BG_Init(void);
void BG_INT_InitTIM(void);
void BG_EnableBGTimer(void);
void BG_DisableBGTimer(void);