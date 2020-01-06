#include "ili9341_graph.h"
#pragma GCC push_options
#pragma GCC optimize ("O0")

void LCD_readPixels(u16 x1, u16 y1, u16 x2, u16 y2, u16 *buf)
{
	u8 red, green, blue;
	u32 count = (u32)((x2 - x1 + 1) * (y2 - y1 + 1));

	LCD_setAddressWindowToRead(x1, y1, x2, y2);

	ILI9341_DC_SET;

	dev_spi_start(m_spi);
	ili9341_receiveDataCont8(&red);

	for (u32 i = 0; i < count; ++i)
	{
		ili9341_receiveDataCont8(&red);
		ili9341_receiveDataCont8(&green);
		ili9341_receiveDataCont8(&blue);

		buf[i] = (u16) ILI9341_COLOR(red, green, blue);
	}
	dev_spi_stop(m_spi);
}
u32 count = 0;
inline void LCD_fillRect(u16 x1, u16 y1, u16 w, u16 h, u16 color)
{
	count = w * h;
	LCD_setAddressWindowToWrite(x1, y1, (u16)(x1 + w - 1), (u16)(y1 + h - 1));
	dev_spi_set16(m_spi);
	ili9341_fill16(color, count);
	dev_spi_set8(m_spi);
}

void LCD_fillScreen(u16 color){
	u16 sx = LCD_getWidth();
	u16 sy = LCD_getHeight();
	
	LCD_fillRect(0, 0, sx, sy, color);
}

inline void LCD_drawFastHLine(u16 x0, u16 y0, u16 w, u16 color)
{
	LCD_setAddressWindowToWrite(x0, y0, x0 + w - 1, y0);
	dev_spi_set16(m_spi);
	ILI9341_DC_SET;
	dev_spi_start(m_spi);
	dev_spi_sendCircular16(m_spi, &color, w);
	dev_spi_wait(m_spi);
	dev_spi_stop(m_spi);
	dev_spi_set8(m_spi);
	/*if (w == 1)
	{
		LCD_putPixel(x0, y0, color);
		return;
	}
	LCD_fillRect(x0, y0, w, 1, color);*/
}

inline void LCD_putPixel(u16 x, u16 y, u16 color)
{
	LCD_setAddressWindowToWrite(x, y, x, y);
	
	dev_spi_set16(m_spi);
	ili9341_fill16(color, 1);
	dev_spi_set8(m_spi);
}

inline static void LCD_putPixelCont(u16 x, u16 y, u16 color)
{
	LCD_setAddressWindowToWrite(x, y, x, y);

	dev_spi_set16(m_spi);
	ili9341_fill16(color, 1);
}

inline void LCD_drawFastVLine(u16 x0, u16 y0, u16 h, u16 color)
{
	LCD_setAddressWindowToWrite(x0, y0, x0, y0 + h - 1);
	dev_spi_set16(m_spi);
	ILI9341_DC_SET;
	dev_spi_start(m_spi);
	dev_spi_sendCircular16(m_spi, &color, h);
	dev_spi_wait(m_spi);
	dev_spi_stop(m_spi);
	dev_spi_set8(m_spi);
	/*if (h == 1)
	{
		LCD_putPixel(x0, y0, color);
		return;
	}
	LCD_fillRect(x0, y0, 1, h, color);*/
}

