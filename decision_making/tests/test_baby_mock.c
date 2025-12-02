#include "unity.h"
#include "baby_mock.h"
#include "time.h"

static struct baby_mock_args
{
    DataBuffer *db_in;
    pthread_mutex_t *mutex_in;
    DataBuffer *db_out;
    pthread_mutex_t *mutex_out;
    int initial_stress;
};

static void *baby_mock_runner(void *args)
{
    struct baby_mock_args *baby_args = (struct baby_mock_args *)args;

    start_baby_loop(baby_args->db_in, baby_args->mutex_in, baby_args->db_out, baby_args->mutex_out, baby_args->initial_stress);
    return NULL;
}

volatile int keep_baby_mock_running = 1;

void test_crying_from_stress(void)
{
    TEST_ASSERT_EQUAL_INT_MESSAGE(100, crying_from_stress(100), "max stress stress -> crying failed.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(25, crying_from_stress(20), "crying intensity calculation failed at 20% stress");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, crying_from_stress(7), "crying intensity calculation failed at 7% stress");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, crying_from_stress(0xFF), "crying did not fail for OOB value ");
}

void test_heartbeat_from_stress(void)
{
    TEST_ASSERT_EQUAL_INT_MESSAGE(140, heartbeat_from_stress(50), "heartbeat calculation failed for 50% stress");
    TEST_ASSERT_EQUAL_INT_MESSAGE(60, heartbeat_from_stress(7), "heartbeat calculation for lower bound stress");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, heartbeat_from_stress(0xFF), "heartbeat calculation did not fail for OOB stress value");
}

void test_baby_mock_check_bare_values(void)
{

    struct timespec ts;
    ts.tv_nsec = 2*BABY_REACTION_DELAY_MSEC * 1000 * 1000;
    ts.tv_sec = 2*BABY_REACTION_DELAY_MSEC / 1000;

    DataBuffer *db_in = databuffer_create(5, 2);
    pthread_mutex_t mutex_in;
    pthread_mutex_init(&mutex_in, NULL);

    DataBuffer *db_out = databuffer_create(5, 2);
    pthread_mutex_t mutex_out;
    pthread_mutex_init(&mutex_out, NULL);
    struct baby_mock_args baby_args = {
        .db_in = db_in,
        .db_out = db_out,
        .mutex_in = &mutex_in,
        .mutex_out = &mutex_out,
        .initial_stress = 100,
    };

    pthread_t baby_mock;
    pthread_create(&baby_mock, NULL, baby_mock_runner, &baby_args);

    // test bare value response
    uint8_t motor_vals[2] = {65, 100};
    databuffer_push(motor_vals, db_in);

    nanosleep(&ts, NULL);
    uint8_t initial_vitals[2] = {heartbeat_from_stress(90), crying_from_stress(90)};
    uint8_t vital_values[2] = {0, 0};
    databuffer_pop(db_out, vital_values);

    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(initial_vitals, vital_values, 2, "vital assertion at initial motor moves failed");

    keep_baby_mock_running = 0;
}