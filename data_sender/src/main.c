#include <libpynq.h>
#include <stdint.h>

#define IIC_SCL_PIN IO_PMODA1
#define IIC_SDA_PIN IO_PMODA2
#define IIC_INDEX IIC0

#define HEARTBEAT_IIC_ADDRESS 0x60
#define HEARTBEAT_IIC_DATA_REGISTER 0
#define HEARTBEAT_BUFFER_POS 0
#define BYTES_HEARTBEAT_DATA_SIZE 1

static volatile int keep_running = 1;

int main()
{
    pynq_init();
    switchbox_init();
    switchbox_set_pin(IIC_SCL_PIN, SWB_IIC0_SCL);
    switchbox_set_pin(IIC_SDA_PIN, SWB_IIC0_SDA);
    iic_init(IIC_INDEX);

    iic_reset(IIC_INDEX);

    uint32_t volatile regs[32] = {0};
    iic_init(IIC_INDEX);
    iic_reset(IIC_INDEX);

    iic_set_slave_mode(IIC_INDEX, HEARTBEAT_IIC_ADDRESS, (uint32_t*) regs, sizeof(regs)/sizeof(regs[0]));
    while (keep_running) {
        iic_slave_mode_handler(IIC_INDEX);
        regs[0] = 0x80; //only up to 4 bytes for testing purposes
        sleep_msec(10);
    }
}