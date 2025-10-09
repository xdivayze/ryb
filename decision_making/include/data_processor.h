#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <data_buffer.h>
#include <pthread.h>

typedef struct
{
    pthread_mutex_t *mutex_in_buffer;
    pthread_mutex_t *mutex_out_buffer;
    DataBuffer *db_in;
    DataBuffer *db_out;
    size_t msec_sleep;
} data_process_args;

void* call_data_process_fromargs(void *args);

int data_process(pthread_mutex_t *mutex_in_buffer, pthread_mutex_t *mutex_out_buffer, DataBuffer *db_in, DataBuffer *db_out, size_t msec_sleep);

#endif