#include <libpynq.h>
#include <pthread.h>
#include <data_processor.h>
#include <stdint.h>
#include <display_ops.h>

#define ADC_CHANNEL ADC2

const int spike_voltage = 1.0f;

display_t display;

int main()
{
    pynq_init();

    adc_init();
    display_init(&display);

    initialize_processor(3.3f, spike_voltage - 0.2f, 200, 4, 3);

    pthread_mutex_t mutex_db_display;
    pthread_mutex_init(&mutex_db_display, NULL);

    float db_display;

    pthread_mutex_t mutex_db_processor;
    pthread_mutex_init(&mutex_db_processor, NULL);
    float db_processor;

    pthread_cond_t cv_processor;
    pthread_cond_init(&cv_processor, NULL);

    input_buffering_args input_args = {
        .adc_channel = ADC_CHANNEL,
        .db_out_displayer = &db_display,
        .db_out_processor = &db_processor,
        .input_cv = &cv_processor,
        .mutex_out_displayer_buffer = &mutex_db_display,
        .mutex_out_processor_buffer = &mutex_db_processor,
        .warmup_count = 50,
    };

    uint8_t data_ready = 0;
    uint8_t processor_out = 0;
    pthread_mutex_t mutex_out;
    pthread_mutex_init(&mutex_out, NULL);

    pthread_cond_t cv_out;
    pthread_cond_init(&cv_out, NULL);

    processor_args p_args = {
        .data_ready = &data_ready,
        .db_in = &db_processor,
        .db_out = &processor_out,
        .input_cv = &cv_processor,
        .mutex_in_buffer = &mutex_db_processor,
        .mutex_out_buffer = &mutex_out,
        .output_cv = &cv_out,
    };

    display_loop_args display_args = {
        .buffer_mutex = &mutex_db_display,
        .cv = &cv_processor,
        .db_in = &db_display,
        .graph_timeframe_msec = 1200,
    };

    pthread_t display_thread;
    pthread_create(&display_thread, NULL, call_display_loop_fromargs, &display_args);

    pthread_t processor_thread;
    pthread_create(&processor_thread, NULL, call_data_process_fromargs, &p_args);

    pthread_t input_thread;

    pthread_create(&input_thread, NULL, call_input_buffering_fromargs, &input_args);

    pthread_join(input_thread, NULL);
    pthread_join(processor_thread, NULL);

    pynq_destroy();

    return 0;
}
