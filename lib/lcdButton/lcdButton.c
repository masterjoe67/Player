
#include "lcdButton.h"
#include <string.h>
//#include "ILI9341_Driver.h"
#include "ili9341_graph.h"
#include "ili9341_text.h"
/**************************************************************************/
/*!
   @brief    Initialize button with our desired color/size/settings
   @param    gfx     Pointer to our display so we can draw to it!
   @param    x       The X coordinate of the center of the button
   @param    y       The Y coordinate of the center of the button
   @param    w       Width of the buttton
   @param    h       Height of the buttton
   @param    outline  Color of the outline (16-bit 5-6-5 standard)
   @param    fill  Color of the button fill (16-bit 5-6-5 standard)
   @param    textcolor  Color of the button label (16-bit 5-6-5 standard)
   @param    label  Ascii string of the text inside the button
   @param    textsize_x The font magnification in X-axis of the label text
   @param    textsize_y The font magnification in Y-axis of the label text
*/
/**************************************************************************/
// Classic initButton() function: pass center & size
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
	uint8_t textsize_x,
	uint8_t textsize_y)
{
	// Tweak arguments and pass to the newer initButtonUL() function...
	BTN_initUL(
		btn,
		x - (w / 2),
		y - (h / 2),
		w,
		h,
		outline,
		fill,
		textcolor,
		label,
		textsize_x,
		textsize_y);
}


/**************************************************************************/
/*!
   @brief    Initialize button with our desired color/size/settings, with upper-left coordinates
   @param    gfx     Pointer to our display so we can draw to it!
   @param    x1       The X coordinate of the Upper-Left corner of the button
   @param    y1       The Y coordinate of the Upper-Left corner of the button
   @param    w       Width of the buttton
   @param    h       Height of the buttton
   @param    outline  Color of the outline (16-bit 5-6-5 standard)
   @param    fill  Color of the button fill (16-bit 5-6-5 standard)
   @param    textcolor  Color of the button label (16-bit 5-6-5 standard)
   @param    label  Ascii string of the text inside the button
   @param    textsize_x The font magnification in X-axis of the label text
   @param    textsize_y The font magnification in Y-axis of the label text
*/
/**************************************************************************/
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
	uint8_t textsize_y)
{
	btn->_x1    				= x1;
	btn->_y1				= y1;
	btn->_w				= w;
	btn->_h				= h;
	btn->_outlinecolor	= outline;
	btn->_fillcolor		= fill;
	btn->_textcolor		= textcolor;
	btn->_textsize_x		= textsize_x;
	btn->_textsize_y		= textsize_y;
	strncpy(btn->_label, label, 9);
}

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
	const GFXfont *f)
{
	btn->_x1    				= x1;
	btn->_y1				= y1;
	btn->_w				= w;
	btn->_h				= h;
	btn->_outlinecolor	= outline;
	btn->_fillcolor		= fill;
	btn->_textcolor		= textcolor;
	btn->_textsize		= textsize;
	btn->_f				= f;
	strncpy(btn->_label, label, 9);
}

void BTN_icon_initUL(
	BTN_Icon_TypeDef* btn,
	int16_t x1,
	int16_t y1,
	uint16_t w,
	uint16_t h,
	uint16_t* bitmap)
{
	btn->_x1    	= x1;
	btn->_y1		= y1;
	btn->_w			= w;
	btn->_h			= h;
	btn->bitmap		= bitmap;
}

/**************************************************************************/
/*!
   @brief    Draw the button on the screen
   @param    inverted Whether to draw with fill/text swapped to indicate 'pressed'
*/
/**************************************************************************/
void BTN_draw(BTN_TypeDef* btn, bool inverted) {
	
	uint16_t fill, outline, text;

	if (!inverted) {
		fill    = btn->_fillcolor;
		outline = btn->_outlinecolor;
		text    = btn->_textcolor;
	}
	else {
		fill    = btn->_textcolor;
		outline = btn->_outlinecolor;
		text    = btn->_fillcolor;
	}

	uint8_t r = min(btn->_w, btn->_h) / 4;    // Corner radius
	LCD_fillRoundRect(btn->_x1, btn->_y1, btn->_w, btn->_h, r, fill);
	LCD_drawRoundRect(btn->_x1, btn->_y1, btn->_w, btn->_h, r, outline);

	if (btn->_label[0] != 0)
	{
		LCD_setCursor(btn->_x1 + (btn->_w / 2) - (strlen(btn->_label) * 3 * btn->_textsize_x),
			btn->_y1 + (btn->_h / 2) - (4 * btn->_textsize_y));
		LCD_setTextColor(text);
		LCD_setTextSize(btn->_textsize_x);
		LCD_setTextBgColor(fill);
		LCD_writeString((unsigned char*)btn->_label);
		//_gfx->print(_label);
	}

}

void BTN_Label_draw(BTN_Label_TypeDef* btn) {
	
	uint16_t fill, outline, text;

	
	text    = btn->_textcolor;
	outline = btn->_outlinecolor;
	fill    = btn->_fillcolor;
	

	uint8_t r = min(btn->_w, btn->_h) / 4;     // Corner radius
	LCD_fillRoundRect(btn->_x1, btn->_y1, btn->_w, btn->_h, r, fill);
	LCD_drawRoundRect(btn->_x1, btn->_y1, btn->_w, btn->_h, r, outline);

	if (btn->_label[0] != 0)
	{
		LCD_setFont(btn->_f);
		uint8_t fontX = btn->_f->glyph->xAdvance;
		uint8_t fontY = btn->_f->yOffset;		
		uint8_t ypos = btn->_y1 + (btn->_h / 2) + fontY;
		
		LCD_setCursor(btn->_x1 + (btn->_w / 2) - ((strlen(btn->_label) * fontX * btn->_textsize) / 2), ypos);
		LCD_setTextColor(text);
		LCD_setTextSize(btn->_textsize);
		LCD_setTextBgColor(fill);
		LCD_writeString((unsigned char*)btn->_label);

	}

}

void BTN_icon_draw(BTN_Icon_TypeDef* btn) {
	
	LCD_drawRGBBitmap(btn->_x1, btn->_y1, (uint16_t *)btn->bitmap, btn->_w, btn->_h);

}

/**************************************************************************/
/*!
    @brief    Helper to let us know if a coordinate is within the bounds of the button
    @param    x       The X coordinate to check
    @param    y       The Y coordinate to check
    @returns  True if within button graphics outline
*/
/**************************************************************************/
bool BTN_contains(BTN_TypeDef* btn, int16_t x, int16_t y) {
	return ((x >= btn->_x1) && (x < (int16_t)(btn->_x1 + btn->_w)) &&
	        (y >= btn->_y1) && (y < (int16_t)(btn->_y1 + btn->_h)));
}

bool BTN_isPressed(BTN_TypeDef* btn, int16_t x, int16_t y)
{
	return ((x >= btn->_x1) && (x < (int16_t)(btn->_x1 + btn->_w)) &&
          (y >= btn->_y1) && (y < (int16_t)(btn->_y1 + btn->_h)));
}

/**************************************************************************/
/*!
   @brief    Query whether the button was pressed since we last checked state
   @returns  True if was not-pressed before, now is.
*/
/**************************************************************************/
bool BTN_justPressed(BTN_TypeDef* btn) { return (btn->currstate && !btn->laststate); }

/**************************************************************************/
/*!
   @brief    Query whether the button was released since we last checked state
   @returns  True if was pressed before, now is not.
*/
/**************************************************************************/
bool BTN_justReleased(BTN_TypeDef* btn) { return (!btn->currstate && btn->laststate); }