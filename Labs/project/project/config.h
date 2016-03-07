#pragma once

#define DEFAULT_FUNC CUDA_OMP_RUN
#define DEFAULT_N 100
#define DEFAULT_K 5
#define DEFAULT_MAX_X 10
#define DEFAULT_MAX_Y 10
#define DEFAULT_POINTS_FILE_NAME "points.txt"
#define DEFAULT_RESULTS_FILE_NAME "results.txt"
#define POINTS_PER_ITERATION 256
#define CUDA_THREADS_PER_BLOCK 1024

enum FUNC { LINEAR_RUN, CUDA_OMP_RUN, GENERATE_POINTS_FILE };
