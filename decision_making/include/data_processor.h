#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <data_buffer.h>
#include <pthread.h>
#include <stdint.h>

#define SINGLE_SIDE_BUFFER_CAPACITY 10

typedef struct
{
    uint16_t y_inner_offset;
    uint16_t x_inner_offset;
    uint16_t x_outer_offset;
    uint16_t y_outer_offset;
    uint16_t line_width;
    FontxFile *fx;
} stylistics;

typedef struct
{
    pthread_mutex_t *mutex_in_buffer;
    pthread_mutex_t *mutex_out_buffer;
    DataBuffer *db_in;
    DataBuffer *db_out;
    size_t msec_sleep;
    display_t *display;
    stylistics *styling;
} data_process_args;

void *call_data_process_fromargs(void *args);

int display_string_on_display(display_t *display, char *val1, char *val2, char *val3, char *val4,
                              stylistics *styling);
int display_draw_default(display_t *display, stylistics *styling);

int data_process(pthread_mutex_t *mutex_in_buffer, pthread_mutex_t *mutex_out_buffer, DataBuffer *db_in, DataBuffer *db_out, size_t msec_sleep, display_t *display, stylistics *styling);

#endif