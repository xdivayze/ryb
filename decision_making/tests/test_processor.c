#include <libpynq.h>
#include <data_buffer.h>
#include <pthread.h>
#include <data_processor.h>
#include "utils.h"
#include "baby_mock.h"
#include "unity.h"


static struct data_process_args
{
    pthread_mutex_t *mutex_in_buffer;
    pthread_mutex_t *mutex_out_buffer;
    DataBuffer *db_in;
    DataBuffer *db_out;
    size_t msec_sleep;
    FontxFile *font_file;
};

static struct baby_mock_args
{
    DataBuffer *db_in;
    pthread_mutex_t *mutex_in;
    DataBuffer *db_out;
    pthread_mutex_t *mutex_out;
    int initial_stress;
};



static void *baby_mock_runner(void *args)
{
    struct baby_mock_args *baby_args = (struct baby_mock_args *)args;

    start_baby_loop(baby_args->db_in, baby_args->mutex_in, baby_args->db_out, baby_args->mutex_out, baby_args->initial_stress);
    return NULL;
}


static void *start_data_process(void *args)
{
    struct data_process_args* casted_args = (struct data_process_args*) args;
    data_process(casted_args->mutex_in_buffer, casted_args->mutex_out_buffer, casted_args->db_in, casted_args->db_out, casted_args->msec_sleep, casted_args->font_file);
    return NULL;
}


void test_processor(void)
{
    max_stress = 100;
    pynq_init();
    initialize_display();
    displayFillScreen(&display, RGB_WHITE);

    DataBuffer *db_in = databuffer_create(10, 2);  // vital signs
    DataBuffer *db_out = databuffer_create(10, 2); // motor data

    pthread_mutex_t mutex_in;
    pthread_mutex_init(&mutex_in, NULL);

    pthread_mutex_t mutex_out;
    pthread_mutex_init(&mutex_out, NULL);

    struct baby_mock_args baby_args = {
        .db_in = db_out,
        .db_out = db_in,
        .initial_stress = 100,
        .mutex_in = &mutex_out,
        .mutex_out = &mutex_in,
    };

    pthread_t baby_thread;
    pthread_create(&baby_thread, NULL, baby_mock_runner, &baby_args);

    FontxFile fx16G[2];
    InitFontx(fx16G, FONT_PATH, "");

    struct data_process_args processor_args = {
        .db_in = db_in,
        .db_out = db_out,
        .msec_sleep = BABY_REACTION_DELAY_MSEC * 2,
        .mutex_in_buffer = &mutex_in,
        .mutex_out_buffer = &mutex_out,
        .font_file = fx16G,
        
    };

    pthread_t processor_thread;
    pthread_create(&processor_thread, NULL, start_data_process, &processor_args);

    pthread_join(processor_thread, NULL);

    keep_baby_mock_running = 0;

    pynq_destroy();
    display_initialized = 0;

    TEST_PASS();
}