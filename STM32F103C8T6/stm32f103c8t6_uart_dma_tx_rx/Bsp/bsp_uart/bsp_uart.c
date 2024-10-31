/*
 * bsp_uart.c
 *
 *  Created on: Nov 17, 2023
 *      Author: KennyMo
 */

#include "bsp_uart.h"
#include "usart.h"
#include "dma.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

bsp_uart_t debug;
static uint8_t debug_tx_buff[DEBUG_TX_SIZE];
static uint8_t debug_rx_buff[DEBUG_RX_SIZE];

/**    
  * @brief  禁用全局中断请求。    
  *         此函数用于暂时屏蔽所有中断请求。    
  * @param  无    
  * @retval	无    
  */  
static void __irq_lock(void)
{
    __disable_irq();
}

/**    
  * @brief  启用全局中断请求。    
  *         此函数用于在中断请求被屏蔽后重新启用它们。    
  * @param  无    
  * @retval 无    
  */   
static void __irq_unlock(void)
{
    __enable_irq();
}

/**    
  * @brief  向BSP UART模块注册UART缓冲区。    
  *         此函数用于初始化UART实例的发送和接收缓冲区。    
  * @param  uart: 指向表示UART实例的bsp_uart_t结构的指针。    
  * @param  tx_buff: 指向发送缓冲区的指针。    
  * @param  rx_buff: 指向接收缓冲区的指针。    
  * @retval 无    
  */
static void bsp_uart_register(bsp_uart_t *uart, uint8_t *tx_buff, uint8_t *rx_buff)
{
	uart->tx_finish = 1;
    uart->rx_finish = 0;
    uart->tx_count = 0;
    uart->rx_count = 0;
    uart->tx_buff = tx_buff;
    uart->rx_buff = rx_buff;
}

/**  
  * @brief 通过USART1以阻塞方式发送一个字符。  
  *        此函数会等待USART1的发送数据寄存器为空，随后发送指定的字符，并等待发送操作完成。  
  * @param c: 需要发送的字符。  
  * @retval 无。  
  */  
void bsp_debug_poll_send_char(char c)
{
    /* 等待发送空 */
    while(LL_USART_IsActiveFlag_TXE(USART1) == RESET);

    LL_USART_TransmitData8(USART1, (uint8_t)c);

    /* 等待发送结束 */
    while(LL_USART_IsActiveFlag_TC(USART1) == RESET);
}

/**  
  * @brief 通过USART1以阻塞方式发送一串数据。  
  *        此函数会逐个字符地发送数据数组中的内容，直至所有数据发送完毕。  
  * @param data: 指向待发送数据数组的指针。  
  * @param len: 需要发送的数据长度（以字节为单位）。  
  * @retval 无。  
  */  
void bsp_debug_poll_send_data(uint8_t *data, uint32_t len)
{
    while(len > 0)
    {
        /* 等待发送空 */
        while(LL_USART_IsActiveFlag_TXE(USART1) == RESET);

        LL_USART_TransmitData8(USART1, *data);

        /* 等待发送结束 */
        while(LL_USART_IsActiveFlag_TC(USART1) == RESET);

        data++;
        len--;
    }
}

/**  
  * @brief 通过USART1以阻塞方式发送格式化的字符串。  
  *        此函数会接收一个格式化的字符串及其参数，将其格式化为指定内容后存入调试发送缓冲区，  
  *        随后通过USART1发送缓冲区中的内容。  
  * @param format: 格式化字符串。  
  * @param ...: 可变参数列表，用于与格式化字符串中的格式占位符相对应。  
  * @retval 无。  
  *        注意：此函数依赖于全局变量'debug'及其成员'tx_buff'和'tx_count'，以及宏定义'DEBUG_TX_SIZE'。  
  */  
void bsp_debug_poll_printf(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    debug.tx_count = vsnprintf((char *)debug.tx_buff, DEBUG_TX_SIZE, (char *)format, args);
    va_end(args);

    bsp_debug_poll_send_data(debug.tx_buff, debug.tx_count);
}


/**  
  * @brief 通过DMA（直接存储器访问）和USART1发送单个字符。  
  *        此函数会等待USART1的发送数据寄存器为空，然后通过DMA配置并发送指定的字符。  
  * @param c: 需要发送的字符。  
  * @param 无  
  * @retval 无  
  */  
void bsp_debug_dma_send_char(char c)  
{  
    /* 等待发送数据完成 */
	if(debug.tx_finish == 1)
	{
		LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)&c);
		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, 1);  
		LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);  		
		debug.tx_finish = 0;
	}
}

/**  
  * @brief 通过DMA（直接存储器访问）和USART1发送一串数据。  
  *        此函数会等待USART1的发送数据寄存器为空，然后通过DMA配置并发送指定的数据数组。  
  * @param data: 指向待发送数据数组的指针。  
  * @param len: 需要发送的数据长度（以字节为单位）。  
  * @retval 无  
  */  
void bsp_debug_dma_send_data(uint8_t *data, uint32_t len)  
{  
    /* 等待发送数据完成 */
	if(debug.tx_finish == 1)
	{
		LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)data);  
		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, len);  
		LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);  
		debug.tx_finish = 0;
	}
}

/**  
  * @brief 通过DMA（直接存储器访问）和USART1发送格式化的字符串。  
  *        此函数会接收一个格式化的字符串及其参数，将其格式化为指定内容后存入调试发送缓冲区，  
  *        然后通过DMA发送缓冲区中的内容。  
  * @param format: 格式化字符串。  
  * @param ...: 可变参数列表，用于与格式化字符串中的格式占位符相对应。  
  * @retval 无  
  *        注意：此函数依赖于全局变量'debug'及其成员'tx_buff'、'tx_count'和'DEBUG_TX_SIZE'。  
  */  
