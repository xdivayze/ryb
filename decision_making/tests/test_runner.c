#include <unity.h>

void test_push_buffer_multiple(void);
void test_push_buffer_single(void);
void test_create_buffer(void);
void test_read_data(void);
void test_processor(void);
void test_write(void);
void test_stress_calculation(void);
void test_tile_detection(void);
void test_tile_detection_with_display(void);
void test_heartbeat_from_stress(void);
void test_crying_from_stress(void);
void test_baby_mock_check_bare_values(void);

void setUp(void)
{
}

void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();

    //pynq independent linux tests
    // RUN_TEST(test_create_buffer);
    // RUN_TEST(test_push_buffer_single);
    // RUN_TEST(test_push_buffer_multiple);
    // RUN_TEST(test_stress_calculation);
    // RUN_TEST(test_heartbeat_from_stress);
    // RUN_TEST(test_crying_from_stress);
    // RUN_TEST(test_baby_mock_check_bare_values);
    RUN_TEST(test_tile_detection);

    //pynq dependent tests

    // RUN_TEST(test_read_data);

    // RUN_TEST(test_processor);

    
    // RUN_TEST(test_tile_detection_with_display);

    return UNITY_END();
}