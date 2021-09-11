/**
 * File:    main.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_ANIM_PARAM_H
#define GRPR_ANIM_PARAM_H

class AnimParam
{
public:
    enum class Type { PosControl, VelControl };

public:
    AnimParam();
    explicit AnimParam(Type type);
    ~AnimParam();

    /**
     * @brief Add acceleration value.
     *   Call this method in mouse motion.
     * @param dx
     */
    void AddAcc(double dx);

    /**
     * @brief Add velocity value.
     *   Call this method in mouse motion.
     * @param dx
     */
    void AddVel(double dx);

    /// Get current position.
    double GetCurr();

    /// Get current velocity.
    double GetVel();

    double GetAccDamp();
    double GetVelDamp();
    double GetVelFactor();
    double GetVelExpon();

    double GetEpsilon();

    void GetAccRange(double& amin, double& amax);
    void GetVelRange(double& vmin, double& vmax);

    void Play(bool play);

    /// Call this method in a plugin's 'Idle' method.
    bool Run(double dt = 0.001);

    /// Set current position
    void SetCurr(double curr);

    /// Set current velocity
    void SetVel(double vel);

    /// Set current acceleration
    void SetAcc(double acc);

    /// This factor is used in 'AddAcc' to scale the input value.
    void SetAccFactor(double factor);

    /// Set acceleration damping value.
    void SetAccDamp(double damp);

    /// This factor is used in 'AddVel' to scale the input value.
    void SetVelFactor(double factor);
    void SetVelDamp(double damp);
    void SetVelExpon(double expon);

    /**
     * @brief Set epsilon threshold.
     *   In 'VelControl' mode, the animation will be stopped if velocity is less than 'eps'.
     * @param eps
     */
    void SetEpsilon(double eps);

    /// Set clamping range for acceleration.
    void SetAccRange(double amin, double amax);

    /// Set clamping range for velocity.
    void SetVelRange(double vmin, double vmax);

    void SetType(Type type);

protected:
    void update(double dt);

protected:
    Type m_type;
    bool m_play;

    /// Multiply input value dx by this factor before adding to acceleration
    double m_acc_factor;
    double m_acc_damp;

    /// Multiply input value dx by this factor before adding to velocity
    double m_vel_factor;
    double m_vel_damp;
    double m_vel_expon;

    double m_acc_range[2];
    double m_vel_range[2];

    double m_eps;

    double m_acc;
    double m_vel;
    double m_pos;
};

#endif // GRPR_ANIM_PARAM_H
