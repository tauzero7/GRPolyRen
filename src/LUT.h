/**
 * File:    LUT.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_LUT_H
#define GRPR_LUT_H

#include "glad/glad.h"

#include <iostream>

class LUT {
public:
    LUT();
    ~LUT();

    float GetCameraPos();
    
    void GetRadialRange(float &rmin, float &rmax);
    
    void GetScaledRange(float rs, float &xmin, float &xscale);

    GLuint GetTexID(unsigned int idx);

    bool Load(const char* filename);

protected:
    size_t getFileSizeInBytes(const char* filename);
    GLuint genRGBAFloatTexture(unsigned int width, unsigned int height, float* data);

protected:
    unsigned int m_Nr;
    unsigned int m_Nphi;
    float m_rmin;
    float m_rmax;
    float m_camPos;
    float* m_data;
    GLuint m_texID[2];    
};

#endif // GRPR_LUT_H
