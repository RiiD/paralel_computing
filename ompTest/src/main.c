#include <omp.h>
#include <stdio.h>

int main(int *argc, char *argv[])
{
	int numOfThreads, threadId, numOfProcs;
	
	numOfProcs = omp_get_num_procs();
	
	printf("Num of procs: %d\n", numOfProcs);
	
	#pragma omp parallel private(threadId)
	{
		threadId = omp_get_thread_num();
		printf("%d\n", threadId);
		
		if(threadId == 0)
		{
			numOfThreads = omp_get_num_threads();
			printf("Count: %d\n", numOfThreads);
		}
	}
}
