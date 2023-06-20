#ifndef ILI9341_GFX_H
#define ILI9341_GFX_H

//#include "stm32f4xx_hal.h"
#include "stm32f1xx_hal.h"
#include "fonts.h"
#include "tm_stm32f1_fonts.h"
#include "Font_my.h"

#define HORIZONTAL_IMAGE	0
#define VERTICAL_IMAGE		1

void ILI9341_DrawHollowCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color);
void ILI9341_DrawFilledCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color);
void ILI9341_DrawHollowRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color);
void ILI9341_DrawFilledRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color);
void ILI9341_DrawChar(char ch, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor);
void ILI9341_DrawText(const char* str, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor);
void ILI9341_DrawImage(const uint8_t* image, uint8_t orientation);
void DrawChar_st7735(char c, uint8_t x, uint8_t y, TM_FontDef_t* font, uint16_t color_text, uint16_t color_backround);
void DrawString_st7735(char *str, uint8_t x, uint8_t y, TM_FontDef_t* font, uint16_t color_text, uint16_t color_backround);
void DrawChar_st7735_myFont (char c, uint8_t x, uint8_t y, const tFont* font, uint16_t color_text, uint16_t color_backround);
void DrawString_st7735_myFont(char *str, uint8_t x, uint8_t y, const tFont* font, uint16_t color_text, uint16_t color_backround);

void DrawString_NUM(char *str, uint8_t x, uint8_t y, TM_FontDef32_t* font, uint16_t color_text, uint16_t color_backround);
void DrawChar_NUM(char c, uint8_t x, uint8_t y, TM_FontDef32_t* font, uint16_t color_text, uint16_t color_backround);
void ILI9341_DrawRct4x4 (uint8_t x, uint8_t y, uint16_t color);
void ILI9341_DrawVl (uint8_t x1, uint8_t y1, uint8_t y2, uint16_t color);
void ILI9341_DrawHl (uint8_t x1, uint8_t x2, uint8_t y1, uint16_t color);

#endif
