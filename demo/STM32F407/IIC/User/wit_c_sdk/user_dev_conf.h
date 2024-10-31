/*
 * user_dev_conf.h
 *
 *  Created on: Dec 30, 2022
 *      Author: FDATA
 */

#ifndef WIT_C_SDK_USER_DEV_CONF_H_
#define WIT_C_SDK_USER_DEV_CONF_H_


#define ACC_UPDATE		0x01
#define GYRO_UPDATE		0x02
#define ANGLE_UPDATE	0x04
#define MAG_UPDATE		0x08
#define READ_UPDATE		0x80


typedef struct
{
	float x;
	float y;
	float z;
}data_typef;

typedef struct
{
	data_typef acc;
	data_typef gyro;
	data_typef angle;
}dev_typef;


void imu_init(void);
void imu_get_data(dev_typef *imu);


#endif /* WIT_C_SDK_USER_DEV_CONF_H_ */
