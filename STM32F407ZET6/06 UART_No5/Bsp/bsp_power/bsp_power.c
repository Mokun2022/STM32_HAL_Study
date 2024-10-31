/*
 * bsp_power.c
 *
 *  Created on: Mar 8, 2023
 *      Author: Fdata
 */

#include "bsp_power.h"
#include "gpio.h"


void bsp_set_power_on(void)
{
	 HAL_GPIO_WritePin(Power5V_GPIO_Port, Power5V_Pin, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(IMU_EN_GPIO_Port, IMU_EN_Pin, GPIO_PIN_SET);
}

void bsp_set_power_off(void)
{
	 HAL_GPIO_WritePin(Power5V_GPIO_Port, Power5V_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(IMU_EN_GPIO_Port, IMU_EN_Pin, GPIO_PIN_RESET);
}
