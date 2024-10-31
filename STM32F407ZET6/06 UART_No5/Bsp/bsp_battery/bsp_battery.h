/*
 * bsp_battary.h
 *
 *  Created on: 2023年3月28日
 *      Author: Fdata
 */

#ifndef BSP_BATTERY_BSP_BATTERY_H_
#define BSP_BATTERY_BSP_BATTERY_H_

#include <stdint.h>
#include <stdbool.h>


typedef struct
{
	//Battery information
	float voltage;		//电池电压
	float current;		//电池电流
	float soc;			//电池电量
	float soh;			//电池健康
	float max_temp;		//电池单节最高温度
	float min_temp;		//电池单节最低温度

	//System state
	bool charger_access;		//充电器接入状态
	bool load_access;			//负载接入状态
	bool charging_state;		//充电状态
	bool discharge_state;		//放电状态
}battery_t;

extern battery_t battery;
extern void bsp_battery_send(void);
extern void bsp_battery_get(battery_t *bat,uint8_t * str);

#endif /* BSP_BATTERY_BSP_BATTERY_H_ */
