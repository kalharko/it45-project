#include <load.h>

void test_load_distance(void) {
    float** data = malloc(sizeof(float*) * 45);

    for (size_t n = 0; n < 45; n++) {
        data[n] = malloc(sizeof(float) * 45);
        for (size_t o = 0; o < 45; o++) {
            data[n][o] = 0.0;
        }
    }

    TEST_ASSERT_EQUAL_INT(
        load_distance(45, 45, "../Instances/45-4/Distances.csv", data),
        0
    );

    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, data[0][0]);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 10749.546, data[0][1]);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 11436.03367, data[0][44]);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 10856.06586, data[1][0]);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 9872.718523, data[44][0]);

    for (size_t i = 0; i < 45; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, data[i][i]);
    }
}
