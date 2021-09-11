/**
 * File:    ImGUIHandle.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_IMGUI_HANDLE_H
#define GRPR_IMGUI_HANDLE_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GLFW/glfw3.h"

class ImGUIHandle
{
public:
    ImGUIHandle();
    ~ImGUIHandle();

    void Draw();

    void NewFrame();

    bool Setup(GLFWwindow* window, const char* glsl_version = "#version 330");

    void Shutdown();

private:
    GLFWwindow* m_window;
};

#endif // GRPR_IMGUI_HANDLE_H
