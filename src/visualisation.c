#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "visualisation.h"
#include "score.h"
#include "defs.h"
#include "utils.h"
#include "constraints.h"


void print_solution(const solution_t* solution, const problem_t* problem) {
    // Gather information
    float wasted_time = 0;
    float overtime = 0;
    for (int i=0; i<problem->n_agents; i++) {
        timetable_t time_table = build_time_table(solution, problem, i);
        wasted_time += time_table_waisted_time(&time_table, problem);
        overtime += time_table_extra_hours(&time_table, problem);
        free_time_table(time_table);
    }

    // Display
    printf("\n\nSolution\n");
    printf("n_assignments\t\t : %zu\n[", solution->n_assignments);
    printf("assignments\t\t :");
    for (int i=0; i<solution->n_assignments; i++) {
        printf("%zu, ", solution->assignments[i]);
    }
    printf("]\n");
    printf("distance_traveled : \t%f\n", solution->distance_traveled);
    printf("Speciality miss match : \t%f\n", score_speciality(solution, problem));
    printf("Wasted time : \t\t%f\n", wasted_time);
    printf("Overtime : \t\t%f\n", overtime);
}

FILE* log_file = NULL;
void log_for_graph(const solution_t* solution, const problem_t* problem)
{
    float f_employees = score_harmony(solution, problem);
    float f_students = score_speciality(solution, problem);
    float f_SESSAD = score_SESSAD(solution, problem);

    fprintf(log_file, "%.3f, %.3f, %.3f\n", f_employees, f_students, f_SESSAD);
}

void log_assignments(const solution_t* solution, const char* path, bool append) {
    FILE* file;
    if (append) {
        file = fopen(path, "a");
    } else {
        file = fopen(path, "w");
    }

    assert(file != NULL);

    bool first = true;
    for (size_t n = 0; n < solution->n_assignments; n++) {
        if (first) first = false;
        else fprintf(file, ",");
        fprintf(file, "%zu", solution->assignments[n]);
    }
    fprintf(file, "\n");

    fclose(file);
}
