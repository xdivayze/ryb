#include "unity.h"
#include "utils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static void *call_pulse_generator(void *void_args)
{
    pulsegenerator_args *args = void_args;
    pulse_generator(args->bpm, args->frequency, args->spike_voltage, args->db, args->cv, args->mutex, args->bpm_mutex);
    return NULL;
}

void test_pulse_generator(void)
{

    pthread_mutex_t bpm_mutex;
    int bpm = 240;
    pthread_mutex_init(&bpm_mutex, NULL);

    pthread_mutex_t db_mutex;
    float val = 0.0f;
    pthread_mutex_init(&db_mutex, NULL);

    pthread_cond_t pulse_generated;
    pthread_cond_init(&pulse_generated, NULL);

    float spike_voltage = 1.2f;

    pulsegenerator_args pg_args = {
        .bpm = &bpm,
        .bpm_mutex = &bpm_mutex,
        .cv = &pulse_generated,
        .db = &val,
        .frequency = 200,
        .mutex = &db_mutex,
        .spike_voltage = spike_voltage,
    };

    pthread_t generator_thread;

    pthread_create(&generator_thread, NULL, call_pulse_generator, &pg_args);
    double last_time = get_time_ms();
    double curr_time = last_time;

    int curr_bpm = 0;

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

        curr_bpm = ceilf(60.0f / ((curr_time - last_time) / 1000.0f));
        if (abs(curr_bpm - bpm) < 10)
        {
            TEST_ASSERT_TRUE_MESSAGE(1, "heartbeat frequency not within error range");
            break;
        }
    }
    pthread_mutex_lock(&bpm_mutex);
    bpm = 100;
    pthread_mutex_unlock(&bpm_mutex);

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

        curr_bpm = ceilf(60.0f / ((curr_time - last_time) / 1000.0f));
        if (abs(curr_bpm - bpm) < 10)
        {
            TEST_ASSERT_TRUE_MESSAGE(1, "heartbeat frequency not within error range");
            break;
        }
    }
}