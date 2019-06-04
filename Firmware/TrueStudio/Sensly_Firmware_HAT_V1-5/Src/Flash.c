/*
 * Flash.c
 *
 *  Created on: 6 Aug 2018
 *      Author: Sam
 */

#include "Flash.h"
#include "spi.h"

uint32_t sFLASH_Init(SPI_HandleTypeDef *hspi)
{
	 uint32_t Device_ID[2] = {0};

	 uint32_t Buff = 0;
     sFLASH_SelectSet(ON);

	 sFLASH_WriteDisable(hspi);

	 sFLASH_ReadID(hspi, Device_ID);

	 sFLASH_EnWriteStatReg(hspi);

	 sFLASH_WriteStatReg(hspi, WRT_PROTECT_OFF);

	 while(sFLASH_ChkBusy(hspi) == 1)
	  {

	  }

	 Buff = sFLASH_ReadStatReg(hspi);
	 return Buff;

}

void sFLASH_SelectSet(uint8_t Status)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, Status);
}

void sFLASH_WriteEnableGL(SPI_HandleTypeDef *hspi)
{
	uint32_t cmd = GLOBALULKCMD;
	uint32_t Buff;

	sFLASH_SelectSet(OFF);
	SPI_Send_N_Read(hspi, &cmd, &Buff, 1);	// Send write enable command
	sFLASH_SelectSet(ON);

}
void sFLASH_WriteEnable(SPI_HandleTypeDef *hspi)
{
	uint32_t cmd = WRENCMD;
	uint32_t Buff;

	sFLASH_SelectSet(OFF);
	SPI_Send_N_Read(hspi, &cmd, &Buff, 1);	// Send write enable command
	sFLASH_SelectSet(ON);

}

void sFLASH_WriteDisable(SPI_HandleTypeDef *hspi)
{
	uint32_t cmd = WRDICMD;
	uint32_t Buff;

	sFLASH_SelectSet(OFF);
	SPI_Send_N_Read(hspi, &cmd, &Buff, 1);	// Send write disable command
	sFLASH_SelectSet(ON);

}

uint32_t sFLASH_Read(SPI_HandleTypeDef *hspi, uint32_t Address)
{
	uint32_t cmd = READCMD;
	uint32_t RxData,nullcmd = 0;
	uint32_t Buff = 0;

	sFLASH_SelectSet(OFF);

	SPI_Send_N_Read(hspi, &cmd, &RxData, 1);

	SPI_Send_N_Read(hspi, &Address, &RxData, 3);

	Buff = SPI_Send_N_Read(hspi, &nullcmd, &RxData, 1);
	sFLASH_SelectSet(ON);
	return Buff;
}

uint8_t sFLASH_Write(SPI_HandleTypeDef *hspi, uint32_t Data, uint32_t Address)
{
	uint8_t Result = 0;
	uint32_t Buff = 0;
	sFLASH_EnWriteStatReg(hspi);
	sFLASH_WriteStatReg(hspi, WRT_PROTECT_OFF);
	Buff = sFLASH_Read(hspi, Address);
	if (Buff != 0xFF)
	{
		sFLASH_SectorErase(hspi, Address);
		while(sFLASH_ChkBusy(hspi) == 1)
					  {

					  }
	}
	sFLASH_ByteProgram(hspi, Data,Address);
	while(sFLASH_ChkBusy(hspi) == 1)
	  {

	  }
	osDelay(100);
	Buff = sFLASH_Read(hspi, Address);

	if (Buff == Data)
	{
		Result = 1;
	}
	else
	{
		Result = 0;
	}
	return Result;
}

void sFLASH_Preserve(SPI_HandleTypeDef *hspi, uint8_t Preserve[], uint16_t Size, uint32_t Address)
{

	for(int i = 0; i < Size; i++)
	{
		Preserve[i] = sFLASH_Read(hspi, Address);

	}
}

uint8_t sFLASH_Over_Write_Sector(SPI_HandleTypeDef *hspi, uint16_t PreserveSize, uint16_t DataSize, uint32_t Data[], uint32_t Address)
{

	uint8_t Preserve[PreserveSize];
	uint8_t Status = 0;

	sFLASH_Preserve(hspi, Preserve, PreserveSize, Address);

	if (DataSize < PreserveSize)
	{
		for (int i = 0; i < DataSize; i++)
		{
			Preserve[i] = Data[i];
		}
		for(int j = 0; j < PreserveSize; j++)
		{
			Status = sFLASH_Write(hspi, Preserve[j], Address);
			Address++;
		}
	}

	return Status;

}

