#ifndef _BSP_UART__H_
#define _BSP_UART__H_


#include <stdint.h>

/****************数据缓冲配置*******************/

#define DEBUG_TX_SIZE  		1024
#define DEBUG_RX_SIZE  		1024

/********************************************/

/*bsp uart handle*/
typedef struct
{
	uint8_t rx_finish;				//receive finish flag
	uint16_t tx_count;				//transmit byte count
	uint16_t rx_count;				//receive byte count
	uint8_t *tx_buff;				//transmit buff
	uint8_t *rx_buff;				//receive  buff
}bsp_uart_t;


/****************数据、函数导出*******************/

extern bsp_uart_t debug;

void bsp_uart_init(void);

void bsp_debug_poll_send_char(char c);
void bsp_debug_poll_send_data(uint8_t *data, uint32_t len);
void bsp_debug_poll_printf(const char *format, ...);

void bsp_debug_tx_clear(void);  
void bsp_debug_rx_clear(void);  

void USER_UART1_IRQHandler(void);

#endif
