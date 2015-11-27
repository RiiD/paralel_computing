#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

#define PI  3.1415
#define MAX_ITER 100000
#define ABS_MAX 5
#define DEFAULT_CHUNK_DELIMETER 10
#define DEFAULT_ARRAY_SIZE 2000
#define DEFAULT_RUN_LINEAR 0
#define DEFAULT_PROGRESS_MODE 0

int rank, commSize, master, maxChunkSize, arraySize, runLinear, progressMode;

void sendJob(int dest, double *numbers, int count);
void slavesJob();
void mastersJob();
int linearPositivesCount(const double* numbers, int size);
void generateArray(double* array, int size);
double func(double x);
double generateRandomDouble(int max, int min);
int parallelRun(double *numbers, int size);

int main(int argc,char *argv[])
{
    MPI_Init(&argc,&argv);
 
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&commSize);
	
	int chunkDelimeter, i;
	
	// Set defaults
	arraySize = DEFAULT_ARRAY_SIZE;
	chunkDelimeter = DEFAULT_CHUNK_DELIMETER;
	runLinear = DEFAULT_RUN_LINEAR;
	progressMode = DEFAULT_PROGRESS_MODE;
	
	for(i = 1; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case 's':
					arraySize = (int) strtol(argv[++i], (char **)NULL, 10);
					break;
				case 'c':
					chunkDelimeter = (int) strtol(argv[++i], (char **)NULL, 10);
					break;
				case 'l':
					runLinear = 1;
					break;
				case 'p':
					progressMode = 1;
					break;
				case 'h':
					if(rank == master)
					{
						printf("\n\n\tLab3 for parallel programming course.\n\tUsage:\n\tmpirun %s [-s arraySoze] [-c chunkDelimeter] [-l]\n", argv[0]);
						printf("\t\tChunk size will be computed by formula: chunkSize = ceil(arraySize / (slavesNum * chunkDelimeter))\n");
						printf("\t-c\t Set chunk delimeter.\n");
						printf("\t-s\t Set array size.\n");
						printf("\t-l\t Run linear test on the array. For testing purposes.\n\n\n");
						printf("\t-p\t Show progress.\n\n\n");
					}
					return 0;
					
			}
		}
	}
	
	maxChunkSize = ceil((double)arraySize / ((commSize - 1) * chunkDelimeter));
	
	if(rank == master)
	{
		mastersJob();
	}else{
		slavesJob();
	}
	
    MPI_Finalize();
 
    return 0;
}

/**
 * Runs the job on multiple procs.
 * @param double* numbers Array of numbers to be proccessed.
 * @param int size Size of the array
 * 
 * @returns int Number of positive runs
 */
