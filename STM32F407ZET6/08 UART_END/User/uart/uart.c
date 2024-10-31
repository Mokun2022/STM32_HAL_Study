/*
 * uart.c
 *
 *  Created on: Apr 10, 2023
 *      Author: Fdata
 */

#include "uart.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/****************数据缓冲配置*******************/
uart_t ble;
fifo_t ble_fifo;
uint8_t ble_tx_buff[BLE_TX_SIZE];
uint8_t ble_rx_buff[BLE_RX_SIZE];
uint8_t ble_rx_fifo_buff[BLE_RX_FIFO_SIZE];

uart_t rtk;
fifo_t rtk_fifo;
uint8_t rtk_tx_buff[RTK_TX_SIZE];
uint8_t rtk_rx_buff[RTK_RX_SIZE];
uint8_t rtk_rx_fifo_buff[RTK_RX_FIFO_SIZE];

uart_t debug;
fifo_t debug_fifo;
uint8_t debug_tx_buff[DEBUG_TX_SIZE];
uint8_t debug_rx_buff[DEBUG_RX_SIZE];
uint8_t debug_rx_fifo_buff[DEBUG_RX_FIFO_SIZE];

uart_t net;
fifo_t net_fifo;
uint8_t net_tx_buff[NET_TX_SIZE];
uint8_t net_rx_buff[NET_RX_SIZE];
uint8_t net_rx_fifo_buff[NET_RX_FIFO_SIZE];

uart_t bat;
fifo_t bat_fifo;
uint8_t bat_tx_buff[BAT_TX_SIZE];
uint8_t bat_rx_buff[BAT_RX_SIZE];
uint8_t bat_rx_fifo_buff[BAT_RX_FIFO_SIZE];

/********************************************/


#if defined ( __GNUC__ ) && !defined (__clang__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
//以下代码支持printf函数,且不需要勾选 use MicroLIB  		KEIL5+AC6编译器
__asm (".global __use_no_semihosting\n\t");

//__use_no_semihosting was requested, but _ttywrch was
void _ttywrch(int ch)
{
    ch = ch;
}
FILE __stdout;
void _sys_exit(int x) //避免使用半主机模式
{
  x = x;
}
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
	while(LL_USART_IsActiveFlag_TC(USART3) == RESET);
	LL_USART_TransmitData8(USART3,(uint8_t)ch);
	return ch;
}


static void __irq_lock(void)
{
	__disable_irq();
}

static void __irq_unlock(void)
{
	__enable_irq();
}



static void uart_register(uart_t *uart , fifo_t *fifo , uint8_t *tx_buff , uint8_t *rx_buff, uint16_t tx_size, uint16_t rx_size)
{
	uart->rx_finish = false;
	uart->tx_count = 0;
	uart->rx_count = 0;
	uart->tx_size = tx_size;
	uart->rx_size = rx_size;
	uart->rx_fifo = fifo;
	uart->tx_buff = tx_buff;
	uart->rx_buff = rx_buff;
}


void uart_send_char(USART_TypeDef *uart, char c)
{
	/* 等待发送空 */
	while(LL_USART_IsActiveFlag_TXE(uart) == RESET);

	LL_USART_TransmitData8(uart, (uint8_t)c);

	/* 等待发送结束 */
	while(LL_USART_IsActiveFlag_TC(uart)== RESET);

}

void uart_send_string(USART_TypeDef *uart, char *s)
{
	while (*s != '\0')
	{
		uart_send_char(uart, *s);
		s++;
	}
}

void uart_send_data(USART_TypeDef *uart, uint8_t *data,uint16_t len)
{
	while(len > 0)
	{
		/* 等待发送空 */
		while(LL_USART_IsActiveFlag_TXE(uart) == RESET);

		LL_USART_TransmitData8(uart, *data);

		/* 等待发送结束 */
		while(LL_USART_IsActiveFlag_TC(uart)== RESET);

		data++;
		len--;
	}
}


void driver_uart_tx_clear(uart_t *uart)
{
	__irq_lock();
	memset(uart->tx_buff, 0x00, uart->tx_size);
	uart->tx_count = 0;
	__irq_unlock();
}

void driver_uart_rx_clear(uart_t *uart)
{
	__irq_lock();
	memset(uart->rx_buff, 0x00, uart->rx_size);
	uart->rx_finish = false;
	uart->rx_count = 0;
	__irq_unlock();
}


