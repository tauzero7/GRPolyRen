/**
 * File:    LightSource.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "LightSource.h"
#include <cmath>

constexpr float PI_F = 3.14159265f;

LightSource::LightSource()
    : m_isActive(true)
    , m_theta(90.0f)
    , m_phi(0.0f)
    , m_factor(0.0f)
{
    calcPosition();
}

LightSource::~LightSource()
{
    //
}

void LightSource::Get(float& theta, float& phi)
{
    theta = m_theta;
    phi = m_phi;
}

float LightSource::GetFactor()
{
    return m_factor;
}

float* LightSource::GetPositionPtr()
{
    return m_position;
}

bool LightSource::IsActive()
{
    return m_isActive;
}

void LightSource::Set(float theta, float phi)
{
    m_theta = theta;
    m_phi = phi;
    calcPosition();
}

void LightSource::SetActive(bool active)
{
    m_isActive = active;
}

void LightSource::SetFactor(float factor)
{
    m_factor = factor;
}

void LightSource::SetUniformName(const char* uname)
{
    m_uniformName = std::string(uname);
}

void LightSource::UpdateGL(GLShader* shader)
{
    if (shader == nullptr) {
        return;
    }

    std::string uact = m_uniformName + ".is_active";
    shader->SetFloat(uact.c_str(), (m_isActive ? 1.0 : 0.0));

    std::string upos = m_uniformName + ".position";
    shader->SetFloat(upos.c_str(), m_position[0], m_position[1], m_position[2]);

    std::string ufac = m_uniformName + ".factor";
    shader->SetFloat(ufac.c_str(), m_factor);
}

void LightSource::calcPosition()
{
    float theta = m_theta / 180.0f * PI_F;
    float phi = m_phi / 180.0f * PI_F;
    m_position[0] = sinf(theta) * cosf(phi);
    m_position[1] = sinf(theta) * sinf(phi);
    m_position[2] = cosf(theta);
}