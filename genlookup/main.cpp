/**
 * File:   main.cpp
 * Author: Thomas Mueller, HdA/MPIA
 * 
 *  Generate lookup table for general-relativistic polygon rendering
 * 
 *  The output file has a 20 byte header of the form
 *      Nr   (unsigned int):  number of radial samples
 *      Nphi (unsigned int):  number of azimuth angle samples
 *      rmin (float):         minimum radius value
 *      rmax (float):         maximum radius value
 *      dist (float):         observer distance
 *      data (float array):   (ksi1,dt1,u1x,u1y)
 *      data (float array):   (ksi2,dt2,u2x.u2y)
 * 
 *  Run:
 *     1.) Adapt parameters in 'main' and compile.
 *     2.) Run ./GenLookupTable
 *     3.) Do not forget to adapt the "lut_filename" in src/main.cpp
 *         and recompile GRPolyRen.
 */
#include <iostream>
#include <chrono>
#include <random>

#define VERBOSE_OUTPUT

#ifdef HAVE_OPENMP_AVAIL
#include <omp.h>
#endif // HAVE_OPENMP_AVAIL

#include "nrRungeKutta.h"
#include "schwarzschild.h"
#include "helper.h"

// number of coordinates (3-pos, 3-vel)
constexpr unsigned int Ncoords = 6;

constexpr unsigned int MaxRandomTries = 3000;
constexpr unsigned int MaxTries = 200;
constexpr double HitRadiusSphere = 1e-5;
constexpr double ksi_eps = 1e-9;

constexpr double rs = 2.0;

constexpr unsigned int MaxNumSteps = 10000;
constexpr double eps_abs = 1e-10;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0, 1);



/**
 * Calculate geodesic up to final point
 * @param rInit
 * @param ksi
 * @param rFinal
 * @param phiFinal
 * @param dr
 * @param dt
 */
bool calcGeodesicUpTo(double rInit, double ksi, double rFinal, double phiFinal, double& dr, double& dt, double* u)
{
    dr = 1e12;
    dt = 1e12;

    double y[Ncoords];
    double ymax[] = { 1e10, rFinal, phiFinal, 0.0, 0.0, 0.0 };
    schwarzschild_initialize(rInit, ksi, y);

    bool isValid = integrate(y, ymax, MaxNumSteps, eps_abs, 0.01, 1e-8, schwarzschild_derivs,
        schwarzschild_breakCondition, schwarzschild_found);

    if (isValid) {
        dr = y[1] - rFinal;
        dt = y[0];

        double r = y[1];
        double phi = y[2];
        double ur = y[4];
        double up = y[5];
        u[0] = ur * cos(phi) - up * r * sin(phi);
        u[1] = ur * sin(phi) + up * r * cos(phi);
    }

    return isValid;
}

/**
 *  Calculate initial angle for flat spacetime
 */
double calcFlatKsi(double rInit, double rFinal, double phiFinal) {
    double d2 = rInit * rInit + rFinal * rFinal - 2.0 * rInit * rFinal * cos(phiFinal);
    double d = sqrt(d2);
    return asin(rInit / d * sin(phiFinal));
}

/**
 * Find geodesic between initial position and final point 
 * @param order
 * @param rInit      Initial radial position (point p)
 * @param rFinal     Final radial position (point q)
 * @param phiFinal   Final azimuth angle (point q)
 * @param ksi        Initial direction
 * @param dt         Light travel time
 * @param derr
 * @param cnt
 * @param u          Direction of light at final point (2-array)
 */
bool findGeodesic(int order, double rInit, double rFinal, double phiFinal, 
    double& ksi, double& dt, double& derr, unsigned int &cnt, double* u)
{
    double ksiCrit = schwarzschild_ksiCrit(rInit);

    double ksiA = (order == 0 ? 0.0 : PI/2);
    double ksiB = (order == 0 ? PI : PI - ksiCrit);
    double ksiC;

    double uA[2], uB[2], uC[2];
    double drA, drB, drC;
    double dtA, dtB, dtC;
    bool validA = calcGeodesicUpTo(rInit, ksiA, rFinal, phiFinal, drA, dtA, uA);
    bool validB = calcGeodesicUpTo(rInit, ksiB, rFinal, phiFinal, drB, dtB, uB);

    if (!validA){
        drA = 1e10;
    }

    if (!validB) {
        drB = -1e10;
    }

    double eps_radius = HitRadiusSphere;
    unsigned int count = 0;
    unsigned int randomCount = 0;

    while ((fabs(drA - drB) > eps_radius) && fabs(ksiA - ksiB) > ksi_eps && count < MaxTries) {
        ksiC = (ksiA + ksiB) * 0.5;

        if (count == 0) {            
            ksiC = calcFlatKsi(rInit, rFinal, phiFinal);
        }

        bool validC = false;
        randomCount = 0;
        while (!validC && randomCount < MaxRandomTries) {
            validC = calcGeodesicUpTo(rInit, ksiC, rFinal, phiFinal, drC, dtC, uC);
            if (!validC) {
                double t = dis(gen);
                ksiC = ksiA * (1.0 - t) + t * ksiB;
                randomCount++;
            }
        }

#if 0
    fprintf(stderr, "%04d %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8e  %12.8e %4d\n", count, toDegree(ksiA), toDegree(ksiB), toDegree(ksiC), 
        drA, drB, drC, dtA, dtB, dtC, fabs(ksiA - ksiB), drC * drA, randomCount);
#endif

        if (fabs(drC) < 1e-15 && validC) {
            break;
        }

        if (drC * drA < 0.0) {
            ksiB = ksiC;
            drB = drC;
            dtB = dtC;
        }
        else {
            ksiA = ksiC;
            drA = drC;
            dtA = dtC;
        }

        count++;
    }

    ksi = ksiC;
    dt = dtC;
    derr = drC;
    cnt = count;
    u[0] = uC[0];
    u[1] = uC[1];
    return count < MaxTries;
}

