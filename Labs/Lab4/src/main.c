#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#define MAX_RANDOM 100

void sort(MPI_Comm);
int oddEvenSort(MPI_Comm comm, int n,int myNum, int dim, int dir);
void printIntMatrix(int *matrix, int cols, int rows);
void generateRandomIntArray(int *numbers, int size, int max);
void showHelp();

int maxRandom = MAX_RANDOM;

/**
 * App entry point
 */
int main(int argc, char *argv[])
{
	int me;
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &me);
	for(int i = 1; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case 'r':
					maxRandom = (int) strtol(argv[++i], (char **)NULL, 10);
					break;
				case 'h':
					if(me == 0)
					{
						showHelp();
					}
					MPI_Finalize();
					return 0;
			}
		}
	}
	
	sort(MPI_COMM_WORLD);
	
	MPI_Finalize();
}

/**
 * This function creates quadratic matrix which fits into given comm.
 * It creates cartesian comm of same size as the matrix.
 * @param MPI_Comm comm Communicator to work in.
 */
void sort(MPI_Comm comm)
{
	int me,
		master = 0,
		commSize,
		dims[2],
		coords[2],
		myNum,
		periods[] = {0, 0},
		*numbers = 0, // Initialized to avoid warnigns
		size;
		
	MPI_Comm cartComm;
	
	// Create quadratic matrix comm which fits in given comm.
	MPI_Comm_size(comm, &commSize);
	
	size = sqrt(commSize);
	
	dims[0] = dims[1] = size;
	
	MPI_Cart_create(comm, 2, dims, periods, 0, &cartComm);
	
	if(cartComm == MPI_COMM_NULL)
	{
		return;
	}
	
	MPI_Comm_rank(cartComm, &me);
	
	// If master allocate space, generate array and print it
	if(me == master)
	{
		numbers = (int*)malloc( (int)pow(size, 2) * sizeof(int) );
		generateRandomIntArray(numbers, (int)pow(size, 2), maxRandom);
		printf("Initial matrix:\n");
		printIntMatrix(numbers, size, size);
	}
	
	// Send the numbers to everyone
	MPI_Scatter(numbers, 1, MPI_INT, &myNum, 1, MPI_INT, master, cartComm);
	
	// Get my coords
	MPI_Cart_coords(cartComm, me, 2, coords);
	
	for(int i = 0; i < log2(size) + 1; i++)
	{
		// Row sort step
		if(coords[0] % 2 == 0)
		{
			myNum = oddEvenSort(cartComm, size, myNum, 1, 0);
		}
		else
		{
			myNum = oddEvenSort(cartComm, size, myNum, 1, 1);
		}
		
		// Column sort step
		myNum = oddEvenSort(cartComm, size, myNum, 0, 0);
	}
	
	// Get results
	MPI_Gather(&myNum, 1, MPI_INT, numbers, 1, MPI_INT, master, cartComm);
	
	// If master print results and free allocated memory
	if(me == master)
	{
		printf("Final matrix:\n");
		printIntMatrix(numbers, size, size);
		free(numbers);
	}
}

/**
 * Does the sorting work for each proc in matrix. 
 * Comm must be initialized as cartesian comm.
 * @param MPI_Comm comm Communicator to wirk in
 * @param int n Size of matrix(quadratic)
 * @param int myNum Current number of current proc
 * @param int dim Dimension to sort: 1 row sort, 0 col sort
 * @param int dir Sort direction: 0 ascending, 1 descending
 */
int oddEvenSort(MPI_Comm comm, int n,int myNum, int dim, int dir)
{
	int coords[2];
	int prev, next, me;
	int buf;
	
	MPI_Comm_rank(comm, &me);
	
	MPI_Cart_coords(comm, me, 2, coords);
	
	MPI_Cart_shift(comm, dim, 1, &prev, &next);
	
	// Increment i twice because we do 2 steps each iteration
	for(int i = 0; i < n; i += 2)
	{
		if(coords[dim] % 2 == 0)
		{
			// Send to next proc in given dimension
			if(next != -1)
			{
				MPI_Sendrecv(&myNum, 1, MPI_INT, next, 0, &buf, 1, MPI_INT, next, 0, comm, MPI_STATUS_IGNORE);
				myNum = dir ? MAX(myNum, buf) : MIN(myNum, buf);
			}
			
			if(prev != -1)
			{
				MPI_Sendrecv(&myNum, 1, MPI_INT, prev, 0, &buf, 1, MPI_INT, prev, 0, comm, MPI_STATUS_IGNORE);
				myNum = dir ? MIN(myNum, buf) : MAX(myNum, buf);
			}
		}
		else
		{
			if(prev  != -1)
			{
				MPI_Sendrecv(&myNum, 1, MPI_INT, prev, 0, &buf, 1, MPI_INT, prev, 0, comm, MPI_STATUS_IGNORE);
				myNum = dir ? MIN(myNum, buf) : MAX(myNum, buf);
			}
			
			if(next  != -1)
			{
				MPI_Sendrecv(&myNum, 1, MPI_INT, next, 0, &buf, 1, MPI_INT, next, 0, comm, MPI_STATUS_IGNORE);
				myNum = dir ? MAX(myNum, buf) : MIN(myNum, buf);
			}
		}
	}
	return myNum;
}


/**
 * Prints array as matrix.
 * @param int* matrix Array
 * @param int cols
 * @param int rows
 */
void printIntMatrix(int *matrix, int cols, int rows)
{
	int i, j;
	for(i = 0; i < rows; i++)
	{
		for(j = 0; j < cols; j++)
		{
			printf("%d\t", matrix[i * cols + j]);
		}
		printf("\n");
	}
}

/**
 * Fills array with random integers.
 * @param int* numbers
 * @param int size
 */
void generateRandomIntArray(int *numbers, int size, int max)
{
	for(int i = 0; i < size; i++)
	{
		numbers[i] = rand() % max;
	}
}

/**
 * Shows help.
 * 
 */
void showHelp()
{
	printf("\nLab 4 for parallel computing course.\n");
	printf("\n\tUsage:\n");
	printf("\n\t\tmpirun -n [numofprocs] main [-r maxRandom].\n");
	printf("\n\tParams:\n");
	printf("\n\t\t-r\tSet max random for random int generator. Default 100\n\n");
}
