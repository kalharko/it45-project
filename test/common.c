#include <defs.h>
#include <stdlib.h>

problem_t empty_problem() {
    problem_t res = {
        .agents = NULL,
        .n_agents = 0,
        .missions = NULL,
        .n_missions = 0
    };
    return res;
}

void free_problem(problem_t problem) {
    free(problem.agents);
    free(problem.missions);
}

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

    if (!problem->missions) {
        TEST_FAIL_MESSAGE("Couldn't realloc problem.missions");
    }

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
    if (!problem->agents) {
        TEST_FAIL_MESSAGE("Couldn't realloc problem.agents");
    }

    agent_t agent = {
        .id = id,
        .skill = skill,
        .speciality = speciality,
        .hours = hours
    };

    problem->agents[problem->n_agents - 1] = agent;
}
