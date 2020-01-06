/*
 * xpt2046.h
 *
 * XPT2046 is the same controller with TI's ADS7843.
 * The manual of ADS7843 can be found here:
 * 	http://www.ti.com/lit/ds/symlink/ads7843.pdf
 *
 * There'a a document that provides some tips for the
 * touch controller:
 * 	"TOUCH SCREEN CONTROLLER TIPS"
 * 	http://www.ti.com/lit/an/sbaa036/sbaa036.pdf
 *
 * For calibration methods see TI's document
 * 	"Calibration in touch-screen systems".
 * 	http://www.ti.com/lit/an/slyt277/slyt277.pdf
 *
 *
 *  Created on: Jul 21, 2018
 *      Author: Dimitris Tassopoulos
 */

#ifndef XPT2046_ADS7843_H_
#define XPT2046_ADS7843_H_

/*#include "platform_config.h"

#include "dev_spi.h"
#include "cortexm_delay.h"*/
#include "stm32f4xx.h"
#include <stdbool.h>

#define MATRIX_AN 5992
#define MATRIX_BN -35
#define MATRIX_CN -2611392
#define MATRIX_DN 28
#define MATRIX_EN 4460
#define MATRIX_FN -1300816
#define MATRIX_DIV 65536


typedef struct Matrix 
{
	/* This arrangement of values facilitates  calculations within getDisplayPoint() */
	int  
	An, /* A = An/Divider */
	Bn, /* B = Bn/Divider */   
	Cn, /* C = Cn/Divider */   
	Dn, /* D = Dn/Divider */   
	En, /* E = En/Divider */   
	Fn, /* F = Fn/Divider */   
	Divider;   
} Matrix ;

typedef struct {
	int x[5], xfb[5];
	int y[5], yfb[5];
	int a[7];
} calibration;

typedef	struct POINT 
{
	int x;
	int y;
}Coordinate;

extern Matrix matrix;
extern Coordinate  display;

/* Private define ------------------------------------------------------------*/
/* AD channel selection command */
#define	CHX 	        0x90 	/* Channel X+ command*/	
#define	CHY 	        0xd0	/* Channel Y+ command* */

#define TP_CS(x)	((x) ? (GPIO_SetBits(GPIOD, GPIO_Pin_6)) : (GPIO_ResetBits(GPIOD, GPIO_Pin_6) ) )

#define TP_INT_IN   (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3))

/* Private function prototypes -----------------------------------------------*/				
void xpt2046_init(void);	
Coordinate *Read_Value(void);
int getDisplayPoint(Coordinate * displayPtr, Coordinate * screenPtr, Matrix * matrixPtr);
int setCalibrationMatrix(Coordinate * displayPtr, Coordinate * screenPtr, Matrix * matrixPtr);
void ts_draw_point(int x, int y, int color);
void ts_draw_cross(int Xpos, int Ypos);
void ts_calibrate(int x_size, int y_size);
bool TouchPressed();

#endif /* XPT2046_ADS7843_H_ */
