/*
 * OLED_SSD1306.c
 *
 *  The MIT License.
 *  Created on: 27.01.2017
 *      Author: Mateusz Salamon
 *      www.msalamon.pl
 *      mateusz@msalamon.pl
 */

#include "main.h"
#include "stm32l4xx_hal.h"
#if defined(SSD1306_RESET_USE) | !defined(SPI_CS_HARDWARE_CONTROL)
#include "gpio.h"
#endif
#include "OLED_SSD1306.h"
#ifdef SSD1306_I2C_CONTROL
#include "i2c.h"
#endif
#ifdef SSD1306_SPI_CONTROL
#include "spi.h"
#endif
#include "string.h"

#ifdef SSD1306_I2C_CONTROL
I2C_HandleTypeDef *ssd1306_i2c;
#endif
#ifdef SSD1306_SPI_CONTROL
SPI_HandleTypeDef *ssd1306_spi;
#endif
static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8];

void SSD1306_Command(uint8_t com)
{
#ifdef SSD1306_I2C_CONTROL
	// I2C
	HAL_I2C_Mem_Write(ssd1306_i2c, SSD1306_I2C_ADDRESS, 0x00, 1, &com, sizeof(com), 100);
#endif
#ifdef SSD1306_SPI_CONTROL
	//SPI
	HAL_GPIO_WritePin(SSD1306_DC_GPIO_Port, SSD1306_DC_Pin, GPIO_PIN_RESET);
#ifndef SPI_CS_HARDWARE_CONTROL
	HAL_GPIO_WritePin(SSD1306_CS_GPIO_Port, SSD1306_CS_Pin, GPIO_PIN_RESET);
#endif
	HAL_SPI_Transmit(ssd1306_spi, &com, 1, 10);
#ifndef SPI_CS_HARDWARE_CONTROL
	HAL_GPIO_WritePin(SSD1306_CS_GPIO_Port, SSD1306_CS_Pin, GPIO_PIN_SET);
#endif
#endif
}

void SSD1306_Data(uint8_t dat)
{
#ifdef SSD1306_I2C_CONTROL
	// I2C
	HAL_I2C_Mem_Write(ssd1306_i2c, SSD1306_I2C_ADDRESS, 0x40, 1, &dat, sizeof(dat), 100);
#endif
#ifdef SSD1306_SPI_CONTROL
	// SPI
	HAL_GPIO_WritePin(SSD1306_DC_GPIO_Port, SSD1306_DC_Pin, GPIO_PIN_SET);
#ifndef SPI_CS_HARDWARE_CONTROL
	HAL_GPIO_WritePin(SSD1306_CS_GPIO_Port, SSD1306_CS_Pin, GPIO_PIN_RESET);
#endif
	HAL_SPI_Transmit(ssd1306_spi, &dat, 1, 10);
#ifndef SPI_CS_HARDWARE_CONTROL
	HAL_GPIO_WritePin(SSD1306_CS_GPIO_Port, SSD1306_CS_Pin, GPIO_PIN_SET);
#endif
#endif
}

