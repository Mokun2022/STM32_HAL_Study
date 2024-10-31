/*
 * bsp_battary.c
 *
 *  Created on: 2023年3月28日
 *      Author: Fdata
 */

#include <stdio.h>
#include "bsp_battery.h"
#include "../bsp_uart/bsp_uart.h"


battery_t battery;

static uint8_t Data[]={0x3A,0x7E,0x01,0x01,0x1E,0x00,0xD8};



void bsp_battery_send(void)
{
	bsp_uart_send(USART6,Data,sizeof(Data)/sizeof(uint8_t));
}


void bsp_battery_get(battery_t *bat,uint8_t * str)
{
	uint8_t *ptr = str;
	uint8_t sum = 0;

	if(*ptr == 0x3B)
	{
		for(int i=0;i<44;i++)
		{
			sum+=*(ptr+i);
		}
		if(*(str+44) == sum)
		{
			bat->voltage = ((*(str+7) << 8) | *(str+6)) * 0.01;
			bat->current = ((*(str+9) << 8) | *(str+8));
			if(bat->current >= 0x7fff)		//电流正负值转化，最高位为符号位
				bat->current = (bat->current - 0xffff) * 0.01;
			else
				bat->current *= 0.01;
			bat->soc = ((*(str+11) << 8) | *(str+10)) * 0.02;
			bat->soh = ((*(str+13) << 8) | *(str+12));
			bat->max_temp = (((*(str+25) << 8) | *(str+24)) - 2731) * 0.1;
			bat->min_temp = (((*(str+29) << 8) | *(str+28)) - 2731) * 0.1;
			bat->load_access = ((*(str+42)) & 0x40) >> 6;
			bat->charger_access = ((*(str+42)) & 0x20) >> 5;
			bat->charging_state = ((*(str+43)) & 0x02) >> 1;
			bat->discharge_state = ((*(str+43)) & 0x04) >> 2;

			printf("voltage:%.2f  current:%.2f  soc:%.2f  soh:%.2f\r\n",bat->voltage,bat->current,bat->soc,bat->soh);
			printf("max_temp:%.2f  min_temp:%.2f\r\n",bat->max_temp,bat->min_temp);
			printf("load_access:%d  charger_access:%d  charging_state:%d  discharge_state:%d\r\n\r\n",bat->load_access,bat->charger_access,bat->charging_state,bat->discharge_state);
		}
	}
}







