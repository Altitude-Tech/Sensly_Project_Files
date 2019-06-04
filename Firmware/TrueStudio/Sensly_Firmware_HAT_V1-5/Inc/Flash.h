/*
 * Flash.h
 *
 *  Created on: 6 Aug 2018
 *      Author: Sam
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define OFF 0
#define ON !OFF

#define READCMD						0x03				// Read Command
#define HSREADCMD					0x0B				// High Speed Read Command
#define SECTORERASECMD				0x20				// Sector Erase Command
#define BLOCKERASECMD				0x52				// Block Erase Command
#define CHIPERASECMD				0x60				// Chip Erase Command
#define BYTEPROGCMD					0x02				// Byte Program Command
#define AAIPROGCMD					0xAF				// Auto Address Increment Program Command
#define RDSRCMD						0x05				// Read Status Register Command
#define EWSRCMD						0x50				// Enable Write Status Register Command
#define WRSRCMD						0x01				// Write Status Register Command
#define WRENCMD						0x06				// Write Enable Command
#define WRDICMD						0x04				// Write Disable Command
#define READIDCMD					0xAB				// Read ID Command
#define GLOBALULKCMD				0x98				//Global unlock command
#define WRT_PROTECT_OFF				0x00
#define WRT_PROTECT_ON				0x0C
#define sFLASH_SECTOR_SIZE			1000
#define NULLCMD						0xFF
#define ssFLASH_CMD_EBSY            0x70				/* Enable SO RY/BY# Status */
#define ssFLASH_CMD_DBSY            0x80				/* Disable SO RY/BY# Status */




void sFLASH_SelectSet(uint8_t Status);
void sFLASH_WriteEnable(SPI_HandleTypeDef *hspi);
void sFLASH_WriteDisable(SPI_HandleTypeDef *hspi);
void sFLASH_ByteProgram(SPI_HandleTypeDef *hspi, uint32_t Data, uint32_t Address);
void sFLASH_AAIProgram(SPI_HandleTypeDef *hspi, uint32_t Address, uint32_t DataBuff[], uint8_t DataNoBytes);
void sFLASH_SendByte(SPI_HandleTypeDef *hspi,  uint32_t Address, uint32_t cmd, uint32_t Data);

void sFLASH_SectorErase(SPI_HandleTypeDef *hspi, uint32_t Address);
void sFLASH_BlockErase(SPI_HandleTypeDef *hspi, uint32_t Address);
void sFLASH_ChipErase(SPI_HandleTypeDef *hspi);

void sFLASH_EnWriteStatReg(SPI_HandleTypeDef *hspi);
void sFLASH_WriteStatReg(SPI_HandleTypeDef *hspi, uint32_t Data);
void sFLASH_WriteEnableGL(SPI_HandleTypeDef *hspi);
void sFLASH_Preserve(SPI_HandleTypeDef *hspi, uint8_t Preserve[], uint16_t Size, uint32_t Address);

uint8_t sFLASH_Write(SPI_HandleTypeDef *hspi, uint32_t Data, uint32_t Address);
uint8_t sFLASH_WriteDP(SPI_HandleTypeDef *hspi, uint32_t Data, uint32_t Address);
uint8_t sFLASH_WriteAAI(SPI_HandleTypeDef *hspi, uint32_t Address, uint32_t Data[], uint8_t ArraySize);

uint32_t sFLASH_Init(SPI_HandleTypeDef *hspi);

uint32_t sFLASH_Read(SPI_HandleTypeDef *hspi, uint32_t Address);
uint32_t sFLASH_ReadStatReg(SPI_HandleTypeDef *hspi);
uint32_t sFLASH_ChkBusy(SPI_HandleTypeDef *hspi);
uint32_t sFLASH_BusyAAI(SPI_HandleTypeDef *hspi);
uint32_t sFLASH_ChkAAIStatus(SPI_HandleTypeDef *hspi);
void sFLASH_ReadID(SPI_HandleTypeDef *hspi, uint32_t Buff[]);

#endif /* FLASH_H_ */
