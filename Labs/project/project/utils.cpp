#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "utils.h"

/**
 * Generic array printer.
 * @param const void** array
 * @param const int arraySize
 * @param const int typeSize
 * @param callable* print(const void* item)
 */
void printArray(const void** array, const int arraySize, const int typeSize, const int itemsPerLine, void (*print)(const void* item)) {
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
 * @param const void** num
 */
void doublePrinter(const void* num) {
	double* dNum = (double*)num;
	printf("%.2f", *dNum);
}

/**
 * Prints array of doubles.
 * 
 * @param const double* arr
 * @param const int size
 */
void printDoubleArray(const double arr[], const int size, const int itemsPerLine) {
	printArray((const void**)arr, size, sizeof(double), itemsPerLine, doublePrinter);
}

/**
* Prints array of ints.
*
* @param const int* arr
* @param const int size
*/
void printIntArray(const int arr[], const int size, const int itemsPerLine) {
	int i;
	for (i = 0; i < size; i++) {
		
		if (i && i % itemsPerLine == 0) {
			printf(NEWLINE);
		}

		printf("%10d", arr[i]);
	}
}

/**
 * Generates array of indexes of smallest elemnts in array.
 * @param double* arr
 * @param int* dest
 * @param const int n
 * @param const int k
 */
void sortKElements(double arr[], int dest[], const int n, const int k)
{
	int i, j, min_idx;

	for (i = 0; i < k; i++)
	{
		min_idx = i;
		for (j = i + 1; j < n; j++)
			if (arr[j] < arr[min_idx])
				min_idx = j;

		arr[min_idx] = DBL_MAX;
		dest[i] = min_idx;
	}
}
