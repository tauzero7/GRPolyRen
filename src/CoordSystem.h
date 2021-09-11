/**
 * File:    CoordSystem.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_COORD_SYSTEM_H
#define GRPR_COORD_SYSTEM_H

#include "Object.h"

class CoordSystem : public Object
{
public:
    CoordSystem();
    virtual ~CoordSystem();

    virtual void Draw(const float* projMXptr, const float* viewMXptr, const float* modelMXptr = nullptr);

    virtual bool Init();

    virtual bool ReloadShaders();

    void SetAxisColor(unsigned int axis, float r, float g, float b);

    void SetAxisLength(unsigned int axis, float length);

    void SetViewport(int x, int y, int width, int height);

protected:
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

    typedef struct Axis_t {
        glm::vec3 color;
        glm::mat4 rotMX;
        float length;
    } Axis;

    void createArrow();

protected:
    virtual void setModelMatrix();    

protected:
    int m_viewport_x;
    int m_viewport_y;
    int m_viewport_width;
    int m_viewport_height;

    float m_fovy; // vertical field-of-view in degrees
    float m_cylRadius;
    float m_coneRadius;
    float m_coneHeight;
    int m_numStreaks;

    int m_numVertsCyl;
    int m_numVertsCone;
    int m_numVertsDisk;

    glm::mat4 projMX;
    Axis m_axis[3];
    float m_scaleFactor;

    float m_dist;
};

#endif // GRPR_COORD_SYSTEM_H
