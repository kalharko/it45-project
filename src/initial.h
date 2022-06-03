#ifndef INITIAL_H
#define INITIAL_H

#include <stdlib.h>
#include "defs.h"

//! Computes an initial solution to the problem

/// Contains the parameters for building the initial solution
struct initial_params {
    float survival_rate; // [0-1]
    float reproduction_rate; // [0-1], proportion of the population that gets to reproduce
    float mutation_rate; // [0-1], the probability for an individual to mutate
} typedef initial_params_t;

solution_t build_naive(problem_t* problem);

/// Sorts the array individuals by the `score` key
void sort_individuals(solution_t* individuals, size_t length);

solution_t reproduce(solution_t* father, solution_t* mother, const initial_params_t* initial_params);

solution_t build_initial_solution(problem_t* problem, const initial_params_t* initial_params);

#endif // INITIAL_H
