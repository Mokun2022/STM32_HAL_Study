/*
 * ultrasonic.h
 *
 *  Created on: Dec 12, 2022
 *      Author: FDATA
 */

#ifndef ULTRASONIC_ULTRASONIC_H_
#define ULTRASONIC_ULTRASONIC_H_

#include "main.h"

#define Echo0_Pin GPIO_PIN_9
#define Echo0_GPIO_Port GPIOE
#define Trig0_Pin GPIO_PIN_10
#define Trig0_GPIO_Port GPIOE
#define Echo1_Pin GPIO_PIN_11
#define Echo1_GPIO_Port GPIOE
#define Trig1_Pin GPIO_PIN_12
#define Trig1_GPIO_Port GPIOE
#define Echo2_Pin GPIO_PIN_13
#define Echo2_GPIO_Port GPIOE
#define Trig2_Pin GPIO_PIN_14
#define Trig2_GPIO_Port GPIOE


typedef enum
{
	None=0,
	Front,
	Left,
	Right,
}Ultrasonic_Channel;


typedef struct
{
	Ultrasonic_Channel channel;	//通道
	uint8_t start;		//捕获开始标志
	uint8_t finish;		//捕获结束标志
	uint32_t count;		// TIM计数值
	uint32_t period;	// TIM溢满次数
}Capture;

extern TIM_HandleTypeDef htim1;

void ultrasonic_init(void);
uint8_t ultrasonic_single_measure(Ultrasonic_Channel channel , float *distance);
void ultrasonic_multiple_measure(float *distance);

#endif /* ULTRASONIC_ULTRASONIC_H_ */
