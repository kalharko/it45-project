#include "prelude.c"
#include <time.h>
#include <string.h>

#define ROUNDS 200
#define MAX_TIME 60

#define BENCH_BEGIN { \
    bench_t res; \
    res.n_agents = n_agents; \
    res.n_missions = n_missions; \
    double time = 0.0; \
    double time_squared = 0.0; \
    size_t n = 0; \
    for (; n < ROUNDS || time > MAX_TIME; n++) { \
        struct timespec start, end;

#define BENCH_START clock_getres(CLOCK_THREAD_CPUTIME_ID, &start);
#define BENCH_STOP clock_getres(CLOCK_THREAD_CPUTIME_ID, &end);

#define BENCH_END \
        double current_time = (double)(end.tv_nsec - start.tv_nsec) * 1e-9 + (double)(end.tv_sec - start.tv_sec); \
        time += current_time; \
        time_squared += current_time * current_time; \
    } \
    res.avg = time / (double)n; \
    res.sigma = sqrt(time_squared - time * time); \
    return res; \
}

// bench_t bench_naive_sub(size_t n_agents, size_t n_missions)
// BENCH_BEGIN
//     problem_t problem = empty_problem();
// BENCH_START
//     initial_params_t initial_params;
//     initial_params.population = 200;
//     initial_params.rounds = 1000;
//     initial_params.survival_rate = 0.5;
//     initial_params.reproduction_rate = 0.25;
//     initial_params.mutation_rate = 0.25;
//     initial_params.unassigned_penalty = 30.0; // how many minutes an unassigned mission is worth
//     solution_t initial_solution = build_initial_solution(&problem, initial_params);
//     solution_t solution = initial_solution;
//     for (int i = 0; i < 3; i++) {
//         problem.current_objective = i;
//         problem.temperature = temperature;
//         solution = optimize_solution(solution, &problem);
//         problem.validated_scores[i] = solution.score;
//     }
// BENCH_STOP

bench_t bench_naive_sub(size_t n_agents, size_t n_missions) {
    bench_t res;
    res.n_agents = n_agents;
    res.n_missions = n_missions;

    size_t count_success = 0;

    size_t n = 0;
    for (; n < ROUNDS; n++) {
        problem_t problem = empty_problem();

        for (size_t agent = 0; agent < n_agents; agent++) {
            problem_push_agent(&problem,
                agent,
                rand() % N_SKILLS,
                rand() % N_SPECIALTIES,
                24 + rand() % 16
            );
        }

        for (size_t mission = 0; mission < n_missions; mission++) {
            uint64_t start = 8 * 60 + rand() % (10 * 60);
            problem_push_mission(&problem,
                mission,
                rand() % N_SKILLS,
                rand() % N_SPECIALTIES,
                1 + rand() % (N_DAYS - 2),
                start,
                start + (6 + rand() % 13) * 10
            );
        }

        problem_shuffle_missions(&problem);
        problem_set_random_distances(&problem, 10000);

        solution_t solution = build_naive(&problem);
        if (is_solution_valid(&solution, &problem)) {
            count_success++;
        }
    }

    res.avg = (float)count_success / (float)n;
    return res;
}

void bench_naive() {
    FILE* out = fopen("naive.csv", "w");

    for (size_t n_agents = 2; n_agents <= 16; n_agents++) {
        fprintf(out, ",%zu", n_agents);
    }
    fprintf(out, "\n");

    for (size_t n_missions = 10; n_missions <= 100; n_missions += 5) {
        fprintf(out, "%zu", n_missions);
        for (size_t n_agents = 2; n_agents <= 16; n_agents++) {
            bench_t res = bench_naive_sub(n_agents, n_missions);
            printf("%zu/%zu: %.4f ± %.4f\n", res.n_agents, res.n_missions, res.avg, res.sigma);

            fprintf(out, ",%.6f", res.avg);
        }
        fprintf(out, "\n");
    }

    fclose(out);
}

