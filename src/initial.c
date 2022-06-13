#include "initial.h"
#include <stdbool.h>
#include <assert.h>
#include "utils.h"
#include "constraints.h"

bool is_initial_assignment_valid(const solution_t* solution, const problem_t* problem, size_t last_agent) {
    timetable_t time_table = build_time_table(solution, problem, last_agent);
    //@zig `defer free_time_table(time_table);`

    // Check that the mission skills match
    if (!has_matching_skills(&time_table, problem)) {
        free_time_table(time_table);
        return false;
    }

    // Check that no missions overlap
    for (size_t day = 0; day < N_DAYS; day++) {
        if (time_table_distance(&time_table, problem, day) < 0.0) {
            free_time_table(time_table);
            return false;
        }
    }

    // Check that there is a lunch break
    if (!has_lunch_break(&time_table, problem)) {
        free_time_table(time_table);
        return false;
    }

    free_time_table(time_table);
    return true;
}

solution_t build_naive(problem_t* problem) {
    solution_t solution = empty_solution(problem->n_missions);
    size_t* available_missions = malloc(sizeof(size_t) * problem->n_missions);
    size_t n_available_missions = problem->n_missions;
    for (size_t n = 0; n < n_available_missions; n++) {
        available_missions[n] = n;
    }

    assert(problem->n_agents > 0);
    size_t agent_index = 0;
    size_t misses = 0;
    while (n_available_missions) {
        agent_t* agent = &problem->agents[agent_index];
        size_t index = rand() % n_available_missions;

        // Try to assign a random, available, fully-compatible mission
        bool has_assigned = false;
        size_t index2 = index;
        do {
            mission_t* mission = &problem->missions[available_missions[index2]];
            if (mission->skill == agent->skill && mission->speciality == agent->speciality) {
                solution.assignments[available_missions[index2]] = agent_index;

                if (is_initial_assignment_valid(&solution, problem, agent_index)) {
                    // available_missions.remove(index2)
                    for (size_t i = index2 + 1; i < n_available_missions; i++) {
                        available_missions[i - 1] = available_missions[i];
                    }
                    n_available_missions--;

                    has_assigned = true;
                    break;
                } else {
                    // Backtrack
                    solution.assignments[available_missions[index2]] = SIZE_MAX;
                }
            }
            index2 = (index2 + 1) % n_available_missions;
        } while (index2 != index);

        if (!has_assigned) {
            misses++;
            if (misses >= problem->n_agents) {
                break;
            }
        } else {
            misses = 0;
        }

        agent_index = (agent_index + 1) % problem->n_agents;
    }

    misses = 0;

    // TODO: step 2

    free(available_missions);

    return solution;
}

solution_t build_initial_solution(problem_t* problem, const initial_params_t* initial_params) {
    return build_naive(problem);
}
