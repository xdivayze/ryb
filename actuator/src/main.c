#include <libpynq.h>
#include <pthread.h>
#include <data_buffer.h>
#include <data_processor.h>
#include <data_reader.h>

#define GLOBAL_DATA_BUFFER_IN_CAPACITY 10
#define GLOBAL_DATA_BUFFER_IN_ARRAY_LENGTH 2

#define GLOBAL_DATA_BUFFER_OUT_CAPACITY 2
#define GLOBAL_DATA_BUFFER_OUT_ARRAY_LENGTH 1

#define UART_INDEX UART0
#define UART_TX IO_AR3
#define UART_RX IO_AR4

#define PROCESSOR_SLEEP_DURATION 200

#define FONT_PATH "/home/student/ryb/fonts/ILGH16XB.FNT"

int main()
{
    pynq_init();
    switchbox_init();

    switchbox_set_pin(UART_TX, SWB_UART0_TX);
    switchbox_set_pin(UART_RX, SWB_UART0_RX);

    uart_init(UART_INDEX);

    DataBuffer *databuffer_in = databuffer_create(GLOBAL_DATA_BUFFER_IN_CAPACITY, GLOBAL_DATA_BUFFER_IN_ARRAY_LENGTH);
    pthread_mutex_t buffer_in_mutex;
    pthread_mutex_init(&buffer_in_mutex, NULL);

    DataBuffer *databuffer_out = databuffer_create(GLOBAL_DATA_BUFFER_OUT_CAPACITY, GLOBAL_DATA_BUFFER_OUT_ARRAY_LENGTH);
    pthread_mutex_t buffer_out_mutex;
    pthread_mutex_init(&buffer_out_mutex, NULL);

    reader_args* r_args = malloc(sizeof(reader_args));
    r_args->db = databuffer_in;
    r_args->mutex = &buffer_in_mutex;
    r_args->uart_index = UART_INDEX;
    

    pthread_t reader;
    pthread_create(&reader, NULL, call_reader_fromargs, r_args);

    // INITIALIZE DATA PROCESSING THREAD

    display_t display;
    display_init(&display);

    displayFillScreen(&display, RGB_WHITE);

    FontxFile fx16G[2];
    InitFontx(fx16G, FONT_PATH, "");

    stylistics *styling = malloc(sizeof(stylistics));
    styling->line_width = 5;
    styling->x_inner_offset = 50;
    styling->x_outer_offset = 20;
    styling->y_inner_offset = 50;
    styling->y_outer_offset = 20;
    styling->fx = fx16G;

    data_process_args *processor_args = malloc(sizeof(data_process_args));
    processor_args->db_in = databuffer_in;
    processor_args->mutex_in_buffer = &buffer_in_mutex;
    processor_args->msec_sleep = PROCESSOR_SLEEP_DURATION;
    processor_args->styling = styling;
    processor_args->display = &display;
    processor_args->db_out = databuffer_out;
    processor_args->mutex_out_buffer = &buffer_out_mutex;

    pthread_t processor;
    pthread_create(&processor, NULL, call_data_process_fromargs, processor_args);

    //PWM WRITER INITIALIZATION 

    pthread_join(reader, NULL);
    pthread_join(processor, NULL);

    free(r_args);
    free(processor_args);

    pynq_destroy();

    return 0;
}
