#ifndef SCORE_H
#define SCORE_H



float score_solution(solution_t* solution, const problem_t* problem);


float score_speciality(const solution_t* solution, const problem_t* problem);


float score_distance(solution_t* solution, const problem_t* problem);


float score_overtime(solution_t* solution, const problem_t* problem);

float score_harmony(const solution_t* solution, const problem_t* problem);

float score_SESSAD(const solution_t* solution, const problem_t* problem);

#endif // SCORE_H
