/*
 * bsp_uart.c
 *
 *  Created on: 2023年3月9日
 *      Author: Fdata
 */

#include "bsp_uart.h"
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

static void __irq_lock(void)
{
	__disable_irq();
}

static void __irq_unlock(void)
{
	__enable_irq();
}


void bsp_uart_init(void)
{
	/*Initialize hardware */

	/*Create one fifo */
	fifo_register(&debug.rx_fifo, debug.fifo_buff, FIFO_RX_LEN, __irq_lock, __irq_unlock);

	/*Clear receive buffer*/
	bsp_uart_rx_clear(&debug);
}

void bsp_uart_rx_clear(uart_t *uart)
{
	__disable_irq();
	memset(uart->rx_buff, 0x00, RX_LEN);
	uart->rx_finish = false;
	uart->rx_count = 0;
	__enable_irq();
}


void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	uint8_t byte;
	if(huart->Instance == USART3)
	{
		if(RESET != __HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE))	//receive 1 byte
		{
			__HAL_UART_CLEAR_FLAG(&huart3,UART_FLAG_RXNE);	//Clears the UART RXNE pending flag.
		/*
			if(debug.rx_count < RX_LEN)		//Max receive count = RX_LEN
			{
				debug.rx_buff[debug.rx_count++] = huart->Instance->DR;	//receive 1 byte
				if(debug.rx_count == RX_LEN)
				{
					debug.rx_count = RX_LEN - 1;	// truncation
				}
			}
		*/
			byte = huart->Instance->DR;
			fifo_write(&debug.rx_fifo, &byte , 1);	//receive 1 byte
		}
		else if(RESET != __HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE))	//receive 1 packet
		{
			__HAL_UART_CLEAR_IDLEFLAG(&huart3);		//Clears the UART IDLE pending flag.
		/*
			debug.rx_count = 0;
			debug.rx_finish = true;
		*/
			debug.rx_finish = true;
			debug.rx_count = fifo_get_occupy_size(&debug.rx_fifo);
		}
	}
}











