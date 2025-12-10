#ifndef UTILS_H
#define UTILS_H
#include <pthread.h>
typedef struct
{
    int *bpm;
    int frequency;
    float spike_voltage;
    float *db;
    pthread_cond_t *cv;
    pthread_mutex_t *mutex;
    pthread_mutex_t *bpm_mutex;
} pulsegenerator_args;

double get_time_ms(void);
void *call_pulse_generator(void *void_args);
void pulse_generator(int *bpm, int frequency, float spike_voltage, float *db,
                     pthread_cond_t *cv, pthread_mutex_t *mutex, pthread_mutex_t *bpm_mutex);
#endif