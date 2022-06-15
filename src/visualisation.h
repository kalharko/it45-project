#ifndef VISUALISATION_H
#define VISUALISATION_H


#include "defs.h"
#include "utils.h"


// Global
extern FILE *log_file;


/// Displays a solution to the consol
void print_solution(const solution_t *solution, const problem_t* problem);

/// Records objectives to the global variable file log_file
void log_for_graph(const solution_t* solution, const problem_t* problem);


#endif // VISUALISATION_H