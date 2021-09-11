/**
 * File:    main.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */

#include "Utilities.h"
#include "Camera.h"

// enable/disable in cmake
//#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"

const std::uint8_t Camera::NumProjections = 5;

const char* const Camera::ProjectionNames[]
    = { "Orthographic", "Perspective", "Unknown" };

const std::uint8_t Camera::NumTypes = 4;

const char* const Camera::TypeNames[] = { "Orbit", "Quat", "Free", "Unknown" };

Camera::Projection Camera::GetProjByName(const char* name)
{
    for (std::uint8_t i = 0; i < NumProjections; i++) {
        if (strcmp(name, ProjectionNames[i]) == 0) {
            return static_cast<Projection>(i);
        }
    }
    return Projection::UNKNOWN;
}

const char* Camera::GetProjName(Projection proj)
{
    std::uint8_t idx = static_cast<std::uint8_t>(proj);
    if (idx < NumProjections) {
        return ProjectionNames[idx];
    }
    return ProjectionNames[NumProjections - 1];
}

Camera::Type Camera::GetTypeByName(const char* name)
{
    for (std::uint8_t i = 0; i < NumTypes; i++) {
        if (strcmp(name, TypeNames[i]) == 0) {
            return static_cast<Type>(i);
        }
    }
    return Type::UNKNOWN;
}

const char* Camera::GetTypeName(Type type)
{
    std::uint8_t idx = static_cast<std::uint8_t>(type);
    if (idx < NumTypes) {
        return TypeNames[idx];
    }
    return TypeNames[NumTypes - 1];
}

Camera::Camera()
    : cmView(CMView::NoCubemap)
    , camType(Type::ORBIT_SPHERICAL)
    , camAllowedActions(static_cast<int>(Action::NOTHING))
    , camProjection(Projection::PERSPECTIVE)
    , camActFlyScaleFactor(0.01)
    , camActMoveScaleFactor(0.01)
    , camActRotScaleFactor(0.01)
    , camActRollScaleFactor(0.05)
    , m_useInverseYaw(false)
{
    viewMX = glm::mat4(1.0);
    projMX = glm::mat4(1.0);
    m_quat = Quaternion<double>(0, 0, 0, 1);

    setStandardCamera();

    for (int i = 0; i < 6; i++) {
        m_specPoI[i] = glm::dvec3(0.0, 0.0, 0.0);
    }
    m_keepCurrPoIForSpec = false;
}

Camera::Camera(const Camera& other)
    : viewMX(other.viewMX)
    , projMX(other.projMX)
    , cmView(CMView::NoCubemap)
    , camType(Type::ORBIT_SPHERICAL)
    , camAllowedActions(other.camAllowedActions)
    , camProjection(other.camProjection)
    , camActFlyScaleFactor(other.camActFlyScaleFactor)
    , camActMoveScaleFactor(other.camActMoveScaleFactor)
    , camActRotScaleFactor(other.camActRotScaleFactor)
    , camRes(other.camRes)
    , camAspect(other.camAspect)
    , camFoVv(other.camFoVv)
    , camFoVh(other.camFoVh)
    , camZnear(other.camZnear)
    , camZfar(other.camZfar)
    , camPos(other.camPos)
    , camPoI(other.camPoI)
    , camRFup(other.camRFup)
    , camRFright(other.camRFright)
    , camRFdir(other.camRFdir)
    , camOrthoView(other.camOrthoView)
    , camViewAngleKsi(other.camViewAngleKsi)
    , camViewAngleChi(other.camViewAngleChi)
    , cam_base_rot1(other.cam_base_rot1)
    , cam_base_rot2(other.cam_base_rot2)
    , cam_pix_offset(other.cam_pix_offset)
    , m_useInverseYaw(other.m_useInverseYaw)
{
    m_quat = other.m_quat;
    calcClippingNormals();
    UpdateMatrices();

    for (int i = 0; i < 6; i++) {
        m_specPoI[i] = other.m_specPoI[i];
    }
    m_keepCurrPoIForSpec = other.m_keepCurrPoIForSpec;
}

Camera::Camera(const Camera& other, CoordAxis ca)
    : viewMX(other.viewMX)
    , projMX(other.projMX)
    , cmView(CMView::NoCubemap)
    , camType(Type::ORBIT_SPHERICAL)
    , camAllowedActions(other.camAllowedActions)
    , camProjection(other.camProjection)
    , camActFlyScaleFactor(other.camActFlyScaleFactor)
    , camActMoveScaleFactor(other.camActMoveScaleFactor)
    , camActRotScaleFactor(other.camActRotScaleFactor)
    , camActRollScaleFactor(other.camActRollScaleFactor)
    , camRes(other.camRes)
    , camAspect(other.camAspect)
    , camFoVv(other.camFoVv)
    , camFoVh(other.camFoVh)
    , camZnear(other.camZnear)
    , camZfar(other.camZfar)
    , camPos(other.camPos)
    , camPoI(other.camPoI)
    , camRFup(other.camRFup)
    , camRFright(other.camRFright)
    , camRFdir(other.camRFdir)
    , camOrthoView(other.camOrthoView)
    , camViewAngleKsi(other.camViewAngleKsi)
    , camViewAngleChi(other.camViewAngleChi)
    , m_useInverseYaw(other.m_useInverseYaw)
{
    glm::dvec3 r, d, u;
    getLRFrame(ca, r, d, u);

    camRFdir = d;
    camRFup = u;
    camRFright = r;

    m_quat = Quaternion<double>(0, 0, 0, 1);
    calcClippingNormals();
    UpdateMatrices();

    for (int i = 0; i < 6; i++) {
        m_specPoI[i] = other.m_specPoI[i];
    }
    m_keepCurrPoIForSpec = other.m_keepCurrPoIForSpec;
}

Camera::~Camera(void) {}

const float* Camera::GetViewMatrixPtr() const
{
    if (cmView == CMView::NoCubemap) {
        return glm::value_ptr(viewMX);
    }

    return glm::value_ptr(cmViewMX);
}

const float* Camera::GetInvViewMatrixPtr() const
{
    return glm::value_ptr(invViewMX);
}

const float* Camera::GetProjMatrixPtr() const
{
    return glm::value_ptr(projMX);
}

Camera::Projection Camera::GetProjection()
{
    return camProjection;
}

const char* Camera::GetProjectionName()
{
    return GetProjName(camProjection);
}

double Camera::GetAlpha()
{
    return glm::degrees(camAlpha);
}

float Camera::GetAlphaF()
{
    return glm::degrees(static_cast<float>(camAlpha));
}

int Camera::GetApparentSphereSize(const double radius)
{
    double dist = glm::length(camPoI - camPos);
    return GetApparentSphereSize(radius, dist);
}

int Camera::GetApparentSphereSize(const double radius, const double dist)
{
    if (fabs(radius / dist) > 1.0) {
        return -1;
    }

    double xi = asin(radius / dist);
    double pixRes = GetPixelResolution();
    return static_cast<int>(xi / pixRes);
}

double Camera::GetAspectRatio()
{
    return camAspect;
}

float Camera::GetAspectRatioF()
{
    return static_cast<float>(camAspect);
}

void Camera::GetClipPlanes(double& zNear, double& zFar)
{
    zNear = camZnear;
    zFar = camZfar;
}

void Camera::GetClipPlanesF(float& zNear, float& zFar)
{
    zNear = static_cast<float>(camZnear);
    zFar = static_cast<float>(camZfar);
}

double Camera::GetDistance()
{
    return glm::length(camPos - camPoI);
}

float Camera::GetDistanceF()
{
    return glm::length(glm::vec3(camPos - camPoI));
}

double Camera::GetFoVh()
{
    return camFoVh;
}

