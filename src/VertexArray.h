/**
 * File:    VertexArray.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_VERTEX_ARRAY_H
#define GRPR_VERTEX_ARRAY_H

#include "glad/glad.h"

#include <cstring>
#include <iostream>

class  VertexArray
{
public:
    /**
     * @brief Standard c'tor.
     *   The standard constructor does nothing - no initialization, no memory
     *   allocation or whatever.
     */
    VertexArray();

    /**
     * @brief D'tor.
     *   The destructor also calls the 'Delete()' method and clears all initialized
     *   buffer objects etc.
     */
    ~VertexArray();

    /// Bind vertex array.
    void Bind();

    void BindBuffer(GLuint idx);

    void BindBufferBase(GLenum target, GLuint idx);

    /**
     * @brief Create vertex array.
     *   If numVerts = 0, an empty dummy VA is created where no data can be
     *   stored. This dummy VA can be used to draw arrays without any geometry.
     *
     * @param numVerts :  Number of vertices to be stored in the VA
     */
    bool Create(const unsigned int numVerts);

    /// Delete vertex array.
    void Delete();

    void Clear();

    bool IsDummy();
    bool IsValid();

    /**
     * @brief Create vertex buffer object for vertices.     *
     * @param idx       Vertex attribute index.
     * @param type      Type of buffer object: GL_FLOAT,...
     * @param dim       Dimension of data entry: (2,3,4)
     * @param data      Pointer to data.
     * @param usage     Usage of vertex buffer
     */
    bool SetArrayBuffer(GLuint idx, GLenum type, unsigned int dim, const void* data, GLenum usage = GL_STATIC_DRAW);

    bool SetElementBuffer(GLuint idx, const unsigned int numElems, const void* data, GLenum usage = GL_STATIC_DRAW);

    /**
     * @brief Update array buffer.
     *
     *  For example, if there are 7 vertices with 4 coordinate values each,
     *  and 'data' is a 8-array containing vertices 4 and 5, then
     *  'offset' = 4 and 'num' = 2.
     *
     * @param idx        Vertex attribute index.
     * @param offset     Offset in numbers (takes dim of array into account).
     * @param num        Number of elements (takes dim of array into account).
     * @param data       pointer to data.
     */
    bool SetSubArrayBuffer(GLuint idx, size_t offset, size_t num, const void* data);

    /**
     * @brief Get part of array buffer.
     *
     * @param idx        Vertex attribute index.
     * @param offset     Offset in numbers.
     * @param num        Number of elements.
     * @param data       pointer to data.
     */
    bool GetSubArrayBuffer(GLuint idx, unsigned int offset, unsigned int num, void* data);

    /// Get maximum number of available vertex attributes.
    unsigned int GetMaxVertexAttribs();

    unsigned int GetDim(GLuint idx);

    /// Get number of vertices stored in vertex array object.
    unsigned int GetNumVertices();

    /// Get number of elements.
    unsigned int GetNumElements();

    /// Release vertex array.
    void Release();

    void UnbindBuffer();

    template <typename T> T GetNumVertices() { return static_cast<T>(numVertices); }
    template <typename T> T GetNumElements() { return static_cast<T>(numElements); }

protected:
    void deleteAll();

private:
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
    /// Is VA a dummy?
    bool isDummy;

    /// Handle to vertex array.
    GLuint va;

    /// List of VBOs
    GLuint* vbo;

    /// List of types of vertex data (GL_FLOAT,...)
    GLenum* vboType;

    /// List of usage types.
    GLenum* vboUsage;

    /// List of data sizes (sizeof(float),...)
    unsigned int* m_sizeOfData;
    unsigned int* m_dim;

    /// Number of vertices.
    unsigned int numVertices;
    unsigned int numElements;

    /// Maximum number of vertex attributes
    GLint maxVertexAttrib;

    /// Number of vertex attributes
    GLuint numVertexAttribs;
};

#endif // GRPR_VERTEX_ARRAY_H
