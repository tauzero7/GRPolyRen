#pragma once

constexpr double PI = 3.141592653589793;

double toRadian(double deg);

double toDegree(double rad);

double mix(double a, double b, double t);

void calcBase(const double* p, double* e1, double* e2);

void calcBaseComp(const double* e1, const double* e2, const double* p, double &r, double &phi);

// in camera coordinates
void calcCoords(const double* e1, const double* e2, double ksi, double dist, double* q);

void calcPerspectiveProjection(const double* p, double &cx, double &cy);
