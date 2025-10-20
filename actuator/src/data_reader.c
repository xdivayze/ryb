#include "data_reader.h"
#include <libpynq.h>
static volatile int keep_running = 1;
#include <stdint.h>

void stop_uart_reader(void)
{
    keep_running = 0;
}

void *call_reader_fromargs(void *args)
{
    reader_args *casted_args = (reader_args *)args;
    return (void*) reader(casted_args->mutex, casted_args->db, casted_args->uart_index);
}

int reader(pthread_mutex_t *mutex,
           DataBuffer *db,
           int uart_index)
{

    uart_reset_fifos(uart_index);
    uint8_t* vals = malloc(sizeof(uint8_t) * db->array_len);
    while (keep_running)
    {
        for (int i = 0; i < db->array_len; i++)
        {
            vals[i] = uart_recv(uart_index);
        }
        databuffer_push( vals, db);
    }
    free(vals);
}