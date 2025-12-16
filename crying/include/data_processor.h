#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <pthread.h>
#include <stdint.h>
#include <adc.h>

#define SINGLE_SIDE_BUFFER_CAPACITY 10

void stop_processing(void);
void stop_input_buffering(void);
extern size_t crying_sampling_frequency;
extern float adc_reference_voltage;

typedef struct
{
    pthread_mutex_t *mutex_in_buffer;
    pthread_mutex_t *mutex_out_buffer;
    float *db_in;
    uint8_t *db_out;
    uint8_t *data_ready;
    pthread_cond_t *input_cv;
    pthread_cond_t *output_cv;
} processor_args;

void *call_data_process_fromargs(void *void_args);

typedef struct
{
    adc_channel_t adc_channel;
    pthread_mutex_t *mutex_out_processor_buffer;
    pthread_mutex_t *mutex_out_displayer_buffer;
    float *db_out_processor;
    float *db_out_displayer;
    size_t warmup_count;
    pthread_cond_t *input_cv;
} input_buffering_args;
void *call_input_buffering_fromargs(void *void_args);
void initialize_processor(float adc_reference_voltage, float process_thresh, size_t heartbeat_sampling_frequency,
                          size_t crying_ema_cutoff_sample_n, size_t buffering_ema_tau);

int input_buffering(adc_channel_t adc_channel, pthread_mutex_t *mutex_out_processor_buffer, pthread_mutex_t *mutex_out_displayer_buffer, float *db_out_processor, float *db_out_displayer, size_t warmup_count,
                    pthread_cond_t *input_cv);
int data_process(pthread_mutex_t *mutex_in_buffer, pthread_mutex_t *mutex_out_buffer, float *db_in,
                 uint8_t *db_out, uint8_t *data_ready, pthread_cond_t *input_cv, pthread_cond_t *output_cv);

#endif