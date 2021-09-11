/**
 * File:    Quaternion.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_QUATERNIONS_H
#define GRPR_QUATERNIONS_H

#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>

#define SIGNUM_SIGN(a) (a >= 0.0 ? '+' : '-')

#define DEF_QEPS 1.0e-15

/**
 * @brief Quaternion
 */
template <class vType> class Quaternion
{
public:
    //! Create a new zero Quaternion.
    Quaternion();

    //! Create a new Quaternion.
    Quaternion(vType xr, vType xi, vType xj, vType xk);
    ~Quaternion();

public:
    void CalcMat();

    void GetVector(unsigned int idx, vType& x, vType& y, vType& z);

    void GetCamDir(vType& x, vType& y, vType& z);
    void GetCamRight(vType& x, vType& y, vType& z);
    void GetCamUp(vType& x, vType& y, vType& z);

    /**
     * @brief Set all parameters of the Quaternion.
     * @param xr
     * @param xi
     * @param xj
     * @param xk
     */
    void Set(vType xr, vType xi, vType xj, vType xk);

    /** Set real component. */
    void SetReal(vType xr);

    /** Set i-component. */
    void SetI(vType xi);

    /** Set j-component. */
    void SetJ(vType xj);

    /** Set k-component. */
    void SetK(vType xk);

    /** Set quaternion by camera's right-, up-, and dir-vectors. */
    void Set(vType* right, vType* up, vType* dir);

    /**
     * @brief Get quaternion components.
     * @param xr
     * @param xi
     * @param xj
     * @param xk
     */
    void Get(vType& xr, vType& xi, vType& xj, vType& xk);

    void GetVector(vType& xi, vType& xj, vType& xk);

    /**
     * @brief Set rotation quaternion.
     *    The resulting Quaternion is given by  "cos(angle/2) + sin(angle/2)*(e1*i + e2*j + e3*k)".
     *    If the axis is zero, the quaternion will not be set.
     *
     * @param angle  Angle in radians
     * @param e1   x-component for rotation axis.
     * @param e2   y-component for rotation axis.
     * @param e3   z-component for rotation axis.
     */
    void SetRot(vType angle, vType e1, vType e2, vType e3);

    //! Get real component.
    vType r() const;

    //! Get i-component.
    vType i() const;

    //! Get j-component.
    vType j() const;

    //! Get k-component.
    vType k() const;

    /**
     * @brief Generate conjugate.
     * @return "conj = xr - xi * i - xj * j - xk * k"
     */
    Quaternion Conj() const;

    /**
     * @brief Generate inverse.
     * @return "inv = conj(x) / abs(x)^2"
     */
    Quaternion Inverse() const;

    /**
     * @brief Calculate length of quaternion.
     * @return sqrt(xr*xr + xi*xi + xj*xj + xk*xk);
     */
    vType Length() const;

    void Normalize();

    //! Assignment operator.
    void operator=(const Quaternion& q);

    //! Addition.
    Quaternion operator+(const Quaternion& q) const;

    //! Subtraction.
    Quaternion operator-(const Quaternion& q) const;

    /**
     * @brief Multiplication between two quaternions x and y
     *   z.r = x.r*y.r - x.i*y.i - x.j*y.j - x.k-y.k
     *   z.i = x.r*y.i + y.r*x.i + x.j*y.k - x.k-y.j
     *   z.j = x.r*y.j + y.r*x.j + x.k*y.i - x.i-y.k
     *   z.k = x.r*y.k + y.r*x.k + x.i*y.j - x.j-y.i
     */
    Quaternion operator*(const Quaternion& q) const;

    /**
     * @brief Scalar multiplication.
     * @param a
     * @return
     */
    Quaternion operator*(const vType a) const;

    /**
     * @brief Operator|
     *    This operator is an abbreviation for
     *       x|rho  with  rho:  x -> rho * x * conj(rho).
     * @param rho
     * @return rho * x * conj(rho).
     */
    Quaternion operator|(const Quaternion& rho) const;

    //! Logical operator "equal".
    int operator==(const Quaternion& q) const;

    //! Logical operator "not-equal".
    int operator!=(const Quaternion& q) const;

    /**
     * @brief Print quaternion as string, e.g. "3.1 + 2.5i + 1.2j + 4.6j".
     * @param ptr
     */
    void Print(FILE* ptr = stderr);

    /**
     * @brief Print quaternion in vector notation, e.g. (3.1,2.5,1.2,4.6).
     * @param ptr
     */
    void PrintQ(FILE* ptr = stderr);

    void PrintMatrix(FILE* ptr = stderr);

    // void SetCamVectors(vType rx, vType ry, vType rz, vType dx, vType dy, vType dz, vType ux, vType uy, vType uz);

    vType* ToMat();

    /**
     * @brief Convert quaternion to euler angle.
     * @param phi
     * @param theta
     * @param psi
     */
    void ToEuler(vType& phi, vType& theta, vType& psi);

    // --------- friend methods -----------
    friend Quaternion operator*(vType a, const Quaternion& q)
    {
        Quaternion n;
        n.Set(a * q.r(), a * q.i(), a * q.j(), a * q.k());
        return n;
    }

    static Quaternion cross(const Quaternion& a, const Quaternion& b)
    {
        Quaternion n;
        n.Set(0, a.j() * b.k() - a.k() * b.j(), a.k() * b.i() - a.i() * b.k(), a.i() * b.j() - a.j() * b.k());
        return n;
    }

    static Quaternion Rotate(const Quaternion& a, vType angle, vType e1, vType e2, vType e3)
    {
        Quaternion<vType> r;
        r.SetRot(angle, e1, e2, e3);
        Quaternion<vType> n;
        n = r * a * r.Conj();
        return n;
    }

    /**
     * @brief Skaral product <a,b>: H x H -> R
     * @param a
     * @param b
     * @return
     */
    static vType dot(const Quaternion& a, const Quaternion& b)
    {
        return a.r() * b.r() + a.i() * b.i() + a.j() * b.j() + a.k() * b.k();
    }

protected:
    vType xr;
    vType xi;
    vType xj;
    vType xk;
    vType m[4][4];
};

