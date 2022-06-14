#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "load.h"
#include "defs.h"

int load_distances(size_t n_missions, char *filename, float **data) {
    FILE *file;
    file = fopen(filename, "r");
    assert(file != NULL);

    int i = 0;
    char line[4098];
    while (fgets(line, 4098, file) && (i < n_missions))
    {
        // float row[ssParams->nreal + 1];
        char* tmp = strdup(line);

        int j = 0;
        const char* tok;
        for (tok = strtok(line, ","); tok && *tok && j < n_missions; j++, tok = strtok(NULL, ","))
        {
            data[i][j] = atof(tok);
            //printf("%0.1f\t", data[i][j]);
        }
        //printf("\n");

        free(tmp);
        i++;
    }

    fclose(file);
    return 0;
}

void load_distances_into_problem(problem_t* problem, float** data) {
    assert(problem != NULL);
    problem->distances = malloc(sizeof(float*) * problem->n_missions);
    assert(problem->distances != NULL);

    for (size_t y = 0; y < problem->n_missions; y++) {
        problem->distances[y] = malloc(sizeof(float) * problem->n_missions);
        assert(problem->distances[y] != NULL);

        for (size_t x = 0; x < problem->n_missions; x++) {
            problem->distances[y][x] = data[y + 1][x + 1];
        }
    }

    problem->sessad_distances = malloc(sizeof(float) * problem->n_missions);
    assert(problem->sessad_distances != NULL);

    for (size_t x = 0; x < problem->n_missions; x++) {
        problem->sessad_distances[x] = data[0][x + 1];
    }
}

int get_file_line_count(char *filename) {
    char line[4098];
    int n_missions = 0;
    FILE *file = fopen(filename, "r");
    assert(file != NULL);

    while (fgets(line, sizeof line, file)) {
        n_missions += 1;
    }

    fclose(file);

    return n_missions;
}

int load_agents(char *filename, agent_t agents[], int nb_agent) {
    FILE *file = fopen(filename, "r");

    assert(file != NULL);

    int i = 0, j, head;
    char buffer[50];
    char line[4098];
    while (fgets(line, sizeof line, file) && (i < nb_agent))
    {
        head = 0;

        // agents.id
        j = 0;
        while (line[head] != ',') {
            buffer[j] = line[head];
            j++;
            head++;
        }
        buffer[j] = '\0';
        head++;
        agents[i].id = atoi(buffer);

        // agents.skill
        j = 0;
        while (line[head] != ',') {
            buffer[j] = line[head];
            j++;
            head++;
        }
        buffer[j] = '\0';
        head++;
        if (strcmp(buffer, "LSF") == 0) {
            agents[i].skill = LSF;
        }
        else {
            agents[i].skill = LPC;
        }

        // agents.speciality
        j = 0;
        while (line[head] != ',') {
            buffer[j] = line[head];
            j++;
            head++;
        }
        buffer[j] = '\0';
        head++;
        if (strcmp(buffer, "Jardinage")==0) {
            agents[i].speciality = JARDINAGE;
        }
        else if(strcmp(buffer, "Mecanique")==0){
            agents[i].speciality = MECANIQUE;
        }
        else if(strcmp(buffer, "Menuiserie")==0){
            agents[i].speciality = MENUISERIE;
        }
        else if(strcmp(buffer, "Electricite")==0){
            agents[i].speciality = ELECTRICITE;
        }
        else if(strcmp(buffer, "Musique")==0){
            agents[i].speciality = MUSIQUE;
        }
        else {
            return 1;
        }

        // agents.hours
        j = 0;
        while (line[head] != '\n' && line[head] != '\r') {
            buffer[j] = line[head];
            j++;
            head++;
        }
        buffer[j] = '\0';
        head++;
        agents[i].hours = atoi(buffer);

        i++;
    }

    fclose(file);
    return 0;
}


int load_missions(char *filename, mission_t missions[], int nb_mission)
{
    FILE *file = fopen(filename, "r");
    assert(file != NULL);

    int i = 0, j, head;
    char buffer[50];
    char line[4098];
    while (fgets(line, sizeof line, file) && (i < nb_mission))
    {
        head = 0;

        // mission.id
        j = 0;
        while (line[head] != ',') {
            buffer[j] = line[head];
            j++;
            head++;
        }
        buffer[j] = '\0';
        head++;
        missions[i].id = atoi(buffer);

        // mission.day
        j = 0;
        while (line[head] != ',') {
            buffer[j] = line[head];
            j++;
            head++;
        }
        buffer[j] = '\0';
        head++;
        missions[i].day = atoi(buffer);

        // mission.start_time
        j = 0;
        while (line[head] != ',') {
            buffer[j] = line[head];
            j++;
            head++;
        }
        buffer[j] = '\0';
        head++;
        missions[i].start_time = atoi(buffer);

        // mission.end_time
        j = 0;
        while (line[head] != ',') {
            buffer[j] = line[head];
            j++;
            head++;
        }
        buffer[j] = '\0';
        head++;
        missions[i].end_time = atoi(buffer);

        // mission.skill
        j = 0;
        while (line[head] != ',') {
            buffer[j] = line[head];
            j++;
            head++;
        }
        buffer[j] = '\0';
        head++;
        if (strcmp(buffer, "LSF") == 0) {
            missions[i].skill = LSF;
        }
        else if (strcmp(buffer, "LPC") == 0){
            missions[i].skill = LPC;
        }
        else {
            return 1;
        }

        // mission.speciality
        j = 0;
        while (line[head] != '\n' && line[head] != '\r') {
            buffer[j] = line[head];
            j++;
            head++;
        }
        buffer[j] = '\0';
        head++;
        if (strcmp(buffer, "Jardinage")==0) {
            missions[i].speciality = JARDINAGE;
        }
        else if(strcmp(buffer, "Mecanique")==0){
            missions[i].speciality = MECANIQUE;
        }
        else if(strcmp(buffer, "Menuiserie")==0){
            missions[i].speciality = MENUISERIE;
        }
        else if(strcmp(buffer, "Electricite")==0){
            missions[i].speciality = ELECTRICITE;
        }
        else if(strcmp(buffer, "Musique")==0){
            missions[i].speciality = MUSIQUE;
        }
        else {
            return 1;
        }

        i++;
    }

    fclose(file);
    return 0;
}
