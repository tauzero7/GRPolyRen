/**
 * File:    main.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 * 
 *  Run:
 *    ./GLPolyRen <object filename>  <setting filename>
 */
#include <chrono>
#include <iostream>
#include <thread>
#include <tuple>

// take care of the order: glad then glfw
#ifdef WIN32
#include <Windows.h>
#endif
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "ImGUIHandle.h"
#include "FPSCounter.h"
#include "Renderer.h"
#include "StringUtils.h"

#include "portable-file-dialogs.h"

#ifdef HAVE_LUA
#include "LuaHandle.h"
#endif // HAVE_LUA

// name of lookup table
//std::string lutFilename = std::string("lut_r40_256x512.dat");
std::string lutFilename = std::string("lut_r40_32x64.dat");

// default name of config setting filename
std::string cfgFilename = std::string("setting.cfg");

// initial window size
int window_width = 1280;
int window_height = 720;

static GLFWwindow* window = nullptr;
Renderer* renderer = nullptr;

static ImGUIHandle imh;
static FPSCounter fpsCounter;

void error_callback(int error, const char* description)
{
    std::ignore = error;
    fputs(description, stderr);
}

void display(GLFWwindow* window)
{
    if (renderer != nullptr) {
        renderer->Display();
    }
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::ignore = scancode;

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) {
        return;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_ESCAPE: {
                glfwSetWindowShouldClose(window, GL_TRUE);
                return;
            }
        }

        if (renderer != nullptr) {
            renderer->KeyPressEvent(key, mods);
        }
    }
}

void mouse(GLFWwindow*, double xpos, double ypos)
{
    ImGuiIO& io = ImGui::GetIO();

    if (renderer != nullptr && !io.WantCaptureMouse) {
        renderer->Motion(xpos, ypos);
    }
}

void mouseButton(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    renderer->UpdateMousePos(xpos, ypos);
    renderer->Mouse(button, action, mods);
}

void resize(GLFWwindow*, int width, int height)
{
    if (renderer != nullptr) {
        renderer->SetWindowSize(width, height);
    }
    glViewport(0, 0, width, height);
}

