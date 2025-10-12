#include <libpynq.h>
#include <data_buffer.h>
#include <pthread.h>
#include <data_processor.h>

void test_processor(void)
{
    pynq_init();
    display_t display;
    display_init(&display);

    DataBuffer *db_in = databuffer_create(10, 2);
    DataBuffer *db_out = databuffer_create(10, 2);

    uint8_t full_array[20];
    for(int i = 0; i<20; i++) {
        full_array[i] = 5;
    }
    
    db_in->data = full_array;

    pthread_mutex_t mutex_in;
    pthread_mutex_init(&mutex_in, NULL);

    pthread_mutex_t mutex_out;
    pthread_mutex_init(&mutex_out, NULL);

    FontxFile fx16G[2];
    InitFontx(fx16G, "../../fonts/ILGH16XB.FNT", "");

    stylistics *styling = malloc(sizeof(stylistics));
    styling->line_width = 5;
    styling->x_inner_offset = 10;
    styling->x_outer_offset = 10;
    styling->y_inner_offset = 10;
    styling->y_outer_offset = 10;
    styling->fx = fx16G;

    data_process_args *p_args = malloc(sizeof(data_process_args));
    p_args->db_in = db_in;
    p_args->db_out = db_out;
    p_args->display = &display;
    p_args->msec_sleep = 200;
    p_args->mutex_in_buffer = &mutex_in;
    p_args->mutex_out_buffer = &mutex_out;
    p_args->styling = styling;

    pthread_t processor_thread;
    pthread_create(&processor_thread, NULL, call_data_process_fromargs, p_args);

    pynq_destroy();

    free(styling);
    free(p_args);
}