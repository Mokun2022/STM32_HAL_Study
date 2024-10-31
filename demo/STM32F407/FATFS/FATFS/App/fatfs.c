/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
#include "fatfs.h"
#include <stdio.h>
#include <string.h>

uint8_t retUSER;    /* Return value for USER */
char USERPath[4];   /* USER logical drive path */
FATFS USERFatFS;    /* File system object for USER logical drive */
FIL USERFile;       /* File object for USER */

/* USER CODE BEGIN Variables */
UINT count;
unsigned char read_buf[50] = {0};
unsigned char write_buf[50] = "hello fatfs\r\n";
BYTE	work_t[_MAX_SS];

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the USER driver ###########################*/
  retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
	return	  ((DWORD)(2023 - 1980) << 25)	/* Year = 2023 */
			| ((DWORD)1 << 21)				/* Month = 1 */
			| ((DWORD)1 << 16)				/* Day_m = 1*/
			| ((DWORD)0 << 11)				/* Hour = 0 */
			| ((DWORD)0 << 5)				/* Min = 0 */
			| ((DWORD)0 >> 1);				/* Sec = 0 */
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */

void fatfs_demo(void)
{
	//W25QXX_Erase_Chip();
	retUSER = f_mount(&USERFatFS,USERPath,1);
	printf("retUSER = %d\r\n",retUSER);

	if(FR_NO_FILESYSTEM == retUSER)
	{
		retUSER = f_mkfs(USERPath,FM_ANY,4096,work_t,sizeof(work_t));
		printf("f_mkfs = %d\r\n",retUSER);
		HAL_Delay(500);
		retUSER = f_mount(NULL,USERPath,1);
		printf("f_mount = %d\r\n",retUSER);
		retUSER = f_mount(&USERFatFS,USERPath,1);
		printf("f_mount = %d\r\n",retUSER);
	}

	printf("\r\n****************** data_write_test ******************\r\n");
	retUSER = f_open(&USERFile, "readme.txt", FA_OPEN_ALWAYS | FA_WRITE);
    if(retUSER == FR_OK)
    {
        printf("open/create readme.txt success,write data now\r\n");
        retUSER = f_write(&USERFile, write_buf, strlen((const char *)write_buf), &count);

        if(retUSER != FR_OK)
        {
            printf("f_write has error,err = %d\r\n", retUSER);
            printf("close readme.txt data\r\n");
            count = 0;
            f_close(&USERFile);
        }
        else
        {
            printf("write data success,data_size:%d\n", count);
            printf("data:\r\n%s\r\n", write_buf);
            printf("close readme.txt data\r\n");
            count = 0;

            f_close(&USERFile);
        }
    }
    else
    	printf("open/create readme.txt data error,err = %d\r\n", retUSER);


    printf("****************** data_read_test ******************\r\n");
    retUSER = f_open(&USERFile, "readme.txt", FA_OPEN_EXISTING | FA_READ);

    if(retUSER == FR_OK)
    {
        printf("open readme.txt data success,read data now\r\n");
        retUSER = f_read(&USERFile, read_buf, sizeof(read_buf), &count);

        if(retUSER != FR_OK)
        {
            printf("f_read has error,err = %d\r\n", retUSER);
            printf("close readme.txt data\r\n");
            f_close(&USERFile);
        }
        else
        {
            printf("read success,read size:%d\n", count);
            printf("read data:\r\n%s\r\n", read_buf);
            printf("close readme.txt data\r\n");
            f_close(&USERFile);
        }
    }
    else
 	   printf("open readme.txt data error,err = %d\r\n", retUSER);
}



/* USER CODE END Application */
