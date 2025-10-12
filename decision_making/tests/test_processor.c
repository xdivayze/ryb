#include <libpynq.h>
#include <data_buffer.h>
#include <pthread.h>
#include <data_processor.h>

void test_processor(void)
{
    pynq_init();
    display_t display;
    display_init(&display);

    displayFillScreen(&display, RGB_WHITE);

    DataBuffer *db_in = databuffer_create(10, 2);
    DataBuffer *db_out = databuffer_create(10, 2);

    uint8_t full_array[2] = {5,5};
    for(int i = 0; i<10; i++) {
        databuffer_push(full_array, db_in);
    }

    pthread_mutex_t mutex_in;
    pthread_mutex_init(&mutex_in, NULL);

    pthread_mutex_t mutex_out;
    pthread_mutex_init(&mutex_out, NULL);

    FontxFile fx16G[2];
    InitFontx(fx16G, "/home/student/ryb/fonts/ILGH16XB.FNT", "");

    stylistics *styling = malloc(sizeof(stylistics));
    styling->line_width = 5;
    styling->x_inner_offset = 50;
    styling->x_outer_offset = 20;
    styling->y_inner_offset = 50;
    styling->y_outer_offset = 20;
    styling->fx = fx16G;

    data_process_args *p_args = malloc(sizeof(data_process_args));
    p_args->db_in = db_in;
    p_args->db_out = db_out;
    p_args->display = &display;
    p_args->msec_sleep = 500;
    p_args->mutex_in_buffer = &mutex_in;
    p_args->mutex_out_buffer = &mutex_out;
    p_args->styling = styling;

    pthread_t processor_thread;
    pthread_create(&processor_thread, NULL, call_data_process_fromargs, p_args);

    sleep_msec(7000);

    // pthread_join(processor_thread, NULL);

    display_destroy(&display);
    pynq_destroy();

    free(styling);
    free(p_args);
}