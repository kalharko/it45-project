#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include <stdbool.h>

#include "defs.h"

struct optimize_params {
    float heat;
    float heat_falloff;
} typedef optimize_params_t;


/// Random (valid) neighbor (using rejection sampling)
void random_neighbor(const solution_t* solution, const problem_t* problem ,solution_t* neighbor);

solution_t optimize_solution(solution_t initial_solution, problem_t* problem);

// Check for solution validity
bool is_solution_valid(solution_t* solution, const problem_t* problem);

#endif // OPTIMIZE_H
