/**
 * File:    EulerRotation.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "EulerRotation.h"
#include "Utilities.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char* const EulerRotation::OrderNames[]
    = { "z_xs_zss", "z_ys_zss", "z_ys_xss" };

EulerRotation::EulerRotation()
    : m_order(Order::Z_XS_ZSS)
    , m_alpha(0.0f)
    , m_beta(0.0f)
    , m_gamma(0.0f)
{
    calc();
}

EulerRotation::EulerRotation(Order order)
    : m_order(order)
    , m_alpha(0.0f)
    , m_beta(0.0f)
    , m_gamma(0.0f)
{
    calc();
}

EulerRotation::~EulerRotation()
{
    // blank
}

void EulerRotation::Apply(float* mat)
{
    glm::mat4 matrix = glm::mat4(mat[0], mat[1], mat[2], mat[3], mat[4], mat[5], mat[6], mat[7], mat[8], mat[9],
        mat[10], mat[11], mat[12], mat[13], mat[14], mat[15]);

    matrix = matrix * m_mat;
    float* mptr = glm::value_ptr(matrix);
    for (unsigned int i = 0; i < 16; i++) {
        mat[i] = *(mptr++);
    }
}

void EulerRotation::ApplyToPosition(float& x, float& y, float& z)
{
    glm::vec4 v = glm::vec4(x, y, z, 1.0f);
    v = m_invMat * v;
    x = v.x;
    y = v.y;
    z = v.z;
}

void EulerRotation::ApplyToVector(float& vx, float& vy, float& vz)
{
    glm::vec4 v = glm::vec4(vx, vy, vz, 0.0f);
    v = m_invMat * v;
    vx = v.x;
    vy = v.y;
    vz = v.z;
}

void EulerRotation::Get(float& alpha, float& beta, float& gamma)
{
    alpha = m_alpha;
    beta = m_beta;
    gamma = m_gamma;
}

void EulerRotation::Get(float* abg)
{
    if (abg == nullptr) {
        return;
    }

    abg[0] = m_alpha;
    abg[1] = m_beta;
    abg[2] = m_gamma;
}

float* EulerRotation::GetMatrixPtr()
{
    return glm::value_ptr(m_mat);
}

float* EulerRotation::GetInvMatrixPtr()
{
    return glm::value_ptr(m_invMat);
}

void EulerRotation::Print(FILE* fptr)
{
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            fprintf(fptr, "%8.4f ", m_mat[row][col]);
        }
        fprintf(fptr, "\n");
    }
}

void EulerRotation::Reset(Angle angle)
{
    switch (angle) {
        case Angle::alpha: {
            m_alpha = 0.0f;
            break;
        }
        case Angle::beta: {
            m_beta = 0.0f;
            break;
        }
        case Angle::gamma: {
            m_gamma = 0.0f;
            break;
        }
    }
    calc();
}

void EulerRotation::ResetAll()
{
    m_alpha = m_beta = m_gamma = 0.0f;
    calc();
}

void EulerRotation::RotAlpha(float dalpha)
{
    m_alpha += dalpha;
    calc();
}

void EulerRotation::RotBeta(float dbeta)
{
    m_beta += dbeta;
    calc();
}

void EulerRotation::RotGamma(float dgamma)
{
    m_gamma += dgamma;
    calc();
}

void EulerRotation::Set(float* angles)
{
    if (angles == nullptr) {
        return;
    }
    Set(angles[0], angles[1], angles[2]);
}

void EulerRotation::Set(float alpha, float beta, float gamma)
{
    m_alpha = alpha;
    m_beta = beta;
    m_gamma = gamma;
    calc();
}

void EulerRotation::SetAlpha(float alpha)
{
    Set(alpha, m_beta, m_gamma);
}

void EulerRotation::SetBeta(float beta)
{
    Set(m_alpha, beta, m_gamma);
}

void EulerRotation::SetGamma(float gamma)
{
    Set(m_alpha, m_beta, gamma);
}

void EulerRotation::SetOrderByName(const char* order)
{
    if (strcmp(order, "z_xs_zss") == 0) {
        SetOrder(Order::Z_XS_ZSS);
    }
    else if (strcmp(order, "z_ys_zss") == 0) {
        SetOrder(Order::Z_YS_ZSS);
    }
    else if (strcmp(order, "z_ys_xss") == 0) {
        SetOrder(Order::Z_YS_XSS);
    }
}

void EulerRotation::SetOrder(Order order)
{
    m_order = order;
    calc();
}

EulerRotation::Order EulerRotation::GetOrder()
{
    return m_order;
}

const char* EulerRotation::GetOrderName()
{
    std::uint8_t idx = static_cast<int>(m_order);
    if (idx < 3) {
        return OrderNames[idx];
    }
    return OrderNames[0];
}

void EulerRotation::calc()
{
    glm::vec3 axis1 = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 axis2 = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 axis3 = glm::vec3(0.0f, 0.0f, 1.0f);

    switch (m_order) {
        case Order::Z_XS_ZSS: {
            axis1 = glm::vec3(0.0f, 0.0f, 1.0f);
            axis2 = glm::vec3(1.0f, 0.0f, 0.0f);
            axis3 = glm::vec3(0.0f, 0.0f, 1.0f);
            break;
        }
        case Order::Z_YS_ZSS: {
            axis1 = glm::vec3(0.0f, 0.0f, 1.0f);
            axis2 = glm::vec3(0.0f, 1.0f, 0.0f);
            axis3 = glm::vec3(0.0f, 0.0f, 1.0f);
            break;
        }
        case Order::Z_YS_XSS: {
            axis1 = glm::vec3(0.0f, 0.0f, 1.0f);
            axis2 = glm::vec3(0.0f, 1.0f, 0.0f);
            axis3 = glm::vec3(1.0f, 0.0f, 0.0f);
            break;
        }
    }

    m_mat = glm::mat4(1.0f);
    m_mat = glm::rotate(m_mat, glm::radians(m_gamma), axis3);
    m_mat = glm::rotate(m_mat, glm::radians(m_beta), axis2);
    m_mat = glm::rotate(m_mat, glm::radians(m_alpha), axis1);

    m_invMat = glm::mat4(1.0f);
    m_invMat = glm::rotate(m_invMat, -glm::radians(m_alpha), axis1);
    m_invMat = glm::rotate(m_invMat, -glm::radians(m_beta), axis2);
    m_invMat = glm::rotate(m_invMat, -glm::radians(m_gamma), axis3);
}

void EulerRotation::GetMatrix(Order order, float alpha, float beta, float gamma, float* mat)
{
    if (mat == nullptr) {
        return;
    }

    EulerRotation erot(order);
    erot.Set(alpha, beta, gamma);
    memcpy(mat, erot.GetMatrixPtr(), sizeof(float) * 16);
}