template <class vType> Quaternion<vType>::Quaternion()
{
    Set(0, 0, 0, 0);
}

template <class vType> Quaternion<vType>::Quaternion(vType xr, vType xi, vType xj, vType xk)
{
    Set(xr, xi, xj, xk);
}

template <class vType> Quaternion<vType>::~Quaternion() {}

template <class vType> void Quaternion<vType>::CalcMat()
{
    ToMat();
}

template <class vType> void Quaternion<vType>::GetVector(unsigned int idx, vType& x, vType& y, vType& z)
{
    if (idx < 3) {
        x = m[0][idx];
        y = m[1][idx];
        z = m[2][idx];
    }
}

template <class vType> void Quaternion<vType>::GetCamDir(vType& x, vType& y, vType& z)
{
    this->GetVector(2, x, y, z);
}

template <class vType> void Quaternion<vType>::GetCamRight(vType& x, vType& y, vType& z)
{
    this->GetVector(0, x, y, z);
}

template <class vType> void Quaternion<vType>::GetCamUp(vType& x, vType& y, vType& z)
{
    this->GetVector(1, x, y, z);
}

template <class vType> void Quaternion<vType>::Set(vType xr, vType xi, vType xj, vType xk)
{
    this->xr = xr;
    this->xi = xi;
    this->xj = xj;
    this->xk = xk;
}

template <class vType> void Quaternion<vType>::SetReal(vType xr)
{
    this->xr = xr;
}

template <class vType> void Quaternion<vType>::SetI(vType xi)
{
    this->xi = xi;
}

template <class vType> void Quaternion<vType>::SetJ(vType xj)
{
    this->xj = xj;
}

template <class vType> void Quaternion<vType>::SetK(vType xk)
{
    this->xk = xk;
}

// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
template <class vType> void Quaternion<vType>::Set(vType* right, vType* up, vType* dir)
{
    vType vm[3][3];
    vm[0][0] = right[0];
    vm[1][0] = right[1];
    vm[2][0] = right[2];
    vm[0][1] = up[0];
    vm[1][1] = up[1];
    vm[2][1] = up[2];
    vm[0][2] = -dir[0];
    vm[1][2] = -dir[1];
    vm[2][2] = -dir[2];

    vType qw, qx, qy, qz;
    vType tr = vm[0][0] + vm[1][1] + vm[2][2];

    vType one_fourth = static_cast<vType>(0.25);

    if (tr > 0) {
        vType S = sqrt(tr + 1.0) * 2; // S=4*qw
        qw = one_fourth * S;
        qx = (vm[2][1] - vm[1][2]) / S;
        qy = (vm[0][2] - vm[2][0]) / S;
        qz = (vm[1][0] - vm[0][1]) / S;
    }
    else if ((vm[0][0] > vm[1][1]) & (vm[0][0] > vm[2][2])) {
        vType S = sqrt(1.0 + vm[0][0] - vm[1][1] - vm[2][2]) * 2; // S=4*qx
        qw = (vm[2][1] - vm[1][2]) / S;
        qx = one_fourth * S;
        qy = (vm[0][1] + vm[1][0]) / S;
        qz = (vm[0][2] + vm[2][0]) / S;
    }
    else if (vm[1][1] > vm[2][2]) {
        vType S = sqrt(1.0 + vm[1][1] - vm[0][0] - vm[2][2]) * 2; // S=4*qy
        qw = (vm[0][2] - vm[2][0]) / S;
        qx = (vm[0][1] + vm[1][0]) / S;
        qy = one_fourth * S;
        qz = (vm[1][2] + vm[2][1]) / S;
    }
    else {
        vType S = sqrt(1.0 + vm[2][2] - vm[0][0] - vm[1][1]) * 2; // S=4*qz
        qw = (vm[1][0] - vm[0][1]) / S;
        qx = (vm[0][2] + vm[2][0]) / S;
        qy = (vm[1][2] + vm[2][1]) / S;
        qz = one_fourth * S;
    }
    Set(qw, qx, qy, qz);
}

template <class vType> void Quaternion<vType>::Get(vType& xr, vType& xi, vType& xj, vType& xk)
{
    xr = this->xr;
    xi = this->xi;
    xj = this->xj;
    xk = this->xk;
}

template <class vType> void Quaternion<vType>::GetVector(vType& xi, vType& xj, vType& xk)
{
    xi = this->xi;
    xj = this->xj;
    xk = this->xk;
}

template <class vType> void Quaternion<vType>::SetRot(vType angle, vType e1, vType e2, vType e3)
{
    double norm = sqrt(e1 * e1 + e2 * e2 + e3 * e3);
    if (norm <= 0.0) {
        return;
    }

    norm = 1.0 / norm;

    double sa = sin(0.5 * angle);
    xr = static_cast<vType>(cos(0.5 * angle));
    xi = static_cast<vType>(sa * e1 * norm);
    xj = static_cast<vType>(sa * e2 * norm);
    xk = static_cast<vType>(sa * e3 * norm);
}

template <class vType> vType Quaternion<vType>::r() const
{
    return xr;
}

template <class vType> vType Quaternion<vType>::i() const
{
    return xi;
}

template <class vType> vType Quaternion<vType>::j() const
{
    return xj;
}

template <class vType> vType Quaternion<vType>::k() const
{
    return xk;
}

template <class vType> Quaternion<vType> Quaternion<vType>::Conj() const
{
    return Quaternion<vType>(xr, -xi, -xj, -xk);
}

template <class vType> Quaternion<vType> Quaternion<vType>::Inverse() const
{
    Quaternion<vType> n = Conj();
    vType val = Length();

    if (fabs(val) < DEF_QEPS) {
        return Quaternion<vType>();
    }

    val = static_cast<vType>(1) / val;
    return n * val;
}

template <class vType> vType Quaternion<vType>::Length() const
{
    return static_cast<vType>(sqrt(xr * xr + xi * xi + xj * xj + xk * xk));
}

template <class vType> void Quaternion<vType>::Normalize()
{
    vType n = Length();
    *this = static_cast<vType>(1) / n * (*this);
}

template <class vType> void Quaternion<vType>::operator=(const Quaternion<vType>& q)
{
    xr = q.r();
    xi = q.i();
    xj = q.j();
    xk = q.k();
}

template <class vType> Quaternion<vType> Quaternion<vType>::operator+(const Quaternion<vType>& q) const
{
    Quaternion<vType> n;
    n.Set(xr + q.r(), xi + q.i(), xj + q.j(), xk + q.k());
    return n;
}

template <class vType> Quaternion<vType> Quaternion<vType>::operator-(const Quaternion<vType>& q) const
{
    Quaternion<vType> n;
    n.Set(xr - q.r(), xi - q.i(), xj - q.j(), xk - q.k());
    return n;
}

template <class vType> Quaternion<vType> Quaternion<vType>::operator*(const Quaternion<vType>& q) const
{
    Quaternion<vType> n;
    n.SetReal(xr * q.r() - xi * q.i() - xj * q.j() - xk * q.k());
    n.SetI(xr * q.i() + xi * q.r() + xj * q.k() - xk * q.j());
    n.SetJ(xr * q.j() - xi * q.k() + xj * q.r() + xk * q.i());
    n.SetK(xr * q.k() + xi * q.j() - xj * q.i() + xk * q.r());

    // n.SetI(xr*q.i() + q.r()*xi + xj*q.k() - xk*q.j());
    // n.SetJ(xr*q.j() + q.r()*xj + xk*q.i() - xi*q.k());
    // n.SetK(xr*q.k() + q.r()*xk + xi*q.j() - xj*q.i());
    return n;
}

