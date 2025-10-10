#include <utils.h>
#include <libpynq.h>
#include <pthread.h>
#include <data_read.h>
#include <unity.h>

#define SLAVE_IIC_INDEX IIC0
#define SLAVE_IIC_SCL IO_PMODA1
#define SLAVE_IIC_SDA IO_PMODA2
#define SLAVE_IIC_ADDR 0x60

#define MASTER_IIC_INDEX IIC1
#define MASTER_IIC_SCL IO_PMODA3
#define MASTER_IIC_SDA IO_PMODA4

#define DATA_TO_SEND 0x76

//THESE TESTS MUST BE RUN ON A PYNQ BOARD WITH A FEEDBACK LOOP
void test_read_data(void) {
    pynq_init();
    switchbox_init();
    switchbox_set_pin(SLAVE_IIC_SCL, SWB_IIC0_SCL); //! hard coded
    switchbox_set_pin(SLAVE_IIC_SDA, SWB_IIC0_SDA);

    switchbox_set_pin(MASTER_IIC_SCL, SWB_IIC1_SCL);
    switchbox_set_pin(MASTER_IIC_SDA, SWB_IIC1_SDA);

    writer_args* w_args = malloc(sizeof(writer_args));
    w_args->data_to_send = DATA_TO_SEND;
    w_args->IIC_ADDR = SLAVE_IIC_ADDR;
    w_args->IIC_INDEX = SLAVE_IIC_INDEX;
    w_args->register_to_write = 0;

    pthread_t slave;
    pthread_create(&slave, NULL, i2c_writer, w_args);

    DataBuffer* db_in = databuffer_create(10, 1);
    pthread_mutex_t mutex_in;
    pthread_mutex_init(&mutex_in, NULL);

    submodule_iic_map* slave_map = create_submodule_iic_map(MASTER_IIC_SDA, 0, 0, 1, SLAVE_IIC_ADDR);

    submodule_iic_map* maps[1] = {slave_map};

    read_from_iic_to_databuffer_args* r_args = malloc(sizeof(read_from_iic_to_databuffer_args));
    r_args->db = db_in;
    r_args->iic = MASTER_IIC_INDEX;
    r_args->iic_map = maps;
    r_args->msec_sleep_duration = 20;
    r_args->mutex = &mutex_in;

    pthread_t master;
    pthread_create(&master, NULL, call_read_from_iic_to_databuffer_fromargs, r_args);

    sleep_msec(2000);

    stop_i2c_reader();
    stop_i2c_writer();

    pthread_join(slave, NULL);
    pthread_join(master, NULL);

    uint8_t* last_data;
    databuffer_pop(db_in, last_data); 

    
    TEST_ASSERT_EQUAL_UINT8(last_data[0], DATA_TO_SEND);

    free(slave_map);
    free(r_args);
    free(w_args);
    switchbox_destroy();
    pynq_destroy();


}