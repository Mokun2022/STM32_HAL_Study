/*
 * user_dev_conf.c
 *
 *  Created on: Dec 30, 2022
 *      Author: FDATA
 */


#include "user_dev_conf.h"
#include "i2c.h"
#include <stdio.h>
#include "../wit_c_sdk/wit_c_sdk.h"


static volatile char s_cDataUpdate = 0, s_cCmd = 0xff;


int32_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t *data, uint32_t length)
{
	if(HAL_OK == HAL_I2C_Mem_Read(&hi2c2,dev,reg,I2C_MEMADD_SIZE_8BIT, data,length,1000))
		return 1;
	else
		return 0;
}


int32_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t* data, uint32_t length)
{
	if(HAL_OK == HAL_I2C_Mem_Write(&hi2c2,dev,reg,I2C_MEMADD_SIZE_8BIT, data,length,1000))
		return 1;
	else
		return 0;
}




void CopeCmdData(unsigned char ucData)
{
	static unsigned char s_ucData[50], s_ucRxCnt = 0;

	s_ucData[s_ucRxCnt++] = ucData;
	if(s_ucRxCnt<3)return;										//Less than three data returned
	if(s_ucRxCnt >= 50) s_ucRxCnt = 0;
	if(s_ucRxCnt >= 3)
	{
		if((s_ucData[1] == '\r') && (s_ucData[2] == '\n'))
		{
			s_cCmd = s_ucData[0];
			memset(s_ucData,0,50);//
			s_ucRxCnt = 0;
		}
		else
		{
			s_ucData[0] = s_ucData[1];
			s_ucData[1] = s_ucData[2];
			s_ucRxCnt = 2;
		}
	}

}


//static void ShowHelp(void)
//{
//	printf("\r\n************************	 WIT_SDK_DEMO	************************");
//	printf("\r\n************************          HELP           ************************\r\n");
//	printf("UART SEND:a\\r\\n   Acceleration calibration.\r\n");
//	printf("UART SEND:m\\r\\n   Magnetic field calibration,After calibration send:   e\\r\\n   to indicate the end\r\n");
//	printf("UART SEND:U\\r\\n   Bandwidth increase.\r\n");
//	printf("UART SEND:u\\r\\n   Bandwidth reduction.\r\n");
//	printf("UART SEND:B\\r\\n   Baud rate increased to 115200.\r\n");
//	printf("UART SEND:b\\r\\n   Baud rate reduction to 9600.\r\n");
//	printf("UART SEND:h\\r\\n   help.\r\n");
//	printf("******************************************************************************\r\n");
//}

//static void CmdProcess(void)
//{
//	switch(s_cCmd)
//	{
//		case 'a':
//				if(WitStartAccCali() != WIT_HAL_OK)
//					printf("\r\nSet AccCali Error\r\n");
//			break;
//		case 'm':
//				if(WitStartMagCali() != WIT_HAL_OK)
//					printf("\r\nStart MagCali Error\r\n");
//			break;
//		case 'e':
//				if(WitStopMagCali() != WIT_HAL_OK)
//					printf("\r\nEnd MagCali Error\r\n");
//			break;
//		case 'u':
//				if(WitSetBandwidth(BANDWIDTH_5HZ) != WIT_HAL_OK)
//					printf("\r\nSet Bandwidth Error\r\n");
//			break;
//		case 'U':
//				if(WitSetBandwidth(BANDWIDTH_256HZ) != WIT_HAL_OK)
//					printf("\r\nSet Bandwidth Error\r\n");
//			break;
//		case 'B':
//				if(WitSetUartBaud(WIT_BAUD_115200) != WIT_HAL_OK)
//					printf("\r\nSet Baud Error\r\n");
//			break;
//		case 'b':
//				if(WitSetUartBaud(WIT_BAUD_9600) != WIT_HAL_OK)
//					printf("\r\nSet Baud Error\r\n");
//			break;
//		case 'h':
//				ShowHelp();
//			break;
//		default : return ;
//	}
//	s_cCmd = 0xff;
//}

