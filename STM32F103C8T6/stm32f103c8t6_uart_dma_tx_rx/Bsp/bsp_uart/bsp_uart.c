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
  * @brief  ����ȫ���ж�����    
  *         �˺���������ʱ���������ж�����    
  * @param  ��    
  * @retval	��    
  */  
static void __irq_lock(void)
{
    __disable_irq();
}

/**    
  * @brief  ����ȫ���ж�����    
  *         �˺����������ж��������κ������������ǡ�    
  * @param  ��    
  * @retval ��    
  */   
static void __irq_unlock(void)
{
    __enable_irq();
}

/**    
  * @brief  ��BSP UARTģ��ע��UART��������    
  *         �˺������ڳ�ʼ��UARTʵ���ķ��ͺͽ��ջ�������    
  * @param  uart: ָ���ʾUARTʵ����bsp_uart_t�ṹ��ָ�롣    
  * @param  tx_buff: ָ���ͻ�������ָ�롣    
  * @param  rx_buff: ָ����ջ�������ָ�롣    
  * @retval ��    
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
  * @brief ͨ��USART1��������ʽ����һ���ַ���  
  *        �˺�����ȴ�USART1�ķ������ݼĴ���Ϊ�գ������ָ�����ַ������ȴ����Ͳ�����ɡ�  
  * @param c: ��Ҫ���͵��ַ���  
  * @retval �ޡ�  
  */  
void bsp_debug_poll_send_char(char c)
{
    /* �ȴ����Ϳ� */
    while(LL_USART_IsActiveFlag_TXE(USART1) == RESET);

    LL_USART_TransmitData8(USART1, (uint8_t)c);

    /* �ȴ����ͽ��� */
    while(LL_USART_IsActiveFlag_TC(USART1) == RESET);
}

/**  
  * @brief ͨ��USART1��������ʽ����һ�����ݡ�  
  *        �˺���������ַ��ط������������е����ݣ�ֱ���������ݷ�����ϡ�  
  * @param data: ָ����������������ָ�롣  
  * @param len: ��Ҫ���͵����ݳ��ȣ����ֽ�Ϊ��λ����  
  * @retval �ޡ�  
  */  
void bsp_debug_poll_send_data(uint8_t *data, uint32_t len)
{
    while(len > 0)
    {
        /* �ȴ����Ϳ� */
        while(LL_USART_IsActiveFlag_TXE(USART1) == RESET);

        LL_USART_TransmitData8(USART1, *data);

        /* �ȴ����ͽ��� */
        while(LL_USART_IsActiveFlag_TC(USART1) == RESET);

        data++;
        len--;
    }
}

/**  
  * @brief ͨ��USART1��������ʽ���͸�ʽ�����ַ�����  
  *        �˺��������һ����ʽ�����ַ�����������������ʽ��Ϊָ�����ݺ������Է��ͻ�������  
  *        ���ͨ��USART1���ͻ������е����ݡ�  
  * @param format: ��ʽ���ַ�����  
  * @param ...: �ɱ�����б��������ʽ���ַ����еĸ�ʽռλ�����Ӧ��  
  * @retval �ޡ�  
  *        ע�⣺�˺���������ȫ�ֱ���'debug'�����Ա'tx_buff'��'tx_count'���Լ��궨��'DEBUG_TX_SIZE'��  
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
  * @brief ͨ��DMA��ֱ�Ӵ洢�����ʣ���USART1���͵����ַ���  
  *        �˺�����ȴ�USART1�ķ������ݼĴ���Ϊ�գ�Ȼ��ͨ��DMA���ò�����ָ�����ַ���  
  * @param c: ��Ҫ���͵��ַ���  
  * @param ��  
  * @retval ��  
  */  
void bsp_debug_dma_send_char(char c)  
{  
    /* �ȴ������������ */
	if(debug.tx_finish == 1)
	{
		LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)&c);
		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, 1);  
		LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);  		
		debug.tx_finish = 0;
	}
}

/**  
  * @brief ͨ��DMA��ֱ�Ӵ洢�����ʣ���USART1����һ�����ݡ�  
  *        �˺�����ȴ�USART1�ķ������ݼĴ���Ϊ�գ�Ȼ��ͨ��DMA���ò�����ָ�����������顣  
  * @param data: ָ����������������ָ�롣  
  * @param len: ��Ҫ���͵����ݳ��ȣ����ֽ�Ϊ��λ����  
  * @retval ��  
  */  
void bsp_debug_dma_send_data(uint8_t *data, uint32_t len)  
{  
    /* �ȴ������������ */
	if(debug.tx_finish == 1)
	{
		LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)data);  
		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, len);  
		LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);  
		debug.tx_finish = 0;
	}
}

