#include <libpynq.h>
#include <pthread.h>
#include <data_processor.h>
#include "utils.h"
#include "unity.h"
#include <stdint.h>
#include "display_ops.h"

display_t display;

void test_processor_with_display(void)
{
    pynq_init();
    display_init(&display);

    displayFillScreen(&display, RGB_WHITE);

     pthread_mutex_t frequency_mutex;
    int frequency = 10;
    pthread_mutex_init(&frequency_mutex, NULL);

    pthread_mutex_t db_mutex;
    float val = 0.0f;
    pthread_mutex_init(&db_mutex, NULL);

    pthread_cond_t pulse_generated;
    pthread_cond_init(&pulse_generated, NULL);

    pthread_mutex_t spike_mutex;
    pthread_mutex_init(&spike_mutex, NULL);

    float spike_voltage = 2.0f;

    pulsegenerator_args pg_args = {
        .sampling_freq= 4000,
        .frequency_mutex = &frequency_mutex,
        .cv = &pulse_generated,
        .db = &val,
        .frequency = &frequency,
        .mutex = &db_mutex,
        .spike_voltage = &spike_voltage,
        .spike_mutex = &spike_mutex,
    };

    pthread_t generator_thread;

    pthread_create(&generator_thread, NULL, call_pulse_generator, &pg_args);

    pthread_t display_thread;

    display_loop_args d_args = {
        .buffer_mutex = &db_mutex,
        .cv = &pulse_generated,
        .db_in = &val,
        .graph_timeframe_msec = 1200,
    };

    pthread_create(&display_thread, NULL, call_display_loop_fromargs, &d_args);

    pthread_join(display_thread, NULL);

    pynq_destroy(); 
}

void test_processor_no_display(void)
{
    pthread_mutex_t frequency_mutex;
    int frequency = 500;
    pthread_mutex_init(&frequency_mutex, NULL);

    pthread_mutex_t db_mutex;
    float val = 0.0f;
    pthread_mutex_init(&db_mutex, NULL);

    pthread_cond_t pulse_generated;
    pthread_cond_init(&pulse_generated, NULL);

    pthread_mutex_t spike_mutex;
    pthread_mutex_init(&spike_mutex, NULL);

    float spike_voltage = 2.0f;

    pulsegenerator_args pg_args = {
        .sampling_freq= 4000,
        .frequency_mutex = &frequency_mutex,
        .cv = &pulse_generated,
        .db = &val,
        .frequency = &frequency,
        .mutex = &db_mutex,
        .spike_voltage = &spike_voltage,
        .spike_mutex = &spike_mutex,
    };

    pthread_t generator_thread;

    pthread_create(&generator_thread, NULL, call_pulse_generator, &pg_args);

    initialize_processor(3.3f, 0.1f, 4000, 4, 2);

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
        data_ready = 0;

        if (processor_out == 100) {
            
            pthread_mutex_unlock(&mutex_out);
            TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, n, "cutoff value reached after 1 time constant");

            break;
        }

        

        pthread_mutex_unlock(&mutex_out);
    }

    n = 0;

    int last_crying = processor_out;

    pthread_mutex_lock(&spike_mutex);
    spike_voltage = 1.0f;
    pthread_mutex_unlock(&spike_mutex);

    while (1)
    {

        pthread_mutex_lock(&mutex_out);
        while (!data_ready)
        {
            pthread_cond_wait(&cv_out, &mutex_out);
        }

        cutoff = (uint8_t)(last_crying - ((last_crying - processor_out) * 0.6));
        n++;
        data_ready = 0;
        if (processor_out <= cutoff)
        {
            TEST_ASSERT_TRUE_MESSAGE(1, "cutoff value not reached after n samples");
            pthread_mutex_unlock(&mutex_out);

            break;
        }

        

        pthread_mutex_unlock(&mutex_out);
    }
}
