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
    
    data_process(NULL, NULL, NULL, NULL, 50, NULL, ADC_CHANNEL);
    
    pynq_destroy();

    return 0;
}
