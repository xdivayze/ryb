#include <unity.h>

void test_processor(void);
void test_pulse_generator(void);
void test_processor_no_display(void);
void test_processor_with_display(void);

void setUp(void)
{
}

void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();

    // RUN_TEST(test_pulse_generator);
    RUN_TEST(test_processor_no_display);
    //RUN_TEST(test_processor_with_display);

    return UNITY_END();
}