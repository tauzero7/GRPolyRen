/**
 * File:    CoordSystem.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "CoordSystem.h"
#include "Utilities.h"

CoordSystem::CoordSystem()
    : m_fovy(45.0f)
    , m_cylRadius(0.1f)
    , m_coneRadius(0.2f)
    , m_coneHeight(0.3f)
    , m_numStreaks(50)
    , m_numVertsCyl(0)
    , m_numVertsCone(0)
    , m_numVertsDisk(0)
    , m_scaleFactor(1.0f)
    , m_dist(4.0f)
{
    // axis 1 definitions
    m_axis[0].color = glm::vec3(1.0f, 0.0f, 0.0f);
    m_axis[0].rotMX = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    m_axis[0].length = 1.0f + m_coneHeight;

    // axis 2 definitions
    m_axis[1].color = glm::vec3(0.0f, 1.0f, 0.0f);
    m_axis[1].rotMX = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    m_axis[1].length = 1.0f + m_coneHeight;

    // axis 3 definitions
    m_axis[2].color = glm::vec3(0.0f, 0.0f, 1.0f);
    m_axis[2].rotMX = glm::mat4(1.0f);
    m_axis[2].length = 1.0f + m_coneHeight;

    SetViewport(0, 0, 100, 100);
}

CoordSystem::~CoordSystem() {}

void CoordSystem::Draw(const float* projMXptr, const float* viewMXptr, const float* modelMXptr)
{
    // save current viewport, blending and depth testing
    int oldViewport[4];
    glGetIntegerv(GL_VIEWPORT, oldViewport);

    GLboolean isDepthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLboolean isBlendingEnabled = glIsEnabled(GL_BLEND);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set coordinate axes viewport
    glViewport(m_viewport_x, m_viewport_y, m_viewport_width, m_viewport_height);

    // draw axes always on top
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

  if (!m_visible) {
        return;
    }

    if (viewMXptr == nullptr) {
        std::cerr << "Error: CoordAxes::Draw(): viewMXptr = nullptr!\n";
        return;
    }

    glm::mat4 invMV = glm::mat4(1.0f);
    glm::mat4 modelMX = glm::mat4(1.0f);
    glm::mat4 viewMX = glm::make_mat4(viewMXptr);

    glm::mat4 preViewMX = glm::make_mat4(viewMXptr);
    viewMX = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -m_dist));
    viewMX = viewMX * preViewMX;
    
    m_shader.Bind();
    m_shader.SetFloatMatrix("projMX", 4, 1, GL_FALSE, glm::value_ptr(projMX));
    m_shader.SetFloatMatrix("viewMX", 4, 1, GL_FALSE, glm::value_ptr(viewMX));

    // ---------
    //  axis
    // ---------
    for (unsigned int i = 0; i < 3; i++) {
        modelMX = m_axis[i].rotMX;
        invMV = glm::inverse(viewMX * modelMX);

        m_shader.SetFloatMatrix("modelMX", 4, 1, GL_FALSE, glm::value_ptr(modelMX));
        m_shader.SetFloatMatrix("invMV", 4, 1, GL_FALSE, glm::value_ptr(invMV));
        m_shader.SetFloatArray("color", 3, 1, glm::value_ptr(m_axis[i].color));

        m_va.Bind();

        // draw cylinder
        m_shader.SetFloat("scale", m_cylRadius * m_scaleFactor);
        m_shader.SetFloat("offset", 0.0f);
        m_shader.SetFloat("length", m_axis[i].length - m_coneHeight * m_scaleFactor);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m_numVertsCyl);

        // draw tip
        m_shader.SetFloat("scale", m_coneRadius * m_scaleFactor);
        m_shader.SetFloat("offset", m_axis[i].length - m_coneHeight * m_scaleFactor);
        m_shader.SetFloat("length", m_coneHeight * m_scaleFactor);
        glDrawArrays(GL_TRIANGLE_FAN, m_numVertsCyl, m_numVertsCone);

        // draw bottom of tip
        m_shader.SetFloat("scale", m_coneRadius * m_scaleFactor);
        m_shader.SetFloat("offset", m_axis[i].length - m_coneHeight * m_scaleFactor);
        m_shader.SetFloat("length", 0.0f);
        glDrawArrays(GL_TRIANGLE_FAN, m_numVertsCyl + m_numVertsCone, m_numVertsDisk);

        // draw bottom of cylinder
        m_shader.SetFloat("scale", m_cylRadius * m_scaleFactor);
        m_shader.SetFloat("offset", 0.0f);
        m_shader.SetFloat("length", 0.0f);
        glDrawArrays(GL_TRIANGLE_FAN, m_numVertsCyl + m_numVertsCone, m_numVertsDisk);
        m_va.Release();
    }

    m_shader.Release();

    // restore old viewport, blending and depth testing
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);

    if (!isDepthTestEnabled) {
        glDisable(GL_DEPTH_TEST);
    }

    if (!isBlendingEnabled) {
        glDisable(GL_BLEND);
    }
}

bool CoordSystem::Init()
{
    createArrow();

    std::string vShaderName = "shader/geomCoordAxis.vert";
    std::string fShaderName = "shader/geomCoordAxis.frag";
    m_shader.SetFileNames(vShaderName.c_str(), fShaderName.c_str());

    projMX = glm::perspective(glm::radians(m_fovy), 1.0f, 0.01f, 100.0f);    
    return ReloadShaders();
}

bool CoordSystem::ReloadShaders()
{
    return m_shader.ReloadShaders();
}

void CoordSystem::SetAxisColor(unsigned int axis, float r, float g, float b)
{
    if (axis <= 2) {
        m_axis[axis].color = glm::vec3(r, g, b);
    }
}

void CoordSystem::SetAxisLength(unsigned int axis, float length)
{
    if (axis <= 2) {
        m_axis[axis].length = length;
    }
}

void CoordSystem::SetViewport(int x, int y, int width, int height)
{
    m_viewport_x = x;
    m_viewport_y = y;
    m_viewport_width = width;
    m_viewport_height = height;
}

void CoordSystem::createArrow()
{
    m_va.Release();

    m_numVertsCyl = (m_numStreaks + 1) * 2;
    m_numVertsCone = m_numStreaks + 2;
    m_numVertsDisk = m_numStreaks + 2;

    size_t numVerts = static_cast<size_t>(m_numVertsCyl + m_numVertsCone + m_numVertsDisk);
    float step = glm::radians(360.0f) / m_numStreaks;

    float* verts = new float[numVerts * 3];
    float* norms = new float[numVerts * 3];

    float* vptr = verts;
    float* nptr = norms;
    float phi, cosphi, sinphi;

    for (int i = 0; i < m_numStreaks + 1; i++) {
        phi = i * step;
        cosphi = cosf(phi);
        sinphi = sinf(phi);
        *(vptr++) = cosphi;
        *(vptr++) = sinphi;
        *(vptr++) = 0.0f;
        *(nptr++) = cosphi;
        *(nptr++) = sinphi;
        *(nptr++) = 0.0f;

        *(vptr++) = cosphi;
        *(vptr++) = sinphi;
        *(vptr++) = 1.0f;
        *(nptr++) = cosphi;
        *(nptr++) = sinphi;
        *(nptr++) = 0.0f;
    }

    // tip of cone
    *(vptr++) = 0.0f;
    *(vptr++) = 0.0f;
    *(vptr++) = 1.0f;
    *(nptr++) = 0.0f;
    *(nptr++) = 0.0f;
    *(nptr++) = 1.0f;

    glm::vec3 r, t, b, n;
    for (int i = 0; i < m_numStreaks + 1; i++) {
        phi = i * step;
        cosphi = cosf(phi);
        sinphi = sinf(phi);
        r = glm::vec3(cosphi, sinphi, 0.0f);
        *(vptr++) = r.x;
        *(vptr++) = r.y;
        *(vptr++) = 0.0f;

        t = glm::vec3(-r.y, r.x, 0.0f);
        b = -r + glm::vec3(0, 0, 1.0f);
        n = glm::normalize(glm::cross(t, b));
        *(nptr++) = n.x;
        *(nptr++) = n.y;
        *(nptr++) = n.z;
    }

    // center of disk;
    *(vptr++) = 0.0f;
    *(vptr++) = 0.0f;
    *(vptr++) = 0.0f;
    *(nptr++) = 0.0f;
    *(nptr++) = 0.0f;
    *(nptr++) = -1.0f;

    for (int i = 0; i < m_numStreaks + 1; i++) {
        phi = i * step;
        cosphi = cosf(phi);
        sinphi = sinf(phi);
        *(vptr++) = cosphi;
        *(vptr++) = sinphi;
        *(vptr++) = 0.0f;
        *(nptr++) = 0.0f;
        *(nptr++) = 0.0f;
        *(nptr++) = -1.0f;
    }

    m_va.Create(static_cast<unsigned int>(numVerts));
    m_va.SetArrayBuffer(0, GL_FLOAT, 3, verts);
    m_va.SetArrayBuffer(1, GL_FLOAT, 3, norms);

    delete[] verts;
    delete[] norms;
}

void CoordSystem::setModelMatrix()
{
    modelMX = glm::mat4(1.0f);
}