/**
 * File:   nrRungeKutta.h
 * Author: Thomas Mueller, HdA/MPIA
 * 
 *  Runge-Kutta integration following "Numerical Recipes in C".
 *  Chapter 16.2 "Adaptive Stepsize Control for Runge-Kutta".
 */
#ifndef NR_RUNGE_KUTTA_H
#define NR_RUNGE_KUTTA_H

#include <cmath>
#include <cstdio>
#include <iostream>

#define DEG_TO_RAD M_PI / 180.0

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define SAFETY 0.9
#define PGROW -0.2
#define PSHRNK -0.25
#define ERRCON 1.89e-4
#define TINY 1.0e-30

const double a2 = 0.2, a3 = 0.3, a4 = 0.6, a5 = 1.0, a6 = 0.875, b21 = 0.2, b31 = 3.0 / 40.0, b32 = 9.0 / 40.0,
             b41 = 0.3, b42 = -0.9, b43 = 1.2, b51 = -11.0 / 54.0, b52 = 2.5, b53 = -70.0 / 27.0, b54 = 35.0 / 27.0,
             b61 = 1631.0 / 55296.0, b62 = 175.0 / 512.0, b63 = 575.0 / 13824.0, b64 = 44275.0 / 110592.0,
             b65 = 253.0 / 4096.0, c1 = 37.0 / 378.0, c3 = 250.0 / 621.0, c4 = 125.0 / 594.0, c6 = 512.0 / 1771.0,
             dc5 = -277.0 / 14336.0;

const double dc1 = c1 - 2825.0 / 27648.0, dc3 = c3 - 18575.0 / 48384.0, dc4 = c4 - 13525.0 / 55296.0, dc6 = c6 - 0.25;

/**
 *  Runge-Kutta Cash-Karp step
 */
void rkck(double* y, double* dydx, double x, double h, double* yout, double* yerr,
    void (*derivs)(double, double*, double*));

/**
 *  stepper function
 */
void rkqs(double* y, double* dydx, double* x, double htry, double eps, double* yscal, double& hdid,
    double& hnext, void (*derivs)(double, double*, double*));

/**
 *
 */
void odeint(double* ystart, int maxSteps, double eps, double h1, double hmin,
    void (*derivs)(double, double*, double*), bool (*breakCond)(double*) = nullptr);

/**
 *
 */
bool integrate(double* ystart, double* ymax, int maxSteps, double eps, double h1, double hmin,
    void (*derivs)(double, double*, double*), bool (*breakCond)(double*),
    bool (*found)(double*, double*, double*, double&));

void interpolate(double* y, double* yprev, double t, double* yres);

#endif // NR_RUNGE_KUTTA_H
