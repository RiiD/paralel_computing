#pragma once

/**
 * A struct to store point values.
 */
struct Point {
	int id;
	double x;
	double y;
};

struct Distance {
	Point* p1;
	Point* p2;
	double distance;
};

Point* loadPoints(char* fileName, int *n, int *k);
void distance(Point* point1, Point* point2, Distance* distance);
void generatePoints(int n, Point dest[], double maxX, double maxY);
void savePoints(char* fileName, Point points[], int n, int k);
int quick_select_index(int* input, int p, int r, int k);
void printDistances(Distance distances[], int size, int colsPerRow);
void printDistancesIndices(Distance distances[], int size, int colsPerRow);
void sortDistances(Distance distances[], int size);
