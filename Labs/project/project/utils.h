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

void printDoubleArray(const double* arr, const int size, const int itemsPerLine);
void printIntArray(const int arr[], const int size, const int itemsPerLine);
void sortKElements(double arr[], int dest[], const int n, const int k);
