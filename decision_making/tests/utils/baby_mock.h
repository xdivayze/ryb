#ifndef BABY_MOCK_H

#define BABY_MOCK_H

#define BABY_REACTION_DELAY_MSEC 100

#include "pthread.h"
#include "data_buffer.h"

extern volatile int keep_baby_mock_running;

void start_baby_loop(DataBuffer *db_in, pthread_mutex_t *mutex_in, DataBuffer *db_out, pthread_mutex_t *mutex_out, int initial_stress);
//-1 on invalid range
int heartbeat_from_stress(int stress);

//-1 on invalid range
int crying_from_stress(int stress);

#endif