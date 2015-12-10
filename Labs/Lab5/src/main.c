#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_HIST_SIZE 256
#define DEFAULT_ARRAY_SIZE 10000
#define DEFAULT_PARALLEL_RUN 1

void generateRandomIntArray(int *numbers, int size, int max);
void printIntArray(const int *array, int size);
void createHistParallel(const int* numbers, int size, int *hist, int histSize);
void createHistLinear(const int* numbers, int size, int *hist);
void normalRun();
void test1();
void tests();
void showHelp();

int gArraySize = DEFAULT_ARRAY_SIZE,
	gHistSize = DEFAULT_HIST_SIZE,
	gParallelRun = DEFAULT_PARALLEL_RUN;

/**
 * Entry point. Configure app.
 */
int main(int argc, char *argv[])
{
	for(int i = 1; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case 's':
					gArraySize = (int) strtol(argv[++i], (char **)NULL, 10);
					break;
				case 'm':
					gHistSize = (int) strtol(argv[++i], (char **)NULL, 10);
					break;
				case 'l':
					gParallelRun = 0;
					break;
				case 't':
					tests();
					return 0;
				case 'h':
					showHelp();
					return 0;
			}
		}
	}
	
	normalRun();
}

/**
 * Runs the application in normal mode.
 */
void normalRun()
{
	
	int *hist,
		*numbers,
		startTime,
		endTime;
	printf("\n\n");

	printf("Running parameters:\n");
	printf("Array size:\t%d\n", gArraySize);
	printf("Histogram size:\t%d\n", gHistSize);
	printf("Parallel run?\t%s\n", gParallelRun ? "Yes" : "No");

	printf("\n\n");
	
	printf("Allocating memory...\t");
	numbers = (int*) malloc( sizeof(int) * gArraySize );
	
	if(!numbers)
	{
		printf("FAILED\n");
		return;
	}
	
	hist = (int*) calloc(gHistSize, sizeof(int));

	if(!numbers)
	{
		printf("FAILED\n");
		return;
	}
	
	printf("OK\n");
	
	printf("Generating number of arrays...\t");
	generateRandomIntArray(numbers, gArraySize, gHistSize);
	printf("OK\n");

	printf("Generating histogram...\t");
	startTime = omp_get_wtime();
	if(gParallelRun)
	{
		createHistParallel(numbers, gArraySize, hist, gHistSize);
	}
	else
	{
		createHistLinear(numbers, gArraySize, hist);
	}
	endTime = omp_get_wtime();
	printf("OK\n");
	
	printf("Histogram:\n");
	printIntArray(hist, gHistSize);
	
	printf("Took: %fms\n", (endTime - startTime) / 1000);
	
	printf("Releasing memmory...");
	free(numbers);
	free(hist);
	printf("OK\n");
}

/**
 * Generates histogram of given array. Runs on parallel.
 * 
 * @param int*	numbers		Array of numbers
 * @param int	size		Size of array
 * @param int*	hist		Pointer to allocated and intialized to zeros histogram array
 * @param int	histSize	Histogram size
 */
void createHistParallel(const int* numbers, int size, int *hist, int histSize)
{	
	#pragma omp parallel shared(hist, histSize, numbers, size)
	{
		int tid,
			*myHist;
		
		myHist = (int*) calloc(histSize, sizeof(int));
		
		tid = omp_get_thread_num();
		
		#pragma omp for
		for(int i = 0; i < size; i++)
		{
			myHist[numbers[i]]++;
		}
		
		for(int i = 0; i < histSize; i++)
		{
			#pragma omp barrier
			{
				hist[(i + tid) % histSize] += myHist[(i + tid) % histSize];
			}
		}
		
		free(myHist);
	}
}

/**
 * Fills array with random integers.
 * @param int* numbers
 * @param int size
 */
void generateRandomIntArray(int *numbers, int size, int max)
{
	#pragma omp parallel
	{
		srand(time(NULL));
		#pragma omp for
		for(int i = 0; i < size; i++)
		{
			numbers[i] = rand() % max;
		}
	}
}

/**
 * Prints out given array.
 * 
 * @param	int*	array	Array to print
 * @param	int		size	Size of the array
 */
void printIntArray(const int *array, int size)
{
	for(int i = 0; i < size; i++)
	{
		if(i % 17 == 0)
		{
			printf("\n");
		}
		printf("%d\t", array[i]);
	}
	printf("\n");
}

/**
 * Generates histogram of given array. Runs linearly.
 * 
 * @param int*	numbers		Array of numbers
 * @param int	size		Size of array
 * @param int*	hist		Pointer to allocated and intialized to zeros histogram array
 */
void createHistLinear(const int* numbers, int size, int *hist)
{
	for(int i = 0; i < size; i++)
	{
		hist[numbers[i]]++;
	}
}

/**
 * Run some tests.
 * 
 */
void tests()
{
	int linearHist[DEFAULT_HIST_SIZE] = {0},
		parallelHist[DEFAULT_HIST_SIZE] = {0},
		numbers[DEFAULT_ARRAY_SIZE],
		cmpRes,
		count = 0;
	
	generateRandomIntArray(numbers, DEFAULT_ARRAY_SIZE, DEFAULT_HIST_SIZE);

	createHistLinear(numbers, DEFAULT_ARRAY_SIZE, linearHist);
	createHistParallel(numbers, DEFAULT_ARRAY_SIZE, parallelHist, DEFAULT_HIST_SIZE);
		
	printf("Linear run and parallel run should generate same histogram:\t");
	cmpRes = memcmp(linearHist, parallelHist, DEFAULT_HIST_SIZE * sizeof(int));
	if(cmpRes == 0)
	{
		printf("PASS\n");
	}else{
		printf("FAILED\n");
	}
	
	printf("Sum of histogram elements should be equal to array size:\t");
	for(int i = 0; i < DEFAULT_HIST_SIZE; i++)
	{
		count += parallelHist[i];
	}
	
	if(count == DEFAULT_ARRAY_SIZE)
	{
		printf("PASS\n");
	}else{
		printf("FAILED\n");
	}
	printf("\n\n");
}

/**
 * Shows help.
 */
void showHelp()
{
	printf("Lab 5 on Parallel computing course.\n");
	printf("Generates histogram out of array of given size.\n");
	printf("Usage:\n");
	printf("\tmain [-s <arraySize>] [-m <histSize>] [-l] [-t] [-h]\n\n");
	printf("\t-s n\tSet array size to n. Default %d\n", DEFAULT_ARRAY_SIZE);
	printf("\t-m n\tSet histogram size to n. Default %d\n", DEFAULT_HIST_SIZE);
	printf("\t-l\tRun in linear mode. No threading.\n");
	printf("\t-t\tRun tests.\n");
	printf("\t-h\tShow this help.\n");
	printf("\n\n");
}
