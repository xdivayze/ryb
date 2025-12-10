#include <libpynq.h>
#include <pthread.h>
#include <data_processor.h>

#define ADC_CHANNEL ADC2

display_t display;

int main()
{
    pynq_init();

    adc_init();
    
    
    pynq_destroy();

    return 0;
}
