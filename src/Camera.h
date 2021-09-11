/**
 * File:    main.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_CAMERA_H
#define GRPR_CAMERA_H

#include <glm/glm.hpp>
#include <string>

#include "Quaternion.h"

enum class CoordAxis : std::uint8_t {
    Undefined = 0,
    Xpos, //!< Positive x-axis
    Xneg, //!< Negative x-axis
    Ypos, //!< Positive y-axis
    Yneg, //!< Negative y-axis
    Zpos, //!< Positive z-axis
    Zneg, //!< Negative z-axis
    X, //!< X-axis
    Y, //!< Y-axis
    Z //!< Z-axis
};

enum class CoordPlane {
    XY = 0, //!< XY coordinate plane
    XZ, //!< XZ coordinate plane
    YZ //!< YZ coordinate plane
};


enum class BaseAxes : std::uint8_t { Undefined = 0, XYZ, ZXY, YZX };

static const std::uint8_t NumBaseAxes = 4;


class Camera
{
public:
    //! Projection types
    enum class Projection {
        ORTHOGRAPHIC = 0, //!<  Orthographic view
        PERSPECTIVE, //!<  Perspective view with vertical field-of-view
        UNKNOWN
    };

    //! Moving types.
    enum class Type {
        ORBIT_SPHERICAL = 0, //!< orbit around point of interest on a sphere
        ORBIT_QUATERNION, //!< orbit around point of interest using quaternion
        FREE_FLIGHT, //!< free-flight motion
        DOME,
        UNKNOWN
    };

    enum class Action : int {
        NOTHING = 0, //!< No action selected
        ORBIT = 1 << 0, //!< Orbit around point of interest
        ORBIT_Z = 1 << 1, //!< Orbit around point of interest with local z-axis
        DOLLY = 1 << 2, //!< Dolly
        MOVE_X = 1 << 3, //!< Move along x axis
        MOVE_Y = 1 << 4, //!< Move along y axis
        MOVE_Z = 1 << 5, //!< Move along z axis
        MOVE_XY = 1 << 6, //!< Move in the xy plane
        MOVE_XZ = 1 << 7, //!< Move in the xz plane
        MOVE_YZ = 1 << 8, //!< Move in the yz plane
        PAN = 1 << 9, //!< Paning
        PITCH = 1 << 10, //!< Pitch
        YAW = 1 << 11, //!< Yaw
        ROLL = 1 << 12, //!< Roll
        FLY = 1 << 13, //!< Fly
        FLY_SCALED = 1 << 14, //!< Fly distance scaled
        ALL = ((1 << 15) - 1) //!< Sum of all actions
    };

    enum class CMView : int { PosX = 0, NegX, PosY, NegY, PosZ, NegZ, NoCubemap };

    static const std::uint8_t NumProjections;
    static const char* const ProjectionNames[];

    static const std::uint8_t NumTypes;
    static const char* const TypeNames[];

protected:   

    Projection GetProjByName(const char* name);
    const char* GetProjName(Projection proj);

    Type GetTypeByName(const char* name);
    const char* GetTypeName(Type type);

public:
    /// Standard constructor initializes standard parameters.
    Camera();

    /// Standard copy constructor
    Camera(const Camera& other);

    /**
     * @brief Copy constructor with specific coordinate axes
     * @param other
     * @param ca
     */
    Camera(const Camera& other, CoordAxis ca);

    /// Destructor.
    virtual ~Camera();

    /**
     * @brief Disable allowed action.
     *   Will be used in UpdateMouse().
     * @param act
     */
    void DisableAllowedAction(Action act);

    /**
     * @brief Dolly.
     * @param step
     */
    void Dolly(double step);

    /**
     * @brief Enable allowed action.
     * @param act
     */
    void EnableAllowedAction(Action act);
    void EnableAllActions();

    /**
     * @brief Find intersection of light ray with plane.
     * @param px        Pixel x-coordinate.
     * @param py        Pixel y-coordinate.
     * @param cplane    Coordinate plane of interest
     * @param ipx       x-coordinate of intersection point.
     * @param ipy       y-coordinate of intersection point.
     * @param ipz       z-coordinate of intersection point.
     * @param upsideDown
     */
    bool FindIntersec(
        int px, int py, CoordPlane cplane, double& ipx, double& ipy, double& ipz, bool upsideDown = false);

    /**
     * @brief Move a step forward in the direction of view.
     *  Only position will be modified. UpdateMatrices() has to be called afterwards.
     * @param step
     * @param distScaled
     */
    void Fly(double step, bool distScaled = false);

    /**
     * @brief Get opening angle of viewing cone defined by the angle to
     *   one of the frustum corners.
     * @return opening angle in degree.
     */
    double GetAlpha();
    float GetAlphaF();

    /**
     * @brief Get apparent size of a sphere in pixels (pinhole camera).
     *
     * @param radius  Radius of the sphere located at the point-of-interest.
     * @return -1 if radius is bigger than the distance to the poi.
     */
    int GetApparentSphereSize(const double radius);
    int GetApparentSphereSize(const double radius, const double dist);

    /**
     * @brief Get aspect ratio of the camera (horizontal / vertical resolution).
     */
    double GetAspectRatio();
    float GetAspectRatioF();

    /**
     * @brief Get clipping plane values.
     * @param zNear   Near clipping plane z-value
     * @param zFar    Far clipping plane z-value
     */
    void GetClipPlanes(double& zNear, double& zFar);
    void GetClipPlanesF(float& zNear, float& zFar);

    /**
     * @brief Get distance between camera position and point-of-interest
     * @return
     */
    double GetDistance();
    float GetDistanceF();

    /**
     * @brief Get horizontal field of view.
     * @return Horizontal field of view in degrees.
     */
    double GetFoVh();

    /**
     * @brief Get vertical field of view.
     * @return Vertical field of view in degrees.
     */
    double GetFoVv();

    /**
     * @brief Get gobal view dir from pixel coordinates.
     *   Depending on the camera's current reference frame, the pixel (px,py)
     *   represents a direction in world coordinates (dx,dy,dz).
     *   The direction is normalized.
     * @param px    Pixel x-coordinate.
     * @param py    Pixel y-coordinate.
     * @param dx    x-component of global view dir.
     * @param dy    y-component of global view dir.
     * @param dz    z-component of global view dir.
     * @param upsideDown
     */
    void GetGlobalViewDir(int px, int py, double& dx, double& dy, double& dz, bool upsideDown = false);

    /**
     * @brief Get the camera's vertical resolution.
     *   Get horizontal and vertical resolution with GetResolution().
     * @return Vertical resolution in pixels.
     */
    int GetHeight();

    /**
     * @brief Get local normalized view dir from pixel coordinates.
     *   The pixel (px,py) represents a direction in camera coordinates (kd, kr, ku).
     *   The direction is normalized.
     * @param px    Pixel x-coordinate.
     * @param py    Pixel y-coordinate.
     * @param kd    dir-component of local view dir.
     * @param kr    right-component of local view dir.
     * @param ku    up-component of local view dir.
     * @param upsideDown
     */
    void GetLocalViewDir(int px, int py, double& kd, double& kr, double& ku, bool upsideDown = false);


    /** Determine pixel coordinates (pinhole camera) from global direction.
     *   Depending on the camera's current reference frame, the direction (dx,dy,dz)
     *   in world coordinates maps to the pixel coordinate (px,py).
     * @param dx   x-component of global direction
     * @param dy   y-component of global direction
     * @param dz   z-component of global direction
     * @param px   pixel coordinate
     * @param py   pixel coordinate
     * @param upsideDown
     */
    bool GetPixelFromGlobalDir(
        const double dx, const double dy, const double dz, int& px, int& py, bool upsideDown = false);

    /**
     * @brief Calculate pixel coordinate of a point located at (x,y,z) in
     *  world coordinates.
     *
     * @param x    x-coordinate of point in world coordinates.
     * @param y    y-coordinate of point in world coordinates.
     * @param z    z-coordinate of point in world coordinates.
     * @param px   Pixel x-coordinate.
     * @param py   Pixel y-coordinate.
     * @param upsideDown
     * @return true if point is in the view frustum
     */
    bool GetPixelFromGlobalPos(double x, double y, double z, int& px, int& py, bool upsideDown = false);
    bool GetPixelFromGlobalPos(double* pos, int& px, int& py, bool upsideDown = false);

    /**
     * @brief Get pixel resolution.
     *   Depending on the camera resolution and field-of-view, this methods returns
     *   the pixel resolution for the central pixel in case of a pinhole camera.
     *
     * @return Pixel resolution in radians.
     */
    double GetPixelResolution();

    /**
     * @brief Get point of interest.
     * @param x
     * @param y
     * @param z
     */
    void GetPoI(double& x, double& y, double& z);
    void GetPoI(double* xyz);

    void GetPoIF(float& x, float& y, float& z);
    void GetPoIF(float* xyz);

    /**
     * @brief Get camera position.
     * @param px   reference to x-coordinate.
     * @param py   reference to y-coordinate.
     * @param pz   reference to z-coordinate.
     */
    void GetPosition(double& px, double& py, double& pz);
    void GetPositionF(float& px, float& py, float& pz);

    /**
     * @brief Get camera position.
     * @param xyz   Pointer to valid 3-array.
     */
    void GetPosition(double* xyz);
    void GetPositionF(float* xyz);

    /**
     * @brief GetPositionPtr
     * @return pointer to camera position (3-vector)
     */
    double* GetPositionPtr();

    /**
     * @brief Get relative position wrt point-of-interest
     * @param rx   posX - poiX
     * @param ry   posY - poiY
     * @param rz   posZ - poiZ
     */
    void GetRelativePosition(double& rx, double& ry, double& rz);
    void GetRelativePositionF(float& rx, float& ry, float& rz);

    /**
     * @brief Get relative position wrt point-of-interest in spherical coordinates.
     * @param r         Distance to point-of-interest.
     * @param theta     Colatitude angle (radians).
     * @param phi       Azimuth angle (radians).
     */
    void GetRelativeSphericalPosition(double& r, double& theta, double& phi);

    /// Get Projection type.
    Projection GetProjection();
    const char* GetProjectionName();

    /// Get pointer to projection matrix which can be directly used in glUniformMatrix4fv.
    const float* GetProjMatrixPtr() const;

    /**
     * @brief Get camera resolution.
     *   The individual values can be obtained using GetWidth() and GetHeight().
     * @param resH    Horizontal resolution (pixels).
     * @param resV    Vertical resolution (pixels).
     */
    void GetResolution(int& resH, int& resV);

    /**
     * @brief Get viewing direction vector.
     * @param dx
     * @param dy
     * @param dz
     */
    void GetDirVec(double& dx, double& dy, double& dz);
    void GetDirVecF(float& dx, float& dy, float& dz);
    void GetDirVecF(float* dxyz);

    /**
     * @brief Get right direction vector.
     * @param rx
     * @param ry
     * @param rz
     */
    void GetRightVec(double& rx, double& ry, double& rz);
    void GetRightVecF(float& rx, float& ry, float& rz);
    void GetRightVecF(float* xyz);

    /**
     * @brief Get up vector.
     *
     * @param ux
     * @param uy
     * @param uz
     */
    void GetUpVec(double& ux, double& uy, double& uz);
    void GetUpVecF(float& ux, float& uy, float& uz);
    void GetUpVecF(float* xyz);

    /**
     * @brief Get pointer to view matrix which can be directly used in glUniformMatrix4fv.
     */
    virtual const float* GetViewMatrixPtr() const;

    /**
     * @brief Get pointer to inverse view matrix which can be directly used in glUniformMatrix4fv.
     * @return
     */
    virtual const float* GetInvViewMatrixPtr() const;

    /**
     * @brief Get the camera's horizontal resolution.
     *   Get horizontal and vertical resolution with GetResolution().
     * @return Horizontal resolution in pixels.
     */
    int GetWidth();

    /**
     * @brief Test if point is inside the camera's view frustum.
     *  Near and far clipping planes are not considered.
     * @param posX   x-coordinate of point.
     * @param posY   y-coordinate of point.
     * @param posZ   z-coordinate of point.
     */
    bool IsPointInClippingVolume(double posX, double posY, double posZ);

    /**
     * @brief Test if sphere is in the camera's view frustum.
     *  Near and far clipping planes are not considered.
     * @param cx     x-coordinate of sphere's center.
     * @param cy     y-coordinate of sphere's center.
     * @param cz     z-coordinate of sphere's center.
     * @param radius  radius of sphere.
     */
    bool IsSphereVisible(double cx, double cy, double cz, double radius);
    bool IsSphereVisibleF(float cx, float cy, float cz, float radius);

    /**
     * @brief Test if sphere is in the camera's view frustum.
     *  Near and far clipping planes are not considered.
     * @param c        pointer to coordinates of sphere's center.
     * @param radius   radius of sphere.
     */
    bool IsSphereVisible(double* c, double radius);
    bool IsSphereVisibleF(float* c, float radius);

    /**
     * @brief Move camera along coordinate plane.
     *   Only position will be modified. UpdateMatrices() has to be called afterwards.
     * @param plane
     * @param dx
     * @param dy
     */
    void Move(CoordPlane plane, double dx, double dy);

    /**
     * @brief Move position and point-of-interest along coordinate axis.
     *   UpdateMatrices() has to be called afterwards.
     * @param axis
     * @param d
     */
    void MoveAxis(CoordAxis axis, double d);

    /**
     * @brief Move position and point-of-interest along coordinate plane.
     *   UpdateMatrices() has to be called afterwards.
     * @param plane
     * @param dx
     * @param dy
     */
    void MovePlane(CoordPlane plane, double dx, double dy);

    /**
     * @brief Move point-of-interest to origin.
     *   The position relative to the point-of-interest is fixed.
     */
    void MovePOItoOrigin();

    /**
     * @brief Orbit around point-of-interest on a spherical orbit.
     *   The third axis defines the up-vector (see SetBaseAxes()).
     *   The delta-azimuth and -latitude angles will be scaled by the
     *   camera rotation scaling factor.
     *   This method only affects orbital camera.
     *   UpdateMatrices() will be called automatically.
     * @param dazi   Delta-azimuth in radians.
     * @param dlat   Delta-latitude in radians.
     * @param use_local_z  (only used for quaternion camera)
     */
    void Orbit(double dazi, double dlat, bool use_local_z = false);

    /** Orbit around point-of-interest on a spherical orbit.
     *   UpdateMatrices() will be called automatically.
     * @param dazi   Azimuth angle in degree
     * @param dlat   Colatitude angle in degree
     * @param use_local_z  (only used for quaternion camera)
     */
    void OrbitByAngle(double dazi, double dlat, bool use_local_z = false);

    /**
     * @brief Set base axes.
     *   In case of the orbit spherical camera, the orbital motion is defined
     *   by the orientation of the base axes (default: XYZ). In order to orbit
     *   around a different axis, define the base axes accordingly.
     *   UpdateMatrices() has to be called afterwards.
     * @param axes
     */
    void SetBaseAxes(BaseAxes axes = BaseAxes::XYZ);

    /**
     * @brief Pan along the right and up direction of the camera.
     *   Position and point-of-interest will be modified.
     *   UpdateMatrices() has to be called afterwards.
     * @param h  Horizontal panning step.
     * @param v  Vertical panning step.
     */
    void Pan(const double h, const double v);

    /**
     * @brief Pan along the right and up direction of the camera.
     *   The panning steps are modified by the camera resolution,
     *   field-of-view, and moving scale factor.
     *   Position and point-of-interest will be modified.
     *   UpdateMatrices() has to be called afterwards.
     * @param px
     * @param py
     */
    void Pan(const int px, const int py);

    /**
     * @brief Pan camera position and point-of-interest along coordinates.
     *   UpdateMatrices() is called inside.
     * @param dx
     * @param dy
     * @param dz
     */
    void Pan(const double dx, const double dy, const double dz);

    /**
     * @brief Apply pitch rotation (rotate around the right-vector).
     *   A positive pitch angle moves the nose of the aircraft upward.
     * @param angle  Pitch angle in degree.
     */
    void Pitch(double angle);

    void Reset();

    /**
     * @brief Apply roll rotation (rotate around viewing direction).
     *  A positive roll angle lifts the left wing and lowers the right wing.
     * @param angle Roll angle in degree.
     */
    void Roll(double angle);

    /**
     * @brief Set allowed actions for motion.
     *   Will be used in UpdateMouse().
     * @param act
     */
    void SetAllowedActions(int act);

    /**
     * @brief Add action to allowed actions.
     *   Will be used in UpdateMouse().
     * @param act
     */
    void SetAllowedAction(Action act);

    /**
     * @brief Set near and far clipping planes.
     *  UpdateMatrices() will be called automatically.
     * @param zNear   z-value of near clipping plane.
     * @param zFar    z-value of far clipping plane
     */
    void SetClipPlanes(double zNear, double zFar);

    /**
     * @brief SetCurrentView
     * @param view (standard: CMView::NoCubemap)
     */
    void SetCurrentView(CMView view = CMView::NoCubemap);

    /**
     * @brief Set camera distance to point-of-interest.
     *  Only position will be changed.
     *  UpdateMatrices() will be called automatically.
     * @param dist
     */
    void SetDistance(double dist);

    /**
     * @brief Set vertical field of view.
     *  All internal parameters will be updated including matrices.
     * @param fovY  Field of view in degree.
     */
    virtual void SetFoVy(double fovY);


    /**
     * @brief Set point-of-interest.
     *  The view dir of the camera will be adapted according to the camera's position
     *  and the point-of-interest.
     *  UpdateMatrices() will be called automatically.
     * @param px   x-coordinate of point-of-interest
     * @param py   y-coordinate of point-of-interest
     * @param pz   z-coordinate of point-of-interest
     */
    void SetPoI(double px, double py, double pz);
    void SetPoIF(float px, float py, float pz);

    /**
     * @brief Set point-of-interest to coordinate origin.
     *  UpdateMatrices() will be called automatically.
     */
    void SetPoI2Origin();

    /**
     * @brief Set point-of-interest.
     *    The view dir of the camera will be adapted according to the camera's position
     *    and the point-of-interest.
     *
     * @param poi   Pointer to coordinates of point-of-interest (must be non-null array of size 3
     *              or larger!)
     */
    void SetPoI(double* poi);
    void SetPoIF(float* poi);

    /**
     * @brief Set camera position.
     *  If the camera is of 'orbital' type, then the viewing direction will be
     *  set to the point-of-interest and UpdateMatrices() will be called automatically.
     *  Otherwise, only the position will be changed and UpdateMatrices() has to be called afterwards.
     *
     * @param px  x-coordinate of camera position.
     * @param py  y-coordinate of camera position.
     * @param pz  z-coordinate of camera position.
     */
    void SetPosition(double px, double py, double pz);
    void SetPositionF(float px, float py, float pz);

    /**
     * @brief Set camera position.
     * @param pos
     */
    void SetPosition(double* pos);
    void SetPositionF(float* pos);

    /**
     * @brief Set position and reference frame.
     *  The up-vector is only a hint and will be adjusted such that the
     *  reference frame is orthonormal.
     *  UpdateMatrices() will be called automatically.
     *
     * @param posX
     * @param posY
     * @param posZ
     * @param dirX
     * @param dirY
     * @param dirZ
     * @param upX
     * @param upY
     * @param upZ
     */
    void SetPosRFrame(double posX, double posY, double posZ, double dirX, double dirY, double dirZ, double upX,
        double upY, double upZ);
    void SetPosRFrame(double* pos, double* dir, double* up);
    void SetPosRFrame(double* p);

    void SetRFrame(double dirX, double dirY, double dirZ, double upX, double upY, double upZ);
    void SetRFrame(double* dir, double* up);

    /**
     * @brief Set projection type.
     *  UpdateMatrices() will be called automatically.
     * @param proj
     */
    void SetProjection(Projection proj);
    void SetProjection(const char* projName);

    /**
     * @brief Set projection matrix.
     * @param matrix   Pointer to projection matrix.
     * @param is_glm   If true, then pointer is column-major-order as glm::mat4.
     */
    void SetProjectionMatrix(const float* matrix, bool is_glm = true);

    /**
     * @brief Set quadratic camera resolution
     * @param res  Resolution in pixel.
     */
    void SetResolution(const int res);

    /**
     * @brief Set resolution in pixel.
     * @param resH  Horizontal resolution.
     * @param resV  Vertical resolution.
     */
    void SetResolution(const int resH, const int resV);

    /** Set scale factors for mouse update
     * @param moveFactor
     * @param rotFactor
     */
    void SetScaleFactors(double moveFactor, double rotFactor);

    /**
     * @brief Set fly scale factor.
     * @param flyFactor
     */
    void SetFlyScaleFactor(double flyFactor);

    /**
     * @brief Set move scale factor.
     * @param moveFactor
     */
    void SetMoveScaleFactor(double moveFactor);


    void SetRollScaleFactor(double rollFactor);

    /// Get mouse scale factor for fly opertations.
    double GetFlyScaleFactor();

    /// Get mouse scale factor for move operations.
    double GetMoveScaleFactor();

    /// Get mouse scale factor for rotation operations.
    double GetRotScaleFactor();

    /// Get mouse scale factor for roll operations.
    double GetRollScaleFactor();

    /**
     * @brief Set special position of the camera.
     *    If 'axis' is set to 'CoordAxis::Xpos', the camera will be put onto the x-axis with the
     *    current distance between eye point and point-of-interest and looking into the negative
     *    x-direction.
     *
     * @param axis   Enumeration which defines the camera's position.
     */
    void SetSpecialPos(const CoordAxis axis);

    void DefineSpecialPos(const CoordAxis axis, double poiX, double poiY, double poiZ);
    void DefineSpecialPos(const CoordAxis axis, double* poi);
    void GetSpecialPos(const CoordAxis axis, double& poiX, double& poiY, double& poiZ);
    void GetSpecialPos(const CoordAxis axis, double* poi);

    void DefineSpecialPosF(const CoordAxis axis, float poiX, float poiY, float poiZ);
    void DefineSpecialPosF(const CoordAxis axis, float* poi);
    void GetSpecialPosF(const CoordAxis axis, float& poiX, float& poiY, float& poiZ);
    void GetSpecialPosF(const CoordAxis axis, float* poi);

    void SetSpecialPosKeepPoI(bool keep);
    bool GetSpecialPosKeepPoI();

    /**
     * @brief Set vertical up-vector.
     *  The right and dir vectors will be set such that the reference frame is orthonormal.
     *  UpdateMatrices() will be called automatically.
     * @param ux
     * @param uy
     * @param uz
     */
    void SetUpVec(double ux, double uy, double uz);
    void SetUpVec(double* up);

    /**
     * @brief Set view direction using spherical angles.
     *   Up- and right-vector depend on the base axes, see "SetBaseAxes".
     *     dx = cos(chi) * cos(ksi)
     *     dy = cos(chi) * sin(ksi)
     *     dz = sin(chi)
     *  UpdateMatrices() will be called automatically.
     * @param ksi   Azimuth angle in degree.
     * @param chi   Latitude angle in degree.
     */
    void SetViewAngles(double ksi, double chi);

    /**
     * @brief Set view direction using spherical angles.
     *  UpdateMatrices() will be called automatically.
     * @param ksi  Azimuth angle in degree.
     */
    void SetViewAngleKsi(double ksi);

    /**
     * @brief Set view direction using spherical angles.
     *  UpdateMatrices() will be called automatically.
     * @param chi  Latitude angle in degree.
     */
    void SetViewAngleChi(double chi);

    /**
     * @brief Set view direction and update camera reference frame.
     *  UpdateMatrices() will be called automatically.
     * @param dx
     * @param dy
     * @param dz
     */
    void SetViewDir(double dx, double dy, double dz);
    void SetViewDirF(float dx, float dy, float dz);

    /**
     * @brief Set view direction and update camera reference frame.
     *  UpdateMatrices() will be called automatically.
     * @param dir
     */
    void SetViewDir(double* dir);
    void SetViewDirF(float* dir);

    /**
     * @brief Update view and projection matrix.
     *   The projection matrix depends on the projection type (orthographic, perspective).
     */
    void UpdateMatrices();

    /** Update position etc. via mouse control.
     *    According to the allowed actions, the mouse movement will be mapped to a particular camera motion.
     *
     * @param dx     Mouse motion in x-direction (in pixels).
     * @param dy     Mouse motion in y-direction (in pixels).
     * @param act    Allowed action for the camera motion
     */
    bool UpdateMouse(double dx = 0.0, double dy = 0.0, Action act = Action::NOTHING);

    /**
     * @brief Apply 'yaw' rotation.
     *   Note that yaw-axis points downward and a positive yaw angle moves nose
     *   of the aircraft to the right.
     * @param angle:  Yaw angle in degree
     */
    void Yaw(double angle);

    void SetInverseYaw(bool inverse = true);
    bool IsInverseYaw();

    void SetPixelOffset(double px, double py);

    /**
     * @brief Set camera type.
     *  The camera type is only interesting for the orbital motion and for the
     *   general UpdateMouse() method.
     * @param type
     */
    void SetType(Type type);

    /**
     * @brief Get camera type.
     * @return
     */
    Type GetType();

    /// Get size of viewing plane at point-of-interest (distance between camera and poi)
    void GetOrthoSize(double& sx, double& sy);
    void GetOrthoSizeF(float& sx, float& sy);

    /**
     * @brief Set camera orientation via (yaw->pitch->roll).
     *
     *   The standard orientation (r,d,u) is defined as r=x, d=y, u=z.
     * @param yaw    Yaw angle in degree.
     * @param pitch  Pitch angle in degree.
     * @param roll   Roll angle in degree.
     */
    virtual void SetYawPitchRoll(double yaw, double pitch, double roll);