void driver_uart_init(void)
{
	/*Initialize hardware */
	//Go to main.c ---> MX_USART1_UART_Init() ...

	/*Create the fifo */
	fifo_register(&ble_fifo, ble_rx_fifo_buff, BLE_RX_FIFO_SIZE, __irq_lock, __irq_unlock);
	fifo_register(&rtk_fifo, rtk_rx_fifo_buff, RTK_RX_FIFO_SIZE, __irq_lock, __irq_unlock);
	fifo_register(&debug_fifo, debug_rx_fifo_buff, DEBUG_RX_FIFO_SIZE, __irq_lock, __irq_unlock);
	fifo_register(&net_fifo, net_rx_fifo_buff, NET_RX_FIFO_SIZE, __irq_lock, __irq_unlock);
	fifo_register(&bat_fifo, bat_rx_fifo_buff,BAT_RX_FIFO_SIZE, __irq_lock, __irq_unlock);

	/*Register uart handle*/
	uart_register(&ble, &ble_fifo, ble_tx_buff, ble_rx_buff, BLE_TX_SIZE, BLE_RX_SIZE);
	uart_register(&rtk, &rtk_fifo, rtk_tx_buff,rtk_rx_buff, RTK_TX_SIZE, RTK_RX_SIZE);
	uart_register(&debug, &debug_fifo, debug_tx_buff,debug_rx_buff, DEBUG_TX_SIZE, DEBUG_RX_SIZE);
	uart_register(&net, &net_fifo, net_tx_buff,net_rx_buff, NET_TX_SIZE, NET_RX_SIZE);
	uart_register(&bat, &bat_fifo, bat_tx_buff,bat_rx_buff, BAT_TX_SIZE, BAT_RX_SIZE);

	/*Enable IT RXNE and IDLE*/
	LL_USART_EnableIT_RXNE(USART1);
	LL_USART_EnableIT_RXNE(USART2);
	LL_USART_EnableIT_RXNE(USART3);
	LL_USART_EnableIT_RXNE(UART4);
	LL_USART_EnableIT_RXNE(USART6);

	LL_USART_EnableIT_IDLE(USART1);
	LL_USART_EnableIT_IDLE(USART2);
	LL_USART_EnableIT_IDLE(USART3);
	LL_USART_EnableIT_IDLE(UART4);
	LL_USART_EnableIT_IDLE(USART6);


	/*Clear receive buffer*/
	driver_uart_tx_clear(&ble);
	driver_uart_tx_clear(&rtk);
	driver_uart_tx_clear(&debug);
	driver_uart_tx_clear(&net);
	driver_uart_tx_clear(&bat);

	driver_uart_rx_clear(&ble);
	driver_uart_rx_clear(&rtk);
	driver_uart_rx_clear(&debug);
	driver_uart_rx_clear(&net);
	driver_uart_rx_clear(&bat);

	/*Clear Flag_TC*/
	LL_USART_ClearFlag_TC(USART1);
	LL_USART_ClearFlag_TC(USART2);
	LL_USART_ClearFlag_TC(USART3);
	LL_USART_ClearFlag_TC(UART4);
	LL_USART_ClearFlag_TC(USART6);

	LL_USART_ClearFlag_IDLE(USART1);
	LL_USART_ClearFlag_IDLE(USART2);
	LL_USART_ClearFlag_IDLE(USART3);
	LL_USART_ClearFlag_IDLE(UART4);
	LL_USART_ClearFlag_IDLE(USART6);
}


void ble_printf(const char *format,...)
{
	va_list args;

	va_start(args, format);
	ble.tx_count = vsnprintf((char*)ble.tx_buff, ble.tx_size, (char*)format, args);
	va_end(args);

	if(ble.tx_count)
		uart_send_data(USART1, ble.tx_buff, ble.tx_count);
}

void rtk_printf(const char *format,...)
{
	va_list args;

	va_start(args, format);
	rtk.tx_count = vsnprintf((char*)rtk.tx_buff, rtk.tx_size, (char*)format, args);
	va_end(args);
	if(rtk.tx_count)
		uart_send_data(USART2, rtk.tx_buff, rtk.tx_count);
}

void debug_printf(const char *format,...)
{
#if DEBUG_ENABLE
	va_list args;

	va_start(args, format);
	debug.tx_count = vsnprintf((char*)debug.tx_buff, debug.tx_size, (char*)format, args);
	va_end(args);

	if(debug.tx_count)
		uart_send_data(USART3, debug.tx_buff, debug.tx_count);
#endif
}

