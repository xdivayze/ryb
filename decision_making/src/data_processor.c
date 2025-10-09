#include <data_buffer.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <libpynq.h>
#include <data_processor.h>

void* call_data_process_fromargs(void *args)
{
    data_process_args *dp_args = (data_process_args*) args;

    return (void *) data_process(dp_args->mutex_in_buffer, dp_args->mutex_out_buffer, dp_args->db_in, dp_args->db_out, dp_args->msec_sleep);
}

int data_process(pthread_mutex_t *mutex_in_buffer, pthread_mutex_t *mutex_out_buffer, DataBuffer *db_in, DataBuffer *db_out, size_t msec_sleep)
{
    uint8_t *val = malloc(sizeof(uint8_t) * 2);

    while (1)
    {
        pthread_mutex_lock(mutex_in_buffer);
        databuffer_pop(db_in, val);
        pthread_mutex_unlock(mutex_in_buffer);

        pthread_mutex_lock(mutex_out_buffer);
        databuffer_push(val, db_out);
        pthread_mutex_unlock(mutex_out_buffer);

        sleep_msec(msec_sleep);
    }

    free(val);
    return 0;
}
