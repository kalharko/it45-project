#include <unity.h>

#include "common.c"

#include "load.c"
#include "initial.c"
#include "optimize.c"
#include "utils.c"

void setUp(void) {}

void tearDown(void) {}

void test_hello(void) {
    TEST_IGNORE_MESSAGE("Hello, world!");
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_hello);
    RUN_TEST(test_load_distances);
    RUN_TEST(test_load_agents);
    RUN_TEST(test_load_missions);
    RUN_TEST(test_get_file_line_count);

    UNITY_END();

    test_utils();
    test_initial();

    return 0;
}
