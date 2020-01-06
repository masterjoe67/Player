#pragma once
#include "stdlib.h"
#include "math.h"
#include "defines.h"
#include "stm32f4xx.h"
#include <stdbool.h>
#include "ili9341_text.h"

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

typedef struct
{
	int16_t       _x1, _y1;  // Coordinates of top-left corner
	uint16_t      _w, _h;
	uint8_t       _textsize_x;
	uint8_t       _textsize_y;
	uint16_t      _outlinecolor, _fillcolor, _textcolor;
	char          _label[10];
	bool currstate, laststate;
}BTN_TypeDef;

typedef struct
{
	int16_t       _x1, _y1;   // Coordinates of top-left corner
	uint16_t      _w, _h;
	uint8_t       _textsize;
	const GFXfont *_f;
	uint16_t      _outlinecolor, _fillcolor, _textcolor;
	char          _label[10];
	bool currstate, laststate;
}BTN_Label_TypeDef;

typedef struct
{
	int16_t       _x1, _y1;   // Coordinates of top-left corner
	uint16_t      _w, _h;
	uint16_t* bitmap;
	bool currstate, laststate;
}BTN_Icon_TypeDef;

//extern _m_button lcd_button;

void BTN_init(
	BTN_TypeDef* btn,
	int16_t x,
	int16_t y,
	uint16_t w,
	uint16_t h,
	uint16_t outline,
	uint16_t fill,
	uint16_t textcolor,
	char *label,
	uint8_t textsize,
	uint8_t textsize_y);


void BTN_initUL(
	BTN_TypeDef* btn,
	int16_t x1,
	int16_t y1,
	uint16_t w,
	uint16_t h,
	uint16_t outline,
	uint16_t fill,
	uint16_t textcolor,
	char *label,
	uint8_t textsize_x,
	uint8_t textsize_y);

void BTN_icon_initUL(
	BTN_Icon_TypeDef* btn,
	int16_t x1,
	int16_t y1,
	uint16_t w,
	uint16_t h,
	uint16_t* bitmap);

void BTN_icon_draw(BTN_Icon_TypeDef* btn);

void BTN_draw(BTN_TypeDef* btn, bool inverted);

bool BTN_isPressed(BTN_TypeDef* btn, int16_t x, int16_t y);

void BTN_Label_initUL(
	BTN_Label_TypeDef* btn,
	int16_t x1,
	int16_t y1,
	uint16_t w,
	uint16_t h,
	uint16_t outline,
	uint16_t fill,
	uint16_t textcolor,
	char *label,
	uint8_t textsize,
	const GFXfont *f);

void BTN_Label_draw(BTN_Label_TypeDef* btn);
