/**
 * File:    CrossHairs3D.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_CROSS_HAIRS_3D_H
#define GRPR_CROSS_HAIRS_3D_H

#include "Object.h"

class CrossHairs3D : public Object
{
public:
    enum class CoordAxis : int { X = 0, Y, Z };
public:
    CrossHairs3D();
    virtual ~CrossHairs3D();

    virtual void Draw(const float* projMXptr, const float* viewMXptr, const float* modelMXptr = nullptr);

    virtual bool Init();

    virtual bool ReloadShaders();

    void SetColor(CoordAxis axis, float r, float g, float b);
    void SetColor(CoordAxis axis, float* rgb);

    void SetLineLength(float len);
    void SetLineWidth(float lw);

protected:
    virtual void setModelMatrix();

protected:
    float m_lineLength;
    float m_lineWidth;
};


#endif // GRPR_CROSS_HAIRS_3D_H
