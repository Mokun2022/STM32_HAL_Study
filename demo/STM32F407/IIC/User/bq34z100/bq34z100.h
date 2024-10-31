/*
 * bq34z100.h
 *
 *  Created on: Dec 30, 2022
 *      Author: FDATA
 */

#ifndef BQ34Z100_BQ34Z100_H_
#define BQ34Z100_BQ34Z100_H_


#define DVE_SLAVEADDR  0xAA

#define VOLTAGE_CMD_LSB  		0X08
#define VOLTAGE_CMD_MSB   		0X09
#define CURRENT_CMD_LSB	 		0X0A
#define CURRENT_CMD_MSB			0X0B
#define QUANTITY_CMD			0X02
#define TEMPERATURE_CMD_LSB		0X0C
#define TEMPERATURE_CMD_MSB		0X0D


void bq34z100_test(void);



#endif /* BQ34Z100_BQ34Z100_H_ */
