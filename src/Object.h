/**
 * File:    Object.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_OBJECT_H
#define GRPR_OBJECT_H

#include "GLShader.h"
#include "VertexArray.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Object
{
public:
    Object();
    virtual ~Object();

    virtual void Draw(const float* projMXptr, const float* viewMXptr, const float* modelMXptr = nullptr) = 0;

    virtual bool Init() = 0;

    virtual bool ReloadShaders();

    void GetColor(float &r, float &g, float &b, float &a);
    void GetColor(float* rgba);

    bool IsVisible();

    void SetColor(float r, float g, float b, float a = 1.0f);
    void SetColor(float x, float a = 1.0f);
    void SetColor(float* rgba);

    void SetFlatShading(bool flatShading);

    void Show(bool show = true);
    void ToggleVisibility();

protected:
    virtual void setModelMatrix() = 0;

protected:
    GLShader m_shader;
    VertexArray m_va;

    glm::vec3 m_center;
    glm::vec3 m_size;
    glm::mat4 modelMX;

    glm::vec4 m_color;
    bool m_isFlatShading;
    bool m_visible;
};


#endif // GRPR_OBJECT