/**
 * Generate lookup table
 * @param rInit
 * @param rmin
 * @param ramax
 * @param Nr
 * @param Nphi
 */
void genLUT(double rInit, double rmin, double rmax, unsigned int Nr, unsigned int Nphi,
    const char* filename)
{
    fprintf(stderr, "Gen LUT for rInit = %f, range=[%f,%f], Nr=%u, Nphi=%u\n", rInit, rmin, rmax, Nr, Nphi);
    double xmin = rs / rmax;
    double xmax = rs / rmin;
    double xStep = (xmax - xmin) / (Nr - 1);

    double eps = 1e-4;
    double phimin = eps;
    double phimax = PI - eps;
    double phiStep = (phimax - phimin) / (Nphi - 1);
    double ksi, dt, derr;
    double u[2];
    bool isValid;
    unsigned int cnt;

    float* lut_0 = new float[Nr * Nphi * 4];
    float* lut_1 = new float[Nr * Nphi * 4];

    auto t1 = std::chrono::system_clock::now();

#ifndef HAVE_OPENMP_AVAIL
    // NOT CORRECT - OLD LUTs...
    for (unsigned int ir = 0; ir < Nr; ir++) {
        double x = xmin + ir * xStep;
        double r = rs / x;
        for (unsigned int ip = 0; ip < Nphi; ip++) {
            unsigned int num = ir * Nphi + ip;

            double phi1 = phimin + ip * phiStep;
            isValid = findGeodesic(0, rInit, r, phi1, ksi, dt, derr, cnt, u);
#ifdef VERBOSE_OUTPUT
            fprintf(stderr, "1: %4d %4d  %14.8f %14.8f %14.8f %14.8f %14.8f %2d %4d\n", ir, ip, r, phi1, toDegree(ksi), dt,
                    derr, (isValid ? 1 : -1), cnt);
#endif            
            lut_0[4 * num + 0] = static_cast<float>(ksi);
            lut_0[4 * num + 1] = (isValid ? static_cast<float>(fabs(dt)) : -1.0f);
            lut_0[4 * num + 2] = static_cast<float>(u[0]);
            lut_0[4 * num + 3] = static_cast<float>(u[1]);

            double phi2 = 2.0 * PI - phi1;
            isValid = findGeodesic(1, rInit, r, phi2, ksi, dt, derr, cnt, u);
#ifdef VERBOSE_OUTPUT
            fprintf(stderr, "1: %4d %4d  %14.8f %14.8f %14.8f %14.8f %14.8f %2d %4d\n", ir, ip, r, phi2, toDegree(ksi), dt,
                    derr, (isValid ? 1 : -1), cnt);
#endif                    
            lut_1[4 * num + 0] = static_cast<float>(ksi);
            lut_1[4 * num + 1] = (isValid ? static_cast<float>(fabs(dt)) : -1.0f);
            lut_1[4 * num + 2] = static_cast<float>(u[0]);
            lut_1[4 * num + 3] = static_cast<float>(u[1]);
        }
    }

#else // HAVE_OPENMP_AVAIL

    int ir, ip, n;
    int NperThread = static_cast<int>(Nr * Nphi) / omp_get_max_threads();
    int threadID = 0;

    // 'private' clause is super-important here!!!!
    #pragma omp parallel for private(n,ir,ip)
    for(n = 0; n < Nr * Nphi; n++) {
        ir = n / Nphi;
        ip = n % Nphi;

        #pragma omp critical
        {
            threadID = omp_get_thread_num();
            if (threadID == 0) {
                fprintf(stderr, "#: %4d/%4d\r", n, NperThread);
            }
        }

        double x = xmin + ir * xStep;
        double r = rs / x;
        double phi1 = phimin + ip * phiStep;

        isValid = findGeodesic(0, rInit, r, phi1, ksi, dt, derr, cnt, u);
        lut_0[4 * n + 0] = static_cast<float>(ksi);
        lut_0[4 * n + 1] = (isValid ? static_cast<float>(fabs(dt)) : -1.0f);
        lut_0[4 * n + 2] = static_cast<float>(u[0]);
        lut_0[4 * n + 3] = static_cast<float>(u[1]);

        double phi2 = 2.0 * PI - phi1;
        isValid = findGeodesic(1, rInit, r, phi2, ksi, dt, derr, cnt, u);
        lut_1[4 * n + 0] = static_cast<float>(ksi);
        lut_1[4 * n + 1] = (isValid ? static_cast<float>(fabs(dt)) : -1.0f);
        lut_1[4 * n + 2] = static_cast<float>(u[0]);
        lut_1[4 * n + 3] = static_cast<float>(u[1]);
    }
#endif // HAVE_OPENMP_AVAIL    

    auto t2 = std::chrono::system_clock::now();
    double dt_calc = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
    fprintf(stderr, "\ncalc: %f s\n", dt_calc * 1e-3);


    FILE* fptr = fopen(filename, "wb");
    if (fptr == nullptr) {
        fprintf(stderr, "ohoh!\n");
    }
    else {
        float fmin = static_cast<float>(rmin);
        float fmax = static_cast<float>(rmax);
        float fdist = static_cast<float>(rInit);
        fwrite(&Nr, sizeof(unsigned int), 1, fptr);
        fwrite(&Nphi, sizeof(unsigned int), 1, fptr);
        fwrite(&fmin, sizeof(float), 1, fptr);
        fwrite(&fmax, sizeof(float), 1, fptr);
        fwrite(&fdist, sizeof(float), 1, fptr);
        fwrite(lut_0, sizeof(float), Nr * Nphi * 4, fptr);
        fwrite(lut_1, sizeof(float), Nr * Nphi * 4, fptr);
        fclose(fptr);
    }

    delete [] lut_1;
    delete [] lut_0;
}

