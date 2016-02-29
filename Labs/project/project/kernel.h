#pragma once

#include "points.h"

int runOnCUDA(Point points[], int n, double* distances, int startX, int startY, int winSize);