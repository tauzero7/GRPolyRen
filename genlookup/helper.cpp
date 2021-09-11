
#include "helper.h"
#include <cmath>
#include <iostream>

double toRadian(double deg)
{
    return deg / 180.0 * PI;
}

double toDegree(double rad)
{
    return rad / PI * 180.0;
}

double mix(double a, double b, double t) {
    return a * (1.0 - t) + t * b;
}

void crossProd(const double* a, const double* b, double* c) {
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

double dotProd(const double* a, const double* b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void normalize(double* v) {
    double n = 1.0 / sqrt(v[0]*v[0] + v[1]*v[1] + v[2] * v[2]);
    v[0] *= n;
    v[1] *= n;
    v[2] *= n;
}

void calcBase(const double* p, double* e1, double* e2) {
    e1[0] = 1.0;
    e1[1] = 0.0;
    e1[2] = 0.0;

    double n[3];
    crossProd(e1, p, n);
    crossProd(n, e1, e2);
    normalize(e2);
}

void calcBaseComp(const double* e1, const double* e2, const double* p, double &r, double &phi) {
    double x = dotProd(p, e1);
    double y = dotProd(p, e2);
    r = sqrt(x*x + y*y);
    phi = atan2(y, x);
}

void calcCoords(const double* e1, const double* e2, double ksi, double dist, double* q) {
    //std::cerr << rInit << " " << dist << " " << ksi << "\n";
    double x = dist * cos(ksi);
    double y = dist * sin(ksi);
    q[0] = x * e1[0] + y * e2[0];
    q[1] = x * e1[1] + y * e2[1];
    q[2] = x * e1[2] + y * e2[2];
}

void calcPerspectiveProjection(const double* p, double &cx, double &cy) {
    cx = p[1] / (-p[0]) * 40;
    cy = p[2] / (-p[0]) * 40;
}