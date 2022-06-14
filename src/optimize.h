#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include <stdbool.h>

#include "defs.h"

struct optimize_params {
    float heat;
    float heat_falloff;
} typedef optimize_params_t;

/// Returns false if the `mission` cannot be assigned to `agent`.
/// If true is returned, then it is still possible that `mission` cannot be assigned to `agent`.
bool can_assign(const solution_t* solution, const problem_t* problem, size_t agent, size_t mission);

/// Random (valid) neighbor (using rejection sampling)
bool random_neighbor(const solution_t* solution, const problem_t* problem ,solution_t* neighbor);

solution_t optimize_solution(solution_t initial_solution, const problem_t* problem);

// Check for solution validity
bool is_solution_valid(solution_t* solution, const problem_t* problem);

#endif // OPTIMIZE_H
