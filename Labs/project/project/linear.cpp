#include <stdlib.h>

#include "points.h"
#include "utils.h"

/**
 * Calculates distances matrix. Very simple.
 * TODO: Optimize!
 *
 * @param Point[] points
 * @param int n
 * @param double[] distances
 */
void linearDinstanceCalculate(Point points[], int n, double distances[]) {
	int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			distances[j + i * n] = distance(&(points[i]), &(points[j]));
		}
	}
}
