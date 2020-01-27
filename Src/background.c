
#include "background.h"
#include "display.h"
#include "phone.h"
#include "main.h"

uint8_t BG_TIM_Initialized = 0;
__IO uint32_t BG_Time_sec, BG_Time_10sec = 0;


void BG_Init(void) {
	/* If already initialized */
	if (BG_TIM_Initialized) {
		return;
	}
	vbat_refresh = false;
	battery_level = 0;
	/* Enable External HSE clock */
	RCC_HSEConfig(RCC_HSE_ON);
	
	/* Wait for stable clock */
	while (!RCC_WaitForHSEStartUp()) ;
	

	BG_INT_InitTIM();

	TM_ADC_Init(ADC1, 1);

	LONG_CYCLE_Flag = false;
	/* Set initialized flag */
	BG_TIM_Initialized = 1;
}

void BG_INT_InitTIM(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	TM_TIMER_PROPERTIES_t TIM_Data;
	
	/* Get timer properties */
	TM_TIMER_PROPERTIES_GetTimerProperties(BG_TIM, &TIM_Data);
	
	/* Generate timer properties, 1us ticks */
	TM_TIMER_PROPERTIES_GenerateDataForWorkingFrequency(&TIM_Data, 1);
	
	/* Enable clock for TIMx */
	TM_TIMER_PROPERTIES_EnableClock(BG_TIM);
	
	/* Set timer settings */
	TIM_TimeBaseStruct.TIM_ClockDivision = 256;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStruct.TIM_Period = 9999; /* 10 millisecond */
	TIM_TimeBaseStruct.TIM_Prescaler = 8192; // SystemCoreClock / (1000000 * (SystemCoreClock / TIM_Data.TimerFrequency)) - 1; /* With prescaler for 1 microsecond tick */
	//TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
	
	/* Initialize timer */
	TIM_TimeBaseInit(BG_TIM, &TIM_TimeBaseStruct);
	
	/* Enable interrupt */
	TIM_ITConfig(BG_TIM, TIM_IT_Update, ENABLE);
	
	/* Set NVIC parameters */
	NVIC_InitStruct.NVIC_IRQChannel = BG_TIM_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStruct);
	
	/* Start timer */
	//TIM_Cmd(BG_TIM, ENABLE);
}

void BG_TIM_IRQ_HANDLER(void) {
	TIM_ClearITPendingBit(BG_TIM, TIM_IT_Update);
	TIM_Cmd(BG_TIM, DISABLE);
	//BG_Time_sec++;
	if (system_initialized) ///(BG_Time_sec > 100 && system_initialized)
	{
		//__disable_irq();
		BG_Time_10sec++;
		VBAT = TM_ADC_Read(ADC1, 1) * ADC_SUPPLY_VOLTAGE / 0xFFF * 1.777f;
		if (VBAT > 4100)
			battery_level = 4;
		else if (VBAT > 3800)
			battery_level = 3;
		else if (VBAT > 3500)
			battery_level = 2;
		else if (VBAT > 3300)
			battery_level = 1;
		else battery_level = 0;
		TM_DISCO_LedToggle(LED_ORANGE);
		//BG_Time_sec = 0;
		//if (vbat_refresh == true)
		//	battery_helper();
		//__enable_irq(); 
	}
	if (BG_Time_10sec > 3 && phone_initialized)
	{
		//__disable_irq();
		BG_Time_10sec = 0;
		LONG_CYCLE_Flag = true;
		//__enable_irq(); 
	}
	
	BG_EnableBGTimer();
}

void BG_EnableBGTimer(void) {
	/* Check if library is even initialized */
	if (!BG_TIM_Initialized) {
		return;
	}
	TIM_Cmd(BG_TIM, ENABLE);

}

void BG_DisableBGTimer(void) {

	TIM_Cmd(BG_TIM, DISABLE);

}