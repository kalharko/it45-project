#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "defs.h"
#include "load.h"
#include "initial.h"
#include "optimize.h"

int main(int argc, char **argv) {
    char path[128] = "../Instances/45-4/"; // will be replaced by argument
    int n_agents = 4;
    int n_missions = 45;
    double temperature = 5;
    double temperature_mult = 0.95;
    double temperature_threshold = 0.15;

    // Initializes random number generator */
    srand(time(NULL));


    // // Initialize the problem and load data from csv
    problem_t problem;
    problem.n_agents = n_agents;
    problem.n_missions = n_missions;
    problem.temperature = temperature;
    problem.temperature_mult = temperature_mult;
    problem.temperature_threshold = temperature_threshold;

    // Distance
    float **distances;
    distances = (float **)malloc(problem.n_missions+1 * sizeof(float *));
    for (int i = 0; i < problem.n_missions+1; ++i){
        distances[i] = (float *)malloc(problem.n_missions+1 * sizeof(float));
    }
    load_distances(problem.n_missions+1, strcat(path, "Distances.csv"), distances);
    problem.distances = distances;

    // Agent
    agent_t agents[n_agents];
    load_agents(strcat(path, "Intervenants.csv"), agents, n_agents);
    problem.agents = agents;

    // Missions
    mission_t missions[n_missions];
    load_missions(strcat(path, "Intervenants.csv"), missions, n_missions);
    problem.missions = missions;


    // // Initial solution



    // // Launch optimization
    // problem.validated_scores = maloc toussa toussa à 3 et initialisé à 0
    // optimises for each objective
    // solution_t solution = initial_solution;
    // for (int i = 0; i < 3; i++) {
    //     problem.current_objective = i;
    //     problem.temperature = temperature;
    //     solution = optimize_solution(solution_t solution, problem_t* problem)
    //     problem.validated_scores[i] = solution.score;
    // }


    // // Display and save result to file



    return 0;
}
