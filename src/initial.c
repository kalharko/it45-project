#include "initial.h"
#include <stdbool.h>
#include <assert.h>

solution_t new_solution(problem_t* problem) {
    assert(problem->n_missions > 0);
    solution_t res;
    res.assignements = (size_t*)malloc(sizeof(size_t) * problem->n_missions);

    for (size_t n = 0; n < problem->n_missions; n++) {
        res.assignements[n] = SIZE_MAX;
    }

    res.score = 0.0;

    return res;
}

solution_t build_naive(problem_t* problem) {
    solution_t solution = new_solution(problem);
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
                solution.assignements[available_missions[index2]] = agent_index;

                // available_missions.remove(index2)
                for (size_t i = n_available_missions - 1; i > index2; i--) {
                    available_missions[i - 1] = available_missions[i];
                }
                n_available_missions--;

                has_assigned = true;
                break;
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
