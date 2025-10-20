#ifndef DATA_READER_H
#define DATA_READER_H

#include <data_buffer.h>
#include <libpynq.h>
#include <pthread.h>

typedef struct
{
    pthread_mutex_t *mutex;
    DataBuffer *db;
    int uart_index;
} reader_args;
void stop_uart_reader(void);
void *call_reader_fromargs(void *args);

int reader(pthread_mutex_t *mutex,
           DataBuffer *db,
           int uart_index);

#endif