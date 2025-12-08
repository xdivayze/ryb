#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <pthread.h>
#include <stdint.h>
#include <adc.h>

#define SINGLE_SIDE_BUFFER_CAPACITY 10

void stop_processing(void);
void stop_input_buffering(void);
extern size_t heartbeat_sampling_frequency;
extern float adc_reference_voltage;

void *call_data_process_fromargs(void *args);

void initialize_processor();

int input_buffering(adc_channel_t adc_channel, pthread_mutex_t *mutex_out_buffer, float *db_out, size_t warmup_count);

int data_process(pthread_mutex_t *mutex_in_buffer, pthread_mutex_t *mutex_out_buffer, float *db_in,
                 uint8_t *db_out, uint8_t *data_ready);
#endif