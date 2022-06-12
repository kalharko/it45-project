#include <stdlib.h>
#include <stdbool.h>

#include "constraints.h"
#include "utils.h"
#include "defs.h"



bool has_matching_skills(const timetable_t* time_table, const problem_t* problem) {
    for (size_t day = 0; day < N_DAYS; day++) {
        for (size_t j = 0; j < time_table->lengths[day]; j++) {
            skill_t agent_skill = problem->agents[time_table->agent].skill;
            skill_t mission_skill = problem->missions[time_table->assignments[day][j]].skill;
            if (agent_skill != mission_skill) return false;
        }
    }
    return true;
}



float time_table_distance(const timetable_t* time_table, const problem_t* problem, size_t day)
{
    float total_distance = 0;
    float distance;
    float time;
    float speed = 833.333; //833.333 m/min = 50km/h
    size_t* assignments = time_table->assignments[day];

    if (time_table->lengths[day] == 0) { //no assignments that day
        return 0;
    }

    // travel distance between the SESSAD and first mission
    total_distance = problem->distances[0][assignments[0]];

    for (int a = 0; a < time_table->lengths[day] - 1; a++) {
        distance = problem->distances[assignments[a]][assignments[a+1]];

        total_distance += distance;
        time = problem->missions[assignments[a+1]].start_time;
        time -= problem->missions[assignments[a]].end_time;

        if (!(distance / speed <= time)) {
            return -1;
        }
    }

    // travel distance between the last mission and SESSAD
    total_distance += problem->distances[assignments[time_table->lengths[day]-1]][0];

    return total_distance;
}
