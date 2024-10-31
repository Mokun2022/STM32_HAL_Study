#ifndef _BSP_KEY__H_
#define _BSP_KEY__H_

#include "gpio.h"

#define GET_KEY_GPIO	HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin)

uint8_t bsp_key_scanf(void);

#endif
