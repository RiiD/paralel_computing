#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <float.h>

#include "points.h"
#include "utils.h"
#include "linear.h"
#include "config.h"
#include "kernel.h"

// Declarations
void parseArgs(int argc, char *argv[]);
void generatePointsFile();
void findKNearest(void(*strategy)(Point *points, int *kNearest, void(*statusCallback)(const double percent)));
void linearStrategy(Point *points, int *kNearest, void(*statusCallback)(const double percent));
void cudaOmpStrategy(Point *points, int *kNearest, void(*statusCallback)(const double percent));
void saveResults(int *kNearest);

// Global parameters
FUNC func = DEFAULT_FUNC;		// Function to run
int n = DEFAULT_N;				// Number of points
int k = DEFAULT_K;				// Number of nearest points
double maxX = DEFAULT_MAX_X;	// Maximum x axis value
double maxY = DEFAULT_MAX_Y;	// Maximum y axis value
char* pointsFileName = DEFAULT_POINTS_FILE_NAME; // File to be readed
char* resultFileName = DEFAULT_RESULTS_FILE_NAME;

/**
 * Bootstraps the application.
 * @param int argc
 * @param char** argv
 * @returns int
 */
int main(int argc, char *argv[]) {
	
	parseArgs(argc, argv);

	switch (func) {
		case GENERATE_POINTS_FILE:
			generatePointsFile();
			break;
		case LINEAR_RUN:
			findKNearest(linearStrategy);
			break;
		case CUDA_OMP_RUN:
			findKNearest(cudaOmpStrategy);
			break;
	}

	return 0;
}

/**
 * Runs over arguments and applies them.
 * @params int argc
 * @params char* argv
 */
void parseArgs(int argc, char *argv[]) {
	int i;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {

			case 'g':
				func = GENERATE_POINTS_FILE;
				break;

			case 'f':
				switch (argv[i][2])
				{
					case 'p':
						pointsFileName = argv[++i];
						break;
					case 'r':
						resultFileName = argv[++i];
				}
				break;

			case 'n':
				n = atoi(argv[++i]);
				break;

			case 'k':
				k = atoi(argv[++i]);
				break;

			case 'l':
				func = LINEAR_RUN;
				break;
			}
		}
	}
}

/**
 * Generates n points and saves them to file.
 */
void generatePointsFile() {
	Point* points;

	printf("Creating points file:%s", NEWLINE);
	printf("\tFile name:\t%s%s", pointsFileName, NEWLINE);
	printf("\tPoints number:\t%d%s", n, NEWLINE);
	printf("\tNeares numbers count:\t%d%s%s", k, NEWLINE, NEWLINE);

	printf("Allocating memory...\t");
	points = (Point*) malloc(n * sizeof(Point));
	if (points == NULL) {
		printf("FALIED%s", NEWLINE);
		return;
	}

	printf("OK%s", NEWLINE);

	printf("Generating points...\t");
	generatePoints(n, points, maxX, maxY);
	printf("OK%s", NEWLINE);

	printf("Saving points...\t");
	savePoints(pointsFileName, points, n, k);
	printf("OK%s", NEWLINE);

	printf("Freeing memory...\t");
	free(points);
	printf("OK%s", NEWLINE);

	printf("Points file created!%s", NEWLINE);
}

/**
 * Prints current run configuration.
 */
void printRunConfiguration() {
	printf("Run configuration:%s", NEWLINE);
	printf("\tType:\t");
	switch (func) {
		case LINEAR_RUN:
			printf("Linear");
			break;
		case CUDA_OMP_RUN:
			printf("Parallel");
			break;
	}
	printf(NEWLINE);
	printf("\tN: %d%s", n, NEWLINE);
	printf("\tK: %d%s", k, NEWLINE);
}

/**
* Prints given number as percent.
* @param const double percent
*/
void printStatus(const double percent) {
	printf("\r%5.2f%% completed", percent);
	fflush(stdout);
}

/**
 * finds k nearest elemrnts.
 * @param char* fileName
 * @param void* strategy callback strategy to run.
 */
