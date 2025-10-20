#include "data_writer.h"
#include <libpynq.h>
#include <stdint.h>
#include <stdlib.h>

static volatile int keep_writer_running = 1;

void stop_writer(void)
{
    keep_writer_running = 0;
}

void *writer_fromargs(void *args)
{
    writer_args *casted_args = (writer_args *)args;
    return (void *)writer(casted_args->db, casted_args->sleep_dur, casted_args->sub_def, casted_args->mutex);
}
int writer(DataBuffer *db, size_t sleep_dur, submodule_defs *sub_def, pthread_mutex_t *mutex)
{

    uint32_t volatile regs[32] = {0};
    iic_reset(sub_def->iic);

    iic_set_slave_mode(sub_def->iic, sub_def->addr, regs, sizeof(regs) / sizeof(regs[0]));

    uint8_t *vals = malloc(sizeof(uint8_t) * db->array_len);

    while (keep_writer_running)
    {
        if (db->count == 0)
        {
            sleep_msec(sleep_dur);
            continue;
        }

        pthread_mutex_lock(mutex);
        databuffer_pop(db, vals); // wont return empty buffer error since empty buffers iterations are skipped
        pthread_mutex_unlock(mutex);

        for (int i = 0; i < db->array_len; i++)
        {
            regs[i + sub_def->register_offset] = vals[i];
        }

        iic_slave_mode_handler(sub_def->iic);
        sleep_msec(10);
    }
}