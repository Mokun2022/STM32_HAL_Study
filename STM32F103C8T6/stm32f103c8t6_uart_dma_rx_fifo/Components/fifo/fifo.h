/*
 * fifo.h
 *
 *  Created on: Apr 10, 2023
 *      Author: Fdata
 */

#ifndef UART_FIFO_H_
#define UART_FIFO_H_

#include <stdint.h>

typedef void (*lock_fun)(void);

typedef struct
{
	void (*lock)(void);		/* Exclusive lock */
	void (*unlock)(void);	/* Exclusive unlock */
	uint32_t buf_size;    	/* Size of buff */
    uint32_t occupy_size;   /* Occupation size */
	uint8_t *buf;      	    /* Buff of fifo */
	uint8_t *pwrite;      	/* Tail of queue */
	uint8_t *pread;       	/* Head of queue*/
}fifo_t;

void fifo_register(fifo_t *pfifo, uint8_t *pfifo_buf, uint32_t size, lock_fun lock, lock_fun unlock);
void fifo_release(fifo_t *pfifo);

uint32_t fifo_get_occupy_size(fifo_t *pfifo);
uint32_t fifo_get_total_size(fifo_t *pfifo);
uint32_t fifo_get_free_size(fifo_t *pfifo);
uint32_t fifo_write(fifo_t *pfifo, const uint8_t *pbuf, uint32_t size);
uint32_t fifo_read(fifo_t *pfifo, uint8_t *pbuf, uint32_t size);



#endif /* UART_FIFO_H_ */