double Camera::GetFoVv()
{
    return camFoVv;
}

void Camera::GetLocalViewDir(int px, int py, double& kd, double& kr, double& ku, bool upsideDown)
{
    glm::dvec3 viewDir = getLocalViewDir(px, py, upsideDown);
    kd = viewDir.x;
    kr = viewDir.y;
    ku = viewDir.z;
}

glm::dvec3 Camera::getLocalViewDir(int px, int py, bool upsideDown)
{
    double kd = 0.0, kr = 0.0, ku = 0.0;

    if (camProjection == Projection::PERSPECTIVE) {
        double tf = tan(glm::radians(camFoVv) * 0.5);
        kd = 1.0;
        kr = camAspect * (1.0 - 2.0 * (px + cam_pix_offset.x) / static_cast<double>(camRes.x)) * tf;
        if (upsideDown) {
            ku = (2.0 * (py + cam_pix_offset.y) / static_cast<double>(camRes.y) - 1.0) * tf;
        }
        else {
            ku = (1.0 - 2.0 * (py + cam_pix_offset.y) / static_cast<double>(camRes.y)) * tf;
        }
    }
    
    glm::dvec3 viewDir = glm::dvec3(kd, kr, ku);
    return glm::normalize(viewDir);
}

void Camera::GetGlobalViewDir(int px, int py, double& dx, double& dy, double& dz, bool upsideDown)
{
    glm::dvec3 viewDir = getGlobalViewDir(px, py, upsideDown);
    dx = viewDir.x;
    dy = viewDir.y;
    dz = viewDir.z;
}

glm::dvec3 Camera::getGlobalViewDir(int px, int py, bool upsideDown)
{
    glm::dvec3 viewDirLoc = getLocalViewDir(px, py, upsideDown);
    glm::dvec3 viewDir = viewDirLoc.x * camRFdir + viewDirLoc.y * camRFright + viewDirLoc.z * camRFup;
    return viewDir;
}

bool Camera::GetPixelFromGlobalDir(const double dx, const double dy, const double dz, int& px, int& py, bool upsideDown)
{
    glm::dvec3 d = glm::dvec3(dx, dy, dz);
    double ku = glm::dot(camRFup, d);
    double kr = glm::dot(camRFright, d);
    double kv = glm::dot(camRFdir, d);

    ku /= kv;
    kr /= kv;

    px = py = -1;

    if (camProjection == Projection::PERSPECTIVE) {
        double tf = tan(glm::radians(camFoVv) * 0.5);
        px = static_cast<int>(0.5 * camRes.x * (1.0 - kr / (camAspect * tf)) + 0.0);
        if (upsideDown) {
            py = static_cast<int>((1.0 + ku / tf) * camRes.y * 0.5 + 0.5);
        }
        else {
            py = static_cast<int>((1.0 - ku / tf) * camRes.y * 0.5 + 0.5);
        }
    }

    return (px >= 0 && px < camRes[0] && py >= 0 && py < camRes[1] && kv > 0);
}

bool Camera::GetPixelFromGlobalPos(double x, double y, double z, int& px, int& py, bool upsideDown)
{
    glm::dvec3 p = glm::dvec3(x, y, z);
    glm::dvec3 d = p - camPos;

    return GetPixelFromGlobalDir(d[0], d[1], d[2], px, py, upsideDown);
}

bool Camera::GetPixelFromGlobalPos(double* pos, int& px, int& py, bool upsideDown)
{
    if (pos == nullptr) {
        return false;
    }
    return GetPixelFromGlobalPos(pos[0], pos[1], pos[2], px, py, upsideDown);
}

double Camera::GetPixelResolution()
{
    double tf = tan(0.5 * glm::radians(camFoVv));
    return static_cast<double>(atan(2.0 * tf * camRes[1] / (camRes[1] * camRes[1] - tf * tf)));
}

bool Camera::FindIntersec(int px, int py, CoordPlane cplane, double& ipx, double& ipy, double& ipz, bool upsideDown)
{
    glm::dvec3 d = getGlobalViewDir(px, py, upsideDown);

    switch (cplane) {
        case CoordPlane::XY: {
            if (fabs(d.z) < 1e-6 || fabs(camPos.z) < 1e-6)
                return false;
            double lambda = -camPos.z / d.z;
            glm::dvec3 ip = camPos + lambda * d;
            ipx = ip.x;
            ipy = ip.y;
            ipz = ip.z;
            return true;
        }
        case CoordPlane::XZ: {
            // TODO
            break;
        }
        case CoordPlane::YZ: {
            // TODO
            break;
        }
    }
    return false;
}

void Camera::GetPoI(double& x, double& y, double& z)
{
    x = camPoI.x;
    y = camPoI.y;
    z = camPoI.z;
}

void Camera::GetPoI(double* xyz)
{
    xyz[0] = camPoI.x;
    xyz[1] = camPoI.y;
    xyz[2] = camPoI.z;
}

void Camera::GetPoIF(float& x, float& y, float& z)
{
    x = static_cast<float>(camPoI.x);
    y = static_cast<float>(camPoI.y);
    z = static_cast<float>(camPoI.z);
}

void Camera::GetPoIF(float* xyz)
{
    if (xyz != nullptr) {
        GetPoIF(xyz[0], xyz[1], xyz[2]);
    }
}

void Camera::GetPosition(double& px, double& py, double& pz)
{
    px = camPos.x;
    py = camPos.y;
    pz = camPos.z;
}

void Camera::GetPosition(double* xyz)
{
    xyz[0] = camPos.x;
    xyz[1] = camPos.y;
    xyz[2] = camPos.z;
}

double* Camera::GetPositionPtr()
{
    return glm::value_ptr(camPos);
}

void Camera::GetPositionF(float& px, float& py, float& pz)
{
    px = static_cast<float>(camPos.x);
    py = static_cast<float>(camPos.y);
    pz = static_cast<float>(camPos.z);
}

void Camera::GetPositionF(float* xyz)
{
    this->GetPositionF(xyz[0], xyz[1], xyz[2]);
}

void Camera::GetRelativePosition(double& rx, double& ry, double& rz)
{
    rx = camPos.x - camPoI.x;
    ry = camPos.y - camPoI.y;
    rz = camPos.z - camPoI.z;
}

void Camera::GetRelativePositionF(float& rx, float& ry, float& rz)
{
    glm::vec3 rp = glm::vec3(camPos - camPoI);
    rx = rp.x;
    ry = rp.y;
    rz = rp.z;
}

void Camera::GetRelativeSphericalPosition(double& r, double& theta, double& phi)
{
    double rx, ry, rz;
    GetRelativePosition(rx, ry, rz);
}

void Camera::GetResolution(int& resH, int& resV)
{
    resH = camRes.x;
    resV = camRes.y;
}

void Camera::GetDirVec(double& dx, double& dy, double& dz)
{
    dx = camRFdir.x;
    dy = camRFdir.y;
    dz = camRFdir.z;
}

void Camera::GetDirVecF(float& dx, float& dy, float& dz)
{
    dx = static_cast<float>(camRFdir.x);
    dy = static_cast<float>(camRFdir.y);
    dz = static_cast<float>(camRFdir.z);
}

void Camera::GetDirVecF(float* dxyz)
{
    if (dxyz == nullptr) {
        return;
    }

    GetDirVecF(dxyz[0], dxyz[1], dxyz[2]);
}

void Camera::GetRightVec(double& rx, double& ry, double& rz)
{
    rx = camRFright.x;
    ry = camRFright.y;
    rz = camRFright.z;
}

void Camera::GetRightVecF(float& rx, float& ry, float& rz)
{
    rx = static_cast<float>(camRFright.x);
    ry = static_cast<float>(camRFright.y);
    rz = static_cast<float>(camRFright.z);
}