void findKNearest(void(*strategy)(Point *points, int *kNearest, void(*statusCallback)(const double percent))) {
	
	int *kNearest, startTime, endTime;
	Point* points;

	printf("Loading points...\t");
	points = loadPoints(pointsFileName, &n, &k);

	if (points == NULL) {
		printf("Failed to load points!%s", NEWLINE);
		return;
	}
	printf("OK%s", NEWLINE);

	printRunConfiguration();

	printf("Allocating memory...\t");
	kNearest = (int*)malloc(k * sizeof(int) * n);

	if (kNearest == NULL) {
		printf("FAILED");
		return;
	}
	printf("OK%s", NEWLINE);
	printf("Calculating...%s", NEWLINE);
	startTime = (int)time(NULL);
	strategy(points, kNearest, printStatus);
	endTime = (int)time(NULL);
	printf(NEWLINE);

	printf("Saving results...");
	saveResults(kNearest);
	printf("OK%s", NEWLINE);

	printf("Took: %d s", endTime - startTime);

	free(kNearest);
	free(points);
}

/**
 * Calculates indexes of k nearest points for each point in points array in linear way.
 * @param Point* points
 * @param int* kNearest points array. Must be initialized
 * @param void* statusCallback callable will be called when need to update status.
 */
void linearStrategy(Point *points, int *kNearest, void (*statusCallback)(const double percent)) {
	double *distances;

	distances = (double*)calloc(n * n, sizeof(double));
	
	if (distances == NULL) {
		printf("Failed to allocate memory for temporary distances matrix!%s", NEWLINE);
		return;
	}

	statusCallback(0);
	linearDinstanceCalculate(points, n, distances);
	statusCallback(50);
	for (int i = 0; i < n; i++) {
		distances[i * n + i] = DBL_MAX; // Set (x, x) elemnt to max double so it will not be considered as nearest point to itself.
		sortKElements(distances + i * n, kNearest + i * k, n, k);
		statusCallback( 50 + 50.0 * (i + 1) / n);
	}

	free(distances);
}

/**
* Calculates indexes of k nearest points for each point in points array using CUDA and omp.
* @param Point* points
* @param int* kNearest points array. Must be initialized
* @param void* statusCallback callable will be called when need to update status.
*/
void cudaOmpStrategy(Point *points, int *kNearest, void(*statusCallback)(const double percent)) {
	double *distances;
	distances = (double*)malloc(n * POINTS_PER_ITERATION * sizeof(double));

	if (distances == NULL) {
		printf("Failed to allocate memory for temporary distances matrix!%s", NEWLINE);
		return;
	}

	cudaInit(points, n, POINTS_PER_ITERATION);

	for (int i = 0; i <= n; i = i + POINTS_PER_ITERATION) {

		if (i < n)
			runOnCUDA(distances, n, k, i, POINTS_PER_ITERATION);

		if (i > 0) {
		#pragma omp parallel for
			for (int j = 0; j < POINTS_PER_ITERATION; j++) {
				distances[n * j + (i - POINTS_PER_ITERATION) + j] = DBL_MAX; // Max out distance from point itself so sort algorithm will ignore it
				sortKElements(distances + j * n, kNearest + k * (i - POINTS_PER_ITERATION + j), n, k);
			}
		}
		if (i < n)
			cudaResult(distances, n, POINTS_PER_ITERATION);
		statusCallback(100.0 * i / n);
	}
	cudaFinalize();
	free(distances);
}

/**
 * Save results to results file.
 * @param int* kNearest
 */
void saveResults(int *kNearest) {
	FILE *fh;
	fopen_s(&fh, resultFileName, "w");

	if (fh == NULL) {
		printf("Failed to open file %s for writing!%s", resultFileName, NEWLINE);
		return;
	}

	for (int i = 0; i < n; i++) {
		fprintf(fh, "%d ", i);
		for (int j = 0; j < k; j++) {
			fprintf(fh, "%d ", kNearest[k * i + j]);
		}
		fprintf(fh, NEWLINE);
	}

	fclose(fh);
}