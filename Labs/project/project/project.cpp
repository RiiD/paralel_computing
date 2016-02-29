#include <stdio.h>
#include <stdlib.h>

#include "points.h"
#include "utils.h"
#include "linear.h"
#include "config.h"
#include "kernel.h"

// Declarations
void parseArgs(int argc, char *argv[]);
void generatePointsFile(char* fileName, int n, int k, double maxX, double maxY);
void linearRun(char* fileName);
void parallelRun(char* fileName);

// Global parameters
FUNC func = DEFAULT_FUNC;		// Function to run
int n = DEFAULT_N;				// Number of points
int k = DEFAULT_K;				// Number of nearest points
double maxX = DEFAULT_MAX_X;	// Maximum x axis value
double maxY = DEFAULT_MAX_Y;	// Maximum y axis value
char* filename = DEFAULT_FILE_NAME; // File to be readed

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
			generatePointsFile(filename, n, k, maxX, maxY);
			break;
		case LINEAR_RUN:
			linearRun(filename);
			break;
		case PARALLEL_RUN:
			parallelRun(filename);
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
				filename = argv[++i];
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
 * @param char* fileName
 * @param int n
 * @param int k
 * @param double maxX
 * @param double maxY
 */
void generatePointsFile(char* fileName, int n, int k, double maxX, double maxY) {
	Point* points;

	printf("Creating points file:%s", NEWLINE);
	printf("\tFile name:\t%s%s", fileName, NEWLINE);
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
	savePoints(fileName, points, n, k);
	printf("OK%s", NEWLINE);

	printf("Freeing memory...\t");
	free(points);
	printf("OK%s", NEWLINE);

	printf("Points file created!%s", NEWLINE);
}

/**
 * Calculates distances syncronius. For testing.
 * @param char* fileName
 */
void linearRun(char* fileName) {
	double* distances;
	int n, k;
	Point* points;
	int *sortedIndexes, i;

	printf("Running linear algoritm.%s", NEWLINE);

	printf("Loading points...\t");
	points = loadPoints(fileName, &n, &k);

	if (points == NULL) {
		printf("Failed to load points!%s", NEWLINE);
		return;
	}

	printf("OK%s", NEWLINE);

	printf("Running params:%s", NEWLINE);
	printf("\tN: %d%s", n, NEWLINE);
	printf("\tK: %d%s", k, NEWLINE);

	printf("Allocating memory for distances matrix...\t");
	distances = (double*)calloc(n * n, sizeof(double));
	if (distances == NULL) {
		printf("FAILED");
		return;
	}

	printf("OK%s", NEWLINE);

	printf("Calculating distances...\t");
	linearDinstanceCalculate(points, n, distances);
	printf("OK%s", NEWLINE);

	printf("Distances:%s", NEWLINE);
	printDoubleArray(distances, n * n, n);
	printf(NEWLINE);

	sortedIndexes = (int*)malloc(n * sizeof(n));

	printf("Results:%s", NEWLINE);
	for (i = 0; i < n; i++) {
		printf("%3d:\t", i);
		getSortedIndexes(distances + i * n, sortedIndexes, n);
		printIntArray(sortedIndexes, k, k);
		printf(NEWLINE);
	}
	printf(NEWLINE);

	free(sortedIndexes);
	free(points);
	free(distances);
}

/**
* Calculates distances asyncronius.
* @param char* fileName
*/
void parallelRun(char* fileName) {
	double* distances;
	int n, k;
	Point* points;
	int *sortedIndexes, i;

	printf("Running linear algoritm.%s", NEWLINE);

	printf("Loading points...\t");
	points = loadPoints(fileName, &n, &k);

	if (points == NULL) {
		printf("Failed to load points!%s", NEWLINE);
		return;
	}

	printf("OK%s", NEWLINE);

	printf("Running params:%s", NEWLINE);
	printf("\tN: %d%s", n, NEWLINE);
	printf("\tK: %d%s", k, NEWLINE);

	printf("Allocating memory for distances matrix...\t");
	distances = (double*)calloc(n * n, sizeof(double));
	if (distances == NULL) {
		printf("FAILED");
		return;
	}

	printf("OK%s", NEWLINE);

	printf("Calculating distances...\t");
	runOnCUDA(points, n, distances, 0, 0, n);
	printf("OK%s", NEWLINE);

	printf("Distances:%s", NEWLINE);
	printDoubleArray(distances, n * n, n);
	printf(NEWLINE);

	/*sortedIndexes = (int*)malloc(n * sizeof(n));

	printf("Results:%s", NEWLINE);
	for (i = 0; i < n; i++) {
		printf("%3d:\t", i);
		getSortedIndexes(distances + i * n, sortedIndexes, n);
		printIntArray(sortedIndexes, k, k);
		printf(NEWLINE);
	}
	printf(NEWLINE);

	free(sortedIndexes);*/
	free(points);
	free(distances);
}