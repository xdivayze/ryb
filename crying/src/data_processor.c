#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <libpynq.h>

#include "data_processor.h"

#include <time.h>
#include <math.h>

#define HEARTBEAT_DISPLAY_TEXT "IN1"
#define CRYING_DISPLAY_TEXT "IN2"

#define OUT_FREQ_TEXT "OUT1"
#define OUT_AMPL_TEXT "OUT2"

float adc_reference_voltage = 3.3f;
static float process_thresh = 0.1f;
size_t crying_sampling_frequency = 200;
static size_t crying_ema_cutoff_sample_n = 4;
static size_t buffering_ema_tau = 3;

static inline float convert_raw_adc_to_voltage(uint32_t adc_raw)
{
    return ((float)adc_raw / (float)UINT16_MAX) * adc_reference_voltage;
}

static volatile int keep_processor_running = 1;

void stop_processing(void)
{
    keep_processor_running = 0;
}

static volatile int keep_input_buffering_running = 1;

void stop_input_buffering(void)
{
    keep_input_buffering_running = 0;
}

void *call_input_buffering_fromargs(void *void_args)
{
    input_buffering_args *args = void_args;
    input_buffering(args->adc_channel, args->mutex_out_processor_buffer, args->mutex_out_displayer_buffer,
                    args->db_out_processor, args->db_out_displayer, args->warmup_count, args->input_cv);
    return NULL;
}

void *call_data_process_fromargs(void *void_args)
{
    processor_args *args = void_args;
    data_process(args->mutex_in_buffer, args->mutex_out_buffer, args->db_in, args->db_out, args->data_ready,
                 args->input_cv, args->output_cv);
    return NULL;
}

static double get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (double)ts.tv_sec * 1000.0 +
           (double)ts.tv_nsec / 1e6;
}

void initialize_processor(float adc_reference_voltage, float process_thresh, size_t crying_sampling_frequency,
                          size_t crying_ema_cutoff_sample_n, size_t buffering_ema_tau)
{
    adc_reference_voltage = adc_reference_voltage;
    process_thresh = process_thresh;
    crying_sampling_frequency = crying_sampling_frequency;
    crying_ema_cutoff_sample_n = crying_ema_cutoff_sample_n;
    buffering_ema_tau = buffering_ema_tau;
}

int input_buffering(adc_channel_t adc_channel, pthread_mutex_t *mutex_out_processor_buffer, pthread_mutex_t *mutex_out_displayer_buffer, float *db_out_processor, float *db_out_displayer, size_t warmup_count,
                    pthread_cond_t *input_cv)
{
    uint32_t raw_adc = adc_read_channel_raw(adc_channel);
    float current_voltage = convert_raw_adc_to_voltage(raw_adc);
    float alpha = 1.0f - expf((-1.0f / crying_sampling_frequency) / buffering_ema_tau);
    float baseline = current_voltage;

    struct timespec ts;
    ts.tv_nsec = (1 / (float)crying_sampling_frequency) * 1000000;
    ts.tv_sec = (1 / (float)crying_sampling_frequency) / 1000;

    for (int i = 0; i < warmup_count; i++)
    {
        current_voltage = convert_raw_adc_to_voltage(adc_read_channel_raw(adc_channel));
        baseline = current_voltage * alpha + baseline * (1.0f - alpha);
        nanosleep(&ts, NULL);
    }

    while (keep_input_buffering_running)
    {
        current_voltage = convert_raw_adc_to_voltage(adc_read_channel_raw(adc_channel));

        pthread_mutex_lock(mutex_out_processor_buffer);
        pthread_mutex_lock(mutex_out_displayer_buffer);

        db_out_processor[0] = current_voltage - baseline;
        db_out_displayer[0] = current_voltage - baseline;

        // printf("voltage spike at:%.2f\n", current_voltage-baseline);

        pthread_mutex_unlock(mutex_out_processor_buffer);
        pthread_mutex_unlock(mutex_out_displayer_buffer);

        pthread_cond_broadcast(input_cv);

        baseline = current_voltage * alpha + baseline * (1.0f - alpha);
        nanosleep(&ts, NULL);
    }
}

int data_process(pthread_mutex_t *mutex_in_buffer, pthread_mutex_t *mutex_out_buffer, float *db_in,
                 uint8_t *db_out, uint8_t *data_ready, pthread_cond_t *input_cv, pthread_cond_t *output_cv)
{

    float vmax = 0;

    float ema_bpm_alpha = 1.0f - expf(-1.0f / crying_ema_cutoff_sample_n);
    float ema_bpm_initial = 240;

    float instantaneous_crying = ema_bpm_initial;
    float last_instantaneous_crying = instantaneous_crying;

    int crying_ready_counter = 0;

    float ema_crying = instantaneous_crying;

    float current_voltage = 0.0f;


    while (keep_processor_running)
    {
        pthread_mutex_lock(mutex_in_buffer);
        pthread_cond_wait(input_cv, mutex_in_buffer);
        current_voltage = db_in[0];
        pthread_mutex_unlock(mutex_in_buffer);

        if (current_voltage > process_thresh)
        {
            last_instantaneous_crying = instantaneous_crying;
            ema_crying = ((1.0f - ema_bpm_alpha) * ema_crying) + (ema_bpm_alpha * instantaneous_crying);
            crying_ready_counter++;
        }

        if (crying_ready_counter == crying_ema_cutoff_sample_n - 1)
        {
            pthread_mutex_lock(mutex_out_buffer);
            db_out[0] = (uint8_t)ema_crying;
            data_ready[0] = 1;
            pthread_mutex_unlock(mutex_out_buffer);
            pthread_cond_broadcast(output_cv);
            crying_ready_counter = 0;
        }
    }
    return 0;
}
