#include <initial.h>
#include <stdbool.h>

bool is_solution_assigned(solution_t* solution, size_t n_agents) {
    for (size_t n = 0; n < solution->n_assignements; n++) {
        if (solution->assignements[n] >= n_agents) return false;
    }
    return true;
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
        0,
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
        0,
        LSF,
        JARDINAGE,
        0,
        10*60,
        11*60
    );

    solution_t solution = build_naive(&problem);

    TEST_ASSERT(is_solution_assigned(&solution, problem.n_agents));
    TEST_ASSERT_EQUAL_INT(solution.assignements[0], 0);
    TEST_ASSERT_EQUAL_INT(solution.assignements[1], 1);

    free_problem(problem);
}

void test_initial() {
    RUN_TEST(test_build_naive);
}
