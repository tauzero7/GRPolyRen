/**
 * File:    Renderer.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_RENDERER_H
#define GRPR_RENDERER_H

#include <iostream>
#include <vector>

#include "AnimOrbitCam.h"
#include "Camera.h"
#include "CoordSystem.h"
#include "CrossHairs3D.h"
#include "EulerRotation.h"
#include "GLShader.h"
#include "LightSource.h"
#include "LUT.h"
#include "Mouse.h"
#include "OBJLoader.h"
#include "SDSphere.h"
#include "TransScale.h"
#include "VertexArray.h"

class Renderer
{
public:
    enum class MouseCtrl : int { Camera = 0, Object, Count };
    enum class ViewMode : int { Flat = 0, GR, GRgeom, GRtess, Count };

    static const char* const MouseCtrlNames[];
    static const char* const ViewModeNames[];

public:
    Renderer();
    ~Renderer();

    bool Display();

    bool Idle(double time);

    bool Init(int width, int height);

    bool KeyPressEvent(int key, int mods);

    bool LoadLUT(const char* filename);

    bool LoadObjOrSetting(const char* filename);

    bool LoadObject(const char* filename);

    bool LoadSetting(const char* filename);

    bool Motion(double x, double y);

    bool Mouse(int button, int action, int mods);

    bool ReloadShaders();

    void SaveSetting(const char* filename);

    void SetWindowSize(int width, int height);

    void UpdateMousePos(double x, double y);

    void RenderGUI();

    void SetViewMode(ViewMode mode);
    void SetViewModeByName(const char* mode);

    void GetClearColor(float* rgb);
    void SetClearColor(float* rgb);
    void SetClearColor(float r, float g, float b);

protected:
    bool isLMBpressed();
    bool isMMBpressed();
    bool isRMBpressed();
    bool isCtrlPressed();
    bool isShiftPressed();

    bool mouseCameraCtrl(double x, double y);
    bool mouseObjectCtrl(double x, double y);

    void drawObject(GLShader* shader, bool drawAsPatch);

#ifdef HAVE_IMGUI
    void renderGUImouse();
    void renderGUIcamera();
    void renderGUIobject();
    void renderGUIblackhole();
    void renderGUIview();
    void renderGUIlights();
    void renderGUIBackground();
#endif // HAVE_IMGUI

    void loadSetting(const char* filename);
    void saveSetting(const char* filename);

public:
    Camera m_camera;
    TransScale m_transScale;
    EulerRotation m_eulerRot;
    SDSphere m_blackhole;
    CoordSystem m_coordSystem;
    CrossHairs3D m_crossHairs;

    int m_maxTessLevel;
    float m_tessFactor;
    float m_tessExpon;
    float m_distRelation;
    int m_patFreq[2];

    static const size_t m_numLights = 1;
    LightSource m_lights[m_numLights];

    OBJLoader m_obj;
    
protected:
    GLShader m_shaderFlat;
    GLShader m_shaderGR;
    GLShader m_shaderGRgeom;
    GLShader m_shaderGRtess;
    GLShader* m_activeShader;

    AnimOrbitCam m_animCam;

    VertexArray m_objVA;
    std::vector<GLuint> m_objTexIDs;

    LUT m_lut;

    double prevTime;
    grpr::Mouse lastMouse;

    MouseCtrl m_mouseCtrl;
    ViewMode m_viewMode;

    float m_clearColor[3];
    bool m_wireframe;
    bool m_isInitialized;
};

#endif // GRPR_RENDERER_H
