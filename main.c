
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



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
            printf("%f\t", data[i][j]);
        }
        //printf("\n");

        free(tmp);
        i++;
    }
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

    // Missions



    return 0;
}
