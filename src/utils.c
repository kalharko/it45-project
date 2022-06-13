#include "utils.h"
#include <stdbool.h>
#include <assert.h>

solution_t empty_solution(size_t n_assignments) {
    solution_t res;
    res.n_assignments = n_assignments;

    if (n_assignments > 0) {
        res.assignments = (size_t*)malloc(sizeof(size_t) * n_assignments);

        assert(res.assignments != NULL);
    } else {
        res.assignments = NULL;
    }

    for (size_t n = 0; n < n_assignments; n++) {
        res.assignments[n] = SIZE_MAX;
    }

    res.score = 0.0;

    return res;
}

void free_solution(solution_t solution) {
    if (solution.assignments) free(solution.assignments);
}



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
                // Insert if mission[n].start_time <= mission[n+1].start_time
                if (mission->start_time <= problem->missions[assignments_today[n]].start_time) {
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
                assignments_today[res.lengths[day]] = i;
            }
            res.lengths[day]++;
        }
    }

    // Resize arrays to save on memory (remove this if we need to mutate timetable_t in hot-path code)
    for (size_t i = 0; i < N_DAYS; i++) {
        if (res.lengths[i] > 0) {
            res.assignments[i] = (size_t*)realloc(res.assignments[i], res.lengths[i] * sizeof(size_t));

            assert(res.assignments[i] != NULL);
        } else {
            free(res.assignments[i]);
            res.assignments[i] = NULL;
        }
    }

    return res;
}

void free_time_table(timetable_t time_table) {
    for (size_t i = 0; i < N_DAYS; i++) {
        if (time_table.assignments[i] != NULL) {
            free(time_table.assignments[i]);
        }
    }
}
