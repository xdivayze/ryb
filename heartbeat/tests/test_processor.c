#include <libpynq.h>
#include <pthread.h>
#include <data_processor.h>
#include "utils.h"
#include "unity.h"
#include <stdint.h>

display_t display;

void test_processor_with_display(void)
{
    pthread_mutex_t bpm_mutex;
    int bpm = 180;
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

    initialize_processor(3.3f, spike_voltage - 0.2f, 200, 4, 3);

    uint8_t data_ready = 0;
    uint8_t processor_out = 0;
    pthread_mutex_t mutex_out;
    pthread_mutex_init(&mutex_out, NULL);

    pthread_cond_t cv_out;
    pthread_cond_init(&cv_out, NULL);

    processor_args p_args = {
        .data_ready = &data_ready,
        .db_in = &val,
        .db_out = &processor_out,
        .input_cv = &pulse_generated,
        .mutex_in_buffer = &db_mutex,
        .mutex_out_buffer = &mutex_out,
        .output_cv = &cv_out,
    };

    int n = 0;

    pthread_t processor_thread;
    pthread_create(&processor_thread, NULL, call_data_process_fromargs, &p_args);
}

void test_processor_no_display(void)
{
    pthread_mutex_t bpm_mutex;
    int bpm = 180;
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

    initialize_processor(3.3f, spike_voltage - 0.2f, 200, 4, 3);

    uint8_t data_ready = 0;
    uint8_t processor_out = 0;
    pthread_mutex_t mutex_out;
    pthread_mutex_init(&mutex_out, NULL);

    pthread_cond_t cv_out;
    pthread_cond_init(&cv_out, NULL);

    processor_args p_args = {
        .data_ready = &data_ready,
        .db_in = &val,
        .db_out = &processor_out,
        .input_cv = &pulse_generated,
        .mutex_in_buffer = &db_mutex,
        .mutex_out_buffer = &mutex_out,
        .output_cv = &cv_out,
    };

    int n = 0;

    pthread_t processor_thread;
    pthread_create(&processor_thread, NULL, call_data_process_fromargs, &p_args);
    uint8_t cutoff = 0xFF;
    while (1)
    {

        pthread_mutex_lock(&mutex_out);
        while (!data_ready)
        {
            pthread_cond_wait(&cv_out, &mutex_out);
        }
        n++;

        cutoff = (uint8_t)(240 - ((240 - bpm) * 0.6));
        if (processor_out <= cutoff)
        {
            TEST_ASSERT_TRUE_MESSAGE(1, "cutoff value not reached after n samples");
            pthread_mutex_unlock(&mutex_out);
            TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, n, "cutoff value reached after 1 time constant");

            break;
        }

        data_ready = 0;

        pthread_mutex_unlock(&mutex_out);
    }

    n = 0;

    int last_bpm = processor_out;

    pthread_mutex_lock(&bpm_mutex);
    bpm = 100;
    pthread_mutex_unlock(&bpm_mutex);

    while (1)
    {

        pthread_mutex_lock(&mutex_out);
        while (!data_ready)
        {
            pthread_cond_wait(&cv_out, &mutex_out);
        }

        cutoff = (uint8_t)(last_bpm - ((last_bpm - bpm) * 0.6));
        n++;
        if (processor_out <= cutoff)
        {
            TEST_ASSERT_TRUE_MESSAGE(1, "cutoff value not reached after n samples");
            pthread_mutex_unlock(&mutex_out);
            printf("cutoff reached for 180->100 at %i samples\n", n);

            break;
        }

        data_ready = 0;

        pthread_mutex_unlock(&mutex_out);
    }
}