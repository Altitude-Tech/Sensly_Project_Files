/*
 * Sensors.h
 *
 *  Created on: 2 Aug 2018
 *      Author: Sam
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "tim.h"

//#define MQ2								0
#define MQ2CMD							0
#define MQ7								1
#define MQ135							2
#define PM_2_5							3
#define PM								4

#define MQ2CHANNEL						htim4
#define MQ2HEATPIN						TIM_CHANNEL_3

#define MQ7CHANNEL						htim4
#define MQ7HEATPIN						TIM_CHANNEL_4

#define MQ135CHANNEL					0//htim2
#define MQ135HEATPIN					TIM_CHANNEL_1

#define FULL_T2							330
#define FULL_T4 						10000
#define _1_4_V 							2800

#define NOPWM							0
#define PWMSET 							!NOPWM

#define SENSOR_TIME_HIGH				900	// 900 = 90 seconds
#define SENSOR_TIME_LOW					600//  600 = 60 seconds
#define LOADRESISTANCE					10 // Load resistance in Kilo ohms

#define CALIBARAION_SAMPLE_TIMES	    100		// Number of sample times when calibrating
#define CALIBRATION_SAMPLE_INTERVAL		1000	// Time between samples when calibrating
#define READ_SAMPLE_TIMES				10		// Number of Samples to be taken when reading sensors
#define READ_SAMPLE_INTERVAL			100		// Time between samples

#define SAMPLING_TIME 				 	280
#define DELTA_TIME 						40
#define SLEEP_TIME 						9680
#define CONVRRTIO						0.2
#define NODUSTVOLTAGE					500		//millivolts

#define ID_ADDR							0x01FEFF
#define MQ2_R0_ADDR						0x007000
#define MQ7_R0_ADDR						0x008000
#define MQ7_FLAG_ADDR					0x008017
#define MQ135_R0_ADDR					0x009000


#define ADDR_BYTES						3
#define DATAMASK						0xFF
#define TIMEMAX							1000	//Milliseconds
#define I2C_MAX_RESET_FAIL_COUNT 		20		//Milliseconds

#define CALIBRATION


void HeaterPWM_Init();
void Counter_Init(TIM_HandleTypeDef *COUNTtim);
void Get_Sensor_Data(ADC_HandleTypeDef* hadc, uint16_t Data2Send[], uint32_t adcBuffer[], uint8_t ArraySize);
uint16_t Get_Sensor_Data_Avg(ADC_HandleTypeDef* hadc, uint32_t adcBuffer[], uint8_t Sensor);

void Send_Sensor_Data(I2C_HandleTypeDef *hi2c, uint16_t Data2Send[], uint8_t Sensor);
void Send_Sensor_Data_Batch(I2C_HandleTypeDef *hi2c, uint16_t Data2Send[],  uint8_t aTxBuffer[], uint8_t ArraySize);
void Heat_Sensor(uint16_t HeatPin, uint16_t DutyCycle);
void Fetch_Data(I2C_HandleTypeDef *hi2c, uint16_t aRxBuffer[], uint8_t NumOfBytes);
void Store_Data(SPI_HandleTypeDef *hspi, uint8_t aRxBuffer[], uint8_t NumOfBytes, uint32_t Address);
void Send_Data(I2C_HandleTypeDef *hi2c, uint8_t Data_Buffer[], uint8_t NumOfBytes);

uint32_t Read_Data(SPI_HandleTypeDef *hspi, uint32_t Address, uint8_t Data_Buffer[],uint8_t NumOfBytes);
float Raw_2_Resistance(uint16_t RawData);
float Calc_RZero(float RSAir);

void MQCalibration(ADC_HandleTypeDef* hadc, uint16_t AvRawData[]);
float MQGetRSRORatio(float RS, float R0);
float MQGetPPM(float RS_R0, float gas_curve[]);
uint16_t PMGetRaw(ADC_HandleTypeDef* hadc, uint16_t Data2Send[], uint32_t* ADC_Buffer, uint8_t ArraySize);
float PMGetVoltage(uint16_t RawData);
float PMGetDensity(uint16_t RawData);
void PM_Wait();




#endif /* SENSORS_H_ */
