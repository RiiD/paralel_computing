#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <float.h>
#include <mpi.h>
#include <string.h>

#include "points.h"
#include "utils.h"
#include "linear.h"
#include "config.h"
#include "kernel.h"

#define MASTER_RANK 0

// Declarations
void parseArgs(int argc, char *argv[]);
void generatePointsFile();
void findKNearest(void(*strategy)(const Point *points, int *kNearest, int startPoint, int numberOfPoints, void(*statusCallback)(double percent)));
void linearStrategy(const Point *points, int *kNearest, int startPoint, int numberOfPoints, void(*statusCallback)(double percent));
void cudaOmpStrategy(const Point *points, int *kNearest, int startPoint, int numberOfPoints, void(*statusCallback)(double percent));
void mpiCudaOmpStrategy(const Point *points, int *kNearest, int startPoint, int numberOfPoints, void(*statusCallback)(double percent));
void saveResults(const int *kNearest);
void mpiOmpCudaRun();

// Global parameters
FUNC func = DEFAULT_FUNC;		// Function to run
int n = DEFAULT_N;				// Number of points
int k = DEFAULT_K;				// Number of nearest points
const double maxX = DEFAULT_MAX_X;	// Maximum x axis value
const double maxY = DEFAULT_MAX_Y;	// Maximum y axis value
const char* pointsFileName = DEFAULT_POINTS_FILE_NAME; // File to be readed
const char* resultFileName = DEFAULT_RESULTS_FILE_NAME; // Results file
int commSize, rank;

/**
 * Bootstraps the application.
 * @param int argc
 * @param char** argv
 * @returns int
 */
int main(int argc, char *argv[]) {
	MPI_Init(&argc, &argv);
	
	parseArgs(argc, argv);

	MPI_Comm_size(MPI_COMM_WORLD, &commSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

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
		case MPI_CUDA_OMP_RUN:
			findKNearest(mpiCudaOmpStrategy);
			break;
	}

	MPI_Finalize();
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
				switch (argv[i][2]) {
					case 'l':
						func = LINEAR_RUN;
						break;
					case 'p':
						func = MPI_CUDA_OMP_RUN;
				}
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

	if (rank != MASTER_RANK)
		return;

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
	printf("\tType:\t\t");
	switch (func) {
		case LINEAR_RUN:
			printf("Linear");
			break;
		case CUDA_OMP_RUN:
			printf("CUDA + OMP");
			break;
		case MPI_CUDA_OMP_RUN:
			printf("MPI + CUDA + OMP");
			break;
		case GENERATE_POINTS_FILE:
			// Ignore.
			break;
	}
	printf(NEWLINE);
	printf("\tN:\t\t%d%s", n, NEWLINE);
	printf("\tK:\t\t%d%s", k, NEWLINE);
	printf("\tComm size:\t%d%s", commSize, NEWLINE);
	printf("\tComm rank:\t%d%s", rank, NEWLINE);
}

/**
* Prints given number as percent.
* @param double percent
*/
void printStatus(double percent) {
	printf("\r%5.2f%% completed", percent);
	fflush(stdout);
}

/**
* Silent status callback for slaves.
* @param double percent
*/
void silentStatus(double percent) {
	
}

/**
 * finds k nearest elemrnts.
 * @param void* strategy callback strategy to run.
 */
void findKNearest(
	void(*strategy)(
		const Point *points, 
		int *kNearest,
		int startPoint, 
		int numberOfPoints, 
		void(*statusCallback)(double percent)
	)
) {
	
	int *kNearest = NULL, startTime, endTime;
	Point* points = NULL;

	if (rank == MASTER_RANK) {
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
	}

	strategy(points, kNearest, 0, n, printStatus);

	if (rank == MASTER_RANK) {
		endTime = (int)time(NULL);
		printf(NEWLINE);

		printf("Saving results...");
		saveResults(kNearest);
		printf("OK%s", NEWLINE);

		printf("Took: %d s", endTime - startTime);

		free(kNearest);
		free(points);
	}
}

/**
 * Calculates indexes of k nearest points for each point in points array in linear way.
 * @param const Point* points
 * @param int* kNearest points array. Must be initialized
 * @param void* statusCallback callable will be called when need to update status.
 */
