#ifndef DATA_WRITER_H
#define DATA_WRITER_H

#include <stdint.h>
#include <stdlib.h>
#include <data_buffer.h>
#include <pthread.h>
#include <libpynq.h>

#define DATA_SIZE_BYTES 1

typedef struct
{
    iic_index_t iic;
    uint8_t addr;
    uint8_t register_offset;
} submodule_defs;

typedef struct
{
    DataBuffer *db;
    size_t sleep_dur;
    submodule_defs *sub_def;
    pthread_mutex_t *mutex;
} writer_args;

void stop_writer(void);

void *writer_fromargs(void *args);

int writer(DataBuffer *db, size_t sleep_dur, submodule_defs *sub_def, pthread_mutex_t *mutex);

#endif