static void CopeSensorData(uint32_t uiReg, uint32_t uiRegNum)
{
	int i;
    for(i = 0; i < uiRegNum; i++)
    {
        switch(uiReg)
        {
//            case AX:
//            case AY:
            case AZ:
				s_cDataUpdate |= ACC_UPDATE;
            break;
//            case GX:
//            case GY:
            case GZ:
				s_cDataUpdate |= GYRO_UPDATE;
            break;
//            case HX:
//            case HY:
            case HZ:
				s_cDataUpdate |= MAG_UPDATE;
            break;
//            case Roll:
//            case Pitch:
            case Yaw:
				s_cDataUpdate |= ANGLE_UPDATE;
            break;
            default:
				s_cDataUpdate |= READ_UPDATE;
			break;
        }
		uiReg++;
    }
}

static void Delayms(uint16_t ucMs)
{
	HAL_Delay(ucMs);
}

static void AutoScanSensor(void)
{
	int i, iRetry;

	for(i = 0; i < 0x7F; i++)
	{
		WitInit(WIT_PROTOCOL_I2C, i);
		iRetry = 2;
		do
		{
			s_cDataUpdate = 0;
			WitReadReg(AX, 3);
			HAL_Delay(5);
			if(s_cDataUpdate != 0)
			{
				printf("find %02X addr sensor\r\n", i);
//				ShowHelp();
				return ;
			}
			iRetry--;
		}while(iRetry);
	}
	printf("can not find sensor\r\n");
	printf("please check your connection\r\n");
}

















void imu_init(void)
{
	WitInit(WIT_PROTOCOL_I2C, 0x50);	//初始化IIC协议，设置设备地�???
	WitI2cFuncRegister(IICwriteBytes, IICreadBytes); //注册IIC的读和写回调函数
	WitRegisterCallBack(CopeSensorData);	//注册获取传感器数据回调函�???
	WitDelayMsRegister(Delayms);  //注册延迟函数
	AutoScanSensor();		//自动搜索传感�???
}

void imu_get_data(dev_typef *imu)
{
	float fAcc[3], fGyro[3], fAngle[3];
	int i;

	WitReadReg(AX, 12);	//间隔的读传感器数据
	if(s_cDataUpdate)
	{
		for(i = 0; i < 3; i++)
		{
			fAcc[i] = sReg[AX+i] / 32768.0f * 16.0f;
			fGyro[i] = sReg[GX+i] / 32768.0f * 2000.0f;
			fAngle[i] = sReg[Roll+i] / 32768.0f * 180.0f;
		}
		if(s_cDataUpdate & ACC_UPDATE)
		{
			imu->acc.x = fAcc[0];
			imu->acc.y = fAcc[1];
			imu->acc.z = fAcc[2];
			printf("acc:%.3f %.3f %.3f\r\n", imu->acc.x, imu->acc.y, imu->acc.z);
			s_cDataUpdate &= ~ACC_UPDATE;
		}
		if(s_cDataUpdate & GYRO_UPDATE)
		{
			imu->gyro.x = fGyro[0];
			imu->gyro.y = fGyro[1];
			imu->gyro.z = fGyro[2];
			printf("gyro:%.3f %.3f %.3f\r\n", imu->gyro.x, imu->gyro.y, imu->gyro.z);
			s_cDataUpdate &= ~GYRO_UPDATE;
		}
		if(s_cDataUpdate & ANGLE_UPDATE)
		{
			imu->angle.x = fAngle[0];
			imu->angle.y = fAngle[1];
			imu->angle.z = fAngle[2];
			printf("angle:%.3f %.3f %.3f\r\n", imu->angle.x, imu->angle.y, imu->angle.z);
			s_cDataUpdate &= ~ANGLE_UPDATE;
		}
		if(s_cDataUpdate & MAG_UPDATE)
		{
//			printf("mag:%d %d %d\r\n", sReg[HX], sReg[HY], sReg[HZ]);
			s_cDataUpdate &= ~MAG_UPDATE;
		}
	}
}

