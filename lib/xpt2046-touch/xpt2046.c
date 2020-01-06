/*
 * xpt2046.c
 *
 *  Created on: Jul 21, 2018
 *      Author: Dimitris Tassopoulos
 */

#include <stdbool.h>
#include "xpt2046.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_delay.h"
#include "stm32f4xx_syscfg.h"
#include "ili9341_graph.h"


#define T_CS_HIGT	GPIO_SetBits(XPT2046_CS_GPIO_Port, XPT2046_CS_PIN)
#define T_CS_LOW	GPIO_ResetBits(XPT2046_CS_GPIO_Port, XPT2046_CS_PIN)

/* Private define ------------------------------------------------------------*/
#define THRESHOLD 2   /* The bigger of the value, the lower the sensitivity */

static Coordinate ts_position[5];
static Coordinate display_position[5];
static calibration cal;

Matrix matrix;
Coordinate display;

static void delay_nus(int cnt)
{
	int i, us;
	for (i = 0; i < cnt; i++)
	{
		us = 40;
		while (us--)     /* delay	*/
		{
		}
	}
}

//----------------------------------------------------------------------------XPT2046 SPI initialization
void xpt2046_init(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;
	//SPI_InitTypeDef  SPI_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB| RCC_AHB1Periph_GPIOC
	| RCC_AHB1Periph_GPIOD| RCC_AHB1Periph_GPIOE,
		ENABLE); 
	
	TM_SPI_Init(SPI1, TM_SPI_PinsPack_1);


	/* xpt2046  CS */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = XPT2046_CS_PIN;
	GPIO_Init(XPT2046_CS_GPIO_Port, &GPIO_InitStructure);
	/* xpt2046  IRQ */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = XPT2046_IRQ_PIN;
	GPIO_Init(XPT2046_IRQ_PORT, &GPIO_InitStructure);

	matrix.An = MATRIX_AN;
	matrix.Bn = MATRIX_BN;
	matrix.Cn = MATRIX_CN;
	matrix.Dn = MATRIX_DN;
	matrix.En = MATRIX_EN;
	matrix.Fn = MATRIX_FN;
	matrix.Divider = MATRIX_DIV;
	
}

