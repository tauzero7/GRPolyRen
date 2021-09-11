/**
 * File:    main.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "AnimParam.h"
#include "Utilities.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

#ifdef _WIN32
#define AMIN(x, y) ((x < y) ? (x) : (y))
#define AMAX(x, y) ((x > y) ? (x) : (y))
#else
#define AMIN(x, y) std::min(x, y)
#define AMAX(x, y) std::max(x, y)
#endif

AnimParam::AnimParam()
    : m_type(Type::PosControl)
    , m_play(false)
    , m_acc_factor(1e-3)
    , m_acc_damp(0.8)
    , m_vel_factor(1e-3)
    , m_vel_damp(3.0)
    , m_vel_expon(1.0)
    , m_eps(1e-5)
    , m_acc(0.0)
    , m_vel(0.0)
    , m_pos(0.0)
{
    m_acc_range[0] = -std::numeric_limits<double>::max();
    m_acc_range[1] = std::numeric_limits<double>::max();

    m_vel_range[0] = -std::numeric_limits<double>::max();
    m_vel_range[1] = std::numeric_limits<double>::max();
}

AnimParam::AnimParam(Type type)
    : m_type(type)
    , m_play(false)
    , m_acc_factor(1e-3)
    , m_acc_damp(0.8)
    , m_vel_factor(1e-3)
    , m_vel_damp(3.0)
    , m_vel_expon(1.0)
    , m_eps(1e-5)
    , m_acc(0.0)
    , m_vel(0.0)
    , m_pos(0.0)
{
    m_acc_range[0] = -std::numeric_limits<double>::max();
    m_acc_range[1] = std::numeric_limits<double>::max();

    m_vel_range[0] = -std::numeric_limits<double>::max();
    m_vel_range[1] = std::numeric_limits<double>::max();
}

AnimParam::~AnimParam()
{
    // intentionally left blank
}

void AnimParam::AddAcc(double dx)
{
    m_acc = dx * m_acc_factor;
    m_play = true;
}

void AnimParam::AddVel(double dx)
{
    m_vel += dx * m_vel_factor;
    m_play = true;
}

double AnimParam::GetCurr()
{
    return m_pos;
}

double AnimParam::GetVel()
{
    return m_vel;
}

double AnimParam::GetAccDamp()
{
    return m_acc_damp;
}

double AnimParam::GetVelDamp()
{
    return m_vel_damp;
}

double AnimParam::GetVelFactor()
{
    return m_vel_factor;
}

double AnimParam::GetVelExpon()
{
    return m_vel_expon;
}

double AnimParam::GetEpsilon()
{
    return m_eps;
}

void AnimParam::GetAccRange(double& amin, double& amax)
{
    amin = m_acc_range[0];
    amax = m_acc_range[1];
}

void AnimParam::GetVelRange(double& vmin, double& vmax)
{
    vmin = m_vel_range[0];
    vmax = m_vel_range[1];
}

void AnimParam::Play(bool play)
{
    m_play = play;
    if (!m_play) {
        m_acc = 0.0;
        m_vel = 0.0;
    }
}

bool AnimParam::Run(double dt)
{
    if (m_play) {
        update(dt);

        if (m_type == Type::PosControl) {
            if (fabs(m_pos) < m_eps) {
                Play(false);
                return false;
            }
        }
        else if (m_type == Type::VelControl) {
            if (fabs(m_vel) < m_eps) {
                Play(false);
                return false;
            }
        }
        return true;
    }
    return false;
}

void AnimParam::update(double dt)
{
    double vh = m_vel + m_acc * 0.5 * dt;
    double xh = m_pos + vh * 0.5 * dt;

    m_pos = xh + vh * 0.5 * dt;
    m_vel = vh + m_acc * 0.5 * dt;

    double vsign = (std::signbit(m_vel) ? -1.0 : 1.0);

    m_acc -= m_acc * m_acc_damp * dt;
    m_vel -= vsign * pow(fabs(m_vel), m_vel_expon) * m_vel_damp * dt;

    m_acc = Clamp(m_acc, m_acc_range[0], m_acc_range[1]);
    m_vel = Clamp(m_vel, m_vel_range[0], m_vel_range[1]);

    // fprintf(stderr, "%12.8f %12.8f %12.8f %12.8f\n", m_acc, m_vel, m_pos, dt);
}

void AnimParam::SetCurr(double curr)
{
    m_pos = curr;
}

void AnimParam::SetVel(double vel)
{
    m_vel = vel;
}

void AnimParam::SetAcc(double acc)
{
    m_acc = acc;
}

void AnimParam::SetAccFactor(double factor)
{
    m_acc_factor = std::max(0.0, factor);
}

void AnimParam::SetAccDamp(double damp)
{
    m_acc_damp = Clamp(damp, 0.0, 1.0);
}

void AnimParam::SetVelFactor(double factor)
{
    m_vel_factor = std::max(0.0, factor);
}

void AnimParam::SetVelDamp(double damp)
{
    m_vel_damp = std::max(0.0, damp);
}

void AnimParam::SetVelExpon(double expon)
{
    m_vel_expon = expon;
}

void AnimParam::SetEpsilon(double eps)
{
    m_eps = fabs(eps);
}

void AnimParam::SetAccRange(double amin, double amax)
{
    if (amin > amax) {
        std::swap(amin, amax);
    }

    m_acc_range[0] = amin;
    m_acc_range[1] = amax;
}

void AnimParam::SetVelRange(double vmin, double vmax)
{
    if (vmin > vmax) {
        std::swap(vmin, vmax);
    }

    m_vel_range[0] = vmin;
    m_vel_range[1] = vmax;
}

void AnimParam::SetType(Type type)
{
    m_type = type;
}
