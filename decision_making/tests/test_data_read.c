#include <unity/unity.h>
#include <data_read.h>
#include <stdlib.h>

void setUp(void)
{
}

void tearDown(void) {}

static void test_push_buffer(void) { //TODO add a test to test for when there are multiple arrays pushed
    size_t buffer_size = 25;
    size_t array_len = 2;
    DataBuffer* db = databuffer_create(buffer_size, array_len);

    float val[] = {2.0f, 5.0f};
    int status = databuffer_push(val,db);
    TEST_ASSERT_EQUAL_INT(0, status);
    TEST_ASSERT_EQUAL_size_t(1, db ->count);
    TEST_ASSERT_EQUAL_size_t(1, db ->head);

    float *out_array = malloc((sizeof(float) * array_len));
    databuffer_pop(db, out_array);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(val, out_array, array_len);

    free(out_array);
    TEST_ASSERT_EQUAL_INT(0,databuffer_destroy(db));
    
}

static void test_create_buffer(void) {
    size_t buffer_size = 25;
    size_t array_len = 2;
    DataBuffer* db = databuffer_create(buffer_size, array_len);

    TEST_ASSERT_EQUAL_size_t(buffer_size, db->buffer_capacity );
    TEST_ASSERT_EQUAL_size_t(array_len, db->array_len);
    TEST_ASSERT_EQUAL_size_t(0, db->count);
    TEST_ASSERT_EQUAL_size_t(0, db->tail);
    TEST_ASSERT_EQUAL_size_t(0, db->head);

    TEST_ASSERT_EQUAL_INT(0,databuffer_destroy(db));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_buffer);
    RUN_TEST(test_push_buffer);
    return UNITY_END();
}