void Camera::GetRightVecF(float* xyz)
{
    if (xyz != nullptr) {
        GetRightVecF(xyz[0], xyz[1], xyz[2]);
    }
}

void Camera::GetUpVec(double& ux, double& uy, double& uz)
{
    ux = camRFup.x;
    uy = camRFup.y;
    uz = camRFup.z;
}

void Camera::GetUpVecF(float& ux, float& uy, float& uz)
{
    ux = static_cast<float>(camRFup.x);
    uy = static_cast<float>(camRFup.y);
    uz = static_cast<float>(camRFup.z);
}

void Camera::GetUpVecF(float* xyz)
{
    if (xyz != nullptr) {
        GetUpVecF(xyz[0], xyz[1], xyz[2]);
    }
}

int Camera::GetHeight()
{
    return camRes[1];
}

int Camera::GetWidth()
{
    return camRes[0];
}

void Camera::Move(CoordPlane plane, double dx, double dy)
{
    switch (plane) {
        case CoordPlane::XY: {
            camPos[0] += dx;
            camPos[1] += dy;
            break;
        }
        case CoordPlane::XZ: {
            camPos[0] += dx;
            camPos[2] += dy;
            break;
        }
        case CoordPlane::YZ: {
            camPos[1] += dx;
            camPos[2] += dy;
            break;
        }
    }
}

void Camera::MoveAxis(CoordAxis axis, double d)
{
    double dist = GetDistance();
    d = d / static_cast<double>(camRes.y) * tan(glm::radians(std::min(camFoVv, 85.0))) * dist;

    switch (axis) {
        default:
            break;
        case CoordAxis::X: {
            camPos[0] += d;
            camPoI[0] += d;
            break;
        }
        case CoordAxis::Y: {
            camPos[1] += d;
            camPoI[1] += d;
            break;
        }
        case CoordAxis::Z: {
            camPos[2] += d;
            camPoI[2] += d;
            break;
        }
    }
}

void Camera::MovePlane(CoordPlane plane, double dx, double dy)
{
    double dist = GetDistance();
    dx = dx / static_cast<double>(camRes.y) * tan(glm::radians(camFoVv)) * dist;
    dy = dy / static_cast<double>(camRes.y) * tan(glm::radians(camFoVv)) * dist;

    switch (plane) {
        case CoordPlane::XY: {
            camPos[0] -= dx;
            camPos[1] += dy;
            camPoI[0] -= dx;
            camPoI[1] += dy;
            break;
        }
        case CoordPlane::XZ: {
            camPos[0] += dx;
            camPos[2] += dy;
            camPoI[0] += dx;
            camPoI[2] += dy;
            break;
        }
        case CoordPlane::YZ: {
            camPos[1] += dx;
            camPos[2] += dy;
            camPoI[1] += dx;
            camPoI[2] += dy;
            break;
        }
    }
}

void Camera::MovePOItoOrigin()
{
    glm::dvec3 rel = camPos - camPoI;
    camPos = rel;
    camPoI = glm::dvec3(0.0, 0.0, 0.0);
}

void Camera::Pan(const double h, const double v)
{
    camPos = camPos + h * camRFright + v * camRFup;
    camPoI = camPoI + h * camRFright + v * camRFup;
}

void Camera::Pan(const int px, const int py)
{
    double dist = GetDistance();
    dist = pow(dist, 1.5);
    double h
        = px / static_cast<double>(camRes.x) * tan(glm::radians(0.5 * camFoVh)) * dist * camActMoveScaleFactor * 2.0;
    double v
        = py / static_cast<double>(camRes.y) * tan(glm::radians(0.5 * camFoVv)) * dist * camActMoveScaleFactor * 2.0;
    Pan(h, v);
}

void Camera::Pan(const double dx, const double dy, const double dz)
{
    camPos += glm::dvec3(dx, dy, dz);
    camPoI += glm::dvec3(dx, dy, dz);

    calcClippingNormals();
    UpdateMatrices();
}

void Camera::Pitch(double angle)
{
    double pitch = glm::radians(angle);
    camRFdir = glm::rotate(camRFdir, pitch, camRFright);
    camRFup = glm::rotate(camRFup, pitch, camRFright);

    calcClippingNormals();
    UpdateMatrices();
}

void Camera::Yaw(double angle)
{
    double yaw = glm::radians(angle) * (m_useInverseYaw ? -1.0 : 1.0);
    camRFdir = glm::rotate(camRFdir, yaw, -camRFup);
    camRFright = glm::rotate(camRFright, yaw, -camRFup);

    calcClippingNormals();
    UpdateMatrices();
}

void Camera::SetInverseYaw(bool inverse)
{
    m_useInverseYaw = inverse;
}

bool Camera::IsInverseYaw()
{
    return m_useInverseYaw;
}

void Camera::SetPixelOffset(double px, double py)
{
    cam_pix_offset = glm::dvec2(px, py);
}

void Camera::Roll(double angle)
{
    if (camType == Type::ORBIT_QUATERNION) {
        Quaternion<double> rl;
        rl.SetRot(-glm::radians(angle), camRFdir.x, camRFdir.y, camRFdir.z);
        m_quat = rl * m_quat;

        double dist = GetDistance();
        camPos = glm::dvec3(0, 0, dist);
        Quaternion<double> p(0.0, camPos.x, camPos.y, camPos.z);
        p = p | m_quat;

        double x, y, z;
        p.GetVector(x, y, z);
        camPos = glm::dvec3(x, y, z);
        camPos += camPoI;

        m_quat.CalcMat();
        m_quat.GetCamRight(x, y, z);
        camRFright = glm::dvec3(x, y, z);

        m_quat.GetCamUp(x, y, z);
        camRFup = glm::dvec3(x, y, z);
    }
    else {
        double roll = glm::radians(angle);
        camRFright = glm::rotate(camRFright, roll, camRFdir);
        camRFup = glm::rotate(camRFup, roll, camRFdir);
    }

    calcClippingNormals();
    UpdateMatrices();
}

void Camera::Fly(double step, bool distScaled)
{
    double fac = (distScaled ? GetDistance() : 1.0);
    camPos += step * fac * camRFdir;
}

void Camera::Dolly(double step)
{
    glm::dvec3 rpos = camPos - camPoI;
    double distToPoI = glm::length(rpos);
    distToPoI += step;
    camPos = camPoI - distToPoI * camRFdir;
}

void Camera::Orbit(double dazi, double dlat, bool use_local_z)
{
    // ----------------------------
    //  spherical orbit camera
    // ----------------------------
    if (camType == Type::ORBIT_SPHERICAL) {
        glm::dvec3 rpos = camPos - camPoI;
        double distToPoI = glm::length(rpos);

        double theta = acos(rpos.z / glm::length(rpos));
        double phi = atan2(rpos.y, rpos.x);

        phi += dazi * camActRotScaleFactor;
        theta += dlat * camActRotScaleFactor;
        theta = glm::clamp(theta, 1e-6, glm::radians(180.0) - 1e-6);

        camPos = camPoI + distToPoI * glm::dvec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

        camRFdir = glm::normalize(camPoI - camPos);
        glm::dvec3 z = glm::dvec3(0, 0, 1);
        camRFright = glm::normalize(glm::cross(camRFdir, z));
        camRFup = glm::cross(camRFright, camRFdir);
    }
    // ----------------------------
    //  quaternion camera
    // ----------------------------
    else if (camType == Type::ORBIT_QUATERNION) {
        double al = dlat * camActRotScaleFactor;
        double az = dazi * camActRotScaleFactor;

        Quaternion<double> rz;
        if (use_local_z) {
            rz.SetRot(az, 0.0, 0.0, 1.0);
        }
        else {
            rz.SetRot(az, camRFup.x, camRFup.y, camRFup.z);
        }

        Quaternion<double> rl;
        // rl.SetRot(al, 0.0, 1.0, 0.0);
        rl.SetRot(al, camRFright.x, camRFright.y, camRFright.z);
        m_quat = rz * rl * m_quat;

        double dist = GetDistance();
        camPos = glm::dvec3(0, 0, dist);
        Quaternion<double> p(0.0, camPos.x, camPos.y, camPos.z);
        p = p | m_quat;

        double x, y, z;
        p.GetVector(x, y, z);
        camPos = glm::dvec3(x, y, z);
        camPos += camPoI;

        m_quat.CalcMat();
        m_quat.GetCamDir(x, y, z);
        camRFdir = glm::dvec3(-x, -y, -z);

        m_quat.GetCamRight(x, y, z);
        camRFright = glm::dvec3(x, y, z);

        m_quat.GetCamUp(x, y, z);
        camRFup = glm::dvec3(x, y, z);
    }

    calcClippingNormals();
    UpdateMatrices();
}

