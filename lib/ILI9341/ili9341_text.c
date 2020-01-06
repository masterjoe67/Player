#include "ili9341_text.h"
#pragma GCC push_options
#pragma GCC optimize ("O0")

#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))

u8 _cp437 = 0;

u16 cursorX = 0, cursorY = 0;

u8 textSize = 1;

u8 wrap = 1;

u16 textColor = RED, textBgColor = TRANSPARENT_COLOR;

GFXglyph * pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c)
{
	return &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
}

uint8_t * pgm_read_bitmap_ptr(const GFXfont *gfxFont)
{
	return (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);
}


GFXfont *gfxFont;         ///< Pointer to special font

inline static void LCD_drawChar(u16 x0,
	u16 y0,
	unsigned char c,
	u16 color,
	u16 bg,
	uint8_t size)
{
	u16 scaledWidth = (u16)(size * 6), doubleScaledWidth = scaledWidth * size;

	u16 x1 = (u16)(x0 + scaledWidth - 1), y1 = (u16)(y0 + 8 * size - 1);

	u16 doubleSize = size * size;
	u16 count = (u16)(48 * doubleSize);

	u16 charPixels[count];

	u16 mx, my;
	s8 i, j, sx, sy;
	u8 line;
	u16 pixelColor;

	if (x0 >= LCD_getWidth() || y0 >= LCD_getHeight() || x1 < 0 || y1 < 0)
		return;

	if (!_cp437 && (c >= 176))
		c++;  // Handle 'classic' charset behavior

	u16 characterNumber = (u16)(c * 5);

	if (bg == TRANSPARENT_COLOR)
	{
		LCD_readPixels(x0, y0, x1, y1, charPixels);
	}

	LCD_setAddressWindowToWrite(x0, y0, x1, y1);

	for (i = 0; i < 6; i++)
	{
		line = (u8)(i < 5 ? pgm_read_byte(font + characterNumber + i) : 0x0);
		my = (u16)(i * size);

		for (j = 0; j < 8; j++, line >>= 1)
		{
			mx = (u16)(j * doubleScaledWidth);

			pixelColor = line & 0x1 ? color : bg;

			if (pixelColor == TRANSPARENT_COLOR)
				continue;

			for (sx = 0; sx < size; ++sx)
			{
				for (sy = 0; sy < size; ++sy)
				{
					charPixels[mx + my + sy * scaledWidth + sx] = pixelColor;
				}
			}
		}
	}

	dev_spi_set16(m_spi);
	ili9341_sendData16(charPixels, count);
	dev_spi_set8(m_spi);
}


void LCD_setFont(const GFXfont *f) {
	if (f) {
		gfxFont = (GFXfont *)f;
		cursorY = (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
		//cursorY += 6;
	}
	
}

uint16_t xpos = 0;

uint16_t ypos = 0;

inline static void LCD_drawCharC(uint16_t x, uint16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y)
{
	c -= (uint8_t)pgm_read_byte(&gfxFont->first);
	GFXglyph *glyph  = pgm_read_glyph_ptr(gfxFont, c);
	uint8_t  *bitmap = pgm_read_bitmap_ptr(gfxFont);

	uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
	uint8_t  w  = pgm_read_byte(&glyph->width),
	         h  = pgm_read_byte(&glyph->height);
	int8_t  xo = pgm_read_byte(&glyph->xOffset),
	         yo = pgm_read_byte(&glyph->yOffset);
	uint8_t  xx, yy, bits = 0, bit = 0;
	int16_t  xo16 = 0, yo16 = 0;

	if (size_x > 1 || size_y > 1) {
		xo16 = xo;
		yo16 = yo;
	}

	// Todo: Add character clipping here

	// NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
	// THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
	// has typically been used with the 'classic' font to overwrite old
	// screen contents with new data.  This ONLY works because the
	// characters are a uniform size; it's not a sensible thing to do with
	// proportionally-spaced fonts with glyphs of varying sizes (and that
	// may overlap).  To replace previously-drawn text when using a custom
	// font, use the getTextBounds() function to determine the smallest
	// rectangle encompassing a string, erase the area with fillRect(),
	// then draw new text.  This WILL infortunately 'blink' the text, but
	// is unavoidable.  Drawing 'background' pixels will NOT fix this,
	// only creates a new set of problems.  Have an idea to work around
	// this (a canvas object type for MCUs that can afford the RAM and
	// displays supporting setAddrWindow() and pushColors()), but haven't
	// implemented this yet.


	for(yy = 0 ; yy < h ; yy++) {
		for (xx = 0; xx < w; xx++) {
			if (!(bit++ & 7)) {
				bits = pgm_read_byte(&bitmap[bo++]);
			}
			if (bits & 0x80) {
				if (size_x == 1 && size_y == 1) {
					xpos = (uint16_t)x;
					xpos += (int16_t)xo;
					xpos += (uint16_t)xx;
					ypos = (uint16_t)y;
					ypos += (int16_t)yo;
					ypos += (uint16_t)yy;
					//LCD_putPixel(x + xo + xx, y + yo + yy, color);
					LCD_putPixel(xpos, ypos, color);
				}
				else {
					LCD_fillRect(x+(xo16 + xx)*size_x,
						y+(yo16 + yy)*size_y,
						size_x,
						size_y,
						color);
				}
			}
			bits <<= 1;
		}
	}

}


inline void LCD_write(unsigned char c)
{
	/*if (c == '\n')
	{
		cursorY += textSize * 8;
		cursorX = 0;
	}
	else if (c == '\r')
	{
		cursorX = 0;
	}
	else
	{
		if (wrap && ((cursorX + textSize * 6) >= LCD_getWidth()))
		{ // Heading off edge?
			cursorX = 0;            // Reset x to zero
			cursorY += textSize * 8; // Advance y one line
		}
		//LCD_drawChar(cursorX, cursorY, c, textColor, textBgColor, textSize);
		LCD_drawCharC(cursorX, cursorY, c, textColor, textBgColor, 1, 1);
		cursorX += textSize * 6;
	}*/
	
	if (c == '\n') {
		cursorX  = 0;
		cursorY += (int16_t)textSize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
	}
	else if (c != '\r') {
		uint8_t first = pgm_read_byte(&gfxFont->first);
		if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
			GFXglyph *glyph  = pgm_read_glyph_ptr(gfxFont, c - first);
			uint8_t   w     = pgm_read_byte(&glyph->width),
			          h     = pgm_read_byte(&glyph->height);
			if ((w > 0) && (h > 0)) {
				// Is there an associated bitmap?
			   int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset);   // sic
			   if(wrap && ((cursorX + textSize * (xo + w)) > LCD_getWidth())) {
					cursorX  = 0;
					cursorY += (int16_t)textSize *
					  (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
				}
				LCD_drawCharC(cursorX, cursorY, c, textColor, textBgColor, textSize, textSize);
			}
			cursorX += (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textSize;
		}
	}
}

void LCD_writeString(unsigned char *s)
{
	while (*(s))
		LCD_write(*s++);
}

void LCD_setCursor(u16 x, u16 y)
{
	cursorX = x;
	cursorY = y;
}

void LCD_setTextSize(u8 size)
{
	textSize = size;
}

void LCD_setTextColor(u16 color)
{
	textColor = color;
}

void LCD_setTextBgColor(u16 color)
{
	textBgColor = color;
}

u16 LCD_getCursorX()
{
	return cursorX;
}

u16 LCD_getCursorY()
{
	return cursorY;
}

#pragma GCC pop_options