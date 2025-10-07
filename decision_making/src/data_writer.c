#include "data_writer.h"
#include <libpynq.h>
#include <stdint.h>
#include <stdlib.h>

static inline uint32_t ticks_duty_cycle_calculator(uint8_t data, uint32_t ticks_period)
{
    return (ticks_period * data) / UINT8_MAX;
}

int pwm_multiplex_writer(size_t msec_signal_duration, uint32_t ticks_period, uint8_t sync_signal, DataBuffer *buffer, pthread_mutex_t *mutex, int pwm_chan)
{
    uint8_t *val = malloc(sizeof(uint8_t) * buffer->array_len);
    while (1)
    {
        pthread_mutex_lock(mutex);
        databuffer_pop(buffer, val);
        pthread_mutex_unlock(mutex);

        pwm_set_duty_cycle(pwm_chan, ticks_duty_cycle_calculator(sync_signal, ticks_period));
        sleep_msec(msec_signal_duration);

        for (size_t i = 0; i < buffer->array_len; i++)
        {
            pwm_set_duty_cycle(pwm_chan, ticks_duty_cycle_calculator(val[i], ticks_period));
            sleep_msec(msec_signal_duration);
        }
    }
    free(val);
}