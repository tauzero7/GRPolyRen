/**
 * File:    VertexArray.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "VertexArray.h"
#include "Utilities.h"

#include <fstream>
#include <sstream>

VertexArray::VertexArray()
    : isDummy(true)
    , va(0)
    , vbo(nullptr)
    , vboType(nullptr)
    , vboUsage(nullptr)
    , m_sizeOfData(nullptr)
    , m_dim(nullptr)
    , numVertices(0)
    , numElements(0)
    , numVertexAttribs(0)
{
    if (!gladLoadGL()) {
        fprintf(stderr, "VertexArray: Failed to initialize GLAD.\n");
    }
}

VertexArray::~VertexArray()
{
    Delete();
    SafeDelete<GLuint>(vbo);
    SafeDelete<GLenum>(vboType);
    SafeDelete<GLenum>(vboUsage);
    SafeDelete<unsigned int>(m_sizeOfData);
    SafeDelete<unsigned int>(m_dim);
}

bool VertexArray::Create(const unsigned int numVerts)
{
    if (numVerts > 0) {
        isDummy = false;
    }

    numVertices = numVerts;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttrib);

    numVertexAttribs = static_cast<unsigned int>(maxVertexAttrib);
    if (isDummy) {
        numVertexAttribs = 1;
    }

    vbo = new GLuint[numVertexAttribs];
    vboType = new GLenum[numVertexAttribs];
    vboUsage = new GLenum[numVertexAttribs];
    m_sizeOfData = new unsigned int[numVertexAttribs];
    m_dim = new unsigned int[numVertexAttribs];

    for (unsigned int i = 0; i < numVertexAttribs; i++) {
        vbo[i] = 0;
        vboType[i] = GL_FLOAT;
        vboUsage[i] = GL_STATIC_DRAW;
        m_sizeOfData[i] = sizeof(float);
        m_dim[i] = 1;
    }

    glGenVertexArrays(1, &va);
    return va > 0;
}

void VertexArray::Clear()
{
    if (vbo != nullptr) {
        for (unsigned int i = 0; i < numVertexAttribs; i++) {
            if (glIsBuffer(vbo[i])) {
                glDeleteBuffers(1, &vbo[i]);
                vbo[i] = 0;
                glDisableVertexAttribArray(i);
            }
        }
    }
}

void VertexArray::Delete()
{
    if (vbo != nullptr) {
        for (unsigned int i = 0; i < numVertexAttribs; i++) {
            if (glIsBuffer(vbo[i])) {
                glDeleteBuffers(1, &vbo[i]);
                vbo[i] = 0;
                glDisableVertexAttribArray(i);
            }
        }
    }

    if (va > 0) {
        glDeleteVertexArrays(1, &va);
        va = 0;
    }

    SafeDelete<GLuint>(vbo);
    SafeDelete<GLenum>(vboType);
    SafeDelete<GLenum>(vboUsage);
    SafeDelete<unsigned int>(m_sizeOfData);
    SafeDelete<unsigned int>(m_dim);

    numVertices = 0;
}

bool VertexArray::IsDummy()
{
    return isDummy;
}

bool VertexArray::IsValid()
{
    return (numVertices > 0);
}

void VertexArray::Bind()
{
    if (va > 0) {
        glBindVertexArray(va);
    }
    else {
        fprintf(stderr, "Error in VertexArray::Bind ... not a vertex array\n");
    }
}

void VertexArray::BindBuffer(GLuint idx)
{
    if (idx >= numVertexAttribs) {
        fprintf(stderr, "Error in VertexArray::BindBuffer ... index out of range!\n");
        return;
    }

    if (glIsBuffer(vbo[idx])) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[idx]);
    }
}

void VertexArray::UnbindBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexArray::BindBufferBase(GLenum target, GLuint idx)
{
    if (idx >= numVertexAttribs) {
        fprintf(stderr, "Error in VertexArray::BindBufferBase ... index out of range!\n");
        return;
    }

    if (glIsBuffer(vbo[idx])) {
        glBindBufferBase(target, idx, vbo[idx]);
    }
}

bool VertexArray::SetArrayBuffer(GLuint idx, GLenum type, unsigned int dim, const void* data, GLenum usage)
{
    if (isDummy) {
        fprintf(stderr, "Error: it's a dummy VA: no array buffer can be set!\n");
        return false;
    }

    if (idx >= numVertexAttribs) {
        fprintf(stderr, "Error in VertexArray::CreateVBO() ... index out of range!\n");
        return false;
    }

    if (glIsBuffer(vbo[idx])) {
        glDeleteBuffers(1, &vbo[idx]);
        vbo[idx] = 0;
    }

    this->vboType[idx] = type;
    this->vboUsage[idx] = usage;
    m_dim[idx] = dim;

    switch (type) {
        default:
        case GL_BYTE: {
            m_sizeOfData[idx] = sizeof(char);
            break;
        }
        case GL_UNSIGNED_BYTE: {
            m_sizeOfData[idx] = sizeof(unsigned char);
            break;
        }
        case GL_SHORT: {
            m_sizeOfData[idx] = sizeof(short);
            break;
        }
        case GL_UNSIGNED_SHORT: {
            m_sizeOfData[idx] = sizeof(unsigned short);
            break;
        }
        case GL_INT: {
            m_sizeOfData[idx] = sizeof(int);
            break;
        }
        case GL_UNSIGNED_INT: {
            m_sizeOfData[idx] = sizeof(unsigned int);
            break;
        }
        case GL_FLOAT: {
            m_sizeOfData[idx] = sizeof(float);
            break;
        }
        case GL_DOUBLE: {
            m_sizeOfData[idx] = sizeof(double);
            break;
        }
    }

    glGenBuffers(1, &vbo[idx]);

    Bind();
    glBindBuffer(GL_ARRAY_BUFFER, vbo[idx]);
    glEnableVertexAttribArray(idx);
    glBufferData(GL_ARRAY_BUFFER, m_sizeOfData[idx] * numVertices * dim, data, usage);

    if (type == GL_BYTE || type == GL_UNSIGNED_BYTE || type == GL_SHORT || type == GL_UNSIGNED_SHORT || type == GL_INT
        || type == GL_UNSIGNED_INT) {
        glVertexAttribIPointer(idx, static_cast<GLint>(dim), type, 0, nullptr);
        // glVertexAttribPointer( idx, dim, type, GL_FALSE, 0, nullptr );
    }
    else if (type == GL_DOUBLE) {
        glVertexAttribLPointer(idx, static_cast<GLint>(dim), type, 0, nullptr);
    }
    else {
        glVertexAttribPointer(idx, static_cast<GLint>(dim), type, GL_FALSE, 0, nullptr);
    }
    Release();
    return true;
}

bool VertexArray::SetSubArrayBuffer(GLuint idx, size_t offset, size_t num, const void* data)
{
    if (offset > this->numVertices || offset + num > numVertices || idx >= static_cast<unsigned int>(maxVertexAttrib)) {
        return false;
    }

    if (!glIsBuffer(vbo[idx])) {
        return false;
    }

    GLintptr boffset = static_cast<GLintptr>(offset * m_sizeOfData[idx] * m_dim[idx]);
    GLsizeiptr bsize = static_cast<GLsizeiptr>(num * m_sizeOfData[idx] * m_dim[idx]);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[idx]);
    // glBufferSubData(GL_ARRAY_BUFFER, offset * m_sizeOfData[idx] * m_dim[idx],
    //    num * m_sizeOfData[idx] * m_dim[idx], data);
    glBufferSubData(GL_ARRAY_BUFFER, boffset, bsize, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

bool VertexArray::GetSubArrayBuffer(GLuint idx, unsigned int offset, unsigned int num, void* data)
{
    if (offset > this->numVertices || offset + num > numVertices || idx >= static_cast<unsigned int>(maxVertexAttrib)
        || data == nullptr) {
        return false;
    }

    if (!glIsBuffer(vbo[idx])) {
        return false;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo[idx]);
    glGetBufferSubData(
        GL_ARRAY_BUFFER, offset * m_sizeOfData[idx] * m_dim[idx], num * m_sizeOfData[idx] * m_dim[idx], data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

bool VertexArray::SetElementBuffer(GLuint idx, const unsigned int numElems, const void* data, GLenum usage)
{
    if (idx >= static_cast<unsigned int>(maxVertexAttrib)) {
        fprintf(stderr, "Error in VertexArray::CreateVBO() ... index out of range!\n");
        return false;
    }

    if (glIsBuffer(vbo[idx])) {
        glDeleteBuffers(1, &vbo[idx]);
        vbo[idx] = 0;
    }

    this->vboType[idx] = GL_UNSIGNED_INT;
    this->vboUsage[idx] = usage;

    glGenBuffers(1, &vbo[idx]);

    Bind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[idx]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElems * sizeof(GLuint), data, usage);
    Release();

    numElements = numElems;
    return true;
}

unsigned int VertexArray::GetMaxVertexAttribs()
{
    return static_cast<unsigned int>(maxVertexAttrib);
}

unsigned int VertexArray::GetDim(GLuint idx)
{
    if (static_cast<int>(idx) < maxVertexAttrib && m_dim != nullptr) {
        return m_dim[idx];
    }
    return 0;
}

unsigned int VertexArray::GetNumElements()
{
    return numElements;
}

unsigned int VertexArray::GetNumVertices()
{
    return numVertices;
}

void VertexArray::Release()
{
    glBindVertexArray(0);
}