bool initGLFW()
{
    // -----------------------------------
    //  Initialize GLFW
    // -----------------------------------
    if (!glfwInit()) {
        fprintf(stderr, "Cannot initialize glfw.\n");
        return false;
    }

    // ---------------------------------------
    //  Set window hints
    // ---------------------------------------
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    // ---------------------------------------
    //  Set error callback function
    // ---------------------------------------
    glfwSetErrorCallback(error_callback);

    // ---------------------------------------
    //  Get primary monitor
    // ---------------------------------------
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    // ---------------------------------------
    //  Get video mode
    // ---------------------------------------
    auto mode = glfwGetVideoMode(monitor);
    if (true) {
        fprintf(stderr, "VideoMode:\n");
        fprintf(stderr, "    Screen resolution  : %d x %d\n", mode->width, mode->height);
        fprintf(stderr, "    Color bits (r,g,b) : %d %d %d\n", mode->redBits, mode->greenBits, mode->blueBits);
        fprintf(stderr, "    Refreshrate        : %d\n", mode->refreshRate);
    }

    window = glfwCreateWindow(window_width, window_height, "GRPolyRen", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Window or context creation failed.\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    return true;
}

void initDebugging()
{
#ifdef _DEBUG
    // ---------------------------------------
    //  Initialize debugging
    // ---------------------------------------
    GLint numExt;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
    bool khrDebugFound = false;
    for (unsigned int x = 0; x < static_cast<unsigned int>(numExt); x++) {
        const GLubyte* str = glGetStringi(GL_EXTENSIONS, x);
        if (strcmp(reinterpret_cast<const char*>(str), "GL_KHR_debug") == 0) {
            // printf("%s ", str);
            khrDebugFound = true;

            glEnable(GL_DEBUG_OUTPUT);
#ifndef _WIN32
            glDebugMessageCallback(oglview::DebugCallback, nullptr);
#endif
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

   //        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 1, &ignoreID, GL_FALSE);
            // oglview::SetDebugIgnoreID(131184); // 0x20070
            // oglview::SetDebugIgnoreID(131185); // 0x20071
            // oglview::SetDebugIgnoreID(131204); // 0x20084
            // oglview::SetDebugIgnoreID(131076); // 0x20004  Usage warning: Generic vertex ... (0x(nil))
        }
    }
#endif // _DEBUG
}

/**
 * Set callback functions
 */
void setCallbackFunctions()
{
    glfwSetKeyCallback(window, keyboard);
    glfwSetMouseButtonCallback(window, mouseButton);
    glfwSetCursorPosCallback(window, mouse);
    glfwSetFramebufferSizeCallback(window, resize);
    glfwSetWindowRefreshCallback(window, display);
}

/**
 * File loading.
 */
bool loadFiles(const std::vector<std::string> files)
{
    if (files.size() > 0) {
        if (StringEndsWith(files[0].c_str(), ".lua")) {
#ifdef HAVE_LUA
            LInit();
            LRunFile(files[0].c_str());
            LClose();
            return true;
#else
            fprintf(stderr, "  lua: must be enabled at build-time.\n");
            return false;
#endif
        }
        else {
            bool ok = renderer->LoadObjOrSetting(files[0].c_str());
            if (files.size() > 1) {
                ok = ok && renderer->LoadObjOrSetting(files[1].c_str());
            }
            return ok;
        }
    }
    else {
        return false;
    }
}

/**
 * Render UI
 */
void renderGUI()
{
    imh.NewFrame();
    ImGui::Begin("GRPolyRen - Control");

    if (ImGui::Button("Load Files")) {
        std::vector<std::string> filters = { "Lua Files", "*.lua", "Object/Config Files", "*.obj *.cfg" };
        auto files = pfd::open_file("Load files", ".", filters, pfd::opt::multiselect).result();
        if (!loadFiles(files)) {
            fprintf(stderr, "Loading files failed");
        }
    }

    renderer->RenderGUI();

    if (ImGui::Button("Save current state")) {
        renderer->SaveSetting(cfgFilename.c_str());
    }

    ImGui::SameLine();

    if (ImGui::Button(" Quit ")) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    ImGui::End();
    imh.Draw();
}


#ifdef HAVE_LUA
void draw() {
    fprintf(stderr, "  draw() : cannot be used in interactive version.\n");
}

void setWindowSize(int width, int height) {
    window_width = width;
    window_height = height;
    glfwSetWindowSize(window, width, height);
    renderer->SetWindowSize(width, height);
}

bool saveImageToFile(const char* ) {
    fprintf(stderr, "  saveImageToFile() : cannot be used in interactive version.\n");
    return false;
}
#endif // HAVE_LUA

/**
 * main
 */
int main(int argc, char* argv[])
{
    if (!initGLFW() || window == nullptr) {
        return -1;
    }

//    initDebugging();

    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize GLAD.\n");
        return -1;
    }

    setCallbackFunctions();

    glfwSwapInterval(1);
    glfwSetWindowPos(window, 50, 50);

    //glEnable(GL_MULTISAMPLE);
    //glfwWindowHint(GLFW_SAMPLES, 4);

    renderer = new Renderer();
    renderer->Init(window_width, window_height);
    renderer->LoadLUT(lutFilename.c_str());

    if (argc > 1) {
        std::vector<std::string> files;
        for (int i = 1; i < argc; ++i) {
            files.push_back(std::string(argv[i]));
        }
        if (!loadFiles(files)) {
            fprintf(stderr, "Usage: ./GRPolyRen[d] <filename.obj>  [<setting.cfg>]\n");
#ifdef HAVE_LUA
            fprintf(stderr, "or:    ./GRPolyRen[d] <script.lua>\n");
#endif
            return -1;
        }
    }

    glfwShowWindow(window);
    imh.Setup(window);

    // ---------------------------------------
    //  main render loop
    // ---------------------------------------
#ifdef USE_FPS    
    fpsCounter.SetNumFrames(100);
    fpsCounter.Start();
    glfwSwapInterval(0);
#endif // USE_FPS

    while (!glfwWindowShouldClose(window)) {

        renderer->Idle(glfwGetTime());
        display(window);

#ifdef USE_FPS            
        double fps = fpsCounter.GetFPS();
        fprintf(stderr, "\r %6.1f  ", fps);
#endif // USE_FPS        

        renderGUI();
        glfwSwapBuffers(window);

#ifndef USE_FPS            
        std::this_thread::sleep_for(std::chrono::microseconds(5000));
#endif // USE_FPS

        glfwPollEvents();
    }
    fprintf(stderr, "\n");

    imh.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}