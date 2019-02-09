/*
 * GFX_BW.h
 *
 *  The MIT License.
 *  Created on: 25.05.2017
 *      Author: Mateusz Salamon
 *      www.msalamon.pl
 *      mateusz@msalamon.pl
 */

#include "main.h"
#include "stm32l4xx_hal.h"

#include "OLED_SSD1306.h"
#include "../Inc/GFX_BW.h"

#if USING_LINES == 1
#include <stdlib.h> // for abs() function
#endif
#if USING_IMAGE_ROTATE == 1
#include <math.h>
#endif

#define _swap_int(a, b) { int t = a; a = b; b = t; }

#if  USING_STRINGS == 1
const uint8_t* font;
uint8_t size = 1;

void GFX_SetFont(const uint8_t* font_t)
{
	font = font_t;
}

void GFX_SetFontSize(uint8_t size_t)
{
	if(size_t != 0)
		size = size_t;
}

uint8_t GFX_GetFontHeight(void)
{
	return font[0];
}

uint8_t GFX_GetFontWidth(void)
{
	return font[1];
}

uint8_t GFX_GetFontSize(void)
{
	return size;
}

void GFX_DrawChar(int x, int y, char chr, uint8_t color, uint8_t background)
{
	if(chr > 0x7E) return; // chr > '~'

	for(uint8_t i=0; i<font[1]; i++ )
	{
        uint8_t line = (uint8_t)font[(chr-0x20) * font[1] + i + 2];

        for(int8_t j=0; j<font[0]; j++, line >>= 1)
        {
            if(line & 1)
            {
            	if(size == 1)
            		GFX_DrawPixel(x+i, y+j, color);
            	else
            		GFX_DrawFillRectangle(x+i*size, y+j*size, size, size, color);
            }
            else if(background == 0)
            {
            	if(size == 1)
					GFX_DrawPixel(x+i, y+j, background);
				else
					GFX_DrawFillRectangle(x+i*size, y+j*size, size, size, background);
            }
        }
    }
}

void GFX_DrawString(int x, int y, char* str, uint8_t color, uint8_t background)
{
	int x_tmp = x;
	char znak;
	znak = *str;
	while(*str++)
	{
		GFX_DrawChar(x_tmp, y, znak, color, background);
		x_tmp += ((uint8_t)font[1] * size) + 1;
		if(background == 0)
		{
			for(uint8_t i=0; i<(font[0]*size); i++)
			{
				GFX_DrawPixel(x_tmp-1, y+i, PIXEL_BLACK);
			}
		}
		znak = *str;
	}
}
#endif
#if USING_LINES == 1
void GFX_WriteLine(int x_start, int y_start, int x_end, int y_end, uint8_t color)
{
	int16_t steep = abs(y_end - y_start) > abs(x_end - x_start);

	    if (steep) {
	        _swap_int(x_start, y_start);
	        _swap_int(x_end, y_end);
	    }

	    if (x_start > x_end) {
	        _swap_int(x_start, x_end);
	        _swap_int(y_start, y_end);
	    }

	    int16_t dx, dy;
	    dx = x_end - x_start;
	    dy = abs(y_end - y_start);

	    int16_t err = dx / 2;
	    int16_t ystep;

	    if (y_start < y_end) {
	        ystep = 1;
	    } else {
	        ystep = -1;
	    }

	    for (; x_start<=x_end; x_start++) {
	        if (steep) {
	        	GFX_DrawPixel(y_start, x_start, color);
	        } else {
	        	GFX_DrawPixel(x_start, y_start, color);
	        }
	        err -= dy;
	        if (err < 0) {
	            y_start += ystep;
	            err += dx;
	        }
	    }
}

void GFX_DrawFastVLine(int x_start, int y_start, int h, uint8_t color)
{
	GFX_WriteLine(x_start, y_start, x_start, y_start+h-1, color);
}

void GFX_DrawFastHLine(int x_start, int y_start, int w, uint8_t color)
{
	GFX_WriteLine(x_start, y_start, x_start+w-1, y_start, color);
}

