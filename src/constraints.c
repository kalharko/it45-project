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


bool has_lunch_break(const timetable_t* time_table, const problem_t* problem)
{
    for (int day = 1; day < N_DAYS; day++)
    {
        size_t* assignments = time_table->assignments[day];
        // if more than one assignments and an assignment before and after 13h
        if (time_table->lengths[day] > 1
            && problem->missions[assignments[0]].end_time <= 780
            && problem->missions[assignments[time_table->lengths[day]-1]].start_time >= 780) {

            // search first assignment after 13h
            int a = 0;
            while (problem->missions[assignments[a]].end_time < 780) {
                a++;
            }

            if (problem->missions[assignments[a]].start_time - problem->missions[assignments[a-1]].end_time < 60) {
                return false;
            }

        }
    }

    return true;
}

float time_table_extra_hours(const int agent, const timetable_t* time_table, const problem_t* problem)
{
    float total_work_time = 0;
    float extra_work_time = 0;
    float day_work_time;
    float distance;
    float speed = 833.333; //833.333 m/min = 50km/h

    for (int day = 1; day < N_DAYS; day++)
    {
        size_t* assignments = time_table->assignments[day];
        day_work_time = 0;
        // travel distance between SESSAD and first mission
        distance = problem->distances[0][assignments[0]];

        // distances between missions and time of missions
        for (int a = 0; a < time_table->lengths[day] - 2; a++) {
            day_work_time += problem->missions[assignments[a]].end_time;
            day_work_time -= problem->missions[assignments[a]].start_time;
            distance += problem->distances[assignments[a]][assignments[a+1]];
        }
        day_work_time += problem->missions[assignments[time_table->lengths[day]-1]].end_time;
        day_work_time -= problem->missions[assignments[time_table->lengths[day]-1]].start_time;

        // travel distance between the last mission and SESSAD
        distance += problem->distances[assignments[time_table->lengths[day]-1]][0];
        // add travel time
        day_work_time += distance / speed;

        // check day work time
        if (day_work_time > 600) { // > 10h
            return -1;
        }
        else if (day_work_time >480) { // >8h
            extra_work_time += day_work_time - 480;
        }

        total_work_time += day_work_time;
    }

    // check week work time against the maximum legal work time
    if (total_work_time > 2880) { // >48h
        return -1;
    }

    return extra_work_time;
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
