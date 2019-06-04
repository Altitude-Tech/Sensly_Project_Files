/*
 * Sensors.c
 *
 *  Created on: 2 Aug 2018
 *      Author: Sam
 */


#include "Sensors.h"
#include "i2c.h"
#include "math.h"
#include "LED.h"
#include "adc.h"

extern uint8_t SequenceCompletedFlag;

/* A function to initialse the PWM peripheral for the Sensor Heat Pins */
void HeaterPWM_Init()
{

	HAL_TIM_PWM_Start(&MQ7CHANNEL, MQ7HEATPIN);

}

void Counter_Init(TIM_HandleTypeDef *COUNTtim)
{
	HAL_TIM_Base_Start_IT(COUNTtim);
}

/* This function is used to heat the sensors by either turning on a pin so polling the PWM duty cycle
 * Parameters:
 * HeatPin: Sensor pin you want to heat up
 * DutyCycle: Value you want the voltage to be. Range 0 - 330 representing 0 - 3.3 Volts
 */
void Heat_Sensor(uint16_t HeatPin, uint16_t DutyCycle)
{
	if (HeatPin == MQ2HEATPIN)
	{

	}
	else if (HeatPin == MQ7HEATPIN)
	{
		__HAL_TIM_SET_COMPARE(&MQ7CHANNEL, HeatPin, DutyCycle);

	}
	else if (HeatPin == MQ135HEATPIN)
	{

	}
	else if (HeatPin == GPIO_PIN_8)
	{
		HAL_GPIO_WritePin(GPIOB, HeatPin,GPIO_PIN_SET); // Set Sensor heat pin to high, turning on the transistor
	}
	else if (HeatPin == GPIO_PIN_15)
	{
		HAL_GPIO_WritePin(GPIOA, HeatPin,GPIO_PIN_SET); // Set Sensor heat pin to high, turning on the transistor
	}

}

/* This function reads the ADC peripheral and stores the values in an array
 * Parameters:
 * Data2Send[]: Array to store the ADC values after being read
 * adcBuffer[]: pointer to the buffer that will store the ADC values/conversion
 * ArraySize: Size of the array
 */
void Get_Sensor_Data(ADC_HandleTypeDef* hadc, uint16_t Data2Send[], uint32_t adcBuffer[], uint8_t ArraySize)
{
	uint8_t i = 0;


	/*##-5- Get the converted value of regular channel  ########################*/
	HAL_ADC_GetValue(hadc);

	/* Add new data to Data2Send array */
	for (i = 0; i < ArraySize; i++)
	{
		Data2Send[i] = adcBuffer[i];
	}
}

/* This function Sends the sensor data via the I2C peripheral
 * Parameters:
 * Data2Send: As the name implies this is the data to be sent
 * aTxBuffer: Buffer where data to be sent is stored
 * Sensor: Sensor data you need to send.
 */
void Send_Sensor_Data(I2C_HandleTypeDef *hi2c, uint16_t Data2Send[], uint8_t Sensor)
{
		uint32_t Data = Data2Send[Sensor];
		I2C_Send_N_Bytes(hi2c, Data, 2);

}


/************************************** PM Functions *********************************************************/

uint16_t PMGetRaw(ADC_HandleTypeDef* hadc, uint16_t Data2Send[], uint32_t* ADC_Buffer, uint8_t ArraySize)
{
	  HAL_GPIO_WritePin(GPIOA, PM_LED_Pin,GPIO_PIN_SET);
	  PM_Wait();
	  Get_Sensor_Data(hadc,Data2Send,ADC_Buffer,ArraySize);
	  ADC_Reset(hadc, ADC_Buffer);
	  HAL_GPIO_WritePin(GPIOA, PM_LED_Pin,GPIO_PIN_RESET);
	  return Data2Send[PM];
}

float PMGetVoltage(uint16_t RawData)
{
	float PMRaw = RawData;
	return (3300.00/4095.00)* PMRaw*11.00;
}

float PMGetDensity(uint16_t RawData)
{
	float Density,PMVolt = 0.00;

	PMVolt = PMGetVoltage(RawData);

	if (PMVolt >= NODUSTVOLTAGE)
	{
		PMVolt -= NODUSTVOLTAGE;
		Density = PMVolt * CONVRRTIO;
	}
	else
	{
		Density = 0;
	}
	return Density;
}

void PM_Wait()
{
	osDelay(280);
}

void Fetch_Data(I2C_HandleTypeDef *hi2c, uint16_t aRxBuffer[], uint8_t NumOfBytes)
{
	uint8_t i;
	for(i=0;i<NumOfBytes; i++)
	{
		I2C_Read(hi2c,(uint8_t *)&aRxBuffer[i]);
		osDelay(10);
	}
}
