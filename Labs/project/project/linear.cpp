#include <stdlib.h>

#include "points.h"
#include "utils.h"

/**
 * Calculates distances matrix. Very simple.
 *
 * @param Point[] points
 * @param int n
 * @param double[] distances
 */
void linearDinstanceCalculate(Point points[], int n, double distances[]) {
	int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j <= i; j++) {
			distances[j + i * n] = distances[j * n + i] = distance(&(points[i]), &(points[j]));
		}
	}
}
