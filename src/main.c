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

#ifdef unix
#include <unistd.h>
#endif

int main(int argc, char **argv) {
    char path[128] = "../Instances/100-10/"; // will be replaced by argument
    char concat_path[128];
    int n_agents = 10;
    int n_missions = 100;
    double temperature = 10;
    double temperature_mult = 0.99;
    double temperature_threshold = 0.15;

    // Initializes random number generator */
    #ifdef unix
    srand(time(NULL) ^ getpid());
    #else
    srand(time(NULL));
    #endif


    // // Initialize the problem and load data from csv
    problem_t problem;
    problem.n_agents = n_agents;
    problem.n_missions = n_missions;
    problem.current_objective = 0;
    problem.temperature = temperature;
    problem.temperature_mult = temperature_mult;
    problem.temperature_threshold = temperature_threshold;

    float validated_scores[3] = {0,0,0};
    problem.validated_scores = validated_scores;

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

    // Agent
    agent_t agents[n_agents];
    strcpy(concat_path, path);
    strcat(concat_path, "Intervenants.csv");
    load_agents(concat_path, agents, n_agents);
    problem.agents = agents;

    // // Missions
    mission_t missions[n_missions];
    strcpy(concat_path, path);
    strcat(concat_path, "Missions.csv");
    load_missions(concat_path, missions, n_missions);
    problem.missions = missions;


    // // Initial solution
    initial_params_t initial_params;
    initial_params.population = 200;
    initial_params.rounds = 1000;
    initial_params.survival_rate = 0.5;
    initial_params.reproduction_rate = 0.25;
    initial_params.mutation_rate = 0.25;
    initial_params.unassigned_penalty = 30.0; // how many minutes an unassigned mission is worth
    solution_t initial_solution = build_initial_solution(&problem, initial_params);
    score_solution(&initial_solution, &problem);
    printf("\nSolution initiale :\n");
    print_solution(initial_solution);

    if (initial_solution.score < 0.0) return 127;


    // // Launch optimization
    // optimises for each objective
    solution_t solution = initial_solution;
    for (int i = 0; i < 3; i++) {
        printf("\nOptimize objective %d\n", i);
        problem.current_objective = i;
        problem.temperature = temperature;
        solution = optimize_solution(solution, &problem);
        problem.validated_scores[i] = solution.score;
    }

    // // Display and save result to file
    printf("\nFinal solution\n");
    print_solution(solution);

    printf("Objective 1 : nb of speciality miss match :\n\t%f\n", problem.validated_scores[0]);
    printf("Objective 2 : total distance traveled\n\t%f\n", problem.validated_scores[1]);
    printf("Objective 3 : overtime\n\t%f\n", problem.validated_scores[2]);

    return 0;
}
