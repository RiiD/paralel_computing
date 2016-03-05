#pragma once

#include<stdio.h>

#include "config.h"

#ifdef _WIN32
	#define NEWLINE "\r\n"
#elif defined macintosh // OS 9
	#define NEWLINE "\r"
#else
	#define NEWLINE "\n" // Mac OS X uses \n
#endif

void printDoubleArray(double* arr, int size, int itemsPerLine);
void printArray(void** array, int arraySize, int typeSize, int itemsPerLine, void(*print)(void* item));
void printIntArray(int arr[], int size, int itemsPerLine);
void sortKElements(double arr[], int dest[], int n, int k);
