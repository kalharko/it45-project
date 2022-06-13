#include <utils.h>
#include <constraints.h>

void test_build_time_table(void) {
    problem_t problem = empty_problem();

    problem_push_agent(&problem, 0, LSF, MECANIQUE, 32);
    problem_push_agent(&problem, 1, LPC, JARDINAGE, 32);

    problem_push_mission(&problem, 0, LSF, MECANIQUE,
        0,
        10 * 60,
        11 * 60
    );

    problem_push_mission(&problem, 1, LPC, JARDINAGE,
        0,
        10 * 60,
        11 * 60
    );

    problem_push_mission(&problem, 2, LSF, MECANIQUE,
        0,
        8 * 60,
        9 * 60
    );

    problem_push_mission(&problem, 3, LPC, JARDINAGE,
        0,
        12 * 60,
        13 * 60
    );

    problem_push_mission(&problem, 4, LSF, MECANIQUE,
        1,
        10 * 60,
        11 * 60
    );

    solution_t solution = empty_solution(5);
    solution.assignments[0] = 0;
    solution.assignments[1] = 1;
    solution.assignments[2] = 0;
    solution.assignments[3] = 1;
    solution.assignments[4] = 0;

    timetable_t tt_0 = build_time_table(&solution, &problem, 0);
    timetable_t tt_1 = build_time_table(&solution, &problem, 1);

    TEST_ASSERT_EQUAL(2, tt_0.lengths[0]);
    TEST_ASSERT_EQUAL(1, tt_0.lengths[1]);

    TEST_ASSERT_EQUAL(2, tt_1.lengths[0]);
    TEST_ASSERT_EQUAL(0, tt_1.lengths[1]);

    for (size_t n = 2; n < N_DAYS; n++) {
        TEST_ASSERT_EQUAL(0, tt_0.lengths[n]);
        TEST_ASSERT_EQUAL(0, tt_1.lengths[n]);
    }

    // Misison 2 is before mission 0, so it should come first
    TEST_ASSERT_EQUAL(2, tt_0.assignments[0][0]);
    TEST_ASSERT_EQUAL(0, tt_0.assignments[0][1]);

    TEST_ASSERT_EQUAL(4, tt_0.assignments[1][0]);

    TEST_ASSERT_EQUAL(1, tt_1.assignments[0][0]);
    TEST_ASSERT_EQUAL(3, tt_1.assignments[0][1]);

    free_time_table(tt_0);
    free_time_table(tt_1);

    free_problem(problem);
    free_solution(solution);
}

void test_build_time_table_random(void) {
    for (size_t round = 0; round < 1000; round++) {
        problem_t problem = empty_problem();
        size_t n_agents = 2 + rand() % 10;
        size_t n_missions[16] = {0,};
        size_t sum_missions = 0;

        for (size_t n = 0; n < n_agents; n++) {
            skill_t skill = rand() % N_SKILLS;
            problem_push_agent(&problem, n, skill, rand() % N_SPECIALTIES, 20 + rand() % 16);

            n_missions[n] = 1 + rand() % 32;
            sum_missions += n_missions[n];
            for (size_t o = 0; o < n_missions[n]; o++) {
                int start = rand() % (8 * 60) + 8 * 60;
                problem_push_mission(
                    &problem,
                    o,
                    skill,
                    rand() % N_SPECIALTIES,
                    rand() % N_DAYS,
                    start,
                    start + 30 + (rand() % 60)
                );
            }
        }

        solution_t solution = empty_solution(sum_missions);

        size_t mission_index = 0;
        for (size_t n = 0; n < n_agents; n++) {
            for (size_t o = 0; o < n_missions[n]; o++) {
                solution.assignments[mission_index] = n;
                mission_index++;
            }
        }

        for (size_t n = 0; n < n_agents; n++) {
            timetable_t time_table = build_time_table(&solution, &problem, n);

            // Check all the invariants on time_table!

            // return.agent == agent
            TEST_ASSERT_EQUAL(n, time_table.agent);

            // ∀d, ∀i, i < lengths[d] <=> problem->missions[d][i] is defined
            for (size_t day; day < N_DAYS; day++) {
                for (size_t i = 0; i < time_table.lengths[day]; i++) {
                    TEST_ASSERT_LESS_THAN_UINT64(time_table.assignments[day][i], sum_missions);
                }
            }

            // ∀d, ∀i, problem->missions[assignments[d][i]].day == d
            for (size_t day; day < N_DAYS; day++) {
                for (size_t i = 0; i < time_table.lengths[day]; i++) {
                    TEST_ASSERT_EQUAL_UINT64(
                        day,
                        problem.missions[time_table.assignments[day][i]].day
                    );
                }
            }

            // ∀d, ∀i>0, problem->missions[assignments[d][i]].end_time > problem->missions[assignments[d][i-1]].start_time
            for (size_t day; day < N_DAYS; day++) {
                for (size_t i = 0; i + 1 < time_table.lengths[day]; i++) {
                    TEST_ASSERT_LESS_OR_EQUAL_UINT64(
                        problem.missions[time_table.assignments[day][i]].end_time,
                        problem.missions[time_table.assignments[day][i+1]].start_time
                    );
                }
            }

            // Test has_matching_skills while we're at it (TODO: move to a different test case)
            TEST_ASSERT_TRUE(has_matching_skills(&time_table, &problem));

            free_time_table(time_table);
        }

        free_solution(solution);
        free_problem(problem);
    }
}

void test_utils() {
    UNITY_BEGIN();

    RUN_TEST(test_build_time_table);
    RUN_TEST(test_build_time_table_random);

    UNITY_END();
}
