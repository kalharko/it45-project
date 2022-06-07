#ifndef LOAD_H
#define LOAD_H

#include <stddef.h>
#include "defs.h"

int load_distance(size_t row, size_t col, char *filename, float **data);

int load_agents(char *filename, agent_t agents[], int nb_agent);

int load_missions(char *filename, mission_t missions[], int nb_mission);

#endif // LOAD_H
