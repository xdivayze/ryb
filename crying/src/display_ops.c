#include "display_ops.h"
#include <time.h>
#include <pthread.h>
#include "data_processor.h"
#include <math.h>
#define GRAPH_OFFSET_FROM_MIDDLE 100

static double get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (double)ts.tv_sec * 1000.0 +
           (double)ts.tv_nsec / 1e6;
}

static volatile int keep_display_loop_running = 1;

void *call_display_loop_fromargs(void *void_args)
{
    display_loop_args *args = void_args;
    start_display_loop(args->db_in, args->graph_timeframe_msec, args->cv, args->buffer_mutex);

    return NULL;
}

int start_display_loop(float *db_in, size_t graph_timeframe_msec, pthread_cond_t *cv, pthread_mutex_t *buffer_mutex)
{

    if (graph_timeframe_msec % DISPLAY_WIDTH)
    {
        fprintf(stderr, "provided graph timeframe is not divisible by display width\n");
        // return -1;
    }
    size_t sample_per_pixel = ceilf((float) graph_timeframe_msec * heartbeat_sampling_frequency / (1000.0f * DISPLAY_WIDTH));

    printf("sample per pixel: %i\n", sample_per_pixel);

    size_t current_sample_n = 0;

    float data = 0;

    int current_row = DISPLAY_HEIGHT / 2;
    int last_row = current_row;

    int current_col = 0;

    while (keep_display_loop_running)
    {
        pthread_mutex_lock(buffer_mutex);

        while (current_sample_n % sample_per_pixel)
        {
            pthread_cond_wait(cv, buffer_mutex);
            current_sample_n++;
        }

        data = *db_in;

        pthread_mutex_unlock(buffer_mutex);

        current_col = (current_sample_n / sample_per_pixel) % (DISPLAY_WIDTH - 1);

        last_row = current_row;
        current_row = (DISPLAY_HEIGHT / 2) - ((GRAPH_OFFSET_FROM_MIDDLE) * (data / adc_reference_voltage));

        displayDrawFillRect(&display, current_col, (DISPLAY_HEIGHT / 2) - GRAPH_OFFSET_FROM_MIDDLE, ((current_col + 10) < DISPLAY_WIDTH ? (current_col + 10) : (DISPLAY_WIDTH - 1)), (DISPLAY_HEIGHT / 2) + GRAPH_OFFSET_FROM_MIDDLE, RGB_WHITE);
        displayDrawLine(&display, current_col, last_row, current_col + 1, current_row, RGB_BLACK);

        current_sample_n++;
    }
}