public:
    static void CoordsToPixel(double left, double right, double bottom, double top, int wWidth, int wHeight, double x,
        double y, int& pixx, int& pixy);

    /**
     * @brief Convert pixel coordinate to coordinate in orthographic projection coordinate system.
     *
     *    Origin of pixel coordinates is in the upper left corner.
     *
     * @param left    Left value of orthographic projection.
     * @param right   Right value of orthographic projection.
     * @param bottom  Bottom value of orthographic projection.
     * @param top     Top value of orthographic projection.
     * @param width
     * @param height
     * @param pixx    x-coordinate of pixel.
     * @param pixy    y-coordinate of pixel
     * @param x       x-coordinate of ortho system.
     * @param y       y-coordinate of ortho system.
     */
    static void PixelToCoords(double left, double right, double bottom, double top, int width, int height, int pixx,
        int pixy, double& x, double& y);

    static CMView GetCurrentViewByInt(int view);

    /**
     * @brief Get world position from 4x4 view matrix.
     * @param mat  Pointer to view matrix.
     * @param x
     * @param y
     * @param z
     */
    static bool GetPositionFromViewMatrix(const float* mat, float& x, float& y, float& z);
    static bool GetPositionFromViewMatrix(const float* mat, float* xyz);

    static bool GetViewMatrixFromPosUpDir(double* pos, double* up, double* dir, float*& mat);

