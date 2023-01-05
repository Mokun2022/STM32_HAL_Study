/*
 * bq34z100.c
 *
 *  Created on: Dec 30, 2022
 *      Author: FDATA
 */

#include "bq34z100.h"
#include "i2c.h"
#include <stdio.h>


int32_t bq34z100_read(uint8_t dev, uint8_t reg, uint8_t* data, uint32_t length)
{
	if(HAL_OK == HAL_I2C_Mem_Read(&hi2c1,dev,reg,I2C_MEMADD_SIZE_8BIT, data,length,10))
		return 1;
	else
		return 0;
}


int32_t bq34z100_write(uint8_t dev, uint8_t reg, uint8_t* data, uint32_t length)
{
	if(HAL_OK == HAL_I2C_Mem_Write(&hi2c1,dev,reg,I2C_MEMADD_SIZE_8BIT, data,length,10))
		return 1;
	else
		return 0;
}


unsigned int transBytes2UnsignedInt(unsigned char msb, unsigned char lsb)
{
  unsigned int tmp;
  tmp = ((msb << 8) & 0xFF00);
  return ((unsigned int)(tmp + lsb) & 0x0000FFFF);
}



void bq34z100_test(void)
{
	unsigned char lsb,msb;
	unsigned int  temperature;
	unsigned int  voltage;
	unsigned int  current;


//	bq34z100_read(DVE_SLAVEADDR,TEMPERATURE_CMD_LSB,&lsb,1);
//	bq34z100_read(DVE_SLAVEADDR,TEMPERATURE_CMD_MSB,&msb,1);
//	temperature = transBytes2UnsignedInt(msb, lsb);
////	printf("\r\nmsb:%X\r\nlsb:%X\r\n",msb,lsb);
//	printf("temperature: %d  0.1K\r\n",temperature);
//	HAL_Delay(1000);

	bq34z100_read(DVE_SLAVEADDR,VOLTAGE_CMD_LSB,&lsb,1);
	bq34z100_read(DVE_SLAVEADDR,VOLTAGE_CMD_MSB,&msb,1);
	voltage = transBytes2UnsignedInt(msb, lsb);
//	printf("\r\nmsb:%X\r\nlsb:%X\r\n",msb,lsb);
	printf("voltage: %d mV\r\n",voltage);
	HAL_Delay(1000);

	bq34z100_read(DVE_SLAVEADDR,CURRENT_CMD_LSB,&lsb,1);
	bq34z100_read(DVE_SLAVEADDR,CURRENT_CMD_MSB,&msb,1);
	current = transBytes2UnsignedInt(msb, lsb);
//	printf("\r\nmsb:%X\r\nlsb:%X\r\n",msb,lsb);
	printf("current: %d mA\r\n",current);

	HAL_Delay(1000);

	bq34z100_read(DVE_SLAVEADDR,QUANTITY_CMD,&lsb,1);
	printf("quantity: %d %%\r\n",lsb);
	HAL_Delay(1000);
}


