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
void generatePoints(const int n, Point dest[], const double maxX, const double maxY);
void savePoints(const char* fileName, const Point points[], const int n, const int k);
