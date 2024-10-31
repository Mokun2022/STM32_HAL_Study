/*
 * uart.h
 *
 *  Created on: Apr 10, 2023
 *      Author: Fdata
 */

#ifndef UART_UART_H_
#define UART_UART_H_

#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"
#include "../uart/fifo.h"

/*
 *    PA9   ------> USART1_TX  ------>  ble_tx
 * 	  PA10   ------> USART1_RX  ------>  ble_rx
 *    PA2   ------> USART2_TX   ------>  rtk_tx
 * 	  PA3   ------> USART2_RX   ------>  rtk_rx
 *    PD8   ------> USART3_TX  ------>  debug_tx
 *    PD9   ------> USART3_RX  ------>  debug_rx
 *    PC10   ------> UART4_TX  ------>  net_tx
 *    PC11   ------> UART4_RX  ------>  net_rx
 *    PG9   ------> USART6_RX   ------>  bat_tx
 *    PG14   ------> USART6_TX   ------> bat_rx
 */



/****************debug串口使能*******************/

#define DEBUG_ENABLE 		1


/****************数据缓冲配置*******************/
#define BLE_TX_SIZE  		128
#define BLE_RX_SIZE  		128
#define BLE_RX_FIFO_SIZE	128
#define RTK_TX_SIZE			128
#define RTK_RX_SIZE			128
#define RTK_RX_FIFO_SIZE	128
#define DEBUG_TX_SIZE		2048
#define DEBUG_RX_SIZE		2048
#define DEBUG_RX_FIFO_SIZE	2048
#define NET_TX_SIZE			128
#define NET_RX_SIZE			128
#define NET_RX_FIFO_SIZE	128
#define BAT_TX_SIZE			2048
#define BAT_RX_SIZE			2048
#define BAT_RX_FIFO_SIZE	2048

/********************************************/

/*bsp uart handle*/
typedef struct
{
	bool rx_finish;					//receive finish flag
	uint16_t tx_count;				//transmit byte count
	uint16_t rx_count;				//receive byte count
	uint16_t tx_size;				//transmit buff size
	uint16_t rx_size;				// receive  buff size
	uint8_t *tx_buff;				//transmit buff
	uint8_t *rx_buff;				//receive  buff
	fifo_t *rx_fifo;				//  fifo handle
}uart_t;


/****************数据、函数导出*******************/
extern uart_t ble;
extern uart_t rtk;
extern uart_t debug;
extern uart_t net;
extern uart_t bat;

extern void driver_uart_init(void);
extern void driver_uart_tx_clear(uart_t *uart);
extern void driver_uart_rx_clear(uart_t *uart);

extern void ble_printf(const char *format,...);
extern void rtk_printf(const char *format,...);
extern void debug_printf(const char *format,...);
extern void net_printf(const char *format,...);
extern void bat_printf(const char *format,...);

extern void uart_send_char(USART_TypeDef *uart, char c);
extern void uart_send_string(USART_TypeDef *uart,char *s);
extern void uart_send_data(USART_TypeDef *uart, uint8_t *data,uint16_t len);

extern void USER_UART1_IRQHandler(void);
extern void USER_UART2_IRQHandler(void);
extern void USER_UART3_IRQHandler(void);
extern void USER_UART4_IRQHandler(void);
extern void USER_UART6_IRQHandler(void);
/********************************************/

#endif /* UART_UART_H_ */
