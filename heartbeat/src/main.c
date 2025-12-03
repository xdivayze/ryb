#include <libpynq.h>
#include <pthread.h>
#include <data_buffer.h>
#include <data_writer.h>
#include <data_processor.h>

#define ADC_CHANNEL ADC2

int main()
{
    pynq_init();

    adc_init();
    
    while(1) {
        adc_read_channel_raw(ADC_CHANNEL);
    }
    
    pynq_destroy();

    return 0;
}
