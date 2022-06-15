#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <defs.h>
#include <utils.h>
#include <optimize.h>
#include <initial.h>

struct bench {
    float avg; // In seconds
    float sigma; // Variance in seconds

    size_t n_agents;
    size_t n_missions;
} typedef bench_t;

void problem_push_mission(
    problem_t* problem,
    uint64_t id,
    skill_t skill,
    speciality_t speciality,
    uint32_t day,
    uint16_t start_time,
    uint16_t end_time
) {
    problem->missions = realloc(problem->missions, sizeof(agent_t) * ++problem->n_missions);
    assert(problem->missions != NULL);

    mission_t mission = {
        .id = id,
        .day = day,
        .start_time = start_time,
        .end_time = end_time,
        .skill = skill,
        .speciality = speciality
    };

    problem->missions[problem->n_missions - 1] = mission;
}

void problem_push_agent(
    problem_t* problem,
    uint64_t id,
    skill_t skill,
    speciality_t speciality,
    uint32_t hours
) {
    problem->agents = realloc(problem->agents, sizeof(agent_t) * ++problem->n_agents);
    assert(problem->agents != NULL);

    agent_t agent = {
        .id = id,
        .skill = skill,
        .speciality = speciality,
        .hours = hours
    };

    problem->agents[problem->n_agents - 1] = agent;
}

void problem_shuffle_missions(problem_t* problem) {
    for (size_t n = 0; n < problem->n_missions - 1; n++) {
        size_t o = n + rand() % (problem->n_missions - 1 - n) + 1;
        mission_t tmp = problem->missions[n];
        problem->missions[n] = problem->missions[o];
        problem->missions[o] = tmp;
    }
}