void Camera::OrbitByAngle(double dazi, double dlat, bool use_local_z)
{
    // ----------------------------
    //  spherical orbit camera
    // ----------------------------
    if (camType == Type::ORBIT_SPHERICAL) {
        glm::dvec3 rpos = camPos - camPoI;
        double distToPoI = glm::length(rpos);

        double theta = acos(rpos.z / glm::length(rpos));
        double phi = atan2(rpos.y, rpos.x);

        phi += glm::radians(dazi);
        theta += glm::radians(dlat);
        theta = glm::clamp(theta, 1e-6, glm::radians(180.0) - 1e-6);

        camPos = camPoI + distToPoI * glm::dvec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

        camRFdir = glm::normalize(camPoI - camPos);
        glm::dvec3 z = glm::dvec3(0, 0, 1);
        camRFright = glm::normalize(glm::cross(camRFdir, z));
        camRFup = glm::cross(camRFright, camRFdir);
    }
    // ----------------------------
    //  quaternion camera
    // ----------------------------
    else if (camType == Type::ORBIT_QUATERNION) {
        double al = glm::radians(dlat);
        double az = glm::radians(dazi);

        Quaternion<double> rz;
        if (use_local_z) {
            rz.SetRot(az, 0.0, 0.0, 1.0);
        }
        else {
            rz.SetRot(az, camRFup.x, camRFup.y, camRFup.z);
        }

        Quaternion<double> rl;
        // rl.SetRot(al, 0.0, 1.0, 0.0);
        rl.SetRot(al, camRFright.x, camRFright.y, camRFright.z);
        m_quat = rz * rl * m_quat;

        double dist = GetDistance();
        camPos = glm::dvec3(0, 0, dist);
        Quaternion<double> p(0.0, camPos.x, camPos.y, camPos.z);
        p = p | m_quat;

        double x, y, z;
        p.GetVector(x, y, z);
        camPos = glm::dvec3(x, y, z);
        camPos += camPoI;

        m_quat.CalcMat();
        m_quat.GetCamDir(x, y, z);
        camRFdir = glm::dvec3(-x, -y, -z);

        m_quat.GetCamRight(x, y, z);
        camRFright = glm::dvec3(x, y, z);

        m_quat.GetCamUp(x, y, z);
        camRFup = glm::dvec3(x, y, z);
    }

    calcClippingNormals();
    UpdateMatrices();
}

void Camera::SetBaseAxes(BaseAxes axes)
{
    switch (axes) {
        default:
        case BaseAxes::XYZ: {
            cam_base_rot1 = glm::vec4(1, 0, 0, 0);
            cam_base_rot2 = glm::vec4(1, 0, 0, 0);
            break;
        }
        case BaseAxes::ZXY: {
            cam_base_rot1 = glm::vec4(1, 0, 0, glm::radians(90.0f));
            cam_base_rot2 = glm::vec4(0, 0, 1, glm::radians(90.0f));
            break;
        }
        case BaseAxes::YZX: {
            cam_base_rot1 = glm::vec4(0, 1, 0, -glm::radians(90.0f));
            cam_base_rot2 = glm::vec4(0, 0, 1, -glm::radians(90.0f));
            break;
        }
    }

    cam_base_axes = axes;
}

bool Camera::IsPointInClippingVolume(double posX, double posY, double posZ)
{
    return IsSphereVisible(posX, posY, posZ, 0.0);
}

bool Camera::IsSphereVisible(double cx, double cy, double cz, double radius)
{
    glm::dvec3 center = glm::dvec3(cx, cy, cz);
    glm::dvec3 r = center - camPos;

    if (glm::length(r) <= radius) {
        return true;
    }

    if ((glm::dot(r, camNup) > radius) || (glm::dot(r, camNbt) > radius) || (glm::dot(r, camNleft) > radius)
        || (glm::dot(r, camNright) > radius)) {
        return false;
    }

    return true;
}

bool Camera::IsSphereVisibleF(float cx, float cy, float cz, float radius)
{
    double dx = static_cast<double>(cx);
    double dy = static_cast<double>(cy);
    double dz = static_cast<double>(cz);
    double dr = static_cast<double>(radius);
    return IsSphereVisible(dx, dy, dz, dr);
}

bool Camera::IsSphereVisible(double* c, double radius)
{
    if (c == nullptr) {
        return false;
    }
    return IsSphereVisible(c[0], c[1], c[2], radius);
}

bool Camera::IsSphereVisibleF(float* c, float radius)
{
    if (c == nullptr) {
        return false;
    }
    return IsSphereVisibleF(c[0], c[1], c[2], radius);
}

bool Camera::UpdateMouse(double dx, double dy, Action act)
{
    bool actionHandled = false;

    if ((static_cast<int>(act) & camAllowedActions) == static_cast<int>(act)) {
        actionHandled = true;
        switch (act) {
            case Action::NOTHING: {
                break;
            }
            case Action::ORBIT: {
                Orbit(dx, dy);
                break;
            }
            case Action::ORBIT_Z: {
                Orbit(dx, dy, true);
                break;
            }
            case Action::DOLLY: {
                Dolly(dy * camActMoveScaleFactor);
                break;
            }
            case Action::MOVE_X: {
                MoveAxis(CoordAxis::X, dx * camActMoveScaleFactor);
                break;
            }
            case Action::MOVE_Y: {
                MoveAxis(CoordAxis::Y, dy * camActMoveScaleFactor);
                break;
            }
            case Action::MOVE_Z: {
                MoveAxis(CoordAxis::Z, dy * camActMoveScaleFactor);
                break;
            }
            case Action::MOVE_XY: {
                MovePlane(CoordPlane::XY, dx * camActMoveScaleFactor, dy * camActMoveScaleFactor);
                break;
            }
            case Action::MOVE_XZ: {
                MovePlane(CoordPlane::XZ, dx * camActMoveScaleFactor, dy * camActMoveScaleFactor);
                break;
            }
            case Action::MOVE_YZ: {
                MovePlane(CoordPlane::YZ, dx * camActMoveScaleFactor, dy * camActMoveScaleFactor);
                break;
            }
            case Action::PAN: {
                Pan(static_cast<int>(dx), static_cast<int>(dy));
                break;
            }
            case Action::PITCH: {
                Pitch(dy * camActRotScaleFactor);
                break;
            }
            case Action::YAW: {
                Yaw(dx * camActRotScaleFactor);
                break;
            }
            case Action::ROLL: {
                Roll(dx * camActRollScaleFactor);
                break;
            }
            case Action::FLY: {
                Fly(dy * camActFlyScaleFactor);
                break;
            }
            case Action::FLY_SCALED: {
                Fly(dy * camActFlyScaleFactor, true);
                break;
            }
            case Action::ALL: {
                // Do not use this...
                break;
            }
        }
    }

    UpdateMatrices();
    return actionHandled;
}

