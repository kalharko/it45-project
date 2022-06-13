#include <stdlib.h>
#include <math.h>

#include "utils.h"
#include "score.h"


float score_solution(solution_t* solution, const problem_t* problem) {
    float score = 0;

    switch (problem->current_objective)
    {
        case 0 : // minimize the number of affectation with the wrong specialty
            score = score_speciality(solution, problem);
        break;

        case 1 : // minimize the total distance traveled
            if (score_speciality(solution, problem) < problem->validated_scores[0]) {
                return -1;
            }
            score = score_distance(solution, problem);
        break;

        case 2 : // minimize the overtime
            if (score_speciality(solution, problem) < problem->validated_scores[0] ||
                score_distance(solution, problem) < problem->validated_scores[1]) {
                return -1;
            }
            score = score_overtime(solution, problem);
        break;

        default :
        break;
    }

    //printf("%d \t %f\n", problem->current_objective, score);
    // solution->score = score;
    return score;
}


// returns the number of assignments where the agent and mission specialty do not match
float score_speciality(solution_t* solution, const problem_t* problem)
{
    float score = 0;

    for (int i=0; i<solution->n_assignments; i++)
    {
        if (problem->missions[solution->assignments[i]].speciality
            != problem->agents[solution->assignments[i]].speciality) {
                score += 1;
            }
    }

    return score;
}


// returns the total distance traveled by the agents
float score_distance(solution_t* solution, const problem_t* problem)
{
    // should be already calculated by the is_solution_valid() function
    if (solution->distance_traveled != 0) {
        return solution->distance_traveled;
    }

    return 0;
}


float score_overtime(solution_t* solution, const problem_t* problem)
{
    return 0;
}
