#include "optimize.h"

float score_solution(solution_t* solution, problem_t* problem) {
    return 0.0;
}

solution_t random_neighbor(solution_t* solution, problem_t problem) {
    int neighbor_agent = rand() % problem.n_agents;
    int neighbor_mission = rand() % problem.n_missions;

    int old_mission = solution->assignments[neighbor_agent];
    solution->assignments[neighbor_agent] = neighbor_mission;

    while (is_solution_valid(solution, problem) == false) {
        // reverse the change
        solution->assignments[neighbor_agent] = old_mission;

        // generate new neighbor
        neighbor_agent = rand() % problem.n_agents;
        neighbor_mission = rand() % problem.n_missions;
        old_mission = solution->assignments[neighbor_agent];
        solution->assignments[neighbor_agent] = neighbor_mission;
    }

    return *solution;
}

solution_t optimize_solution(solution_t initial_solution, problem_t* problem, optimize_params_t* params) {
    solution_t solution = initial_solution;


}


bool is_solution_valid(solution_t* solution, problem_t problem)
{
    // Check that each agent can go to each of his assignments
    for (int i=0; i<problem.n_agents; i++) {

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
                    if (time_table[problem.missions[j].day][period] == 0) {
                        time_table[problem.missions[j].day][period] = j;
                        break;
                    }
                    else if (problem.missions[time_table[problem.missions[j].day][period]].end_time <= problem.missions->start_time) {
                        period += 1;
                        int temp = time_table[problem.missions[j].day][period];
                        int temp2;
                        time_table[problem.missions[j].day][period] = j;
                        period += 1;

                        while (temp != 0) {
                            temp2 = time_table[problem.missions[j].day][period];
                            time_table[problem.missions[j].day][period] = temp;
                            temp = temp2;
                            period += 1;
                        }
                        break;
                    }
                }

                // Check skill match
                if (problem.agents[i].skill != problem.missions[j].skill) {
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
                    distance = problem.distances[time_table[day][a]][time_table[day][a+1]];
                    solution->distance_traveled += distance;
                    time = problem.missions[time_table[day][a+1]].start_time;
                    time -= problem.missions[time_table[day][a]].end_time;
                    if (!(distance / 50.0 <= time)) {
                        return false;
                    }
                }
            }
        }

    }

    return true;
}
