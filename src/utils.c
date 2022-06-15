#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "load.h"
#include "constraints.h"
#include "score.h"

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
                assert(assignments_today[n] < problem->n_missions);

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

problem_t empty_problem() {
    problem_t res = {
        .agents = NULL,
        .n_agents = 0,
        .missions = NULL,
        .n_missions = 0,
        .distances = NULL,
        .sessad_distances = NULL
    };
    return res;
}

void free_problem(problem_t problem) {
    free(problem.agents);
    free(problem.missions);

    if (problem.distances != NULL) {
        for (size_t n = 0; n < problem.n_missions; n++) {
            free(problem.distances[n]);
        }
        free(problem.distances);
    }
    if (problem.sessad_distances != NULL) free(problem.sessad_distances);
}

bool check_path(char* path, bool verbose) {
    // Check path ends with '/'
    int i;
    for (i=0; path[i] != '\0'; i++) {
    }
    if (path[i-1] != '/') {
        path[i] = '/';
        path[i+1] = '\0';
    }

    // Check path validity
    char file_names[4][20] = {
        "Distances.csv",
        "Intervenants.csv",
        "Missions.csv"
    };
    char concat_path[128];
    bool tests[4];
    bool valid = true;
    FILE *file;


    for (i=0; i<3; i++) {
        strcpy(concat_path, path);
        strcat(concat_path, file_names[i]);
        file = fopen(concat_path, "r");
        if (file != NULL) {
            tests[i] = true;
            fclose(file);
        } else {
            tests[i] = false;
            valid = false;
        }
    }

    // Number of missions and matching distances
    tests[3] = false;
    if (valid) {
        strcpy(concat_path, path);
        strcat(concat_path, "Missions.csv");
        int n_missions = get_file_line_count(concat_path);
        strcpy(concat_path, path);
        strcat(concat_path, "Distances.csv");
        if (get_file_line_count(concat_path) == n_missions + 1) {
            tests[3] = true;
        }
        else {
            valid = false;
        }
    }

    // Report
    if (valid == false && verbose) {
        printf("Data structure invalid / file not found :\n");
        for (i=0; i<3; i++) {
            printf("%s : ", file_names[i]);
            if (tests[i]) {
                printf("PASS\n");
            }
            else {
                printf("FAIL / Missing\n");
            }
        }

        if (tests[3] == false) {
            printf("Miss match between number of missions and the distance matrix size\n");
        }
    }

    return valid;
}


float time_table_waisted_time(const timetable_t* time_table, const problem_t* problem)
{
    float distance;
    float wasted_time = 0;
    float time;
    float speed = 50.0 * 1000.0 / 60.0; //833.333 m/min = 50km/h

    for (size_t day = 0; day < N_DAYS; day++) {
        if (time_table->lengths[day] > 1) {
            size_t* assignments = time_table->assignments[day];
            for (size_t j = 0; j < time_table->lengths[day]-1; j++) {
                time = problem->missions[assignments[j]].end_time - problem->missions[assignments[j]].start_time;
                distance = problem->distances[j][j+1];
                wasted_time += time - distance / speed;
            }
        }
    }

    return wasted_time;
}

float kapa_distance(const problem_t* problem)
{
    float total = 0;
    for (int i=0; i<problem->n_missions; i++) {
        total += problem->sessad_distances[i];
    }
    return total / problem->n_missions;
}
