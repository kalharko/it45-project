#include <stdlib.h>
#include <math.h>

#include "optimize.h"
#include "utils.h"

float score_solution(solution_t* solution, problem_t* problem) {
    float score = 0;

    switch (problem->current_objective)
    {
        case 0 :
        break;

        case 1 :
        break;

        case 2 :
        break;

        default :
        break;
    }

    return score;
}

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
        double current_score = score_solution(&current_solution, problem);
        bool solution_accepted = false;

        // search for a solution until it is good enough
        do {
            next_solution = random_neighbor(&current_solution, problem);
            double delta_f = score_solution(&next_solution, problem) - current_score;

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
    for (int agent_index = 0; agent_index < problem->n_agents; agent_index++) {
        timetable_t time_table = build_time_table(solution, problem, agent_index);

        // Check skill match
        for (size_t day = 0; day < N_DAYS; day++) {
            for (size_t j = 0; j < time_table.lengths[day]; j++) {
                if (problem->agents[agent_index].skill != problem->missions[time_table.assignments[day][j]].skill) {
                    return false;
                }
            }
        }

        // TODO: add a distance computation in utils.c?
        // Check travel times
        float distance;
        float time;
        solution->distance_traveled = 0;
        for (int day = 1; day < N_DAYS; day++) {
            size_t* assignments = time_table.assignments[day];

            for (int a = 0; a < time_table.lengths[day] - 1; a++) {
                distance = problem->distances[assignments[a]][assignments[a+1]];

                solution->distance_traveled += distance;
                time = problem->missions[assignments[a+1]].start_time;
                time -= problem->missions[assignments[a]].end_time;
                // TODO: un-magic that number
                if (!(distance / 50.0 <= time)) {
                    return false;
                }
            }
        }

    }

    return true;
}
