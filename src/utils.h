#ifndef UTILS_H
#define UTILS_H

#include "defs.h"
#include <stdbool.h>

/**
    Returns a new, empty solution with room for n_assignments.
    If `n_assignments == 0`, then no array will be allocated.
**/
solution_t empty_solution(size_t n_assignments);

/**
    Frees a `solution_t`.
**/
void free_solution(solution_t solution);

#define N_DAYS 6

struct timetable {
    size_t* assignments[N_DAYS];
    size_t lengths[N_DAYS];
    size_t agent;
} typedef timetable_t;

/**
    Constructs the time table for a given `agent` from a `solution` to `problem`.
    The returned time table is guaranteed to have the following properties:
    - `∀d, ∀i, i < lengths[d] <=> problem->missions[d][i] is defined`
    - `∀d, ∀i, problem->missions[assignments[d][i]].day == d`
    - `∀d, ∀i>0, problem->missions[assignments[d][i-1]].end_time <= problem->missions[assignments[d][i]].start_time`
    - `return.agent == agent`
**/
timetable_t build_time_table(
    const solution_t* solution,
    const problem_t* problem,
    size_t agent
);

/// Frees the memory allocated for a `timetable_t` instance
void free_time_table(timetable_t time_table);



#endif // UTILS_H
