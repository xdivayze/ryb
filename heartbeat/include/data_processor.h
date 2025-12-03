#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <data_buffer.h>
#include <pthread.h>
#include <stdint.h>
#include <adc.h>

#define SINGLE_SIDE_BUFFER_CAPACITY 10

void stop_processing(void);

void *call_data_process_fromargs(void *args);

int data_process(pthread_mutex_t *mutex_in_buffer, pthread_mutex_t *mutex_out_buffer, DataBuffer *db_in, DataBuffer *db_out, size_t msec_sleep, display_t *display, adc_channel_t adc_channel);

#endif