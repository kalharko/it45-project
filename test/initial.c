#include <initial.h>
#include <stdbool.h>
#include <utils.h>
#include <inttypes.h>

bool is_solution_assigned(solution_t* solution, size_t n_agents) {
    for (size_t n = 0; n < solution->n_assignments; n++) {
        if (solution->assignments[n] >= n_agents) {
            fprintf(stderr, "Solution %zu is unassigned!\n", n);
            return false;
        }
    }
    return true;
}

bool is_mission_assigned(solution_t* solution, problem_t* problem, uint64_t agent, uint64_t id, bool print) {
    size_t agent_index = SIZE_MAX;
    for (size_t n = 0; n < problem->n_agents; n++) {
        if (problem->agents[n].id == agent) {
            agent_index = n;
            break;
        }
    }

    if (agent_index == SIZE_MAX) {
        if (print) fprintf(stderr, "Agent #%" PRIu64 " does not exist!\n", agent);
        return false;
    }

    for (size_t mission = 0; mission < problem->n_missions; mission++) {
        if (problem->missions[mission].id == id) {
            if (solution->assignments[mission] == agent_index) {
                return true;
            } else {
                if (print) fprintf(
                    stderr,
                    "Mission #%" PRIu64 " isn't assigned to agent #%" PRIu64 ", but agent #%" PRIu64 "\n",
                    id,
                    agent,
                    problem->agents[solution->assignments[mission]].id
                );
                return false;
            }
        }
    }

    if (print) fprintf(stderr, "Mission #%" PRIu64 " does not exist!\n", id);
    return false;
}

void test_build_naive(void) {
    problem_t problem = empty_problem();

    problem_push_agent(&problem,
        0,
        LSF,
        MECANIQUE,
        35
    );
    problem_push_agent(&problem,
        1,
        LSF,
        JARDINAGE,
        35
    );

    problem_push_mission(&problem,
        0,
        LSF,
        MECANIQUE,
        0,
        8*60,
        9*60
    );
    problem_push_mission(&problem,
        1,
        LSF,
        JARDINAGE,
        0,
        10*60,
        11*60
    );

    float distances[] = {
        0.0, 10.0, 10.0,
        10.0, 0.0, 15.0,
        10.0, 15.0, 0.0
    };
    problem_set_distances(&problem, distances);

    solution_t solution = build_naive(&problem);

    TEST_ASSERT(is_solution_assigned(&solution, problem.n_agents));
    TEST_ASSERT_EQUAL_INT(solution.assignments[0], 0);
    TEST_ASSERT_EQUAL_INT(solution.assignments[1], 1);

    free_solution(solution);
    free_problem(problem);
}


void test_build_naive_phase2(void) {
    for (size_t round = 0; round < 100; round++) {
        problem_t problem = empty_problem();

        problem_push_agent(&problem,
            0,
            LSF,
            MECANIQUE,
            35
        );
        problem_push_agent(&problem,
            1,
            LPC,
            JARDINAGE,
            35
        );

        problem_push_mission(&problem,
            0,
            LSF,
            MECANIQUE,
            0,
            8*60,
            9*60
        );
        problem_push_mission(&problem,
            1,
            LPC,
            JARDINAGE,
            0,
            10*60,
            11*60
        );

        // Only two of these missions should be assigned
        problem_push_mission(&problem,
            2,
            LSF,
            MENUISERIE,
            0,
            9*60 + 30,
            10*60 + 30
        );
        problem_push_mission(&problem,
            3,
            LPC,
            ELECTRICITE,
            0,
            11*60 + 30,
            12*60 + 30
        );
        problem_push_mission(&problem,
            4,
            LSF,
            MUSIQUE,
            0,
            9*60 + 30,
            10*60 + 30
        );
        problem_push_mission(&problem,
            5,
            LPC,
            MECANIQUE,
            0,
            11*60 + 30,
            12*60 + 30
        );

        problem_shuffle_missions(&problem);
        problem_set_dummy_distances(&problem, 10.0, 15.0);

        solution_t solution = build_naive(&problem);

        TEST_ASSERT_TRUE(is_mission_assigned(&solution, &problem, 0, 0, true));
        TEST_ASSERT_TRUE(is_mission_assigned(&solution, &problem, 1, 1, true));

        // One of the two remaining missions for each agent should be assigned
        TEST_ASSERT_TRUE(
            is_mission_assigned(&solution, &problem, 0, 2, false)
            || is_mission_assigned(&solution, &problem, 0, 4, false)
        );
        TEST_ASSERT_TRUE(
            is_mission_assigned(&solution, &problem, 1, 3, false)
            || is_mission_assigned(&solution, &problem, 1, 5, false)
        );

        // But not both, as they overlap!
        TEST_ASSERT_FALSE(
            is_mission_assigned(&solution, &problem, 0, 2, false)
            && is_mission_assigned(&solution, &problem, 0, 4, false)
        );
        TEST_ASSERT_FALSE(
            is_mission_assigned(&solution, &problem, 1, 3, false)
            && is_mission_assigned(&solution, &problem, 1, 5, false)
        );

        free_solution(solution);
        free_problem(problem);
    }
}