void LCD_drawCircle(u16 x0, u16 y0, u16 r, u16 color)
{
	if (r == 0)
	{
		LCD_putPixel(x0, y0, color);
		return;
	}

	s16 f = (s16) (1 - r), dx = 1, dy = (s16) (-2 * r), x = 0;

	u16 y = r;

	dev_spi_set16(m_spi);

	LCD_putPixelCont(x0, y0 + r, color);
	LCD_putPixelCont(x0, y0 - r, color);
	LCD_putPixelCont(x0 + r, y0, color);
	LCD_putPixelCont(x0 - r, y0, color);

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			dy += 2;
			f += dy;
		}
		x++;
		dx += 2;
		f += dx;

		LCD_putPixelCont(x0 + x, y0 + y, color);
		LCD_putPixelCont(x0 - x, y0 + y, color);
		LCD_putPixelCont(x0 + x, y0 - y, color);
		LCD_putPixelCont(x0 - x, y0 - y, color);
		LCD_putPixelCont(x0 + y, y0 + x, color);
		LCD_putPixelCont(x0 - y, y0 + x, color);
		LCD_putPixelCont(x0 + y, y0 - x, color);
		LCD_putPixelCont(x0 - y, y0 - x, color);
	}

	dev_spi_set8(m_spi);
}

// Used to do circles and roundrects
void LCD_fillCircleHelper(u16 x0, u16 y0, u16 r, u8 cornername, s16 delta, u16 color)
{
	if (r == 0)
		return;
	if (r == 1)
	{
		LCD_putPixel(x0, y0, color);
		return;
	}
	s16 f = (s16) (1 - r), dx = 1, dy = (s16) (-2 * r), x = 0;

	u16 y = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			dy += 2;
			f += dy;
		}
		x++;
		dx += 2;
		f += dx;

		if (cornername & 0x1)
		{
			LCD_drawFastVLine(x0 + x, y0 - y, (u16) (2 * y + 1 + delta), color);
			LCD_drawFastVLine(x0 + y, y0 - x, (u16) (2 * x + 1 + delta), color);
		}
		if (cornername & 0x2)
		{
			LCD_drawFastVLine(x0 - x, y0 - y, (u16) (2 * y + 1 + delta), color);
			LCD_drawFastVLine(x0 - y, y0 - x, (u16) (2 * x + 1 + delta), color);
		}
	}
}

void LCD_fillCircle(u16 x0, u16 y0, u16 r, u16 color)
{
	LCD_drawFastVLine(x0, y0 - r, (u16) (2 * r + 1), color);
	LCD_fillCircleHelper(x0, y0, r, 3, 0, color);
}

void LCD_drawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 color)
{
	s16 Dx = (s16) abs(x1 - x0), Dy = (s16) abs(y1 - y0);

	if (Dx == 0 && Dy == 0)
	{
		LCD_putPixel(x0, y0, color);
		return;
	}

	s16 steep = Dy > Dx;
	s16 dx, dy, err, yStep;

	if (steep)
	{
		_int16_swap(x0, y0);
		_int16_swap(x1, y1);
	}

	if (x0 > x1)
	{
		_int16_swap(x0, x1);
		_int16_swap(y0, y1);
	}

	dx = x1 - x0;
	dy = (s16) abs(y1 - y0);

	err = (s16) (dx / 2);

	if (y0 < y1)
	{
		yStep = 1;
	}
	else
	{
		yStep = -1;
	}

	for (; x0 <= x1; x0++)
	{
		if (steep)
		{
			LCD_putPixel(y0, x0, color);
		}
		else
		{
			LCD_putPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0)
		{
			y0 += yStep;
			err += dx;
		}
	}
}

void LCD_drawRect(u16 x, u16 y, u16 w, u16 h, u16 color)
{
	if (w == 0 || h == 0)
		return;
	if (w == 1)
	{
		LCD_drawFastVLine(x, y, h, color);
		return;
	}
	if (h == 1)
	{
		LCD_drawFastHLine(x, y, w, color);
		return;
	}
	LCD_drawFastHLine(x, y, w, color);
	LCD_drawFastHLine(x, (u16) (y + h - 1), w, color);
	LCD_drawFastVLine(x, y, h, color);
	LCD_drawFastVLine((u16) (x + w - 1), y, h, color);
}

void LCD_setVerticalScrolling(u16 startY, u16 endY)
{
	ili9341_sendCmd(LCD_VSCRDEF);
	u16 d[] =
	{ startY, (u16)(LCD_PIXEL_HEIGHT - startY - endY), endY };
	dev_spi_set16(m_spi);
	ili9341_sendData16(d, 3);
	dev_spi_set8(m_spi);
}

