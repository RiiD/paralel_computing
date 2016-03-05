#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "utils.h"

struct Entry {
	int key;
	double value;
};

/**
 * Generic array printer.
 * @param void** array
 * @param int arraySize
 * @param int typeSize
 * @param callable* print(void* item)
 */
void printArray(void** array, int arraySize, int typeSize, int itemsPerLine, void (*print)(void* item)) {
	int i;
	for (i = 0; i < arraySize; i++) {
		
		if (i && i % itemsPerLine == 0) {
			printf(NEWLINE);
		}

		print(array + i * typeSize / sizeof(void*));
		printf("\t");
	}
}

/**
 * Prints single double.
 * @param void** num
 */
void doublePrinter(void* num) {
	double* dNum = (double*)num;
	printf("%.2f", *dNum);
}

/**
 * Prints array of doubles.
 * 
 * @param double* arr
 * @param int size
 */
void printDoubleArray(double arr[], int size, int itemsPerLine) {
	printArray((void**)arr, size, sizeof(double), itemsPerLine, doublePrinter);
}

/**
* Prints array of ints.
*
* @param int* arr
* @param int size
*/
void printIntArray(int arr[], int size, int itemsPerLine) {
	int i;
	for (i = 0; i < size; i++) {
		
		if (i && i % itemsPerLine == 0) {
			printf(NEWLINE);
		}

		printf("%10d", arr[i]);
	}
}

void sortKElements(double arr[], int dest[], int n, int k)
{
	int i, j, min_idx;

	// One by one move boundary of unsorted subarray
	for (i = 0; i < k; i++)
	{
		// Find the minimum element in unsorted array
		min_idx = i;
		for (j = i + 1; j < n; j++)
			if (arr[j] < arr[min_idx])
				min_idx = j;

		// Swap the found minimum element with the first element
		arr[min_idx] = DBL_MAX;
		dest[i] = min_idx;
	}
}