void linearStrategy(const Point *points, int *kNearest, int startPoint, int numberOfPoints, void(*statusCallback)(double percent)) {
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
* @param const Point* points
* @param int* kNearest points array. Must be initialized
* @param void* statusCallback callable will be called when need to update status.
*/
void cudaOmpStrategy(const Point *points, int *kNearest, int startPoint, int numberOfPoints, void(*statusCallback)(double percent)) {
	double *distances;
	distances = (double*)malloc(n * POINTS_PER_ITERATION * sizeof(double));

	if (distances == NULL) {
		printf("Failed to allocate memory for temporary distances matrix!%s", NEWLINE);
		return;
	}

	cudaInit(points, n, POINTS_PER_ITERATION);

	for (int i = 0; i <= numberOfPoints; i += POINTS_PER_ITERATION) {

		if (i < n)
			runOnCUDA(n, i + startPoint, POINTS_PER_ITERATION);

		if (i > 0) {
		#pragma omp parallel for
			for (int j = 0; j < POINTS_PER_ITERATION; j++) {
				distances[n * j + (i + startPoint - POINTS_PER_ITERATION) + j] = DBL_MAX; // Max out distance from point itself so sort algorithm will ignore it
				sortKElements(distances + j * n, kNearest + k * (i - POINTS_PER_ITERATION + j), n, k);
			}
		}
		if (i < n)
			cudaResult(distances, n, POINTS_PER_ITERATION);
		statusCallback(100.0 * i / numberOfPoints);
	}
	cudaFinalize();
	free(distances);
}

void mpiMasterJob(const Point* points, int* result, void(*statusCallback)(double percent)) {
	int done = 0, sent = 0, packPos = 0, params[] = {n, k, 0};
	MPI_Status status;
	int *buf = (int*)malloc((MPI_JOBS_PER_ITERATION * k + 1) * sizeof(int));

	// Send every one points array
	for (int i = 0; i < commSize; i++) {
		if (i != MASTER_RANK) {
			
			params[2] = sent;
			MPI_Send(params, 3, MPI_INT, i, 0, MPI_COMM_WORLD);
			sent += MPI_JOBS_PER_ITERATION;

			MPI_Send(points, n * 2, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
		}
	}

	// Sends jobs.
	while (done < n) {
		statusCallback(100.0 * done / n);
		MPI_Recv(buf, MPI_JOBS_PER_ITERATION * k + 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		
		done += MPI_JOBS_PER_ITERATION;

		memcpy(result + buf[0] * k, buf + 1, MPI_JOBS_PER_ITERATION);

		MPI_Send(&sent, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
		sent += MPI_JOBS_PER_ITERATION;
	}

	free(buf);
}

void mpiSlaveJob() {

	Point* points;
	int t;

	int *buf = (int*)malloc(3 * sizeof(int));

	MPI_Recv(buf, 3, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	n = buf[0];
	k = buf[1];
	t = buf[2];

	buf = (int*)realloc(buf, (MPI_JOBS_PER_ITERATION * k + 1) * sizeof(int));

	buf[0] = t;

	points = (Point*)malloc(sizeof(Point) * n);

	MPI_Recv(points, n * 2, MPI_DOUBLE, MASTER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	do {
		cudaOmpStrategy(points, buf + 1, buf[0], MPI_JOBS_PER_ITERATION, silentStatus);
		MPI_Send(buf, MPI_JOBS_PER_ITERATION  * k + 1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD);
		MPI_Recv(buf, 1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	} while (buf[0] < n);

	free(points);
	free(buf);
}

void mpiCudaOmpStrategy(const Point *points, int *kNearest, int startPoint, int numberOfPoints, void(*statusCallback)(double percent)) {

	if (rank == MASTER_RANK) {
		mpiMasterJob(points, kNearest, statusCallback);
	} else {
		mpiSlaveJob();
	}
}

/**
 * Save results to results file.
 * @param const int* kNearest
 */
void saveResults(const int *kNearest) {
	FILE *fh;
	fh = fopen(resultFileName, "w");

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
