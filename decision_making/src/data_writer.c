#include "data_writer.h"
#include <libpynq.h>
#include <stdint.h>
#include <stdlib.h>

void *uart_writer_fromargs(void *args)
{
    uart_writer_args *writer_args = (uart_writer_args *)args;
    return (void *)uart_writer(writer_args->buffer, writer_args->mutex, writer_args->uart_index, writer_args->sleep_duration_msec);
}

int uart_writer(DataBuffer *buffer, pthread_mutex_t *mutex, int uart_index, size_t sleep_duration_msec)
{
    uart_reset_fifos(uart_index);

    uint8_t *val = malloc(sizeof(uint8_t) * buffer->array_len);
    while (1)
    {
        if (buffer->count == 0)
        {
            continue;
        }
        pthread_mutex_lock(mutex);
        databuffer_pop(buffer, val);
        pthread_mutex_unlock(mutex);

        for (size_t i = 0; i < buffer->array_len; i++)
        {
            uart_send(uart_index, val[i]);
        }
        sleep_msec(sleep_duration_msec);
    }
    free(val);
}