/**
 * Calculate geodesic (for test only)
 */
void calcGeodesic()
{
    double* y = new double[Ncoords];
    schwarzschild_initialize(10.0, toRadian(152), y);
    odeint(y, 1000, 1e-8, 0.01, 1e-6, schwarzschild_derivs, schwarzschild_breakCondition);
}

void calcLineDistortion(double rInit)
{
    // points in world space
    double p1[] = {-5.0, 3.5,-2.0};
    double p2[] = {-5.0, 3.5, 3.0};

    unsigned int N = 21;
    double lambdaStep = 1.0 / (N - 1);

    double e1[3], e2[3], r, phi, q[3];
    double lambda, ksi, dt, derr, u[2];
    double cx, cy;
    unsigned int cnt;

    for(unsigned int i = 0; i < N; i++) {
        lambda = i * lambdaStep;
        double p[] = {mix(p1[0], p2[0], lambda), mix(p1[1],p2[1], lambda), mix(p1[2],p2[2],lambda)};
        calcBase(p, e1, e2);
        calcBaseComp(e1, e2, p, r, phi);
        findGeodesic(0, rInit, r, phi, ksi, dt, derr, cnt, u);
        calcCoords(e1, e2, ksi, fabs(dt), q);
        calcPerspectiveProjection(q, cx, cy);
        fprintf(stdout, "%f %f  %f %f\n", p[1], p[2], cx, cy);
        fprintf(stderr, "%f %f %f  %f %f %f\n", r, ksi, dt, q[0], q[1], q[2]);
    }
}

/**
 * main
 */
int main(int argc, char* argv[])
{
#ifdef HAVE_OPENMP_AVAIL
    fprintf(stderr, "Generate lookup table with OpenMP support...\n");
#else
    fprintf(stderr, "Generate lookup table without OpenMP support...\n");
#endif

    unsigned int Nr = 32; //256; //256;
    unsigned int Nphi = 64; // 512; //512;
    double rmin = 2.5;
    double rmax = 30.0;
    double rInit = 40.0;

#if 0
    unsigned int cnt;
    double ksi, dt, derr, u[2];    
    double x = std::atof(argv[1]);
    double y = std::atof(argv[2]);
    double rf = sqrt(x*x+y*y);
    double phif = 2.0 * PI- atan2(y, x);
    findGeodesic(1, 40.0, rf, phif, ksi, dt, derr, cnt, u);
    fprintf(stderr, "%12.8f %12.8f %12.8f %4d\n", 180.0-toDegree(ksi), dt, derr, cnt);
#endif

#if 1
    char filename[256];
    sprintf(filename, "lut_r%d_%dx%d.dat", static_cast<int>(rInit), Nr, Nphi);
    genLUT(rInit, rmin, rmax, Nr, Nphi, filename);
#endif

#if 0
    calcLineDistortion(rInit);
#endif    
    return 0;
}
