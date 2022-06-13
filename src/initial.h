#ifndef INITIAL_H
#define INITIAL_H

#include <stdlib.h>
#include "defs.h"

//! Computes an initial solution to the problem

/// Contains the parameters for building the initial solution
struct initial_params {
    size_t population; // number of inidividuals

    uint64_t rounds; // number of steps in the simulation

    float survival_rate; // [0.0-1.0]
    float reproduction_rate; // [0.0-1.0], proportion of the population that gets to reproduce
    float mutation_rate; // [0.0-1.0], the probability for an individual to mutate

    float unassigned_penalty; // how many minutes an unassigned mission is worth
} typedef initial_params_t;

solution_t build_naive(const problem_t* problem);

/// Returns the score of a given solution for the initial phase, +∞ if it is an invalid initial solution
float initial_score(
    const solution_t* solution,
    const problem_t* problem,
    initial_params_t initial_params
);

/// Sorts the array individuals by the `score` key
void sort_individuals(solution_t* individuals, size_t length);

solution_t reproduce(solution_t* father, solution_t* mother, const initial_params_t* initial_params);

solution_t build_initial_solution(const problem_t* problem, initial_params_t initial_params);

/// Frees population and returns `population[index]`
solution_t drop_population(solution_t* population, size_t n_individuals, size_t index);

#endif // INITIAL_H
