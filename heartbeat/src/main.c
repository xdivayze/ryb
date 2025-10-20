#include <libpynq.h>
#include <pthread.h>
#include <data_buffer.h>
#include <data_writer.h>
#include <data_processor.h>

#define GLOBAL_DATA_BUFFER_IN_CAPACITY 10
#define GLOBAL_DATA_BUFFER_IN_ARRAY_LENGTH 1

#define GLOBAL_DATA_BUFFER_OUT_CAPACITY 2
#define GLOBAL_DATA_BUFFER_OUT_ARRAY_LENGTH 1

#define IIC_SCL_PIN IO_AR0
#define IIC_SDA_PIN IO_AR1
#define IIC_INDEX IIC0

#define HEARTBEAT_IIC_ADDRESS 0x60
#define HEARTBEAT_IIC_DATA_REGISTER 0
#define HEARTBEAT_BUFFER_POS 0
#define BYTES_HEARTBEAT_DATA_SIZE 1

#define MSEC_IN_READ_SLEEP_DURATION 200

#define PROCESSOR_SLEEP_DURATION 200

#define FONT_PATH "/home/student/ryb/fonts/ILGH16XB.FNT"

int main()
{
    pynq_init();
    switchbox_init();

    DataBuffer *databuffer_in = databuffer_create(GLOBAL_DATA_BUFFER_IN_CAPACITY, GLOBAL_DATA_BUFFER_IN_ARRAY_LENGTH);
    pthread_mutex_t buffer_in_mutex;
    pthread_mutex_init(&buffer_in_mutex, NULL);

    DataBuffer *databuffer_out = databuffer_create(GLOBAL_DATA_BUFFER_OUT_CAPACITY, GLOBAL_DATA_BUFFER_OUT_ARRAY_LENGTH);
    pthread_mutex_t buffer_out_mutex;
    pthread_mutex_init(&buffer_out_mutex, NULL);

    // INITIALIZE INPUT BUFFER, FOR PROCESS WHERE THE INPUT BUFFER IS FILLED

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
    processor_args->db_out = databuffer_out;
    processor_args->mutex_in_buffer = &buffer_in_mutex;
    processor_args->mutex_out_buffer = &buffer_out_mutex;
    processor_args->msec_sleep = PROCESSOR_SLEEP_DURATION;
    processor_args->styling = styling;
    processor_args->display = &display;

    pthread_t processor_thread;
    pthread_create(&processor_thread, NULL, call_data_process_fromargs, processor_args);

    // INITIALIZE OUTPUT BUFFER

    submodule_defs *heartbeat_def = malloc(sizeof(submodule_defs));
    heartbeat_def->addr = HEARTBEAT_IIC_ADDRESS;
    heartbeat_def->iic = IIC_INDEX;
    heartbeat_def->register_offset = HEARTBEAT_IIC_DATA_REGISTER;

    writer_args *w_args = malloc(sizeof(writer_args));
    w_args->db = databuffer_out;
    w_args->mutex = &buffer_out_mutex;
    w_args->sleep_dur = MSEC_IN_READ_SLEEP_DURATION;
    w_args->sub_def = heartbeat_def;

    switchbox_set_pin(IIC_SCL_PIN, SWB_IIC0_SCL);
    switchbox_set_pin(IIC_SDA_PIN, SWB_IIC0_SDA);

    iic_init(IIC_INDEX);

    pthread_t writer_thread;
    pthread_create(&writer_thread, NULL, writer_fromargs, w_args);

    pthread_join(writer_thread, NULL);
    pthread_join(processor_thread, NULL);

    pynq_destroy();

    free(heartbeat_def);
    free(w_args);
    free(processor_args);

    return 0;
}
