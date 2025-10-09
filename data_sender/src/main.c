#include <libpynq.h>
#include <stdint.h>

#define IIC_SCL_PIN IO_PMODA3
#define IIC_SDA_PIN IO_PMODA4
#define IIC_INDEX IIC0

#define HEARTBEAT_IIC_ADDRESS 0x60
#define HEARTBEAT_IIC_DATA_REGISTER 1
#define HEARTBEAT_BUFFER_POS 0
#define BYTES_HEARTBEAT_DATA_SIZE 1

int main()
{
    pynq_init();
    switchbox_init();
    switchbox_set_pin(IIC_SCL_PIN, SWB_IIC0_SCL);
    switchbox_set_pin(IIC_SDA_PIN, SWB_IIC0_SDA);
    iic_init(IIC_INDEX);

    iic_reset(IIC_INDEX);
    uint32_t my_register_map;

    iic_set_slave_mode(IIC0, HEARTBEAT_IIC_ADDRESS, &my_register_map, BYTES_HEARTBEAT_DATA_SIZE);
    
    while (1)
    {
        ((uint8_t *)&my_register_map)[0] = 0x60;
        iic_slave_mode_handler(IIC_INDEX);
        sleep_msec(10);
    }

    pynq_destroy();
    return 0;
}