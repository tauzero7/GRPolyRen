/**
 * File:    AnimOrbitCam.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_ANIM_ORBIT_CAM_H
#define GRPR_ANIM_ORBIT_CAM_H

#include "AnimParam.h"
#include "Camera.h"

class AnimOrbitCam
{
public:
    AnimOrbitCam();
    ~AnimOrbitCam();

    void AddVel(double vtheta, double vphi, double vdist);
    void AddThetaVel(double val);
    void AddPhiVel(double val);
    void AddDistVel(double val);
    void AddPanVel(double vx, double vy);

    void GetVel(double& vtheta, double& vphi, double& vdist);
    double GetThetaVel();
    double GetPhiVel();
    double GetDistVel();
    void GetPanVel(double& vx, double& vy);

    bool Idle(double dt);
    bool Idle(Camera* cam, double dt);

    void Play(bool play);
    void PlayTheta(bool play);
    void PlayPhi(bool play);
    void PlayDist(bool play);
    void PlayPan(bool play);

    void SetFactor(double ftheta, double fphi, double fdist);
    void SetThetaFactor(double factor);
    void SetPhiFactor(double factor);
    void SetDistFactor(double factor);
    void SetPanFactor(double factor);

    void SetLocalZ(bool use_local_z);

private:
    AnimParam animTheta;
    AnimParam animPhi;
    AnimParam animDist;
    AnimParam animPan[2];
    bool m_useLocalZ;
};

#endif // GRPR_ANIM_ORBIT_CAM_H
