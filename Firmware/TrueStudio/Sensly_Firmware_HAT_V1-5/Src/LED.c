/*
 * LED.c
 *
 *  Created on: 2 Aug 2018
 *      Author: Sam
 */

#include "LED.h"
void LED_Init(TIM_HandleTypeDef *LEDtim)
{

	HAL_TIMEx_PWMN_Start(LEDtim, BLUE);
  	HAL_TIMEx_PWMN_Start(LEDtim, GREEN);
  	HAL_TIMEx_PWMN_Start(LEDtim, RED);
}

/* This function sets the PWM mode
 * Parameters
 * TIM_num: This is the timer number for the PWM channel you want to control
 * Colour: This is the channel number for PWM channel you want to control, you can use the macros BLUE, GREEN and RED instead
 * Brightness: This is controls how bright the LEDs will be it is equivalent to the PWM duty cycle value can be from 0 - 255*/

void LED_Control(TIM_HandleTypeDef *htim, uint32_t Colour, uint16_t Brightness)
{
	__HAL_TIM_SET_COMPARE(htim, Colour, Brightness);
}

/* This function blends the three PWM channels to get the full colour spectrum
 * Parameters
 * Red_Brightness: Duty cycle value for the RED led 0-330
 * Blue_Brightness: Duty cycle value for the BLUE led 0-330
 * Green_Brightness: Duty cycle value for the GREEN led 0-330*/
void LED_Colour(TIM_HandleTypeDef *htim, uint16_t Red_Brightness, uint16_t Green_Brightness, uint16_t Blue_Brightness)
{
	LED_Control(htim, RED, Red_Brightness);
	LED_Control(htim, GREEN, Green_Brightness);
	LED_Control(htim, BLUE, Blue_Brightness);

}

/* This function toggles your chosen PWM channel between 0 and MAX duty cycle
 * Parameters
 * TIM_num: This is the timer number for the PWM channel you want to control
 * Colour: This is the channel number for PWM channel you want to control, you can use the macros BLUE, GREEN and RED instead
 * Delay: Time in milliseconds between each toggle.  */
void LED_Toggle(TIM_HandleTypeDef *htim, uint32_t Colour, uint32_t Delay)
{
	LED_Control(htim, Colour, 255);
	osDelay(Delay);
	LED_Control(htim, Colour, 0);
	osDelay(Delay);
}

/* This function is used to cycle through the led colours going from a duty cycle of 0 - 255 for each colour then from 255 - 0 */
void LED_Fade(TIM_HandleTypeDef *htim)
{
	uint16_t x = 330;
	uint16_t y = 0;
	uint16_t z = 0;

	for (int i = 0; i < 331; i ++)
	  {
		  LED_Colour(htim, z, y, x);
		  z++;
		  x--;
		  osDelay(20);
	  }
	for (int i = 0; i < 331; i ++)
	  {
		  LED_Colour(htim, z, y, x);
		  y++;
		  z--;
		  osDelay(20);
	  }
	for (int i = 0; i < 331; i ++)
	  {
		  LED_Colour(htim, z, y, x);
		  x++;
		  y--;
		  osDelay(20);
	  }
	for (int i = 0; i < 331; i ++)
	  {
		  LED_Colour(htim, z, y, x);
		  y++;
		  x--;
		  osDelay(20);
	  }
	for (int i = 0; i < 331; i ++)
	  {
		  LED_Colour(htim, z, y, x);
		  z++;
		  y--;
		  osDelay(20);
	  }
	for (int i = 0; i < 331; i ++)
	  {
		  LED_Colour(htim, z, y, x);
		  x++;
		  z--;
		  osDelay(20);
	  }
	osDelay(1000);   //hold for 1000ms
}
