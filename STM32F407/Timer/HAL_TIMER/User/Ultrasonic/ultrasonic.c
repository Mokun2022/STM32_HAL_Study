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


/***************************************************************
  *  @brief     for循环实现延时us延迟
  *  @param     None
  *  @retval	None
 **************************************************************/
static void ultrasonic_delay_us(uint32_t nus)
{
	 uint32_t Delay = nus * 168/8;
	 do
	 {
		 __NOP();
	 }
	 while (Delay --);
}


TIM_HandleTypeDef htim1;

/***************************************************************
  *  @brief     超声波外设初始化
  *  @param     None
  *  @retval	None
 **************************************************************/
void ultrasonic_init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_IC_InitTypeDef sConfigIC = {0};
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE();

	/*Configure GPIO pins : PEPin PEPin PEPin */
   GPIO_InitStruct.Pin = Echo0_Pin|Echo1_Pin|Echo2_Pin;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_PULLDOWN;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
   GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
   HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

   HAL_GPIO_WritePin(GPIOE, Trig0_Pin|Trig1_Pin|Trig2_Pin, GPIO_PIN_SET);

	/**TIM1 GPIO Configuration
    PE9     ------> TIM1_CH1
    PE11     ------> TIM1_CH2
    PE13     ------> TIM1_CH3
    */
	GPIO_InitStruct.Pin = Trig0_Pin|Trig1_Pin|Trig2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	 /* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 167;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 999;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim1);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig);
	HAL_TIM_IC_Init(&htim1);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);

	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;

	HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1);
	HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_2);
	HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_3);

    /* TIM1 interrupt Init */
    HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);

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



/***************************************************************
  *  @brief     单通道超声波测距
  *  @param     channel 超声波通道
  *  @param     *distance 距离
  *  @retval	测量状态
 **************************************************************/
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


/***************************************************************
  *  @brief     多通道超声波轮询测量
  *  @param     *distance 存储距离的数组
  *  @retval	None
 **************************************************************/
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


/***************************************************************
  *  @brief     update定时器回调函数
  *  @param     定时器句柄
  *  @retval	None
 **************************************************************/
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

/***************************************************************
  *  @brief     Capture回调函数
  *  @param     定时器句柄
  *  @retval	None
 **************************************************************/
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