/**  
  * @brief ͨ��DMA��ֱ�Ӵ洢�����ʣ���USART1���͸�ʽ�����ַ�����  
  *        �˺��������һ����ʽ�����ַ�����������������ʽ��Ϊָ�����ݺ������Է��ͻ�������  
  *        Ȼ��ͨ��DMA���ͻ������е����ݡ�  
  * @param format: ��ʽ���ַ�����  
  * @param ...: �ɱ�����б��������ʽ���ַ����еĸ�ʽռλ�����Ӧ��  
  * @retval ��  
  *        ע�⣺�˺���������ȫ�ֱ���'debug'�����Ա'tx_buff'��'tx_count'��'DEBUG_TX_SIZE'��  
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
  * @brief ������Է��ͻ�������  
  *        �˺����������жϣ���յ��Է��ͻ����������ݣ������÷��ͼ�������  
  * @param ��  
  * @retval ��  
  *        ע�⣺�˺���������ȫ�ֱ���'debug'�����Ա'tx_buff'��'tx_count'��  
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
  * @brief ������Խ��ջ�������  
  *        �˺����������жϣ���յ��Խ��ջ����������ݣ����ý�����ɱ�־�ͽ��ռ�������  
  * @param ��  
  * @retval ��  
  *        ע�⣺�˺���������ȫ�ֱ���'debug'�����Ա'rx_buff'��'rx_finish'��'rx_count'��  
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
  * @brief ��ʼ��UART��DMAӲ������������ص��жϺͻ�������  
  *        �˺������ʼ��USART1��UART���ܣ�����DMA����UART��TX��RX���䣬  
  *        ��ע��UART������䷢�ͺͽ��ջ�������  
  * @param ��  
  * @retval ��  
  *        ע�⣺�˺���������ȫ�ֱ���'debug'��'debug_tx_buff'��'debug_rx_buff'��  
  *               'DEBUG_TX_SIZE'��'DEBUG_RX_SIZE'��  
  */  
void bsp_uart_init(void)  
{  
    /* ��ʼ��UART��DMAӲ��������ʵ�ֿ�����main.c��MX_USART1_UART_Init()�Ⱥ����У� */  
  
    /* ע��UART������䷢�ͺͽ��ջ����� */  
    bsp_uart_register(&debug, debug_tx_buff, debug_rx_buff);  
  
    /* ����DMA����UART TX�ĵ�ַ���ڴ浽���裩 */  
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_4, (uint32_t)debug_tx_buff, (uint32_t)&(USART1->DR), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);  
    /* ����DMA����UART RX�ĵ�ַ�����赽�ڴ棩 */  
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5, (uint32_t)&(USART1->DR), (uint32_t)debug_rx_buff, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);  
  
    /* ����DMA����Ĵ�С */  
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, DEBUG_TX_SIZE);  
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, DEBUG_RX_SIZE);  
  
    /* ���UART��TX��RX������ */  
    bsp_debug_tx_clear();  
    bsp_debug_rx_clear();  
  
    /* ���USART�ı�־λ */  
    LL_USART_ClearFlag_TC(USART1);  
    LL_USART_ClearFlag_IDLE(USART1);  
  
    /* ʹ��USART��TX��RX��DMA���� */  
    LL_USART_EnableDMAReq_TX(USART1);  
    LL_USART_EnableDMAReq_RX(USART1);  
  
    /* ʹ��DMAͨ��4�Ĵ�������жϣ�������DMAͨ��5 */  
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);  
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);  
  
    /* ʹ��USART��IDLE�жϣ�����RX */  
    LL_USART_EnableIT_IDLE(USART1);  
}  

/**  
  * @brief USART1��IDLE�жϴ�������  
  *        ��USART1���յ�һ�����ݰ�������⵽IDLE��־��ʱ���˺����ᱻ���á�  
  *        ���ᴦ����յ������ݣ�����������DMA�Խ�����һ�����ݰ���  
  * @param ��  
  * @retval ��  
  */  
void USER_UART1_IRQHandler(void)
{
    uint32_t dma_length;

    if(RESET != LL_USART_IsActiveFlag_IDLE(USART1))	/* ���USART1�Ƿ��⵽IDLE��־ */ 
    {
        LL_USART_ClearFlag_IDLE(USART1);			 /* ���USART1��IDLE��־ */  

        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);	 /* ����DMAͨ��5 */  
		
		dma_length = LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_5);	//TODO: ������յ����ݳ��ȳ���DMA���õĴ��䳤�ȣ���ô��������ݻ���������ǰ��
		debug.rx_count = DEBUG_RX_SIZE - dma_length;	/* ����ʵ�ʽ��յ������ݳ��ȣ�DEBUG_RX_SIZE - ʣ�೤�ȣ� */  
		
		//���ݴ���...
		//bsp_debug_poll_printf("%s", debug.rx_buff);

		/* ���ý�����ɱ�־ */  
        debug.rx_finish = 1;
		
		  /*��������DMA�Խ�����һ�����ݰ� */ 
		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, DEBUG_RX_SIZE);
        LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
    }
}


/**  
  * @brief DMA1ͨ��4�Ĵ�������жϴ�������  
  *        ��DMA1ͨ��4������ݴ���ʱ���˺����ᱻ���á�  
  *        ���������ص��жϱ�־��������DMAͨ��4��  
  * @param ��  
  * @retval ��  
  */  
void USRT_DMA1_Channel4_IRQHandler(void)
{
	if(RESET !=LL_DMA_IsActiveFlag_TC4(DMA1))	//DMA������ɱ�־
	{
		LL_DMA_ClearFlag_TC4(DMA1);				//����жϱ�־
		LL_USART_ClearFlag_TC(USART1);			//�������1�ı�־λ
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
		debug.tx_finish = 1; 					//���÷�����ɱ�־
	}
}
