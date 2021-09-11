/**
 * File:    Mouse.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_MOUSE_H
#define GRPR_MOUSE_H

namespace grpr {

class Mouse
{
public:
    Mouse();
    Mouse(int button, int action, int mods);
    Mouse(double xpos, double ypos);

    void Delta(const double x, const double y, double& dx, double& dy);

    double Dx(const double x);

    double Dy(const double y);

    /// Get last x-coordinate of mouse position in pixels (double) from upper left corner.
    double x();

    /// Get last y-coordinate of mouse position in pixels (double) from upper left corner
    double y();

    /// Get last x-coordinate of mouse position in pixels (int) from upper left corner.
    int px();

    /// Get last y-coordinate of mouse position in pixels (int) from upper left corner.
    int py();

    void Reset();

public:
    int button;
    int action;
    int mods;
    double xpos;
    double ypos;
};

} // namespace grpr

#endif // GRPR_MOUSE_H
