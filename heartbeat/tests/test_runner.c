#include <unity.h>

void test_processor(void);
void test_pulse_generator(void);


void setUp(void)
{
}

void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_pulse_generator);

    return UNITY_END();
}