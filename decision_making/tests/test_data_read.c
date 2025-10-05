#include <unity/unity.h>
#include <data_read.h>

void setUp(void)
{
}

void tearDown(void) {}

static void test_create_buffer(void) {
    size_t buffer_size = 25;
    size_t array_len = 2;
    DataBuffer* db = databuffer_create(buffer_size, array_len);

    TEST_ASSERT_EQUAL_size_t(buffer_size, db->buffer_capacity );
    TEST_ASSERT_EQUAL_size_t(array_len, db->array_len);
    TEST_ASSERT_EQUAL_size_t(0, db->count);
    TEST_ASSERT_EQUAL_size_t(0, db->tail);
    TEST_ASSERT_EQUAL_size_t(0, db->head);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_buffer);
    return UNITY_END();
}