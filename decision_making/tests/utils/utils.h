#ifndef UTILS_H
#define UTILS_H

#include <libpynq.h>
#include <stdint.h>

typedef struct {
    iic_index_t IIC_INDEX;
    uint8_t IIC_ADDR;
    uint32_t data_to_send;
    uint8_t register_to_write;
}writer_args; 

void stop_i2c_writer(void);

void* i2c_writer(void * args) ;

#endif