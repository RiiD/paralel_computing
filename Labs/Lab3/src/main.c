#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI  3.1415
#define MAX_ITER 100000
#define ABS_MAX 5
#define CHUNK_DELIMETER 100
#define ARRAY_SIZE 1000

unsigned int positives, chunkSize;

int rank, numOfProcs, master = 0;

double *numbers;

unsigned int linearPositivesCount(const double* numbers, const unsigned int size);
void generateArray(double* array, unsigned int size);
double func(double x);
unsigned int getChunkSize(unsigned int numOfProcs, unsigned int arraySize);
double generateRandomDouble(int max, int min);

int main(int argc,char *argv[])
{
	int i;
	
	double *currentNumber;
    
    MPI_Init(&argc,&argv);
 
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numOfProcs);
	
	if(numOfProcs > ARRAY_SIZE){
		printf("number of procs must be less than array size.\n");
		return 1;
	}
	
	chunkSize = getChunkSize(numOfProcs, ARRAY_SIZE);
	
	if(rank == master){
		numbers = malloc(sizeof(double) * ARRAY_SIZE);
		generateArray(numbers, ARRAY_SIZE);
	}else{
		numbers = malloc(sizeof(double) * chunkSize);
	}
	
	// Send to everyone
	MPI_Scatter(numbers, chunkSize, MPI_DOUBLE, numbers, chunkSize, MPI_DOUBLE, master, MPI_COMM_WORLD);
	
	currentNumber = numbers + chunkSize * numOfProcs;
	
	while(lastNumber - currentNumber > 0){
		
	}
	
    MPI_Finalize();
 
    return 0;
}

/**
 * Function that will be runned on master proc.
 */
void mastersJob(int arraySize){
	int i;
	
	
	
	while(lastNumber > currentNUmber){
		if((lastNumber - currentNumber) / chunkSize){
			
		}else{
			
		}
	}
}

/**
 * Function will be runned on slave proc.
 */
void slavesJob(){
	
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

unsigned int getChunkSize(unsigned int numOfProcs, unsigned int arraySize){
	return (arraySize / (numOfProcs * CHUNK_DELIMETER)) || 1;
}

/**
 * Generate random array in range [-ABS_MAX...ABS_MAX] of given size;
 * @param double* numbers Array of numbers.
 * @param uint size Size of array.
 */
void generateArray(double* numbers, unsigned int size){
	unsigned int i;
	for(i = 0; i < size; i++){
		numbers[i] = generateRandomDouble(ABS_MAX, -ABS_MAX);
	}
}

/**
 * Generate random double between min and max
 * @param int min 
 * @param int max
 */
double generateRandomDouble(int max, int min){
	double scaled = (double)rand()/RAND_MAX;
	return (max - min +1)*scaled + min;
}

/**
 * Counts positives in same proc.
 * @param double* numbers Array of numbers.
 * @param uint size Size of array.
 */
unsigned int linearPositivesCount(const double* numbers, unsigned int size){
	unsigned int i, positives = 0;
	for(i = 0; i < size; i++){
		if(func(numbers[i]) > 0){
			positives++;
		}
	}
	return positives;
}

/**
 * Counts positives in parallel procs.
 * @param double* numbers Array of numbers.
 * @param uint size Size of array.
 */
unsigned int parallelPositivesCount(const double* numbers, unsigned int size){
	return 0;
}
