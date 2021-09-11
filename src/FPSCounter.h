/**
 * File:    FPSCounter.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_FPS_COUNTER_H
#define GRPR_FPS_COUNTER_H

#include <chrono>

class FPSCounter {
public:
    FPSCounter();
    ~FPSCounter();

    double GetFPS();

    void SetNumFrames(unsigned int numFrames);

    void Start();

protected: 
    void clear();

protected:
    double* m_frameTimes;
    unsigned int m_numFrames;
    unsigned int m_currFrame;

    std::chrono::time_point<std::chrono::system_clock> m_prevTime;
};

#endif // GRPR_FPS_COUNTER_H
