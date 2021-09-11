/**
 * File:    FPSCounter.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "FPSCounter.h"
#include <algorithm>

FPSCounter::FPSCounter()
    : m_frameTimes(nullptr)
    , m_numFrames(50)
    , m_currFrame(0)
{
    SetNumFrames(m_numFrames);
}

FPSCounter::~FPSCounter()
{
    clear();
}

double FPSCounter::GetFPS()
{
    auto currTime = std::chrono::system_clock::now();
    double dt = std::chrono::duration_cast<std::chrono::microseconds>(currTime - m_prevTime).count();
    m_prevTime = currTime;

    m_frameTimes[m_currFrame] = dt * 1e-6;

    double sum = 0.0;
    for(unsigned int i = 0; i < m_numFrames; i++) {
        sum += m_frameTimes[i];
    }

    m_currFrame++;
    if (m_currFrame > m_numFrames) {
        m_currFrame = 0;
    }

    return m_numFrames / sum;
}

void FPSCounter::SetNumFrames(unsigned int numFrames)
{
    clear();
    m_numFrames = std::max(1U, numFrames);
    m_frameTimes = new double[m_numFrames];

    for(unsigned int i = 0; i < m_numFrames; i++) {
        m_frameTimes[i] = 0.0;
    }
}

void FPSCounter::Start()
{
    m_prevTime = std::chrono::system_clock::now();
    m_currFrame = 0;
}

void FPSCounter::clear()
{
    if (m_frameTimes != nullptr) {
        delete [] m_frameTimes;
        m_frameTimes = nullptr;
    }
}