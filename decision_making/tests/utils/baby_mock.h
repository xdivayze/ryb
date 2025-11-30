#ifndef BABY_MOCK_H

#define BABY_MOCK_H

#define BABY_REACTION_DELAY_MSEC 100

void start_baby_loop(DataBuffer *db_in, pthread_mutex_t *mutex_in, DataBuffer *db_out, pthread_mutex_t *mutex_out);

int heartbeat_from_stress(int stress);
int crying_from_stress(int stress);

#endif