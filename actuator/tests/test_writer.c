#include <libpynq.h>
#include <data_writer.h>
#include <data_buffer.h>
#include <utils.h>
#include <unity.h>

#define MASTER_UART UART0
#define MASTER_UART_RX IO_AR3
#define MASTER_UART_TX IO_AR4

#define SLAVE_UART UART1
#define SLAVE_UART_RX IO_AR5
#define SLAVE_UART_TX IO_AR6

#define UART_WRITER_SLEEP_DURATION_MSEC 10

void test_write(void)
{
    pynq_init();

    switchbox_set_pin(MASTER_UART_RX, SWB_UART0_RX);
    switchbox_set_pin(MASTER_UART_TX, SWB_UART0_TX);

    switchbox_set_pin(SLAVE_UART_RX, SWB_UART1_RX);
    switchbox_set_pin(SLAVE_UART_TX, SWB_UART1_TX);

    uart_init(SLAVE_UART);
    uart_init(MASTER_UART);

    DataBuffer *db_writer = databuffer_create(10, 2);
    DataBuffer *db_reader = databuffer_create(10, 2);

    uint8_t fill[2] = {10, 10};
    for (int i = 0; i < db_writer->buffer_capacity; i++)
    {
        databuffer_push(fill, db_writer);
    }

    pthread_mutex_t mutex_reader;
    pthread_mutex_init(&mutex_reader, NULL);

    reader_args *r_args = malloc(sizeof(reader_args));

    r_args->db_reader = db_reader;
    r_args->mutex_reader = &mutex_reader;
    r_args->uart_index = SLAVE_UART;

    pthread_t reader;
    pthread_create(&reader, NULL, uart_reader, r_args);

    pthread_mutex_t mutex_writer;
    pthread_mutex_init(&mutex_writer, NULL);

    uart_writer_args *w_args = malloc(sizeof(uart_writer_args));
    w_args->buffer = db_writer;
    w_args->mutex = &mutex_writer;
    w_args->uart_index = MASTER_UART;
    w_args->sleep_duration_msec = UART_WRITER_SLEEP_DURATION_MSEC;

    pthread_t writer;
    pthread_create(&writer, NULL, uart_writer_fromargs, w_args);

    pthread_join(reader, NULL);

    uint8_t reader_db_vals[2] = {};
    databuffer_pop(db_reader, reader_db_vals);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(reader_db_vals, fill, 2);

    pynq_destroy();

    free(db_reader);
    free(db_writer);
}