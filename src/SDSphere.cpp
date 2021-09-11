/**
 * File:    SDSphere.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "SDSphere.h"

// basic vertices of an icosahedron
#define ICSH_X .5257311f
#define ICSH_Z .8506508f

const float icsh_data[12][3]
    = { { -ICSH_X, 0.0f, ICSH_Z }, { ICSH_X, 0.0f, ICSH_Z }, { -ICSH_X, 0.0f, -ICSH_Z }, { ICSH_X, 0.0f, -ICSH_Z },
          { 0.0f, ICSH_Z, ICSH_X }, { 0.0f, ICSH_Z, -ICSH_X }, { 0.0f, -ICSH_Z, ICSH_X }, { 0.0f, -ICSH_Z, -ICSH_X },
          { ICSH_Z, ICSH_X, 0.0f }, { -ICSH_Z, ICSH_X, 0.0f }, { ICSH_Z, -ICSH_X, 0.0f }, { -ICSH_Z, -ICSH_X, 0.0f } };

const GLuint icsh_indices[20][3] = { { 0, 4, 1 }, { 0, 9, 4 }, { 9, 5, 4 }, { 4, 5, 8 }, { 4, 8, 1 }, { 8, 10, 1 },
    { 8, 3, 10 }, { 5, 3, 8 }, { 5, 2, 3 }, { 2, 7, 3 }, { 7, 10, 3 }, { 7, 6, 10 }, { 7, 11, 6 }, { 11, 0, 6 },
    { 0, 1, 6 }, { 6, 1, 10 }, { 9, 0, 11 }, { 9, 11, 2 }, { 9, 2, 5 }, { 7, 2, 11 } };

SDSphere::SDSphere()
{
    m_center = glm::vec3(0.0f);
    SetRadius(1.0f);
}

SDSphere::~SDSphere() {}

void SDSphere::Draw(const float* projMXptr, const float* viewMXptr, const float* modelMXptr)
{
    if (!m_visible) {
        return;
    }
    std::ignore = modelMXptr;

    glm::mat4 viewMX = glm::mat4(1.0f);
    if (viewMXptr != nullptr) {
        viewMX = glm::make_mat4(viewMXptr);
    }

    glm::mat4 invMV = glm::inverse(viewMX);

    m_shader.Bind();
    m_shader.SetFloatMatrix("invMVMX", 4, 1, GL_FALSE, glm::value_ptr(invMV));
    m_shader.SetFloatMatrix("projMX", 4, 1, GL_FALSE, projMXptr);
    m_shader.SetFloatMatrix("viewMX", 4, 1, GL_FALSE, glm::value_ptr(viewMX));
    m_shader.SetFloatMatrix("modelMX", 4, 1, GL_FALSE, glm::value_ptr(modelMX));

    m_shader.SetFloatArray("m_color", 4, 1, glm::value_ptr(m_color));
    m_shader.SetFloat("flatShading", (m_isFlatShading ? 1.0f : 0.0f));
    m_va.Bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_numFaces * 3), GL_UNSIGNED_INT, nullptr);
    m_va.Release();

    m_shader.Release();
}

bool SDSphere::Init()
{
    //fprintf(stderr, "Initialize SDSphere ...\n");

    SetSubdivisions(1);

    setModelMatrix();

    std::string vShaderName = "shader/geomSDSphere.vert";
    std::string fShaderName = "shader/geomSDSphere.frag";
    m_shader.SetFileNames(vShaderName.c_str(), fShaderName.c_str());
    return ReloadShaders();
}

bool SDSphere::ReloadShaders()
{
    return m_shader.ReloadShaders();
}

float SDSphere::GetRadius()
{
    return m_radius;
}

void SDSphere::SetRadius(float radius)
{
    m_radius = radius;
    m_size = glm::vec3(radius, radius, radius);
    setModelMatrix();
}

void SDSphere::SetSubdivisions(unsigned int numSubDivs)
{
    clear();
    m_numVertices = 12;

    // copy vertices from static array to vector array and generate texture coordinates
    for (unsigned int i = 0; i < m_numVertices; i++) {
        glm::vec3 nv = glm::vec3(icsh_data[i][0], icsh_data[i][1], icsh_data[i][2]);
        glm::vec2 tc;
        calcTexCoords(nv, tc);
        svertices.push_back(nv);
        snormals.push_back(glm::normalize(glm::vec3(icsh_data[i][0], icsh_data[i][1], icsh_data[i][2])));
        stcoords.push_back(tc);
    }
    assert(svertices.size() == m_numVertices);

    // copy indices from static array to vector array
    m_numFaces = 20;
    for (unsigned int i = 0; i < m_numFaces; i++) {
        int ni0 = static_cast<int>(icsh_indices[i][0]);
        int ni1 = static_cast<int>(icsh_indices[i][1]);
        int ni2 = static_cast<int>(icsh_indices[i][2]);
        sindices.push_back(ni0);
        sindices.push_back(ni1);
        sindices.push_back(ni2);
    }
    assert(sindices.size() == m_numFaces * 3);

    // do subdivision
    unsigned int nf = m_numFaces;
    if (numSubDivs > 0) {
        for (unsigned int f = 0; f < nf; f++) {
            glm::vec3 v1 = glm::vec3(svertices[static_cast<size_t>(sindices[3 * f + 0])]);
            glm::vec3 v2 = glm::vec3(svertices[static_cast<size_t>(sindices[3 * f + 1])]);
            glm::vec3 v3 = glm::vec3(svertices[static_cast<size_t>(sindices[3 * f + 2])]);
            // fprintf(stderr,"face %2d : %2d %2d %2d\n",f,sindices[3*f+0],sindices[3*f+1],sindices[3*f+2]);
            subdivideTriangle(v1, v2, v3, numSubDivs);
        }

        // remove the original faces
        std::vector<int>::iterator istart = sindices.begin();
        std::vector<int>::iterator iend = istart + nf * 3;
        sindices.erase(istart, iend);
        m_numFaces -= nf;
    }

    for (unsigned int f = 0; f < m_numFaces; f++) {
        size_t idx1 = static_cast<size_t>(sindices[3 * f + 0]);
        size_t idx2 = static_cast<size_t>(sindices[3 * f + 1]);
        size_t idx3 = static_cast<size_t>(sindices[3 * f + 2]);
        glm::vec3 v1 = svertices[idx1];
        glm::vec3 v2 = svertices[idx2];
        glm::vec3 v3 = svertices[idx3];
        glm::vec2 tc1 = stcoords[idx1];
        glm::vec2 tc2 = stcoords[idx2];
        glm::vec2 tc3 = stcoords[idx3];

        if (v1[0] < 0.0f && v2[0] < 0.0f && v3[0] < 0.0f) {
            //  if (v1[1]<0.0) {

            if (!((v1[1] < 0 && v2[1] < 0 && v3[1] < 0) || (v1[1] > 0 && v2[1] > 0 && v3[1] > 0))) {
                if (v1[1] < 0)
                    tc1 = glm::vec2(tc1[0] + 1.0f, tc1[1]);
                if (v2[1] < 0)
                    tc2 = glm::vec2(tc2[0] + 1.0f, tc2[1]);
                if (v3[1] < 0)
                    tc3 = glm::vec2(tc3[0] + 1.0f, tc3[1]);

                stcoords[idx1] = tc1;
                stcoords[idx2] = tc2;
                stcoords[idx3] = tc3;
            }
        }
    }

    m_va.Delete();
    m_va.Create(m_numVertices);
    m_va.SetArrayBuffer(0, GL_FLOAT, 3, &svertices[0]);
    m_va.SetArrayBuffer(1, GL_FLOAT, 3, &snormals[0]);
    m_va.SetArrayBuffer(3, GL_FLOAT, 2, &stcoords[0]);
    m_va.SetElementBuffer(0, m_numFaces * 3, &sindices[0]);
}

void SDSphere::addTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
    size_t idx = svertices.size();

    svertices.push_back(v1);
    svertices.push_back(v2);
    svertices.push_back(v3);
    snormals.push_back(v1);
    snormals.push_back(v2);
    snormals.push_back(v3);

    glm::vec2 tc1, tc2, tc3;
    calcTexCoords(v1, tc1);
    calcTexCoords(v2, tc2);
    calcTexCoords(v3, tc3);
    stcoords.push_back(tc1);
    stcoords.push_back(tc2);
    stcoords.push_back(tc3);

    sindices.push_back(static_cast<int>(idx) + 0);
    sindices.push_back(static_cast<int>(idx) + 1);
    sindices.push_back(static_cast<int>(idx) + 2);

    m_numFaces++;
    m_numVertices += 3;
}

void SDSphere::calcTexCoords(glm::vec3 v, glm::vec2& tc)
{
    double th, ph;
    th = acos(v[2]);
    ph = atan2(v[1], v[0]);
    tc[0] = static_cast<float>(ph / (2.0 * glm::pi<double>()) + 0.5);
    tc[1] = static_cast<float>(th / glm::pi<double>());
}

void SDSphere::clear()
{
    svertices.clear();
    snormals.clear();
    stcoords.clear();
    sindices.clear();
}

void SDSphere::subdivideTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, unsigned int depth)
{
    glm::vec3 v12;
    glm::vec3 v23;
    glm::vec3 v31;

    if (depth == 0) {
        addTriangle(v1, v2, v3);
        return;
    }

    for (int i = 0; i < 3; i++) {
        v12[i] = v1[i] + v2[i];
        v23[i] = v2[i] + v3[i];
        v31[i] = v3[i] + v1[i];
    }

    v12 = glm::normalize(v12);
    v23 = glm::normalize(v23);
    v31 = glm::normalize(v31);
    subdivideTriangle(glm::normalize(v1), v12, v31, depth - 1);
    subdivideTriangle(glm::normalize(v2), v23, v12, depth - 1);
    subdivideTriangle(glm::normalize(v3), v31, v23, depth - 1);
    subdivideTriangle(v12, v23, v31, depth - 1);
}

void SDSphere::setModelMatrix()
{
    modelMX = glm::mat4(1.0f);
    modelMX = glm::translate(modelMX, m_center);
    modelMX = glm::scale(modelMX, m_size);
}
