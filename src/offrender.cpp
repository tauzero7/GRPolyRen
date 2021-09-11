/**
 * File:    offrender.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 *
 *  If Lua is available then run:
 *    ./OfflineRen.exe  filename.lua
 *
 *  otherwise run:
 *    ./OfflineRen.exe  object.obj [settings.cfg]
 */
#include <chrono>
#include <iostream>
#include <thread>
#include <tuple>

// glad then glfw
#include "glad/glad.h"

#include "GLFW/glfw3.h"
#include "Renderer.h"

#ifdef HAVE_LUA
#include "LuaHandle.h"
#endif // HAVE_LUA

std::string lutFilename = std::string("lut_r40_256x512.dat");
std::string cfgFilename = std::string("setting.cfg");

int window_width = 1280;
int window_height = 720;

static GLFWwindow* window = nullptr;
Renderer* renderer = nullptr;
GLuint fbo = 0, fboDepth = 0, fboImg = 0;

void deleteFBO() {
    if (glIsTexture(fboImg)) {
        glDeleteTextures(1, &fboImg);
        fboImg = 0;
    }

    if (glIsRenderbuffer(fboDepth)) {
        glDeleteRenderbuffers(1, &fboDepth);
        fboDepth = 0;
    }

    if (glIsFramebuffer(fbo)) {
        glDeleteFramebuffers(1, &fbo);
        fbo = 0;
    }
}

/**
 * 
 */
bool createFBO() {
    deleteFBO();

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenRenderbuffers(1, &fboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, window_width, window_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboDepth);
    
    glGenTextures(1, &fboImg);
    glBindTexture(GL_TEXTURE_2D, fboImg);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboImg, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "FBO incomplete!\n");
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    fprintf(stderr, "FBO complete.\n");
    return true;
}

/**
 * 
 */
void draw() {
    fprintf(stderr, "Render image...\n");
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, window_width, window_height);

    renderer->Display();
    glfwSwapBuffers(window);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * 
 */
bool saveImageToFile(const char* filename) {
    size_t bufSize = window_width * window_height * 3;
    unsigned char* rgb = new unsigned char[bufSize];

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, window_width, window_height, GL_RGB, GL_UNSIGNED_BYTE, rgb);

    FILE* fptr = nullptr;
#ifdef _WIN32
    fopen_s(&fptr, filename, "wb");
#else
    fptr = fopen(filename, "wb");
#endif

    if (fptr == nullptr) {
        fprintf(stderr, "Error...\n");
        delete [] rgb;
        return false;
    }

    fprintf(stderr, "Save image to file '%s'.\n", filename);

    fprintf(fptr, "P6\n%d %d\n255\n", window_width, window_height);    
    //fwrite(rgb, sizeof(unsigned char), window_width * window_height * 3, fptr);

    unsigned char* dptr;
    for(unsigned int r = 0; r < window_height; r++) {
        dptr = &rgb[3 * (window_height - r - 1) * window_width];
        fwrite(dptr, sizeof(unsigned char), window_width * 3, fptr);
    }
    fclose(fptr);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    delete [] rgb;
    return true;
}

void setWindowSize(int width, int height) {
    window_width = width;
    window_height = height;
    renderer->SetWindowSize(width, height);
    createFBO();
}

/**
 * 
 */
int main(int argc, char* argv[]) {

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
    window = glfwCreateWindow(640, 480, "", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize GLAD.\n");
        return -1;
    }

    createFBO();

    renderer = new Renderer();
    renderer->Init(window_width, window_height);
    renderer->LoadLUT(lutFilename.c_str());


#ifdef HAVE_LUA
    LInit();
    if (argc > 1) {
        LRunFile(argv[1]);
    }
    LClose();
#else
    if (argc > 1) {
        renderer->LoadObject(argv[1]);
    }

    if (argc > 2) {
        renderer->LoadSetting(argv[2]);
    }

    draw();
    saveImageToFile("out.ppm");    
#endif    

    deleteFBO();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}