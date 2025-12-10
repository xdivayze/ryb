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

void initialize_processor(float adc_reference_voltage, float difference_thresh, size_t heartbeat_sampling_frequency,
                          size_t heartbeat_ema_cutoff_sample_n, size_t buffering_ema_tau);

int input_buffering(adc_channel_t adc_channel, pthread_mutex_t *mutex_out_processor_buffer, pthread_mutex_t *mutex_out_displayer_buffer, float *db_out_processor, float *db_out_displayer, size_t warmup_count,
                    pthread_cond_t *input_cv);
void initialize_processor(float adc_reference_voltage, float difference_thresh, size_t heartbeat_sampling_frequency,
                          size_t heartbeat_ema_cutoff_sample_n, size_t buffering_ema_tau);
#endif