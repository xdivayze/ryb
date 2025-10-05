#ifndef DATA_READ_H
#define DATA_READ_H

#include <data_buffer.h>
#include <pinmap.h>
#include <pthread.h>
#include <libpynq.h>

#define PYNQ_MAX_REGISTER 32

typedef struct
{
    io_t iic_data_pin;
    size_t read_register;         // register to read data from
    size_t buffer_array_position; // index where the data will be inserted in the buffer arrays
    size_t data_size;             // size of each data that will be read
    size_t addr;

} submodule_iic_map;

submodule_iic_map *create_submodule_iic_map(io_t iic_data_pin, size_t read_register, size_t buffer_array_position, size_t data_size, size_t addr);

// constantly read from iic and write to respective buffer positions
//sending iic_map array longer than buffer's array will result in a sigsegv
//initializes a while loop
//return 0 on success
//-1 on error while reading register
//-2 on mutex operation fail
//-3 on buffer fail
int read_from_iic_to_databuffer(submodule_iic_map **iic_map, size_t msec_sleep_duration, DataBuffer *db, pthread_mutex_t *mutex, iic_index_t iic);

#endif