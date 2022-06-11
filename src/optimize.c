#include <stdlib.h>
#include <math.h>

#include "optimize.h"

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
    bool solution_accepted;
    double current_score;
    double delta_f;

    // runs until the recuit temperature is low enough
    while (problem->temperature > problem->temperature_threshold){
        next_solution = random_neighbor(&current_solution, problem);
        current_score = score_solution(&current_solution, problem);
        delta_f = score_solution(&next_solution, problem) - current_score;

        // check if the next solution is good
        if (delta_f < 0) {
            solution_accepted = true;
        }
        else if (rand() < exp(-(delta_f)/problem->temperature)){
            solution_accepted = true;
        }
        else {
            solution_accepted = false;
        }

        // search for another next solution if the first one was not good
        while (!solution_accepted) {
            next_solution = random_neighbor(&current_solution, problem);
            delta_f = score_solution(&next_solution, problem) - current_score;

            if (delta_f < 0) {
                solution_accepted = true;
            }
            else if (rand() < exp(-(delta_f)/problem->temperature)){
                solution_accepted = true;
            }
        }

        current_solution = next_solution;
        problem->temperature *= problem->temperature_mult;
    }
}


bool is_solution_valid(solution_t* solution, problem_t* problem)
{
    // Check that each agent can go to each of his assignments
    for (int i=0; i<problem->n_agents; i++) {

        // time_table[day of the week][mission] for one agent
        uint8_t **time_table;
        time_table = (uint8_t **)malloc(6 * sizeof(uint8_t *));
        for (int i = 0; i < 5; ++i){
            time_table[i] = (uint8_t *)malloc(solution->n_assignments * sizeof(uint8_t));
        }

        int head_assignments = 0;
        // Gather an agent's missions
        for (int j=0; j<solution->n_assignments; i++) {
            if (solution->assignments[j] == i) {
                // Fill the time_table so the assignments are in order
                for (int period=0; period < solution->n_assignments; period++) {
                    if (time_table[problem->missions[j].day][period] == 0) {
                        time_table[problem->missions[j].day][period] = j;
                        break;
                    }
                    else if (problem->missions[time_table[problem->missions[j].day][period]].end_time <= problem->missions->start_time) {
                        period += 1;
                        int temp = time_table[problem->missions[j].day][period];
                        int temp2;
                        time_table[problem->missions[j].day][period] = j;
                        period += 1;

                        while (temp != 0) {
                            temp2 = time_table[problem->missions[j].day][period];
                            time_table[problem->missions[j].day][period] = temp;
                            temp = temp2;
                            period += 1;
                        }
                        break;
                    }
                }

                // Check skill match
                if (problem->agents[i].skill != problem->missions[j].skill) {
                    return false;
                }
            }
        }

        // Check travel times
        float distance;
        float time;
        solution->distance_traveled = 0;
        for (int day=1; day<6; day++) {
            int nb_assignment_day = 0;
            while (time_table[day][nb_assignment_day] != 0) {
                nb_assignment_day += 1;
            }
            if (nb_assignment_day > 1) {
                for (int a=0; a<nb_assignment_day-1; a++) {
                    distance = problem->distances[time_table[day][a]][time_table[day][a+1]];
                    solution->distance_traveled += distance;
                    time = problem->missions[time_table[day][a+1]].start_time;
                    time -= problem->missions[time_table[day][a]].end_time;
                    if (!(distance / 50.0 <= time)) {
                        return false;
                    }
                }
            }
        }

    }

    return true;
}
