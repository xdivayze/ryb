#include <libpynq.h>
#include <pthread.h>
#include <data_buffer.h>
#include <data_read.h>
#include <data_writer.h>
#include <data_processor.h>

#define GLOBAL_DATA_BUFFER_CAPACITY 10
#define GLOBAL_DATA_BUFFER_IN_ARRAY_LENGTH 2

#define GLOBAL_DATA_BUFFER_OUT_ARRAY_LENGTH 2

#define IIC_SCL_PIN IO_PMODA3
#define IIC_SDA_PIN IO_PMODA4
#define IIC_INDEX IIC0

#define HEARTBEAT_IIC_ADDRESS 0x60
#define HEARTBEAT_IIC_DATA_REGISTER 1
#define HEARTBEAT_BUFFER_POS 0
#define BYTES_HEARTBEAT_DATA_SIZE 1

#define CRYING_IIC_ADDRESS 0x47
#define CRYING_IIC_DATA_REGISTER 1
#define CRYING_BUFFER_POS 1
#define BYTES_CRYING_DATA_SIZE 1

#define PWM_PIN IO_AR7
#define PWM_CHAN PWM0
#define PWM_PERIOD_CLOCK_TICKS 100000
#define PWM_SYNC_SIGNAL 0xFF

#define MSEC_IN_READ_SLEEP_DURATION 20

#define PROCESSOR_SLEEP_DURATION 20

#define MSEC_PWM_SIGNAL_DURATION 2

int main()
{
    pynq_init();
    switchbox_init();
    iic_init(IIC_INDEX);

    switchbox_set_pin(IIC_SCL_PIN, SWB_IIC0_SCL);
    switchbox_set_pin(IIC_SDA_PIN, SWB_IIC0_SDA);

    DataBuffer *databuffer_in = databuffer_create(GLOBAL_DATA_BUFFER_CAPACITY, GLOBAL_DATA_BUFFER_IN_ARRAY_LENGTH);
    pthread_mutex_t *buffer_in_mutex;
    pthread_mutex_init(buffer_in_mutex, NULL);

    DataBuffer *databuffer_out = databuffer_create(GLOBAL_DATA_BUFFER_CAPACITY, GLOBAL_DATA_BUFFER_OUT_ARRAY_LENGTH);
    pthread_mutex_t *buffer_out_mutex;
    pthread_mutex_init(buffer_out_mutex, NULL);

    // INITIALIZE INPUT BUFFER
    submodule_iic_map *reader_heartbeat = create_submodule_iic_map(IIC_SDA_PIN, HEARTBEAT_IIC_DATA_REGISTER,
                                                                   HEARTBEAT_BUFFER_POS, BYTES_HEARTBEAT_DATA_SIZE, HEARTBEAT_IIC_ADDRESS);
    submodule_iic_map *reader_crying = create_submodule_iic_map(IIC_SDA_PIN, CRYING_IIC_DATA_REGISTER,
                                                                CRYING_BUFFER_POS, BYTES_CRYING_DATA_SIZE, CRYING_IIC_ADDRESS);
    submodule_iic_map *submodule_iic_maps_in[GLOBAL_DATA_BUFFER_IN_ARRAY_LENGTH] = {reader_heartbeat, reader_crying};

    read_from_iic_to_databuffer_args *reader_in_args = malloc(sizeof(read_from_iic_to_databuffer_args));
    reader_in_args->db = databuffer_in;
    reader_in_args->iic = IIC_INDEX;
    reader_in_args->iic_map = submodule_iic_maps_in;
    reader_in_args->msec_sleep_duration = MSEC_IN_READ_SLEEP_DURATION;
    reader_in_args->mutex = buffer_in_mutex;

    pthread_t reader;
    pthread_create(&reader, NULL, call_read_from_iic_to_databuffer_fromargs, reader_in_args);

    // INITIALIZE DATA PROCESSING THREAD

    data_process_args *processor_args = malloc(sizeof(data_process_args));
    processor_args->db_in = databuffer_in;
    processor_args->db_out = databuffer_out;
    processor_args->mutex_in_buffer = buffer_in_mutex;
    processor_args->mutex_out_buffer = buffer_out_mutex;
    processor_args->msec_sleep = PROCESSOR_SLEEP_DURATION;

    pthread_t processor;
    pthread_create(&processor, NULL, call_data_process_fromargs, processor_args);

    // INITIALIZE OUTPUT BUFFER
    switchbox_set_pin(PWM_CHAN, PWM_PIN);

    pwm_init(PWM_CHAN, PWM_PERIOD_CLOCK_TICKS);
    pwm_set_steps(PWM_CHAN, -1);

    pwm_multiplex_writer_args *writer_out_args = malloc(sizeof(pwm_multiplex_writer_args));
    writer_out_args->buffer = databuffer_out;
    writer_out_args->msec_signal_duration = MSEC_IN_READ_SLEEP_DURATION;
    writer_out_args->mutex = buffer_out_mutex;
    writer_out_args->pwm_chan = PWM_CHAN;
    writer_out_args->sync_signal = PWM_SYNC_SIGNAL;
    writer_out_args->ticks_period = PWM_PERIOD_CLOCK_TICKS;

    pthread_t writer;
    pthread_create(&writer, NULL, pwm_multiplex_writer_fromargs, writer_out_args);

    pthread_join(reader, NULL);
    pthread_join(writer, NULL);
    pthread_join(processor, NULL);

    free(reader_in_args);
    free(writer_out_args);
    free(processor_args);

    iic_destroy(IIC_INDEX);
    switchbox_destroy();
    pynq_destroy();

    return 0;
}
