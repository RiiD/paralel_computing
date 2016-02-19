#include <stdio.h>
#include <stdlib.h>

#include "points.h"
#include "utils.h"
#include "linear.h"
#include "config.h"

// Declarations
void parseArgs(int argc, char *argv[]);
void generatePointsFile(char* fileName, int n, int k, double maxX, double maxY);
void linearRun(char* fileName);

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
 * Calculates distances syncronius.
 * @param char* fileName
 */
void linearRun(char* fileName) {
	Distance* distances;
	int n, k;
	Point* points;

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
	distances = (Distance*)calloc(n * n, sizeof(Distance));
	if (distances == NULL) {
		printf("FAILED");
		return;
	}

	printf("OK%s", NEWLINE);

	printf("Calculating distances...\t");
	linearDinstanceCalculate(points, n, distances);
	printf("OK%s", NEWLINE);

	printf("Result:%s", NEWLINE);
	printDistances(distances, n * n, n);
	printf(NEWLINE);

	for (int i = 0; i < n; i++) {
		sortDistances(distances + i * n, n);
	}

	printf("Result:%s", NEWLINE);
	printDistancesIndices(distances, n * n, n);
	printf(NEWLINE);

	free(points);
	free(distances);
}
