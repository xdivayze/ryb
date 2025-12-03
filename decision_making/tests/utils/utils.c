#include "utils.h"
#include <libpynq.h>

static volatile int keep_running = 1;

display_t display;

static int display_initialized = 0;

void initialize_display()
{
    if (!display_initialized)
    {
        display_init(&display);
        display_initialized = 1;
    }
}

const char *FONT_PATH = "/home/student/ryb/fonts/ILGH16XB.FNT";

void stop_i2c_writer(void)
{
    keep_running = 0;
}

static volatile int keep_uart_reader_running = 1;

void stop_uart_reader(void)
{
    keep_uart_reader_running = 0;
}

void *i2c_writer(void *args)
{

    uint32_t volatile regs[32] = {0};

    writer_args *casted_args = (writer_args *)args;
    iic_init(casted_args->iic_index);
    iic_reset(casted_args->iic_index);

    iic_set_slave_mode(casted_args->iic_index, casted_args->iic_addr, (uint32_t *)regs, sizeof(regs) / sizeof(regs[0]));
    while (keep_running)
    {
        iic_slave_mode_handler(casted_args->iic_index);
        regs[0] = casted_args->data_to_send; // only up to 4 bytes for testing purposes
        sleep_msec(10);
    }
}

void *uart_reader(void *args)
{
    reader_args *casted_args = (reader_args *)args;

    uart_init(casted_args->uart_index);
    uart_reset_fifos(casted_args->uart_index);

    uint8_t *vals = malloc(sizeof(uint8_t) * casted_args->db_reader->array_len);
    while (keep_uart_reader_running)
    {
        if (casted_args->db_reader->count == casted_args->db_reader->buffer_capacity)
        {
            stop_uart_reader();
        }

        for (int i = 0; i < casted_args->db_reader->array_len; i++)
        {
            vals[i] = uart_recv(casted_args->uart_index);
        }
    }

    free(vals);
}