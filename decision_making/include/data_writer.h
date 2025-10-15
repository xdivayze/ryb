#ifndef DATA_WRITER_H
#define DATA_WRITER_H

#include <stdint.h>
#include <stdlib.h>
#include <data_buffer.h>
#include <pthread.h>

typedef struct
{
    DataBuffer *buffer;
    pthread_mutex_t *mutex;
    int uart_index;
    size_t sleep_duration_msec;
} uart_writer_args;

void *uart_writer_fromargs(void *args);

// Duty encoded byte per frame
// Duty Cycle(%) * 255(0b11111111) = data
int uart_writer(DataBuffer *buffer, pthread_mutex_t *mutex, int uart_index, size_t sleep_duration_msec);

#endif