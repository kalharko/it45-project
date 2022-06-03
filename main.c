
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct agent
{
   int id;
   int skill; // LSF = 0, LPC = 1
   int speciality;
   int hours;
} typedef agent;

struct mission
{
    int id;
    int day;
    int start_time; // in minit
    int end_time; // in minit
    int skill;
    int speciality;
} typedef mission;


int load_distance(int row, int col, char *filename, float **data){
    FILE *file;
    file = fopen(filename, "r");

    int i = 0;
    char line[4098];
    while (fgets(line, 4098, file) && (i < row))
    {
        // float row[ssParams->nreal + 1];
        char* tmp = strdup(line);

        int j = 0;
        const char* tok;
        for (tok = strtok(line, ","); tok && *tok; j++, tok = strtok(NULL, ","))
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

int load_agents(char *filename, agent agents[], int nb_agent) {
    FILE *file;
    file = fopen(filename, "r");

    int i = 0, j, head;
    char buffer[50];
    char line[4098];
    char speciality[5][20] = {"Jardinage", "Mecanique", "Menuiserie", "Musique", "Electricite"};
    while (fgets(line, 4098, file) && (i < nb_agent))
    {
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

        j = 0;
        while (line[head] != ',') {
            buffer[j] = line[head];
            j++;
            head++;
        }
        buffer[j] = '\0';
        head++;
        if (strcmp(buffer, "LSF") == 0) {
            agents[i].skill = 0;
        }
        else {
            agents[i].skill = 1;
        }

        j = 0;
        while (line[head] != ',') {
            buffer[j] = line[head];
            j++;
            head++;
        }
        buffer[j] = '\0';
        head++;
        j = 0;
        while (strcmp(buffer, speciality[i]) != 0) {
            j++;
        }
        agents[i].speciality = j;

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






int main(int argc, char **argv)
{


    char path[128] = "./Instances/45-4/"; // will be replaced by argument
    int nb_agent = 4;
    int nb_mission = 45;

    // // Open data from csv files
    // Distance
    float **distance_csv;
    distance_csv = (float **)malloc(45 * sizeof(float *));
    for (int i = 0; i < 45; ++i){
        distance_csv[i] = (float *)malloc(45 * sizeof(float));
    }
    load_distance(45, 45, strcat(path, "Distances.csv"), distance_csv);

    // Agent
    agent agents[nb_agent];
    load_agents(strcat(path, "Intervenants.csv"), agents, nb_agent);

    for (int i=0; i<nb_agent; i++) {
        printf("%d, %d, %d, %d\n", agents[i].id, agents[i].skill, agents[i].speciality, agents[i].hours);
    }


    // Missions



    return 0;
}
