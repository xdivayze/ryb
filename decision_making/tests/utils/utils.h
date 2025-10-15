#ifndef UTILS_H
#define UTILS_H

#include <libpynq.h>
#include <stdint.h>
#include <data_buffer.h>
#include <pthread.h>

typedef struct
{
    iic_index_t IIC_INDEX;
    uint8_t IIC_ADDR;
    uint32_t data_to_send;
    uint8_t register_to_write;
} writer_args;

typedef struct
{
    int uart_index;
    DataBuffer *db_reader;
    pthread_mutex_t *mutex_reader;
} reader_args;

void stop_i2c_writer(void);
void stop_uart_reader(void);

void *i2c_writer(void *args);
void *uart_reader(void *args);

#endif