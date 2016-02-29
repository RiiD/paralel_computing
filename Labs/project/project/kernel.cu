
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "kernel.h"
#include "utils.h"

#define MAX_THREADS_DIM 32

__global__ void distanceKernel(Point points[], int n, double* distances, int startX, int startY) {
	int x = blockIdx.x * blockDim.x + threadIdx.x + startX;
	int y = blockIdx.y * blockDim.y + threadIdx.y + startY;

	if (x < n && y < n) {
		distances[x + y * n] = distances[x * n + y] = pow(points[x].x - points[y].x, 2) + pow(points[x].y - points[y].y, 2);
	}
}

int runOnCUDA(Point points[], int n, double* distances, int startX, int startY, int winSize) {
	Point *dev_points;
	double *dev_distances, *tmp_distances;
	cudaError_t cudaStatus;
	int i, blocks, threads;

	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		return 1;
	}

	cudaStatus = cudaMalloc((void**)&dev_points, n * sizeof(Point));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		return 1;
	}

	cudaStatus = cudaMalloc((void**)&dev_distances, n * n * sizeof(double));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		return 1;
	}

	// Copy input vectors from host memory to GPU buffers.
	cudaStatus = cudaMemcpy(dev_points, points, n * sizeof(Point), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed! 1");
		return 1;
	}

	cudaStatus = cudaMemcpy(dev_distances, distances, n * n * sizeof(double), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed! 2");
		return 1;
	}

	blocks = (int)ceil((double) winSize / MAX_THREADS_DIM);
	threads = (int)fmin((double)MAX_THREADS_DIM, (double)winSize);

	printf("Blocks: %d, Threads: %d", blocks, threads);

	// Launch a kernel on the GPU with one thread for each element.
	distanceKernel << <dim3(blocks, blocks), dim3(threads, threads) >> >(dev_points, n, dev_distances, startX, startY);

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "distanceKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		return 1;
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		return 1;
	}

	tmp_distances = (double*) malloc(n * n * sizeof(double));

	// Copy output vector from GPU buffer to host memory.
	cudaStatus = cudaMemcpy(tmp_distances, dev_distances, n * n * sizeof(double), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		return 1;
	}

	cudaFree(dev_points);
	cudaFree(dev_distances);

	for (i = startY; i < startY + winSize; i++) {
		memcpy(distances + i * n + startX, tmp_distances + i * n + startX, winSize * sizeof(double));
	}

	for (i = startX; i < startX + winSize; i++) {
		memcpy(distances + i * n + startY, tmp_distances + i * n + startY, winSize * sizeof(double));
	}

	free(tmp_distances);

	// cudaDeviceReset must be called before exiting in order for profiling and
	// tracing tools such as Nsight and Visual Profiler to show complete traces.
	cudaStatus = cudaDeviceReset();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceReset failed!");
		return 1;
	}

	return cudaStatus;
}