void GFX_DrawLine(int x_start, int y_start, int x_end, int y_end, uint8_t color)
{
	if(x_start == x_end){
	        if(y_start > y_end) _swap_int(y_start, y_end);
	        GFX_DrawFastVLine(x_start, y_start, y_end - y_start + 1, color);
	    } else if(y_start == y_end){
	        if(x_start > x_end) _swap_int(x_start, x_end);
	        GFX_DrawFastHLine(x_start, y_start, x_end - x_start + 1, color);
	    } else {

	    	GFX_WriteLine(x_start, y_start, x_end, y_end, color);
	    }
}
#endif
#if USING_RECTANGLE == 1
void GFX_DrawRectangle(int x, int y, uint16_t w, uint16_t h, uint8_t color)
{

    GFX_DrawFastHLine(x, y, w, color);
    GFX_DrawFastHLine(x, y+h-1, w, color);
    GFX_DrawFastVLine(x, y, h, color);
    GFX_DrawFastVLine(x+w-1, y, h, color);

}
#endif
#if USING_FILL_RECTANGLE == 1
void GFX_DrawFillRectangle(int x, int y, uint16_t w, uint16_t h, uint8_t color)
{
    for (int i=x; i<x+w; i++) {
    	GFX_DrawFastVLine(i, y, h, color);
    }

}
#endif
#if USING_CIRCLE == 1
void GFX_DrawCircle(int x0, int y0, uint16_t r, uint8_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    GFX_DrawPixel(x0  , y0+r, color);
    GFX_DrawPixel(x0  , y0-r, color);
    GFX_DrawPixel(x0+r, y0  , color);
    GFX_DrawPixel(x0-r, y0  , color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        GFX_DrawPixel(x0 + x, y0 + y, color);
        GFX_DrawPixel(x0 - x, y0 + y, color);
        GFX_DrawPixel(x0 + x, y0 - y, color);
        GFX_DrawPixel(x0 - x, y0 - y, color);
        GFX_DrawPixel(x0 + y, y0 + x, color);
        GFX_DrawPixel(x0 - y, y0 + x, color);
        GFX_DrawPixel(x0 + y, y0 - x, color);
        GFX_DrawPixel(x0 - y, y0 - x, color);
    }

}
#endif
#ifdef CIRCLE_HELPER
void GFX_DrawCircleHelper( int x0, int y0, uint16_t r, uint8_t cornername, uint8_t color)
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
            GFX_DrawPixel(x0 + x, y0 + y, color);
            GFX_DrawPixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            GFX_DrawPixel(x0 + x, y0 - y, color);
            GFX_DrawPixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            GFX_DrawPixel(x0 - y, y0 + x, color);
            GFX_DrawPixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            GFX_DrawPixel(x0 - y, y0 - x, color);
            GFX_DrawPixel(x0 - x, y0 - y, color);
        }
    }
}
#endif
#ifdef FILL_CIRCLE_HELPER
void GFX_DrawFillCircleHelper(int x0, int y0, uint16_t r, uint8_t cornername, int16_t delta, uint8_t color)
{

    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;

        if (cornername & 0x1) {
            GFX_DrawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
            GFX_DrawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
        }
        if (cornername & 0x2) {
            GFX_DrawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
            GFX_DrawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
        }
    }
}
#endif
#if USING_FILL_CIRCLE == 1
void GFX_DrawFillCircle(int x0, int y0, uint16_t r, uint8_t color)
{

	GFX_DrawFastVLine(x0, y0-r, 2*r+1, color);
    GFX_DrawFillCircleHelper(x0, y0, r, 3, 0, color);
}
#endif
#if USING_ROUND_RECTANGLE == 1
void GFX_DrawRoundRectangle(int x, int y, uint16_t w, uint16_t h, uint16_t r, uint8_t color)
{
	GFX_DrawFastHLine(x+r  , y    , w-2*r, color); // Top
    GFX_DrawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
    GFX_DrawFastVLine(x    , y+r  , h-2*r, color); // Left
    GFX_DrawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    GFX_DrawCircleHelper(x+r    , y+r    , r, 1, color);
    GFX_DrawCircleHelper(x+w-r-1, y+r    , r, 2, color);
    GFX_DrawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    GFX_DrawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}
#endif
#if USING_FILL_ROUND_RECTANGLE == 1
void GFX_DrawFillRoundRectangle(int x, int y, uint16_t w, uint16_t h, uint16_t r, uint8_t color)
{
    // smarter version

	GFX_DrawFillRectangle(x+r, y, w-2*r, h, color);

    // draw four corners
	GFX_DrawFillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
	GFX_DrawFillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}