void Camera::GetOrthoSize(double& sx, double& sy)
{
    double osize = GetDistance() * tan(glm::radians(camFoVv) * 0.5);
    sx = osize * camAspect;
    sy = osize;
}

void Camera::GetOrthoSizeF(float& sx, float& sy)
{
    double dx, dy;
    GetOrthoSize(dx, dy);
    sx = static_cast<float>(dx);
    sy = static_cast<float>(dy);
}

void Camera::SetYawPitchRoll(double yaw, double pitch, double role)
{
    camPos = glm::dvec3(0.0, 0.0, 0.0);

    camRFdir = glm::dvec3(0.0, 1.0, 0.0);
    camRFright = glm::dvec3(1.0, 0.0, 0.0);
    camRFup = glm::dvec3(0.0, 0.0, 1.0);

    Yaw(yaw);
    Pitch(pitch);
    Roll(role);
}

void Camera::calc_ortho_matrix(glm::mat4& mat)
{
    double sx, sy;
    GetOrthoSize(sx, sy);
    // double osize = GetDistance() * tan(glm::radians(camFoVv) * 0.5);
    // camOrthoView = glm::vec4(-osize*camAspect, osize*camAspect, -osize, osize);
    camOrthoView = glm::vec4(-sx, sx, -sy, sy);
    glm::vec4 orthoView = glm::vec4(camOrthoView);
    mat = glm::ortho(
        orthoView.x, orthoView.y, orthoView.z, orthoView.w, static_cast<float>(camZnear), static_cast<float>(camZfar));
}

void Camera::calc_persp_matrix(glm::mat4& mat)
{
    // projMX = glm::perspective( camFoVv, camAspect, camZnear, camZfar );
    double tf = tan(glm::radians(camFoVv) * 0.5);
    mat = glm::mat4(static_cast<float>(1.0 / (camAspect * tf)), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f / static_cast<float>(tf),
        0.0f, 0.0f, 0.0f, 0.0f, -static_cast<float>((camZfar + camZnear) / (camZfar - camZnear)),
        -static_cast<float>(2.0 * camZfar * camZnear / (camZfar - camZnear)), 0.0f, 0.0f, -1.0f, 0.0f);

    mat = glm::transpose(mat);
}

void Camera::UpdateMatrices()
{
    // -----------------------------
    //  update perspective matrix
    // -----------------------------
    if (camProjection == Projection::ORTHOGRAPHIC) {
        calc_ortho_matrix(projMX);
    }
    else if (camProjection == Projection::PERSPECTIVE) {
        calc_persp_matrix(projMX);
    }

    // -----------------------------
    //  update view matrix
    // -----------------------------
    if (camType == Type::ORBIT_SPHERICAL || camType == Type::FREE_FLIGHT || camType == Type::DOME) {
        glm::dmat4 vm = glm::mat4(camRFright.x, camRFright.y, camRFright.z, 0.0, camRFup.x, camRFup.y, camRFup.z, 0.0,
            -camRFdir.x, -camRFdir.y, -camRFdir.z, 0.0, 0.0, 0.0, 0.0, 1.0);

        viewMX = glm::mat4(vm);
        viewMX = glm::transpose(viewMX);
        viewMX = glm::translate(viewMX, -glm::vec3(camPos));

    }
    else {
        // Finished ???
        glm::dmat4 vmx = glm::make_mat4(m_quat.ToMat());
        viewMX = glm::mat4(vmx);
        viewMX = glm::translate(viewMX, -glm::vec3(camPos));
    }

    if (cam_base_axes != BaseAxes::XYZ) {
        viewMX = glm::rotate(
            viewMX, static_cast<float>(cam_base_rot2.w), glm::vec3(cam_base_rot2.x, cam_base_rot2.y, cam_base_rot2.z));
        viewMX = glm::rotate(
            viewMX, static_cast<float>(cam_base_rot1.w), glm::vec3(cam_base_rot1.x, cam_base_rot1.y, cam_base_rot1.z));
    }

    invViewMX = glm::inverse(viewMX);
}

