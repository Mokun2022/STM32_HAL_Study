/*
 * bsp_uart.h
 *
 *  Created on: 2023年3月9日
 *      Author: Fdata
 */

#ifndef BSP_UART_BSP_UART_H_
#define BSP_UART_BSP_UART_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

#define TX_LEN	512				// Max transmit byte
#define RX_LEN	1024				// Max receive byte

typedef enum
{
  FALSE = 0,
  TRUE = 1,
}bool_t;


typedef struct
{
	//TX
	uint8_t tx_buff[TX_LEN];	//transmit buff

	//RX
	uint16_t rx_count;			//receive byte count
	bool_t rx_finish;			//receive finish flag
	uint8_t rx_buff[RX_LEN];	//receive buff
}uart_t;


extern uart_t debug;

void bsp_uart_init(void);
void bsp_uart_rx_clear(uart_t *uart);
void bsp_debug_printf(const char *format,...);

void USER_UART_IRQHandler(UART_HandleTypeDef *huart);

#endif /* BSP_UART_BSP_UART_H_ */
