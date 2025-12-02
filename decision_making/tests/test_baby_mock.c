#include "unity.h"
#include "baby_mock.h"

void test_crying_from_stress(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(100, crying_from_stress(100), "max stress stress -> crying failed.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(25, crying_from_stress(20), "crying intensity calculation failed at 20% stress");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, crying_from_stress(7), "crying intensity calculation failed at 7% stress");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, crying_from_stress(0xFF), "crying did not fail for OOB value ");
}

void test_heartbeat_from_stress(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(140, heartbeat_from_stress(50), "heartbeat calculation failed for 50% stress" );
    TEST_ASSERT_EQUAL_INT_MESSAGE(60, heartbeat_from_stress(7), "heartbeat calculation for lower bound stress");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, heartbeat_from_stress(0xFF), "heartbeat calculation did not fail for OOB stress value");
}

