/*
 * bsp_uart.h
 *
 *  Created on: 2023年3月9日
 *      Author: Fdata
 */

#ifndef BSP_UART_BSP_UART_H_
#define BSP_UART_BSP_UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "../bsp_uart/bsp_fifo.h"

#define RX_LEN	1024				// Max receive byte
#define FIFO_RX_LEN 1024				// Max fifo receive byte


typedef struct
{
	bool rx_finish;			//receive finish flag
	uint16_t rx_count;			//receive byte count
	uint8_t rx_buff[RX_LEN];	//receive buff
	fifo_t rx_fifo;				// fifo
	uint8_t	fifo_buff[FIFO_RX_LEN];	//fifo buff
}uart_t;

extern uart_t debug;

extern void bsp_uart_init(void);
extern void bsp_uart_rx_clear(uart_t *uart);
extern void USER_UART_IRQHandler(UART_HandleTypeDef *huart);


#endif /* BSP_UART_BSP_UART_H_ */
