#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

void sort(MPI_Comm);
int oddEvenSort(MPI_Comm comm, int n,int myNum, int dim, int dir);
void printIntMatrix(int *matrix, int cols, int rows);
void generateRandomIntArray(int *numbers, int size);

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	
	sort(MPI_COMM_WORLD);
	
	MPI_Finalize();
}

void sort(MPI_Comm comm)
{
	int me, master = 0, commSize;
	int dims[2];
	int coords[2];
	int myNum;
	MPI_Comm cartComm;
	int periods[] = {0, 0};
	int *numbers, size;
	
	MPI_Comm_size(comm, &commSize);
	
	size = sqrt(commSize);
		
	dims[0] = dims[1] = size;
	
	MPI_Cart_create(comm, 2, dims, periods, 0, &cartComm);
	
	if(cartComm == MPI_COMM_NULL)
	{
		return;
	}
	
	MPI_Comm_rank(cartComm, &me);
	
	if(me == master)
	{
		numbers = (int*)malloc(pow(size, 2) * sizeof(int));
		generateRandomIntArray(numbers, pow(size, 2));
		printf("Initial matrix:\n");
		printIntMatrix(numbers, size, size);
	}
	
	MPI_Scatter(numbers, 1, MPI_INT, &myNum, 1, MPI_INT, master, cartComm);
	
	MPI_Cart_coords(cartComm, me, 2, coords);
	
	for(int i = 0; i < log2(size) + 1; i++)
	{
		if(coords[0] % 2 == 0)
		{
			myNum = oddEvenSort(cartComm, size, myNum, 1, 0);
		}
		else
		{
			myNum = oddEvenSort(cartComm, size, myNum, 1, 1);
		}
		
		myNum = oddEvenSort(cartComm, size, myNum, 0, 0);
	}
	
	MPI_Gather(&myNum, 1, MPI_INT, numbers, 1, MPI_INT, master, cartComm);
	
	if(me == master)
	{
		printf("Final matrix:\n");
		printIntMatrix(numbers, size, size);
		free(numbers);
	}
}

int oddEvenSort(MPI_Comm comm, int n,int myNum, int dim, int dir)
{
	int coords[2];
	int prev, next, me;
	int buf;
	
	MPI_Comm_rank(comm, &me);
	
	MPI_Cart_coords(comm, me, 2, coords);
	
	MPI_Cart_shift(comm, dim, 1, &prev, &next);
	
	for(int i = 0; i < n; i += 2)
	{
		if(coords[dim] % 2 == 0)
		{
			if(next != -1)
			{
				MPI_Send(&myNum, 1, MPI_INT, next, 0, comm);
				MPI_Recv(&buf, 1, MPI_INT, next, 0, comm, MPI_STATUS_IGNORE);
				myNum = dir ? MAX(myNum, buf) : MIN(myNum, buf);
			}
			
			if(prev != -1)
			{
				MPI_Send(&myNum, 1, MPI_INT, prev, 0, comm);
				MPI_Recv(&buf, 1, MPI_INT, prev, 0, comm, MPI_STATUS_IGNORE);
				myNum = dir ? MIN(myNum, buf) : MAX(myNum, buf);
			}
		}
		else
		{
			if(prev  != -1)
			{
				MPI_Recv(&buf, 1, MPI_INT, prev, 0, comm, MPI_STATUS_IGNORE);
				MPI_Send(&myNum, 1, MPI_INT, prev, 0, comm);
				myNum = dir ? MIN(myNum, buf) : MAX(myNum, buf);
			}
			
			if(next  != -1)
			{
				MPI_Recv(&buf, 1, MPI_INT, next, 0, comm, MPI_STATUS_IGNORE);
				MPI_Send(&myNum, 1, MPI_INT, next, 0, comm);
				myNum = dir ? MAX(myNum, buf) : MIN(myNum, buf);
			}
		}
	}
	return myNum;
}

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

void generateRandomIntArray(int *numbers, int size)
{
	for(int i = 0; i < size; i++)
	{
		numbers[i] = rand() % 10;
	}
}
