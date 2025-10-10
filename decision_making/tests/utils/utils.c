#include "utils.h"
#include <libpynq.h>

static volatile int keep_running = 1;

void stop_i2c_writer(void) {
    keep_running = 0;
}

void* i2c_writer(void * args) {
    
    uint32_t volatile regs[32] = {0};

    writer_args* casted_args = (writer_args*) args;
    iic_init(casted_args->IIC_INDEX);
    iic_reset(casted_args->IIC_INDEX);

    iic_set_slave_mode(casted_args->IIC_INDEX, casted_args->IIC_ADDR, (uint32_t*) regs, sizeof(regs)/sizeof(regs[0]));
    while (keep_running) {
        iic_slave_mode_handler(casted_args->IIC_INDEX);
        regs[0] = casted_args->data_to_send; //only up to 4 bytes for testing purposes
        sleep_msec(10);
    }
}