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
void sortLines(double *distances, int n);
int quick_select_index(double* input, int p, int r, int k);
int quick_select(double* input, int p, int r, int k);
