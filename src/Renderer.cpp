/**
 * File:    Renderer.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "glad/glad.h"

#include "FileTokenizer.h"
#include "Renderer.h"
#include "StringUtils.h"
#include "Utilities.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr float r_s = 2.0f;

#ifdef HAVE_IMGUI
#include "imgui.h"
#endif // HAVE_IMGUI

const char* const Renderer::MouseCtrlNames[] = { "Camera", "Object" };
const char* const Renderer::ViewModeNames[] = { "Flat", "GR", "GRgeom", "GRtess" };

Renderer::Renderer()
    : m_activeShader(nullptr)
    , prevTime(0.0)
    , m_mouseCtrl(MouseCtrl::Object)
    , m_viewMode(ViewMode::Flat)
    , m_maxTessLevel(32)
    , m_tessFactor(1.0f)
    , m_tessExpon(0.75f)
    , m_distRelation(100.0f)
    , m_wireframe(false)
    , m_isInitialized(false)
{
    m_patFreq[0] = m_patFreq[1] = 8;

    m_lights[0].SetUniformName("light1");
    m_lights[0].SetFactor(1.0f);

    m_clearColor[0] = m_clearColor[1] = m_clearColor[2] = 0.0f;
}

Renderer::~Renderer()
{
    //
}

bool Renderer::Display()
{
    if (m_activeShader == nullptr || !m_isInitialized) {
        return false;
    }

    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, (m_wireframe ? GL_LINE : GL_FILL));

    glm::mat4 transMX = glm::make_mat4(m_transScale.GetTransMatrixPtr());
    glm::mat4 scaleMX = glm::make_mat4(m_transScale.GetScaleMatrixPtr());
    glm::mat4 rotMX = glm::make_mat4(m_eulerRot.GetMatrixPtr());

    glm::mat4 modelMX = glm::mat4(1.0f);
    //modelMX = transMX * scaleMX * rotMX;
    modelMX = transMX * rotMX * scaleMX;

    m_activeShader->Bind();
    m_activeShader->SetFloatMatrix("projMX", 4, 1, GL_FALSE, m_camera.GetProjMatrixPtr());
    m_activeShader->SetFloatMatrix("viewMX", 4, 1, GL_FALSE, m_camera.GetViewMatrixPtr());
    m_activeShader->SetFloatMatrix("modelMX", 4, 1, GL_FALSE, glm::value_ptr(modelMX));

    m_activeShader->SetFloat("obsCamPos", m_lut.GetCameraPos(), 0.0f, 0.0f);

    float xmin, xscale;
    m_lut.GetScaledRange(r_s, xmin, xscale);
    m_activeShader->SetFloat("xmin", xmin);
    m_activeShader->SetFloat("xscale", xscale);

    m_activeShader->SetInt("maxTessLevel", m_maxTessLevel);
    m_activeShader->SetFloat("tessFactor", m_tessFactor);
    m_activeShader->SetFloat("tessExpon", m_tessExpon);
    m_activeShader->SetFloat("distRelation", m_distRelation);

    m_activeShader->SetFloat("patFreq", static_cast<float>(m_patFreq[0]), static_cast<float>(m_patFreq[1]));

    GLenum filter = GL_LINEAR;
    if (glIsTexture(m_lut.GetTexID(0)) && glIsTexture(m_lut.GetTexID(1))) {
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, m_lut.GetTexID(0));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        m_activeShader->SetInt("lutTex0", 10);

        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, m_lut.GetTexID(1));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        m_activeShader->SetInt("lutTex1", 11);
    }

    for(size_t i = 0; i < m_numLights; i++) {
        m_lights[i].UpdateGL(m_activeShader);
    }

    if (m_viewMode == ViewMode::GRtess) {
        Camera obsCam(m_camera);
        obsCam.SetPositionF(m_lut.GetCameraPos(), 0.0f, 0.0f);
        m_activeShader->SetFloatMatrix("obsCamViewMX", 4, 1, GL_FALSE, obsCam.GetViewMatrixPtr());
    }

    bool asPatch = (m_viewMode == ViewMode::GRtess);

    m_activeShader->SetFloat("imageOrder", 0.0f);
    drawObject(m_activeShader, asPatch);

    if (m_viewMode == ViewMode::GR || m_viewMode == ViewMode::GRgeom || m_viewMode == ViewMode::GRtess) {
        m_activeShader->SetFloat("imageOrder", 1.0f);
        drawObject(m_activeShader, asPatch);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    m_activeShader->Release();

    m_crossHairs.Draw(m_camera.GetProjMatrixPtr(), m_camera.GetViewMatrixPtr());
    m_blackhole.Draw(m_camera.GetProjMatrixPtr(), m_camera.GetViewMatrixPtr());

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (m_coordSystem.IsVisible()) {
        Camera sysCam(m_camera);
        sysCam.MovePOItoOrigin();
        sysCam.SetDistance(0.0);
        m_coordSystem.Draw(nullptr, sysCam.GetViewMatrixPtr());
    }
    return true;
}

bool Renderer::Idle(double time)
{
    if (!m_isInitialized) {
        return false;
    }

    double dt = time - prevTime;

    m_animCam.Idle(&m_camera, dt);

    prevTime = time;
    return true;
}

bool Renderer::Init(int width, int height)
{
    // -----------------------------
    //  initialize shaders
    // -----------------------------
    std::string myPath = std::string(".");
    std::string vFlatShaderName = "shader/flat.vert";
    std::string fFlatShaderName = "shader/flat.frag";
    m_shaderFlat.SetFileNames(vFlatShaderName.c_str(), fFlatShaderName.c_str());
    m_shaderFlat.SetLocalPath(myPath.c_str());

    std::string vGRShaderName = "shader/grpr_standard.vert";
    std::string fGRShaderName = "shader/grpr_standard.frag";
    m_shaderGR.SetFileNames(vGRShaderName.c_str(), fGRShaderName.c_str());
    m_shaderGR.SetLocalPath(myPath.c_str());

    std::string vGRGeomShaderName = "shader/grpr_geom.vert";
    std::string gGRGeomShaderName = "shader/grpr_geom.geom";
    std::string fGRGeomShaderName = "shader/grpr_geom.frag";
    m_shaderGRgeom.SetFileName(GLShader::Type::Vert, vGRGeomShaderName.c_str());
    m_shaderGRgeom.SetFileName(GLShader::Type::Geom, gGRGeomShaderName.c_str());
    m_shaderGRgeom.SetFileName(GLShader::Type::Frag, fGRGeomShaderName.c_str());
    m_shaderGRgeom.SetLocalPath(myPath.c_str());

    std::string vGRAdaptShaderName = "shader/grpr.vert";
    std::string tcGRAdaptShaderName = "shader/grpr.tc";
    std::string teGRAdaptShaderName = "shader/grpr.te";
    std::string gGRAdaptShaderName = "shader/grpr.geom";
    std::string fGRAdaptShaderName = "shader/grpr.frag";
    m_shaderGRtess.SetFileName(GLShader::Type::Vert, vGRAdaptShaderName.c_str());
    m_shaderGRtess.SetFileName(GLShader::Type::TCtrl, tcGRAdaptShaderName.c_str());
    m_shaderGRtess.SetFileName(GLShader::Type::TEval, teGRAdaptShaderName.c_str());
    m_shaderGRtess.SetFileName(GLShader::Type::Geom, gGRAdaptShaderName.c_str());
    m_shaderGRtess.SetFileName(GLShader::Type::Frag, fGRAdaptShaderName.c_str());
    m_shaderGRtess.SetLocalPath(myPath.c_str());

    // -----------------------------
    //  initialize camera
    // -----------------------------
    using CA = Camera::Action;
    m_camera.SetResolution(width, height);
    m_camera.SetClipPlanes(0.01, 1000.0);
    m_camera.SetAllowedAction(CA::ORBIT);
    m_camera.SetAllowedAction(CA::ORBIT_Z);
    m_camera.SetAllowedAction(CA::ROLL);
    m_camera.SetAllowedAction(CA::PAN);
    m_camera.SetType(Camera::Type::ORBIT_QUATERNION);
    m_camera.SetFoVy(30.0);
    m_camera.SetSpecialPos(CoordAxis::Xpos);
    m_camera.SetDistance(10.0);

    m_animCam.SetLocalZ(true);
    m_animCam.SetFactor(0.05, 0.05, 0.003);
    m_animCam.SetPanFactor(0.002);

    m_eulerRot.Set(0.0, 90.0, 90.0);

    // -----------------------------
    //  initialize objects
    // -----------------------------
    m_coordSystem.Init();

    m_crossHairs.Init();
    m_crossHairs.SetLineLength(0.5f);

    m_blackhole.Init();
    m_blackhole.SetSubdivisions(5);
    m_blackhole.SetColor(0.3f);
    m_blackhole.SetRadius(r_s);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    ReloadShaders();
    SetViewMode(m_viewMode);

    m_isInitialized = true;
    return true;
}

bool Renderer::KeyPressEvent(int key, int mods)
{
    //fprintf(stderr, "0x%x 0x%x\n", key, mods);
    float rotStep = 0.1f;


    if (key == 0x42) { // B
        m_blackhole.ToggleVisibility();
    }
    else if (key == 0x43) { // C
        // m_coordSystem.ToggleVisibility();
        m_mouseCtrl = MouseCtrl::Camera;
    }
    else if (key == 0x4a) { // j
        m_eulerRot.RotAlpha((mods == 0x1 ? -rotStep : rotStep));
    }
    else if (key == 0x4b) { // k
        m_eulerRot.RotBeta((mods == 0x1 ? -rotStep : rotStep));
    }
    else if (key == 0x4c) { // l
        m_eulerRot.RotGamma((mods == 0x1 ? -rotStep : rotStep));
    }
    else if (key == 0x4f) { // O
        m_mouseCtrl = MouseCtrl::Object;
    }
    else if (key == 0x53) { // S
        ReloadShaders();
    }
    else if (key == 0x54) { // T
        m_crossHairs.ToggleVisibility();
    }
    else if (key == 0x57) { // W
        m_wireframe = !m_wireframe;
    }
    else if (key == 0x58) { // X
        if ((mods & 0x0002) == 0x0002) {
            m_camera.SetSpecialPos(CoordAxis::Xneg);
        }
        else {
            m_camera.SetSpecialPos(CoordAxis::Xpos);
        }
        m_animCam.Play(false);
    }
    else if (key == 0x5a) { // Y
        if ((mods & 0x0002) == 0x0002) {
            m_camera.SetSpecialPos(CoordAxis::Yneg);
        }
        else {
            m_camera.SetSpecialPos(CoordAxis::Ypos);
        }
        m_animCam.Play(false);
    }
    else if (key == 0x59) { // Z
        if ((mods & 0x0002) == 0x0002) {
            m_camera.SetSpecialPos(CoordAxis::Zneg);
        }
        else {
            m_camera.SetSpecialPos(CoordAxis::Zpos);
        }
        m_animCam.Play(false);
    }
    else if (key == 0x31) { // 1
        SetViewMode(ViewMode::Flat);
    }
    else if (key == 0x32) { // 2
        SetViewMode(ViewMode::GR);
    }
    else if (key == 0x33) { // 3
        SetViewMode(ViewMode::GRgeom);
    }
    else if (key == 0x34) { // 4
        SetViewMode(ViewMode::GRtess);
    }
    else if (key == 0x106) { // right arrow
        m_transScale.Rotate(0.01);
    }
    else if (key == 0x107) { // left arrow
        m_transScale.Rotate(-0.01);
    }

    return true;
}

bool Renderer::LoadLUT(const char* filename)
{
    return m_lut.Load(filename);
}

bool Renderer::LoadObjOrSetting(const char* filename)
{
    if (StringEndsWith(filename, ".obj")) {
        return LoadObject(filename);
    }
    else if (StringEndsWith(filename, ".cfg")) {
        return LoadSetting(filename);
    }
    return false;
}

bool Renderer::LoadObject(const char* filename)
{
    if (filename == nullptr) {
        return false;
    }

    char* fpath = nullptr;
    char* fname = nullptr;
    SplitFilePath(filename, fpath, fname);

    m_objTexIDs.clear();
    m_obj.ClearAll();

    unsigned int numTriangles;
    bool isOkay = false;

    if (m_obj.ReadObjFile(fpath, fname)) {
        float *verts = nullptr, *norm = nullptr, *tc = nullptr;

        if (m_obj.GenDrawObjects(verts, norm, tc)) {
            m_objVA.Delete();
            m_objVA.Create(m_obj.GetNumDrawVertices());
            m_objVA.SetArrayBuffer(0, GL_FLOAT, 4, verts);
            m_objVA.SetArrayBuffer(1, GL_FLOAT, 3, norm);
            m_objVA.SetArrayBuffer(2, GL_FLOAT, 2, tc);

            numTriangles = m_obj.GetNumDrawVertices() / 3;
        }

        SafeDelete<float>(verts);
        SafeDelete<float>(norm);
        SafeDelete<float>(tc);

        unsigned int numTextures = m_obj.GetNumTextures();
        for (unsigned int i = 0; i < numTextures; i++) {
            m_objTexIDs.push_back(0);
        }

        char* buf = nullptr;
        int texIDref;

        for (unsigned int i = 0; i < numTextures; i++) {
            const char* fn = m_obj.GetTextureName(i, texIDref);
            if (fn == nullptr) {
                continue;
            }

            std::string texFilename = std::string(fpath) + "/" + std::string(fn);
            if (texIDref >= 0 && texIDref < static_cast<int>(m_objTexIDs.size())) {
                GLuint texID = 0;
                //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                // if (oglview::utils::ImageIO::Upload(texFilename.c_str(), texID)) {
                //    m_texIDs[static_cast<size_t>(texIDref)] = texID;
                //}
            }
        }

        SafeDelete<char>(buf);

        // m_centerOfVertices = m_obj.CenterOfVertices();
        isOkay = true;
    }

    SafeDelete<char>(fpath);
    SafeDelete<char>(fname);
    return isOkay;
}

bool Renderer::LoadSetting(const char* filename)
{
    loadSetting(filename);
    return true;
}

bool Renderer::Motion(double x, double y)
{
    bool postRedisplay = false;

    if (m_mouseCtrl == MouseCtrl::Camera) {
        postRedisplay = mouseCameraCtrl(x, y);
    }
    else {
        postRedisplay = mouseObjectCtrl(x, y);
    }

    UpdateMousePos(x, y);
    return postRedisplay;
}

bool Renderer::Mouse(int button, int action, int mods)
{
    lastMouse.button = button;
    lastMouse.action = action;
    lastMouse.mods = mods;
    return true;
}

bool Renderer::ReloadShaders()
{
    bool isOkay = true;
    isOkay &= m_shaderFlat.ReloadShaders();
    isOkay &= m_shaderGR.ReloadShaders();
    isOkay &= m_shaderGRgeom.ReloadShaders();
    isOkay &= m_shaderGRtess.ReloadShaders();
    isOkay &= m_coordSystem.ReloadShaders();
    isOkay &= m_crossHairs.ReloadShaders();
    isOkay &= m_blackhole.ReloadShaders();
    
    if (!isOkay) {
        fprintf(stderr, "Error loading shaders!\n");
    }
    return isOkay;
}

void Renderer::SaveSetting(const char* filename)
{
    saveSetting(filename);
}

void Renderer::SetWindowSize(int width, int height)
{
    std::cerr << "window size: " << width << " " << height << std::endl;
    m_camera.SetResolution(width, height);
}

void Renderer::UpdateMousePos(double x, double y)
{
    lastMouse.xpos = x;
    lastMouse.ypos = y;
}

bool Renderer::isLMBpressed()
{
    return (lastMouse.action == 1 && lastMouse.button == 0);
}

bool Renderer::isMMBpressed()
{
    return (lastMouse.action == 1 && lastMouse.button == 2);
}

bool Renderer::isRMBpressed()
{
    return (lastMouse.action == 1 && lastMouse.button == 1);
}

bool Renderer::isCtrlPressed()
{
    return (lastMouse.mods & 0x002) == 0x002;
}

bool Renderer::isShiftPressed()
{
    return (lastMouse.mods & 0x001) == 0x001;
}

bool Renderer::mouseCameraCtrl(double x, double y)
{
    bool postRedisplay = false;
    double dx = x - lastMouse.xpos;
    double dy = y - lastMouse.ypos;

    if (isLMBpressed()) {
        if (isCtrlPressed()) {
            m_animCam.Play(false);
            m_camera.UpdateMouse(-dx, -dy, Camera::Action::ORBIT_Z);
        }
        else if (isShiftPressed()) {
            m_camera.UpdateMouse(dx, dy, Camera::Action::ROLL);
        }
        else {
            m_animCam.AddVel(-dy, -dx, 0.0);
        }
        postRedisplay = true;
    }
    else if (isMMBpressed()) {
        if (isCtrlPressed()) {
            double dist = m_camera.GetDistance();
            m_camera.Fly(dy * 0.01 * dist);
            m_camera.UpdateMatrices();
        }
        else {
            m_animCam.AddVel(0.0, 0.0, dy);
        }
        postRedisplay = true;
    }
    else if (isRMBpressed()) {
        if (isCtrlPressed()) {
            double moveFactor = m_camera.GetMoveScaleFactor() * 10;
            m_camera.Pan(-dx * moveFactor, dy * moveFactor);
            m_camera.UpdateMatrices();
        }
        else {
            m_animCam.AddPanVel(-dx, dy);
        }

        postRedisplay = true;
    }

    return postRedisplay;
}

bool Renderer::mouseObjectCtrl(double x, double y)
{
    bool postRedisplay = false;
    double dx = x - lastMouse.xpos;
    double dy = y - lastMouse.ypos;
    double fac = 0.01;

    if (isLMBpressed()) {
        m_transScale.Move(0.0, fac * dx, -fac * dy);
        postRedisplay = true;
    }
    else if (isRMBpressed()) {
        m_transScale.Move(fac * dy, 0.0, 0.0);
        postRedisplay = true;
    }

    return postRedisplay;
}

void Renderer::drawObject(GLShader* shader, bool drawAsPatch)
{
    if (shader == nullptr) {
        return;
    }

    unsigned int* objOffsets = m_obj.GetDrawOffsets();
    if (objOffsets != nullptr) {
        OBJLoader::obj_material* mat = nullptr;

        m_objVA.Bind();
        for (unsigned int i = 0; i < m_obj.GetNumDrawObjects(); i++) {
            mat = m_obj.GetMaterial(i);

            if (mat != nullptr) {
                shader->SetFloatArray("ambient", 3, 1, mat->Ka);
                shader->SetFloatArray("diffuse", 3, 1, mat->Kd);
                shader->SetInt("useTexs", 0);

                if (mat->mapID >= 0 && mat->mapID < static_cast<int>(m_objTexIDs.size())) {
                    size_t mID = static_cast<size_t>(mat->mapID);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, m_objTexIDs[mID]);
                    shader->SetInt("tex", 1);
                    shader->SetInt("useTexs", (m_objTexIDs[mID] > 0 ? 1 : 0));
                }
            }
            else {
                shader->SetFloat("Ka", 0.1f, 0.1f, 0.1f);
                shader->SetFloat("Kd", 0.8f, 0.8f, 0.8f);
            }

            m_obj.UpdateGL(shader);

            if (drawAsPatch) {
                glPatchParameteri(GL_PATCH_VERTICES, 3);
                glDrawArrays(GL_PATCHES, static_cast<GLsizei>(objOffsets[i]),
                    static_cast<GLsizei>(objOffsets[i + 1] - objOffsets[i]));
            }
            else {
                glDrawArrays(GL_TRIANGLES, static_cast<GLsizei>(objOffsets[i]),
                    static_cast<GLsizei>(objOffsets[i + 1] - objOffsets[i]));
            }
        }
        m_objVA.Release();
    }
}

void Renderer::RenderGUI()
{
#ifdef HAVE_IMGUI    
    ImVec2 spacing(1,8);

    renderGUImouse();
    ImGui::Dummy(spacing);
    renderGUIcamera();
    ImGui::Dummy(spacing);
    renderGUIobject();
    ImGui::Dummy(spacing);
    renderGUIblackhole();
    ImGui::Dummy(spacing);
    renderGUIview();
    ImGui::Dummy(spacing);
    renderGUIlights();
    ImGui::Dummy(spacing);
    renderGUIBackground();
    ImGui::Dummy(spacing);
#endif    
}

void Renderer::SetViewMode(ViewMode mode)
{
    switch (mode) {
        case ViewMode::Flat: {
            m_activeShader = &m_shaderFlat;
            break;
        }
        case ViewMode::GR: {
            m_activeShader = &m_shaderGR;
            break;
        }
        case ViewMode::GRgeom: {
            m_activeShader = &m_shaderGRgeom;
            break;
        }
        case ViewMode::GRtess: {
            m_activeShader = &m_shaderGRtess;
            break;
        }
        case ViewMode::Count: {
            return;
        }
    }

    m_viewMode = mode;
}

void Renderer::SetViewModeByName(const char* mode)
{
    if (mode == nullptr) {
        return;
    }

    for(int i = 0; i < static_cast<int>(ViewMode::Count); i++) {
        if (strcmp(mode, ViewModeNames[i]) == 0) {
            ViewMode mode = static_cast<ViewMode>(i);
            SetViewMode(mode);
            break;
        }
    }
}

void Renderer::SetClearColor(float r, float g, float b)
{
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
}

void Renderer::GetClearColor(float* rgb)
{
    rgb[0] = m_clearColor[0];
    rgb[1] = m_clearColor[1];
    rgb[2] = m_clearColor[2];
}

void Renderer::SetClearColor(float* rgb)
{
    m_clearColor[0] = rgb[0];
    m_clearColor[1] = rgb[1];
    m_clearColor[2] = rgb[2];
}

#ifdef HAVE_IMGUI
void Renderer::renderGUImouse()
{
    const ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
    const ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_DefaultOpen;

    const char* currMouseCtrl = MouseCtrlNames[static_cast<int>(m_mouseCtrl)];

    if (ImGui::CollapsingHeader("Mouse", headerFlags)) {
        if (ImGui::BeginCombo("control", currMouseCtrl)) {
            for (int n = 0; n < static_cast<int>(MouseCtrl::Count); n++) {
                bool is_selected = (currMouseCtrl == MouseCtrlNames[n]);
                if (ImGui::Selectable(MouseCtrlNames[n], is_selected)) {
                    currMouseCtrl = MouseCtrlNames[n];
                    m_mouseCtrl = static_cast<MouseCtrl>(n);
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
}

void Renderer::renderGUIcamera()
{
    const ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_EnterReturnsTrue;
    const ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_DefaultOpen;

    const char* currentItem = m_camera.GetProjectionName();

    float pos[3], poi[3], fov;
    m_camera.GetPositionF(pos);
    m_camera.GetPoIF(poi);
    fov = m_camera.GetFoVv();

    if (ImGui::CollapsingHeader("Camera", headerFlags)) {
        if (ImGui::BeginCombo("projection", currentItem)) {
            for (int n = 0; n < 2; n++) {
                bool is_selected = (currentItem == Camera::ProjectionNames[n]);
                if (ImGui::Selectable(Camera::ProjectionNames[n], is_selected)) {
                    currentItem = Camera::ProjectionNames[n];
                    m_camera.SetProjection(currentItem);
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::InputFloat3("position", pos, "%6.3f", textFlags)) {
            m_camera.SetPositionF(pos);
        }

        if (ImGui::InputFloat3("poi", poi, "%6.3f", textFlags)) {
            m_camera.SetPoIF(poi);
        }

        if (ImGui::Button("Reset position")) {
            m_animCam.Play(false);
            m_camera.SetPosition(10.0, 0.0, 0.0);
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset poi")) {
            m_animCam.Play(false);
            m_camera.SetPoI(0.0, 0.0, 0.0);            
        }

        if (ImGui::SliderFloat("FoVy", &fov, 1.0f, 120.0f, "%.1f")) {
            m_camera.SetFoVy(fov);
        }
    }
}

void Renderer::renderGUIobject()
{
    const ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
    const ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_DefaultOpen;

    const char* currEulerRot = m_eulerRot.GetOrderName();

    float trans[3], scale[3], rot[3];
    m_transScale.GetScale(scale);
    m_transScale.GetTrans(trans);
    m_eulerRot.Get(rot);
    int patFreq[2] = {m_patFreq[0], m_patFreq[1]};

static float phi = 0.0f;

    if (ImGui::CollapsingHeader("Object", headerFlags)) {
        if (ImGui::InputFloat3("trans", trans, "%6.3f", flags)) {
            m_transScale.SetTrans(trans);
        }

        if (ImGui::InputFloat3("scale", scale, "%6.3f", flags)) {
            m_transScale.SetScale(scale);
        }

        if (ImGui::Button("Reset translation")) {
            m_transScale.ResetTrans();
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset scale")) {
            m_transScale.ResetScale();
        }

        if (ImGui::InputFloat3("euler-angle", rot, "%6.3f", flags)) {
            m_eulerRot.Set(rot);
        }

        if (ImGui::BeginCombo("euler-order", currEulerRot)) {
            for (int n = 0; n < 3; n++) {
                bool is_selected = (currEulerRot == EulerRotation::OrderNames[n]);
                if (ImGui::Selectable(EulerRotation::OrderNames[n], is_selected)) {
                    currEulerRot = EulerRotation::OrderNames[n];
                    m_eulerRot.SetOrderByName(currEulerRot);
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Reset Euler")) {
            m_eulerRot.ResetAll();
        }

        if (ImGui::SliderInt2("patFreq", patFreq, 1, 64, "%d", flags)) {
            m_patFreq[0] = patFreq[0];
            m_patFreq[1] = patFreq[1];
        }

        if (ImGui::DragFloat("orbit-rotate", &phi, 0.0001f, -0.1f, 0.1f, "%.4f")) {
            phi = Clamp(phi, -1.0f, 1.0f);
        }

        if (fabsf(phi) > 1e-5f) {
            m_transScale.Rotate(phi);
        }
    }
}

void Renderer::renderGUIblackhole()
{
    const ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
    const ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_None; //ImGuiTreeNodeFlags_DefaultOpen;

    float radius = m_blackhole.GetRadius();
    float color[4];
    m_blackhole.GetColor(color);
    bool visible = m_blackhole.IsVisible();

    if (ImGui::CollapsingHeader("BlackHole", headerFlags)) {
        
        if (ImGui::InputFloat("radius", &radius, 0.01f, 0.1f, "%0.2f", flags)) {
            m_blackhole.SetRadius(radius);
        }

        if (ImGui::ColorEdit4("color", color)) {
            m_blackhole.SetColor(color);
        }

        if (ImGui::Checkbox("visible", &visible)) {
            m_blackhole.Show(visible);
        }
    }
}

void Renderer::renderGUIview()
{
    const ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
    const ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_DefaultOpen;

    const char* currViewItem = ViewModeNames[static_cast<int>(m_viewMode)];
    bool wireframe = m_wireframe;

    if (ImGui::CollapsingHeader("View", headerFlags)) {
        if (ImGui::BeginCombo("mode", currViewItem)) {
            for (int n = 0; n < static_cast<int>(ViewMode::Count); n++) {
                bool is_selected = (currViewItem == ViewModeNames[n]);
                if (ImGui::Selectable(ViewModeNames[n], is_selected)) {
                    currViewItem = ViewModeNames[n];
                    SetViewMode(static_cast<ViewMode>(n));
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::SliderInt("maxTessLevel", &m_maxTessLevel, 1, 64);
        
        if (ImGui::InputFloat("tessFactor", &m_tessFactor, 0.1f, 1.0f, "%0.1f", flags)) {
            // 
        }

        if (ImGui::InputFloat("tessExpon", &m_tessExpon, 0.1f, 5.0f, "%0.2f", flags)) {
            //
        }

        if (ImGui::InputFloat("distRelation", &m_distRelation, 1.0f, 1000.0f, "%.0f", flags)) {
            //
        }

        if (ImGui::Checkbox("wireframe", &wireframe)) {
            m_wireframe = wireframe;
        }
    }
}

void Renderer::renderGUIlights()
{
    //const ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_DefaultOpen;
    const ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_None;

    float lightPos[2];
    m_lights[0].Get(lightPos[0], lightPos[1]);

    float lightFactor = m_lights[0].GetFactor();
    bool isActive = m_lights[0].IsActive();

    if (ImGui::CollapsingHeader("LightSource", headerFlags)) {
        if (ImGui::Checkbox("active", &isActive)) {
            m_lights[0].SetActive(isActive);
        }

        if (ImGui::SliderFloat("theta", &lightPos[0], 0.01f, 179.99f, "%.2f")) {
            m_lights[0].Set(lightPos[0], lightPos[1]);
        }

        if (ImGui::SliderFloat("phi", &lightPos[1], 0.0f, 360.0f, "%.2f")) {
            m_lights[0].Set(lightPos[0], lightPos[1]);
        }

        if (ImGui::SliderFloat("factor", &lightFactor, 0.0f, 1.0f, "%.3f")) {
            m_lights[0].SetFactor(lightFactor);
        }
    }
}

void Renderer::renderGUIBackground() 
{
    const ImGuiTreeNodeFlags headerFlags = ImGuiTreeNodeFlags_None;

    float color[4];
    GetClearColor(color);

    if (ImGui::CollapsingHeader("Background", headerFlags)) {
        if (ImGui::ColorEdit3("bgcolor", color)) {
            SetClearColor(color);
        }
    }
}

#endif // HAVE_IMGUI

void Renderer::loadSetting(const char* filename)
{
    FileTokenizer ft(filename);
    //ft.ShowAll();

    float data[3], color[4], fval, angles[2];
    int idata[2], ival;
    double val;
    char* buf = nullptr;

    if (ft.GetSubToken("CAMERA_PROJ", 1, buf)) {
        m_camera.SetProjection(buf);
    }

    if (ft.GetSubTokens<float>("CAMERA_POS", 1, 3, data)) {
        m_camera.SetPositionF(data);
    }

    if (ft.GetSubTokens<float>("CAMERA_POI", 1, 3, data)) {
        m_camera.SetPoIF(data);
    }

    if (ft.GetSubToken<double>("CAMERA_FOV", 1, val)) {
        m_camera.SetFoVy(val);
    }

    if (ft.GetSubTokens<float>("OBJECT_TRANS", 1, 3, data)) {
        m_transScale.SetTrans(data);
    }

    if (ft.GetSubTokens<float>("OBJECT_SCALE", 1, 3, data)) {
        m_transScale.SetScale(data);
    }

    if (ft.GetSubTokens<float>("OBJECT_EULER_ROT", 1, 3, data)) {
        m_eulerRot.Set(data);
    }

    if (ft.GetSubTokens<int>("OBJECT_PAT_FREQ", 1, 2, idata)) {
        m_patFreq[0] = idata[0];
        m_patFreq[1] = idata[1];
    }

    if (ft.GetSubToken("OBJECT_EULER_ORDER", 1, buf)) {
        m_eulerRot.SetOrderByName(buf);
    }

    if (ft.GetSubToken<float>("BLACKHOLE_RADIUS", 1, fval)) {
        m_blackhole.SetRadius(fval);
    }

    if (ft.GetSubTokens<float>("BLACKHOLE_COLOR", 1, 4, color)) {
        m_blackhole.SetColor(color);
    }

    if (ft.GetSubToken("VIEW_MODE", 1, buf)) {
        SetViewModeByName(buf);
    }

    ft.GetSubToken<int>("VIEW_MAX_TESS_LEVEL", 1, m_maxTessLevel);
    ft.GetSubToken<float>("VIEW_TESS_FACTOR", 1, m_tessFactor);
    ft.GetSubToken<float>("VIEW_TESS_EXPON", 1, m_tessExpon);
    ft.GetSubBoolToken("VIEW_WIREFRAME", 1, m_wireframe);

    if (ft.GetSubToken<int>("LIGHT_SOURCE_ACTIVE", 1, ival)) {
        m_lights[0].SetActive(ival == 1);
    }

    if (ft.GetSubTokens<float>("LIGHT_SOURCE_ANGLES", 1, 2, angles)) {
        m_lights[0].Set(angles[0], angles[1]);
    }

    if (ft.GetSubToken<float>("LIGHT_SOURCE_FACTOR", 1, fval)) {
        m_lights[0].SetFactor(fval);
    }

    if (ft.GetSubTokens<float>("BACKGROUND_COLOR", 1, 3, color)) {
        m_clearColor[0] = color[0];
        m_clearColor[1] = color[1];
        m_clearColor[2] = color[2];
    }

    SafeDelete<char>(buf);
    fprintf(stderr, "Setting loaded from '%s'\n", filename);
}

void Renderer::saveSetting(const char* filename)
{
    FILE* fptr = nullptr;
#ifdef _WIN32
    fopen_s(&fptr, filename, "wt");
#else
    fptr = fopen(filename, "wt");
#endif  

    if (fptr == nullptr) {
        fprintf(stderr, "Cannot write settings file '%s'\n", filename);
        return;
    }

    float pos[3], poi[3], trans[3], scale[3], rot[3], color[4];
    m_camera.GetPositionF(pos);
    m_camera.GetPoIF(poi);
    double fov = m_camera.GetFoVv();

    m_transScale.GetScale(scale);
    m_transScale.GetTrans(trans);
    m_eulerRot.Get(rot);
    m_blackhole.GetColor(color);

    float theta,phi;
    m_lights[0].Get(theta, phi);

    fprintf(fptr, "CAMERA_PROJ %s\n", m_camera.GetProjectionName());
    fprintf(fptr, "CAMERA_POS  %6.3f %6.3f %6.3f\n", pos[0], pos[1], pos[2]);
    fprintf(fptr, "CAMERA_POI  %6.3f %6.3f %6.3f\n", poi[0], poi[1], poi[2]);
    fprintf(fptr, "CAMERA_FOV  %4.1f\n", fov);
    fprintf(fptr, "\n");

    fprintf(fptr, "OBJECT_TRANS  %6.3f %6.3f %6.3f\n", trans[0], trans[1], trans[2]);
    fprintf(fptr, "OBJECT_SCALE  %6.3f %6.3f %6.3f\n", scale[0], scale[1], scale[2]);
    fprintf(fptr, "\n");
    fprintf(fptr, "OBJECT_EULER_ROT    %6.3f %6.3f %6.3f\n", rot[0], rot[1], rot[2]);
    fprintf(fptr, "OBJECT_EULER_ORDER  %s\n", m_eulerRot.GetOrderName());
    fprintf(fptr, "\n");
    fprintf(fptr, "OBJECT_PAT_FREQ  %d %d\n", m_patFreq[0], m_patFreq[1]);
    fprintf(fptr, "\n");

    fprintf(fptr, "BLACKHOLE_RADIUS  %4.2f\n", m_blackhole.GetRadius());
    fprintf(fptr, "BLACKHOLE_COLOR   %5.3f %5.3f %5.3f %5.3f\n", color[0], color[1], color[2], color[3]);
    fprintf(fptr, "\n");

    fprintf(fptr, "VIEW_MODE            %s\n", ViewModeNames[static_cast<int>(m_viewMode)]);
    fprintf(fptr, "VIEW_MAX_TESS_LEVEL  %d\n", m_maxTessLevel);
    fprintf(fptr, "VIEW_TESS_FACTOR     %.1f\n", m_tessFactor);
    fprintf(fptr, "VIEW_TESS_EXPON      %.2f\n", m_tessExpon);
    fprintf(fptr, "VIEW_WIREFRAME       %d\n", (m_wireframe ? 1 : 0));
    fprintf(fptr, "\n");

    fprintf(fptr, "LIGHT_SOURCE_ACTIVE  %d\n", (m_lights[0].IsActive() ? 1 : 0));
    fprintf(fptr, "LIGHT_SOURCE_ANGLES  %5.2f %5.2f\n", theta, phi);
    fprintf(fptr, "LIGHT_SOURCE_FACTOR  %5.3f\n", m_lights[0].GetFactor());
    fprintf(fptr, "\n");

    fprintf(fptr, "BACKGROUND_COLOR     %5.3f %5.3f %5.3f\n", m_clearColor[0], m_clearColor[1], m_clearColor[2]);
    fprintf(fptr, "\n");
    fclose(fptr);

    fprintf(stderr, "Setting saved to '%s'\n", filename);
}
