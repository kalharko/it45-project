#include "utils.h"
#include <stdbool.h>
#include <assert.h>

timetable_t build_time_table(
    const solution_t* solution,
    const problem_t* problem,
    size_t agent
) {
    timetable_t res;
    res.agent = agent;

    // Initialize the table with zeroes
    for (size_t i = 0; i < N_DAYS; i++) {
        res.assignments[i] = (size_t*)malloc(solution->n_assignments * sizeof(size_t));
        res.lengths[i] = 0;

        assert(res.assignments[i] != NULL);

        for (size_t j = 0; j < solution->n_assignments; j++) {
            res.assignments[i][j] = SIZE_MAX;
        }
    }

    for (size_t i = 0; i < solution->n_assignments; i++) {
        if (solution->assignments[i] == agent) {
            mission_t* mission = &problem->missions[i];

            assert(mission->day < N_DAYS);

            size_t day = (size_t)mission->day;
            size_t* assignments_today = res.assignments[day];

            bool inserted = false;
            // Find the first mission in `assignments_today` that's after the current mission,
            // and insert the current mission index (i) at this spot
            for (size_t n = 0; n < res.lengths[day]; n++) {
                if (problem->missions[assignments_today[n]].end_time <= mission->start_time) {
                    // assignments_today[n..].rshift()
                    for (size_t o = res.lengths[day]; o > n; o--) {
                        assignments_today[o] = assignments_today[o - 1];
                    }
                    // insert mission index (i)
                    assignments_today[n] = i;

                    inserted = true;
                    break;
                }
            }

            // No mission found, append current mission
            if (!inserted) {
                assignments_today[res.lengths[day]++] = i;
            }
        }
    }

    // Resize arrays to save on memory (remove this if we need to mutate timetable_t in hot-path code)
    for (size_t i = 0; i < N_DAYS; i++) {
        res.assignments[i] = (size_t*)realloc(res.assignments[i], res.lengths[i] * sizeof(size_t));

        assert(res.assignments[i] != NULL);
    }

    return res;
}

bool has_matching_skills(const timetable_t* time_table, const problem_t* problem) {
    for (size_t day = 0; day < N_DAYS; day++) {
        for (size_t j = 0; j < time_table.lengths[day]; j++) {
            if (problem->agents[agent_index].skill != problem->missions[time_table.assignments[day][j]].skill) {
                return false;
            }
        }
    }
    return true;
}
