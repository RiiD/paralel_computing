#pragma once

#include "points.h"

int cudaInit(Point points[], int n, int pointsCount);
int runOnCUDA(double* distances, int n, int k, int startPoint, int pointsCount);
int cudaResult(double* distances, int n, int pointsCount);
int cudaFinalize();