#if defined(SSD1306_SPI_CONTROL) || defined(SSD1306_RESET_USE)
void SSD1306_Reset(void)
{
	HAL_GPIO_WritePin(SSD1306_RESET_GPIO_Port, SSD1306_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(SSD1306_RESET_GPIO_Port, SSD1306_RESET_Pin, GPIO_PIN_SET);
}
#endif

//
// Configuration functions
//
void SSD1306_InvertColors(uint8_t Invert)
{
	SSD1306_Command(Invert ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
}

void SSD1306_RotateDisplay(uint8_t Rotate)
{
	if(Rotate > 1) Rotate = 1;

	SSD1306_Command(0xA0 | (0x01 & Rotate));  // Set Segment Re-Map Default
							// 0xA0 (0x00) => column Address 0 mapped to 127
                			// 0xA1 (0x01) => Column Address 127 mapped to 0

	SSD1306_Command(0xC0 | (0x08 & (Rotate<<3)));  // Set COM Output Scan Direction
							// 0xC0	(0x00) => normal mode (RESET) Scan from COM0 to COM[N-1];Where N is the Multiplex ratio.
							// 0xC8	(0xC8) => remapped mode. Scan from COM[N-1] to COM0;;Where N is the Multiplex ratio.
}

void SSD1306_DisplayON(uint8_t On)
{
	SSD1306_Command(On ? SSD1306_DISPLAYON : SSD1306_DISPLAYOFF);
}

void SSD1306_SetContrast(uint8_t Contrast)
{
	SSD1306_Command(0x81);	// Set Contrast Control
	SSD1306_Command(Contrast);
}

#if GRAPHIC_ACCELERATION_COMMANDS == 1
//
// Graphic Acceleration Command
//
void SSD1306_StartScrollRight(uint8_t StartPage, uint8_t EndPage, uint8_t Speed)
{
	SSD1306_Command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
	SSD1306_Command(0x00);
	SSD1306_Command(StartPage);
	SSD1306_Command(Speed);
	SSD1306_Command(EndPage);
	SSD1306_Command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_StartScrollLeft(uint8_t StartPage, uint8_t EndPage, uint8_t Speed)
{
	SSD1306_Command(SSD1306_LEFT_HORIZONTAL_SCROLL);
	SSD1306_Command(0x00);
	SSD1306_Command(StartPage);
	SSD1306_Command(Speed);
	SSD1306_Command(EndPage);
	SSD1306_Command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_ScrollingHorVer(uint8_t Direction, uint8_t StartPage, uint8_t EndPage, uint8_t HorizontalSpeed, uint8_t VerticalOffset)
{
	if(VerticalOffset > 127) VerticalOffset = 127;
	SSD1306_Command(Direction);
	SSD1306_Command(0x00);
	SSD1306_Command(StartPage);
	SSD1306_Command(HorizontalSpeed);
	SSD1306_Command(EndPage);
	SSD1306_Command(VerticalOffset);
	SSD1306_Command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_StartScrollLeftUp(uint8_t StartPage, uint8_t EndPage, uint8_t HorizontalSpeed, uint8_t VerticalOffset)
{
	SSD1306_ScrollingHorVer(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL, StartPage, EndPage, HorizontalSpeed, VerticalOffset);
}

void SSD1306_StartScrollRightUp(uint8_t StartPage, uint8_t EndPage, uint8_t HorizontalSpeed, uint8_t VerticalOffset)
{
	SSD1306_ScrollingHorVer(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL, StartPage, EndPage, HorizontalSpeed, VerticalOffset);
}

void SSD1306_StopScroll(void)
{
	SSD1306_Command(SSD1306_DEACTIVATE_SCROLL);
}
#endif

#if ADVANCED_GRAPHIC_COMMANDS == 1
//
// Advanced Graphic Commands
//
void SSD1306_StartFadeOut(uint8_t Interval)
{
	SSD1306_Command(SSD1306_FADE_OUT);
	SSD1306_Command(0x20 | (0x0F & Interval));
}

void SSD1306_StartBlinking(uint8_t Interval)
{
	SSD1306_Command(SSD1306_FADE_OUT);
	SSD1306_Command(0x30 | (0x0F & Interval));
}

void SSD1306_StopFadeOutOrBlinking(void)
{
	SSD1306_Command(SSD1306_FADE_OUT);
	SSD1306_Command(0x00);
}

void SSD1306_ZoomIn(uint8_t Zoom)
{
	SSD1306_Command(SSD1306_ZOOM_IN);
	SSD1306_Command(Zoom ? 0x01 : 0x00);
}
#endif

//
// Initialization
//
void SSD1306_Init(void)
{
	SSD1306_Command(0xAE);  // Display Off

	SSD1306_Command(0x00);
	SSD1306_Command(0x10);
	SSD1306_Command(0x40);
	SSD1306_Command(0x20);  // Set addressing mode
	SSD1306_Command(0x00);  // Horizontal Addressing Mode

	SSD1306_SetContrast(0xFF);

	SSD1306_RotateDisplay(1);

	SSD1306_Command(0xA6);  // Set Normal Display

	SSD1306_Command(0xA8);  // Select Multiplex Ratio
	SSD1306_Command(0x3F);  // Default => 0x3F (1/64 Duty)	0x1F(1/32 Duty)

	SSD1306_Command(0xD3);  // Setting Display Offset
	SSD1306_Command(0x00);  // 00H Reset

	SSD1306_Command(0xD5);  // SET DISPLAY CLOCK
	SSD1306_Command(0x80);  // 105HZ

	SSD1306_Command(0xD9);	// Set Pre-Charge period
	SSD1306_Command(0x22);

	SSD1306_Command(0xDA);  // Set COM Hardware Configuration
	SSD1306_Command(0x12);  // Alternative COM Pin---See IC Spec page 34
							// (0x02)=> A4=0;Sequential COM pin configuration;A5=0;Disable COM Left/Right remap

	SSD1306_Command(0xDB);	// Set Deselect Vcomh level
	SSD1306_Command(0x40);

	SSD1306_Command(0x8D);  // Set Charge Pump
	//SSD1306_Command(0x10);  // Disable Charge Pump
	SSD1306_Command(0x14);  // Endable Charge Pump

	SSD1306_Command(0xA4);  // Entire Display ON

#if GRAPHIC_ACCELERATION_COMMANDS == 1
	SSD1306_StopScroll();
#endif

#if ADVANCED_GRAPHIC_COMMANDS == 1
	SSD1306_StopFadeOutOrBlinking();
#endif

	SSD1306_DisplayON(1);
}

#ifdef SSD1306_I2C_CONTROL
void SSD1306_I2cInit(I2C_HandleTypeDef *i2c)
{
	ssd1306_i2c = i2c;

	SSD1306_Init();
}
#endif

#ifdef SSD1306_SPI_CONTROL
void SSD1306_SpiInit(SPI_HandleTypeDef *spi)
{
	ssd1306_spi = spi;

#if defined(SSD1306_RESET_USE)
	SSD1306_Reset();
#endif
	SSD1306_Init();
}
#endif

//
// Draw pixel in the buffer
//
void SSD1306_DrawPixel(int16_t x, int16_t y, uint8_t Color)
{
	 if ((x < 0) || (x >= SSD1306_LCDWIDTH) || (y < 0) || (y >= SSD1306_LCDHEIGHT))
		 return;

	 switch(Color)
	 {
		 case WHITE:   buffer[x+ (y/8)*SSD1306_LCDWIDTH] |=  (1 << (y&7)); break;
		 case BLACK:   buffer[x+ (y/8)*SSD1306_LCDWIDTH] &= ~(1 << (y&7)); break;
		 case INVERSE: buffer[x+ (y/8)*SSD1306_LCDWIDTH] ^=  (1 << (y&7)); break;
	 }
}

//
// Clear the buffer
//
void SSD1306_Clear(uint8_t Color)
{
	switch (Color)
	{
		case WHITE:
			memset(buffer, 0xFF, (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8));
			break;
		case BLACK:
			memset(buffer, 0x00, (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8));
			break;
	}
}

//
// Send buffer to OLDE GCRAM
//
void SSD1306_Display(void)
{
	SSD1306_Command(0x22);
	SSD1306_Command(0x00);
	SSD1306_Command(0x07);
#ifdef SSD1306_I2C_CONTROL
#ifdef SSD1306_I2C_DMA_ENABLE
	if(ssd1306_i2c->hdmatx->State == HAL_DMA_STATE_READY)
	{
		HAL_I2C_Mem_Write_DMA(ssd1306_i2c, SSD1306_I2C_ADDRESS, 0x40, 1, (uint8_t*)&buffer, (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8));
	}
#else
	HAL_I2C_Mem_Write(ssd1306_i2c, SSD1306_I2C_ADDRESS, 0x40, 1, (uint8_t*)&buffer, (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8), 100);
#endif
#endif
#ifdef SSD1306_SPI_CONTROL
#ifdef SSD1306_SPI_DMA_ENABLE
	HAL_GPIO_WritePin(SSD1306_DC_GPIO_Port, SSD1306_DC_Pin, GPIO_PIN_SET);
#ifndef SPI_CS_HARDWARE_CONTROL
	HAL_GPIO_WritePin(SSD1306_CS_GPIO_Port, SSD1306_CS_Pin, GPIO_PIN_RESET);
#endif
	if(ssd1306_spi->hdmatx->State == HAL_DMA_STATE_READY)
	{
		HAL_SPI_Transmit_DMA(ssd1306_spi, (uint8_t*)&buffer, (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8));
	}
#else
	HAL_GPIO_WritePin(SSD1306_DC_GPIO_Port, SSD1306_DC_Pin, GPIO_PIN_SET);
#ifndef SPI_CS_HARDWARE_CONTROL
	HAL_GPIO_WritePin(SSD1306_CS_GPIO_Port, SSD1306_CS_Pin, GPIO_PIN_RESET);
#endif
	HAL_SPI_Transmit(ssd1306_spi, (uint8_t*)&buffer, (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8), 100);
#ifndef SPI_CS_HARDWARE_CONTROL
	HAL_GPIO_WritePin(SSD1306_CS_GPIO_Port, SSD1306_CS_Pin, GPIO_PIN_SET);
#endif
#endif
#endif
}

//
// Display Bitmap directly on screen
//
void SSD1306_Bitmap(uint8_t *bitmap)
{
	SSD1306_Command(0x22);
	SSD1306_Command(0x00);
	SSD1306_Command(0x07);
#ifdef SSD1306_I2C_CONTROL
#ifdef SSD1306_I2C_DMA_ENABLE
	if(ssd1306_i2c->hdmatx->State == HAL_DMA_STATE_READY)
	{
		HAL_I2C_Mem_Write_DMA(ssd1306_i2c, SSD1306_I2C_ADDRESS, 0x40, 1, bitmap, (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8));
	}
#else
	HAL_I2C_Mem_Write(ssd1306_i2c, SSD1306_I2C_ADDRESS, 0x40, 1, bitmap, (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8), 100);
#endif
#endif
#ifdef SSD1306_SPI_CONTROL
#ifdef SSD1306_SPI_DMA_ENABLE
	HAL_GPIO_WritePin(SSD1306_DC_GPIO_Port, SSD1306_DC_Pin, GPIO_PIN_SET);
#ifndef SPI_CS_HARDWARE_CONTROL
	HAL_GPIO_WritePin(SSD1306_CS_GPIO_Port, SSD1306_CS_Pin, GPIO_PIN_RESET);
#endif
	if(ssd1306_spi->hdmatx->State == HAL_DMA_STATE_READY)
	{
		HAL_SPI_Transmit_DMA(ssd1306_spi, bitmap, (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8));
	}
#else
	HAL_GPIO_WritePin(SSD1306_DC_GPIO_Port, SSD1306_DC_Pin, GPIO_PIN_SET);
#ifndef SPI_CS_HARDWARE_CONTROL
	HAL_GPIO_WritePin(SSD1306_CS_GPIO_Port, SSD1306_CS_Pin, GPIO_PIN_RESET);
#endif
	HAL_SPI_Transmit(ssd1306_spi, bitmap, (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8), 100);
#ifndef SPI_CS_HARDWARE_CONTROL
	HAL_GPIO_WritePin(SSD1306_CS_GPIO_Port, SSD1306_CS_Pin, GPIO_PIN_SET);
#endif
#endif
#endif
}

#if defined(SSD1306_SPI_CONTROL) && !defined(SSD1306_SPI_DMA_ENABLE) && !defined(SPI_CS_HARDWARE_CONTROL)
void SSD1306_DmaEndCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi == ssd1306_spi)
	{
		HAL_GPIO_WritePin(SSD1306_CS_GPIO_Port, SSD1306_CS_Pin, GPIO_PIN_SET);
	}
}
#endif

