#include <stdlib.h>
#include <math.h>

#include "optimize.h"
#include "utils.h"
#include "score.h"
#include "constraints.h"


float score_solution(solution_t* solution, const problem_t* problem) {
    float score = 0;
    int alpha = 100/ problem->n_missions; // alpha as described in the subject

    switch (problem->current_objective) {
        case 0 : // minimize the number of affectation with the wrong specialty
            score = score_harmony(solution, problem);
        break;

        case 1 : // minimize the total distance traveled
            if (score_harmony(solution, problem) > problem->validated_scores[0]) {
                solution->score = -1;
                return -1;
            }
            score = score_speciality(solution, problem) * alpha;
        break;

        case 2 : // minimize the overtime
            if (score_harmony(solution, problem) > problem->validated_scores[0] ||
                score_speciality(solution, problem) * alpha > problem->validated_scores[1]
            ) {
                solution->score = -1;
                return -1;
            }
            score = score_SESSAD(solution, problem);
        break;

        default :
        break;
    }

    //printf("%d \t %f\n", problem->current_objective, score);
    solution->score = score;
    return score;
}


// returns the number of assignments where the agent and mission specialty do not match
float score_speciality(const solution_t* solution, const problem_t* problem) {
    float score = 0;

    for (int i=0; i<solution->n_assignments; i++) {
        size_t assignment = solution->assignments[i];
        if (assignment > SIZE_MAX) continue;

        if (problem->missions[assignment].speciality != problem->agents[assignment].speciality) {
            score += 1.0;
        }
    }

    return score;
}


// returns the total distance traveled by the agents
float score_distance(solution_t* solution, const problem_t* problem) {
    // should be already calculated by the is_solution_valid() function
    if (solution->distance_traveled != 0) {
        return solution->distance_traveled;
    } else {
        is_solution_valid(solution, problem);
        return solution->distance_traveled;
    }
}


float score_overtime(solution_t* solution, const problem_t* problem) {
    return 0;
}


float score_harmony(const solution_t* solution, const problem_t* problem) {
    // // Waisted hours
    float waisted_hours[problem->n_agents];
    timetable_t time_table;
    // gather waisted hours
    for (int i=0; i<problem->n_agents; i++) {
        time_table = build_time_table(solution, problem, i);
        waisted_hours[i] = time_table_waisted_time(&time_table, problem);
    }

    // mean
    float waisted_hours_mean = 0;
    for (int i=0; i<problem->n_agents; i++) {
        waisted_hours_mean += waisted_hours[i];
    }
    waisted_hours_mean /= problem->n_agents;

    // standard deviation
    float waisted_hours_sd = 0;
    for (int i=0; i<problem->n_agents; i++) {
        waisted_hours_sd += pow(waisted_hours_mean - waisted_hours[i], 2);
    }
    waisted_hours_sd /= problem->n_agents;
    waisted_hours_sd = sqrt(waisted_hours_sd);


    // // Overtime hours
    float overtime_hours[problem->n_agents];
    // gather waisted hours
    for (int i=0; i<problem->n_agents; i++) {
        time_table = build_time_table(solution, problem, i);
        overtime_hours[i] = time_table_extra_hours(&time_table, problem);
    }

    // mean
    float overtime_mean = 0;
    for (int i=0; i<problem->n_agents; i++) {
        overtime_mean += overtime_hours[i];
    }
    overtime_mean /= problem->n_agents;

    // standard deviation
    float overtime_sd = 0;
    for (int i=0; i<problem->n_agents; i++) {
        overtime_sd += pow(overtime_mean - overtime_hours[i], 2);
    }
    overtime_sd /= problem->n_agents;
    overtime_sd = sqrt(overtime_sd);


    // // Distance
    float distances[problem->n_agents];
    // gather distance
    for (int i=0; i<problem->n_agents; i++) {
        time_table = build_time_table(solution, problem, i);
        distances[i] = 0;
        for (int day=0; day<N_DAYS; day++) {
            distances[i] += time_table_distance(&time_table, problem, day);
        }
    }

    // mean
    float distance_mean = 0;
    for (int i=0; i<problem->n_agents; i++) {
        distance_mean += distances[i];
    }
    distance_mean /= problem->n_agents;

    // standard deviation
    float distance_sd = 0;
    for (int i=0; i<problem->n_agents; i++) {
        distance_sd += pow(distance_mean - distances[i], 2);
    }
    distance_sd /= problem->n_agents;
    distance_sd = sqrt(distance_sd);

    // // regime mean
    float regime_mean = 0;
    for (int i=0; i<problem->n_agents; i++) {
        regime_mean += problem->agents[i].hours;
    }
    regime_mean /= problem->n_agents;

    // // final calcul
    float out;

    float zeta = 100 / regime_mean;
    float lambda = 100 / 10;        // 10 is number of tolerated overtime per week
    float kapa = 100 / kapa_distance(problem);

    out += zeta * waisted_hours_sd;
    out += lambda * overtime_sd;
    out += kapa * distance_sd;
    out /= 3;

    return out;
}


float score_SESSAD(const solution_t* solution, const problem_t* problem)
{

    // // sumWHO
    float sumWHO = 0;
    timetable_t time_table;
    // gather waisted hours
    for (int i=0; i<problem->n_agents; i++) {
        time_table = build_time_table(solution, problem, i);
        sumWHO += time_table_waisted_time(&time_table, problem);
        sumWHO += time_table_extra_hours(&time_table, problem);
    }

    // // Distance
    float distances[problem->n_agents];
    float distance_max = 0;
    // gather distance
    for (int i=0; i<problem->n_agents; i++) {
        time_table = build_time_table(solution, problem, i);
        distances[i] = 0;
        for (int day=0; day<N_DAYS; day++) {
            distances[i] += time_table_distance(&time_table, problem, day);
        }
        if (distances[i] > distance_max) {
            distance_max = distances[i];
        }
    }

    // mean
    float distance_mean = 0;
    for (int i=0; i<problem->n_agents; i++) {
        distance_mean += distances[i];
    }
    distance_mean /= problem->n_agents;


    float out;

    float beta = 100 / 45; // 45 = max nb of work hours in a week
    float kapa = 100 / kapa_distance(problem);

    out = beta * sumWHO;
    out += kapa * distance_mean;
    out += kapa * distance_max;
    out /= 3;

    return out;

}
