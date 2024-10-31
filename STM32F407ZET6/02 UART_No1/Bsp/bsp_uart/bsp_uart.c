/*
 * bsp_uart.c
 *
 *  Created on: 2023年3月9日
 *      Author: Fdata
 */

#include  "bsp_uart.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart3, (uint8_t*)&ch,1,HAL_MAX_DELAY);
    return ch;
}



uart_t debug;

void bsp_uart_init(void)
{
	/*Initialize hardware */

	/*Clear receive buffer*/
	bsp_uart_rx_clear(&debug);

	/*Open receive interrupt*/
	HAL_UART_Receive_IT(&huart3, &(debug.rx_temp), 0x01);  //start receive 1 byte by interrupt
}

void bsp_uart_rx_clear(uart_t *uart)
{
	memset(uart->rx_buff, 0x00, RX_LEN);
	uart->rx_finish = FALSE;
	uart->rx_count = 0;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		if(debug.rx_count < RX_LEN)		//Max receive count = RX_LEN
		{
			debug.rx_buff[debug.rx_count++] = debug.rx_temp;
			if(debug.rx_temp == '\n')		//finish flag is "\r\n"
			{
				debug.rx_finish = TRUE;
			}
		}
		else
		{
			bsp_uart_rx_clear(&debug);
		}
		HAL_UART_Receive_IT(&huart3, &(debug.rx_temp), 0x01);	//receive next byte
	}
}














