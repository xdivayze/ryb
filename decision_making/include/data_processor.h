#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <data_buffer.h>
#include <pthread.h>
#include <stdint.h>
#include "display_ops.h"

#define SINGLE_SIDE_BUFFER_CAPACITY 10

#define N_HEARTBEAT_SAMPLES 10
#define SAMPLING_SLEEP 25

void stop_processing(void);

typedef struct
{
    pthread_mutex_t *mutex_in_buffer;
    pthread_mutex_t *mutex_out_buffer;
    DataBuffer *db_in;
    DataBuffer *db_out;
    size_t msec_sleep;
    FontxFile* font_file;
} data_process_args;

void *call_data_process_fromargs(void *args);

// return -1 if initial stat values are not within range
int data_process(pthread_mutex_t *mutex_in_buffer, pthread_mutex_t *mutex_out_buffer, DataBuffer *db_in, DataBuffer *db_out,
                 size_t msec_sleep, FontxFile* font_file);
#endif