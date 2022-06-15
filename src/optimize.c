#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "optimize.h"
#include "utils.h"
#include "score.h"
#include "constraints.h"
#include "visualisation.h"

/// Returns false if the `mission` cannot be assigned to `agent`.
/// If true is returned, then it is still possible that `mission` cannot be assigned to `agent`.
bool can_assign(const solution_t* solution, const problem_t* problem, size_t agent, size_t mission, size_t ignore_overlap) {
    mission_t* mission1 = &problem->missions[mission];

    if (
        solution->assignments[mission] == agent
        || mission1->skill != problem->agents[agent].skill
    ) return false;

    for (size_t n = 0; n < solution->n_assignments; n++) {
        if (solution->assignments[n] == agent && n != ignore_overlap) {
            mission_t* mission2 = &problem->missions[n];

            if (
                mission1->day == mission2->day
                && mission1->start_time <= mission2->end_time
                && mission1->end_time >= mission2->start_time
            ) {
                return false;
            }
        }
    }

    return true;
}


bool random_neighbor_single(const solution_t* solution, const problem_t* problem, solution_t* neighbor) {
    memcpy(neighbor->assignments, solution->assignments, sizeof(size_t)*solution->n_assignments);
    neighbor->distance_traveled = 0.0;

    size_t new_agent = rand() % problem->n_agents;
    size_t new_mission = rand() % problem->n_missions;
    size_t count = 0;

    for (; count < problem->n_agents; count++) {
        // Check if agent has another overlapping mission
        if (can_assign(solution, problem, new_agent, new_mission, SIZE_MAX)) {
            break;
        } else {
            new_agent = (new_agent + 1) % problem->n_agents;
        }
    }

    if (count >= problem->n_agents) return false;

    neighbor->assignments[new_mission] = new_agent;

    bool is_valid = is_solution_valid(neighbor, problem);
    score_solution(neighbor, problem);

    return is_valid && neighbor->score >= 0.0;
}

bool random_neighbor_dual(const solution_t* solution, const problem_t* problem, solution_t* neighbor) {
    memcpy(neighbor->assignments, solution->assignments, sizeof(size_t)*solution->n_assignments);
    neighbor->distance_traveled = 0.0;

    size_t new_agent = rand() % problem->n_agents;
    size_t new_mission = rand() % problem->n_missions;
    size_t old_mission = rand() % problem->n_missions;

    // Find old mission (TODO: optimize)
    for (size_t n = 0; n < problem->n_missions; n++) {
        if (solution->assignments[(n + old_mission) % problem->n_missions] == new_agent) {
            old_mission = (old_mission + n) % problem->n_missions;
            break;
        }
    }

    size_t count = 0;
    for (; count < problem->n_agents; count++) {
        // Check if agent has another overlapping mission
        if (can_assign(solution, problem, new_agent, new_mission, old_mission)) {
            break;
        } else {
            new_agent = (new_agent + 1) % problem->n_agents;

            // Find a new old mission
            old_mission = rand() % problem->n_missions;
            for (size_t n = 0; n < problem->n_missions; n++) {
                if (solution->assignments[(n + old_mission) % problem->n_missions] == new_agent) {
                    old_mission = (old_mission + n) % problem->n_missions;
                    break;
                }
            }
        }
    }

    if (count >= problem->n_agents) return false;

    size_t old_agent = solution->assignments[new_mission];
    neighbor->assignments[new_mission] = new_agent;
    neighbor->assignments[old_mission] = old_agent;

    bool is_valid = is_solution_valid(neighbor, problem);
    score_solution(neighbor, problem);

    return is_valid && neighbor->score >= 0.0;
}

bool random_neighbor(const solution_t* solution, const problem_t* problem, solution_t* neighbor) {
    if (rand() % 4 == 0) {
        return random_neighbor_single(solution, problem, neighbor);
    } else {
        return random_neighbor_dual(solution, problem, neighbor);
    }
}

solution_t optimize_solution(solution_t initial_solution, const problem_t* problem, int cut_of) {
    int cut_of_value = 0;
    solution_t current_solution = initial_solution;
    bool is_valid = is_solution_valid(&current_solution, problem);
    assert(is_valid);

    score_solution(&current_solution, problem);

    solution_t next_solution = empty_solution(problem->n_missions);

    float temperature = problem->temperature;
    float best_score = current_solution.score;

    // runs until the recuit temperature is low enough
    while (temperature > problem->temperature_threshold) {
        bool solution_accepted = false;

        // search for a solution until it is good enough
        uint64_t attempts = 0;
        do {
            // TODO: problem->max_attempts
            if (attempts++ > 10000) {
                fprintf(stderr, "Couldn't find a valid neighbor!\n");
                free_solution(next_solution);
                return current_solution;
            }

            if (!random_neighbor(&current_solution, problem, &next_solution)) {
                continue;
            }

            float delta_f = next_solution.score - current_solution.score;
            // printf("delta_f : %f - %f\n", next_solution.score, current_solution.score);

            if (delta_f == 0) {
                solution_accepted = true;
            } else if (delta_f < 0) {
                solution_accepted = true;
            } else if (1.0/(rand()%100) < exp(-(delta_f) / temperature)) {
                solution_accepted = true;
                //printf("luck\n");
            }
        } while (!solution_accepted);

        if (problem->current_objective == 2) {
            assert(next_solution.distance_traveled <= current_solution.distance_traveled);
        }

        solution_t tmp = current_solution;
        current_solution = next_solution;
        next_solution = tmp;

        if (current_solution.score < best_score) {
            //printf("Achieved new best score: old = %.4f, new = %.4f\n", best_score, current_solution.score);
            best_score = current_solution.score;
            cut_of_value = 0;
        }
        else {
            cut_of_value += 1;
            if (cut_of_value >= cut_of) {
                log_for_graph(&current_solution, problem);
                printf("Cut_off");
                return current_solution;
            }
        }
        // print_solution(next_solution);

        log_for_graph(&current_solution, problem);
        temperature *= problem->temperature_mult;
    }

    free_solution(next_solution);

    return current_solution;
}

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
                free_time_table(time_table);
                return false;
            }
            else {
                solution->distance_traveled += distance;
            }
        }

        free_time_table(time_table);
    }

    return true;
}
