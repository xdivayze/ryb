#include "unity.h"
#include "utils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void test_pulse_generator(void)
{

    pthread_mutex_t frequency_mutex;
    int frequency = 500;
    pthread_mutex_init(&frequency_mutex, NULL);

    pthread_mutex_t db_mutex;
    float val = 0.0f;
    pthread_mutex_init(&db_mutex, NULL);

    pthread_cond_t pulse_generated;
    pthread_cond_init(&pulse_generated, NULL);

    float spike_voltage = 1.2f;

    int sampling_freq = 4000;

    pulsegenerator_args pg_args = {
        .sampling_freq = sampling_freq,
        .frequency_mutex = &frequency_mutex,
        .cv = &pulse_generated,
        .db = &val,
        .frequency = &frequency,
        .mutex = &db_mutex,
        .spike_voltage = spike_voltage,
    };

    pthread_t generator_thread;

    pthread_create(&generator_thread, NULL, call_pulse_generator, &pg_args);
    double last_time = get_time_ms();
    double curr_time = last_time;

    float curr_freq = 0.0f;

    while (1)
    {

        pthread_mutex_lock(&db_mutex);
        pthread_cond_wait(&pulse_generated, &db_mutex);

        if (val != spike_voltage)
        {
            pthread_mutex_unlock(&db_mutex);
            continue;
        }

        last_time = curr_time;
        curr_time = get_time_ms();

        pthread_mutex_unlock(&db_mutex);

        curr_freq = 1.0f / ((curr_time - last_time) / 1000.0f);
        if (fabs (curr_freq - frequency) < 100)
        {
            TEST_ASSERT_TRUE_MESSAGE(1, "heartbeat frequency not within error range");
            break;
        }
    }
    pthread_mutex_lock(&frequency_mutex);
    frequency = 100;
    pthread_mutex_unlock(&frequency_mutex);

    while (1)
    {

        pthread_mutex_lock(&db_mutex);
        pthread_cond_wait(&pulse_generated, &db_mutex);

        if (val != spike_voltage)
        {
            pthread_mutex_unlock(&db_mutex);
            continue;
        }

        last_time = curr_time;
        curr_time = get_time_ms();

        pthread_mutex_unlock(&db_mutex);

        curr_freq = 1.0f / ((curr_time - last_time) / 1000.0f);

        if (fabs(curr_freq - frequency) < 20)
        {
            TEST_ASSERT_TRUE_MESSAGE(1, "updated heartbeat frequency not within error range");
            break;
        }
    }
}