/**
 * File:    TransScale.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_TRANS_SCALE_H
#define GRPR_TRANS_SCALE_H

#include <iostream>

#include <glm/glm.hpp>

class TransScale
{
public:
    enum class Order : int { TransScale = 0, ScaleTrans };

public:
    TransScale();
    ~TransScale();

    void GetScale(float& x, float& y, float& z);
    void GetScale(float* xyz);

    void GetTrans(float& x, float& y, float& z);
    void GetTrans(float* xyz);

    void Move(float dx, float dy, float dz);

    void SetScale(float s);
    void SetScale(float x, float y, float z);
    void SetScale(float* xyz);

    void SetTrans(float x, float y, float z);
    void SetTrans(float* xyz);

    void ResetScale();
    void ResetTrans();

    void Rotate(float dphi);

    float* GetMatrixPtr();
    float* GetTransMatrixPtr();
    float* GetScaleMatrixPtr();

    void SetOrder(Order order);
    Order GetOrder();

protected:
    void calc();

private:
    Order m_order;
    glm::vec3 m_trans;
    glm::vec3 m_scale;

    glm::mat4 m_mat;
    glm::mat4 m_matTrans;
    glm::mat4 m_matScale;
};

#endif // GRPR_TRANS_SCALE_H
