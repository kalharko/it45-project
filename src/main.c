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

    // Initializes random number generator */
    srand((unsigned) time(&t));

    // // Open data from csv files
    // Distance
    float **distance_csv;
    distance_csv = (float **)malloc(45 * sizeof(float *));
    for (int i = 0; i < 45; ++i){
        distance_csv[i] = (float *)malloc(45 * sizeof(float));
    }
    load_distance(45, 45, strcat(path, "Distances.csv"), distance_csv);

    // Agent
    agent_t agents[nb_agent];
    load_agents(strcat(path, "Intervenants.csv"), agents, nb_agent);

    // Missions
    mission_t missions[nb_mission];
    load_missions(strcat(path, "Intervenants.csv"), missions, nb_mission);



    return 0;
}
