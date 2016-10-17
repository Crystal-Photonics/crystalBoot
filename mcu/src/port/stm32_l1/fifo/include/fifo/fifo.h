#ifndef _FIFO_H_
#define _FIFO_H_

#include <stdint.h>

typedef enum {
    FIFO_FALSE = 0,
    FIFO_TRUE = 1,
    FIFO_SUCCESS = 2,
    FIFO_ERROR = 3
} fifo_result_t;

typedef struct {
    uint8_t *buffer;
    uint16_t size;
    uint16_t first;
    uint16_t elements_n;
} fifo_t;

fifo_result_t fifo_init(fifo_t * fifo, uint8_t * buffer, uint16_t size);
fifo_result_t fifo_push(fifo_t * fifo, uint8_t data);
fifo_result_t fifo_push_multiple(fifo_t * fifo, uint8_t * data, uint16_t size);
fifo_result_t fifo_pop(fifo_t * fifo, uint8_t * data);
fifo_result_t fifo_pop_multiple(fifo_t * fifo, uint8_t * data, uint16_t size);
fifo_result_t fifo_is_empty(fifo_t * fifo);
fifo_result_t fifo_is_full(fifo_t * fifo);
uint16_t fifo_count_elements(fifo_t * fifo);
fifo_result_t fifo_search(fifo_t * fifo, uint8_t data);

#endif /* _FIFO_H_ */