#include <libpynq.h>
#include <pthread.h>
#include <data_buffer.h>
#include <data_read.h>
#include <data_writer.h>
#include <data_processor.h>

#define GLOBAL_DATA_BUFFER_IN_CAPACITY 10
#define GLOBAL_DATA_BUFFER_IN_ARRAY_LENGTH 2

#define GLOBAL_DATA_BUFFER_OUT_CAPACITY 1
#define GLOBAL_DATA_BUFFER_OUT_ARRAY_LENGTH 2

#define IIC_SCL_PIN IO_AR0
#define IIC_SDA_PIN IO_AR1
#define IIC_INDEX IIC0

#define HEARTBEAT_IIC_ADDRESS 0x60
#define HEARTBEAT_IIC_DATA_REGISTER 0
#define HEARTBEAT_BUFFER_POS 0
#define BYTES_HEARTBEAT_DATA_SIZE 1

#define CRYING_IIC_ADDRESS 0x47
#define CRYING_IIC_DATA_REGISTER 0
#define CRYING_BUFFER_POS 1
#define BYTES_CRYING_DATA_SIZE 1

#define UART_INDEX UART0
#define UART_TX IO_AR3
#define UART_RX IO_AR4

#define MSEC_IN_READ_SLEEP_DURATION 20

#define PROCESSOR_SLEEP_DURATION 200

#define FONT_PATH "/home/student/ryb/fonts/ILGH16XB.FNT"

int main()
{
    pynq_init();
    switchbox_init();

    switchbox_set_pin(IIC_SCL_PIN, SWB_IIC0_SCL);
    switchbox_set_pin(IIC_SDA_PIN, SWB_IIC0_SDA);

    iic_init(IIC_INDEX);
    iic_reset(IIC_INDEX);

    DataBuffer *databuffer_in = databuffer_create(GLOBAL_DATA_BUFFER_IN_CAPACITY, GLOBAL_DATA_BUFFER_IN_ARRAY_LENGTH);
    pthread_mutex_t buffer_in_mutex;
    pthread_mutex_init(&buffer_in_mutex, NULL);

    DataBuffer *databuffer_out = databuffer_create(GLOBAL_DATA_BUFFER_IN_CAPACITY, GLOBAL_DATA_BUFFER_OUT_ARRAY_LENGTH);
    pthread_mutex_t buffer_out_mutex;
    pthread_mutex_init(&buffer_out_mutex, NULL);

    // INITIALIZE INPUT BUFFER
    submodule_iic_map *reader_heartbeat = create_submodule_iic_map(IIC_SDA_PIN, HEARTBEAT_IIC_DATA_REGISTER,
                                                                   HEARTBEAT_BUFFER_POS, BYTES_HEARTBEAT_DATA_SIZE, HEARTBEAT_IIC_ADDRESS);
    submodule_iic_map *reader_crying = create_submodule_iic_map(IIC_SDA_PIN, CRYING_IIC_DATA_REGISTER,
                                                                CRYING_BUFFER_POS, BYTES_CRYING_DATA_SIZE, CRYING_IIC_ADDRESS);
    submodule_iic_map *submodule_iic_maps_in[GLOBAL_DATA_BUFFER_IN_ARRAY_LENGTH] = {reader_heartbeat, reader_crying};

    read_from_iic_to_databuffer_args *reader_in_args = malloc(sizeof(read_from_iic_to_databuffer_args));
    reader_in_args->db = databuffer_in;
    reader_in_args->iic = IIC_INDEX;
    reader_in_args->iic_map = submodule_iic_maps_in;
    reader_in_args->msec_sleep_duration = MSEC_IN_READ_SLEEP_DURATION;
    reader_in_args->mutex = &buffer_in_mutex;

    pthread_t reader;
    pthread_create(&reader, NULL, call_read_from_iic_to_databuffer_fromargs, reader_in_args);

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

    pthread_t processor;
    pthread_create(&processor, NULL, call_data_process_fromargs, processor_args);

    // INITIALIZE OUTPUT BUFFER
    switchbox_set_pin(UART_RX, SWB_UART0_RX);
    switchbox_set_pin(UART_TX, SWB_UART0_TX);

    uart_init(UART_INDEX);

    uart_writer_args *w_args = malloc(sizeof(uart_writer_args));

    pthread_t writer;
    pthread_create(&writer, NULL, uart_writer_fromargs, w_args);

    pthread_join(reader, NULL);
    pthread_join(writer, NULL);
    pthread_join(processor, NULL);

    free(reader_in_args);
    free(w_args);
    free(processor_args);

    pynq_destroy();

    return 0;
}
