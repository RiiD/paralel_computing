#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "points.h"
#include "utils.h"

#define ROW_BUF_SIZE 255
#define POINT_AXIS_SEPARATOR " "

/**
* Deserializes point.
*
* @param char* serializedPoint
* @param Point* dest
*/
void deserializePoint(char* serializedPoint, Point* dest) {
	// I assume that ID's of points are series of natural numbers [0-n)
	strtok(serializedPoint, POINT_AXIS_SEPARATOR);
	dest->x = atof(strtok(NULL, POINT_AXIS_SEPARATOR));
	dest->y = atof(strtok(NULL, POINT_AXIS_SEPARATOR));
}

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

	if (points == NULL) {
		printf("Failed to allocate memory for points!%s", NEWLINE);
		return NULL;
	}

	for (i = 0; i < (*n); i++) {
		str = fgets(buf, ROW_BUF_SIZE, fh);
		deserializePoint(str, &points[i]);
	}

	fclose(fh);

	return points;
}

/**
 * Serializes point.
 *
 * @param Point* point
 * @param int id
 * @param char* dest
 */
void serializePoint(Point* point, int id, char* dest) {
	sprintf(dest, "%d %f %f", id, point->x, point->y);
}

/**
 * Calculates distance between 2 points.
 * For better performance this function doesn't calculates real distance. 
 * Instead it returns distance^2.
 *
 * @param Point* point1
 * @param Point* point2
 * @returns double - Distance
 */
double distance(Point* point1, Point* point2) {
	return pow(point1->x - point2->x, 2) + pow(point1->y - point2->y, 2);
}

/**
* Generates point.
* @param Point* dest
* @param double maxX
* @param double maxY
*/
void generatePoint(Point* dest, double maxX, double maxY) {
	dest->x = (double)rand() / ((double)RAND_MAX / maxX);
	dest->y = (double)rand() / ((double)RAND_MAX / maxY);
}

/**
 * Generates points. Dest should be n * sizeof(Point) bytes.
 *
 * @param int n - number of points to generate
 * @param Point[] dest
 * @param double maxX
 * @param double maxY
 */
void generatePoints(int n, Point dest[], double maxX, double maxY) {
	int i;
	srand((int)time(NULL));

	for (i = 0; i < n; i++) {
		generatePoint(&(dest[i]), maxX, maxY);
	}
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
		serializePoint(&points[i], i, buf);
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
	printf("(%.2f, %.2f)", dPoint->x, dPoint->y);
}
