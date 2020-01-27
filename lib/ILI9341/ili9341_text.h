#ifndef TEST1_TEXT_H
#define TEST1_TEXT_H

#include "ili9341_graph.h"
#include "fonts/glcdfont.h"

#define TRANSPARENT_COLOR CYAN






/// Font data stored PER GLYPH
typedef struct {
	uint16_t bitmapOffset;      ///< Pointer into GFXfont->bitmap
	uint8_t  width;             ///< Bitmap dimensions in pixels
    uint8_t  height;            ///< Bitmap dimensions in pixels
	uint8_t  xAdvance;          ///< Distance to advance cursor (x axis)
	int8_t   xOffset;           ///< X dist from cursor pos to UL corner
    int8_t   yOffset;           ///< Y dist from cursor pos to UL corner
} GFXglyph;

/// Data stored for FONT AS A WHOLE
typedef struct { 
	uint8_t  *bitmap;       ///< Glyph bitmaps, concatenated
	GFXglyph *glyph;        ///< Glyph array
	uint8_t   first;        ///< ASCII extents (first char)
	uint8_t   last;         ///< ASCII extents (last char)
	uint8_t   yAdvance;     ///< Newline distance (y axis)
	uint8_t	  yOffset;
} GFXfont;




void LCD_write(unsigned char c);
void LCD_writeString(unsigned char *s);
void LCD_writeString2(unsigned char *string);

void LCD_setCursor(u16 x, u16 y);
void LCD_setTextSize(u8 size);
void LCD_setTextColor(u16 color);
void LCD_setTextBgColor(u16 color);

u16 LCD_getCursorX();
u16 LCD_getCursorY();
void LCD_setCursorX(u16 x);
void LCD_setCursorY(u16 y);

void LCD_setFont(const GFXfont *f);

#endif //TEST1_TEXT_H
