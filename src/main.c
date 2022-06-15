#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "defs.h"
#include "load.h"
#include "initial.h"
#include "optimize.h"
#include "utils.h"
#include "score.h"
#include "visualisation.h"

#ifdef unix
#include <unistd.h>
#endif

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage : ./it45-project <path to folder data> <nb of iteration> <cut off>\n");
        return 1;
    }

    char path[128];
    strcpy(path, argv[1]);
    printf("\n%s\n", path);
    if (check_path(path, true) == false) {
        return 1;
    }
    char concat_path[128];

    int n_iterations = atoi(argv[2]);
    int cut_off = atoi(argv[3]);

    // open file for logging
    log_file = fopen("../log.csv", "w");

    double temperature = 10; //20
    double temperature_mult = 0.99; //0.995
    double temperature_threshold = 0.15; //4

    // Initializes random number generator
    #ifdef unix
    srand(time(NULL) ^ getpid());
    #else
    srand(time(NULL));
    #endif


    // // Initialize the problem and load data from csv
    problem_t problem = empty_problem();
    problem.current_objective = 0;
    problem.temperature = temperature;
    problem.temperature_mult = temperature_mult;
    problem.temperature_threshold = temperature_threshold;

    float validated_scores[3] = {0,0,0};
    problem.validated_scores = validated_scores;

    // Agent
    strcpy(concat_path, path);
    strcat(concat_path, "Intervenants.csv");
    problem.n_agents = get_file_line_count(concat_path);
    agent_t* agents = (agent_t*)malloc(sizeof(agent_t) * problem.n_agents);
    load_agents(concat_path, agents, problem.n_agents);
    problem.agents = agents;

    // // Missions
    strcpy(concat_path, path);
    strcat(concat_path, "Missions.csv");
    problem.n_missions = get_file_line_count(concat_path);
    mission_t* missions = (mission_t*)malloc(sizeof(mission_t) * problem.n_missions);
    load_missions(concat_path, missions, problem.n_missions);
    problem.missions = missions;

    // Distance
    float** distances = malloc(sizeof(float*) * (problem.n_missions + 1));

    for (size_t n = 0; n < problem.n_missions+1; n++) {
        distances[n] = malloc(sizeof(float) * (problem.n_missions + 1));
        for (size_t o = 0; o < problem.n_missions+1; o++) {
            distances[n][o] = 0.0;
        }
    }
    strcpy(concat_path, path);
    strcat(concat_path, "Distances.csv");
    load_distances(problem.n_missions+1, concat_path, distances);
    load_distances_into_problem(&problem, distances);
    for (size_t n = 0; n < problem.n_missions+1; n++) {
        free(distances[n]);
    }
    free(distances);
    distances = NULL;

    // // Initial solution
    initial_params_t initial_params;
    initial_params.population = 200;
    initial_params.rounds = 1000;
    initial_params.survival_rate = 0.5;
    initial_params.reproduction_rate = 0.25;
    initial_params.mutation_rate = 0.25;
    initial_params.unassigned_penalty = 30.0; // how many minutes an unassigned mission is worth

    solution_t final_solution = empty_solution(problem.n_missions);
    float final_employees = 1000000;
    float final_students = 1000000;
    float final_SESSAD = 1000000;
    for (int iteration=0; iteration<n_iterations; iteration++) {
        printf("\n\n====== Iteration %d ======\n", iteration);

        solution_t initial_solution = build_initial_solution(&problem, initial_params);
        score_solution(&initial_solution, &problem);
        //printf("\nSolution initiale :");
        //print_solution(&initial_solution, &problem);

        if (initial_solution.score < 0.0) {
            fprintf(stderr, "No valid initial solution found!\n");
            return 127;
        }


        // // Launch optimization
        // optimises for each objective
        solution_t solution = initial_solution;
        for (int i = 0; i < 3; i++) {
            printf("\nOptimize objective %d", i);
            problem.current_objective = i;
            problem.temperature = temperature;
            solution = optimize_solution(solution, &problem, cut_off);
            problem.validated_scores[i] = solution.score;
        }

        // // check if it is the best solution encountered yet
        if (problem.validated_scores[0] < final_employees) {
            final_employees = problem.validated_scores[0];
            final_solution.distance_traveled = solution.distance_traveled;
            final_solution.assignments = solution.assignments;
            printf("  Solution améliorée, f_employees : %f\n", final_employees);
            print_solution(&solution, &problem);
        }

        // // Reset for next iteration
        if (iteration+1 < n_iterations) {
            fprintf(log_file, "===========\n");
            for (int j=0; j<3; j++) {
            problem.validated_scores[j] = 0;
            }
        }
        problem.current_objective = 0;
        problem.temperature = temperature;
    }

    // // Display
    printf("\n\n===== Final solution =====\n");
    print_solution(&final_solution, &problem);

    printf("\nObjectifs :\n");
    printf("f_employees :\t\t%f\n", problem.validated_scores[0]);
    printf("f_students :\t\t%f\n", problem.validated_scores[1]);
    printf("f_SESSAD :\t\t%f\n", problem.validated_scores[2]);

    // // Save agents time tables
    save_agents_EDT(&final_solution, &problem);

    // // Free Memory
    // problem_set_random_distances(&problem, 10000);
    // write_problem(&problem, "../Instances/with-pos");

    free_problem(problem);
    free_solution(final_solution);
    fclose(log_file);

    return 0;
}
