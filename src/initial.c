#include "initial.h"
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "utils.h"
#include "constraints.h"
#include "optimize.h"

bool is_initial_assignment_valid_sub(const timetable_t* time_table, const problem_t* problem) {
    // Check that the mission skills match
    if (!has_matching_skills(time_table, problem)) {
        return false;
    }

    // Check that no missions overlap
    for (size_t day = 0; day < N_DAYS; day++) {
        if (time_table_distance(time_table, problem, day) < 0.0) {
            return false;
        }
    }

    // Check that there is a lunch break
    if (!has_lunch_break(time_table, problem)) {
        return false;
    }

    // TODO: amplitude

    return true;
}

bool is_initial_assignment_valid(const solution_t* solution, const problem_t* problem, size_t last_agent) {
    timetable_t time_table = build_time_table(solution, problem, last_agent);
    //@zig `defer free_time_table(time_table);`

    bool res = is_initial_assignment_valid_sub(&time_table, problem);

    free_time_table(time_table);
    return res;
}

size_t build_naive_phase(
    size_t* available_missions,
    size_t n_available_missions,
    solution_t* solution,
    const problem_t* problem,
    bool speciality_should_match
) {
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

            // Check whether the skills/specialities match
            if (
                mission->skill == agent->skill
                && (!speciality_should_match || mission->speciality == agent->speciality)
            ) {
                solution->assignments[available_missions[index2]] = agent_index;

                if (is_initial_assignment_valid(solution, problem, agent_index)) {
                    // assignment is considered valid; keep it and remove it from the available missions pool

                    // available_missions.remove(index2)
                    for (size_t i = index2 + 1; i < n_available_missions; i++) {
                        available_missions[i - 1] = available_missions[i];
                    }
                    n_available_missions--;

                    has_assigned = true;
                    break;
                } else {
                    // assignment isn't valid; remove it and leave it in the available missions pool
                    solution->assignments[available_missions[index2]] = SIZE_MAX;
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

    return n_available_missions;
}

solution_t build_naive(const problem_t* problem) {
    solution_t solution = empty_solution(problem->n_missions);
    size_t* available_missions = malloc(sizeof(size_t) * problem->n_missions);
    size_t n_available_missions = problem->n_missions;
    for (size_t n = 0; n < n_available_missions; n++) {
        available_missions[n] = n;
    }

    assert(problem->n_agents > 0);
    n_available_missions = build_naive_phase(
        available_missions,
        n_available_missions,
        &solution,
        problem,
        true
    );

    if (n_available_missions > 0) {
        n_available_missions = build_naive_phase(
            available_missions,
            n_available_missions,
            &solution,
            problem,
            false
        );
    }

    free(available_missions);

    return solution;
}

solution_t drop_population(solution_t* population, size_t n_individuals, size_t index) {
    assert(index < n_individuals);
    // Solution to return
    solution_t res = population[index];
    // Free all the other solutions
    for (size_t n = 0; n < n_individuals; n++) {
        if (n != index) {
            free_solution(population[n]);
        }
    }
    free(population);

    return res;
}

// TODO: store a "dirty" flag for each agent for memoization
float initial_score(
    const solution_t* solution,
    const problem_t* problem,
    initial_params_t initial_params
) {
    float score = 0.0;

    for (size_t agent = 0; agent < problem->n_agents; agent++) {
        timetable_t time_table = build_time_table(solution, problem, agent);

        if (!is_initial_assignment_valid_sub(&time_table, problem)) {
            free_time_table(time_table);
            return INFINITY;
        }

        float total_work_time = 0;
        float extra_work_time = 0;

        for (size_t day = 0; day < N_DAYS; day++) {
            float work_time = calc_day_work_time(&time_table, problem, day);

            total_work_time += work_time;
            if (work_time >= 8 * 60) {
                extra_work_time += work_time - 8 * 60;
            }

            // We are exceeding the 10h daily limit
            if (work_time >= 10 * 60) {
                score += work_time - 10 * 60;
            }
        }

        // Total work time exceeds the 48h limit
        if (total_work_time > 48 * 60) {
            score += total_work_time - 48 * 60;
        }

        // Extra work time exceeds the 10h limit
        if (extra_work_time > 10 * 60) {
            score += extra_work_time - 10 * 60;
        }

        free_time_table(time_table);
    }

    for (size_t n = 0; n < solution->n_assignments; n++) {
        if (solution->assignments[n] == SIZE_MAX) {
            score += initial_params.unassigned_penalty;
        }
    }

    return score;
}

void sort_individuals(solution_t* population, size_t n_individuals) {
    // Implementing it using bubble sort because I'm lazy
    for (size_t iter = 0; iter < n_individuals - 1; iter++) {
        bool has_swapped = false;
        for (size_t n = 0; n < n_individuals - 1; n++) {
            if (population[n].score > population[n + 1].score) {
                has_swapped = true;
                solution_t tmp = population[n];
                population[n] = population[n + 1];
                population[n + 1] = tmp;
            }
        }
        if (!has_swapped) break;
    }
}

solution_t new_individual(
    const problem_t* problem,
    initial_params_t initial_params,
    const solution_t* father,
    const solution_t* mother
) {
    solution_t res = empty_solution(mother->n_assignments);

    for (size_t n = 0; n < mother->n_assignments; n++) {
        res.assignments[n] = rand() % 2 == 0 ? mother->assignments[n] : father->assignments[n];
    }

    if ((float)rand() / (float)RAND_MAX < initial_params.mutation_rate) {
        res.assignments[rand() % res.n_assignments] = rand() % problem->n_agents;
    }

    return res;
}

solution_t initial_genetical_simulation(
    const problem_t* problem,
    initial_params_t initial_params,
    solution_t* population
) {
    assert(initial_params.survival_rate >= 0.0 && initial_params.survival_rate <= 1.0);

    size_t survival_population = (size_t)(initial_params.survival_rate * (float)initial_params.population);
    size_t reproduction_population = (size_t)(initial_params.reproduction_rate * (float)initial_params.population);

    for (uint64_t round = 0; round < initial_params.rounds; round++) {
        // Score all individuals
        for (size_t n = 0; n < initial_params.population; n++) {
            population[n].score = initial_score(&population[n], problem, initial_params);
        }

        // Sort individuals by their score
        sort_individuals(population, initial_params.population);

        // printf(
        //     "Round: %zu, best score: %f, median score: %f\n",
        //     round,
        //     population[0].score,
        //     population[initial_params.population / 2].score
        // );

        // Remove all individuals past `survival_population`
        size_t n_individuals = initial_params.population;
        for (size_t n = initial_params.population - 1; n > 0; n--) {
            if (!isfinite(population[n].score) || n > survival_population) {
                free_solution(population[n]);
                n_individuals--;
            } else if (is_solution_valid(&population[n], problem)) {
                // Valid solution found, return it!
                return drop_population(population, n_individuals, n);
            }
        }

        // Reproduce the best individuals until the pool is back up
        while (n_individuals < initial_params.population) {
            size_t subpop = n_individuals;
            if (subpop > reproduction_population) subpop = subpop - reproduction_population;

            size_t father_index = rand() % subpop;
            size_t mother_index = rand() % subpop;
            if (mother_index == father_index) mother_index = (mother_index + 1) % subpop;

            population[n_individuals++] = new_individual(problem, initial_params, &population[father_index], &population[mother_index]);
        }
    }

    // Return best individual instead
    solution_t res = drop_population(population, initial_params.population, 0);
    res.score = -1.0;
    return res;
}

solution_t build_initial_solution(const problem_t* problem, initial_params_t initial_params) {
    solution_t naive = build_naive(problem);

    if (is_solution_valid(&naive, problem)) {
        // Naive solution is good enough, return it
        return naive;
    }

    // Naive solution is not good enough, start building a pool of initial solutions
    //@invariant ∀i, i < n_individuals -> population[i]: defined
    solution_t* population = malloc(sizeof(solution_t) * initial_params.population);
    size_t n_individuals = 1;
    population[0] = naive;

    for (; n_individuals < initial_params.population; n_individuals++) {
        population[n_individuals] = build_naive(problem);

        if (is_solution_valid(&population[n_individuals], problem)) {
            // return this solution
            return drop_population(population, n_individuals + 1, n_individuals);
        }
    }

    // No valid solution found so far, running the genetical algorithm

    return initial_genetical_simulation(problem, initial_params, population);
}
