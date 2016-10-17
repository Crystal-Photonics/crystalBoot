#include <stdint.h>
#include <stdlib.h>

#include "fifo/fifo.h"

fifo_result_t fifo_init(fifo_t * fifo, uint8_t * buffer, uint16_t size)
{
    if (NULL != fifo && NULL != buffer && 0 < size) {
        fifo->buffer = buffer;
        fifo->size = size;
        fifo->first = 0;
        fifo->elements_n = 0;

        return FIFO_SUCCESS;
    } else {
        return FIFO_ERROR;
    }
}

fifo_result_t fifo_push(fifo_t * fifo, uint8_t data)
{
    if (NULL != fifo && FIFO_FALSE == fifo_is_full(fifo)) {
        fifo->buffer[(fifo->first + fifo->elements_n) % fifo->size] = data;
        ++fifo->elements_n;
        return FIFO_SUCCESS;
    } else {
        return FIFO_ERROR;
    }
}

fifo_result_t fifo_push_multiple(fifo_t * fifo, uint8_t * data, uint16_t size)
{
    if (NULL != fifo && NULL != data && fifo->elements_n + size <= fifo->size) {
        while (size--) {
            fifo->buffer[(fifo->first + fifo->elements_n) % fifo->size] = *data++;
            ++fifo->elements_n;
        }
        return FIFO_SUCCESS;
    } else {
        return FIFO_ERROR;
    }
}

fifo_result_t fifo_pop(fifo_t * fifo, uint8_t * data)
{
    if (NULL != fifo && FIFO_FALSE == fifo_is_empty(fifo)) {
        *data = fifo->buffer[fifo->first];
        fifo->first = (fifo->first + 1) % fifo->size;
        --fifo->elements_n;

        return FIFO_SUCCESS;
    } else {
        return FIFO_ERROR;
    }
}

fifo_result_t fifo_pop_multiple(fifo_t * fifo, uint8_t * data, uint16_t size)
{
    if (NULL != fifo && NULL != data && fifo->elements_n >= size) {
        while (size--) {
            *data++ = fifo->buffer[fifo->first];
            fifo->first = (fifo->first + 1) % fifo->size;
            --fifo->elements_n;
        }
        return FIFO_SUCCESS;
    } else {
        return FIFO_ERROR;
    }
}

fifo_result_t fifo_is_empty(fifo_t * fifo)
{
    if (NULL != fifo) {
        if (0 == fifo->elements_n) {
            return FIFO_TRUE;
        } else {
            return FIFO_FALSE;
        }
    } else {
        return FIFO_ERROR;
    }
}

fifo_result_t fifo_is_full(fifo_t * fifo)
{
    if (NULL != fifo) {
        if (fifo->size == fifo->elements_n) {
            return FIFO_TRUE;
        } else {
            return FIFO_FALSE;
        }
    } else {
        return FIFO_ERROR;
    }
}

inline uint16_t fifo_count_elements(fifo_t * fifo)
{
    return fifo->elements_n;
}
