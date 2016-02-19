#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void doublePrinter(void* num);
int doubleComparator(const void* a, const void* b);

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
* Sorts each line of distances matrix using quick sort.
*
* @params double *distances Distances matrix.
* @params int n Size of demnsions of distances matrix.
*/
void sortLines(double *distances, int n) {
	int i;

	for (i = 0; i < n; i++) {
		qsort(distances + i * n, n, sizeof(double), doubleComparator);
	}
}

/**
* Double comparator.
* Retruns 1 if a > b
*        -1 if a < b
*         0 if a == b.
*
* @param const void* a
* @param const void* b
* @returns int
*/
int doubleComparator(const void* a, const void* b) {
	double *k, *w;
	k = (double*)a;
	w = (double*)b;
	if (*k < *w)
		return -1;
	else if (*k > *w)
		return 1;
	else
		return 0;
}
//
//// Quick select code source: http://www.sourcetricks.com/2011/06/quick-select
//
///**
// * 
// */
//int partition(void* input, int p, int r, void (*compare)(void))
//{
//	double pivot = input[r];
//
//	while (p < r)
//	{
//		while (input[p] < pivot)
//			p++;
//
//		while (input[r] > pivot)
//			r--;
//
//		if (input[p] == input[r])
//			p++;
//		else if (p < r) {
//			int tmp = input[p];
//			input[p] = input[r];
//			input[r] = tmp;
//		}
//	}
//
//	return r;
//}
//
///**
//* Quick select algorithm.
//*/
//int quick_select(double* input, int p, int r, int k)
//{
//	if (p == r) return input[p];
//	int j = partition(input, p, r);
//	int length = j - p + 1;
//	if (length == k) return input[j];
//	else if (k < length) return quick_select(input, p, j - 1, k);
//	else  return quick_select(input, j + 1, r, k - length);
//}