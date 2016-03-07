#include "points.h"

/**
 * Calculates distances matrix. Very simple.
 * TODO: Optimize!
 *
 * @param const Point[] points
 * @param const int n
 * @param double[] distances
 */
void linearDinstanceCalculate(const Point points[], const int n, double distances[]) {
	int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			distances[j + i * n] = distance(&(points[i]), &(points[j]));
		}
	}
}
