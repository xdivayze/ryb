#include "utils.h"
#include <libpynq.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

double get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (double)ts.tv_sec * 1000.0 +
           (double)ts.tv_nsec / 1e6;
}

static volatile int keep_pulse_generator_running = 1;
void *call_pulse_generator(void *void_args)
{
    pulsegenerator_args *args = void_args;
    pulse_generator(args->sampling_freq, args->frequency, args->spike_voltage, args->db, args->cv, args->mutex, args->frequency_mutex);
    return NULL;
}
void pulse_generator(int sampling_freq, int *frequency, float spike_voltage, float *db, pthread_cond_t *cv, pthread_mutex_t *mutex, pthread_mutex_t *frequency_mutex)
{
    struct timespec ts;
    ts.tv_nsec = (1.0f / sampling_freq) * 1e9;
    ts.tv_sec = (1.0f / sampling_freq);
    pthread_mutex_lock(frequency_mutex);
    float pulse_frequency_discrete = 2 * M_PI * ((float)(*frequency) / sampling_freq);
    pthread_mutex_unlock(frequency_mutex);

    int n = 0;

    while (keep_pulse_generator_running)
    {
        pthread_mutex_lock(frequency_mutex);
        pulse_frequency_discrete = 2 * M_PI * ((float)(*frequency) / sampling_freq);
        pthread_mutex_unlock(frequency_mutex);

        pthread_mutex_lock(mutex);
        *db = spike_voltage * sinf(pulse_frequency_discrete * n);
        pthread_mutex_unlock(mutex);
        pthread_cond_broadcast(cv);

        n++;

        nanosleep(&ts, NULL);
    }
}
