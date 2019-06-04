/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "tim.h"
#include "LED.h"
#include "Sensors.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "Flash.h"
#include "gpio.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;

/* USER CODE BEGIN Variables */
osThreadId heatThreadHandle;
extern uint8_t flag;
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
void StartHeatTask(void const * argument);

static int _filter(int m)
{
  static int flag_first = 0, _buff[10], sum;
  const int _buff_max = 10;
  int i;

  if(flag_first == 0)
  {
    flag_first = 1;

    for(i = 0, sum = 0; i < _buff_max; i++)
    {
      _buff[i] = m;
      sum += _buff[i];
    }
    return m;
  }
  else
  {
    sum -= _buff[0];
    for(i = 0; i < (_buff_max - 1); i++)
    {
      _buff[i] = _buff[i + 1];
    }
    _buff[9] = m;
    sum += _buff[9];

    i = sum / 10.0;
    return i;
  }
}
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  osThreadDef(heatTask, StartHeatTask, osPriorityRealtime, 0, 128);
  heatThreadHandle = osThreadCreate(osThread(heatTask), NULL);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */
  uint8_t ArraySize = 5;

  /* Sensor Variables */
  uint8_t cmd = 0x00;				/* I2C command buffer*/
  uint16_t RGB_Buffer[3];			/* LED colour buffer RGB[0] = RED, RGB[1] = Green, RGB[2] = Blue*/
  float Density = 0.00;				/* Density value calclated from PM10 Sensor */

  /* Flash Variables */

  /* ADC Variables */
  uint16_t Data2Send[ArraySize];	/* Buffer for data to be sent via I2C */
  uint32_t ADC_Buffer[ArraySize];	/* Buffer to Store ADC variables as they are read */

  /* Calibrate the ADC peripheral */
  if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
  {
	Error_Handler();
  }
  /* Start the ADC peripheral in Direct Memory Access(DMA) Mode */
  Enable_ADC_DMA(&hadc1, ADC_Buffer);
  /* Infinite loop */
  for(;;)
  {
	  /* Wait for initial command to send via I2C peripheral */
	  I2C_Read(&hi2c1,&cmd);

	  /* */
	  switch (cmd)
	  {
		  case 0x00:
			  LED_WHITE();
			  break;
		  case 0x01:
			  /* Get MQ2 Data from the ADC */
			  Get_Sensor_Data(&hadc1, Data2Send,ADC_Buffer,ArraySize);
			  /* Send MQ2 Data via I2C peripheral */
			  Send_Sensor_Data(&hi2c1, Data2Send, MQ2CMD);
			  /* Reset the ADC peripheral */
			  ADC_Reset(&hadc1, ADC_Buffer);
			  break;

		  case 0x02:
			  /* Get MQ7 Data from the ADC */
			  Get_Sensor_Data(&hadc1, Data2Send,ADC_Buffer,ArraySize);
			  /* Send MQ7 Data via I2C peripheral */
			  Send_Sensor_Data(&hi2c1, Data2Send, MQ7);
			  /* Reset the ADC peripheral */
			  ADC_Reset(&hadc1, ADC_Buffer);
			  break;

		  case 0x03:													// Get MQ135 Data
			  Get_Sensor_Data(&hadc1, Data2Send,ADC_Buffer,ArraySize);
			  Send_Sensor_Data(&hi2c1, Data2Send, MQ135);
			  /* Reset the ADC peripheral */
			  ADC_Reset(&hadc1, ADC_Buffer);
			  break;

		  case 0x04:													//Get PM Data
			  /* Turn on PM LED pin */
			  HAL_GPIO_WritePin(GPIOA, PM_LED_Pin,GPIO_PIN_SET);
			  /* Wait for reading to be ready */
			  PM_Wait();
			  /* Get the PM Data from the ADC */
			  Get_Sensor_Data(&hadc1,Data2Send,ADC_Buffer,ArraySize);
			  /* Take the average of the data read */
			  Data2Send[PM] = _filter(Data2Send[PM]);
			  /* Send PM Data via I2C peripheral */
			  Send_Sensor_Data(&hi2c1, Data2Send, PM);
			  /* Reset the ADC peripheral */
			  ADC_Reset(&hadc1, ADC_Buffer);
			  /* Turn off PM LED pin */
			  HAL_GPIO_WritePin(GPIOA, PM_LED_Pin,GPIO_PIN_RESET);
			  break;

		  case 0x07:													// Set LED Colour
			  /* Get the RGB Values for the LED via I2C*/
			  Fetch_Data(&hi2c1,RGB_Buffer, 3);
			  /* Set the LED colour based on the value received */
			  LED_Colour(&htim1, RGB_Buffer[0], RGB_Buffer[1], RGB_Buffer[2]);
			  break;

		  default:
			  LED_RESET();
			  osDelay(1000);
			  LED_GREEN();
	  }
	osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Application */
void StartHeatTask(void const * argument)
{
	Heat_Sensor(GPIO_PIN_8,0);
	Heat_Sensor(MQ7HEATPIN,FULL_T4);
	Heat_Sensor(GPIO_PIN_15,0);

#ifdef CALIBRATION
	/* Infinite loop */
	for(;;)
	{
	  LED_Fade(&htim1);
	  osDelay(100);
	}
#else
	for(;;)
	{

	  osDelay(110);

	}
#endif
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
