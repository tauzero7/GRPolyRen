/**
 * File:    TransScale.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "TransScale.h"
#include "Utilities.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

TransScale::TransScale()
{
    m_order = Order::TransScale;

    m_trans = glm::vec3(0.0f);
    m_scale = glm::vec3(1.0f);
    calc();
}

TransScale::~TransScale()
{
    // nothing
}

void TransScale::GetScale(float& x, float& y, float& z)
{
    x = m_scale.x;
    y = m_scale.y;
    z = m_scale.z;
}

void TransScale::GetScale(float* xyz)
{
    xyz[0] = m_scale.x;
    xyz[1] = m_scale.y;
    xyz[2] = m_scale.z;
}

void TransScale::GetTrans(float& x, float& y, float& z)
{
    x = m_trans.x;
    y = m_trans.y;
    z = m_trans.z;
}

void TransScale::GetTrans(float* xyz)
{
    xyz[0] = m_trans.x;
    xyz[1] = m_trans.y;
    xyz[2] = m_trans.z;
}

void TransScale::Move(float dx, float dy, float dz)
{
    m_trans += glm::vec3(dx, dy, dz);
    calc();
}

void TransScale::SetScale(float s)
{
    SetScale(s, s, s);
}

void TransScale::SetScale(float x, float y, float z)
{
    m_scale = glm::vec3(x, y, z);
    calc();
}

void TransScale::SetScale(float* xyz)
{
    if (xyz != nullptr) {
        SetScale(xyz[0], xyz[1], xyz[2]);
    }
}

void TransScale::SetTrans(float x, float y, float z)
{
    m_trans = glm::vec3(x, y, z);
    calc();
}

void TransScale::SetTrans(float* xyz)
{
    if (xyz != nullptr) {
        SetTrans(xyz[0], xyz[1], xyz[2]);
    }
}

void TransScale::ResetScale()
{
    SetScale(1.0);
}

void TransScale::ResetTrans()
{
    SetTrans(0.0f, 0.0f, 0.0f);
}

void TransScale::Rotate(float dphi)
{
    float r = glm::length(m_trans);
    float phi = atan2f(m_trans.y, m_trans.x);
    float theta = acosf(m_trans.z / r);

    phi += dphi;

    float x = r * sin(theta) * cos(phi);
    float y = r * sin(theta) * sin(phi);
    float z = r * cos(theta);
    SetTrans(x, y, z);
}

float* TransScale::GetMatrixPtr()
{
    return glm::value_ptr(m_mat);
}

float* TransScale::GetTransMatrixPtr()
{
    return glm::value_ptr(m_matTrans);
}

float* TransScale::GetScaleMatrixPtr()
{
    return glm::value_ptr(m_matScale);
}

void TransScale::SetOrder(Order order)
{
    m_order = order;
    calc();
}

TransScale::Order TransScale::GetOrder()
{
    return m_order;
}

void TransScale::calc()
{
    m_mat = glm::mat4(1.0f);
    m_matTrans = glm::mat4(1.0f);
    m_matScale = glm::mat4(1.0f);

    m_matTrans = glm::translate(m_matTrans, m_trans);
    m_matScale = glm::scale(m_matScale, m_scale);

    if (m_order == Order::TransScale) {
        m_mat = m_matTrans * m_matScale;
    }
    else {
        m_mat = m_matScale * m_matTrans;
    }
}
