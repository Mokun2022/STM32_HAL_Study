/*
 * fifo.c
 *
 *  Created on: Apr 10, 2023
 *      Author: Fdata
 */

#include "fifo.h"
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

/**
  * @brief  Register the fifo
  * @param  pfifo: fifo structure pointer
		    pfifo_buf: Buff of fifo
		    size: Size of buff
		    lock: Process lock
		    unlock: Process unlock
  * @retval none
*/
void fifo_register(fifo_t *pfifo, uint8_t *pfifo_buf, uint32_t size,
                   lock_fun lock, lock_fun unlock)
{
	pfifo->buf_size = size;
	pfifo->buf 	= pfifo_buf;
	pfifo->pwrite = pfifo->buf;
	pfifo->pread  = pfifo->buf;
    pfifo->occupy_size = 0;
    pfifo->lock = lock;
    pfifo->unlock = unlock;
}

/**
  * @brief  release the fifo
  * @param  pfifo: Struct of fifo
  * @retval none
*/
void fifo_release(fifo_t *pfifo)
{
	pfifo->buf_size = 0;
    pfifo->occupy_size = 0;
	pfifo->buf 	= NULL;
	pfifo->pwrite = 0;
	pfifo->pread  = 0;
    pfifo->lock = NULL;
    pfifo->unlock = NULL;
}


/**
  * @brief  Gets the total size of the fifo
  * @param  pfifo: fifo structure pointer
  * @retval Size of fifo
*/
uint32_t fifo_get_total_size(fifo_t *pfifo)
{
	if(pfifo==NULL)
		return 0;

	return pfifo->buf_size;
}

/**
  * @brief  Gets the used size of the fifo
  * @param  pfifo: fifo structure pointer
  * @retval Used size of fifo
*/
uint32_t fifo_get_occupy_size(fifo_t *pfifo)
{
	if(pfifo==NULL)
		return 0;

	return  pfifo->occupy_size;
}

/**
  * @brief  Gets the free size of the fifo
  * @param  pfifo: fifo structure pointer
  * @retval Free size of fifo
*/
uint32_t fifo_get_free_size(fifo_t *pfifo)
{
	uint32_t size;

	if(pfifo==NULL)
		return 0;

    size = pfifo->buf_size - fifo_get_occupy_size(pfifo);

	return size;
}


/**
  * @brief  Write data to the fifo
  * @param  pfifo: fifo structure pointer
		    pbuf: Write of data
		    size: Size of Write data
  * @retval Actual write size
*/
uint32_t fifo_write(fifo_t *pfifo, const uint8_t *pbuf, uint32_t size)
{
	uint32_t w_size= 0,free_size = 0;

	if((size==0) || (pfifo==NULL) || (pbuf==NULL))
	{
		return 0;
	}

    free_size = fifo_get_free_size(pfifo);
    if(free_size == 0)
    {
        return 0;
    }

    if(free_size < size)
    {
        size = free_size;
    }
	w_size = size;
    if(pfifo->lock != NULL)
    {
        pfifo->lock();
    }
	while(w_size-- > 0)
	{
		*pfifo->pwrite++ = *pbuf++;
		if (pfifo->pwrite >= (pfifo->buf + pfifo->buf_size))
		{
			pfifo->pwrite = pfifo->buf;
		}
        pfifo->occupy_size++;
	}
    if(pfifo->unlock != NULL)
    {
        pfifo->unlock();
    }
	return size;
}

/**
  * @brief  Read data from the fifo
  * @param  pfifo: fifo structure pointer
		    pbuf:  read of data
		    size: Size of data
  * @retval Actual read size
*/
uint32_t fifo_read(fifo_t *pfifo, uint8_t *pbuf, uint32_t size)
{
	uint32_t r_size = 0,occupy_size = 0;

	if ((size==0) || (pfifo==NULL) || (pbuf==NULL))
	{
		return 0;
	}

    occupy_size = fifo_get_occupy_size(pfifo);
    if(occupy_size == 0)
    {
        return 0;
    }

    if(occupy_size < size)
    {
        size = occupy_size;
    }
    if (pfifo->lock != NULL)
    {
        pfifo->lock();
    }
	r_size = size;
	while(r_size-- > 0)
	{
		*pbuf++ = *pfifo->pread++;
		if (pfifo->pread >= (pfifo->buf + pfifo->buf_size))
		{
			pfifo->pread = pfifo->buf;
		}
        pfifo->occupy_size--;
	}
    if (pfifo->unlock != NULL)
    {
        pfifo->unlock();
    }
	return size;
}


