#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

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


bool has_lunch_break(const timetable_t* time_table, const problem_t* problem)
{
    for (int day = 1; day < N_DAYS; day++)
    {
        size_t* assignments = time_table->assignments[day];
        // if more than one assignments and an assignment before and after 13h
        if (time_table->lengths[day] > 1
            && problem->missions[assignments[0]].end_time <= 13 * 60
            && problem->missions[assignments[time_table->lengths[day]-1]].start_time >= 13 * 60
        ) {

            // search first assignment after 13h
            int a = 1;
            while (problem->missions[assignments[a]].end_time < 780 && a < time_table->lengths[day]) {
                a++;
            }

            if (problem->missions[assignments[a]].start_time - problem->missions[assignments[a-1]].end_time < 60) {
                return false;
            }

        }
    }

    return true;
}

float calc_day_work_time(const timetable_t* time_table, const problem_t* problem, size_t day) {
    if (time_table->lengths[day] == 0) return 0.0;

    // TODO: make constant for speed
    float speed = 50.0 * 1000.0 / 60.0; // 833.333 m/min = 50km/h

    size_t* assignments = time_table->assignments[day];
    float res = 0;
    // travel distance between SESSAD and first mission
    float distance = problem->distances[0][assignments[0]];

    // distances between missions
    for (size_t a = 0; a + 1 < time_table->lengths[day]; a++) {
        distance += problem->distances[assignments[a]][assignments[a+1]];
    }

    // time of missions
    for (size_t a = 0; a < time_table->lengths[day]; a++) {
        res += problem->missions[assignments[a]].end_time;
        res -= problem->missions[assignments[a]].start_time;
    }

    // travel distance between the last mission and SESSAD
    distance += problem->distances[assignments[time_table->lengths[day]-1]][0];
    // add travel time
    res += distance / speed;

    return res;
}

float time_table_extra_hours(const timetable_t* time_table, const problem_t* problem)
{
    float total_work_time = 0;
    float extra_work_time = 0;

    for (int day = 0; day < N_DAYS; day++)
    {
        size_t* assignments = time_table->assignments[day];
        float day_work_time = calc_day_work_time(time_table, problem, day);

        // check day work time
        if (day_work_time > 10 * 60) { // > 10h
            return -1;
        }
        else if (day_work_time > 8 * 60) { // >8h
            extra_work_time += day_work_time - 8 * 60;
        }

        total_work_time += day_work_time;
    }

    // check week work time against the maximum legal work time
    if (total_work_time > 48 * 60) { // >48h
        return -1;
    }

    return extra_work_time;
}



float time_table_distance(const timetable_t* time_table, const problem_t* problem, size_t day) {
    float total_distance = 0;
    float distance;
    float time;
    float speed = 50.0 * 1000.0 / 60.0; //833.333 m/min = 50km/h

    assert(day < N_DAYS);
    size_t* assignments = time_table->assignments[day];

    if (time_table->lengths[day] == 0) { //no assignments that day
        return 0;
    }

    // travel distance between the SESSAD and first mission
    assert(assignments[0] < problem->n_missions);
    assert(problem->distances != NULL);
    assert(problem->sessad_distances != NULL);
    total_distance = problem->sessad_distances[assignments[0]];

    for (int a = 0; a < time_table->lengths[day] - 1; a++) {
        distance = problem->distances[assignments[a]][assignments[a+1]];

        total_distance += distance;
        time = problem->missions[assignments[a+1]].start_time;
        time -= problem->missions[assignments[a]].end_time;

        if (!(distance / speed <= time)) {
            return distance / speed - time;
        }
    }

    // travel distance between the last mission and SESSAD
    total_distance += problem->sessad_distances[assignments[time_table->lengths[day]-1]];

    return total_distance;
}
