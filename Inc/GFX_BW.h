/*
 * GFX_BW.h
 *
 *  The MIT License.
 *  Created on: 25.05.2017
 *      Author: Mateusz Salamon
 *      www.msalamon.pl
 *      mateusz@msalamon.pl
 */

#ifndef GFX_BW_H_
#define GFX_BW_H_

/***************************************************************
 *
 * 		SETTINGS
 *
 * 		Please set what functionality you want to use.
 * 		Some functions need other functionalities. It should works automatically.
 *
 * 		1 - will be compiled
 * 		0 - won't be compiled
 *
 * */
#define AVR_USING 0
#define STM32_USING 1

#define GFX_DrawPixel(x,y,color) SSD1306_DrawPixel(x,y,color)
#define WIDTH SSD1306_LCDWIDTH
#define HEIGHT SSD1306_LCDHEIGHT
#define PIXEL_BLACK	BLACK
#define PIXEL_WHITE	WHITE
#define PIXEL_INVERSE	INVERSE

#define USING_STRINGS 1 // 0 - do not compile, 1 - compile

#define USING_IMAGE 1
#if USING_IMAGE == 1
#define USING_IMAGE_ROTATE 0
#endif

// Trygonometric graphic functions
#define USING_RECTANGLE 1
#define USING_CIRCLE 1
#define USING_FILL_CIRCLE 1
#define USING_ROUND_RECTANGLE 1
#define USING_FILL_ROUND_RECTANGLE 1
#define USING_TRIANGLE 1
#define USING_FILL_TRIANGLE 1
#if ((USING_FILL_ROUND_RECTANGLE == 0) && (USING_STRINGS == 0))
#define USING_FILL_RECTANGLE 0
#endif
#if (USING_RECTANGLE == 0) && (USING_FILL_RECTANGLE == 0) && (USING_FILL_CIRCLE == 0) && (USING_ROUND_RECTANGLE == 0) && (USING_TRIANGLE == 0) && (USING_FILL_TRIANGLE == 0)
#define USING_LINES 0
#endif

/****************************************************************/

#if (USING_FILL_ROUND_RECTANGLE == 1 || USING_STRINGS == 1)
#define USING_FILL_RECTANGLE 1
#endif
#if (USING_RECTANGLE == 1) || (USING_FILL_RECTANGLE == 1) || (USING_FILL_CIRCLE == 1) || (USING_ROUND_RECTANGLE == 1) || (USING_TRIANGLE == 1) || (USING_FILL_TRIANGLE == 1)
#define USING_LINES 1
#endif
#if USING_ROUND_RECTANGLE == 1
#define CIRCLE_HELPER
#endif
#if (USING_FILL_CIRCLE == 1) || (USING_FILL_ROUND_RECTANGLE == 1)
#define FILL_CIRCLE_HELPER
#endif

#if USING_STRINGS == 1
/*
 *
 */
void GFX_SetFont(const uint8_t* font_t);
void GFX_SetFontSize(uint8_t size_t);
uint8_t GFX_GetFontHeight(void);
uint8_t GFX_GetFontWidth(void);
uint8_t  GFX_GetFontSize(void);
void GFX_DrawChar(int x, int y, char chr, uint8_t color, uint8_t background);
void GFX_DrawString(int x, int y, char* str, uint8_t color, uint8_t background);
#endif

#if USING_LINES == 1
void GFX_DrawLine(int x_start, int y_start, int x_end, int y_end, uint8_t color);
#endif

#if USING_RECTANGLE == 1
void GFX_DrawRectangle(int x, int y, uint16_t w, uint16_t h, uint8_t color);
#endif
#if USING_FILL_RECTANGLE ==1
void GFX_DrawFillRectangle(int x, int y, uint16_t w, uint16_t h, uint8_t color);
#endif
#if USING_CIRCLE == 1
void GFX_DrawCircle(int x0, int y0, uint16_t r, uint8_t color);
#endif
#if USING_FILL_CIRCLE == 1
void GFX_DrawFillCircle(int x0, int y0, uint16_t r, uint8_t color);
#endif
#if USING_ROUND_RECTANGLE == 1
void GFX_DrawRoundRectangle(int x, int y, uint16_t w, uint16_t h, uint16_t r, uint8_t color);
#endif
#if USING_FILL_ROUND_RECTANGLE == 1
void GFX_DrawFillRoundRectangle(int x, int y, uint16_t w, uint16_t h, uint16_t r, uint8_t color);
#endif
#if USING_TRIANGLE == 1
void GFX_DrawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t color);
#endif
#if USING_FILL_TRIANGLE == 1
void GFX_DrawFillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t color);
#endif
#if USING_IMAGE == 1
#if AVR_USING ==1
void GFX_Image_P(int x, int y, uint8_t *img, uint8_t w, uint8_t h, uint8_t color);
#endif
#if STM32_USING ==1
void GFX_Image(int x, int y, const uint8_t *img, uint8_t w, uint8_t h, uint8_t color);
#if USING_IMAGE_ROTATE == 1
void GFX_ImageRotate(int x, int y, const uint8_t *img, uint8_t w, uint8_t h, uint8_t color, uint16_t angle);
#endif
#endif
#endif

#endif /* GFX_BW_H_ */
