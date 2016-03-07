#pragma once

#include "points.h"

int cudaInit(const Point points[], int n, int pointsCount);
int runOnCUDA(int n, int startPoint, int pointsCount);
int cudaResult(double* distances, int n, int pointsCount);
int cudaFinalize();