//------------------------------------------------------------------------------delay function
int getDisplayPoint(Coordinate * displayPtr, Coordinate * screenPtr, Matrix * matrixPtr)
{
	/* Operation order is important since we are doing integer math. 
	   Make sure you add all terms together before dividing, so that 
	   the remainder is not rounded off prematurely. */
	
	if (matrixPtr->Divider != 0)
	{
		/* XD = AX+BY+C */        
		displayPtr->x = ((matrixPtr->An * screenPtr->x) + 
		                  (matrixPtr->Bn * screenPtr->y) + 
		                   matrixPtr->Cn 
						) / matrixPtr->Divider;
		/* YD = DX+EY+F */        
		displayPtr->y = ((matrixPtr->Dn * screenPtr->x) + 
		                  (matrixPtr->En * screenPtr->y) + 
		                   matrixPtr->Fn 
						) / matrixPtr->Divider;
	}
	else
	{
		return -1;
	}
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------
int perform_calibration(calibration *cal) 
{
	int j;
	float n, x, y, x2, y2, xy, z, zx, zy;
	float det, a, b, c, e, f, i;
	float scaling = 65536.0;

	/* Get sums for matrix */
	n = x = y = x2 = y2 = xy = 0;
	for (j = 0; j < 5; j++) {
		n += 1.0;
		x += (float)cal->x[j];
		y += (float)cal->y[j];
		x2 += (float)(cal->x[j]*cal->x[j]);
		y2 += (float)(cal->y[j]*cal->y[j]);
		xy += (float)(cal->x[j]*cal->y[j]);
	}

	/* Get determinant of matrix -- check if determinant is too small */
	det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
	if (det < 0.1 && det > -0.1) {
		/* ts_calibrate determinant is too small */
		return 0;
	}

	/* Get elements of inverse matrix */
	a = (x2*y2 - xy*xy) / det;
	b = (xy*y - x*y2) / det;
	c = (x*xy - y*x2) / det;
	e = (n*y2 - y*y) / det;
	f = (x*y - n*xy) / det;
	i = (n*x2 - x*x) / det;

	/* Get sums for x calibration */
	z = zx = zy = 0;
	for (j = 0; j < 5; j++) {
		z += (float)cal->xfb[j];
		zx += (float)(cal->xfb[j]*cal->x[j]);
		zy += (float)(cal->xfb[j]*cal->y[j]);
	}
	/* Now multiply out to get the calibration for framebuffer x coord */
	cal->a[2] = (int)((a*z + b*zx + c*zy)*(scaling));
	cal->a[0] = (int)((b*z + e*zx + f*zy)*(scaling));
	cal->a[1] = (int)((c*z + f*zx + i*zy)*(scaling));

	/* Get sums for y calibration */
	z = zx = zy = 0;
	for (j = 0; j < 5; j++) {
		z += (float)cal->yfb[j];
		zx += (float)(cal->yfb[j]*cal->x[j]);
		zy += (float)(cal->yfb[j]*cal->y[j]);
	}
	/* Now multiply out to get the calibration for framebuffer y coord */
	cal->a[5] = (int)((a*z + b*zx + c*zy)*(scaling));
	cal->a[3] = (int)((b*z + e*zx + f*zy)*(scaling));
	cal->a[4] = (int)((c*z + f*zx + i*zy)*(scaling));

	/* assign scaling */
	cal->a[6] = (int)scaling;
	return 1;
}

//------------------------------------------------------------------------------------------------------------------------------
void ts_calibrate( int x_size, int y_size )
{
	unsigned char i;
	uint32_t count;
	Coordinate * Ptr;
	
	display_position[0].x = 20;              display_position[0].y = 20;
	display_position[1].x = x_size - 20;     display_position[1].y = 20;
	display_position[2].x = x_size - 20;     display_position[2].y = y_size - 20;
	display_position[3].x = 20;              display_position[3].y = y_size - 20;
	display_position[4].x = x_size / 2;      display_position[4].y = y_size / 2;

	for( i=0; i<5; i++ )
	{
		LCD_fillScreen(BLACK);
		
		//for( count=0; count<1000*10000; count++ );   /* delay */
		Delayms(100);
		
		ts_draw_cross(display_position[i].x,display_position[i].y);
		do
		{
			Ptr = Read_Value();
		}
		while( Ptr == (void*)0 );
		ts_position[i].x= Ptr->x; ts_position[i].y= Ptr->y;
	}
	
	LCD_fillScreen(BLACK);
	
	cal.xfb[0] = display_position[0].x;
	cal.yfb[0] = display_position[0].y;

	cal.x[0] = ts_position[0].x;
	cal.y[0] = ts_position[0].y;

	cal.xfb[1] = display_position[1].x;
	cal.yfb[1] = display_position[1].y;

	cal.x[1] = ts_position[1].x;
	cal.y[1] = ts_position[1].y;

	cal.xfb[2] = display_position[2].x;
	cal.yfb[2] = display_position[2].y;

	cal.x[2] = ts_position[2].x;
	cal.y[2] = ts_position[2].y;

	cal.xfb[3] = display_position[3].x;
	cal.yfb[3] = display_position[3].y;

	cal.x[3] = ts_position[3].x;
	cal.y[3] = ts_position[3].y;

	cal.xfb[4] = display_position[4].x;
	cal.yfb[4] = display_position[4].y;

	cal.x[4] = ts_position[4].x;
	cal.y[4] = ts_position[4].y;

	perform_calibration(&cal);

	matrix.An = cal.a[0];
	matrix.Bn = cal.a[1];
	matrix.Cn = cal.a[2];
	matrix.Dn = cal.a[3];
	matrix.En = cal.a[4];
	matrix.Fn = cal.a[5];
	matrix.Divider = cal.a[6];
}
//------------------------------------------------------------------------------------------------------------------------------
void ts_draw_cross(int xpos, int ypos)
{
	LCD_drawLine(WHITE, xpos - 15, ypos, xpos - 2, ypos);
	LCD_drawLine(WHITE, xpos + 2, ypos, xpos + 15, ypos);
	LCD_drawLine(WHITE, xpos, ypos - 15, xpos, ypos - 2);
	LCD_drawLine(WHITE, xpos, ypos + 2, xpos, ypos + 15);

	LCD_drawLine(RED, xpos - 15, ypos + 15, xpos - 7, ypos + 15);
	LCD_drawLine(RED, xpos - 15, ypos + 7, xpos - 15, ypos + 15);

	LCD_drawLine(RED, xpos - 15, ypos - 15, xpos - 7, ypos - 15);
	LCD_drawLine(RED, xpos - 15, ypos - 7, xpos - 15, ypos - 15);

	LCD_drawLine(RED, xpos + 7, ypos + 15, xpos + 15, ypos + 15);
	LCD_drawLine(RED, xpos + 15, ypos + 7, xpos + 15, ypos + 15);

	LCD_drawLine(RED, xpos + 7, ypos - 15, xpos + 15, ypos - 15);
	LCD_drawLine(RED, xpos + 15, ypos - 15, xpos + 15, ypos - 7);
}
//------------------------------------------------------------------------------------------------------------------------------
void ts_draw_point(int x, int y, int color)
{
	if (x >= LCD_getWidth() || y >= LCD_getHeight())
	{
		return;
	}

	if( x == 0 || y == 0 )
	{
		return;
	}

	LCD_putPixel(x, y, color);         
	LCD_putPixel(x - 1, y, color);                
	LCD_putPixel(x, y - 1, color);
	LCD_putPixel(x - 1, y - 1, color);
	LCD_putPixel(x + 1, y - 1, color);
	LCD_putPixel(x, y + 1, color);
	LCD_putPixel(x - 1, y + 1, color);
	LCD_putPixel(x + 1, y + 1, color);
	LCD_putPixel(x + 1, y, color);
}
//------------------------------------------------------------------------SPI bus transmit and receive data
static unsigned char WR_CMD (unsigned char cmd)  
{ 
	/* Wait for SPI Tx buffer empty */ 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
	/* Send SPI data */ 
	SPI_I2S_SendData(SPI1,cmd); 
	/* Wait for SPI data reception */ 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
	/* Read SPI received data */ 
	return SPI_I2S_ReceiveData(SPI1); 
} 
//-------------------------------------------------------------------------read xpt2046 adc value
static int RD_AD(void)  
{ 
	unsigned short buf,temp; 

	temp = WR_CMD(0x00);
	buf = temp << 8; 
	delay_nus(1); 
	temp = WR_CMD(0x00);;
	buf |= temp; 
	buf >>= 3; 
	buf &= 0xfff; 
	return buf;
}
//------------------------------------------------------------------------read xpt2046 channel X+ adc value
int Read_X(void)  
{  
	unsigned short temp;
	TP_CS(0); 
	delay_nus(1); 
	WR_CMD(CHX); 
	delay_nus(1); 
	temp = RD_AD(); 
	TP_CS(1); 
	return temp;    
}  
//-----------------------------------------------------------------------read xpt2046 channel Y+ adc value
int Read_Y(void)  
{  
	unsigned short temp;
	TP_CS(0);
	delay_nus(1);
	WR_CMD(CHY);
	delay_nus(1);
	temp = RD_AD();
	TP_CS(1); 
	return temp;
} 
//----------------------------------------------------------------read xpt2046 channel X+ channel Y+ adc value
void TP_GetAdXY(int *x,int *y)  
{ 
	int adx,ady; 
	adx = Read_X(); 
	delay_nus(1); 
	ady = Read_Y(); 
	*x = adx; 
	*y = ady; 
}
//-------------------------------------------get xpt2046 channel X+ channel Y+ adc filtering value, some interference values can be excluded
Coordinate *Read_Value(void)
{
	static Coordinate screen;
	int m0,m1,m2,TP_X[1],TP_Y[1],temp[3];
	unsigned char count=0;
	int buffer[2][9]={{0},{0}};  /* channel X+ Y+ for sampling buffer */
	do                           /* 9 sampling times */
	{		   
		TP_GetAdXY(TP_X,TP_Y);  
		buffer[0][count]=TP_X[0];  
		buffer[1][count]=TP_Y[0];
		count++;  
	}
	while(!TP_INT_IN&& count<9);  /* TP_INT_IN interrupt pin for the touch screen, when the user clicks on the touch screen£¬
	                                 TP_INT_IN Low level */

	if(count==9)   /* sampled 9 times, then filtering some interference values can be excluded*/ 
	{  
		/* average value of the 3 groups */
		temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
		temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
		temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;
		/* D-value between the 3 groups */
		m0=temp[0]-temp[1];
		m1=temp[1]-temp[2];
		m2=temp[2]-temp[0];
		/* absolute value of D-value */
		m0=m0>0?m0:(-m0);
		m1=m1>0?m1:(-m1);
		m2=m2>0?m2:(-m2);

		/* judging Whether the absolute value of D-value is more than threshold, if three absolute value of D-value is 
		   more than threshold value, then determine the sampling point for the interference, ignore the sampling point, 
		   the threshold value is setting to 2, in this example */
		if( m0>THRESHOLD  &&  m1>THRESHOLD  &&  m2>THRESHOLD ) return 0;
		/* calculate channel X+ average values,then assign them to screen */
		if(m0<m1)
		{
			if(m2<m0) 
			screen.x=(temp[0]+temp[2])/2;
			else 
			screen.x=(temp[0]+temp[1])/2;	
		}
		else if(m2<m1) 
			screen.x=(temp[0]+temp[2])/2;
		else 
			screen.x=(temp[1]+temp[2])/2;

		/* same as above, this is channel Y+ */
		temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
		temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
		temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
		m0=temp[0]-temp[1];
		m1=temp[1]-temp[2];
		m2=temp[2]-temp[0];
		m0=m0>0?m0:(-m0);
		m1=m1>0?m1:(-m1);
		m2=m2>0?m2:(-m2);
		if(m0>THRESHOLD&&m1>THRESHOLD&&m2>THRESHOLD) return 0;

		if(m0<m1)
		{
			if(m2<m0) 
				screen.y=(temp[0]+temp[2])/2;
			else 
				screen.y=(temp[0]+temp[1])/2;	
		}
		else if(m2<m1) 
			screen.y=(temp[0]+temp[2])/2;
		else
			screen.y=(temp[1]+temp[2])/2;

		return &screen;
	}  
	return 0; 
}


bool TouchPressed() {
	return !TP_INT_IN;
	//return GPIO_ReadInputDataBit(XPT2046_IRQ_PORT, XPT2046_IRQ_PIN) == 0;
}


