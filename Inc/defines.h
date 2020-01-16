	/**
 *  Defines for your entire project at one place
 * 
 *	@author 	Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@version 	v1.0
 *	@ide		Keil uVision 5
 *	@license	GNU GPL v3
 *	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#ifndef TM_DEFINES_H
#define TM_DEFINES_H
#include "tm_stm32f4_spi.h"

/* Put your global defines for all libraries here used in your project */

/* TIM3 is used for background function */
#define BG_TIM				TIM3
#define BG_TIM_IRQ			TIM3_IRQn
#define BG_TIM_IRQ_HANDLER	TIM3_IRQHandler

/* TIM4 is used for delay functions */
#define TM_DELAY_TIM				TIM4
#define TM_DELAY_TIM_IRQ			TIM4_IRQn
#define TM_DELAY_TIM_IRQ_HANDLER	TIM4_IRQHandler

#define TM_USART2_USE_CUSTOM_IRQ
#define TM_USART2_ReceiveHandler(args...) Sim800l_onSerialDataReceived(args);

/* Uncomment if you want to disable ID pin for USB MSC HOST library */
//#define USB_MSC_HOST_DISABLE_ID
 
/* Uncomment if you want to disable VBUS pin for USB MSC HOST library */
/* If you do this, on F4 and F429 Discovery boards USB will not work */
//#define USB_MSC_HOST_DISABLE_VBUS
 
/* Uncomment if you want to enable USB HS in FS mode */
/* By default, FS mode is used */
//#define USE_USB_OTG_HS        /* STM32F429-Discovery */
 
/* This line MUST BE ADDED in order to use FATFS with USB */
/* Otherwise you will have errors */
#define FATFS_USE_USB        1
 
/* By default, if you use USB, then SD card is disabled */
/* If you don't use USB, then SDIO is used by default! */
/* If you want to use both, USB and sdcard at the same time */
/* Enable SD card communication below: */
 
/* Enable SDIO communication */
//#define FATFS_USE_SDIO    1
/* If you want SPI communication instead of SDIO, enable with */
//#define FATFS_USE_SDIO    0

//----- SPI1 options start -------

#define TM_SPI1_PRESCALER	SPI_BaudRatePrescaler_32
#define TM_SPI1_DATASIZE 	SPI_DataSize_8b
#define TM_SPI1_FIRSTBIT 	SPI_FirstBit_MSB
#define TM_SPI1_MASTERSLAVE	SPI_Mode_Master

#define TM_SPI1_MODE		TM_SPI_Mode_0

//----- SPI1 options end -------

//----- SPI2 options start -------

#define TM_SPI2_PRESCALER	SPI_BaudRatePrescaler_2
#define TM_SPI2_DATASIZE 	SPI_DataSize_8b
#define TM_SPI2_FIRSTBIT 	SPI_FirstBit_MSB
#define TM_SPI2_MASTERSLAVE	SPI_Mode_Master

#define TM_SPI2_MODE		TM_SPI_Mode_0

//----- SPI2 options end -------

#define ILI9341_SCK_Pin GPIO_PIN_5
#define ILI9341_SCK_GPIO_Port GPIOA
#define ILI9341_MISO_Pin GPIO_PIN_6
#define ILI9341_MISO_GPIO_Port GPIOA
#define ILI9341_MOSI_Pin GPIO_PIN_7
#define ILI9341_MOSI_GPIO_Port GPIOA
#define ILI9341_RS_Pin GPIO_Pin_9
#define ILI9341_RS_GPIO_Port GPIOD
#define ILI9341_RST_Pin GPIO_Pin_10
#define ILI9341_RST_GPIO_Port GPIOD
#define ILI9341_CS_Pin GPIO_Pin_12
#define ILI9341_CS_GPIO_Port GPIOB

#define XPT2046_IRQ_PIN		GPIO_Pin_3
#define XPT2046_IRQ_PORT	GPIOD
#define XPT2046_CS_PIN GPIO_Pin_6
#define XPT2046_CS_GPIO_Port GPIOD

#define Touch_SCK_Pin GPIO_Pin_0
#define Touch_SCK_GPIO_Port GPIOD
#define Touch_MISO_Pin GPIO_Pin_1
#define Touch_MISO_GPIO_Port GPIOD
#define Touch_MOSI_Pin GPIO_Pin_2
#define Touch_MOSI_GPIO_Port GPIOD
#define Touch_IRQ_Pin GPIO_Pin_3
#define Touch_IRQ_GPIO_Port GPIOD
#define Touch_CS_Pin GPIO_Pin_6
#define Touch_CS_GPIO_Port GPIOD
#endif
