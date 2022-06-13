#include <stdlib.h>
#include <math.h>

#include "optimize.h"
#include "utils.h"
#include "score.h"
#include "constraints.h"


solution_t random_neighbor(solution_t* solution, problem_t* problem) {
    int neighbor_agent = rand() % problem->n_agents;
    int neighbor_mission = rand() % problem->n_missions;

    int old_mission = solution->assignments[neighbor_agent];
    solution->assignments[neighbor_agent] = neighbor_mission;

    while (is_solution_valid(solution, problem) == false) {
        // reverse the change
        solution->assignments[neighbor_agent] = old_mission;

        // generate new neighbor
        neighbor_agent = rand() % problem->n_agents;
        neighbor_mission = rand() % problem->n_missions;
        old_mission = solution->assignments[neighbor_agent];
        solution->assignments[neighbor_agent] = neighbor_mission;
    }

    return *solution;
}

solution_t optimize_solution(solution_t initial_solution, problem_t* problem) {
    solution_t current_solution = initial_solution;
    solution_t next_solution;

    // runs until the recuit temperature is low enough
    while (problem->temperature > problem->temperature_threshold){
        bool solution_accepted = false;

        // search for a solution until it is good enough
        do {
            next_solution = random_neighbor(&current_solution, problem);
            double delta_f = next_solution.score - current_solution.score;

            if (delta_f < 0) {
                solution_accepted = true;
            }
            else if (rand() < exp(-(delta_f)/problem->temperature)){
                solution_accepted = true;
            }
        } while (!solution_accepted);

        current_solution = next_solution;
        problem->temperature *= problem->temperature_mult;
    }

    return current_solution;
}

// TODO: split into multiple functions for each condition
bool is_solution_valid(solution_t* solution, const problem_t* problem) {
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

    // Check the cascade objective validity
    score_solution(solution, problem);
    if (solution->score == -1) {
        return false;
    }

    return true;
}
