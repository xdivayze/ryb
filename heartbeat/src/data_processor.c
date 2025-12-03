#include <data_buffer.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <libpynq.h>
#include "data_processor.h"
#include <time.h>

#define HEARTBEAT_DISPLAY_TEXT "IN1"
#define CRYING_DISPLAY_TEXT "IN2"

#define OUT_FREQ_TEXT "OUT1"
#define OUT_AMPL_TEXT "OUT2"

#define REFERENCE_VOLTAGE 3.3f

static inline float convert_raw_adc_to_voltage(uint32_t adc_raw)
{
    return ((float)adc_raw / (float)UINT16_MAX) * 3.3f;
}

static volatile int keep_running = 1;

void stop_processing(void)
{
    keep_running = 0;
}

static void int_as_string(uint8_t num, char *buf)
{
    sprintf(buf, "%d", num);
}

static double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (double)ts.tv_sec * 1000.0 +
           (double)ts.tv_nsec / 1e6;
}

const float difference_thresh = 0.5f;

int data_process(pthread_mutex_t *mutex_in_buffer, pthread_mutex_t *mutex_out_buffer, DataBuffer *db_in, DataBuffer *db_out,
                 size_t msec_sleep, display_t *display, adc_channel_t adc_channel)
{

    uint32_t raw_adc = adc_read_channel_raw(adc_channel);

    float current_voltage = convert_raw_adc_to_voltage(raw_adc);
    float last_voltage = current_voltage;
    float vdiff = 0;
    float last_rise_ms = 0;
    float current_rise_ms = 0;

    while (keep_running)
    {
        raw_adc = adc_read_channel_raw(adc_channel);
        last_voltage = current_voltage;
        current_voltage = convert_raw_adc_to_voltage(raw_adc);
        vdiff = (current_voltage - last_voltage);
        if (vdiff > difference_thresh)
        {
            last_rise_ms = current_rise_ms;
            current_rise_ms = get_time_ms();
            printf("edge has risen. instantenous frequency: %.8f\n", (1.0f/((current_rise_ms - last_rise_ms) / 1000)));
        }
        sleep_msec(msec_sleep);
    }
    return 0;
}
