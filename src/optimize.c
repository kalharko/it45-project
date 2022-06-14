#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "optimize.h"
#include "utils.h"
#include "score.h"
#include "constraints.h"

/// Returns false if the `mission` cannot be assigned to `agent`.
/// If true is returned, then it is still possible that `mission` cannot be assigned to `agent`.
bool can_assign(const solution_t* solution, const problem_t* problem, size_t agent, size_t mission) {
    mission_t* mission1 = &problem->missions[mission];

    if (
        solution->assignments[mission] == agent
        || mission1->skill != problem->agents[agent].skill
    ) return false;

    for (size_t n = 0; n < solution->n_assignments; n++) {
        if (solution->assignments[n] == agent) {
            mission_t* mission2 = &problem->missions[n];

            if (mission1->start_time <= mission2->end_time && mission1->end_time >= mission2->start_time) {
                return false;
            }
        }
    }

    return true;
}

bool random_neighbor(const solution_t* solution, const problem_t* problem, solution_t* neighbor) {
    memcpy(neighbor->assignments, solution->assignments, sizeof(size_t)*solution->n_assignments);

    size_t neighbor_agent = rand() % problem->n_agents;
    size_t neighbor_mission = rand() % problem->n_missions;

    size_t count = 0;
    for (; count < problem->n_agents; count++) {
        // Check if agent has another overlapping mission
        if (can_assign(solution, problem, neighbor_agent, neighbor_mission)) {
            break;
        } else {
            neighbor_agent = (neighbor_agent + 1) % problem->n_agents;
        }
    }

    if (count >= problem->n_agents) return false;

    printf("%zu %zu\n", neighbor_agent, neighbor_mission);

    size_t old_mission = solution->assignments[neighbor_agent];
    neighbor->assignments[neighbor_agent] = neighbor_mission;

    score_solution(neighbor, problem);

    return is_solution_valid(neighbor, problem) && neighbor->score >= 0.0;
        // reverse the change
        // neighbor->assignments[neighbor_agent] = old_mission;
        // return false;

        // generate new neighbor
        // neighbor_agent = rand() % problem->n_agents;
        // neighbor_mission = rand() % problem->n_missions;
        // old_mission = solution->assignments[neighbor_agent];
        // neighbor->assignments[neighbor_agent] = neighbor_mission;
    // }
    // return true;
}

solution_t optimize_solution(solution_t initial_solution, const problem_t* problem) {
    solution_t current_solution = initial_solution;
    score_solution(&current_solution, problem);

    solution_t next_solution = empty_solution(problem->n_missions);

    float temperature = problem->temperature;

    // runs until the recuit temperature is low enough
    while (temperature > problem->temperature_threshold) {
        bool solution_accepted = false;

        // search for a solution until it is good enough
        uint64_t attempts = 0;
        do {
            // TODO: problem->max_attempts
            if (attempts++ > 10000) {
                fprintf(stderr, "Couldn't find a valid neighbor!\n");
                return current_solution;
            }

            if (!random_neighbor(&current_solution, problem, &next_solution)) {
                continue;
            }

            float delta_f = next_solution.score - current_solution.score;
            // printf("delta_f : %f - %f\n", next_solution.score, current_solution.score);

            if (delta_f == 0) {
                solution_accepted = true;
                printf("equal\n");
            }
            if (delta_f < 0) {
                solution_accepted = true;
                printf("inferior\n");
            } else if (rand() < exp(-(delta_f) / temperature)){
                solution_accepted = true;
                printf("luck\n");
            }
        } while (!solution_accepted);

        printf("%f -> %f\n", current_solution.score, next_solution.score);
        print_solution(next_solution);
        solution_t tmp = current_solution;
        current_solution = next_solution;
        next_solution = tmp;

        temperature *= problem->temperature_mult;
    }

    return current_solution;
}

// TODO: split into multiple functions for each condition
bool is_solution_valid(solution_t* solution, const problem_t* problem) {
    // Check that every mission is assigned
    for (size_t n = 0; n < solution->n_assignments; n++) {
        if (solution->assignments[n] >= problem->n_agents) return false;
    }

    // Check that each agent can go to each of his assignments
    solution->distance_traveled = 0;
    for (int agent_index = 0; agent_index < problem->n_agents; agent_index++) {
        timetable_t time_table = build_time_table(solution, problem, agent_index);

        // Check skill match
        if (!has_matching_skills(&time_table, problem)) return false;

        // Check lunch break
        if (!has_lunch_break(&time_table, problem)) return false;

        // Check travel times and gather total distance traveled
        float distance;
        float time;
        for (int day = 1; day < N_DAYS; day++) {
            distance = time_table_distance(&time_table, problem, day);
            if (distance == -1) {
                return false;
            }
            else {
                solution->distance_traveled += distance;
            }
        }
    }

    return true;
}