void LCD_scroll(u16 v)
{
	ili9341_sendCmd(LCD_VSCRSADD);
	dev_spi_set16(m_spi);
	ili9341_sendData16(&v, 1);
	dev_spi_set8(m_spi);
}

/**************************************************************************/
/*!
   @brief   Draw a PROGMEM-resident 16-bit image (RGB 5/6/5) at the specified (x,y) position.
   For 16-bit display devices; no color reduction performed.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with 16-bit color bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
*/
/**************************************************************************/
void LCD_drawRGBBitmap(u16 x, u16 y, const u16 bitmap[], u16 w, u16 h) {
	u16 *ptr = bitmap;
	u32 n = w * h;	
	LCD_setAddressWindowToWrite(x, y, x + w - 1, y + h - 1);
	dev_spi_set16(m_spi);
	ILI9341_DC_SET;
	dev_spi_start(m_spi);
	ili9341_sendData16(ptr, n);
	dev_spi_wait(m_spi);
	dev_spi_stop(m_spi);
	dev_spi_set8(m_spi);
	
	/*dev_spi_set16(m_spi);
		
	for (u16 j = 0; j < h; j++, y++) {
		for (int16_t i = 0; i < w; i++) {
			LCD_putPixelCont(x + i, y, bitmap[j * w + i]);

		}
	}
		dev_spi_set8(m_spi);*/
}


/**************************************************************************/
/*!
   @brief   Draw a rounded rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    r   Radius of corner rounding
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void LCD_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
	int16_t max_radius = ((w < h) ? w : h) / 2;   // 1/2 minor axis
	if(r > max_radius) r = max_radius;
	// smarter version
	LCD_drawFastHLine(x + r, y, w - 2*r, color);     // Top
	LCD_drawFastHLine(x + r, y + h - 1, w - 2*r, color);     // Bottom
	LCD_drawFastVLine(x, y + r, h - 2*r, color);     // Left
	LCD_drawFastVLine(x + w - 1, y + r, h - 2*r, color);     // Right
	// draw four corners
	LCD_drawCircleHelper(x + r, y + r, r, 1, color);
	LCD_drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
	LCD_drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
	LCD_drawCircleHelper(x + r, y + h - r - 1, r, 8, color);

}

/**************************************************************************/
/*!
    @brief    Quarter-circle drawer, used to do circles and roundrects
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    cornername  Mask bit #1 or bit #2 to indicate which quarters of the circle we're doing
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void LCD_drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x4) {
			LCD_putPixel(x0 + x, y0 + y, color);
			LCD_putPixel(x0 + y, y0 + x, color);
		}
		if (cornername & 0x2) {
			LCD_putPixel(x0 + x, y0 - y, color);
			LCD_putPixel(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8) {
			LCD_putPixel(x0 - y, y0 + x, color);
			LCD_putPixel(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1) {
			LCD_putPixel(x0 - y, y0 - x, color);
			LCD_putPixel(x0 - x, y0 - y, color);
		}
	}
}

void LCD_fillRoundRect(u16 x, u16 y, u16 w, u16 h, u16 r, u16 color) {
	int16_t max_radius = ((w < h) ? w : h) / 2;    // 1/2 minor axis
	if(r > max_radius) r = max_radius;
	
	//u16 x1 = x + r;
	//u16 x2 = w - 2*r;
	//u16 y2 = h;
	
	LCD_fillRect(x + r, y, w - 2*r, h, color);

	// draw four corners
	LCD_fillCircleHelper((u16)(x + w - r - 1), (u16)(y + r), (u16)r, 1, (u16)(h - 2*r - 1), color);
	LCD_fillCircleHelper(x + r, y + r, r, 2, h - 2*r - 1, color);
	
}

#pragma GCC pop_options