#endif
#if USING_TRIANGLE == 1
void GFX_DrawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t color)
{
	GFX_DrawLine(x0, y0, x1, y1, color);
    GFX_DrawLine(x1, y1, x2, y2, color);
    GFX_DrawLine(x2, y2, x0, y0, color);
}
#endif
#if USING_FILL_TRIANGLE == 1
void GFX_DrawFillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t color)
{

    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
    	_swap_int(y0, y1); _swap_int(x0, x1);
    }
    if (y1 > y2) {
    	_swap_int(y2, y1); _swap_int(x2, x1);
    }
    if (y0 > y1) {
    	_swap_int(y0, y1); _swap_int(x0, x1);
    }

    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        GFX_DrawFastHLine(a, y0, b-a+1, color);
        return;
    }

    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
    int32_t
    sa   = 0,
    sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int(a,b);
        GFX_DrawFastHLine(a, y, b-a+1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int(a,b);
        GFX_DrawFastHLine(a, y, b-a+1, color);
    }
}
#endif
#if USING_IMAGE == 1
#if AVR_USING == 1
void GFX_Image_P(int x, int y, uint8_t *img, uint8_t w, uint8_t h, uint8_t color)
{
	uint8_t i, j, byteWidth = (w+7)/8;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			if(pgm_read_byte(img + j *byteWidth + i /8) & (128 >> (i&7)) )
				GFX_DrawPixel(x+i, y+j, color);
		}
	}
}
#endif
#if STM32_USING ==1
void GFX_Image(int x, int y, const uint8_t *img, uint8_t w, uint8_t h, uint8_t color)
{
	uint8_t i, j, byteWidth = (w+7)/8;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			if(img[j *byteWidth + i /8] & (128 >> (i&7)) )
				GFX_DrawPixel(x+i, y+j, color);
		}
	}
}
#if USING_IMAGE_ROTATE == 1

const double sinus_LUT[] =
{// 0* to 90* = 91 valuse
		0.0000,
		0.0175,
		0.0349,
		0.0523,
		0.0698,
		0.0872,
		0.1045,
		0.1219,
		0.1392,
		0.1564,
		0.1736,
		0.1908,
		0.2079,
		0.2250,
		0.2419,
		0.2588,
		0.2756,
		0.2924,
		0.3090,
		0.3256,
		0.3420,
		0.3584,
		0.3746,
		0.3907,
		0.4067,
		0.4226,
		0.4384,
		0.4540,
		0.4695,
		0.4848,
		0.5000,
		0.5150,
		0.5299,
		0.5446,
		0.5592,
		0.5736,
		0.5878,
		0.6018,
		0.6157,
		0.6293,
		0.6428,
		0.6561,
		0.6691,
		0.6820,
		0.6947,
		0.7071,
		0.7193,
		0.7314,
		0.7431,
		0.7547,
		0.7660,
		0.7771,
		0.7880,
		0.7986,
		0.8090,
		0.8192,
		0.8290,
		0.8387,
		0.8480,
		0.8572,
		0.8660,
		0.8746,
		0.8829,
		0.8910,
		0.8988,
		0.9063,
		0.9135,
		0.9205,
		0.9272,
		0.9336,
		0.9397,
		0.9455,
		0.9511,
		0.9563,
		0.9613,
		0.9659,
		0.9703,
		0.9744,
		0.9781,
		0.9816,
		0.9848,
		0.9877,
		0.9903,
		0.9925,
		0.9945,
		0.9962,
		0.9976,
		0.9986,
		0.9994,
		0.9998,
		1.0000,
};

double sinus(uint16_t angle)
{
	angle %= 360;
	if((angle >= 0) && (angle < 90)) return sinus_LUT[angle];
	if((angle >= 90) && (angle < 180)) return sinus_LUT[180 - angle];
	if((angle >= 180) && (angle < 270)) return -(sinus_LUT[angle - 180]);
	if((angle >= 270) && (angle < 360)) return -(sinus_LUT[180 - (angle - 180)]);
	return 0; // will be never here
}

void GFX_ImageRotate(int x, int y, const uint8_t *img, uint8_t w, uint8_t h, uint8_t color, uint16_t angle)
{
	angle %= 360;

	uint8_t wHalf = w/2;
	uint8_t hHalf = h/2;

	uint8_t i, j, byteWidth = (w+7)/8;

	double sinma = sinus(angle);
	double cosma = sinus(angle + 90);

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			if(img[j *byteWidth + i /8] & (128 >> (i&7)) )
			{
				int xt = i - wHalf;
				int yt = j - hHalf;

				int xs = (xt*cosma - yt*sinma) + wHalf;
				int ys = (xt*sinma + yt*cosma) + hHalf;

				GFX_DrawPixel(xs+x, ys+y, color);
			}
		}
	}
}
#endif
#endif
#endif

