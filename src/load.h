#ifndef LOAD_H
#define LOAD_H

#include <stddef.h>
#include "defs.h"

int load_distances(size_t n_missions, char *filename, float **data);

void load_distances_into_problem(problem_t* problem, float** data);

int load_agents(char *filename, agent_t agents[], int nb_agent);

int load_missions(char *filename, mission_t missions[], int nb_mission);

#endif // LOAD_H
