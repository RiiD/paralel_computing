#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "points.h"
#include "utils.h"

#define ROW_BUF_SIZE 255
#define POINT_AXIS_SEPARATOR " "

void deserializePoint(char* serializedPoint, Point* dest);
void serializePoint(Point* point, char* dest);
void generatePoint(Point* dest, int id, double maxX, double maxY);

/**
* Loads points from given file.
* DON'T FORGET TO FREE RETURNED MEMORY POINTER!!!
*
* @param char* fileName
* @param int* n - Number of ponits will be whiten here
* @param int* k - Number of nearest points will be writen here
* @returns Point* - Points array
*/
Point* loadPoints(char* fileName, int *n, int *k) {
	FILE *fh;
	char buf[ROW_BUF_SIZE], *str;
	int i;
	Point* points;

	fh = fopen(fileName, "r");

	if (fh == NULL) {
		printf("Failed to open the file %s!%s", fileName, NEWLINE);
		return NULL;
	}

	str = fgets(buf, ROW_BUF_SIZE, fh);

	*n = atoi(strtok(str, POINT_AXIS_SEPARATOR));
	*k = atoi(strtok(NULL, POINT_AXIS_SEPARATOR));

	points = (Point*) malloc(*n * sizeof(Point));

	for (i = 0; i < (*n); i++) {
		str = fgets(buf, ROW_BUF_SIZE, fh);
		deserializePoint(str, &points[i]);
	}

	fclose(fh);

	return points;
}

/**
* Deserializes point.
*
* @param char* serializedPoint
* @param Point* dest
*/
void deserializePoint(char* serializedPoint, Point* dest) {
	dest->id = atoi(strtok(serializedPoint, POINT_AXIS_SEPARATOR));

	dest->x = atof(strtok(NULL, POINT_AXIS_SEPARATOR));
	dest->y = atof(strtok(NULL, POINT_AXIS_SEPARATOR));
}

/**
 * Serializes point.
 *
 * @param Point* point
 * @param char* dest
 */
void serializePoint(Point* point, char* dest) {
	sprintf(dest, "%d %f %f", point->id, point->x, point->y);
}

/**
 * Calculates distance between 2 points.
 * For better performance this function doesn't calculates real distance. 
 * Instead it returns distance^2.
 *
 * @param Point* point1
 * @param Point* point2
 * @param Distance* distance
 */
void distance(Point* point1, Point* point2, Distance* distance) {
	distance->distance = pow(point1->x - point2->x, 2) + pow(point1->y - point2->y, 2);
	distance->p1 = point1;
	distance->p2 = point2;
}

/**
 * Generates points. Dest should be n*2*sizeof(double) bytes.
 *
 * @param int n - number of points to generate
 * @param Point[] dest
 * @param double maxX
 * @param double maxY
 */
void generatePoints(int n, Point dest[], double maxX, double maxY) {
	int i;
	srand(time(NULL));

	for (i = 0; i < n; i++) {
		generatePoint(&(dest[i]), i, maxX, maxY);
	}
}

/**
 * Generates point.
 * @param Point* dest
 * @param int id
 * @param double maxX
 * @param double maxY
 */
void generatePoint(Point* dest, int id, double maxX, double maxY) {
	dest->x = (double)rand() / ((double)RAND_MAX / maxX);
	dest->y = (double)rand() / ((double)RAND_MAX / maxY);
	dest->id = id;
}

/**
 * Saves points to file.
 *
 * @param char* fileName
 * @param Point* points
 * @param int n - Number of points
 * @param int k - Number of nearest points.
 */
void savePoints(char* fileName, Point* points, int n, int k) {
	FILE *fh;
	char buf[ROW_BUF_SIZE];
	int i;

	fh = fopen(fileName, "w");

	fprintf(fh, "%d %d%s", n, k, NEWLINE);

	for (i = 0; i < n; i++) {
		serializePoint(&points[i], buf);
		fputs(buf, fh);
		fputs(NEWLINE, fh);
	}

	fclose(fh);
}

/**
 * Prints a point.
 * @param void* point - Pointer to a Point.
 */
void printPoint(void* point) {
	Point *dPoint = (Point*)point;
	printf("%d: (%.2f, %.2f)", dPoint->id, dPoint->x, dPoint->y);
}

void printDistance(void* distance) {
	Distance* cDistance = (Distance*)distance;
	printf("%.2f", cDistance->distance);
}

void printDistances(Distance distances[], int size, int colsPerRow) {
	printArray((void**)distances, size, sizeof(Distance), colsPerRow, printDistance);
}

void printDistanceIndex(void* distance) {
	Distance* cDistance = (Distance*)distance;
	printf("%3d", cDistance->p2->id);
}

void printDistancesIndices(Distance distances[], int size, int colsPerRow) {
	printArray((void**)distances, size, sizeof(Distance), colsPerRow, printDistanceIndex);
}

int distanceComparator(const void* d1, const void* d2) {
	Distance *cd1, *cd2;
	cd1 = (Distance*) d1;
	cd2 = (Distance*) d2;
	return doubleComparator((void*)(&cd1->distance), (void*)(&cd2->distance));
}

void sortDistances(Distance distances[], int size) {
	qsort(distances, size, sizeof(Distance), distanceComparator);
}
