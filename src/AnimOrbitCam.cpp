/**
 * File:    AnimOrbitCam.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "AnimOrbitCam.h"

AnimOrbitCam::AnimOrbitCam()
    : m_useLocalZ(false)
{
    animTheta.SetType(AnimParam::Type::VelControl);
    animPhi.SetType(AnimParam::Type::VelControl);
    animDist.SetType(AnimParam::Type::VelControl);
    animPan[0].SetType(AnimParam::Type::VelControl);
    animPan[1].SetType(AnimParam::Type::VelControl);

    SetThetaFactor(0.1);
    SetPhiFactor(0.1);
    SetDistFactor(0.003);
    SetPanFactor(0.0001);
}

AnimOrbitCam::~AnimOrbitCam()
{
    //
}

void AnimOrbitCam::AddVel(double vtheta, double vphi, double vdist)
{
    animTheta.AddVel(vtheta);
    animPhi.AddVel(vphi);
    animDist.AddVel(vdist);
}

void AnimOrbitCam::AddThetaVel(double val)
{
    animTheta.AddVel(val);
}

void AnimOrbitCam::AddPhiVel(double val)
{
    animPhi.AddVel(val);
}

void AnimOrbitCam::AddDistVel(double val)
{
    animDist.AddVel(val);
}

void AnimOrbitCam::AddPanVel(double vx, double vy)
{
    animPan[0].AddVel(vx);
    animPan[1].AddVel(vy);
}

void AnimOrbitCam::GetVel(double& vtheta, double& vphi, double& vdist)
{
    vtheta = animTheta.GetVel();
    vphi = animPhi.GetVel();
    vdist = animDist.GetVel();
}

double AnimOrbitCam::GetThetaVel()
{
    return animTheta.GetVel();
}

double AnimOrbitCam::GetPhiVel()
{
    return animPhi.GetVel();
}

double AnimOrbitCam::GetDistVel()
{
    return animDist.GetVel();
}

void AnimOrbitCam::GetPanVel(double& vx, double& vy)
{
    vx = animPan[0].GetVel();
    vy = animPan[1].GetVel();
}

bool AnimOrbitCam::Idle(double dt)
{
    bool postRedisplay = false;

    postRedisplay |= animTheta.Run(dt);
    postRedisplay |= animPhi.Run(dt);
    postRedisplay |= animDist.Run(dt);
    postRedisplay |= animPan[0].Run(dt);
    postRedisplay |= animPan[1].Run(dt);
    return postRedisplay;
}

bool AnimOrbitCam::Idle(Camera* cam, double dt)
{
    if (cam == nullptr) {
        return false;
    }

    bool postRedisplay = false;
    if (animTheta.Run(dt)) {
        cam->Orbit(0.0, animTheta.GetVel());
        postRedisplay = true;
    }

    if (animPhi.Run(dt)) {
        cam->Orbit(animPhi.GetVel(), 0.0, m_useLocalZ);
        postRedisplay = true;
    }

    if (animDist.Run(dt)) {
        cam->Fly(animDist.GetVel());
        cam->UpdateMatrices();
        postRedisplay = true;
    }

    if (animPan[0].Run(dt)) {
        cam->Pan(animPan[0].GetVel(), 0.0);
        cam->UpdateMatrices();
        postRedisplay = true;
    }

    if (animPan[1].Run(dt)) {
        cam->Pan(0.0, animPan[1].GetVel());
        cam->UpdateMatrices();
        postRedisplay = true;
    }

    return postRedisplay;
}

void AnimOrbitCam::Play(bool play)
{
    animTheta.Play(play);
    animPhi.Play(play);
    animDist.Play(play);
    animPan[0].Play(play);
    animPan[1].Play(play);
}

void AnimOrbitCam::PlayTheta(bool play)
{
    animTheta.Play(play);
}

void AnimOrbitCam::PlayPhi(bool play)
{
    animPhi.Play(play);
}

void AnimOrbitCam::PlayDist(bool play)
{
    animDist.Play(play);
}

void AnimOrbitCam::PlayPan(bool play)
{
    animPan[0].Play(play);
    animPan[1].Play(play);
}

void AnimOrbitCam::SetFactor(double ftheta, double fphi, double fdist)
{
    SetThetaFactor(ftheta);
    SetPhiFactor(fphi);
    SetDistFactor(fdist);
}

void AnimOrbitCam::SetThetaFactor(double factor)
{
    animTheta.SetVelFactor(factor);
}

void AnimOrbitCam::SetPhiFactor(double factor)
{
    animPhi.SetVelFactor(factor);
}

void AnimOrbitCam::SetDistFactor(double factor)
{
    animDist.SetVelFactor(factor);
}

void AnimOrbitCam::SetPanFactor(double factor)
{
    animPan[0].SetVelFactor(factor);
    animPan[1].SetVelFactor(factor);
}

void AnimOrbitCam::SetLocalZ(bool use_local_z)
{
    m_useLocalZ = use_local_z;
}
