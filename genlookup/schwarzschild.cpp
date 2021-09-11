/**
 * File:   schwarzschild.cpp
 * Author: Thomas Mueller, HdA/MPIA
 * 
 */
#include "schwarzschild.h"

#include <iostream>
#include <cmath>

static const double rs = 2.0;
constexpr double PI = 3.141592653589793;

bool schwarzschild_breakCondition(double* y)
{
    double eps = 1e-2;
    double rmax = 1000.0;

    if (fabs(y[1]) < rs + eps || fabs(y[1]) > rmax) {
        return true;
    }
    return false;
}

void schwarzschild_derivs(double, double* y, double* dydx)
{
    double r = y[1];
    double ut = y[3];
    double ur = y[4];
    double up = y[5];

    dydx[0] = y[3];
    dydx[1] = y[4];
    dydx[2] = y[5];
    dydx[3] = -rs / (r * (r - rs)) * ut * ur;
    dydx[4] = -0.5 * rs * (r - rs) / pow(r, 3.0) * ut * ut + 0.5 * rs / (r * (r - rs)) * ur * ur + (r - rs) * up * up;
    dydx[5] = -2.0 / r * ur * up;
}

bool schwarzschild_found(double* y, double* yprev, double* yend, double &t) 
{
    if (y[2] > yend[2]) {
        t = (yend[2] - yprev[2]) / (y[2] - yprev[2]);
        return true;
    }
    return false;
}

bool schwarzschild_initialize(double r, double ksi, double* y)
{
    double w = sqrt(1.0 - rs / r);
    double dt = -1.0 / w;
    double dr = cos(ksi) * w;
    double dp = sin(ksi) / r;
    y[0] = 0.0;
    y[1] = r;
    y[2] = 0.0;
    y[3] = dt;
    y[4] = dr;
    y[5] = dp;
    return schwarzschild_breakCondition(y);
}

double schwarzschild_ksiCrit(double r) 
{
    double sk2 = 6.75 * rs * rs / (r * r) * (1.0 - rs / r);
    double ksi = asin(sqrt(sk2));

    return ksi;
}