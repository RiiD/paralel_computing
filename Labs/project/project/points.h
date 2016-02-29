#pragma once

/**
 * A struct to store point values.
 */
struct Point {
	double x;
	double y;
};

Point* loadPoints(char* fileName, int *n, int *k);
double distance(Point* point1, Point* point2);
void generatePoints(int n, Point dest[], double maxX, double maxY);
void savePoints(char* fileName, Point points[], int n, int k);
