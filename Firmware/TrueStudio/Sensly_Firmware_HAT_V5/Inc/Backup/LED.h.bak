/*
 * LED.h
 *
 *  Created on: 2 Aug 2018
 *      Author: Sam
 */

#ifndef LED_H_
#define LED_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

extern TIM_HandleTypeDef htim1;

#define LEDTIMER						htim1
#define BLUE 							TIM_CHANNEL_1
#define GREEN							TIM_CHANNEL_3
#define RED 							TIM_CHANNEL_2

#define LED_RED()						LED_Colour(&htim1, 330, 0, 0); // RED, GREEN, BLUE
#define LED_GREEN()						LED_Colour(&htim1, 0, 330, 0);
#define LED_BLUE()						LED_Colour(&htim1, 0, 0, 330);
#define LED_YELLOW()					LED_Colour(&htim1, 330, 330, 0);
#define LED_PURPLE()					LED_Colour(&htim1, 330, 0, 330);
#define LED_WHITE()						LED_Colour(&htim1, 330, 330, 330);
#define LED_RESET()						LED_Colour(&htim1, 0, 0, 0);

void LED_Init(TIM_HandleTypeDef *LEDtim);
void LED_Control(TIM_HandleTypeDef *htim, uint32_t Colour, uint16_t Brightness);
void LED_Colour(TIM_HandleTypeDef *htim, uint16_t Red_Brightness, uint16_t Green_Brightness, uint16_t Blue_Brightness);
void LED_Fade(TIM_HandleTypeDef *htim);
void LED_Toggle(TIM_HandleTypeDef *htim, uint32_t Colour, uint32_t Delay);

#endif /* LED_H_ */
