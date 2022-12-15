/*
 * ultrasonic.c
 *
 *  Created on: Dec 12, 2022
 *      Author: FDATA
 */


#include "ultrasonic.h"
#include "tim.h"
#include "gpio.h"
#include <stdio.h>
#include <stdint.h>

static uint32_t getPeriod = 0;
static Capture capture;


/*
for循环实现延时us
*/
static void ultrasonic_delay_us(uint32_t nus)
{
	 uint32_t Delay = nus * 168/8;
	 do
	 {
		 __NOP();
	 }
	 while (Delay --);
}




void ultrasonic_init(void)
{
	capture.channel = None;
	capture.start = 0;
	capture.finish = 0;
	capture.period = 0;
	capture.count = 0;

	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3);
}




uint8_t ultrasonic_single_measure(Ultrasonic_Channel channel , float *distance)
{
	capture.channel = channel;
	if(0 == capture.finish)	//开始触发
	{
		switch(capture.channel)
		{
			case Front:
				HAL_GPIO_WritePin(GPIOE, Trig0_Pin, GPIO_PIN_RESET);
				ultrasonic_delay_us(30);
//				HAL_Delay(75);
				HAL_GPIO_WritePin(GPIOE, Trig0_Pin, GPIO_PIN_SET);
				break;
			case Left:
				HAL_GPIO_WritePin(GPIOE, Trig1_Pin, GPIO_PIN_RESET);
				ultrasonic_delay_us(30);
//				HAL_Delay(75);
				HAL_GPIO_WritePin(GPIOE, Trig1_Pin, GPIO_PIN_SET);
				break;
			case Right:
				HAL_GPIO_WritePin(GPIOE, Trig2_Pin, GPIO_PIN_RESET);
				ultrasonic_delay_us(30);
//				HAL_Delay(75);
				HAL_GPIO_WritePin(GPIOE, Trig2_Pin, GPIO_PIN_SET);
				break;
			default:
				break;
		}
		return 1;
	}
	else if(1 == capture.finish)//开始计算
	{
		//	处理计数距离
		*distance = capture.period * 17.4 + capture.count * 0.0174;
//		printf("distance :%.2f cm\r\n",*distance);
		capture.start = 0;
		capture.finish = 0;
		return 2;
	}
	else
		return 0;
}

void ultrasonic_multiple_measure(float *distance)
{
	uint8_t index = 0;
	static Ultrasonic_Channel channel_state = Front;

	index = ultrasonic_single_measure(channel_state , &distance[channel_state-1]);
	if(2 == index)
	{
		channel_state++;
		if(channel_state > Right)
			channel_state = Front;
	}
//	printf("%d\r\n",index);
//	printf("%d\r\n",channel_state);
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	//1ms 中断一次
{
	if(TIM1 == htim->Instance)
	{
		getPeriod++;
		if(getPeriod >=800)	//超时处理
		{
			__HAL_TIM_SET_COUNTER(&htim1, 0);	//计数器清0
			getPeriod = 0;
			capture.period = 0;
			capture.count = 0;
			capture.finish = 1;	//强制结束
		}
	}
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(TIM1 == htim->Instance)
	{
		if(0 == capture.start)	//判断上升沿
		{
			switch(capture.channel)
			{
				case Front:
					__HAL_TIM_DISABLE(&htim1);			//先关闭定时器
					getPeriod = 0;
					capture.count=0;
					capture.period=0;
					capture.start = 1;
					__HAL_TIM_SET_COUNTER(&htim1, 0);	//计数器清0
					TIM_RESET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_1);	//重置捕获极性
					TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING);	//下降沿捕获
					__HAL_TIM_ENABLE(&htim1);			//开始计数
					break;
				case Left:
					__HAL_TIM_DISABLE(&htim1);			//先关闭定时器
					getPeriod = 0;
					capture.count=0;
					capture.period=0;
					capture.start = 1;
					__HAL_TIM_SET_COUNTER(&htim1, 0);	//计数器清0
					TIM_RESET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_2);	//重置捕获极性
					TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_2, TIM_ICPOLARITY_FALLING);	//下降沿捕获
					__HAL_TIM_ENABLE(&htim1);			//开始计数
					break;
				case Right:
					__HAL_TIM_DISABLE(&htim1);			//先关闭定时器
					getPeriod = 0;
					capture.count=0;
					capture.period=0;
					capture.start = 1;
					__HAL_TIM_SET_COUNTER(&htim1, 0);	//计数器清0
					TIM_RESET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_3);	//重置捕获极性
					TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_3, TIM_ICPOLARITY_FALLING);	//下降沿捕获
					__HAL_TIM_ENABLE(&htim1);			//开始计数
					break;
				default:
					break;
			}
		}
		else if(1 == capture.start)	//判断下降沿
		{
			switch(capture.channel)
			{
				case Front:
					capture.period = getPeriod;
					capture.count = HAL_TIM_ReadCapturedValue( &htim1, TIM_CHANNEL_1);
					capture.finish = 1;
					TIM_RESET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_1);	//重置捕获极性
					TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING);	//上升沿捕获
					__HAL_TIM_SET_COUNTER(&htim1, 0);	//计数器清0
					break;
				case Left:
					capture.period = getPeriod;
					capture.count = HAL_TIM_ReadCapturedValue( &htim1, TIM_CHANNEL_2);
					capture.finish = 1;
					TIM_RESET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_2);	//重置捕获极性
					TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_2, TIM_ICPOLARITY_RISING);	//上升沿捕获
					__HAL_TIM_SET_COUNTER(&htim1, 0);	//计数器清0
					break;
				case Right:
					capture.period = getPeriod;
					capture.count = HAL_TIM_ReadCapturedValue( &htim1, TIM_CHANNEL_3);
					capture.finish = 1;
					TIM_RESET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_3);	//重置捕获极性
					TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_3, TIM_ICPOLARITY_RISING);	//上升沿捕获
					__HAL_TIM_SET_COUNTER(&htim1, 0);	//计数器清0
					break;
				default:
					break;
			}
		}
	}
}




