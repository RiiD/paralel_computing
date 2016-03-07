#include "points.h"

/**
 * Calculates distances matrix. Very simple.
 *
 * @param const Point[] points
 * @param int n
 * @param double[] distances
 */
void linearDinstanceCalculate(const Point points[], int n, double distances[]) {
	int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j <= i; j++) {
			distances[j + i * n] = distances[j * n + i] = distance(&(points[i]), &(points[j]));
		}
	}
}
