/**
 * File:    SDSphere.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_SDSPHERE_H
#define GRPR_SDSPHERE_H

#include <vector>

#include "Object.h"

class SDSphere : public Object
{
public:
    SDSphere();
    virtual ~SDSphere();

    virtual void Draw(const float* projMXptr, const float* viewMXptr, const float* modelMXptr = nullptr);

    virtual bool Init();

    virtual bool ReloadShaders();

    float GetRadius();

    void SetRadius(float radius);

    void SetSubdivisions(unsigned int numSubDivs);

protected:
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

    void addTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
    void calcTexCoords(glm::vec3 v, glm::vec2& tc);
    void clear();
    void subdivideTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, unsigned int depth);

    void setModelMatrix();

protected:
    float m_radius;
    
    std::vector<glm::vec3> svertices;
    std::vector<glm::vec3> snormals;
    std::vector<glm::vec2> stcoords;
    std::vector<int> sindices;

    unsigned int m_numVertices;
    unsigned int m_numFaces;
};

#endif // GRPR_SDSPHERE_H
