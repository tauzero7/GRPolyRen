/**
 * File:    GLShader.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_SHADER_H
#define GRPR_SHADER_H

#include "glad/glad.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>

class GLShader
{
public:
    enum class Type : int {
        None = 0,
        Vert = 1 << 0,
        Geom = 1 << 1,
        TCtrl = 1 << 2,
        TEval = 1 << 3,
        Frag = 1 << 4,
        Comp = 1 << 5
    };

public:
    GLShader();
    ~GLShader();

    /**
     * @brief Add string substitution.
     *  If "previous text" already exists, the previous substitute text is replaced by the new substitute text.
     * @param prevText   Previous text.
     * @param subsText   Substitute text.
     */
    void AddSubsStrings(const char* prevText, const char* subsText);

    /**
     * @brief Attach shader from file.
     *
     * @param shaderName  File name of shader.
     * @param shaderType  Type of shader (GL_VERTEX_SHADER, etc.).
     * @param linkProg    Link program after attaching shader.
     * @return false if program already exists, or shader could not be compiled, or program could not be linked.
     */
    bool AttachShaderFromFile(const char* shaderName, GLenum shaderType, bool linkProg = false);

    /**
     * @brief Attach shader from file.
     *
     * @param shaderName  File name of shader.
     * @param shaderType  Type of shader (GL_VERTEX_SHADER, etc.).
     * @param linkProg    Link program after attaching shader.
     * @param fptr  File pointer for log messages.
     */
    bool AttachShaderFromFileF(const char* shaderName, GLenum shaderType, bool linkProg, FILE* fptr = stderr);

    /**
     * @brief Attach shader from string.
     *
     * @param shaderText  String text of shader.
     * @param shaderLen   Length of shader text.
     * @param shaderType  Type of shader (GL_VERTEX_SHADER, etc.).
     * @param linkProg    Link program after attaching shader.
     * @return false if program already exists, or shader could not be compiled, or program could not be linked.
     */
    bool AttachShaderFromString(
        const char* shaderText, const size_t shaderLen, GLenum shaderType, bool linkProg = false);

    bool AttachShaderFromString(
        const char* shaderText, const size_t shaderLen, GLenum shaderType, bool linkProg, FILE* fptr = stderr);

    /// Attach vertex shader from file.
    bool AttachVertShaderFromFile(const char* shaderName, bool linkProg = false);

    /// Attach tessellation control shader from file.
    bool AttachTessCtrlShaderFromFile(const char* shaderName, bool linkProg = false);

    /// Attach tessellation evaluation shader from file.
    bool AttachTessEvalShaderFromFile(const char* shaderName, bool linkProg = false);

    /// Attach geometry shader from file.
    bool AttachGeomShaderFromFile(const char* shaderName, bool linkProg = false);

    /// Attach fragment shader from file.
    bool AttachFragShaderFromFile(const char* shaderName, bool linkProg = false);

    /// Bind shader program.
    bool Bind();

    /**
     * @brief Bind shader program attribute location.
     * @param attribIndex   Index of attribute.
     * @param attribName    Name of attribute.
     * @param linkProg      Link program after binding attribute.
     */
    bool BindAttribLocation(unsigned int attribIndex, const char* attribName, bool linkProg = false);

    /**
     * @brief Create compute shader program from file.
     *   Create shader program from file consisting only of a compute shader.
     *
     * @param cShaderName   File name of computer shader.
     * @param fptr  File pointer for log messages
     * @return
     */
    bool CreateCSProgramFromFile(const char* cShaderName, FILE* fptr = stderr);

    /**
     * @brief Create empty shader program.
     *   You have to attach shaders and link the program yourself.
     */
    void CreateEmptyProgram();

    /**
     * @brief Create shader program from file.
     *  Note that all filenames have to be defined in advance via Set methods.
     */
    bool CreateProgramFromFile();

    /**
     * @brief Create shader program from file.
     *   Create program from shaders given as files, where only vertex and
     *   fragment shader are used here.
     *
     * @param vShaderName  File name of vertex shader.
     * @param fShaderName  File name of fragment shader.
     * @param fptr  File pointer for log messages.
     * @return true if successfull
     */
    bool CreateProgramFromFile(const char* vShaderName, const char* fShaderName, FILE* fptr = stderr);

    /**
     * @brief Create shader program from file.
     *   Create program from shaders given as files, where only vertex, geometry,
     *   and fragment shader are used here.
     *
     * @param vShaderName  File name of vertex shader.
     * @param gShaderName  File name of geometry shader.
     * @param fShaderName  File name of fragment shader.
     * @param fptr  File pointer for log messages.
     */
    bool CreateProgramFromFile(
        const char* vShaderName, const char* gShaderName, const char* fShaderName, FILE* fptr = stderr);

    /**
     * @brief Create shader program from file.
     *   Create program from shaders given as files, where vertex, tessellation,
     *   control, tessellation evaluation, and fragment shader are used here.
     *
     * @param vShaderName  File name of vertex shader.
     * @param tcShaderName File name of tessellation control shader.
     * @param teShaderName File name of tessellation evaluation shader.
     * @param fShaderName  File name of fragment shader.
     * @param fptr  File pointer for log messages.
     */
    bool CreateProgramFromFile(const char* vShaderName, const char* tcShaderName, const char* teShaderName,
        const char* fShaderName, FILE* fptr = stderr);

    /**
     * @brief Create shader program from file.
     *   Create program from shaders given as files, where all shader types are
     *   used here.
     *
     * @param vShaderName  File name of vertex shader.
     * @param tcShaderName File name of tessellation control shader.
     * @param teShaderName File name of tessellation evaluation shader.
     * @param gShaderName  File name of geometry shader.
     * @param fShaderName  File name of fragment shader.
     * @param fptr  File pointer for log messages.
     */
    bool CreateProgramFromFile(const char* vShaderName, const char* tcShaderName, const char* teShaderName,
        const char* gShaderName, const char* fShaderName, FILE* fptr = stderr);

    /**
     * @brief Create shader program from strings.
     *
     * @param vShaderText  String text of vertex shader.
     * @param vShaderLen   Length of vertex shader text.
     * @param fShaderText  String text of fragment shader.
     * @param fShaderLen   Length of fragment shader text.
     * @param fptr  File pointer for log messages.
     */
    bool CreateProgramFromString(const char* vShaderText, const size_t vShaderLen, const char* fShaderText,
        const size_t fShaderLen, FILE* fptr = stderr);

    /**
     * @brief Create shader program from strings.
     *
     * @param vShaderText  String text of vertex shader.
     * @param vShaderLen   Length of vertex shader text.
     * @param gShaderText  String text of geometry shader.
     * @param gShaderLen   Length of geometry shader text.
     * @param fShaderText  String text of fragment shader.
     * @param fShaderLen   Length of fragment shader text.
     * @param fptr  File pointer for log messages.
     */
    bool CreateProgramFromString(const char* vShaderText, const size_t vShaderLen, const char* gShaderText,
        const size_t gShaderLen, const char* fShaderText, const size_t fShaderLen, FILE* fptr = stderr);

    /**
     * @brief Create shader program from strings.
     *
     * @param vShaderText  String text of vertex shader.
     * @param vShaderLen   Length of vertex shader text.
     * @param tcShaderText  String text of tessellation control shader.
     * @param tcShaderLen   Length of tessellation control shader text.
     * @param teShaderText  String text of tessellation evaluation shader.
     * @param teShaderLen   Length of tessellation evaluation shader text.
     * @param fShaderText  String text of fragment shader.
     * @param fShaderLen   Length of fragment shader text.
     * @param fptr  File pointer for log messages.
     */
    bool CreateProgramFromString(const char* vShaderText, const size_t vShaderLen, const char* tcShaderText,
        const size_t tcShaderLen, const char* teShaderText, const size_t teShaderLen, const char* fShaderText,
        const size_t fShaderLen, FILE* fptr = stderr);

    /**
     * @brief Create shader program from strings.
     *
     * @param vShaderText  String text of vertex shader.
     * @param vShaderLen   Length of vertex shader text.
     * @param tcShaderText  String text of tessellation control shader.
     * @param tcShaderLen   Length of tessellation control shader text.
     * @param teShaderText  String text of tessellation evaluation shader.
     * @param teShaderLen   Length of tessellation evaluation shader text.
     * @param gShaderText  String text of geometry shader.
     * @param gShaderLen   Length of geometry shader text.
     * @param fShaderText  String text of fragment shader.
     * @param fShaderLen   Length of fragment shader text.
     * @param fptr  File pointer for log messages.
     */
    bool CreateProgramFromString(const char* vShaderText, const size_t vShaderLen, const char* tcShaderText,
        const size_t tcShaderLen, const char* teShaderText, const size_t teShaderLen, const char* gShaderText,
        const size_t gShaderLen, const char* fShaderText, const size_t fShaderLen, FILE* fptr = stderr);

    /**
     * @brief GetAttribLocation
     * @param attribName
     * @return
     */
    GLint GetAttribLocation(const char* attribName);

    /// Get shader program handle.
    GLuint GetProgHandle();

    /**
     * @brief Get uniform block index
     * @param name uniform block name
     * @return
     */
    GLint GetUniformBlockIndex(const char* name);

    /**
     * @brief Get uniform location of shader variable.
     *
     * @param name  Uniform variable name
     */
    GLint GetUniformLocation(const char* name);

    /// Check if particular shader is available.
    bool Has(Type type);

    /// Test if shader program is valid
    bool IsValid();

    /**
     * @brief Link shader program.
     *   Linking is only necessary if you build the shader program yourself.
     */
    bool Link(FILE* fptr = stderr);

    /**
     * @brief print info
     * @param fptr
     */
    void PrintInfo(FILE* fptr = stderr);

    /**
     * @brief Remove all shaders from shader program.
     *   Call this method every time before reloading shaders.
     */
    void RemoveAllShaders();

    /**
     * @brief Reload shaders.
     *
     *  This works only if the shader filenames have been given in advance.
     *  It calls 'RemoveAllShaders' and 'CreateProgramFromFile()'.
     */
    bool ReloadShaders();

    /**
     * @brief Release shader program.
     *   That is equal to calling 'glUseProgram(0)'.
     */
    void Release();

    void ClearSubsStrings();

    /**
     * @brief Prepend the header text to every shader that is read from file.
     *
     * For example, this method can be used to prepend the version number of
     * GLSL which shall be used in the shader program ("#version 330").
     *
     * @param header  Header text.
     */
    void PrependHeaderText(const char* header);

    void SetAutomaticLinking(bool autoLinking);

    /**
     * @brief Set uniform float value.
     * @param uniformName  Name of uniform variable.
     * @param val
     */
    bool SetFloat(const char* uniformName, float val);
    void SetFloat(int uniformLoc, float val);

    /**
     * @brief Set uniform 2-float value.
     * @param uniformName  Name of uniform variable.
     * @param v1
     * @param v2
     */
    bool SetFloat(const char* uniformName, float v1, float v2);
    void SetFloat(int uniformLoc, float v1, float v2);

    /**
     * @brief Set uniform 3-float value.
     * @param uniformName  Name of uniform variable.
     * @param v1
     * @param v2
     * @param v3
     */
    bool SetFloat(const char* uniformName, float v1, float v2, float v3);
    void SetFloat(int uniformLoc, float v1, float v2, float v3);

    /**
     * @brief Set uniform 4-float value.
     * @param uniformName  Name of uniform variable.
     * @param v1
     * @param v2
     * @param v3
     * @param v4
     */
    bool SetFloat(const char* uniformName, float v1, float v2, float v3, float v4);
    void SetFloat(int uniformLoc, float v1, float v2, float v3, float v4);

    /**
     * @brief Set uniform float arrays via glUniform<N>fv.
     * @param uniformName
     * @param dim    Dimension (1,2,3,4)
     * @param count  Number of dim-arrays.
     * @param vals   Pointer to arrays.
     */
    bool SetFloatArray(const char* uniformName, unsigned int dim, unsigned int count, const float* vals);
    void SetFloatArray(int uniformLoc, unsigned int dim, unsigned int count, const float* vals);

    bool SetFloatMatrix(
        const char* uniformName, unsigned int dim, unsigned int count, bool transpose, const float* mat);

    void SetFloatMatrix(int uniformLoc, unsigned int dim, unsigned int count, bool transpose, const float* mat);

    /**
     * @brief Set uniform bool value mapped to either 0=false or 1=true.
     * @param uniformName  Name of uniform variable
     * @param val  Boolean
     * @return
     */
    bool SetBool(const char* uniformName, bool val);
    void SetBool(int uniformLoc, bool val);

    /**
     * @brief Set uniform int value.
     * @param uniformName  Name of uniform variable.
     * @param val
     */
    bool SetInt(const char* uniformName, int val);
    void SetInt(int uniformLoc, int val);

    /**
     * @brief Set uniform 2-int value.
     * @param uniformName  Name of uniform variable.
     * @param v1
     * @param v2
     */
    bool SetInt(const char* uniformName, int v1, int v2);
    void SetInt(int uniformLoc, int v1, int v2);

    /**
     * @brief Set uniform 3-int value.
     * @param uniformName  Name of uniform variable.
     * @param v1
     * @param v2
     * @param v3
     */
    bool SetInt(const char* uniformName, int v1, int v2, int v3);
    void SetInt(int uniformLoc, int v1, int v2, int v3);

    /**
     * @brief Set uniform 4-int value.
     * @param uniformName  Name of uniform variable.
     * @param v1
     * @param v2
     * @param v3
     * @param v4
     */
    bool SetInt(const char* uniformName, int v1, int v2, int v3, int v4);
    void SetInt(int uniformLoc, int v1, int v2, int v3, int v4);

    /**
     * @brief Set uniform int arrays.
     * @param uniformName  Name of uniform variable.
     * @param dim    Dimension (1,2,3,4)
     * @param count  Number of dim-arrays.
     * @param vals   Pointer to arrays.
     */
    bool SetIntArray(const char* uniformName, unsigned int dim, unsigned int count, const int* vals);
    void SetIntArray(int uniformLoc, unsigned int dim, unsigned int count, const int* vals);

    /**
     * @brief Set uniform unsigned int value.
     * @param uniformName  Name of uniform variable.
     * @param val
     */
    bool SetUInt(const char* uniformName, unsigned int val);
    void SetUInt(int uniformLoc, unsigned int val);

    /**
     * @brief Set uniform 2-unsigned int value.
     * @param uniformName  Name of uniform variable.
     * @param v1
     * @param v2
     */
    bool SetUInt(const char* uniformName, unsigned int v1, unsigned int v2);
    void SetUInt(int uniformLoc, unsigned int v1, unsigned int v2);

    /**
     * @brief Set uniform 3-unsigned int value.
     * @param uniformName  Name of uniform variable.
     * @param v1
     * @param v2
     * @param v3
     */
    bool SetUInt(const char* uniformName, unsigned int v1, unsigned int v2, unsigned int v3);
    void SetUInt(int uniformLoc, unsigned int v1, unsigned int v2, unsigned int v3);

    /**
     * @brief Set uniform 4-unsigned int value.
     * @param uniformName  Name of uniform variable.
     * @param v1
     * @param v2
     * @param v3
     * @param v4
     */
    bool SetUInt(const char* uniformName, unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4);
    void SetUInt(int uniformLoc, unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4);

    /**
     * @brief Set uniform unsigned int arrays.
     * @param uniformName  Name of uniform variable.
     * @param dim    Dimension (1,2,3,4)
     * @param count  Number of dim-arrays.
     * @param vals   Pointer to arrays.
     */
    bool SetUIntArray(const char* uniformName, unsigned int dim, unsigned int count, const unsigned int* vals);
    void SetUIntArray(int uniformLoc, unsigned int dim, unsigned int count, const unsigned int* vals);

    /**
     * @brief Set path to executable.
     *
     *  This might be needed in order to emulate the 'include' directive within a shader.
     * @param exePath
     */
    void SetExePath(const char* exePath);

    /**
     * @brief Set path to local folder.
     *   This is necessary in order to emulate the 'include' directive within a shader
     *   for a local shader path.
     * @param localPath
     */
    void SetLocalPath(const char* localPath);

    void SetFileName(Type type, const char* filename);
    void SetFileNames(const char* vertFilename, const char* fragFilename);
    void SetFileNames(const char* vertFilename, const char* geomFilename, const char* fragFilename);

    void SetVertFileName(const char* filename);
    void SetGeomFileName(const char* filename);
    void SetTCtrlFileName(const char* filename);
    void SetTEvalFileName(const char* filename);
    void SetFragFileName(const char* filename);
    void SetCompFileName(const char* filename);

    const char* GetFileName(Type type);

    void ClearFileNames();

