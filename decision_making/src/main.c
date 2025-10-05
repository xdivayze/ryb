#include <libpynq.h>
#include <pthread.h>
#include <data_buffer.h>

#define GLOBAL_DATA_BUFFER_CAPACITY 10
#define GLOBAL_DATA_BUFFER_ARRAY_LENGTH 2

#define IIC_SCL_PIN IO_PMODA3
#define IIC_SDA_PIN IO_PMODA4

#define HEARTBEAT_IIC_ADDRESS 0x60
#define HEARTBEAT_IIC_DATA_REGISTER 1

#define CRYING_IIC_ADDRESS 0x47
#define CRYING_IIC_DATA_REGISTER 1


int main() {
    pynq_init();
    switchbox_init();

    switchbox_set_pin(IIC_SCL_PIN, SWB_IIC0_SCL);
    switchbox_set_pin(IIC_SDA_PIN, SWB_IIC0_SDA);

    DataBuffer* databuffer = databuffer_create(GLOBAL_DATA_BUFFER_CAPACITY, GLOBAL_DATA_BUFFER_ARRAY_LENGTH);
    
    pthread_mutex_t *buffer_mutex;
    pthread_mutex_init(buffer_mutex, NULL);

    //TODO lock buffer while reading as well to avoid flushing
    






    iic_destroy(IIC0);
    switchbox_destroy();
    pynq_destroy();

    return 0;
}