int parallelRun(double *numbers, int size)
{
	// This function become big but I don't know how to divide it to 
	// smaller funtions in elegant way(without passing a lot of parameters).
	double *position, *endOfNumbers;
	int result, *results, i, done, numberOfChunks;
	unsigned char isFinished;
	char* progressFormat = "Parallel run: %f %% done";
	MPI_Request **requests;
	
	// Allocate memory
	requests = (MPI_Request**)calloc(commSize, sizeof(MPI_Request*));
	results = (int*)malloc(commSize * sizeof(int));
	
	// Initial values
	done = 0;
	numberOfChunks = ceil((double)arraySize / maxChunkSize);
	position = numbers; // Current position on numbers array
	endOfNumbers = numbers + size; // End of numbers array
	result = 0;
	isFinished = 0;
	if(progressMode)
	{
		printf("\n");
	}
	
	// Show initial progress
	if(progressMode)
	{
		printf(progressFormat, 0);
	}
	
	while(!isFinished)
	{
		isFinished = 1;
		
		// Iterate over all procs
		for(i = 0; i < commSize; i++)
		{
			int chunkSize;
		
		
			// Omit the master proc
			if(i == master)
			{
				continue;
			}
			
			// Last chunk maybe smaller than others.
			chunkSize = MIN(maxChunkSize, endOfNumbers - position);
			
			// If there is request for current slave
			if(requests[i])
			{
				int isJobFinished;
				// Get received status
				MPI_Test(requests[i], &isJobFinished, MPI_STATUS_IGNORE);
				
				// Check if job is finished
				if(isJobFinished)
				{
					// Print progress if in progress mode
					if(progressMode)
					{
						done++;
						printf("\r");
						printf(progressFormat, (double)done * 100 / numberOfChunks);
					}
					
					// If it is add the result from current job to sum of results
					result += results[i];
					
					// If there is more jobs send one, generate new request and store it. 
					if(chunkSize > 0)
					{
						sendJob(i, position, chunkSize);
						position += chunkSize;
						MPI_Irecv(&results[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, requests[i]);
						isFinished = 0;
					}
					else
					// If there is no jobs release allocated space for reauest and set it to null
					{
						free(requests[i]);
						requests[i] = 0;
					}
				}else{
					// Job is not finished. Continue.
					isFinished = 0;
				}
			}else{
				// If no request for current slave create it and send job to slave.
				if(chunkSize > 0)
				{
					requests[i] = malloc(sizeof(MPI_Request));
					sendJob(i, position, chunkSize);
					position += chunkSize;
					MPI_Irecv(&results[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, requests[i]);
					isFinished = 0;
				}
			}
		}
	}
	
	if(progressMode)
	{
		printf("\n");
	}
	
	// Finalize
	for(i = 0; i < commSize; i ++)
	{
		
	// Omit the master
		if(i == master)
		{
			continue;
		}
		// Tell slave he can finish
		sendJob(i, position, 0);
		
		// Free allocated memory
		if(requests[i])
		{
			free(requests[i]);
		}
	}
	free(requests);
	free(results);
	
	return result;
}

/**
 * Function that will be runned on master proc.
 */
void mastersJob(){
	double *numbers, deltaTime;
	int result;
	time_t	startTime, endTime;
	
	printf("Running with params: \n");
	printf("\tNum of slaves: %d\n", commSize - 1);
	printf("\tArray size: %d\n", arraySize);
	printf("\tChunk size: %d\n\n", maxChunkSize);
	
	printf("Memory allocation...\n");
	// Allocate space for number array
	numbers = (double*)malloc(arraySize * sizeof(double));
	
	printf("Generating array...\n");
	// Generate random array of doubles
	generateArray(numbers, arraySize);
	
	printf("Proccessing...\n");
	// Start timer
	time(&startTime);
	result = parallelRun(numbers, arraySize);
	// Stop timer
	time(&endTime);
	deltaTime = difftime(endTime, startTime);
	printf("Finished!\n");
	
	// Print run results
	printf("Paralel result = %d\n", result);
	printf("Took %.2f s\n", deltaTime);
	
	if(runLinear)
	{
		// Linear run over whone array to check that this program works
		time(&startTime);
		result = linearPositivesCount(numbers, arraySize);
		time(&endTime);
		deltaTime = difftime(endTime, startTime);
		printf("Linear result = %d\nTook %.2f s\n", result, deltaTime);
	}
	
	// Release memory
	free(numbers);
}

/**
 * Sends a job to dest. Packs it as described on recvJob comments.
 * @param int dest Destination proc rank
 * @param *double numbers Array of numbers to be proccessed.
 * @param int count Number of items to send
 */
void sendJob(int dest, double *numbers, int count)
{
	char *buff;
	int position = 0, packSize;
	packSize = sizeof(int) + sizeof(double) * maxChunkSize;
	buff = malloc(packSize);
	
	MPI_Pack(&count, 1, MPI_INT, buff, packSize, &position, MPI_COMM_WORLD);
	MPI_Pack(numbers, count, MPI_DOUBLE, buff, packSize, &position, MPI_COMM_WORLD);
	
	MPI_Send(buff, position, MPI_PACKED, dest, 0, MPI_COMM_WORLD);
	free(buff);
}

/**
 * Receives a job from src. Job must be packed into mpi package, First int is size of job array, second is job array.
 * @param int src Rank of source proc.
 * @param *double numbers Pointer to allocated array of given size
 * @param *int count Pointer to int variable to store size of numbers array
 */
void recvJob(int src, double *numbers, int *count)
{
	unsigned char* buff;
	int packSize, position = 0;
	MPI_Status status;
	
	packSize = sizeof(int) + sizeof(double) * maxChunkSize;
	buff = malloc(packSize);
	position = 0;
	
	MPI_Recv(buff, packSize, MPI_PACKED, src, 0, MPI_COMM_WORLD, &status);
	MPI_Unpack(buff, packSize, &position, count, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buff, packSize, &position, numbers, *count, MPI_DOUBLE, MPI_COMM_WORLD);
	free(buff);
	
}

/**
 * Function will be runned on slave proc.
 * Receives array from master and counts positiove runs of given function.
 */
void slavesJob(){
	double *numbers;
	int size, result;
	
	numbers = malloc(sizeof(double) * maxChunkSize);
	
	while(1)
	{
		recvJob(master, numbers, &size);
		
		if(!size)
		{
			// If master sends size = 0 finish.
			break;
		}
		
		// Linear run
		result = linearPositivesCount(numbers, size);
		
		MPI_Send(&result, 1, MPI_INT, master, 0, MPI_COMM_WORLD);
	}
	
	free(numbers);
}

/**
 * Given test function
 * @param int x Test value.
 */
double func(double x) {
	int i;
	double value = x;
	int limit = rand() % 3 + 1;
		
	for(i = 0;  i < limit * MAX_ITER;  i++)
		value = sin(exp(sin(exp(sin(exp(value))))) - PI / 2) - 0.5;

	return value;
}

/**
 * Generate random array in range [-ABS_MAX...ABS_MAX] of given size;
 * @param double* numbers Array of numbers.
 * @param uint size Size of array.
 */
void generateArray(double* numbers, int size){
	int i;
	for(i = 0; i < size; i++){
		numbers[i] = generateRandomDouble(ABS_MAX, -ABS_MAX);
	}
}

/**
 * Generate random double between min and max
 * @param int min 
 * @param int max
 * @return double Generated value
 */
double generateRandomDouble(int max, int min){
	double scaled = (double)rand()/RAND_MAX;
	return (max - min +1)*scaled + min;
}

/**
 * Counts positives in same proc.
 * @param double* numbers Array of numbers.
 * @param int size Size of array.
 * @return int Number of positioves
 */
int linearPositivesCount(const double* numbers, int size){
	int i, positives = 0;
	for(i = 0; i < size; i++){
		if(func(numbers[i]) > 0){
			positives++;
		}
	}
	return positives;
}
