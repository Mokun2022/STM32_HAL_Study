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
	LL_USART_TransmitData8(USART3,(uint8_t)ch);
	while(LL_USART_IsActiveFlag_TC(USART3) == RESET);

	return ch;
}


uart_t debug;
uart_t rtk;
uart_t cat1;


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
	//Go to main.c ---> MX_USART3_UART_Init()

	/*Create one fifo */
	fifo_register(&cat1.rx_fifo, cat1.fifo_buff, FIFO_RX_LEN, __irq_lock, __irq_unlock);
	fifo_register(&rtk.rx_fifo, rtk.fifo_buff, FIFO_RX_LEN, __irq_lock, __irq_unlock);
	fifo_register(&debug.rx_fifo, debug.fifo_buff, FIFO_RX_LEN, __irq_lock, __irq_unlock);

	/*Enable IT RXNE and IDLE*/
	LL_USART_EnableIT_RXNE(USART2);
	LL_USART_EnableIT_IDLE(USART2);
	LL_USART_EnableIT_RXNE(USART3);
	LL_USART_EnableIT_IDLE(USART3);
	LL_USART_EnableIT_RXNE(USART6);
	LL_USART_EnableIT_IDLE(USART6);


	/*Clear receive buffer*/
	bsp_uart_rx_clear(&rtk);
	bsp_uart_rx_clear(&debug);
	bsp_uart_rx_clear(&cat1);

	/*Clear Flag_TC*/
	LL_USART_ClearFlag_TC(USART2);
	LL_USART_ClearFlag_TC(USART3);
	LL_USART_ClearFlag_TC(USART6);
}

void bsp_uart_rx_clear(uart_t *uart)
{
	__irq_lock();
	memset(uart->rx_buff, 0x00, RX_LEN);
	uart->rx_finish = false;
	uart->rx_count = 0;
	__irq_unlock();
}


void bsp_uart_send(USART_TypeDef *uart,uint8_t *str,uint16_t len)
{
	for(int i=0;i<len;i++)
	{
		LL_USART_TransmitData8(uart,str[i]);
		while(LL_USART_IsActiveFlag_TC(uart) == RESET);
	}
}

void USER_UART2_IRQHandler(void)
{
	uint8_t byte;

	if(RESET != LL_USART_IsActiveFlag_RXNE(USART2))	//receive 1 byte
	{
		LL_USART_ClearFlag_RXNE(USART2);	//Clears the UART RXNE pending flag.

		byte = LL_USART_ReceiveData8(USART2);
		fifo_write(&rtk.rx_fifo, &byte , 1);	//receive 1 byte
	}
	else if(RESET != LL_USART_IsActiveFlag_IDLE(USART2))	//receive 1 packet
	{
		LL_USART_ClearFlag_IDLE(USART2);		//Clears the UART IDLE pending flag.

		rtk.rx_finish = true;
		rtk.rx_count = fifo_get_occupy_size(&rtk.rx_fifo);
	}
}


void USER_UART3_IRQHandler(void)
{
	uint8_t byte;

	if(RESET != LL_USART_IsActiveFlag_RXNE(USART3))	//receive 1 byte
	{
		LL_USART_ClearFlag_RXNE(USART3);	//Clears the UART RXNE pending flag.

		byte = LL_USART_ReceiveData8(USART3);
		fifo_write(&debug.rx_fifo, &byte , 1);	//receive 1 byte
	}
	else if(RESET != LL_USART_IsActiveFlag_IDLE(USART3))	//receive 1 packet
	{
		LL_USART_ClearFlag_IDLE(USART3);		//Clears the UART IDLE pending flag.

		debug.rx_finish = true;
		debug.rx_count = fifo_get_occupy_size(&debug.rx_fifo);
	}
}

void USER_UART6_IRQHandler(void)
{
	uint8_t byte;

	if(RESET != LL_USART_IsActiveFlag_RXNE(USART6))	//receive 1 byte
	{
		LL_USART_ClearFlag_RXNE(USART6);	//Clears the UART RXNE pending flag.

		byte = LL_USART_ReceiveData8(USART6);
		fifo_write(&cat1.rx_fifo, &byte , 1);	//receive 1 byte
	}
	else if(RESET != LL_USART_IsActiveFlag_IDLE(USART6))	//receive 1 packet
	{
		LL_USART_ClearFlag_IDLE(USART6);		//Clears the UART IDLE pending flag.

		cat1.rx_finish = true;
		cat1.rx_count = fifo_get_occupy_size(&cat1.rx_fifo);
	}
}