template <class vType> Quaternion<vType> Quaternion<vType>::operator*(const vType a) const
{
    Quaternion<vType> n;
    n.Set(xr * a, xi * a, xj * a, xk * a);
    return n;
}

template <class vType> Quaternion<vType> Quaternion<vType>::operator|(const Quaternion<vType>& rho) const
{
    Quaternion<vType> n;
    n = rho * (*this) * rho.Conj();
    return n;
}

template <class vType> int Quaternion<vType>::operator==(const Quaternion<vType>& q) const
{
    return (fabs(xr - q.r()) <= DEF_QEPS && fabs(xi - q.i()) <= DEF_QEPS && fabs(xj - q.j()) <= DEF_QEPS
        && fabs(xk - q.k()) <= DEF_QEPS);
}

template <class vType> int Quaternion<vType>::operator!=(const Quaternion<vType>& q) const
{
    return !(*this == q);
}

template <class vType> void Quaternion<vType>::Print(FILE* ptr)
{
    char si = SIGNUM_SIGN(xi);
    char sj = SIGNUM_SIGN(xj);
    char sk = SIGNUM_SIGN(xk);

    fprintf(ptr, "%8.5f %c%8.5fi %c%8.5fj %c%8.5fk\n", xr, si, fabs(xi), sj, fabs(xj), sk, fabs(xk));
}

template <class vType> void Quaternion<vType>::PrintQ(FILE* ptr)
{
    fprintf(ptr, "(%f,%f,%f,%f)\n", xr, xi, xj, xk);
}

template <class vType> void Quaternion<vType>::PrintMatrix(FILE* fptr)
{
    ToMat();
    fprintf(fptr, "((%8.3f %8.3f %8.3f %8.3f)\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    fprintf(fptr, " (%8.3f %8.3f %8.3f %8.3f)\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    fprintf(fptr, " (%8.3f %8.3f %8.3f %8.3f)\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    fprintf(fptr, " (%8.3f %8.3f %8.3f %8.3f))\n", m[3][0], m[3][1], m[3][2], m[3][3]);
    fprintf(fptr, "\n");
}

template <class vType> vType* Quaternion<vType>::ToMat()
{
    vType len = Length();
    double s = 1.0 / (len * len);

    m[0][0] = static_cast<vType>(1.0 - 2.0 * s * (xj * xj + xk * xk));
    m[0][1] = static_cast<vType>(2.0 * s * (xi * xj - xk * xr));
    m[0][2] = static_cast<vType>(2.0 * s * (xi * xk + xj * xr));
    m[0][3] = static_cast<vType>(0.0);

    m[1][0] = static_cast<vType>(2.0 * s * (xi * xj + xk * xr));
    m[1][1] = static_cast<vType>(1.0 - 2.0 * s * (xi * xi + xk * xk));
    m[1][2] = static_cast<vType>(2.0 * s * (xj * xk - xi * xr));
    m[1][3] = static_cast<vType>(0.0);

    m[2][0] = static_cast<vType>(2.0 * s * (xi * xk - xj * xr));
    m[2][1] = static_cast<vType>(2.0 * s * (xj * xk + xi * xr));
    m[2][2] = static_cast<vType>(1.0 - 2.0 * s * (xi * xi + xj * xj));
    m[2][3] = static_cast<vType>(0.0);

    m[3][0] = static_cast<vType>(0.0);
    m[3][1] = static_cast<vType>(0.0);
    m[3][2] = static_cast<vType>(0.0);
    m[3][3] = static_cast<vType>(1.0);

    return &m[0][0];
}

template <class vType> void Quaternion<vType>::ToEuler(vType& phi, vType& theta, vType& psi)
{
    phi = static_cast<vType>(atan2(2.0 * (xr * xi + xj * xk), 1.0 - 2.0 * (xi * xi + xj * xj)));
    theta = static_cast<vType>(asin(2.0 * (xr * xj - xk * xi)));
    psi = static_cast<vType>(atan2(2.0 * (xr * xk + xi * xj), 1.0 - 2.0 * (xj * xj + xk * xk)));
}

#endif // GRPR_QUATERNIONS_H
