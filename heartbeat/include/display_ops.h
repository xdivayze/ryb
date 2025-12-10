#ifndef DISPLAY_OPS_H

#define DISPLAY_OPS_H

#include <display.h>
#include <pthread.h>

extern display_t display;



int start_display_loop(float *db_in, size_t graph_timeframe_msec, pthread_cond_t *cv, pthread_mutex_t *buffer_mutex);

#endif