uint8_t sFLASH_WriteAAI(SPI_HandleTypeDef *hspi, uint32_t Address, uint32_t Data[], uint8_t ArraySize)
{
	uint32_t Buff = 0;

	sFLASH_EnWriteStatReg(hspi);
	sFLASH_WriteStatReg(hspi, WRT_PROTECT_OFF);
	Buff = sFLASH_Read(hspi, Address);
	if (Buff != 0xFF)
	{
		sFLASH_SectorErase(hspi, Address);
		while(sFLASH_ChkBusy(hspi) == 1)
		  {

		  }
	}

	while(sFLASH_ChkBusy(hspi) == 1)
	{

	}

	sFLASH_AAIProgram(hspi, Address, Data, ArraySize);
	osDelay(100);

	return 0;
}

void sFLASH_SendByte(SPI_HandleTypeDef *hspi,  uint32_t Address, uint32_t cmd, uint32_t Data)
{
	uint32_t RxData= 0;
	uint32_t Buff = 0;
	SPI_Send_N_Read(hspi, &cmd, &Buff, 1);		// Send byte program command
	SPI_Send_N_Read(hspi, &Address,&RxData, 3);			// Send start address
	SPI_Send_N_Read(hspi, &Data, &Buff, 1);		// Send data to be stored
}

void sFLASH_ByteProgram(SPI_HandleTypeDef *hspi, uint32_t Data, uint32_t Address)
{
	uint32_t cmd = BYTEPROGCMD;
	uint32_t RxData= 0;
	uint32_t Buff = 0;

	sFLASH_WriteEnable(hspi);
	sFLASH_SelectSet(OFF);
	SPI_Send_N_Read(hspi, &cmd, &Buff, 1);		// Send byte program command
	SPI_Send_N_Read(hspi, &Address,&RxData, 3);			// Send start address
	SPI_Send_N_Read(hspi, &Data, &Buff, 1);		// Send data to be stored
	sFLASH_SelectSet(ON);
	sFLASH_WriteDisable(hspi);
}

void sFLASH_AAIProgram(SPI_HandleTypeDef *hspi, uint32_t Address, uint32_t DataBuff[], uint8_t DataNoBytes)
{
	uint32_t cmd = AAIPROGCMD;
	uint8_t Status = 0;
	uint32_t Buff;
	uint8_t i = 0;


	sFLASH_WriteEnable(hspi);

	//Start Sequence
	sFLASH_SelectSet(OFF);
	sFLASH_SendByte(hspi, Address, cmd, DataBuff[0]);
	sFLASH_SelectSet(ON);
	Status = sFLASH_BusyAAI(hspi);
	while(Status == 1)
	{
		Status = sFLASH_BusyAAI(hspi);
	}
	while (i < DataNoBytes)
	{
		if (Status == 0)
		{
			sFLASH_SelectSet(OFF);
			SPI_Send_N_Read(hspi, &cmd, &Buff, 1);
			SPI_Send_N_Read(hspi, &DataBuff[i], &Buff, 1);
			osDelay(10);
			i++;

		}
		Status = sFLASH_BusyAAI(hspi);
	}

	sFLASH_SelectSet(ON);
	while(Status == 1)
	{
		Status = sFLASH_BusyAAI(hspi);
	}
	sFLASH_WriteDisable(hspi);
	Status = sFLASH_ChkAAIStatus(hspi);
	while (sFLASH_ChkAAIStatus(hspi) == 1)
	{

	}

}

void sFLASH_SectorErase(SPI_HandleTypeDef *hspi, uint32_t Address)
{
	uint32_t cmd = SECTORERASECMD;
	uint32_t NumOfBytes = 3;
	uint32_t Buff;

	sFLASH_WriteEnable(hspi);  // Enable write access of flash chip
	sFLASH_SelectSet(OFF);
	SPI_Send_N_Read(hspi, &cmd, &Buff, 1); // Send commmand to begin sector erase
	SPI_Send_N_Read(hspi, &Address,&Buff, NumOfBytes);	  //  Send start address of block to be erased
	sFLASH_SelectSet(ON);
	sFLASH_WriteDisable(hspi);  // Disable write access
}

