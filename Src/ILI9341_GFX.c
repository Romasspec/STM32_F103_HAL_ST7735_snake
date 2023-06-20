#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
uint16_t buferFont[31*50];
uint8_t buferFontbuzy = 0;
extern uint16_t LCD_HEIGHT;
extern uint16_t LCD_WIDTH;

/* imprecise small delay */
__STATIC_INLINE void DelayUs(volatile uint32_t us)
{
	us *= (SystemCoreClock / 1000000);
	while (us--);
}

void ILI9341_DrawHollowCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color)
{
	int x = radius-1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1);

	while (x >= y)
	{
		ILI9341_DrawPixel(X + x, Y + y, color);
		ILI9341_DrawPixel(X + y, Y + x, color);
		ILI9341_DrawPixel(X - y, Y + x, color);
		ILI9341_DrawPixel(X - x, Y + y, color);
		ILI9341_DrawPixel(X - x, Y - y, color);
		ILI9341_DrawPixel(X - y, Y - x, color);
		ILI9341_DrawPixel(X + y, Y - x, color);
		ILI9341_DrawPixel(X + x, Y - y, color);

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}

		if (err > 0)
		{
			x--;
			dx += 2;
			err += (-radius << 1) + dx;
		}
	}
}

void ILI9341_DrawFilledCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color)
{

	int x = radius;
	int y = 0;
	int xChange = 1 - (radius << 1);
	int yChange = 0;
	int radiusError = 0;

	while (x >= y)
	{
		for (int i = X - x; i <= X + x; i++)
		{
			ILI9341_DrawPixel(i, Y + y,color);
			ILI9341_DrawPixel(i, Y - y,color);
		}

		for (int i = X - y; i <= X + y; i++)
		{
			ILI9341_DrawPixel(i, Y + x,color);
			ILI9341_DrawPixel(i, Y - x,color);
		}

		y++;
		radiusError += yChange;
		yChange += 2;

		if (((radiusError << 1) + xChange) > 0)
		{
			x--;
			radiusError += xChange;
			xChange += 2;
		}
	}
}

void ILI9341_DrawHollowRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color)
{
	uint16_t xLen = 0;
	uint16_t yLen = 0;
	uint8_t negX = 0;
	uint8_t negY = 0;
	float negCalc = 0;

	negCalc = X1 - X0;
	if(negCalc < 0) negX = 1;
	negCalc = 0;

	negCalc = Y1 - Y0;
	if(negCalc < 0) negY = 1;

	//DRAW HORIZONTAL!
	if(!negX)
	{
		xLen = X1 - X0;
	}
	else
	{
		xLen = X0 - X1;
	}
	ILI9341_DrawHLine(X0, Y0, xLen, color);
	ILI9341_DrawHLine(X0, Y1, xLen, color);

	//DRAW VERTICAL!
	if(!negY)
	{
		yLen = Y1 - Y0;
	}
	else
	{
		yLen = Y0 - Y1;
	}

	ILI9341_DrawVLine(X0, Y0, yLen, color);
	ILI9341_DrawVLine(X1, Y0, yLen, color);

	if((xLen > 0)||(yLen > 0))
	{
		ILI9341_DrawPixel(X1, Y1, color);
	}
}

void ILI9341_DrawFilledRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color)
{
	uint16_t xLen = 0;
	uint16_t yLen = 0;
	uint8_t negX = 0;
	uint8_t negY = 0;
	int32_t negCalc = 0;
	uint16_t X0True = 0;
	uint16_t Y0True = 0;

	negCalc = X1 - X0;
	if(negCalc < 0) negX = 1;
	negCalc = 0;

	negCalc = Y1 - Y0;
	if(negCalc < 0) negY = 1;

	if(!negX)
	{
		xLen = X1 - X0;
		X0True = X0;
	}
	else
	{
		xLen = X0 - X1;
		X0True = X1;
	}

	if(!negY)
	{
		yLen = Y1 - Y0;
		Y0True = Y0;
	}
	else
	{
		yLen = Y0 - Y1;
		Y0True = Y1;
	}

	ILI9341_DrawRectangle(X0True, Y0True, xLen, yLen, color);
}

void ILI9341_DrawChar(char ch, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
	if ((ch < 31) || (ch > 127)) return;

	uint8_t fOffset, fWidth, fHeight, fBPL;
	uint8_t *tempChar;

	fOffset = font[0];
	fWidth = font[1];
	fHeight = font[2];
	fBPL = font[3];

	tempChar = (uint8_t*)&font[((ch - 0x20) * fOffset) + 4]; /* Current Character = Meta + (Character Index * Offset) */

	/* Clear background first */
	ILI9341_DrawRectangle(X, Y, fWidth, fHeight, bgcolor);

	for (int j=0; j < fHeight; j++)
	{
		for (int i=0; i < fWidth; i++)
		{
			uint8_t z =  tempChar[fBPL * i + ((j & 0xF8) >> 3) + 1]; /* (j & 0xF8) >> 3, increase one by 8-bits */
			uint8_t b = 1 << (j & 0x07);
			if (( z & b ) != 0x00)
			{
				ILI9341_DrawPixel(X+i, Y+j, color);
			}
		}
	}
}

