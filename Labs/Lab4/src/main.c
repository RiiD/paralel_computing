#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))


#define N 4

MPI_Comm comm;
int num;

void sort();
void sort2(MPI_Comm comm, int size);
void rowSort(MPI_Comm comm, int size);
void colSort(MPI_Comm comm, int size);

int main(int argc, char *argv[])
{
	 int rank, size;
     int dim[2], period[2], reorder;
     int coord[2];
	 int matrix[N * N];

     MPI_Init(&argc, &argv);
     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
     MPI_Comm_size(MPI_COMM_WORLD, &size);

     if (size != N*N) {
         printf("Please run with %d processes and two parametrs - row and column.\n", N*N);fflush(stdout);
         MPI_Abort(MPI_COMM_WORLD, 1);
     }

	 // A two-dimensional cylindr of 12 processes in a 4x3 grid //
     dim[0] = N; 
	 dim[1] = N;
     period[0] = 1; 
	 period[1] = 1;
     reorder = 1;
     MPI_Cart_create(MPI_COMM_WORLD, 2, dim, period, reorder, &comm);
      
	 // Each process displays its rank and cartesian coordinates
     MPI_Cart_coords(comm, rank, 2, coord);
	
	num = rand() % 9;
	MPI_Comm_rank(comm, &rank);
	
	if(rank == 0){
		int i, j;
		for(i = 0; i < N; i++){
			for(j = 0; j < N; j++){
				matrix[i * N + j] = i * N + j;
			}
		}
	}
	
	if(rank == 0){
		printf("Not sorted:\n");
		int i, j;
		for(i = 0; i < N; i++){
			for(j = 0; j < N; j++){
				printf("%d\t", matrix[i * N + j]);
			}
			printf("\n");
		}
	}
	
	MPI_Scatter(matrix, 1, MPI_INT, &num, 1, MPI_INT, 0, comm);
	
	sort2(comm, N);
	
	MPI_Gather(&num, 1, MPI_INT, matrix, 1, MPI_INT, 0, comm);
	if(rank == 0){
		printf("Sorted:\n");
		int i, j;
		for(i = 0; i < N; i++){
			for(j = 0; j < N; j++){
				printf("%d\t", matrix[i * N + j]);
			}
			printf("\n");
		}
	}
	MPI_Finalize();
}

void sort2(MPI_Comm comm, int size)
{
		for(int i = 0; i < 2 * log2(size) + 1; i++){
			if(i % 2 == 0)
			{
				rowSort(comm, size);
			}
			else
			{
				printf("Col sort start\n");
				colSort(comm, size);
			}
		}	
}

void rowSort(MPI_Comm comm, int size)
{
	int myRank, coord[2], col, row, destRank, buf;
	MPI_Comm_rank(comm, &myRank);
	MPI_Cart_coords(comm, myRank, 2, coord);
	row = coord[0];
	col = coord[1];
	
	for(int i = 0; i < size; i++)
	{
		if(col % 2 == 0)
		{
			if(row % 2 == 0 && row != size - 1)
			{
				int destCoord[] = {row, col + 1};
				MPI_Cart_rank(comm, destCoord, &destRank);
				MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
				MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
				num = MIN(num, buf);
			}else if(row != 0){
				int destCoord[] = {row, col - 1};
				MPI_Cart_rank(comm, destCoord, &destRank);
				MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
				MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
				num = MAX(num, buf);
			}
			
		}else{
			if(row % 2 == 0 && row != size - 1)
			{
				int destCoord[] = {row, col + 1};
				MPI_Cart_rank(comm, destCoord, &destRank);
				MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
				MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
				num = MAX(num, buf);
			}else if(row != 0){
				int destCoord[] = {row, col - 1};
				MPI_Cart_rank(comm, destCoord, &destRank);
				MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
				MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
				num = MIN(num, buf);
			}
		}
	}
}

