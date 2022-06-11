#ifndef DEFS_H
#define DEFS_H

#include <stdlib.h>
#include <stdint.h>

enum skill {
    LSF = 0,
    LPC = 1
} typedef skill_t;

enum speciality {
    JARDINAGE,
    MECANIQUE,
    MENUISERIE,
    ELECTRICITE,
    MUSIQUE
} typedef speciality_t;

struct agent {
    uint64_t id;
    skill_t skill; // LSF = 0, LPC = 1
    speciality_t speciality;
    uint32_t hours;
} typedef agent_t;

struct mission {
    uint64_t id;

    uint32_t day;
    uint16_t start_time; // in minutes
    uint16_t end_time; // in minutes

    skill_t skill;
    speciality_t speciality;
} typedef mission_t;

struct problem {
    agent_t* agents;
    size_t n_agents;

    mission_t* missions;
    size_t n_missions;

    float **distances;

    //Recuit parameters
    double temperature;
    double temperature_mult;
    double temperature_threshold;

    //Different objectives management
    int current_objective;
    float *validated_scores; //stores the scores for the objectives that must not change when
                             //optimizing the next ones
} typedef problem_t;

struct solution {
    size_t* assignments; // assignements[i] == j <=> agent[j] goes to mission[i]
    size_t n_assignments;

    // Information used by the different algorithms
    float score;
    float distance_traveled;
} typedef solution_t;

#endif // DEFS_H
