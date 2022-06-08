#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include <stdbool.h>

#include "defs.h"

struct optimize_params {
    float heat;
    float heat_falloff;
} typedef optimize_params_t;

float score_solution(solution_t* solution, problem_t* problem);

/// Random (valid) neighbor (using rejection sampling)
solution_t random_neighbor(solution_t* solution, problem_t* problem);

solution_t optimize_solution(solution_t initial_solution, problem_t* problem, optimize_params_t* params);

// Check for solution validity
bool is_solution_valid(solution_t solution, problem_t problem);

#endif // OPTIMIZE_H
