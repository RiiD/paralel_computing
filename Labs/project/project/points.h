#pragma once

/**
 * A struct to store point values.
 */
struct Point {
	double x;
	double y;
};

Point* loadPoints(const char* fileName, int *n, int *k);
double distance(const Point* point1, const Point* point2);
void generatePoints(int n, Point dest[], double maxX, double maxY);
void savePoints(const char* fileName, const Point points[], int n, int k);
