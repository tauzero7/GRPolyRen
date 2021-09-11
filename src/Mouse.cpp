/**
 * File:    Mouse.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "Mouse.h"

namespace grpr {

Mouse::Mouse()
    : button(-1)
    , action(-1)
    , mods(-1)
    , xpos(0.0)
    , ypos(0.0)
{
    // blank
}

Mouse::Mouse(int button, int action, int mods)
    : button(button)
    , action(action)
    , mods(mods)
    , xpos(0.0)
    , ypos(0.0)
{
    // blank
}

Mouse::Mouse(double xpos, double ypos)
    : button(-1)
    , action(-1)
    , mods(-1)
    , xpos(xpos)
    , ypos(ypos)
{
    // blank
}

void Mouse::Reset()
{
    button = -1;
    action = -1;
    mods = -1;
    xpos = ypos = 0.0;
}

void Mouse::Delta(const double x, const double y, double& dx, double& dy)
{
    dx = x - xpos;
    dy = y - ypos;
}

double Mouse::Dx(const double x)
{
    return x - xpos;
}

double Mouse::Dy(const double y)
{
    return y - ypos;
}

double Mouse::x()
{
    return xpos;
}

double Mouse::y()
{
    return ypos;
}

int Mouse::px()
{
    return static_cast<int>(xpos);
}

int Mouse::py()
{
    return static_cast<int>(ypos);
}

}