void ILI9341_DrawText(const char* str, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
	uint8_t charWidth;			/* Width of character */
	uint8_t fOffset = font[0];	/* Offset of character */
	uint8_t fWidth = font[1];	/* Width of font */

	while (*str)
	{
		ILI9341_DrawChar(*str, font, X, Y, color, bgcolor);

		/* Check character width and calculate proper position */
		uint8_t *tempChar = (uint8_t*)&font[((*str - 0x20) * fOffset) + 4];
		charWidth = tempChar[0];

		if(charWidth + 2 < fWidth)
		{
			/* If character width is smaller than font width */
			X += (charWidth + 2);
		}
		else
		{
			X += fWidth;
		}

		str++;
	}
}

void ILI9341_DrawImage(const uint8_t* image, uint8_t orientation)
{
	if(orientation == SCREEN_HORIZONTAL_1)
	{
		ILI9341_SetRotation(SCREEN_HORIZONTAL_1);
		ILI9341_SetAddress(0,0,ILI9341_SCREEN_WIDTH,ILI9341_SCREEN_HEIGHT);
	}
	else if(orientation == SCREEN_HORIZONTAL_2)
	{
		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
		ILI9341_SetAddress(0,0,ILI9341_SCREEN_WIDTH,ILI9341_SCREEN_HEIGHT);
	}
	else if(orientation == SCREEN_VERTICAL_2)
	{
		ILI9341_SetRotation(SCREEN_VERTICAL_2);
		ILI9341_SetAddress(0,0,ILI9341_SCREEN_HEIGHT,ILI9341_SCREEN_WIDTH);
	}
	else if(orientation == SCREEN_VERTICAL_1)
	{
		ILI9341_SetRotation(SCREEN_VERTICAL_1);
		ILI9341_SetAddress(0,0,ILI9341_SCREEN_HEIGHT,ILI9341_SCREEN_WIDTH);
	}

	uint32_t counter = 0;
	for(uint32_t i = 0; i < ILI9341_SCREEN_WIDTH*ILI9341_SCREEN_HEIGHT*2/BURST_MAX_SIZE; i++)
	{
		ILI9341_WriteBuffer((uint8_t*)(image + counter), BURST_MAX_SIZE);
		counter += BURST_MAX_SIZE;

		/* DMA Tx is too fast, It needs some delay */
		DelayUs(1);
	}
}

void DrawChar_NUM(char c, uint8_t x, uint8_t y, TM_FontDef32_t* font, uint16_t color_text, uint16_t color_backround)
{
	uint8_t width = font->FontWidth;
	uint8_t height = font->FontHeight;
	uint32_t data;	
	
	for (uint8_t i=0; i < height; i++)
	{
		data = font->data [(c-48) * height + i];
		for (uint8_t j=0; j < width; j++)
		{
			if ((data<<j) & (1<<width))
			{
				buferFont[i*width+j] = (color_text>>8)|((color_text&0xFF)<<8);
			}
			else
			{
				buferFont[i*width+j] = (color_backround>>8)|((color_backround&0xFF)<<8);
			}
		}
	}	
	
	//передать буфер
	ILI9341_SetAddress (x, y, x+width-1, y+height-1);
	ILI9341_WriteCommand(0x2C);
	ILI9341_WriteBuffer ((uint8_t*) buferFont, width*height*2);
}

void DrawString_NUM(char *str, uint8_t x, uint8_t y, TM_FontDef32_t* font, uint16_t color_text, uint16_t color_backround)
{
	while (*str)
	{
		if ((x+font->FontWidth) > LCD_WIDTH)
		{
			break;
		}
		DrawChar_NUM (*str, x, y, font, color_text, color_backround);
		
		x += font->FontWidth+2;
		str++;
	}
}

