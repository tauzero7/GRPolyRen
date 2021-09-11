/**
 * File:   nrRungeKutta.cpp
 * Author: Thomas Mueller, HdA/MPIA
 * 
 */
#include "nrRungeKutta.h"

constexpr unsigned int Nvar = 6;

void rkck(double* y, double* dydx, double x, double h, double* yout, double* yerr,
    void (*derivs)(double, double*, double*))
{
    register int i;
    double ak2[Nvar];
    double ak3[Nvar];
    double ak4[Nvar];
    double ak5[Nvar];
    double ak6[Nvar];
    double ytemp[Nvar];

    for (i = 0; i < Nvar; i++) {
        ytemp[i] = y[i] + h * b21 * dydx[i];
    }

    derivs(x + a2 * h, ytemp, ak2);
    for (i = 0; i < Nvar; i++) {
        ytemp[i] = y[i] + h * (b31 * dydx[i] + b32 * ak2[i]);
    }

    derivs(x + a3 * h, ytemp, ak3);
    for (i = 0; i < Nvar; i++) {
        ytemp[i] = y[i] + h * (b41 * dydx[i] + b42 * ak2[i] + b43 * ak3[i]);
    }

    derivs(x + a4 * h, ytemp, ak4);
    for (i = 0; i < Nvar; i++) {
        ytemp[i] = y[i] + h * (b51 * dydx[i] + b52 * ak2[i] + b53 * ak3[i] + b54 * ak4[i]);
    }

    derivs(x + a5 * h, ytemp, ak5);
    for (i = 0; i < Nvar; i++) {
        ytemp[i] = y[i] + h * (b61 * dydx[i] + b62 * ak2[i] + b63 * ak3[i] + b64 * ak4[i] + b65 * ak5[i]);
    }

    derivs(x + a6 * h, ytemp, ak6);
    for (i = 0; i < Nvar; i++) {
        yout[i] = y[i] + h * (c1 * dydx[i] + c3 * ak3[i] + c4 * ak4[i] + c6 * ak6[i]);
        yerr[i] = h * (dc1 * dydx[i] + dc3 * ak3[i] + dc4 * ak4[i] + dc5 * ak5[i] + dc6 * ak6[i]);
    }
}

void rkqs(double* y, double* dydx, double* x, double htry, double eps, double* yscal, double& hdid,
    double& hnext, void (*derivs)(double, double*, double*))
{
    double errmax, h, htemp, xnew;

    double yerr[Nvar];
    double ytemp[Nvar];

    h = htry;
    for (;;) {
        rkck(y, dydx, *x, h, ytemp, yerr, derivs);

        errmax = 0.0;
        for (unsigned int i = 0; i < Nvar; i++) {
            errmax = MAX(errmax, fabs(yerr[i] / yscal[i]));
        }
        errmax /= eps;

        if (errmax <= 1.0) { // Step succeeded. Compute size of next step.
            break;
        }

        htemp = SAFETY * h * pow(errmax, PSHRNK);
        h = (h >= 0.0 ? MAX(htemp, 0.1 * h) : MIN(htemp, 0.1 * h));

        xnew = (*x) + h;
        if (xnew == *x) {
            fprintf(stderr, "stepsize underflow in rkqs");
        }
    }

    if (errmax > ERRCON) {
        hnext = SAFETY * h * pow(errmax, PGROW);
    }
    else {
        hnext = 5.0 * h;
    }

    *x += (hdid = h);
    for (unsigned int i = 0; i < Nvar; i++) {
        y[i] = ytemp[i];
    }
}

void odeint(double* ystart, int maxSteps, double eps, double h1, double hmin,
    void (*derivs)(double, double*, double*),
    bool (*breakCond)(double*))
{
    double hnext, hdid;

    double yscal[Nvar];
    double y[Nvar];
    double dydx[Nvar];

    double h = h1;
    double x = 0.0;

    for (unsigned int i = 0; i < Nvar; i++) {
        y[i] = ystart[i];
    }

    for (unsigned int nstp = 1; nstp <= maxSteps; nstp++) {
        fprintf(stdout, "%12.6f  ", x);
        for (unsigned int i = 0; i < Nvar; i++) {
            fprintf(stdout, "%12.6f ", y[i]);
        }
        fprintf(stdout, "\n");

        derivs(x, y, dydx);
        for (unsigned i = 0; i < Nvar; i++) {
            yscal[i] = fabs(y[i]) + fabs(dydx[i] * h) + TINY;
        }
        rkqs(y, dydx, &x, h, eps, yscal, hdid, hnext, derivs);

        if (breakCond != nullptr) {
            if (breakCond(y)) {
                break;
            }
        }

        if (fabs(hnext) <= hmin) {
            fprintf(stderr, "Step size too small in odeint");
        }
        h = hnext;
    }
}

/**
 * 
 */
bool integrate(double* ystart, double* ymax, int maxSteps, double eps, double h1, double hmin,
    void (*derivs)(double, double*, double*),    
    bool (*breakCond)(double*),
    bool (*found)(double*, double*, double*, double&))
{
    double hnext, hdid;

    double yscal[Nvar];
    double y[Nvar];
    double dydx[Nvar];
    double yprev[Nvar];

    double h = h1;
    double x = 0.0;
    double xprev = 0.0;
    double t;
    
    for (unsigned int i = 0; i < Nvar; i++) {
        y[i] = ystart[i];
    }

    bool valid = false;
    for (unsigned int nstp = 1; nstp <= maxSteps; nstp++) {
        derivs(x, y, dydx);
        for (unsigned i = 0; i < Nvar; i++) {
            yprev[i] = y[i];
            xprev = x;
            yscal[i] = fabs(y[i]) + fabs(dydx[i] * h) + TINY;

            //fprintf(stderr, "%12.8f ", y[i]);
        }
        //fprintf(stderr, "\n");
        
        rkqs(y, dydx, &x, h, eps, yscal, hdid, hnext, derivs);
    
        if (breakCond(y)) {
            break;
        }

        if (found(y, yprev, ymax, t)) {
            interpolate(y, yprev, t, ystart);
            valid = true;
            break;
        }
    
        h = hnext;
    }

    return valid;
}

/**
 * 
 */
void interpolate(double* y, double* yprev, double t, double* yres)
{
    for(unsigned int i = 0; i < Nvar; i++) {
        yres[i] = yprev[i] * (1.0 - t) +  t * y[i];
    }
}