void sFLASH_BlockErase(SPI_HandleTypeDef *hspi, uint32_t Address)
{
	uint32_t cmd = BLOCKERASECMD;
	uint32_t NumOfBytes = 3;
	uint32_t Buff;

	sFLASH_WriteEnable(hspi);	// Enable write access
	sFLASH_SelectSet(OFF);
	SPI_Send_N_Read(hspi, &cmd, &Buff, 1);	//Send command to begin block erase
	SPI_Send_N_Read(hspi, &Address, &Buff, NumOfBytes); // Send start address of block to be erased
	sFLASH_SelectSet(ON);
	sFLASH_WriteDisable(hspi);	// Disable write access
}

void sFLASH_ChipErase(SPI_HandleTypeDef *hspi)
{
	uint32_t cmd = CHIPERASECMD;
	uint32_t Buff;
	sFLASH_WriteEnable(hspi);	// Enable write access of flash chip
	sFLASH_SelectSet(OFF);
	SPI_Send_N_Read(hspi, &cmd, &Buff, 1); // Send chip erase command
	sFLASH_SelectSet(ON);
	sFLASH_WriteDisable(hspi);	// Disable write access
}


uint32_t sFLASH_ReadStatReg(SPI_HandleTypeDef *hspi)
{
	uint32_t cmd = RDSRCMD;
	uint32_t nullcmd = 0xFF;
	uint32_t Data, Buff = 0x00;

	sFLASH_SelectSet(OFF);
	SPI_Send_N_Read(hspi, &cmd, &Buff, 1);	// Send read register command
	Data = SPI_Send_N_Read(hspi, &nullcmd, &Buff, 1);	//Read SPI Rx register
	sFLASH_SelectSet(ON);

	return Data;
}

uint32_t sFLASH_ChkBusy(SPI_HandleTypeDef *hspi)
{
	uint32_t StatReg, BusyFlg;
	uint32_t BusyMsk = 0x01;

	StatReg = sFLASH_ReadStatReg(hspi);	// Read status register
	if (StatReg && BusyMsk == 0x01)			// Compare status register and busy mask if the same set busy flag
	{
		BusyFlg = 1;
	}
	else
	{
		BusyFlg = 0;
	}

	return BusyFlg;
}

uint32_t sFLASH_BusyAAI(SPI_HandleTypeDef *hspi)
{
	volatile uint32_t StatReg, BusyFlg = 0;
	uint32_t BusyMsk = 0x43;

	StatReg = sFLASH_ReadStatReg(hspi);	// Read status register
	if (StatReg && BusyMsk == 0x43)			// Compare status register and busy mask if the same set busy flag
	{
		BusyFlg = 1;
	}
	else
	{
		BusyFlg = 0;
	}

	return BusyFlg;
}

uint32_t sFLASH_ChkAAIStatus(SPI_HandleTypeDef *hspi)
{
	uint32_t StatReg_1, AAIFlg;
	uint32_t AAIMsk = 0x40;

	StatReg_1 = sFLASH_ReadStatReg(hspi);	// Read status register
	if (StatReg_1 && AAIMsk == 0x40)			// Compare status register and AAImask if the same set busy flag
	{
		AAIFlg = 1;
	}
	else
	{
		AAIFlg = 0;
	}

	return AAIFlg;
}

void sFLASH_EnWriteStatReg(SPI_HandleTypeDef *hspi)
{
	uint32_t cmd = EWSRCMD;
	uint32_t Buff;

	sFLASH_SelectSet(OFF);
	SPI_Send_N_Read(hspi, &cmd, &Buff, 1);
	sFLASH_SelectSet(ON);

}

void sFLASH_WriteStatReg(SPI_HandleTypeDef *hspi, uint32_t Data)
{
	uint32_t cmd = WRSRCMD;
	uint32_t Buff;

	sFLASH_SelectSet(OFF);
	SPI_Send_N_Read(hspi, &cmd, &Buff, 1);
	SPI_Send_N_Read(hspi, &Data, &Buff, 1);
	sFLASH_SelectSet(ON);
}

void sFLASH_ReadID(SPI_HandleTypeDef *hspi, uint32_t Buff[])
{
	uint32_t cmd = READIDCMD;
	uint32_t ID_Address = 0x000001;
	uint32_t RxData = 0;
	uint32_t nullcmd = 0xFF;


	sFLASH_SelectSet(OFF);

	SPI_Send_N_Read(hspi, &cmd, &RxData, 1);

	SPI_Send_N_Read(hspi, &ID_Address, &RxData, 3);

	Buff[0] = SPI_Send_N_Read(hspi, &nullcmd, &RxData, 1);
	Buff[1] = SPI_Send_N_Read(hspi, &nullcmd, &RxData, 1);
	sFLASH_SelectSet(ON);

}


