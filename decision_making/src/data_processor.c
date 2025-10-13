#include <data_buffer.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <libpynq.h>
#include "data_processor.h"

#define HEARTBEAT_DISPLAY_TEXT "IN1"
#define CRYING_DISPLAY_TEXT "IN2"

#define OUT_FREQ_TEXT "OUT1"
#define OUT_AMPL_TEXT "OUT2"

static volatile int keep_running = 1;

void stop_processing(void)
{
    keep_running = 0;
}

static void int_as_string(uint8_t num, char *buf)
{
    sprintf(buf, "%d", num);
}

void *call_data_process_fromargs(void *args)
{
    data_process_args *dp_args = (data_process_args *)args;

    return (void *)data_process(dp_args->mutex_in_buffer, dp_args->mutex_out_buffer, dp_args->db_in, dp_args->db_out, dp_args->msec_sleep, dp_args->display, dp_args->styling);
}

int display_draw_default(display_t *display, stylistics *styling)
{
    displayDrawLine(display, DISPLAY_WIDTH / 2, styling->y_outer_offset, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - (styling->y_outer_offset), RGB_BLACK);
    displayDrawLine(display, styling->x_outer_offset, DISPLAY_HEIGHT / 2, DISPLAY_WIDTH - (styling->x_outer_offset), DISPLAY_HEIGHT / 2, RGB_BLACK);

    uint16_t canvas_width = DISPLAY_WIDTH - (styling->x_outer_offset * 2);

    displayDrawString(display, styling->fx, styling->x_outer_offset, styling->y_outer_offset, HEARTBEAT_DISPLAY_TEXT, RGB_BLACK);
    displayDrawString(display, styling->fx, styling->x_inner_offset + DISPLAY_WIDTH / 2, styling->y_outer_offset,
                      CRYING_DISPLAY_TEXT, RGB_BLACK);

    displayDrawString(display, styling->fx, styling->x_outer_offset, DISPLAY_HEIGHT / 2 + styling->y_outer_offset,
                      OUT_AMPL_TEXT, RGB_BLACK);
    displayDrawString(display, styling->fx, styling->x_inner_offset + DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + styling->y_outer_offset,
                      OUT_FREQ_TEXT, RGB_BLACK);

    return 0;
}

int display_string_on_display(display_t *display, char *val1, char *val2, char *val3, char *val4,
                              stylistics *styling)
{
    displayDrawString(display, styling->fx, styling->x_outer_offset + styling->x_inner_offset, styling->y_inner_offset + styling->y_outer_offset, val1, RGB_BLACK);
    displayDrawString(display, styling->fx, styling->x_outer_offset + styling->x_inner_offset + DISPLAY_WIDTH / 2, styling->y_inner_offset + styling->y_outer_offset, val2, RGB_BLACK);
    displayDrawString(display, styling->fx, styling->x_outer_offset + styling->x_inner_offset + DISPLAY_WIDTH / 2, styling->y_inner_offset + styling->y_outer_offset + DISPLAY_HEIGHT / 2, val3, RGB_BLACK);
    displayDrawString(display, styling->fx, styling->x_outer_offset + styling->x_inner_offset, styling->y_inner_offset + styling->y_outer_offset + DISPLAY_HEIGHT / 2, val4, RGB_BLACK);

    return 0;
}

int data_process(pthread_mutex_t *mutex_in_buffer, pthread_mutex_t *mutex_out_buffer, DataBuffer *db_in, DataBuffer *db_out,
                 size_t msec_sleep, display_t *display, stylistics *styling)
{
    uint8_t *val = malloc(sizeof(uint8_t) * 2);

    displaySetFontDirection(display, TEXT_DIRECTION0);

    while (keep_running)
    {
        pthread_mutex_lock(mutex_in_buffer);
        databuffer_pop(db_in, val);
        pthread_mutex_unlock(mutex_in_buffer);

        uint8_t vals_out[2] = {val[0] >> 1, val[1] >> 1};

        displayFillScreen(display, RGB_WHITE);
        display_draw_default(display, styling);

        char s0[4];
        char s1[4];
        char s2[4];
        char s3[4];
        int_as_string(val[0], s0);
        int_as_string(val[1], s1);
        int_as_string(vals_out[1], s2);
        int_as_string(vals_out[0], s3);
        display_string_on_display(display, s0, s1,
                                  s2, s3, styling);

        pthread_mutex_lock(mutex_out_buffer);
        databuffer_push(vals_out, db_out);
        pthread_mutex_unlock(mutex_out_buffer);

        sleep_msec(msec_sleep);
    }
    displayFillScreen(display, RGB_WHITE);
    fprintf(stdout, "Closed display\n");
    free(val);
    return 0;
}