void test_drop_population(void) {
    for (size_t round = 0; round < 100; round++) {
        size_t length = 4 + rand() % 16;
        solution_t* population = (solution_t*)malloc(sizeof(solution_t) * length);

        for (size_t n = 0; n < length; n++) {
            population[n] = empty_solution(1);
            population[n].assignments[0] = n;
        }

        size_t x = rand() % length;

        solution_t sol = drop_population(population, length, x);
        TEST_ASSERT_EQUAL_UINT64(x, sol.assignments[0]);
    }
}

void test_sort_individuals(void) {
    for (size_t round = 0; round < 100; round++) {
        size_t length = 50 + rand() % 250;
        solution_t* population = (solution_t*)malloc(sizeof(solution_t) * length);

        for (size_t n = 0; n < length; n++) {
            population[n] = empty_solution(1);
            population[n].assignments[0] = n;
            population[n].score = (float)rand() / (float)SIZE_MAX * 16.0;
        }

        sort_individuals(population, length);

        for (size_t n = 0; n + 1 < length; n++) {
            TEST_ASSERT_TRUE(population[n + 1].score >= population[n].score);
        }
    }
}

void test_initial_score(void) {
    problem_t problem = empty_problem();

    problem_push_agent(&problem,
        0,
        LSF,
        MECANIQUE,
        32
    );
    problem_push_agent(&problem,
        1,
        LSF,
        JARDINAGE,
        32
    );

    problem_push_mission(&problem,
        0,
        LSF,
        MECANIQUE,
        0,
        8*60,
        12*60
    );

    problem_push_mission(&problem,
        1,
        LSF,
        MECANIQUE,
        0,
        14*60,
        18*60
    );

    problem_push_mission(&problem,
        2,
        LSF,
        MECANIQUE,
        0,
        19*60,
        23*60
    );

    problem_set_dummy_distances(&problem, 0.0, 0.0);

    solution_t solution = empty_solution(3);

    initial_params_t initial_params = {
        .unassigned_penalty = 30.0
    };

    TEST_ASSERT_FLOAT_WITHIN(0.1, 90.0, initial_score(&solution, &problem, initial_params));

    solution.assignments[0] = 0;
    TEST_ASSERT_FLOAT_WITHIN(0.1, 60.0, initial_score(&solution, &problem, initial_params));

    solution.assignments[1] = 0;
    TEST_ASSERT_FLOAT_WITHIN(0.1, 30.0, initial_score(&solution, &problem, initial_params));

    solution.assignments[2] = 0;
    TEST_ASSERT_FLOAT_WITHIN(0.1, 120.0, initial_score(&solution, &problem, initial_params));
}

void test_initial() {
    UNITY_BEGIN();

    RUN_TEST(test_build_naive);
    RUN_TEST(test_build_naive_phase2);

    RUN_TEST(test_drop_population);
    RUN_TEST(test_sort_individuals);

    RUN_TEST(test_initial_score);

    UNITY_END();
}
