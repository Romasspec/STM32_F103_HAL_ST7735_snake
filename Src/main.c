
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include "stm32f4xx_hal.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
//#include "snow_tiger.h"
//#include "Font_my.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
//RNG_HandleTypeDef hrng;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
//static void MX_RNG_Init(void);
char buf[6];
void uint_to_str (uint16_t c)
{	
	buf[0] = ((c%100000) /10000) == 0 ? ' ' :((c%100000) /10000) + '0' ;
	buf[1] = (((c%10000) /1000) + buf[0]) == (0+' ') ?  ' ' : ((c%10000) /1000) + '0' ;
	buf[2] = (((c%1000) /100) + buf[1]) == (0+' ') ?  ' ' : ((c%1000) /100) + '0' ;
	buf[3] = (((c%100) /10) + buf[2]) == (0+' ') ?  ' ' : ((c%100) /10) + '0' ;
	buf[4] = (c%10) /1 + '0';
	buf[5] = 0;
}

static uint16_t RGBto565 (uint8_t r, uint8_t g, uint8_t b)
{
	 return (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
#define SNAKEMAXLENGTH			100
#define HEAD	0
#define XHEADSTART	20
#define YHEADSTART	16
uint32_t tim;


uint8_t rand_my (uint8_t min, uint8_t max)
{	uint8_t temp;
	do{
		tim = (tim+HAL_GetTick()) * HAL_GetTick();
		temp = (uint8_t) ((tim) % (max-min+1)+min);
	} while ((temp > max) | (temp==0));
	return temp; //(uint8_t) ((tim) % (max-min+1)+min);
 
//	tim ^= tim >> 6;
//  tim ^= tim << 11;
//  tim ^= tim >> 15;
//  return ( tim * 3538123 ) % (max-min+1)+min;	
}
uint8_t driwe = 0;
uint8_t restart, flagstrt;

int main(void)
{
	uint8_t i, score;
	
	typedef struct
	{
		uint8_t x;
		uint8_t y;
		uint8_t x_prev;
		uint8_t y_prev;
	} snake_body;
	
	typedef struct
	{
		snake_body body[SNAKEMAXLENGTH];
		uint8_t length;
	}snake;
	
	typedef struct
	{
		uint8_t x;
		uint8_t y;
	} apple;
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
//  MX_RNG_Init();
  /* USER CODE BEGIN 2 */
	ILI9341_Init();
	
	ILI9341_SetRotation (SCREEN_HORIZONTAL_1);
//	uint16_t cnt[5] = {0,0,0,0,0};
  /* USER CODE END 2 */
	
	uint16_t color1 = RGBto565(255,0,0);
	uint16_t color2 = RGBto565(0,255,0);
	uint16_t color3 = RGBto565(0,0,255);
	uint16_t color4 = RGBto565(127,0,255);
	uint16_t color5 = RGBto565(50,50,50);
	
	
//	uint8_t field[40][32];
//	for (uint8_t i = 0; i<40; i++)
//	{
//		for (uint8_t j=0; j<32; j++)
//		{
//			field[i][j] = 0;
//		}
//	}
	
	uint16_t color_head = RGBto565(255,50,0);
	uint16_t color_body = RGBto565(0,255,0);
	uint16_t color_backround	= BLACK;
ini:
	restart = 0;
	score = 0;
	ILI9341_FillScreen(color_backround);
	ILI9341_DrawHl(0, 159, 0, RED);
	ILI9341_DrawVl(0,0, 127, RED);
	ILI9341_DrawHl(0, 159, 127-3, RED);
	ILI9341_DrawVl(159-3,0,127, RED);
	
	uint32_t cur_ms = 0, prev_ms = 0;
	uint8_t gameSpeed = 250;
	driwe = 0;
	flagstrt = 1;
	snake snake1;
	apple apple1;
	
	apple1.x = rand_my (1, 38);
	apple1.y = rand_my (1, 30);
	
	snake1.length = 5;
	
	for (i=0; i<snake1.length; i++)
	{
	snake1.body[i].x = snake1.body[i].x_prev = XHEADSTART-i;
	snake1.body[i].y = snake1.body[i].y_prev = YHEADSTART;		
	}
	
	ILI9341_DrawRct4x4 (snake1.body[HEAD].x, snake1.body[HEAD].y, color_head);
	for (i=1; i<snake1.length; i++)
	{
		ILI9341_DrawRct4x4 (snake1.body[i].x, snake1.body[i].y, color_body);
	}
	
	ILI9341_DrawRct4x4 (apple1.x, apple1.y, GREEN);
	
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	while (1)
	{
		tim++;
		cur_ms = HAL_GetTick ();
		readBtn();
		
		if (restart) goto ini;
		
		if (((cur_ms - prev_ms) > gameSpeed) & flagstrt)
		{
			prev_ms = cur_ms;
						
			if (driwe == 0)
			{
				snake1.body[HEAD].x++;
//				if (snake1.body[HEAD].x == 39)
//				{
//					driwe = 1;
//				}
			}
			else if (driwe == 1)
			{
				snake1.body[HEAD].y++;
//				if (snake1.body[HEAD].y == 31)
//				{
//					driwe = 2;
//				}
			}
			else if (driwe == 2)
			{
				snake1.body[HEAD].x--;
//				if (snake1.body[HEAD].x == 0)
//				{
//					driwe = 3;
//				}
			}
			else if (driwe == 3)
			{
				snake1.body[HEAD].y--;
//				if (snake1.body[HEAD].y == 0)
//				{
//					driwe = 0;
//					apple1.x = rand_my (0, 39);
//					apple1.y = rand_my (0, 31);
//					ILI9341_DrawRct4x4 (apple1.x, apple1.y, GREEN);
//				}
			}
			
			for (i=1; i<snake1.length; i++)
			{
				snake1.body[i].x = snake1.body[i-1].x_prev;
				snake1.body[i].y = snake1.body[i-1].y_prev;
			}
			
			if ((snake1.body[HEAD].x == 39)|((snake1.body[HEAD].x == 0)|(snake1.body[HEAD].y == 31)|(snake1.body[HEAD].y == 0)))
			{
				ILI9341_FillScreen(color_backround);
				DrawString_st7735 ("Game over", 30, 10, &TM_Font_7x10, OLIVE, color_backround);
				DrawString_st7735 ("score", 30, 20, &TM_Font_7x10, OLIVE, color_backround);
				uint_to_str (score);
				DrawString_st7735 (buf, 30, 30, &TM_Font_7x10, color1, color_backround);
				
				flagstrt = 0;
			}
			else
			{
				ILI9341_DrawRct4x4 (snake1.body[HEAD].x, snake1.body[HEAD].y, color_head);
				for (i=1; i<snake1.length; i++)
				{
					ILI9341_DrawRct4x4 (snake1.body[i].x, snake1.body[i].y, color_body);
				}
				
				ILI9341_DrawRct4x4 (snake1.body[snake1.length-1].x_prev, snake1.body[snake1.length-1].y_prev, color_backround);	
				snake1.body[snake1.length-1].x_prev = snake1.body[snake1.length-1].x;
				snake1.body[snake1.length-1].y_prev = snake1.body[snake1.length-1].y;
				
				for (i=0; i<snake1.length-1; i++)
				{
					snake1.body[i].x_prev = snake1.body[i].x;
					snake1.body[i].y_prev = snake1.body[i].y;
				}
				
				if ((snake1.body[HEAD].x == apple1.x) && (snake1.body[HEAD].y == apple1.y))
				{
					apple1.x = rand_my (1, 38);
					apple1.y = rand_my (1, 30);
					ILI9341_DrawRct4x4 (apple1.x, apple1.y, GREEN);
					snake1.length++;
					score++;
					if ((score % 3 ) == 2)
					{
						gameSpeed -=50;
					}
				}
			}
			
			
//			uint_to_str (snake1.body[HEAD].x);
//			DrawString_st7735 (buf, 5, 5, &TM_Font_7x10, color1, BLACK);
//			uint_to_str (snake1.body[HEAD].y);
//			DrawString_st7735 (buf, 5, 15, &TM_Font_7x10, color1, BLACK);
//			uint_to_str (snake1.length);
//			DrawString_st7735 (buf, 50, 5, &TM_Font_7x10, color1, BLACK);
			//while(1);
		}
		
		
		
		
//		static uint16_t x = 0;
//		static uint16_t y = 0;
//		static char BufferText[40];

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	
//	ILI9341_FillScreen(RED);
//	DrawChar_st7735 ('7', 10, 10, &TM_Font_16x26, GREEN, WHITE);
//	DrawString_st7735 ("Start 12345", 10, 10, &TM_Font_11x18, BLUE, WHITE);
//	DrawString_st7735 ("LET'S go", 10, 50, &TM_Font_7x10, BLUE, RED);
//	HAL_Delay (2000);
		
//		ILI9341_FillScreen(GREEN);
//		HAL_Delay (2000);
//		
//		ILI9341_FillScreen(BLUE);
//		HAL_Delay (2000);
		
		
		/* COUNTING MULTIPLE SEGMENTS */
//		static uint8_t rotation = 0;
		
//		if (rotation > 3) rotation = 0;
//		ILI9341_SetRotation(rotation++);
//		ILI9341_DrawText("Counting multiple segments at once", FONT2, 10, 10, BLACK, WHITE);
//		DrawString_st7735 ("Start 12345", 5, 10, &TM_Font_font_ink_free_10, DARKGREEN, BLACK);
//		DrawString_st7735 ("Menu 1", 5, 30, &TM_Font_font_ink_free_10, DARKCYAN, BLACK);
//		DrawString_st7735 ("Menu 2", 5, 50, &TM_Font_font_ink_free_10, ORANGE, BLACK);
//		DrawString_st7735 ("Menu123", 5, 10, &TM_Font_7x10, OLIVE, BLACK);

//			uint_to_str (cnt[0]++);
//			DrawString_st7735 (buf, 5, 5, &TM_Font_7x10, color1, BLACK);
//			uint_to_str (cnt[1]++);
//			DrawString_st7735 (buf, 5, 20, &TM_Font_7x10, color2, BLACK);
//			uint_to_str (cnt[2]++);
//			DrawString_st7735 (buf, 5, 35, &TM_Font_7x10, color3, BLACK);
//			uint_to_str (cnt[3]++);
//			DrawString_st7735 (buf, 5, 50, &TM_Font_7x10, color4, BLACK);
//			uint_to_str (cnt[4]++);
//			DrawString_st7735 (buf, 5, 65, &TM_Font_7x10, color5, BLACK);
//			uint_to_str (cnt[1]++);
//			DrawString_st7735 (buf, 5, 80, &TM_Font_7x10, color1, BLACK);
//			uint_to_str (cnt[2]++);
//			DrawString_st7735 (buf, 5, 95, &TM_Font_7x10, color2, BLACK);
//			uint_to_str (cnt[3]++);
//			DrawString_st7735 (buf, 5, 110, &TM_Font_7x10, color3, BLACK);
//			uint_to_str (cnt[4]++);
//			DrawString_st7735 (buf, 50, 5, &TM_Font_7x10, color4, BLACK);
//			uint_to_str (cnt[4]++);
//			DrawString_st7735 (buf, 50, 20, &TM_Font_7x10, color5, BLACK);
//			uint_to_str (cnt[4]++);
//			DrawString_st7735 (buf, 50, 35, &TM_Font_7x10, color1, BLACK);
//			uint_to_str (cnt[4]++);
//			DrawString_st7735 (buf, 50, 50, &TM_Font_7x10, color2, BLACK);
//			uint_to_str (cnt[4]++);
//			DrawString_st7735 (buf, 50, 65, &TM_Font_7x10, color3, BLACK);			
//			ILI9341_DrawHLine (0,1, 159, RED);

				





//			ILI9341_DrawVLine (0, 0, 127, GREEN);
//			ILI9341_DrawHLine (0,127, 159, WHITE);
//			ILI9341_DrawVLine (159, 0, 127, BLUE);
			
			
//			DrawChar_st7735_myFont ('2',10,10, &Font, BLUE, WHITE);
//			DrawString_st7735_myFont ("0123456789",10,10, &Font, RED, WHITE);
//		ILI9341_DrawText ("012", SevenSegNumFont_32x50, 5, 90, WHITE, BLACK);
//		HAL_Delay(50);


//		for(uint16_t i = 0; i <= 10; i++)
//		{
//			sprintf(BufferText, "Counting: %d", i);
//			ILI9341_DrawText(BufferText, FONT3, 10, 10, BLACK, WHITE);
//			ILI9341_DrawText(BufferText, FONT3, 10, 30, BLUE, WHITE);
//			ILI9341_DrawText(BufferText, FONT3, 10, 50, RED, WHITE);
//			ILI9341_DrawText(BufferText, FONT3, 10, 70, GREEN, WHITE);
//			ILI9341_DrawText(BufferText, FONT3, 10, 90, BLACK, WHITE);
//			ILI9341_DrawText(BufferText, FONT3, 10, 110, BLUE, WHITE);
//			ILI9341_DrawText(BufferText, FONT3, 10, 130, RED, WHITE);
//			ILI9341_DrawText(BufferText, FONT3, 10, 150, GREEN, WHITE);
//			ILI9341_DrawText(BufferText, FONT3, 10, 170, WHITE, BLACK);
//			ILI9341_DrawText(BufferText, FONT3, 10, 190, BLUE, BLACK);
//			ILI9341_DrawText(BufferText, FONT3, 10, 210, RED, BLACK);
//		}
//		HAL_Delay(1000);
		

//		/* COUNTING SINGLE SEGMENT */
//		ILI9341_FillScreen(WHITE);
//		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
//		ILI9341_DrawText("Counting single segment", FONT3, 10, 10, BLACK, WHITE);
//		HAL_Delay(2000);
//		ILI9341_FillScreen(WHITE);

//		for(uint16_t i = 0; i <= 100; i++)
//		{
//			sprintf(BufferText, "Counting: %d", i);
//			ILI9341_DrawText(BufferText, FONT4, 10, 10, BLACK, WHITE);
//			HAL_Delay(10);
//		}
//		HAL_Delay(1000);

//		/* ALIGNMENT TEST */
//		ILI9341_FillScreen(WHITE);
//		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
//		ILI9341_DrawText("Rectangle alignment check", FONT3, 10, 10, BLACK, WHITE);
//		HAL_Delay(2000);
//		ILI9341_FillScreen(WHITE);

//		ILI9341_DrawHollowRectangleCoord(50, 50, 100, 100, BLACK);
//		ILI9341_DrawFilledRectangleCoord(20, 20, 50, 50, BLACK);
//		ILI9341_DrawHollowRectangleCoord(10, 10, 19, 19, BLACK);
//		HAL_Delay(1000);

//		/* LINES EXAMPLE */
//		ILI9341_FillScreen(WHITE);
//		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
//		ILI9341_DrawText("Randomly placed and sized", FONT3, 10, 10, BLACK, WHITE);
//		ILI9341_DrawText("Horizontal and Vertical lines", FONT3, 10, 30, BLACK, WHITE);
//		HAL_Delay(2000);
//		ILI9341_FillScreen(WHITE);

//		for(uint32_t i = 0; i < 30000; i++)
//		{
//			uint32_t random_num = 0;
//			uint16_t xr = 0;
//			uint16_t yr = 0;
//			uint16_t radiusr = 0;
//			uint16_t colourr = 0;

//			/* Get random inputs */
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			xr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			yr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			radiusr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			colourr = random_num;

//			xr &= 0x01FF;
//			yr &= 0x01FF;
//			radiusr &= 0x001F;

//			ILI9341_DrawHLine(xr, yr, radiusr, colourr);
//			ILI9341_DrawVLine(xr, yr, radiusr, colourr);
//		}
//		HAL_Delay(1000);

//		/* HOLLOW CIRCLES EXAMPLE */
//		ILI9341_FillScreen(WHITE);
//		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
//		ILI9341_DrawText("Randomly placed and sized", FONT3, 10, 10, BLACK, WHITE);
//		ILI9341_DrawText("Circles", FONT3, 10, 30, BLACK, WHITE);
//		HAL_Delay(2000);
//		ILI9341_FillScreen(WHITE);

//		for(uint32_t i = 0; i < 3000; i++)
//		{
//			uint32_t random_num = 0;
//			uint16_t xr = 0;
//			uint16_t yr = 0;
//			uint16_t radiusr = 0;
//			uint16_t colourr = 0;

//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			xr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			yr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			radiusr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			colourr = random_num;

//			xr &= 0x01FF;
//			yr &= 0x01FF;
//			radiusr &= 0x001F;

//			ILI9341_DrawHollowCircle(xr, yr, radiusr*2, colourr);
//		}
//		HAL_Delay(1000);

//		/* FILLED CIRCLES EXAMPLE */
//		ILI9341_FillScreen(WHITE);
//		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
//		ILI9341_DrawText("Randomly placed and sized", FONT3, 10, 10, BLACK,WHITE);
//		ILI9341_DrawText("Filled Circles", FONT3, 10, 30, BLACK, WHITE);
//		HAL_Delay(2000);
//		ILI9341_FillScreen(WHITE);

//		for(uint32_t i = 0; i < 1000; i++)
//		{
//			uint32_t random_num = 0;
//			uint16_t xr = 0;
//			uint16_t yr = 0;
//			uint16_t radiusr = 0;
//			uint16_t colourr = 0;

//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			xr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			yr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			radiusr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			colourr = random_num;

//			xr &= 0x01FF;
//			yr &= 0x01FF;
//			radiusr &= 0x001F;

//			ILI9341_DrawFilledCircle(xr, yr, radiusr/2, colourr);
//		}
//		HAL_Delay(1000);

//		/* HOLLOW RECTANGLES EXAMPLE */
//		ILI9341_FillScreen(WHITE);
//		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
//		ILI9341_DrawText("Randomly placed and sized", FONT3, 10, 10, BLACK, WHITE);
//		ILI9341_DrawText("Rectangles", FONT3, 10, 30, BLACK, WHITE);
//		HAL_Delay(2000);
//		ILI9341_FillScreen(WHITE);

//		for(uint32_t i = 0; i < 20000; i++)
//		{
//			uint32_t random_num = 0;
//			uint16_t xr = 0;
//			uint16_t yr = 0;
//			uint16_t radiusr = 0;
//			uint16_t colourr = 0;

//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			xr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			yr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			radiusr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			colourr = random_num;

//			xr &= 0x01FF;
//			yr &= 0x01FF;
//			radiusr &= 0x001F;

//			ILI9341_DrawHollowRectangleCoord(xr, yr, xr+radiusr, yr+radiusr, colourr);
//		}
//		HAL_Delay(1000);

//		/* FILLED RECTANGLES EXAMPLE */
//		ILI9341_FillScreen(WHITE);
//		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
//		ILI9341_DrawText("Randomly placed and sized", FONT3, 10, 10, BLACK, WHITE);
//		ILI9341_DrawText("Filled Rectangles", FONT3, 10, 30, BLACK, WHITE);
//		HAL_Delay(2000);
//		ILI9341_FillScreen(WHITE);

//		for(uint32_t i = 0; i < 20000; i++)
//		{
//			uint32_t random_num = 0;
//			uint16_t xr = 0;
//			uint16_t yr = 0;
//			uint16_t radiusr = 0;
//			uint16_t color = 0;

//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			xr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			yr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			radiusr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			color = random_num;

//			xr &= 0x01FF;
//			yr &= 0x01FF;
//			radiusr &= 0x001F;

//			ILI9341_DrawRectangle(xr, yr, radiusr, radiusr, color);
//		}
//		HAL_Delay(1000);

		/* INDIVIDUAL PIXEL EXAMPLE */

//		ILI9341_FillScreen(WHITE);
//		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
//		ILI9341_DrawText("Slow draw by selecting", FONT3, 10, 10, BLACK, WHITE);
//		ILI9341_DrawText("and adressing pixels", FONT3, 10, 30, BLACK, WHITE);
//		HAL_Delay(2000);
//		ILI9341_FillScreen(WHITE);

//		x = 0;
//		y = 0;

//		while (y < ILI9341_SCREEN_HEIGHT)
//		{
//			while ((x < ILI9341_SCREEN_WIDTH) && (y < ILI9341_SCREEN_HEIGHT))
//			{
//				if(x % 2)
//				{
//					ILI9341_DrawPixel(x, y, BLACK);
//				}
//				x++;
//			}
//			y++;
//			x = 0;
//		}

//		x = 0;
//		y = 0;

//		while (y < ILI9341_SCREEN_HEIGHT)
//		{
//			while ((x < ILI9341_SCREEN_WIDTH) && (y < ILI9341_SCREEN_HEIGHT))
//			{
//				if(y % 2)
//				{
//					ILI9341_DrawPixel(x, y, BLACK);
//				}
//				x++;
//			}
//			y++;
//			x = 0;
//		}
//		HAL_Delay(2000);

//		/* INDIVIDUAL PIXEL EXAMPLE */
//		ILI9341_FillScreen(WHITE);
//		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
//		ILI9341_DrawText("Random position and color", FONT3, 10, 10, BLACK, WHITE);
//		ILI9341_DrawText("500000 pixels", FONT3, 10, 30, BLACK, WHITE);
//		HAL_Delay(2000);
//		ILI9341_FillScreen(WHITE);

//		for(uint32_t i = 0; i < 500000; i++)
//		{
//			uint32_t random_num = 0;
//			uint16_t xr = 0;
//			uint16_t yr = 0;

//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			xr = random_num;
//			random_num = HAL_RNG_GetRandomNumber(&hrng);
//			yr = random_num;
//			uint16_t color = HAL_RNG_GetRandomNumber(&hrng);

//			xr &= 0x01FF;
//			yr &= 0x01FF;

//			ILI9341_DrawPixel(xr, yr, color);
//		}
//		HAL_Delay(2000);

//		/* 565 GRAYSCALE EXAMPLE */
//		ILI9341_FillScreen(WHITE);
//		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
//		ILI9341_DrawText("Color gradient", FONT3, 10, 10, BLACK, WHITE);
//		ILI9341_DrawText("Grayscale", FONT3, 10, 30, BLACK, WHITE);
//		HAL_Delay(2000);

//		for(uint16_t i = 0; i <= ILI9341_SCREEN_WIDTH; i++)
//		{
//			uint16_t Red = 0;
//			uint16_t Green = 0;
//			uint16_t Blue = 0;

//			Red = i/(10);
//			Red <<= 11;
//			Green = i/(5);
//			Green <<= 5;
//			Blue = i/(10);

//			uint16_t RGB = Red + Green + Blue;
//			ILI9341_DrawRectangle(i, x, 1, 240, RGB);

//		}
//		HAL_Delay(2000);

//		/* IMAGE EXAMPLE */
//		ILI9341_FillScreen(WHITE);
//		ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
//		ILI9341_DrawText("RGB Picture", FONT3, 10, 10, RED, YELLOW);
//		ILI9341_DrawText("TIGER", FONT3, 10, 30, BLACK, WHITE);
//		HAL_Delay(2000);
//		ILI9341_DrawImage(snow_tiger, SCREEN_VERTICAL_2);
//		ILI9341_SetRotation(SCREEN_VERTICAL_1);
//		HAL_Delay(5000);
	}
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

//  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
//  RCC_OscInitStruct.PLL.PLLN = 336;
//  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
//  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

///* RNG init function */
//static void MX_RNG_Init(void)
//{

//  hrng.Instance = RNG;
//  if (HAL_RNG_Init(&hrng) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//}

///* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
//  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_LED_GPIO_Port, LCD_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LCD_CS_Pin LCD_DC_Pin LCD_RST_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = LCD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LCD_DC_GPIO_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LCD_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LCD_RST_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LCD_LED_GPIO_Port, &GPIO_InitStruct);
	
	//*******************Keyboard*********************************************************	
	GPIO_InitStruct.Pin 	= KEY_ROW1 | KEY_ROW2 | KEY_ROW3 | KEY_ROW4;
	GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed	= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init (GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 	= KEY_COL1 | KEY_COL2 | KEY_COL3 | KEY_COL4;
	GPIO_InitStruct.Mode 	= GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull	= GPIO_PULLUP;
	GPIO_InitStruct.Speed	= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init (GPIOA,&GPIO_InitStruct);	
}
void readBtn ()
{
	uint8_t temp, row_set = 1, cnt;
	uint16_t col_read;
	uint32_t curtimer = HAL_GetTick();
	static uint32_t prevtimer = 0;
	static uint16_t col_read_first;
	
	if (curtimer - prevtimer > 100)
	{			
		col_read_first = 0;
		for(temp = 0; temp < 4; temp++)
		{			
			GPIOA->BSRR = 0x0F;
			GPIOA->BRR = row_set;
			col_read_first = col_read_first << 4;
			for (cnt=30; cnt>0; cnt--){}
			col_read_first |=(0xF0 & GPIOA->IDR)>> 4;
			row_set = (row_set << 1);			
		}
		GPIOA->BSRR = 0x0F;		
//		if (col_read_first != 0xFFFF)
//		{
//			btn_clik = 1;
//			GPIOB->ODR ^= BUZER_PIN;
//		}
		prevtimer = curtimer;
	}
//	else if ((timer_1ms[3] > 10) && btn_clik == 1)
	else if ((curtimer - prevtimer > 10) && (col_read_first != 0xFFFF))
	{
//		btn_clik = 0;
		for(temp = 0; temp < 4; temp++)
		{
			GPIOA->BSRR = 0x0F;
			GPIOA->BRR = row_set;
			for (cnt=30; cnt>0; cnt--){}
			col_read = col_read << 4;
			col_read |=(0xF0 & GPIOA->IDR)>> 4;
			row_set = (row_set << 1);
		}
		GPIOA->BSRR = 0x0F;		
		
		if (col_read_first == col_read)
		{
			col_read = ~col_read;
			switch (col_read)
			{
				case (1<<BTN_ESC):
					
				break;
				case (1<<BTN_1):
					
				break;
				case (1<<BTN_2):
					if (driwe != 1)
							{					
								driwe = 3;
							}						
				break;
				case (1<<BTN_3):
					
				break;
				case (1<<BTN_F1):
					
				break;
				case (1<<BTN_4):
					if (driwe != 0)
						{
							driwe = 2;
						}							
				break;
				case (1<<BTN_5):
					
				break;
				case (1<<BTN_6):
						if (driwe != 2)
						{
							driwe = 0;
						}								
				break;
				case (1<<BTN_F2):
					
				break;
				case (1<<BTN_7):
					
				break;
				case (1<<BTN_8):
							if (driwe != 3)
						{		
							driwe = 1;
						}
				break;
				case (1<<BTN_9):
					
				break;
				case (1<<BTN_L):
					
				
				break;
				case (1<<BTN_R):
					
				
				break;
				case (1<<BTN_0):
					
				break;
				case (1<<BTN_OK):
						if (flagstrt == 0)
						{
							restart = 1;
						}
				break;
				case (1<<BTN_ESC)|(1<<BTN_OK):
					NVIC_SystemReset();
				break;
			}
		}
		
		col_read_first = 0xFFFF;		
		prevtimer = curtimer;		

	}
}
/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
