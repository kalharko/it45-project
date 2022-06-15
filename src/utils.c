#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

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
        .sessad_distances = NULL,
        .coordinates = NULL
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
    if (problem.coordinates != NULL) free(problem.coordinates);
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

void write_problem(const problem_t* problem, const char* path) {
    const char* SKILLS[] = {
        "LSF",
        "LPC"
    };
    const char* SPECIALITIES[] = {
        "Jardinage",
        "Mecanique",
        "Menuiserie",
        "Electricite",
        "Musique"
    };

    size_t len = strlen(path);
    char* buffer = malloc(sizeof(char) * (len + 32));

    // Create directory if it isn't present
    sprintf(buffer, "mkdir -p \"%s\"", path); // Vulnerable to CWE 78
    if (system(buffer)) {
        free(buffer);
        fprintf(stderr, "Couldn't create directory %s!", path);
        return;
    }

    strcpy(buffer, path);
    if (buffer[len - 1] != '/') {
        buffer[len] = '/';
        buffer[len + 1] = 0;
    }

    char* tail = &buffer[len + 1];

    if (problem->distances && problem->sessad_distances) {
        strcpy(tail, "Distances.csv");
        FILE* distances = fopen(buffer, "w");
        assert(distances != NULL);
        fprintf(distances, "0.0"); // SESSAD->SESSAD

        for (size_t n = 0; n < problem->n_missions; n++) {
            fprintf(distances, ",%f", problem->sessad_distances[n]);
        }
        fprintf(distances, "\n");

        for (size_t i = 0; i < problem->n_missions; i++) {
            fprintf(distances, "%f", problem->sessad_distances[i]);
            for (size_t j = 0; j < problem->n_missions; j++) {
                fprintf(distances, ",%f", problem->distances[i][j]);
            }
            fprintf(distances, "\n");
        }
        fclose(distances);
    }

    if (problem->coordinates) {
        strcpy(tail, "Coordinates.csv");
        FILE* coordinates = fopen(buffer, "w");
        assert(coordinates != NULL);

        for (size_t n = 0; n < problem->n_missions; n++) {
            fprintf(coordinates, "%f,%f\n", problem->coordinates[2 * n + 1], problem->coordinates[2 * n]);
        }

        fclose(coordinates);
    }

    strcpy(tail, "Intervenants.csv");
    FILE* agents = fopen(buffer, "w");
    assert(agents != NULL);

    for (size_t n = 0; n < problem->n_agents; n++) {
        fprintf(agents,
            "%" PRIu64 ",%s,%s,%" PRIu32 "\n",
            problem->agents[n].id,
            SKILLS[problem->agents[n].skill],
            SPECIALITIES[problem->agents[n].speciality],
            problem->agents[n].hours
        );
    }

    fclose(agents);

    strcpy(tail, "Missions.csv");
    FILE* missions = fopen(buffer, "w");
    assert(missions != NULL);

    for (size_t n = 0; n < problem->n_agents; n++) {
        fprintf(missions,
            "%" PRIu64 ",%" PRIu8 ",%" PRIu16 ",%" PRIu16 ",%s,%s\n",
            problem->missions[n].id,
            problem->missions[n].day,
            problem->missions[n].start_time,
            problem->missions[n].end_time,
            SKILLS[problem->missions[n].skill],
            SPECIALITIES[problem->missions[n].speciality]
        );
    }

    fclose(missions);

    free(buffer);
}

void problem_set_random_distances(problem_t* problem, float max_dist) {
    float* coordinates = malloc(sizeof(float) * 2 * problem->n_missions);

    for (size_t n = 0; n < problem->n_missions; n++) {
        // Rejection sampling :)
        float x = 0.0, y = 0.0;
        do {
            x = ((float)rand() * 2.0 / (float)RAND_MAX - 1.0) * max_dist;
            y = ((float)rand() * 2.0 / (float)RAND_MAX - 1.0) * max_dist;
        } while (x * x + y * y > max_dist * max_dist);
        coordinates[n * 2] = y;
        coordinates[n * 2 + 1] = x;
    }

    problem->distances = malloc(sizeof(float*) * problem->n_missions);

    for (size_t y = 0; y < problem->n_missions; y++) {
        problem->distances[y] = malloc(sizeof(float) * problem->n_missions);

        for (size_t x = 0; x < problem->n_missions; x++) {
            float dy = coordinates[x * 2] - coordinates[y * 2];
            float dx = coordinates[x * 2 + 1] - coordinates[y * 2 + 1];

            problem->distances[y][x] = sqrt(dx * dx + dy * dy);
        }
    }

    problem->sessad_distances = malloc(sizeof(float) * problem->n_missions);

    for (size_t x = 0; x < problem->n_missions; x++) {
        float dy = coordinates[x * 2];
        float dx = coordinates[x * 2 + 1];
        problem->sessad_distances[x] = sqrt(dx * dx + dy * dy);
    }

    problem->coordinates = coordinates;
}
