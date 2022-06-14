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

void problem_set_random_distances(problem_t* problem, float max_dist) {
    float* coordinates = malloc(sizeof(float) * 2 * problem->n_missions);

    for (size_t n = 0; n < problem->n_missions; n++) {
        // Rejection sampling :)
        float x = 0.0, y = 0.0;
        do {
            x = ((float)rand() * 2.0 / (float)RAND_MAX - 1.0) * max_dist;
            y = ((float)rand() * 2.0 / (float)RAND_MAX - 1.0) * max_dist;
        } while (x * x + y * y > max_dist * max_dist);
        coordinates[n * 2] = y;
        coordinates[n * 2 + 1] = x;
    }

    problem->distances = malloc(sizeof(float*) * problem->n_missions);

    for (size_t y = 0; y < problem->n_missions; y++) {
        problem->distances[y] = malloc(sizeof(float) * problem->n_missions);

        for (size_t x = 0; x < problem->n_missions; x++) {
            float dy = coordinates[x * 2] - coordinates[y * 2];
            float dx = coordinates[x * 2 + 1] - coordinates[y * 2 + 1];

            problem->distances[y][x] = sqrt(dx * dx + dy * dy);
        }
    }

    problem->sessad_distances = malloc(sizeof(float) * problem->n_missions);

    for (size_t x = 0; x < problem->n_missions; x++) {
        float dy = coordinates[x * 2];
        float dx = coordinates[x * 2 + 1];
        problem->sessad_distances[x] = sqrt(dx * dx + dy * dy);
    }

    free(coordinates);
}

void problem_shuffle_missions(problem_t* problem) {
    for (size_t n = 0; n < problem->n_missions - 1; n++) {
        size_t o = n + rand() % (problem->n_missions - 1 - n) + 1;
        mission_t tmp = problem->missions[n];
        problem->missions[n] = problem->missions[o];
        problem->missions[o] = tmp;
    }
}