void net_printf(const char *format,...)
{
	va_list args;

	va_start(args, format);
	net.tx_count = vsnprintf((char*)net.tx_buff, net.tx_size, (char*)format, args);
	va_end(args);

	if(net.tx_count)
		uart_send_data(UART4, net.tx_buff, net.tx_count);
}


void bat_printf(const char *format,...)
{
	va_list args;

	va_start(args, format);
	bat.tx_count = vsnprintf((char*)bat.tx_buff, bat.tx_size, (char*)format, args);
	va_end(args);

	if(bat.tx_count)
		uart_send_data(USART6, bat.tx_buff, bat.tx_count);
}



void USER_UART1_IRQHandler(void)
{
	uint8_t byte;

	if(RESET != LL_USART_IsActiveFlag_RXNE(USART1))	//receive 1 byte
	{
		LL_USART_ClearFlag_RXNE(USART1);	//Clears the UART RXNE pending flag.

		byte = LL_USART_ReceiveData8(USART1);
		fifo_write(ble.rx_fifo, &byte , 1);	//receive 1 byte
	}
	else if(RESET != LL_USART_IsActiveFlag_IDLE(USART1))	//receive 1 packet
	{
		LL_USART_ClearFlag_IDLE(USART1);		//Clears the UART IDLE pending flag.

		ble.rx_finish = true;
		ble.rx_count = fifo_get_occupy_size(ble.rx_fifo);
	}
}


void USER_UART2_IRQHandler(void)
{
	uint8_t byte;

	if(RESET != LL_USART_IsActiveFlag_RXNE(USART2))	//receive 1 byte
	{
		LL_USART_ClearFlag_RXNE(USART2);	//Clears the UART RXNE pending flag.

		byte = LL_USART_ReceiveData8(USART2);
		fifo_write(rtk.rx_fifo, &byte , 1);	//receive 1 byte
	}
	else if(RESET != LL_USART_IsActiveFlag_IDLE(USART2))	//receive 1 packet
	{
		LL_USART_ClearFlag_IDLE(USART2);		//Clears the UART IDLE pending flag.

		rtk.rx_finish = true;
		rtk.rx_count = fifo_get_occupy_size(rtk.rx_fifo);
	}
}

void USER_UART3_IRQHandler(void)
{
	uint8_t byte;

	if(RESET != LL_USART_IsActiveFlag_RXNE(USART3))	//receive 1 byte
	{
		LL_USART_ClearFlag_RXNE(USART3);	//Clears the UART RXNE pending flag.

		byte = LL_USART_ReceiveData8(USART3);
		fifo_write(debug.rx_fifo, &byte , 1);	//receive 1 byte
	}
	else if(RESET != LL_USART_IsActiveFlag_IDLE(USART3))	//receive 1 packet
	{
		LL_USART_ClearFlag_IDLE(USART3);		//Clears the UART IDLE pending flag.

		debug.rx_finish = true;
		debug.rx_count = fifo_get_occupy_size(debug.rx_fifo);
	}
}

void USER_UART4_IRQHandler(void)
{
	uint8_t byte;

	if(RESET != LL_USART_IsActiveFlag_RXNE(UART4))	//receive 1 byte
	{
		LL_USART_ClearFlag_RXNE(UART4);	//Clears the UART RXNE pending flag.

		byte = LL_USART_ReceiveData8(UART4);
		fifo_write(net.rx_fifo, &byte , 1);	//receive 1 byte
	}
	else if(RESET != LL_USART_IsActiveFlag_IDLE(UART4))	//receive 1 packet
	{
		LL_USART_ClearFlag_IDLE(UART4);		//Clears the UART IDLE pending flag.

		net.rx_finish = true;
		net.rx_count = fifo_get_occupy_size(net.rx_fifo);
	}
}

void USER_UART6_IRQHandler(void)
{
	uint8_t byte;

	if(RESET != LL_USART_IsActiveFlag_RXNE(USART6))	//receive 1 byte
	{
		LL_USART_ClearFlag_RXNE(USART6);	//Clears the UART RXNE pending flag.

		byte = LL_USART_ReceiveData8(USART6);
		fifo_write(bat.rx_fifo, &byte , 1);	//receive 1 byte
	}
	else if(RESET != LL_USART_IsActiveFlag_IDLE(USART6))	//receive 1 packet
	{
		LL_USART_ClearFlag_IDLE(USART6);		//Clears the UART IDLE pending flag.

		bat.rx_finish = true;
		bat.rx_count = fifo_get_occupy_size(bat.rx_fifo);
	}
}


