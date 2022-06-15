#include <stdlib.h>
#include <stdio.h>
#include <string.h>


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


void save_agents_EDT(const solution_t* solution, const problem_t* problem)
{
    FILE* file = fopen("../edt.txt", "w");
    float speed = 50.0 * 1000.0 / 60.0; //833.333 m/min = 50km/h
    char jours[5][15] = {
        "lundi",
        "mardi",
        "mercredi",
        "jeudi",
        "vendredi"
    };

    timetable_t time_table;
    for (int a=0; a<problem->n_agents; a++)
    {
        time_table = build_time_table(solution, problem, a);
        for (int day=0; day<N_DAYS; day++) {
            for (int i=0; i<time_table.lengths[day]; i++) {
                int time = problem->missions[time_table.assignments[day][i]].start_time;
                time -= problem->sessad_distances[time_table.assignments[day][i]] / speed;
                fprintf(file, "TRAJ \t\t");
                fprintf(file, "TP 1 \t");
                fprintf(file, "%s \t", jours[day-1]);
                fprintf(file, "%d:", time/60);
                if (time%60 <10) {
                    fprintf(file, "0");
                }
                fprintf(file, "%d \t", time%60);
                fprintf(file, "%d:", problem->missions[time_table.assignments[day][i]].start_time/60);
                if (problem->missions[time_table.assignments[day][i]].start_time%60 <10) {
                    fprintf(file, "0");
                }
                fprintf(file, "%d \t", problem->missions[time_table.assignments[day][i]].start_time%60);
                fprintf(file, "1 \t");
                fprintf(file, "Distanciel \t\n");

                fprintf(file, "MISS \t\t");
                fprintf(file, "CM 1 \t");
                fprintf(file, "%s \t", jours[day-1]);
                fprintf(file, "%d:", problem->missions[time_table.assignments[day][i]].start_time/60);
                if (problem->missions[time_table.assignments[day][i]].start_time%60 <10) {
                    fprintf(file, "0");
                }
                fprintf(file, "%d \t", problem->missions[time_table.assignments[day][i]].start_time%60);
                fprintf(file, "%d:", problem->missions[time_table.assignments[day][i]].end_time/60);
                if (problem->missions[time_table.assignments[day][i]].end_time%60 <10) {
                    fprintf(file, "0");
                }
                fprintf(file, "%d \t", problem->missions[time_table.assignments[day][i]].end_time%60);
                fprintf(file, "1 \t");
                fprintf(file, "Distanciel \t\n");

                time = problem->missions[time_table.assignments[day][i]].end_time;
                time += problem->sessad_distances[time_table.assignments[day][i]] / speed;
                fprintf(file, "TRAJ \t\t");
                fprintf(file, "TP 1 \t");
                fprintf(file, "%s \t", jours[day-1]);
                fprintf(file, "%d:", problem->missions[time_table.assignments[day][i]].end_time/60);
                if (problem->missions[time_table.assignments[day][i]].end_time%60 <10) {
                    fprintf(file, "0");
                }
                fprintf(file, "%d \t", problem->missions[time_table.assignments[day][i]].end_time%60);
                fprintf(file, "%d:", time/60);
                if (time%60 <10) {
                    fprintf(file, "0");
                }
                fprintf(file, "%d \t", time%60);
                fprintf(file, "1 \t");
                fprintf(file, "Distanciel \t\n");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}
