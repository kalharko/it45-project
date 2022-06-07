#include <load.h>
#include <string.h>

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


void test_load_agents(void) {

    agent_t agents[4];

    TEST_ASSERT_EQUAL_INT(
        0,
        load_agents("../Instances/45-4/Intervenants.csv", agents, 4)
    );

    // Test id
    for (int i=0; i<4; i++) {
        TEST_ASSERT_EQUAL_INT(i+1, agents[i].id);
    }

    // Test skill
    TEST_ASSERT_EQUAL_INT(LSF, agents[0].skill);
    TEST_ASSERT_EQUAL_INT(LPC, agents[1].skill);
    TEST_ASSERT_EQUAL_INT(LSF, agents[2].skill);
    TEST_ASSERT_EQUAL_INT(LPC, agents[3].skill);

    // Test speciality
    TEST_ASSERT_EQUAL_INT(JARDINAGE, agents[0].speciality);
    TEST_ASSERT_EQUAL_INT(MECANIQUE, agents[1].speciality);
    TEST_ASSERT_EQUAL_INT(MENUISERIE, agents[2].speciality);
    TEST_ASSERT_EQUAL_INT(MUSIQUE, agents[3].speciality);

    // Test hours
    TEST_ASSERT_EQUAL_INT(35, agents[0].hours);
    TEST_ASSERT_EQUAL_INT(35, agents[1].hours);
    TEST_ASSERT_EQUAL_INT(24, agents[2].hours);
    TEST_ASSERT_EQUAL_INT(24, agents[3].hours);
}


void test_load_missions(void) {

    mission_t missions[45];

    TEST_ASSERT_EQUAL_INT(
        0,
        load_missions("../Instances/45-4/Missions.csv", missions, 45)
    );

    // Test id
    for (int i=0; i<45; i++) {
    TEST_ASSERT_EQUAL_INT(i+1, missions[i].id);
    }

}