bool Camera::GetViewMatrixFromPosUpDir(double* pos, double* up, double* dir, float*& mat)
{
    glm::vec3 d = glm::vec3(glm::make_vec3(dir));
    glm::vec3 u = glm::vec3(glm::make_vec3(up));
    glm::vec3 r = glm::vec3(glm::cross(d, u));

    glm::mat4 viewMX
        = glm::mat4(r.x, r.y, r.z, 0.0f, u.x, u.y, u.z, 0.0f, -d.x, -d.y, -d.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    viewMX = glm::transpose(viewMX);
    viewMX = glm::translate(viewMX, -glm::vec3(glm::make_vec3(pos)));
    memcpy(mat, glm::value_ptr(viewMX), sizeof(float) * 16);
    return true;
}

void Camera::Reset()
{
    // UpdateMouse(0.0f,0.0f,0.0f,0.0f);
}

void Camera::SetAllowedActions(int act)
{
    camAllowedActions = act;
}

void Camera::SetAllowedAction(Action act)
{
    camAllowedActions |= static_cast<int>(act);
}

void Camera::SetClipPlanes(double zNear, double zFar)
{
    camZnear = zNear;
    camZfar = zFar;
    UpdateMatrices();
}

void Camera::SetCurrentView(CMView view)
{
    cmView = view;

    switch (cmView) {
        case CMView::PosX: {
            cm_curr_dir = camRFright;
            cm_curr_up = camRFup;
            cm_curr_right = -camRFdir;
            break;
        }
        case CMView::NegX: {
            cm_curr_dir = -camRFright;
            cm_curr_up = camRFup;
            cm_curr_right = camRFdir;
            break;
        }
        case CMView::PosY: {
            cm_curr_dir = -camRFup;
            cm_curr_up = camRFdir;
            cm_curr_right = camRFright;
            break;
        }
        case CMView::NegY: {
            cm_curr_dir = camRFup;
            cm_curr_up = -camRFdir;
            cm_curr_right = camRFright;
            break;
        }
        case CMView::NoCubemap:
        case CMView::PosZ: {
            cm_curr_dir = camRFdir;
            cm_curr_up = camRFup;
            cm_curr_right = camRFright;
            break;
        }
        case CMView::NegZ: {
            cm_curr_dir = -camRFdir;
            cm_curr_up = camRFup;
            cm_curr_right = -camRFright;
            break;
        }
    }

    glm::vec3 right = glm::vec3(cm_curr_right);
    glm::vec3 up = glm::vec3(cm_curr_up);
    glm::vec3 dir = glm::vec3(cm_curr_dir);
    glm::vec3 pos = glm::vec3(camPos);

    cmViewMX = glm::mat4(
        right.x, right.y, right.z, 0.0f, up.x, up.y, up.z, 0.0f, -dir.x, -dir.y, -dir.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    cmViewMX = glm::transpose(cmViewMX);
    cmViewMX = glm::translate(cmViewMX, -pos);

    calcCurrCMClippingNormals();
}

void Camera::SetDistance(double dist)
{
    double oldDist = this->GetDistance();
    camPos = camPoI + dist / oldDist * (camPos - camPoI);
    UpdateMatrices();
}

void Camera::EnableAllowedAction(Action act)
{
    camAllowedActions |= static_cast<int>(act);
}

void Camera::EnableAllActions()
{
    camAllowedActions = static_cast<int>(Action::ALL);
}

void Camera::DisableAllowedAction(Action act)
{
    camAllowedActions &= ~(static_cast<int>(act));
}

void Camera::SetFoVy(double fovY)
{
    camFoVv = fovY;
    calcAlpha();
    calcHorizontalFieldOfView();
    calcClippingNormals();
    UpdateMatrices();
}

void Camera::SetProjection(Projection proj)
{
    camProjection = proj;
    UpdateMatrices();
}

void Camera::SetProjection(const char* projName)
{
    if (projName != nullptr) {
        camProjection = GetProjByName(projName);
        UpdateMatrices();
    }
}

void Camera::SetProjectionMatrix(const float* matrix, bool is_glm)
{
    if (matrix == nullptr) {
        return;
    }

    projMX = glm::make_mat4(matrix);
    if (!is_glm) {
        projMX = glm::transpose(projMX);
    }
}

void Camera::SetResolution(const int res)
{
    SetResolution(res, res);
}

void Camera::SetResolution(const int resH, const int resV)
{
    camRes = glm::ivec2(resH, resV);
    camAspect = camRes.x / static_cast<double>(camRes.y);

    calcAlpha();
    calcClippingNormals();
    calcHorizontalFieldOfView();
    UpdateMatrices();
}

void Camera::SetPoI2Origin()
{
    SetPoI(0.0, 0.0, 0.0);
}

void Camera::SetPoI(double px, double py, double pz)
{
    camPoI = glm::dvec3(px, py, pz);
    glm::dvec3 vdir = camPoI - camPos;
    setViewDir(vdir);
}

void Camera::SetPoIF(float px, float py, float pz)
{
    this->SetPoI(static_cast<double>(px), static_cast<double>(py), static_cast<double>(pz));
}

void Camera::SetPoI(double* poi)
{
    if (poi != nullptr) {
        this->SetPoI(poi[0], poi[1], poi[2]);
    }
}

void Camera::SetPoIF(float* poi)
{
    if (poi != nullptr) {
        this->SetPoIF(poi[0], poi[1], poi[2]);
    }
}

void Camera::SetPosition(double px, double py, double pz)
{
    camPos = glm::dvec3(px, py, pz);
    if (camType == Type::ORBIT_SPHERICAL || camType == Type::ORBIT_QUATERNION) {
        glm::dvec3 vdir = camPoI - camPos;
        setViewDir(vdir);
    }
}

void Camera::SetPositionF(float px, float py, float pz)
{
    this->SetPosition(static_cast<double>(px), static_cast<double>(py), static_cast<double>(pz));
}

void Camera::SetPosition(double* pos)
{
    if (pos != nullptr) {
        this->SetPosition(pos[0], pos[1], pos[2]);
    }
}

void Camera::SetPositionF(float* pos)
{
    if (pos != nullptr) {
        this->SetPositionF(pos[0], pos[1], pos[2]);
    }
}

void Camera::SetScaleFactors(double moveFactor, double rotFactor)
{
    camActMoveScaleFactor = moveFactor;
    camActRotScaleFactor = rotFactor;
}

void Camera::SetFlyScaleFactor(double flyFactor)
{
    camActFlyScaleFactor = flyFactor;
}

void Camera::SetMoveScaleFactor(double moveFactor)
{
    camActMoveScaleFactor = moveFactor;
}

void Camera::SetRollScaleFactor(double rollFactor)
{
    camActRollScaleFactor = rollFactor;
}

double Camera::GetFlyScaleFactor()
{
    return camActFlyScaleFactor;
}

double Camera::GetMoveScaleFactor()
{
    return camActMoveScaleFactor;
}

double Camera::GetRotScaleFactor()
{
    return camActRotScaleFactor;
}

double Camera::GetRollScaleFactor()
{
    return camActRollScaleFactor;
}

void Camera::SetSpecialPos(const CoordAxis axis)
{
    double dist = this->GetDistance();

#ifdef BE_VERBOSE
    fprintf(stdout, "Camera::SetSpecialPos() ... %d\n", (int)axis);
#endif // BE_VERBOSE

    Quaternion<double> r1, r2;
    m_quat = Quaternion<double>(0, 0, 0, 1);

    if (!m_keepCurrPoIForSpec) {
        camPoI = glm::dvec3(0.0, 0.0, 0.0);
    }
    glm::dvec3 pos;

    switch (axis) {
        case CoordAxis::Undefined:
            break;

        case CoordAxis::X:
        case CoordAxis::Xpos: {
            if (!m_keepCurrPoIForSpec) {
                camPoI = m_specPoI[0];
            }
            pos = camPoI + glm::dvec3(dist, 0.0, 0.0);
            r1.SetRot(glm::radians(-90.0), 0.0, 0.0, 1.0);
            r2.SetRot(glm::radians(90.0), 1.0, 0.0, 0.0);
            m_quat = m_quat * r1 * r2;
            this->SetPosRFrame(pos.x, pos.y, pos.z, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
            break;
        }
        case CoordAxis::Xneg: {
            if (!m_keepCurrPoIForSpec) {
                camPoI = m_specPoI[1];
            }
            pos = camPoI - glm::dvec3(dist, 0.0, 0.0);
            r1.SetRot(glm::radians(90.0), 0.0, 0.0, 1.0);
            r2.SetRot(glm::radians(90.0), 1.0, 0.0, 0.0);
            m_quat = m_quat * r1 * r2;
            this->SetPosRFrame(pos.x, pos.y, pos.z, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
            break;
        }
        case CoordAxis::Y:
        case CoordAxis::Ypos: {
            if (!m_keepCurrPoIForSpec) {
                camPoI = m_specPoI[2];
            }
            pos = camPoI + glm::dvec3(0.0, dist, 0.0);
            r1.SetRot(glm::radians(90.0), 1.0, 0.0, 0.0);
            m_quat = m_quat * r1;
            this->SetPosRFrame(pos.x, pos.y, pos.z, 0.0, -1.0, 0.0, 0.0, 0.0, 1.0);
            break;
        }
        case CoordAxis::Yneg: {
            if (!m_keepCurrPoIForSpec) {
                camPoI = m_specPoI[3];
            }
            pos = camPoI - glm::dvec3(0.0, dist, 0.0);
            r1.SetRot(glm::radians(-90.0), 1.0, 0.0, 0.0);
            r2.SetRot(glm::radians(180.0), 0.0, 0.0, 1.0);
            m_quat = m_quat * r1 * r2;
            this->SetPosRFrame(pos.x, pos.y, pos.z, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
            break;
        }
        case CoordAxis::Z:
        case CoordAxis::Zpos: {
            if (!m_keepCurrPoIForSpec) {
                camPoI = m_specPoI[4];
            }
            pos = camPoI + glm::dvec3(0.0, -0.0001, dist);
            r1.SetRot(glm::radians(180.0), 0.0, 0.0, 1.0);
            m_quat = m_quat * r1;
            this->SetPosRFrame(pos.x, pos.y, pos.z, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);
            break;
        }
        case CoordAxis::Zneg: {
            if (!m_keepCurrPoIForSpec) {
                camPoI = m_specPoI[5];
            }
            pos = camPoI - glm::dvec3(0.0, -0.0001, dist);
            r1.SetRot(glm::radians(180.0), 0.0, 0.0, 1.0);
            r2.SetRot(glm::radians(180.0), 0.0, 1.0, 0.0);
            m_quat = m_quat * r1 * r2;
            this->SetPosRFrame(pos.x, pos.y, pos.z, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
            break;
        }
    }
}

void Camera::DefineSpecialPos(const CoordAxis axis, double poiX, double poiY, double poiZ)
{
    switch (axis) {
        case CoordAxis::Undefined:
            break;

        case CoordAxis::X:
        case CoordAxis::Xpos: {
            m_specPoI[0] = glm::vec3(poiX, poiY, poiZ);
            break;
        }
        case CoordAxis::Xneg: {
            m_specPoI[1] = glm::vec3(poiX, poiY, poiZ);
            break;
        }
        case CoordAxis::Y:
        case CoordAxis::Ypos: {
            m_specPoI[2] = glm::vec3(poiX, poiY, poiZ);
            break;
        }
        case CoordAxis::Yneg: {
            m_specPoI[3] = glm::vec3(poiX, poiY, poiZ);
            break;
        }
        case CoordAxis::Z:
        case CoordAxis::Zpos: {
            m_specPoI[4] = glm::vec3(poiX, poiY, poiZ);
            break;
        }
        case CoordAxis::Zneg: {
            m_specPoI[5] = glm::vec3(poiX, poiY, poiZ);
            break;
        }
    }
}

void Camera::DefineSpecialPos(const CoordAxis axis, double* poi)
{
    if (poi != nullptr) {
        DefineSpecialPos(axis, poi[0], poi[1], poi[2]);
    }
}

void Camera::GetSpecialPos(const CoordAxis axis, double& poiX, double& poiY, double& poiZ)
{
    int idx = -1;
    switch (axis) {
        case CoordAxis::Undefined:
            break;
        case CoordAxis::X:
        case CoordAxis::Xpos: {
            idx = 0;
            break;
        }
        case CoordAxis::Xneg: {
            idx = 1;
            break;
        }
        case CoordAxis::Y:
        case CoordAxis::Ypos: {
            idx = 2;
            break;
        }
        case CoordAxis::Yneg: {
            idx = 3;
            break;
        }
        case CoordAxis::Z:
        case CoordAxis::Zpos: {
            idx = 4;
            break;
        }
        case CoordAxis::Zneg: {
            idx = 5;
            break;
        }
    }

    if (idx >= 0 && idx < 6) {
        poiX = m_specPoI[idx].x;
        poiY = m_specPoI[idx].y;
        poiZ = m_specPoI[idx].z;
    }
}

void Camera::GetSpecialPos(const CoordAxis axis, double* poi)
{
    if (poi != nullptr) {
        GetSpecialPos(axis, poi[0], poi[1], poi[2]);
    }
}

void Camera::DefineSpecialPosF(const CoordAxis axis, float poiX, float poiY, float poiZ)
{
    double px = static_cast<double>(poiX);
    double py = static_cast<double>(poiY);
    double pz = static_cast<double>(poiZ);
    DefineSpecialPos(axis, px, py, pz);
}

void Camera::DefineSpecialPosF(const CoordAxis axis, float* poi)
{
    if (poi != nullptr) {
        DefineSpecialPosF(axis, poi[0], poi[1], poi[2]);
    }
}

void Camera::GetSpecialPosF(const CoordAxis axis, float& poiX, float& poiY, float& poiZ)
{
    double px = 0.0, py = 0.0, pz = 0.0;
    GetSpecialPos(axis, px, py, pz);
    poiX = static_cast<float>(px);
    poiY = static_cast<float>(py);
    poiZ = static_cast<float>(pz);
}

void Camera::GetSpecialPosF(const CoordAxis axis, float* poi)
{
    if (poi != nullptr) {
        GetSpecialPosF(axis, poi[0], poi[1], poi[2]);
    }
}

void Camera::SetSpecialPosKeepPoI(bool keep)
{
    m_keepCurrPoIForSpec = keep;
}

bool Camera::GetSpecialPosKeepPoI()
{
    return m_keepCurrPoIForSpec;
}

void Camera::SetPosRFrame(
    double posX, double posY, double posZ, double dirX, double dirY, double dirZ, double upX, double upY, double upZ)
{
    camPos = glm::dvec3(posX, posY, posZ);
    camRFdir = glm::normalize(glm::dvec3(dirX, dirY, dirZ));
    camRFup = glm::dvec3(upX, upY, upZ);

    camRFright = glm::normalize(glm::cross(camRFdir, camRFup));
    camRFup = glm::cross(camRFright, camRFdir);

    calcClippingNormals();
    UpdateMatrices();
}

void Camera::SetRFrame(double dirX, double dirY, double dirZ, double upX, double upY, double upZ)
{
    double posX = camPos.x;
    double posY = camPos.y;
    double posZ = camPos.z;
    SetPosRFrame(posX, posY, posZ, dirX, dirY, dirZ, upX, upY, upZ);
}

void Camera::SetRFrame(double* dir, double* up)
{
    if (dir == nullptr || up == nullptr) {
        return;
    }
    SetRFrame(dir[0], dir[1], dir[2], up[0], up[1], up[2]);
}

void Camera::SetPosRFrame(double* pos, double* dir, double* up)
{
    if (pos == nullptr || dir == nullptr || up == nullptr) {
        return;
    }
    SetPosRFrame(pos[0], pos[1], pos[2], dir[0], dir[1], dir[2], up[0], up[1], up[2]);
}

void Camera::SetPosRFrame(double* p)
{
    if (p == nullptr) {
        return;
    }

    SetPosRFrame(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);
}

void Camera::SetUpVec(double ux, double uy, double uz)
{
    camRFup = glm::normalize(glm::vec3(ux, uy, uz));
    camRFright = glm::normalize(glm::cross(camRFdir, camRFup));
    camRFdir = glm::cross(camRFup, camRFright);

    calcClippingNormals();
    UpdateMatrices();
}

void Camera::SetUpVec(double* up)
{
    if (up != nullptr) {
        this->SetUpVec(up[0], up[1], up[2]);
    }
}

void Camera::SetViewAngles(double ksi, double chi)
{
    camViewAngleKsi = ksi;
    camViewAngleChi = chi;

    double rksi = glm::radians(camViewAngleKsi);
    double rchi = glm::radians(camViewAngleChi);

    double x = cos(rchi) * cos(rksi);
    double y = cos(rchi) * sin(rksi);
    double z = sin(rchi);

    camRFdir = glm::dvec3(x, y, z);
    glm::dvec3 lz;

    switch (cam_base_axes) {
        default:
        case BaseAxes::XYZ: {
            lz = glm::dvec3(0, 0, 1);
            break;
        }
        case BaseAxes::ZXY: {
            lz = glm::dvec3(0, 1, 0);
            break;
        }
        case BaseAxes::YZX: {
            lz = glm::dvec3(1, 0, 0);
            break;
        }
    }

    camRFright = glm::cross(camRFdir, lz);
    camRFup = glm::cross(camRFright, camRFdir);

    camRFright = glm::normalize(camRFright);
    camRFup = glm::normalize(camRFup);
    UpdateMatrices();
}

void Camera::SetViewAngleKsi(double ksi)
{
    SetViewAngles(ksi, camViewAngleChi);
}

void Camera::SetViewAngleChi(double chi)
{
    chi = glm::clamp(chi, -89.999, 89.999);
    SetViewAngles(camViewAngleKsi, chi);
}

void Camera::SetViewDir(double dx, double dy, double dz)
{
    camRFdir = glm::normalize(glm::dvec3(dx, dy, dz));

    glm::dvec3 z = glm::dvec3(0, 0, 1);
    if (glm::length(glm::cross(camRFdir, z)) < 1e-6) {
        z = glm::dvec3(glm::sign(glm::dot(camRFdir, z)), 0, 0);
        // z = glm::dvec3(-1, 0, 0);
    }
    camRFright = glm::normalize(glm::cross(camRFdir, z));
    camRFup = glm::cross(camRFright, camRFdir);

    m_quat.Set(glm::value_ptr(camRFright), glm::value_ptr(camRFup), glm::value_ptr(camRFdir));

    calcClippingNormals();
    UpdateMatrices();
}

void Camera::SetViewDirF(float dx, float dy, float dz)
{
    this->SetViewDir(static_cast<double>(dx), static_cast<double>(dy), static_cast<double>(dz));
}

void Camera::SetViewDir(double* dir)
{
    if (dir != nullptr) {
        this->SetViewDir(dir[0], dir[1], dir[2]);
    }
}

void Camera::SetViewDirF(float* dir)
{
    if (dir != nullptr) {
        this->SetViewDirF(dir[0], dir[1], dir[2]);
    }
}

// -------------- static methods ---------------

void Camera::CoordsToPixel(double left, double right, double bottom, double top, int wWidth, int wHeight, double x,
    double y, int& pixx, int& pixy)
{
    double rx = (x - left) / (right - left);
    double ry = (y - bottom) / (top - bottom);

    pixx = static_cast<int>(rx * wWidth);
    pixy = wHeight - static_cast<int>(ry * wHeight);
}

// glm::ivec2 Camera::CoordsToPixel(glm::dvec4 boundary, glm::ivec2 wSize, glm::dvec2 c)
//{
//    int pixx, pixy;
//    Camera::CoordsToPixel(
//        boundary[0], boundary[1], boundary[2], boundary[3], wSize[0], wSize[1], c[0], c[1], pixx, pixy);
//    return glm::ivec2(pixx, pixy);
//}

void Camera::PixelToCoords(double left, double right, double bottom, double top, int wWidth, int wHeight, int pixx,
    int pixy, double& x, double& y)
{
    double rx = pixx / static_cast<double>(wWidth);
    double ry = (wHeight - pixy) / static_cast<double>(wHeight);

    x = left + rx * (right - left);
    y = bottom + ry * (top - bottom);
}

// glm::vec2 Camera::PixelToCoords(glm::dvec4 boundary, glm::ivec2 wSize, glm::ivec2 pix)
//{
//    double x, y;
//    Camera::PixelToCoords(boundary[0], boundary[1], boundary[2], boundary[3], wSize[0], wSize[1], pix[0], pix[1], x,
//    y); return glm::vec2(x, y);
//}

Camera::CMView Camera::GetCurrentViewByInt(int view)
{
    if (view < 0 || view >= static_cast<int>(Camera::CMView::NoCubemap)) {
        return Camera::CMView::NoCubemap;
    }
    return static_cast<Camera::CMView>(view);
}


void Camera::setStandardCamera()
{
    camProjection = Projection::PERSPECTIVE;
    
    camRes = glm::ivec2(720, 576);
    camAspect = camRes.x / static_cast<double>(camRes.y);

    camFoVv = 40.0; 
    camZnear = 0.1;
    camZfar = 100.0;

    camPos = glm::dvec3(0.0, 0.0, 5.0);
    camPoI = glm::dvec3(0.0, 0.0, 0.0);

    camRFdir = glm::normalize(glm::dvec3(-1.0, 0.0, 0.0));
    camRFup = glm::dvec3(0.0, 0.0, 1.0);
    camRFright = glm::normalize(glm::cross(camRFdir, camRFup));
    camRFup = glm::cross(camRFright, camRFdir);

    camOrthoView = glm::dvec4(-camAspect, camAspect, -1.0, 1.0);
    cam_pix_offset = glm::dvec2(0.0, 0.0);

    camViewAngleChi = 0.0;
    camViewAngleKsi = 0.0;

    SetBaseAxes();
    m_quat = Quaternion<double>(0, 1, 0, 0);

    camAlpha = calcAlpha();
    calcClippingNormals();
    calcHorizontalFieldOfView();
    UpdateMatrices();
}


double Camera::calcAlpha()
{
    return atan(sqrt(camAspect * camAspect + 1.0) * tan(glm::radians(camFoVv) * 0.5));
}

void Camera::calcClippingNormals()
{
    double phi = glm::radians(camFoVv) * 0.5;
    double psi = atan(camAspect * tan(phi));

    camNup = -sin(phi) * camRFdir + cos(phi) * camRFup;
    camNbt = -sin(phi) * camRFdir - cos(phi) * camRFup;

    camNleft = -sin(psi) * camRFdir - cos(psi) * camRFright;
    camNright = -sin(psi) * camRFdir + cos(psi) * camRFright;

    camNup = glm::normalize(camNup);
    camNbt = glm::normalize(camNbt);
    camNleft = glm::normalize(camNleft);
    camNright = glm::normalize(camNright);
    // should all be normalized
}

void Camera::calcCurrCMClippingNormals()
{
    double phi = glm::radians(camFoVv) * 0.5;
    double psi = atan(camAspect * tan(phi));

    cm_curr_camNup = -sin(phi) * cm_curr_dir + cos(phi) * cm_curr_up;
    cm_curr_camNbt = -sin(phi) * cm_curr_dir - cos(phi) * cm_curr_up;

    cm_curr_camNleft = -sin(psi) * cm_curr_dir - cos(psi) * cm_curr_right;
    cm_curr_camNright = -sin(psi) * cm_curr_dir + cos(psi) * cm_curr_right;

    cm_curr_camNup = glm::normalize(cm_curr_camNup);
    cm_curr_camNbt = glm::normalize(cm_curr_camNbt);
    cm_curr_camNleft = glm::normalize(cm_curr_camNleft);
    cm_curr_camNright = glm::normalize(cm_curr_camNright);
}

void Camera::calcHorizontalFieldOfView()
{
    camFoVh = glm::degrees(
        static_cast<double>(2.0 * atan(camRes[0] / static_cast<double>(camRes[1]) * tan(0.5 * glm::radians(camFoVv)))));
}

void Camera::getLRFrame(CoordAxis ca, glm::dvec3& r, glm::dvec3& d, glm::dvec3& u)
{
    switch (ca) {
        case CoordAxis::X:
        case CoordAxis::Xpos: {
            r = -camRFdir;
            d = camRFright;
            u = camRFup;
            break;
        }
        case CoordAxis::Xneg: {
            r = camRFdir;
            d = -camRFright;
            u = camRFup;
            break;
        }
        case CoordAxis::Y:
        case CoordAxis::Ypos: {
            r = camRFright;
            d = -camRFup;
            u = camRFdir;
            break;
        }
        case CoordAxis::Yneg: {
            r = camRFright;
            d = camRFup;
            u = -camRFdir;
            break;
        }
        case CoordAxis::Z:
        case CoordAxis::Zpos: {
            r = camRFright;
            d = camRFdir;
            u = camRFup;
            break;
        }
        case CoordAxis::Zneg: {
            r = -camRFright;
            d = -camRFdir;
            u = camRFup;
            break;
        }
        case CoordAxis::Undefined:
            break;
    }
}

void Camera::setViewDir(glm::dvec3 dir)
{
    SetViewDir(dir.x, dir.y, dir.z);
}

void Camera::SetType(Type type)
{
    camType = type;
}

Camera::Type Camera::GetType()
{
    return camType;
}

bool Camera::GetPositionFromViewMatrix(const float* mat, float& x, float& y, float& z)
{
    if (mat == nullptr) {
        return false;
    }
    glm::mat4 viewMX = glm::make_mat4(mat);

    glm::mat4 iVMX = glm::inverse(viewMX);    
    x = iVMX[3][0];
    y = iVMX[3][1];
    z = iVMX[3][2];

    return true;
}

bool Camera::GetPositionFromViewMatrix(const float* mat, float* xyz)
{
    if (mat == nullptr || xyz == nullptr) {
        return false;
    }

    return GetPositionFromViewMatrix(mat, xyz[0], xyz[1], xyz[2]);
}