bench_t bench_initial_sub(size_t n_agents, size_t n_missions) {
    bench_t res;
    res.n_agents = n_agents;
    res.n_missions = n_missions;

    size_t count_success = 0;

    #pragma omp parallel for reduction(+:count_success)
    for (size_t n = 0; n < ROUNDS; n++) {
        problem_t problem = empty_problem();

        for (size_t agent = 0; agent < n_agents; agent++) {
            problem_push_agent(&problem,
                agent,
                rand() % N_SKILLS,
                rand() % N_SPECIALTIES,
                24 + rand() % 16
            );
        }

        for (size_t mission = 0; mission < n_missions; mission++) {
            uint64_t start = 8 * 60 + rand() % (10 * 60);
            problem_push_mission(&problem,
                mission,
                rand() % N_SKILLS,
                rand() % N_SPECIALTIES,
                1 + rand() % (N_DAYS - 2),
                start,
                start + (6 + rand() % 13) * 10
            );
        }

        problem_shuffle_missions(&problem);
        problem_set_random_distances(&problem, 10000);

        initial_params_t initial_params = {
            .population = 100,
            .rounds = 100,
            .survival_rate = 0.75,
            .reproduction_rate = 0.25,
            .mutation_rate = 0.25,
            .unassigned_penalty = 30.0
        };

        solution_t solution = build_initial_solution(&problem, initial_params);
        if (is_solution_valid(&solution, &problem)) {
            count_success++;
        }
    }

    res.avg = (float)count_success / (float)ROUNDS;
    return res;
}

void bench_initial() {
    FILE* out = fopen("initial.csv", "w");

    for (size_t n_agents = 2; n_agents <= 16; n_agents++) {
        fprintf(out, ",%zu", n_agents);
    }
    fprintf(out, "\n");


    for (size_t n_missions = 10; n_missions <= 100; n_missions += 5) {
        fprintf(out, "%zu", n_missions);
        for (size_t n_agents = 2; n_agents <= 16; n_agents++) {
            bench_t res = bench_initial_sub(n_agents, n_missions);
            printf("%zu/%zu: %.4f ± %.4f\n", res.n_agents, res.n_missions, res.avg, res.sigma);

            fprintf(out, ",%.6f", res.avg);
        }
        fprintf(out, "\n");
    }

    fclose(out);
}


bench_t bench_initial_diff_sub(size_t n_agents, size_t n_missions) {
    bench_t res;
    res.n_agents = n_agents;
    res.n_missions = n_missions;

    size_t count_success = 0;

    #pragma omp parallel for reduction(+:count_success)
    for (size_t n = 0; n < ROUNDS; n++) {
        problem_t problem = empty_problem();

        for (size_t agent = 0; agent < n_agents; agent++) {
            problem_push_agent(&problem,
                agent,
                rand() % N_SKILLS,
                rand() % N_SPECIALTIES,
                24 + rand() % 16
            );
        }

        for (size_t mission = 0; mission < n_missions; mission++) {
            uint64_t start = 8 * 60 + rand() % (10 * 60);
            problem_push_mission(&problem,
                mission,
                rand() % N_SKILLS,
                rand() % N_SPECIALTIES,
                1 + rand() % (N_DAYS - 2),
                start,
                start + (6 + rand() % 13) * 10
            );
        }

        problem_shuffle_missions(&problem);
        problem_set_random_distances(&problem, 10000);

        initial_params_t initial_params = {
            .population = 100,
            .rounds = 100,
            .survival_rate = 0.75,
            .reproduction_rate = 0.25,
            .mutation_rate = 0.25,
            .unassigned_penalty = 30.0
        };

        solution_t naive = build_naive(&problem);

        if (!is_solution_valid(&naive, &problem)) {
            solution_t solution = build_initial_solution(&problem, initial_params);
            if (is_solution_valid(&solution, &problem)) {
                count_success++;
            }

            free_solution(solution);
        }

        free_solution(naive);
    }

    res.avg = (float)count_success / (float)ROUNDS;
    return res;
}

void bench_initial_diff() {
    FILE* out = fopen("initial-diff.csv", "w");

    for (size_t n_agents = 2; n_agents <= 16; n_agents++) {
        fprintf(out, ",%zu", n_agents);
    }
    fprintf(out, "\n");


    for (size_t n_missions = 10; n_missions <= 100; n_missions += 5) {
        fprintf(out, "%zu", n_missions);
        for (size_t n_agents = 2; n_agents <= 16; n_agents++) {
            bench_t res = bench_initial_diff_sub(n_agents, n_missions);
            printf("%zu/%zu: %.4f ± %.4f\n", res.n_agents, res.n_missions, res.avg, res.sigma);

            fprintf(out, ",%.6f", res.avg);
        }
        fprintf(out, "\n");
    }

    fclose(out);
}

int main(int argc, char* argv[]) {
    srand(127643); // Nothing up my sleeve

    bool all = argc == 2 && strcmp(argv[1], "all") == 0;

    if (all || argc == 2 && strcmp(argv[1], "naive") == 0) {
        bench_naive();
    }
    if (all || argc == 2 && strcmp(argv[1], "initial") == 0) {
        bench_initial();
    }
    if (all || argc == 2 && strcmp(argv[1], "initial-diff") == 0) {
        bench_initial_diff();
    }
}
