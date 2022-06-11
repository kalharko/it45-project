#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "defs.h"
#include "load.h"
#include "initial.h"
#include "optimize.h"

int main(int argc, char **argv) {
    char path[128] = "../Instances/45-4/"; // will be replaced by argument
    int nb_agent = 4;
    int nb_mission = 45;
    double temperature = 5;
    double temperature_multiplicator = 0.95;
    double temperature_threashold = 0.15;

    // Initializes random number generator */
    srand((unsigned) time(&t));


    // // Initialize the problem and load data from csv
    problem_t problem;
    problem.n_agents = n_agents;
    problem.n_missions = n_missions;
    problem.temperature = temperature;
    problem.temperature_multiplicator = temperature_multiplicator;
    problem.temperature_threashold = temperature_threashold;

    // Distance
    float **distance;
    distance = (float **)malloc(problem.n_missions+1 * sizeof(float *));
    for (int i = 0; i < problem.n_missions+1; ++i){
        distance_csv[i] = (float *)malloc(problem.n_missions+1 * sizeof(float));
    }
    load_distance(problem.n_missions+1, strcat(path, "Distances.csv"), distance);
    problem.distance = distance;

    // Agent
    agent_t agents[nb_agent];
    load_agents(strcat(path, "Intervenants.csv"), agents, nb_agent);
    problem.agents = agents;

    // Missions
    mission_t missions[nb_mission];
    load_missions(strcat(path, "Intervenants.csv"), missions, nb_mission);
    problem.missions = missions;


    // // Initial solution



    // // Launch optimization



    // // Display and save result to file



    return 0;
}
