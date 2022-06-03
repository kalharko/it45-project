#include <unity.h>
#include "load.c"

void setUp(void) {}

void tearDown(void) {}

void test_hello(void) {
    TEST_IGNORE_MESSAGE("Hello, world!");
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_hello);
    RUN_TEST(test_load_distance);

    UNITY_END();
}
