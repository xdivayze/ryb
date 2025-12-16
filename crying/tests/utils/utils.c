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
    pulse_generator(args->bpm, args->frequency, args->spike_voltage, args->db, args->cv, args->mutex, args->bpm_mutex);
    return NULL;
}
void pulse_generator(int *bpm, int frequency, float spike_voltage, float *db, pthread_cond_t *cv, pthread_mutex_t *mutex, pthread_mutex_t *bpm_mutex)
{
    struct timespec ts;
    ts.tv_nsec = (1.0f / frequency) * 1e9;
    ts.tv_sec = (1.0f / frequency);

    pthread_mutex_lock(bpm_mutex);
    int pulse_frequency_discrete = floorf(frequency / ((*bpm) / 60.0f));
    pthread_mutex_unlock(bpm_mutex);

    int n = 1;

    while (keep_pulse_generator_running)
    {
        pthread_mutex_lock(bpm_mutex);
        pulse_frequency_discrete = floorf(frequency / ((*bpm) / 60.0f));
        pthread_mutex_unlock(bpm_mutex);

        pthread_mutex_lock(mutex);
        *db = (n % pulse_frequency_discrete == 0) ? spike_voltage : 0;
        pthread_mutex_unlock(mutex);
        pthread_cond_broadcast(cv);

        n++;

        nanosleep(&ts, NULL);
    }
}
