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
	/*Initialize DMA-UART*/
	 LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_1, (uint32_t)debug.rx_buff);
	 LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_1, (uint32_t)(&USART3->DR));
	 LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_1, RX_LEN);
	 LL_USART_EnableDMAReq_RX(USART3); 				 //dma uart rx
	 LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_1);	//enable dma

	/*Initialize hardware */
	//Go to main.c ---> MX_USART3_UART_Init()

	/*Enable IT RXNE and IDLE*/
	LL_USART_EnableIT_IDLE(USART3);

	/*Clear receive buffer*/
	bsp_uart_rx_clear(&debug);

	/*Clear Flag_TC*/
	LL_USART_ClearFlag_TC(USART3);
}

void bsp_uart_rx_clear(uart_t *uart)
{
	__irq_lock();
	memset(uart->rx_buff, 0x00, RX_LEN);
	uart->rx_finish = false;
	uart->rx_count = 0;
	__irq_unlock();
}



#if 1
void USER_UART3_IRQHandler(void)
{
	if(RESET != LL_USART_IsActiveFlag_IDLE(USART3))	//receive 1 packet
	{
		LL_USART_ClearFlag_IDLE(USART3);		//Clears the UART IDLE pending flag.
		LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_1);	//STOP DMA
		LL_DMA_ClearFlag_TC1(DMA1);				//Clear Stream 1 transfer complete flag.
		debug.rx_count = RX_LEN - LL_DMA_GetDataLength(DMA1, LL_DMA_STREAM_1);	//receive how many count
		debug.rx_finish = true;
	}
}



#else
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
#endif









