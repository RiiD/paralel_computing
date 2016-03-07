#include <math.h>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include "kernel.h"
#include "utils.h"
#include "config.h"

static Point *dev_points;
static double *dev_distances;
static cudaError_t cudaStatus;

/**
 * Calculates distances from one point to another. Runs on CUDA device.
 * @param const Point* points
 * @param double* distances
 * @param int n
 * @param int startPoint
 */
__global__ void distanceKernel(const Point points[], double distances[], int n, int startPoint) {
	int y = blockIdx.y;
	int x = blockIdx.x * blockDim.x + threadIdx.x;

	if (x < n) {
		distances[y * n + x] =
			(points[x].x - points[y + startPoint].x) * 
			(points[x].x - points[y + startPoint].x) + 
			(points[x].y - points[y + startPoint].y) * 
			(points[x].y - points[y + startPoint].y);
	}
}

/**
 * Initializes CUDA device. Allocates memory.
 * @param const Point* points
 * @param int n
 * @param int pointsCount
 * @returns int Cuda status code
 */
int cudaInit(const Point points[], int n, int pointsCount) {

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

	cudaStatus = cudaMalloc((void**)&dev_distances, pointsCount * n * sizeof(double));
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

	return cudaStatus;
}

/**
 * Starts calculating distances for given number of points starting from statPoints on CUDA device.
 * @param double* distances
 * @param int n
 * @param int k
 * @param int startPoint
 * @param int pointsCount
 * @returns int Cuda status code
 */
int runOnCUDA(int n, int startPoint, int pointsCount) {
	// Launch a kernel on the GPU with one thread for each element.
	distanceKernel << < dim3((unsigned)ceil(n / CUDA_THREADS_PER_BLOCK), (unsigned)pointsCount), (unsigned)CUDA_THREADS_PER_BLOCK >> >(dev_points, dev_distances, n, startPoint);

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "distanceKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		return 1;
	}

	return cudaStatus;
}

/**
 * Retrieves results of last distances calculation on cuda.
 * @param double* distances Results destination
 * @param int n
 * @param int pointsCount
 * @returns int Cuda status
 */
int cudaResult(double* distances, int n, int pointsCount) {
	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		return 1;
	}

	// Copy output vector from GPU buffer to host memory.
	cudaStatus = cudaMemcpy(distances, dev_distances, pointsCount * n * sizeof(double), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		return 1;
	}

	return cudaStatus;
}

/**
 * Releases allocated memory and finalizes GPU.
 */
int cudaFinalize() {
	cudaFree(dev_points);
	cudaFree(dev_distances);

	// cudaDeviceReset must be called before exiting in order for profiling and
	// tracing tools such as Nsight and Visual Profiler to show complete traces.
	cudaStatus = cudaDeviceReset();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceReset failed!");
		return 1;
	}

	return cudaStatus;
}