void DrawChar_st7735(char c, uint8_t x, uint8_t y, TM_FontDef_t* font, uint16_t color_text, uint16_t color_backround)
{
	uint8_t width = font->FontWidth;
	uint8_t height = font->FontHeight;
	uint16_t data;
	
	// установить область
	while (buferFontbuzy);
	
	for (uint8_t i=0; i < height; i++)
	{
		data = font->data [(c-32) * height + i];
		for (uint8_t j=0; j < width; j++)
		{
			if ((data<<j) & 0x8000)
			{
				buferFont[i*width+j] = (color_text>>8)|((color_text&0xFF)<<8);
			}
			else
			{
				buferFont[i*width+j] = (color_backround>>8)|((color_backround&0xFF)<<8);
			}
			
//			buferFont[i*width+j] = (GREEN>>8)|((GREEN&0xFF)<<8);
		}
	}	
	
	//передать буфер
	
	ILI9341_SetAddress (x, y, x+width-1, y+height-1);
	ILI9341_WriteCommand(0x2C);
	buferFontbuzy = 1;
	ILI9341_WriteBuffer ((uint8_t*) buferFont, width*height*2);
}

void DrawString_st7735(char *str, uint8_t x, uint8_t y, TM_FontDef_t* font, uint16_t color_text, uint16_t color_backround)
{
	while (*str)
	{
		if ((x+font->FontWidth) > LCD_WIDTH)
		{
			break;
		}
		DrawChar_st7735 (*str, x, y, font, color_text, color_backround);
		
		x += font->FontWidth;
		str++;
	}
}

void DrawChar_st7735_myFont (char c, uint8_t x, uint8_t y, const tFont* font, uint16_t color_text, uint16_t color_backround)
{
	uint8_t width = font->chars->image->width;
	uint8_t height = font->chars->image->height;
	uint8_t data;
	while (buferFontbuzy);
	
	for (uint8_t i=0; i < height; i++)
	{
//		data = font->data [(c-32) * height + i];		
		
		for (uint8_t j=0; j < width; j++)
		{
			for (uint8_t k=0; k<3; k++)
			{
				data = font->chars[c-48].image->data[3*width*i+3*j+k];
				for(uint8_t bit=0; bit<8; bit++)
				{
					if ((data<<bit)&0x80)
					{
						buferFont[k+3*j+3*width*i+bit] = (color_text>>8)|((color_text&0xFF)<<8);
					}
					else
					{
						buferFont[k+3*j+3*width*i+bit] = (color_backround>>8)|((color_backround&0xFF)<<8);
					}
				}
			}						
		}		
	}
	
	ILI9341_SetAddress (x, y, x+width*3-1, y+height-1);
	ILI9341_WriteCommand(0x2C);
	buferFontbuzy = 1;
	ILI9341_WriteBuffer ((uint8_t*) buferFont, width*3*height*2);
}

void DrawString_st7735_myFont(char *str, uint8_t x, uint8_t y, const tFont* font, uint16_t color_text, uint16_t color_backround)
{
	while (*str)
	{
		if ((x+font->chars->image->width*3) > LCD_WIDTH)
		{
			break;
		}
		DrawChar_st7735_myFont (*str, x, y, font, color_text, color_backround);
		//HAL_Delay (2000);
		x += font->chars->image->width*3;
		str++;
	}
}

void ILI9341_DrawRct4x4 (uint8_t x, uint8_t y, uint16_t color)
{
	while (buferFontbuzy);
	for(uint8_t i=0; i<4*4; i++)
	{
		buferFont[i] = (color>>8)|((color&0xFF)<<8);
	}
	
	ILI9341_SetAddress ((x<<2), (y<<2), (x<<2)+4-1, (y<<2)+4-1);
	ILI9341_WriteCommand(0x2C);
	buferFontbuzy = 1;
	ILI9341_WriteBuffer ((uint8_t*) buferFont, 4*4*2);
}
void ILI9341_DrawVl (uint8_t x1, uint8_t y1, uint8_t y2, uint16_t color)
{
	while (buferFontbuzy);
	for(uint16_t i=0; i<((y2-y1+1)<<2); i++)
	{
		buferFont[i] = (color>>8)|((color&0xFF)<<8);
	}
	
	ILI9341_SetAddress (x1, y1, x1+4-1, y2);
	ILI9341_WriteCommand(0x2C);
	buferFontbuzy = 1;
	ILI9341_WriteBuffer ((uint8_t*) buferFont, (y2-y1+1)*8);
}

void ILI9341_DrawHl (uint8_t x1, uint8_t x2, uint8_t y1, uint16_t color)
{
	while (buferFontbuzy);
	for(uint16_t i=0; i<((x2-x1+1)<<2); i++)
	{
		buferFont[i] = (color>>8)|((color&0xFF)<<8);
	}
	
	ILI9341_SetAddress (x1, y1, x2, y1+4-1);
	ILI9341_WriteCommand(0x2C);
	buferFontbuzy = 1;
	ILI9341_WriteBuffer ((uint8_t*) buferFont, (x2-x1+1)*4*2);
}
