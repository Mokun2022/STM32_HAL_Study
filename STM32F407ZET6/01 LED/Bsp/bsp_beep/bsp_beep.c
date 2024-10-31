/*
 * bsp_beep.c
 *
 *  Created on: Mar 8, 2023
 *      Author: Fdata
 */

#include "bsp_beep.h"
#include "gpio.h"

void bsp_beep_init(void)
{
	/*Initialize hardware peripherals*/

	/*Off Beep*/
	HAL_GPIO_WritePin(Beep_GPIO_Port, Beep_Pin, GPIO_PIN_RESET);
}


void bsp_set_beep_on(void)
{
	 HAL_GPIO_WritePin(Beep_GPIO_Port, Beep_Pin, GPIO_PIN_SET);
}


void bsp_set_beep_off(void)
{
	 HAL_GPIO_WritePin(Beep_GPIO_Port, Beep_Pin, GPIO_PIN_RESET);
}
