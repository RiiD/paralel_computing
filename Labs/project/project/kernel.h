#pragma once

#include "points.h"

int cudaInit(const Point points[], const int n, const int pointsCount);
int runOnCUDA(double* distances, const int n, const int k, const int startPoint, const int pointsCount);
int cudaResult(double* distances, const int n, const int pointsCount);
int cudaFinalize();