void bsp_debug_dma_printf(const char *format, ...)  
{  
    va_list args;  
  
    va_start(args, format);  
    debug.tx_count = vsnprintf((char *)debug.tx_buff, DEBUG_TX_SIZE, (char *)format, args);  
    va_end(args);  
    bsp_debug_dma_send_data(debug.tx_buff, debug.tx_count);  
}  

/**  
  * @brief 清除调试发送缓冲区。  
  *        此函数会锁定中断，清空调试发送缓冲区的内容，并重置发送计数器。  
  * @param 无  
  * @retval 无  
  *        注意：此函数依赖于全局变量'debug'及其成员'tx_buff'和'tx_count'。  
  */  
void bsp_debug_tx_clear(void)  
{  
    __irq_lock();  
    memset(debug.tx_buff, 0x00, debug.tx_count); 
	debug.tx_finish = 1;	
    debug.tx_count = 0; 
    __irq_unlock();  
}

/**  
  * @brief 清除调试接收缓冲区。  
  *        此函数会锁定中断，清空调试接收缓冲区的内容，重置接收完成标志和接收计数器。  
  * @param 无  
  * @retval 无  
  *        注意：此函数依赖于全局变量'debug'及其成员'rx_buff'、'rx_finish'和'rx_count'。  
  */  
void bsp_debug_rx_clear(void)  
{  
    __irq_lock();  
    memset(debug.rx_buff, 0x00, debug.rx_count); 
    debug.rx_finish = 0;  
    debug.rx_count = 0;  
    __irq_unlock();  
}

/**  
  * @brief 初始化UART和DMA硬件，并配置相关的中断和缓冲区。  
  *        此函数会初始化USART1的UART功能，配置DMA用于UART的TX和RX传输，  
  *        并注册UART句柄及其发送和接收缓冲区。  
  * @param 无  
  * @retval 无  
  *        注意：此函数依赖于全局变量'debug'、'debug_tx_buff'、'debug_rx_buff'、  
  *               'DEBUG_TX_SIZE'和'DEBUG_RX_SIZE'。  
  */  
void bsp_uart_init(void)  
{  
    /* 初始化UART和DMA硬件（具体实现可能在main.c的MX_USART1_UART_Init()等函数中） */  
  
    /* 注册UART句柄及其发送和接收缓冲区 */  
    bsp_uart_register(&debug, debug_tx_buff, debug_rx_buff);  
  
    /* 配置DMA用于UART TX的地址（内存到外设） */  
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_4, (uint32_t)debug_tx_buff, (uint32_t)&(USART1->DR), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);  
    /* 配置DMA用于UART RX的地址（外设到内存） */  
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5, (uint32_t)&(USART1->DR), (uint32_t)debug_rx_buff, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);  
  
    /* 设置DMA传输的大小 */  
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, DEBUG_TX_SIZE);  
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, DEBUG_RX_SIZE);  
  
    /* 清空UART的TX和RX缓冲区 */  
    bsp_debug_tx_clear();  
    bsp_debug_rx_clear();  
  
    /* 清空USART的标志位 */  
    LL_USART_ClearFlag_TC(USART1);  
    LL_USART_ClearFlag_IDLE(USART1);  
  
    /* 使能USART的TX和RX的DMA请求 */  
    LL_USART_EnableDMAReq_TX(USART1);  
    LL_USART_EnableDMAReq_RX(USART1);  
  
    /* 使能DMA通道4的传输完成中断，并开启DMA通道5 */  
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);  
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);  
  
    /* 使能USART的IDLE中断，用于RX */  
    LL_USART_EnableIT_IDLE(USART1);  
}  

/**  
  * @brief USART1的IDLE中断处理函数。  
  *        当USART1接收到一个数据包（即检测到IDLE标志）时，此函数会被调用。  
  *        它会处理接收到的数据，并重新配置DMA以接收下一个数据包。  
  * @param 无  
  * @retval 无  
  */  
void USER_UART1_IRQHandler(void)
{
    uint32_t dma_length;

    if(RESET != LL_USART_IsActiveFlag_IDLE(USART1))	/* 检查USART1是否检测到IDLE标志 */ 
    {
        LL_USART_ClearFlag_IDLE(USART1);			 /* 清空USART1的IDLE标志 */  

        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);	 /* 禁用DMA通道5 */  
		
		dma_length = LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_5);	//TODO: 如果接收的数据长度超过DMA配置的传输长度，那么多出的数据会在数据最前面
		debug.rx_count = DEBUG_RX_SIZE - dma_length;	/* 计算实际接收到的数据长度（DEBUG_RX_SIZE - 剩余长度） */  
		
		//数据处理...
		//bsp_debug_poll_printf("%s", debug.rx_buff);

		/* 设置接收完成标志 */  
        debug.rx_finish = 1;
		
		  /*重新配置DMA以接收下一个数据包 */ 
		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, DEBUG_RX_SIZE);
        LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
    }
}


/**  
  * @brief DMA1通道4的传输完成中断处理函数。  
  *        当DMA1通道4完成数据传输时，此函数会被调用。  
  *        它会清除相关的中断标志，并禁用DMA通道4。  
  * @param 无  
  * @retval 无  
  */  
void USRT_DMA1_Channel4_IRQHandler(void)
{
	if(RESET !=LL_DMA_IsActiveFlag_TC4(DMA1))	//DMA接收完成标志
	{
		LL_DMA_ClearFlag_TC4(DMA1);				//清除中断标志
		LL_USART_ClearFlag_TC(USART1);			//清除串口1的标志位
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
		debug.tx_finish = 1; 					//设置发送完成标志
	}
}