protected:
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

    /// Calculate opening angle 'alpha' of the viewing cone.
    double calcAlpha();

    /// Calculate normal vectors of the clipping planes.
    void calcClippingNormals();

    void calcCurrCMClippingNormals();

    /**
     * @brief Calculate horizontal field-of-view from the vertical field-of-view
     *   and the camera's aspect ratio.
     */
    void calcHorizontalFieldOfView();

    void getLRFrame(CoordAxis ca, glm::dvec3& r, glm::dvec3& d, glm::dvec3& u);

    /// Set view dir internally.
    void setViewDir(glm::dvec3 dir);

    /// Return normalized view dir (kd,kr,ku) from pixel (px,py)
    glm::dvec3 getLocalViewDir(int px, int py, bool upsideDown = false);
    glm::dvec3 getGlobalViewDir(int px, int py, bool upsideDown = false);

    void setStandardCamera();
    
    /// Calculate orthographic matrix
    void calc_ortho_matrix(glm::mat4& mat);

    /// Calcualte perspective matrix
    void calc_persp_matrix(glm::mat4& mat);

protected:
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
    glm::mat4 viewMX; //!< View matrix.
    glm::mat4 invViewMX; //!< Inverse view matrix.
    glm::mat4 projMX; //!< Projection matrix.

    glm::mat4 cmViewMX; //!< Cubemap view matrix.
    CMView cmView;
    glm::dvec3 cm_curr_dir;
    glm::dvec3 cm_curr_up;
    glm::dvec3 cm_curr_right;
    glm::dvec3 cm_curr_camNup; //!< Normal vector of upper clipping plane
    glm::dvec3 cm_curr_camNbt; //!< Normal vector of lower clipping plane
    glm::dvec3 cm_curr_camNleft; //!< Normal vector of left clipping plane
    glm::dvec3 cm_curr_camNright; //!< Normal vector of right clipping plane

    Type camType;

    int camAllowedActions;
    Projection camProjection; //!< Camera projection (orthographic, perspective)

    double camActFlyScaleFactor;
    double camActMoveScaleFactor;
    double camActRotScaleFactor;
    double camActRollScaleFactor;

    glm::ivec2 camRes; //!< Camera resolution in horizontal and vertical direction.
    double camAspect; //!< Camera's aspect ratio.
    double camFoVv; //!< Vertical field of view in degree.
    double camFoVh; //!< Horizontal field of view in degree.
    double camZnear; //!< Distance to near clipping plane.
    double camZfar; //!< Distance to far clipping plane.
    
    glm::dvec3 camPos; //!< Camera's position.
    glm::dvec3 camPoI; //!< Camera's point-of-interest.

    glm::dvec3 camRFup; //!< up-vector of camera's reference frame
    glm::dvec3 camRFright; //!< right-vector of camera's reference frame
    glm::dvec3 camRFdir; //!< dir-vector of camera's reference frame
    glm::dvec4 camOrthoView; //!< view domain for orthoproj:  xmin,xmax,ymin,ymax

    glm::dvec3 camNup; //!< Normal vector of upper clipping plane
    glm::dvec3 camNbt; //!< Normal vector of lower clipping plane
    glm::dvec3 camNleft; //!< Normal vector of left clipping plane
    glm::dvec3 camNright; //!< Normal vector of right clipping plane

    double camViewAngleKsi; //!< Viewing angle in degree (define camRF wrt base axes
    double camViewAngleChi; //!< Viewing angle in degree

    double camAlpha; //!< Opening angle of viewing cone

    BaseAxes cam_base_axes;
    glm::vec4 cam_base_rot1;
    glm::vec4 cam_base_rot2;
    glm::mat4 cam_base_rotMX;

    glm::dvec2 cam_pix_offset;

    // Special position definition (Xpos,Xneg,Ypos,Yneg,Zpos,Zneg)
    glm::dvec3 m_specPoI[6];
    bool m_keepCurrPoIForSpec;

    bool m_useInverseYaw;

    Quaternion<double> m_quat;
};

#endif // GRPR_CAMERA_H
