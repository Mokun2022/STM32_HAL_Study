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
}

void bsp_uart_rx_clear(uart_t *uart)
{
	memset(uart->rx_buff, 0x00, RX_LEN);
	uart->rx_finish = FALSE;
	uart->rx_count = 0;
}


void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		if(RESET != __HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE))	//receive 1 byte
		{
			__HAL_UART_CLEAR_FLAG(&huart3,UART_FLAG_RXNE);	//Clears the UART RXNE pending flag.
			if(debug.rx_count < RX_LEN)		//Max receive count = RX_LEN
			{
				debug.rx_buff[debug.rx_count++] = huart->Instance->DR;	////receive 1 byte
				if(debug.rx_count == RX_LEN)
				{
					debug.rx_count = RX_LEN - 1;	// truncation
				}
			}
		}
		else if(RESET != __HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE))	//receive 1 packet
		{
			__HAL_UART_CLEAR_IDLEFLAG(&huart3);		//Clears the UART IDLE pending flag.
			debug.rx_count = 0;
			debug.rx_finish = TRUE;
		}
	}
}