protected:
    /**
     * @brief  Create shader from file.
     *   An 'include' directive will be replaced before shader is compiled.
     * @param shaderFilename  Filename of shader.
     * @param type  Type of shader (GL_VERTEX_SHADER, etc.).
     * @param fptr  File pointer for log messages.
     */
    GLuint createShaderFromFile(const char* shaderFilename, GLenum type, FILE* fptr = stderr);

    /**
     * @brief Create shader from string.
     *
     * @param shaderText  Shader text.
     * @param shaderLen   Length of shader text.
     * @param type        Shader type.
     * @param fptr   File pointer for log messages.
     * @return Shader id.
     */
    GLuint createShaderFromString(const char* shaderText, const size_t shaderLen, GLenum type, FILE* fptr = stderr);

    /// Get Name of shader type.
    const char* getShaderTypeName(GLint shaderType);

    /**
     * @brief Read shader from file.
     *
     * @param shaderFilename
     * @param shaderContent
     * @param fptr
     */
    size_t readShaderFromFile(const char* shaderFilename, std::string& shaderContent, FILE* fptr = stderr);

    /**
     * @brief Print shader information log.
     *
     * @param shader  Shader id.
     * @param fptr File pointer for log messages.
     */
    bool printShaderInfoLog(GLuint shader, FILE* fptr = stderr);

    /**
     * @brief Print program information log.
     * @param fptr File pointer for log messages.
     */
    bool printProgramInfoLog(FILE* fptr = stderr);

    void setFlag(Type type);

private:
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
    GLuint progHandle;
    bool automaticLinking;

    std::string headerText;
    std::map<std::string, std::string> subsStrings;
    std::map<std::string, std::string>::iterator subsStringsItr;
    std::string myExePath;
    std::string myLocalPath;

    std::string m_vertFileName;
    std::string m_geomFileName;
    std::string m_tcFileName;
    std::string m_teFileName;
    std::string m_fragFileName;
    std::string m_compFileName;

    /// Bit-field representing currently set shaders
    int m_type;
};

#endif // GRPR_SHADER_H
