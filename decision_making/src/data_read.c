#include "data_read.h"
#include <stdlib.h>
#include <libpynq.h>
#include <stdio.h>

static volatile int keep_running = 1;

#define MSEC_SLEEP_ON_TIMEOUT 250

void stop_i2c_reader(void)
{
    keep_running = 0;
}

void *call_read_from_iic_to_databuffer_fromargs(void *args)
{
    read_from_iic_to_databuffer_args *reader_args = (read_from_iic_to_databuffer_args *)args;
    return (void *)read_from_iic_to_databuffer(reader_args->iic_map, reader_args->msec_sleep_duration, reader_args->db, reader_args->mutex, reader_args->iic);
}

submodule_iic_map *create_submodule_iic_map(io_t iic_data_pin, size_t read_register, size_t buffer_array_position, size_t data_size, size_t addr)
{

    if (read_register > PYNQ_MAX_REGISTER)
    {
        fprintf(stderr, "register exceeding the max register available: %d\n", PYNQ_MAX_REGISTER);
        return NULL;
    }

    submodule_iic_map *map = malloc(sizeof(submodule_iic_map));
    if (!map)
        return NULL;

    map->buffer_array_position = buffer_array_position;
    map->data_size = data_size;
    map->iic_data_pin = iic_data_pin;
    map->read_register = read_register;
    map->addr = addr;

    return map;
}

int read_from_iic_to_databuffer(submodule_iic_map **iic_map, size_t msec_sleep_duration, DataBuffer *db, pthread_mutex_t *mutex, iic_index_t iic)
{
    for (int i = 0; i < db->array_len; i++) // ACK loop
    {
        submodule_iic_map *iic_map_curr = iic_map[i];

        uint8_t d = 0;
        sleep_msec(250);
        if (iic_write_register(iic, iic_map_curr->addr, 0, &d, 1) == 0)
        {
            fprintf(stdout, "ACK Successful on addr: %d\n", iic_map_curr->addr);
        }
        else
        {
            fprintf(stderr, "ACK Failed on addr: %d\n", iic_map_curr->addr);
            return -4;
        }
    }

    uint8_t *val = malloc(db->array_len); // array which will be pushed to the buffer
    uint8_t data_ready = 0;
    while (keep_running)
    {

        int read_status = 0;

        for (size_t i = 0; i < db->array_len; i++)
        { // single thread read data loop through each submodule

            submodule_iic_map *iic_map_curr = iic_map[i];

            if (db->array_len < iic_map_curr->buffer_array_position) // check if OOB array position
            {
                free(val);
                return -4;
            }

            val[i] = 0xFF; // initialize OOB for stress calculation

            

            if (iic_read_register(iic, iic_map_curr->addr, iic_map_curr->read_register + 1, &data_ready, 1)) // data_ready check
            {
                read_status = -1;
                continue;
            }
            else
                read_status = 0;
            if (!data_ready)
                continue;

            uint32_t data = 0;
            size_t data_size = sizeof(data);
            if (iic_read_register(iic, iic_map_curr->addr, iic_map_curr->read_register, (uint8_t *)&data, data_size))
            {

                read_status = -1;
            }
            else
            {
                read_status = 0;
                // fprintf(stdout, "read: 0x%08X\n", data);
                val[iic_map_curr->buffer_array_position] = (uint8_t)(data & 0xFF); // only if data was able to be read and ready
                data_ready = 0;
                iic_write_register(iic, iic_map_curr->addr, iic_map_curr->read_register+1, &data_ready, 1);
            }
        }

        if (read_status != 0)
        {
            sleep_msec(MSEC_SLEEP_ON_TIMEOUT); // wait for i2c bus reset
            continue;
        }

        int status = pthread_mutex_lock(mutex);
        if (status)
        {
            free(val);
            fprintf(stderr, "error occured while locking mutex status code: %d\n", status);
            return -2;
        }

        databuffer_push(val, db);

        status = pthread_mutex_unlock(mutex);
        if (status)
        {
            free(val);
            fprintf(stderr, "error occured while unlocking mutex status code: %d\n", status);
            return -2;
        }

        sleep_msec(msec_sleep_duration);
    }
    free(val);
}