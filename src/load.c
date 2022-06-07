#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "load.h"
#include "defs.h"

int load_distance(size_t row, size_t col, char *filename, float **data) {
    FILE *file;
    file = fopen(filename, "r");
    assert(file != NULL);

    int i = 0;
    char line[4098];
    while (fgets(line, 4098, file) && (i < row))
    {
        // float row[ssParams->nreal + 1];
        char* tmp = strdup(line);

        int j = 0;
        const char* tok;
        for (tok = strtok(line, ","); tok && *tok && j < col; j++, tok = strtok(NULL, ","))
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

int load_agents(char *filename, agent_t agents[], int nb_agent) {
    FILE *file = fopen(filename, "r");

    assert(file != NULL);

    int i = 0, j, head;
    char buffer[50];
    char line[4098];
    while (fgets(line, sizeof line, file) && (i < nb_agent))
    {
        // agents.id
        head = 0;
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
        while (line[head] != '\n') {
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
