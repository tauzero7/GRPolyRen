/**
 * File:    LUT.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "LUT.h"
#include "Utilities.h"
#include <sys/stat.h>

LUT::LUT()
    : m_Nr(0)
    , m_Nphi(0)
    , m_rmin(0.0f)
    , m_rmax(0.0f)
    , m_camPos(10.0f)
    , m_data(nullptr)
{
    m_texID[0] = m_texID[1] = 0;
}

LUT::~LUT()
{
    //
}

float LUT::GetCameraPos()
{
    return m_camPos;
}

void LUT::GetRadialRange(float& rmin, float& rmax)
{
    rmin = m_rmin;
    rmax = m_rmax;
}

void LUT::GetScaledRange(float rs, float& xmin, float& xscale)
{
    float xmax = rs / m_rmin;
    xmin = rs / m_rmax;
    xscale = 1.0f / (xmax - xmin);
}

GLuint LUT::GetTexID(unsigned int idx)
{
    if (idx < 2) {
        return m_texID[idx];
    }
    return 0;
}

bool LUT::Load(const char* filename)
{
    size_t headerSize = sizeof(unsigned int) * 2 + sizeof(float) * 3;

    if (filename == nullptr) {
        fprintf(stderr, "No filename given!\n");
        return false;
    }

    SafeDelete<float>(m_data);
    if (glIsTexture(m_texID[0])) {
        glDeleteTextures(2, m_texID);
        m_texID[0] = 0;
        m_texID[1] = 0;
    }

    size_t fileSizeInBytes = getFileSizeInBytes(filename);
    if (fileSizeInBytes < headerSize) {
        fprintf(stderr, "LUT file '%s' not valid!\n", filename);
        return false;
    }

    FILE* fptr = nullptr;
#ifdef _WIN32
    fopen_s(&fptr, filename, "rb");
#else
    fptr = fopen(filename, "rb");
#endif
    if (fptr == nullptr) {
        fprintf(stderr, "Cannot load LUT '%s'\n", filename);
        return false;
    }

    // read header
    bool isOkay = true;
    isOkay &= (fread(&m_Nr, sizeof(unsigned int), 1, fptr) == 1);
    isOkay &= (fread(&m_Nphi, sizeof(unsigned int), 1, fptr) == 1);
    isOkay &= (fread(&m_rmin, sizeof(float), 1, fptr) == 1);
    isOkay &= (fread(&m_rmax, sizeof(float), 1, fptr) == 1);
    isOkay &= (fread(&m_camPos, sizeof(float), 1, fptr) == 1);

    if (!isOkay) {
        fprintf(stderr, "LUT '%s' has no valid header!\n", filename);
        fclose(fptr);
        return false;
    }

    size_t numEntries = m_Nr * m_Nphi * 4U;
    size_t dataSizeInBytes = sizeof(float) * numEntries * 2U;

    if (dataSizeInBytes + headerSize != fileSizeInBytes) {
        fprintf(stderr, "LUT '%s' has no valid data size!\n", filename);
        fclose(fptr);
        return false;
    }

    m_data = new float[numEntries * 2U];
    isOkay &= (fread(m_data, sizeof(float), numEntries * 2U, fptr) == numEntries * 2U);

    if (isOkay) {
        fprintf(stderr, "Successfully loaded LUT '%s' (Nr:%d, Nphi:%d).\n", filename, m_Nr, m_Nphi);
        m_texID[0] = genRGBAFloatTexture(m_Nphi, m_Nr, m_data);
        m_texID[1] = genRGBAFloatTexture(m_Nphi, m_Nr, &m_data[numEntries]);
    }
    else {
        fprintf(stderr, "Error reading LUT '%s'\n", filename);
    }

    fclose(fptr);
    return true;
}

size_t LUT::getFileSizeInBytes(const char* filename)
{
    struct stat stat_buf;
    int rc = stat(filename, &stat_buf);
    return (rc == 0 ? static_cast<size_t>(stat_buf.st_size) : static_cast<size_t>(0));
}

GLuint LUT::genRGBAFloatTexture(unsigned int width, unsigned int height, float* data)
{
    GLuint texID;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGBA,
        GL_FLOAT, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}