#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1
		
void q1();	
void circular();
void q3();
void q4a();
void q4b();

int main(int argc,char *argv[])
{     
    int myId;
 
    MPI_Init(&argc,&argv);
 
	q1();
 
    MPI_Finalize();
 
    return 0;
}

void q1()
{
	int rank, x, y, procs, dest, src;
	MPI_Status status;
	
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&procs);
	
	x = rank;
	
	dest = (rank + 1) % procs;
	src = (procs + rank - 1) % procs;
	
	printf("[%d] Sending %d to %d and receiving from %d.\n", rank, x, dest, src);
	MPI_Send(&x, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
	MPI_Recv(&y, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
	printf("[%d] Sent to %d, received from %d: %d.\n", rank, dest, src, y);
}

void circular()
{
	int rank, x, y, procs, dest, src;
	MPI_Status status;
	
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&procs);
	
	x = rank;
	
	dest = (rank + 1) % procs;
	src = (procs + rank - 1) % procs;
	
	printf("[%d] Sending %d to %d and receiving from %d.\n", rank, x, dest, src);
	MPI_Sendrecv(&x, 1, MPI_INT, dest, 0, &y, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
	printf("[%d] Sent to %d, received from %d: %d.\n", rank, dest, src, y);
	
}

void q3()
{
	int rank, x, y, procs, dest, src;
	MPI_Request sendRequest, recvRequest;
	MPI_Status status;
	
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&procs);
	
	x = rank;
	
	dest = (rank + 1) % procs;
	src = (procs + rank - 1) % procs;
	
	printf("[%d] Sending %d to %d and receiving from %d.\n", rank, x, dest, src);
	MPI_Irecv(&y, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &recvRequest);
	MPI_Isend(&x, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, &sendRequest);
	
	MPI_Wait(&sendRequest, &status);
	MPI_Wait(&recvRequest, &status);
	
	printf("[%d] Sent to %d, received from %d: %d.\n", rank, dest, src, y);

}

void q4a()
{
	int rank, x, procs, dest, src, master = 0;
	MPI_Status status;
	
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&procs);
	
	if(rank == master)
	{
		for(dest = 1; dest < procs; dest++)
		{
			x = rank;
			printf("[%d] Sending to %d: %d\n", rank, dest, x);
			MPI_Send(&x, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
		}
	}
	else
	{
		MPI_Recv(&x, 1, MPI_INT, master, 0, MPI_COMM_WORLD, &status);
		printf("[%d] Received from %d: %d\n", rank, master, x);
	}
}

void q4b()
{
	int rank, x, y, procs, dest, src;
	MPI_Status status;
	
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&procs);
	
	dest = (rank + 1) % procs;
	src = (procs + rank - 1) % procs;
	
	//printf("[%d] SRC: %d, DEST: %d\n", rank, src, dest);
	
	if(!(rank % 2))
	{
		x = rank;
		printf("[%d] Sending to %d: %d\n", rank, dest, x);
		MPI_Send(&x, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
	}
	
	if(rank % 2 || (rank == 0 && procs % 2))
	{
		MPI_Recv(&x, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
		printf("[%d] Received from %d: %d\n", rank, src, x);
	}
}

void q4c()
{
	int rank, x, procs, dest, src, master;
	MPI_Status status;
	
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&procs);
	
	master = procs - 1;
	
	dest = master;
	
	if(rank == master)
	{
		for(src = 0; src < master; src++)
		{
			x = rank;
			MPI_Recv(&x, 1, MPI_INT, master, 0, MPI_COMM_WORLD, &status);
			printf("[%d] Received from %d: %d\n", rank, master, x);
		}
	}
	else
	{
		printf("[%d] Sending to %d: %d\n", rank, dest, x);
		MPI_Send(&x, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
	}
}
