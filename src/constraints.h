#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

#include "utils.h"


/**
    Returns true iff `∀d, ∀i, problem->missions[time_table[d][i]].skill == problem->agents[time_table.agent].skill`
**/
bool has_matching_skills(const timetable_t* time_table, const problem_t* problem);

/**
    Returns true iff the assignments for the given time table allows an hour of snack time between 12h and 14h every day
**/
bool has_lunch_break(const timetable_t* time_table, const problem_t* problem);

/**
    Returns the number of extra hours in the given time_table (can be 0).
    Returns -1 if the maximum legal work time is exceeded.
**/
float time_table_extra_hours(const int agent, const timetable_t* time_table, const problem_t* problem);


/**
    Computes the distance traveled for a given day.
    Returns 0 if `day >= N_DAYS` or if there are no assignments that day.
    Returns -1 if there is not enough time between assignments for the travel time
**/
float time_table_distance(const timetable_t* time_table, const problem_t* problem, size_t day);

#endif // CONSTRAINTS_H
