#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

struct Entry {
	int key;
	double value;
};

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

		printf("%5d", arr[i]);
	}
}

int entryValueComparator(const void* e1, const void* e2) {
	Entry *ce1, *ce2;
	ce1 = (Entry*)e1;
	ce2 = (Entry*)e2;

	return doubleComparator((void*)&ce1->value, (void*)&ce2->value);
}

void getSortedIndexes(double arr[], int dest[], int size) {
	Entry *entries;
	int i;

	entries = (Entry*)malloc(size * sizeof(Entry));

	for (i = 0; i < size; i++) {
		entries[i].key = i;
		entries[i].value = arr[i];
	}

	qsort(entries, size, sizeof(Entry), entryValueComparator);

	for (i = 0; i < size; i++) {
		dest[i] = entries[i].key;
	}

	free(entries);
}
