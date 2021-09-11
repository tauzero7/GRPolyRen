/**
 * File:    CrossHairs3D.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "CrossHairs3D.h"

#include <iostream>

CrossHairs3D::CrossHairs3D()
    : m_lineLength(1.0f)
    , m_lineWidth(2.0f)
{
    //
}

CrossHairs3D::~CrossHairs3D()
{
    //
}

void CrossHairs3D::Draw(const float* projMXptr, const float* viewMXptr, const float* modelMXptr)
{
    if (!m_visible) {
        return;
    }

    glLineWidth(m_lineWidth);

    m_shader.Bind();
    m_shader.SetFloatMatrix("projMX", 4, 1, GL_FALSE, projMXptr);
    m_shader.SetFloatMatrix("viewMX", 4, 1, GL_FALSE, viewMXptr);
    m_shader.SetFloatMatrix("modelMX", 4, 1, GL_FALSE, glm::value_ptr(modelMX));

    m_va.Bind();
    glDrawArrays(GL_LINES, 0, 6);
    m_va.Release();
    m_shader.Release();

    glLineWidth(1.0f);
}

bool CrossHairs3D::Init()
{
    //fprintf(stdout, "Initialize CrossHairs3D ...\n");

    std::string vShaderName = "shader/geomCrossHairs3D.vert";
    std::string fShaderName = "shader/geomCrossHairs3D.frag";
    m_shader.SetFileNames(vShaderName.c_str(), fShaderName.c_str());

    float verts[4 * 6] = { -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f };
    float colors[4 * 6] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f };

    m_va.Create(6);
    m_va.SetArrayBuffer(0, GL_FLOAT, 4, verts);
    m_va.SetArrayBuffer(1, GL_FLOAT, 4, colors);
    setModelMatrix();

    return ReloadShaders();
}

bool CrossHairs3D::ReloadShaders()
{
    return m_shader.ReloadShaders();
}

void CrossHairs3D::SetColor(CoordAxis axis, float r, float g, float b)
{
    float color[8] = { r, g, b, 1.0f, r, g, b, 1.0f };

    size_t offset = 0;
    if (axis == CoordAxis::Y) {
        offset = 2;
    }
    else if (axis == CoordAxis::Z) {
        offset = 4;
    }

    m_va.SetSubArrayBuffer(1, offset, 2, color);
}

void CrossHairs3D::SetColor(CoordAxis axis, float* rgb)
{
    if (rgb == nullptr) {
        return;
    }
    SetColor(axis, rgb[0], rgb[1], rgb[2]);
}

void CrossHairs3D::SetLineLength(float len)
{
    m_lineLength = len;
    setModelMatrix();
}

void CrossHairs3D::SetLineWidth(float lw)
{
    m_lineWidth = lw;
}

void CrossHairs3D::setModelMatrix()
{
    modelMX = glm::mat4(1.0f);
    modelMX = glm::translate(modelMX, m_center);
    modelMX = glm::scale(modelMX, glm::vec3(m_lineLength));
}
