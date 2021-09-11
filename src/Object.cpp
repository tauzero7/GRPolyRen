/**
 * File:    Object.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "Object.h"

#include <iostream>

Object::Object()
    : m_visible(true)
{
    m_center = glm::vec3(0.0f);
    m_size = glm::vec3(1.0f);
    m_color = glm::vec4(0.5f,0.5f,0.5f,1.0f);
}

Object::~Object()
{
    //
}

bool Object::ReloadShaders()
{
    return m_shader.ReloadShaders();
}

void Object::GetColor(float &r, float &g, float &b, float &a)
{
    r = m_color.r;
    g = m_color.g;
    b = m_color.b;
    a = m_color.a;
}

void Object::GetColor(float* rgba)
{
    rgba[0] = m_color.r;
    rgba[1] = m_color.g;
    rgba[2] = m_color.b;
    rgba[3] = m_color.a;
}

bool Object::IsVisible()
{
    return m_visible;
}

void Object::SetColor(float r, float g, float b, float a)
{
    m_color = glm::vec4(r, g, b, a);
}

void Object::SetColor(float x, float a)
{
    SetColor(x, x, x, a);
}

void Object::SetColor(float* rgba)
{
    SetColor(rgba[0], rgba[1], rgba[2], rgba[3]);
}

void Object::SetFlatShading(bool flatShading)
{
    m_isFlatShading = flatShading;
}

void Object::Show(bool show)
{
    m_visible = show;
}

void Object::ToggleVisibility()
{
    m_visible = !m_visible;
}