void colSort(MPI_Comm comm, int size)
{
	int myRank, coord[2], col, row, destRank, buf;
	MPI_Comm_rank(comm, &myRank);
	MPI_Cart_coords(comm, myRank, 2, coord);
	row = coord[0];
	col = coord[1];
	
	for(int i = 0; i < size; i++)
	{
		if(row % 2 == 0)
			{
				int destCoord[] = {row + 1, col};
				MPI_Cart_rank(comm, destCoord, &destRank);
				MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
				MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
				num = MIN(num, buf);
			}else{
				int destCoord[] = {row - 1, col};
				MPI_Cart_rank(comm, destCoord, &destRank);
				MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
				MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
				num = MAX(num, buf);
			}
		}
	}


void sort()
{
	int count = 0;
	int myRank, destRank = 0;
	int buf;
	int coord[2];
	int destCoord[2];
	MPI_Comm_rank(comm, &myRank);
	
	MPI_Cart_coords(comm, myRank, 2, coord);
	
	for(count = 0; count < 2 * log2(N) + 1; count++){
		int step = 0;
		for(step = 0; step < 4; step++){
			if(step % 4 == 0)
			{
				if(coord[0] % 2 == 0&& coord[0]!= N-1)
				{
					destCoord[0] = (N + 1 + coord[0]) % N;
					destCoord[1] = coord[1];
					MPI_Cart_rank(comm, destCoord, &destRank);
					
					
					//printf("%d:\tsending to:\t%d\n", myRank, destRank);
					MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
					MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
					printf("step1");
					//printf("got %d have %d", buf, num);
					num = MIN(num, buf);
					
				}else if(coord[0]!=0)
					{
					destCoord[0] = (N - 1 + coord[0]) % N;
					destCoord[1] = coord[1];
					MPI_Cart_rank(comm, destCoord, &destRank);
					
					//printf("%d:\tRecv from:\t%d\n", myRank, destRank);
					MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
					MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
					num = MAX(num, buf);
					
				}
			
			}else if(step % 4 == 1){
				if(coord[0] % 2 == 1&&coord[0]!=N-1)
				{
					destCoord[0] = (N + 1 + coord[0]) % N;
					destCoord[1] = coord[1];
					MPI_Cart_rank(comm, destCoord, &destRank);
					
					MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
					MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
					num = MIN(num, buf);
				}else if(coord[0]!=0){
					destCoord[0] = (N - 1 + coord[0]) % N;
					destCoord[1] = coord[1];
					MPI_Cart_rank(comm, destCoord, &destRank);
					
					MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
					MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
					num = MAX(num, buf);
				}
			}else if(step % 4 == 2){
				if(coord[1] % 2 == 0 && coord[1]!=N-1)
				{
					destCoord[1] = (N + 1 + coord[1]) % N;
					destCoord[0] = coord[0];
					MPI_Cart_rank(comm, destCoord, &destRank);
					
					MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
					MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
					num = MIN(num, buf);
				}else if(coord[1]!=0){
					destCoord[1] = (N - 1 + coord[1]) % N;
					destCoord[0] = coord[0];
					MPI_Cart_rank(comm, destCoord, &destRank);
					
					MPI_Cart_shift(comm, 1, -1, &myRank, &destRank);
					MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
					MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
					num = MAX(num, buf);
				}
			}else{
				if(coord[1] % 2 == 1 && coord[1] != N-1)
				{
					destCoord[1] = (N + 1 + coord[1]) % N;
					destCoord[0] = coord[0];
					MPI_Cart_rank(comm, destCoord, &destRank);
					
					MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
					MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
					num = MIN(num, buf);
				}else if(coord[1] != 0){
					destCoord[1] = (N - 1 + coord[1]) % N;
					destCoord[0] = coord[0];
					MPI_Cart_rank(comm, destCoord, &destRank);
					
					MPI_Recv(&buf, 1, MPI_INT, destRank, 0, comm, MPI_STATUS_IGNORE);
					MPI_Send(&num, 1, MPI_INT, destRank, 0, comm);
					num = MAX(num, buf);
				}
			}
		}
	}
}
