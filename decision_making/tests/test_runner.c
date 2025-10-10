#include <unity.h>

void test_push_buffer_multiple(void);
void test_push_buffer_single(void);
void test_create_buffer(void);
void test_read_data(void);

void setUp(void)
{
}

void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_create_buffer);
    RUN_TEST(test_push_buffer_single);
    RUN_TEST(test_push_buffer_multiple);

    RUN_TEST(test_read_data);

    return UNITY_END();
}