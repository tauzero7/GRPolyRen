/**
 * File:   schwarzschild.h
 * Author: Thomas Mueller, HdA/MPIA
 * 
 */
#ifndef SCHWARZSCHILD_H
#define SCHWARZSCHILD_H

bool schwarzschild_breakCondition(double* y);

void schwarzschild_derivs(double, double* y, double* dydx);

bool schwarzschild_found(double* y, double* yprev, double* yend, double& t);

bool schwarzschild_initialize(double r, double ksi, double* y);

double schwarzschild_ksiCrit(double r);

#endif // SCHWARZSCHILD_H
