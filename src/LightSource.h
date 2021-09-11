/**
 * File:    LightSource.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_LIGHTSOURCE_H
#define GRPR_LIGHTSOURCE_H

#include <iostream>
#include "GLShader.h"

class LightSource {
public:    
    LightSource();
    ~LightSource();

    void Get(float& theta, float& phi);

    float GetFactor();

    float* GetPositionPtr();

    bool IsActive();

    /**
     * @brief Set position 
     * @param theta   colatitude [degree]
     * @param phi     azimuth [degree]
     */
    void Set(float theta, float phi);

    void SetActive(bool active);

    void SetFactor(float factor);

    void SetUniformName(const char* uname);

    void UpdateGL(GLShader* shader);

protected:
    void calcPosition();

protected:
    bool m_isActive;
    float m_theta;
    float m_phi;
    float m_factor;

    float m_position[3];

    std::string m_uniformName;
};

#endif // GRPR_LIGHTSOURCE_H
