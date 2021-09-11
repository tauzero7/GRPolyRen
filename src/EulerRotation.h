/**
 * File:    EulerRotation.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_EULER_ROTATION_H
#define GRPR_EULER_ROTATION_H

#include <iostream>

#include <glm/glm.hpp>

class EulerRotation
{
public:
    enum class Angle : int { alpha = 0, beta, gamma };

    enum class Order : int {
        Z_XS_ZSS = 0, //!< Standard x-convention
        Z_YS_ZSS, //!< Standard y-convention
        Z_YS_XSS //!< yaw-pitch-roll
    };

    static const char* const OrderNames[];

public:
    EulerRotation();
    explicit EulerRotation(Order order);
    ~EulerRotation();

    void Apply(float* mat);
    void ApplyToPosition(float& x, float& y, float& z);
    void ApplyToVector(float& vx, float& vy, float& vz);

    void Get(float& alpha, float& beta, float& gamma);
    void Get(float* abg);

    /// Get pointer to 4x4 rotation matrix
    float* GetMatrixPtr();

    /// Get pointer to inverse 4x4 rotation matrix
    float* GetInvMatrixPtr();

    void Print(FILE* fptr = stderr);

    void Reset(Angle angle);
    void ResetAll();

    /**
     * @brief Rotate around alpha-axis
     * @param dalpha   Rotation angle in degree
     */
    void RotAlpha(float dalpha);
    void RotBeta(float dbeta);
    void RotGamma(float dgamma);

    void Set(float* abg);

    /// Set angles in degree
    void Set(float alpha, float beta, float gamma);

    void SetAlpha(float alpha);
    void SetBeta(float beta);
    void SetGamma(float gamma);

    void SetOrderByName(const char* order);
    void SetOrder(Order order);
    Order GetOrder();
    const char* GetOrderName();

    static void GetMatrix(Order order, float alpha, float beta, float gamma, float* mat);

protected:
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

    /// Calculate Euler rotation matrix depending on the angles alpha, beta, gamma, and their order.
    void calc();

private:
    Order m_order;
    float m_alpha; // in degree
    float m_beta;
    float m_gamma;

    glm::mat4 m_mat;
    glm::mat4 m_invMat;
};

#endif // GRPR_EULER_ROTATION_H
