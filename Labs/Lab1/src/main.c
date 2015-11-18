/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
 
/* This is an interactive version of cpi */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
 
#define NEGATIVE -1
#define ZERO 0
#define POSITIVE 1
#define DEBUG 1
 
void q3a();
void q3b();
void q3c();
 
int main(int argc,char *argv[])
{
     
     
    int myId;
 
    MPI_Init(&argc,&argv);
 
    q3c();
 
    MPI_Finalize();
 
    return 0;
}
 
void q3a(){
    int myId;
 
    MPI_Comm_rank(MPI_COMM_WORLD,&myId);
 
    printf("My rank is: %d\n", myId);
}
 
void q3b(){
    int myId;
    int x;
    int y;
    int answer;
 
    x = rand();
    y = rand();
    MPI_Comm_rank(MPI_COMM_WORLD,&myId);
 
    MPI_Status status;
 
    if(myId == 0){
        printf("Rank 0: Sending x = %d to rank 1\n", x);
        MPI_Send(&x, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("Rank 0: Sent x = %d to rank 1\n", x);
 
        printf("Rank 0: Waiting for rank 1...\n");
        MPI_Recv(&answer, 1, MPI_INT, 1, 0,MPI_COMM_WORLD, &status);
        printf("Rank 0: Got answer %d from rank 1\n", answer);
    }else{
        printf("Rank 1: Waiting for rank 0...\n");
        MPI_Recv(&x, 1, MPI_INT, 0, 0,MPI_COMM_WORLD, &status);
        printf("Rank 1: Got x = %d from rank 0\n", x);
        answer = x * y;
        printf("Rank 1: Sending answer = %d to rank 0\n", answer);
        MPI_Send(&answer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("Rank 1: Sent answer = %d to rank 0\n", answer);
    };
 
}
 
void q3c(){
    int myId, num;
    MPI_Status status;
 
    MPI_Comm_rank(MPI_COMM_WORLD,&myId);
 
    if(myId == 0){
        int negatives = 0, positives = 0,  zeros = 0, numOfProcs, i;
 
        MPI_Comm_size(MPI_COMM_WORLD,&numOfProcs);  
        if(DEBUG)
            printf("Rank %d: Number of slaves: %d\n", myId, numOfProcs);
        for(i = 1; i < numOfProcs; i++){
            num = rand() % 100 - 50;
            if(DEBUG)
                printf("Rank %d: Sending to %d\n", myId, i);
            MPI_Send(&num, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            if(DEBUG)
                printf("Rank %d: Sent to %d\n", myId, i);
        }
 
        for(i = 1; i < numOfProcs; i++){
            if(DEBUG)
                printf("Rank %d: Waiting for %d\n", myId, i);
            MPI_Recv(&num, 1, MPI_INT, i, 0,MPI_COMM_WORLD, &status);
            if(DEBUG)
                printf("Rank %d: Got answer from %d\n", myId, i);
            switch(num){
            case POSITIVE:
                positives++;
                break;
            case NEGATIVE:
                negatives++;
                break;
            case ZERO:
                zeros++;
                break;
            }
        }
        if(DEBUG)
            printf("Positives: %d\nNegatives: %d\nZeros: %d\n", positives, negatives, zeros);
    }else{
        if(DEBUG)
            printf("Rank %d: Waiting for %d\n", myId, 0);
        MPI_Recv(&num, 1, MPI_INT, 0, 0,MPI_COMM_WORLD, &status);
        if(DEBUG)
            printf("Rank %d: Got number from %d\n", myId, 0);
        if(num < 0){
            num = NEGATIVE;
        }else if(num > 0){
            num = POSITIVE;
        }else{
            num = ZERO;
        }
        if(DEBUG)
            printf("Rank %d: Sending to %d\n", myId, 0);
        MPI_Send(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        if(DEBUG)
            printf("Rank %d: Sent to %d\n", myId, 0);
    }
}
