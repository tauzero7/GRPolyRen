/**
 * File:    GLShader.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "GLShader.h"
#include "Utilities.h"

#include <fstream>
#include <regex>
#include <sstream>

GLShader::GLShader()
    : progHandle(0)
    , automaticLinking(true)
    , headerText("")
    , myExePath("")
    , myLocalPath("")
    , m_vertFileName("")
    , m_geomFileName("")
    , m_tcFileName("")
    , m_teFileName("")
    , m_fragFileName("")
    , m_compFileName("")
    , m_type(0)
{
    if (!gladLoadGL()) {
        fprintf(stderr, "GLShader: Failed to initialize GLAD.\n");
    }
}

GLShader::~GLShader()
{
    RemoveAllShaders();
    m_type = 0;
}

bool GLShader::AttachShaderFromFile(const char* shaderName, GLenum shaderType, bool linkProg)
{
    return AttachShaderFromFileF(shaderName, shaderType, linkProg, stderr);
}

bool GLShader::AttachVertShaderFromFile(const char* shaderName, bool linkProg)
{
    return AttachShaderFromFile(shaderName, GL_VERTEX_SHADER, linkProg);
}

bool GLShader::AttachFragShaderFromFile(const char* shaderName, bool linkProg)
{
    return AttachShaderFromFile(shaderName, GL_FRAGMENT_SHADER, linkProg);
}

bool GLShader::AttachGeomShaderFromFile(const char* shaderName, bool linkProg)
{
    return AttachShaderFromFile(shaderName, GL_GEOMETRY_SHADER, linkProg);
}

bool GLShader::AttachTessCtrlShaderFromFile(const char* shaderName, bool linkProg)
{
    return AttachShaderFromFile(shaderName, GL_TESS_CONTROL_SHADER, linkProg);
}

bool GLShader::AttachTessEvalShaderFromFile(const char* shaderName, bool linkProg)
{
    return AttachShaderFromFile(shaderName, GL_TESS_EVALUATION_SHADER, linkProg);
}

bool GLShader::AttachShaderFromFileF(const char* shaderName, GLenum shaderType, bool linkProg, FILE* fptr)
{
    if (progHandle == 0) {
        return false;
    }

    GLuint shaderHandle = createShaderFromFile(shaderName, shaderType, fptr);
    if (shaderHandle == 0) {
        return false;
    }

    glAttachShader(progHandle, shaderHandle);
    if (linkProg) {
        return Link();
    }
    return true;
}

bool GLShader::AttachShaderFromString(const char* shaderText, const size_t shaderLen, GLenum shaderType, bool linkProg)
{
    return AttachShaderFromString(shaderText, shaderLen, shaderType, linkProg, stderr);
}

bool GLShader::AttachShaderFromString(
    const char* shaderText, const size_t shaderLen, GLenum shaderType, bool linkProg, FILE* fptr)
{
    if (progHandle == 0) {
        return false;
    }

    GLuint shaderHandle = createShaderFromString(shaderText, shaderLen, shaderType, fptr);
    if (shaderHandle == 0) {
        return false;
    }

    glAttachShader(progHandle, shaderHandle);
    if (linkProg) {
        return Link();
    }
    return true;
}

bool GLShader::Bind()
{
    if (!glIsProgram(progHandle)) {
        return false;
    }

    glUseProgram(progHandle);
    return true;
}

bool GLShader::BindAttribLocation(unsigned int attribIndex, const char* attribName, bool linkProg)
{
    glBindAttribLocation(progHandle, attribIndex, attribName);
    if (linkProg) {
        return Link();
    }
    return true;
}

bool GLShader::CreateCSProgramFromFile(const char* cShaderName, FILE* fptr)
{
    CreateEmptyProgram();
    return AttachShaderFromFileF(cShaderName, GL_COMPUTE_SHADER, true, fptr);
}

void GLShader::CreateEmptyProgram()
{
    if (progHandle != 0) {
        RemoveAllShaders();
    }
    progHandle = glCreateProgram();
}

bool GLShader::CreateProgramFromFile()
{
    int vf = static_cast<int>(Type::Vert) | static_cast<int>(Type::Frag);
    if (m_type == vf) {
        return CreateProgramFromFile(m_vertFileName.c_str(), m_fragFileName.c_str());
    }

    int vgf = vf | static_cast<int>(Type::Geom);
    if (m_type == vgf) {
        return CreateProgramFromFile(m_vertFileName.c_str(), m_geomFileName.c_str(), m_fragFileName.c_str());
    }

    int vttf = vf | static_cast<int>(Type::TCtrl) | static_cast<int>(Type::TEval);
    if (m_type == vttf) {
        return CreateProgramFromFile(
            m_vertFileName.c_str(), m_tcFileName.c_str(), m_teFileName.c_str(), m_fragFileName.c_str());
    }

    int vttgf = vttf | static_cast<int>(Type::Geom);
    if (m_type == vttgf) {
        return CreateProgramFromFile(m_vertFileName.c_str(), m_tcFileName.c_str(), m_teFileName.c_str(),
            m_geomFileName.c_str(), m_fragFileName.c_str());
    }

    int c = static_cast<int>(Type::Comp);
    if (m_type == c) {
        return CreateCSProgramFromFile(m_compFileName.c_str());
    }

    return false;
}

bool GLShader::CreateProgramFromFile(const char* vShaderName, const char* fShaderName, FILE* fptr)
{
    GLuint vShader = createShaderFromFile(vShaderName, GL_VERTEX_SHADER, fptr);
    // fprintf(fptr,"vshader is %d\n",vShader);
    if (vShader == 0)
        return false;

    GLuint fShader = createShaderFromFile(fShaderName, GL_FRAGMENT_SHADER, fptr);
    // fprintf(fptr,"fshader is %d\n",fShader);
    if (fShader == 0)
        return false;

    progHandle = glCreateProgram();
    // fprintf(fptr,"progID: %d\n",progHandle);
    glAttachShader(progHandle, vShader);
    glAttachShader(progHandle, fShader);

    setFlag(Type::Vert);
    setFlag(Type::Frag);

    if (automaticLinking) {
        return Link(fptr);
    }
    return true;
}

bool GLShader::CreateProgramFromFile(
    const char* vShaderName, const char* gShaderName, const char* fShaderName, FILE* fptr)
{
    GLuint vShader = createShaderFromFile(vShaderName, GL_VERTEX_SHADER, fptr);
    if (vShader == 0)
        return false;

    GLuint gShader = createShaderFromFile(gShaderName, GL_GEOMETRY_SHADER, fptr);
    if (gShader == 0)
        return false;

    GLuint fShader = createShaderFromFile(fShaderName, GL_FRAGMENT_SHADER, fptr);
    if (fShader == 0)
        return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader);
    glAttachShader(progHandle, gShader);
    glAttachShader(progHandle, fShader);

    setFlag(Type::Vert);
    setFlag(Type::Geom);
    setFlag(Type::Frag);

    if (automaticLinking) {
        return Link();
    }
    return true;
}

bool GLShader::CreateProgramFromFile(
    const char* vShaderName, const char* tcShaderName, const char* teShaderName, const char* fShaderName, FILE* fptr)
{
    GLuint vShader = createShaderFromFile(vShaderName, GL_VERTEX_SHADER, fptr);
    if (vShader == 0)
        return false;

    GLuint tcShader = createShaderFromFile(tcShaderName, GL_TESS_CONTROL_SHADER, fptr);
    if (tcShader == 0)
        return false;

    GLuint teShader = createShaderFromFile(teShaderName, GL_TESS_EVALUATION_SHADER, fptr);
    if (teShader == 0)
        return false;

    GLuint fShader = createShaderFromFile(fShaderName, GL_FRAGMENT_SHADER, fptr);
    if (fShader == 0)
        return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader);
    glAttachShader(progHandle, tcShader);
    glAttachShader(progHandle, teShader);
    glAttachShader(progHandle, fShader);

    setFlag(Type::Vert);
    setFlag(Type::TCtrl);
    setFlag(Type::TEval);
    setFlag(Type::Frag);

    if (automaticLinking) {
        return Link();
    }
    return true;
}

bool GLShader::CreateProgramFromFile(const char* vShaderName, const char* tcShaderName, const char* teShaderName,
    const char* gShaderName, const char* fShaderName, FILE* fptr)
{
    GLuint vShader = createShaderFromFile(vShaderName, GL_VERTEX_SHADER, fptr);
    if (vShader == 0)
        return false;

    GLuint tcShader = createShaderFromFile(tcShaderName, GL_TESS_CONTROL_SHADER, fptr);
    if (tcShader == 0)
        return false;

    GLuint teShader = createShaderFromFile(teShaderName, GL_TESS_EVALUATION_SHADER, fptr);
    if (teShader == 0)
        return false;

    GLuint gShader = createShaderFromFile(gShaderName, GL_GEOMETRY_SHADER, fptr);
    if (gShader == 0)
        return false;

    GLuint fShader = createShaderFromFile(fShaderName, GL_FRAGMENT_SHADER, fptr);
    if (fShader == 0)
        return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader);
    glAttachShader(progHandle, tcShader);
    glAttachShader(progHandle, teShader);
    glAttachShader(progHandle, gShader);
    glAttachShader(progHandle, fShader);

    setFlag(Type::Vert);
    setFlag(Type::TCtrl);
    setFlag(Type::TEval);
    setFlag(Type::Geom);
    setFlag(Type::Frag);

    if (automaticLinking) {
        return Link();
    }
    return true;
}

bool GLShader::CreateProgramFromString(
    const char* vShaderText, const size_t vShaderLen, const char* fShaderText, const size_t fShaderLen, FILE* fptr)
{
    GLuint vShader = createShaderFromString(vShaderText, vShaderLen, GL_VERTEX_SHADER, fptr);
    if (vShader == 0)
        return false;

    GLuint fShader = createShaderFromString(fShaderText, fShaderLen, GL_FRAGMENT_SHADER, fptr);
    if (fShader == 0)
        return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader);
    glAttachShader(progHandle, fShader);

    if (automaticLinking) {
        return Link(fptr);
    }
    return true;
}

bool GLShader::CreateProgramFromString(const char* vShaderText, const size_t vShaderLen, const char* gShaderText,
    const size_t gShaderLen, const char* fShaderText, const size_t fShaderLen, FILE* fptr)
{
    GLuint vShader = createShaderFromString(vShaderText, vShaderLen, GL_VERTEX_SHADER, fptr);
    if (vShader == 0)
        return false;

    GLuint gShader = createShaderFromString(gShaderText, gShaderLen, GL_GEOMETRY_SHADER, fptr);
    if (gShader == 0)
        return false;

    GLuint fShader = createShaderFromString(fShaderText, fShaderLen, GL_FRAGMENT_SHADER, fptr);
    if (fShader == 0)
        return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader);
    glAttachShader(progHandle, gShader);
    glAttachShader(progHandle, fShader);

    setFlag(Type::Vert);
    setFlag(Type::Geom);
    setFlag(Type::Frag);

    if (automaticLinking) {
        return Link(fptr);
    }
    return true;
}

bool GLShader::CreateProgramFromString(const char* vShaderText, const size_t vShaderLen, const char* tcShaderText,
    const size_t tcShaderLen, const char* teShaderText, const size_t teShaderLen, const char* fShaderText,
    const size_t fShaderLen, FILE* fptr)
{
    GLuint vShader = createShaderFromString(vShaderText, vShaderLen, GL_VERTEX_SHADER, fptr);
    if (vShader == 0)
        return false;

    GLuint tcShader = createShaderFromString(tcShaderText, tcShaderLen, GL_TESS_CONTROL_SHADER, fptr);
    if (tcShader == 0)
        return false;

    GLuint teShader = createShaderFromString(teShaderText, teShaderLen, GL_TESS_EVALUATION_SHADER, fptr);
    if (teShader == 0)
        return false;

    GLuint fShader = createShaderFromString(fShaderText, fShaderLen, GL_FRAGMENT_SHADER, fptr);
    if (fShader == 0)
        return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader);
    glAttachShader(progHandle, tcShader);
    glAttachShader(progHandle, teShader);
    glAttachShader(progHandle, fShader);

    setFlag(Type::Vert);
    setFlag(Type::TCtrl);
    setFlag(Type::TEval);
    setFlag(Type::Frag);

    if (automaticLinking) {
        return Link(fptr);
    }
    return true;
}

bool GLShader::CreateProgramFromString(const char* vShaderText, const size_t vShaderLen, const char* tcShaderText,
    const size_t tcShaderLen, const char* teShaderText, const size_t teShaderLen, const char* gShaderText,
    const size_t gShaderLen, const char* fShaderText, const size_t fShaderLen, FILE* fptr)
{
    GLuint vShader = createShaderFromString(vShaderText, vShaderLen, GL_VERTEX_SHADER, fptr);
    if (vShader == 0)
        return false;

    GLuint tcShader = createShaderFromString(tcShaderText, tcShaderLen, GL_TESS_CONTROL_SHADER, fptr);
    if (tcShader == 0)
        return false;

    GLuint teShader = createShaderFromString(teShaderText, teShaderLen, GL_TESS_EVALUATION_SHADER, fptr);
    if (teShader == 0)
        return false;

    GLuint gShader = createShaderFromString(gShaderText, gShaderLen, GL_GEOMETRY_SHADER, fptr);
    if (gShader == 0)
        return false;

    GLuint fShader = createShaderFromString(fShaderText, fShaderLen, GL_FRAGMENT_SHADER, fptr);
    if (fShader == 0)
        return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader);
    glAttachShader(progHandle, tcShader);
    glAttachShader(progHandle, teShader);
    glAttachShader(progHandle, gShader);
    glAttachShader(progHandle, fShader);

    setFlag(Type::Vert);
    setFlag(Type::TCtrl);
    setFlag(Type::TEval);
    setFlag(Type::Geom);
    setFlag(Type::Frag);

    if (automaticLinking) {
        return Link(fptr);
    }
    return true;
}

void GLShader::Release()
{
    glUseProgram(0);
}

GLint GLShader::GetAttribLocation(const char* attribName)
{
    return glGetAttribLocation(progHandle, attribName);
}

GLuint GLShader::GetProgHandle()
{
    return progHandle;
}

GLint GLShader::GetUniformBlockIndex(const char* name)
{
    return glGetUniformLocation(progHandle, name);
}

GLint GLShader::GetUniformLocation(const char* name)
{
    return glGetUniformLocation(progHandle, name);
}

bool GLShader::Has(Type type)
{
    int itype = static_cast<int>(type);
    return ((m_type & itype) == itype);
}

bool GLShader::IsValid()
{
    // return static_cast<bool>(glIsProgram(progHandle));
    return (glIsProgram(progHandle) == 1);
}

bool GLShader::Link(FILE* fptr)
{
    glLinkProgram(progHandle);
    bool status = printProgramInfoLog(fptr);
    glUseProgram(0);
    return status;
}

void GLShader::PrintInfo(FILE*)
{
    if (progHandle > 0) {
#if 0
        GLsizei maxCount = static_cast<GLsizei>(5);
        GLsizei count; 
        GLuint  *shaders = new GLuint[5];
        glGetAttachedShaders( progHandle, maxCount, &count, shaders );
        for(int i=0; i<static_cast<int>(count); i++) {
            printShaderInfoLog( shaders[i], fptr );
        }

        printProgramInfoLog(fptr);
#endif
    }
}

size_t GLShader::readShaderFromFile(const char* shaderFilename, std::string& shaderContent, FILE* fptr)
{
#ifndef _WIN32
    // WinQ: Does this work on Windows??
    if (shaderFilename[0] == '\0') {
        fprintf(stderr, "GLShader::readShaderFromFile() ... no filename given!\n");
        return 0;
    }
#endif
    std::ifstream in(shaderFilename);
    if (in.bad() || in.fail()) {
        const unsigned int length = 256;
        char msg[length];
#ifdef _WIN32
        sprintf_s(msg, length, "GLShader::readShaderFromFile() ... Cannot open file \"%s\"", shaderFilename);
#else
        sprintf(msg, "GLShader::readShaderFromFile() ... Cannot open file \"%s\"", shaderFilename);
#endif
        fprintf(fptr, "Error: %s\n", msg);
        return 0;
    }

    std::stringstream shaderData;
    shaderData << in.rdbuf();
    in.close();
    shaderContent = shaderData.str();
    // fprintf(fptr,"%s\n",shaderContent.c_str());
    return shaderContent.size();
}

GLuint GLShader::createShaderFromFile(const char* shaderFilename, GLenum type, FILE* fptr)
{
    if (shaderFilename == nullptr) {
        return 0;
    }

    std::string shaderText;
    size_t iShaderLen = readShaderFromFile(shaderFilename, shaderText, fptr);
    if (iShaderLen == std::string::npos || iShaderLen == 0) {
        return 0;
    }

    // -------------------------------
    //  emulate '#include' directive
    // -------------------------------
    std::smatch re_match;

#ifdef __APPLE__
    // really correct?
    std::regex re_fname("#include\\s*<([A-Za-z_\\.\\/]+)>");
#else
    std::regex re_fname("#include\\s*<([A-Za-z\\_\\.\\/]+)>");
#endif

    std::string nShaderText = shaderText;

    std::string::const_iterator search_start(shaderText.begin());
    while (std::regex_search(search_start, shaderText.cend(), re_match, re_fname)) {
        if (re_match.size() > 1) {
            std::string include_filename = re_match[1];
            include_filename = myExePath + include_filename;

            std::string include_local_filename = re_match[1];
            include_local_filename = myLocalPath + include_local_filename;
            if (FileExists(include_local_filename.c_str())) {
                include_filename = include_local_filename;
            }

            std::string include_text;
            size_t nShaderLen = readShaderFromFile(include_filename.c_str(), include_text);
            if (nShaderLen != std::string::npos && nShaderLen > 0) {
                nShaderText
                    = std::regex_replace(nShaderText, re_fname, include_text, std::regex_constants::format_first_only);
                iShaderLen += nShaderLen;
            }
        }
        search_start = re_match.suffix().first;
    }

    shaderText = nShaderText;
    // fprintf(stderr, "\n********************\n%s", shaderText.c_str());

    // -------------------------------
    //  replace substrings
    // -------------------------------
    for (subsStringsItr = subsStrings.begin(); subsStringsItr != subsStrings.end(); ++subsStringsItr) {
        size_t pos = 0;
        std::string phText = subsStringsItr->first;
        std::string subsText = subsStringsItr->second;
        while (pos != std::string::npos) {
            pos = shaderText.find(phText, pos);
            if (pos != std::string::npos) {
                shaderText.replace(pos, phText.length(), subsText);
                pos += phText.length();
                iShaderLen = shaderText.size();
            }
        }
    }

    // -------------------------------
    //  prepend header
    // -------------------------------
    if (!headerText.empty()) {
        shaderText = headerText + "\n" + shaderText;
    }
    iShaderLen = shaderText.size();

    GLuint shader = glCreateShader(type);
    const char* strShaderVar = shaderText.c_str();
    glShaderSource(shader, 1, reinterpret_cast<const GLchar**>(&strShaderVar), reinterpret_cast<GLint*>(&iShaderLen));
    glCompileShader(shader);

    if (!printShaderInfoLog(shader, fptr)) {
        std::stringstream iss(shaderText);
        std::string sLine;
        int lineCounter = 1;
        fprintf(fptr, "====================================================\n");
        while (std::getline(iss, sLine)) {
            fprintf(fptr, "%4d : %s\n", (lineCounter++), sLine.c_str());
        }
        fprintf(fptr, "====================================================\n");
        return 0;
    }
    return shader;
}

GLuint GLShader::createShaderFromString(const char* shaderText, const size_t shaderLen, GLenum type, FILE* fptr)
{
    if (shaderLen == 0) {
        return 0;
    }

    GLuint shader = glCreateShader(type);
    glShaderSource(
        shader, 1, reinterpret_cast<const GLchar**>(&shaderText), reinterpret_cast<const GLint*>(&shaderLen));
    glCompileShader(shader);

    if (!printShaderInfoLog(shader, fptr)) {
        std::stringstream iss(shaderText);
        std::string sLine;
        int lineCounter = 1;
        while (std::getline(iss, sLine)) {
            fprintf(fptr, "%4d : %s\n", (lineCounter++), sLine.c_str());
        }
        return 0;
    }
    return shader;
}

const char* GLShader::getShaderTypeName(GLint shaderType)
{
    if (shaderType == GL_VERTEX_SHADER) {
        return "VertexShader";
    }
    else if (shaderType == GL_GEOMETRY_SHADER) {
        return "GeometryShader";
    }
    return nullptr;
}

bool GLShader::printShaderInfoLog(GLuint shader, FILE* fptr)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    int shaderType = 0;

    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
    glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);

    if (!compileStatus && infoLogLen > 1) {
        GLchar* infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);

        const char* shaderTypeName = getShaderTypeName(shaderType);
        if (shaderTypeName != nullptr) {
            fprintf(fptr, "InfoLog for %s:\n\t%s\n", shaderTypeName, infoLog);
        }
        else {
            fprintf(fptr, "InfoLog: %s\n", infoLog);
        }
        delete[] infoLog;
        return false;
    }
    return (compileStatus == GL_TRUE);
}

bool GLShader::printProgramInfoLog(FILE* fptr)
{
    int infoLogLen = 0;
    int charsWritten = 0;

    GLint linkStatus;
    glGetProgramiv(progHandle, GL_INFO_LOG_LENGTH, &infoLogLen);
    glGetProgramiv(progHandle, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_FALSE && infoLogLen > 1) {
        GLchar* infoLog = new GLchar[infoLogLen];

        // error check for fail to allocate memory omitted
        glGetProgramInfoLog(progHandle, infoLogLen, &charsWritten, infoLog);
        fprintf(fptr, "\nProgramInfoLog :\n\n%s\n", infoLog);
        delete[] infoLog;
        return false;
    }
    return (linkStatus == GL_TRUE);
}

void GLShader::setFlag(Type type)
{
    int iflag = static_cast<int>(type);
    m_type |= iflag;
}

void GLShader::RemoveAllShaders()
{
    if (progHandle == 0) {
        return;
    }

    if (!glIsProgram(progHandle)) {
        return;
    }

    const GLsizei numShaders = 1024;
    GLsizei numReturned;
    GLuint shaders[numShaders];
    glUseProgram(0);

    glGetAttachedShaders(progHandle, numShaders, &numReturned, shaders);
    for (GLsizei i = 0; i < numReturned; i++) {
        glDetachShader(progHandle, shaders[i]);
        glDeleteShader(shaders[i]);
    }
    glDeleteProgram(progHandle);
    progHandle = 0;
}

bool GLShader::ReloadShaders()
{
    RemoveAllShaders();
    return CreateProgramFromFile();
}

void GLShader::ClearSubsStrings()
{
    subsStrings.clear();
}

void GLShader::AddSubsStrings(const char* prevText, const char* subsText)
{
    subsStringsItr = subsStrings.find(std::string(prevText));
    if (subsStringsItr != subsStrings.end()) {
        subsStringsItr->second = std::string(subsText);
    }
    else {
        std::pair<std::string, std::string> subs
            = std::pair<std::string, std::string>(std::string(prevText), std::string(subsText));
        subsStrings.insert(subs);
    }
}

void GLShader::PrependHeaderText(const char* header)
{
    headerText = std::string(header);
}

void GLShader::SetAutomaticLinking(bool autoLinking)
{
    automaticLinking = autoLinking;
}

bool GLShader::SetFloat(const char* uniformName, float val)
{
    GLint loc = this->GetUniformLocation(uniformName);
    glUniform1f(loc, val);
    return (loc >= 0);
}

void GLShader::SetFloat(int uniformLoc, float val)
{
    glUniform1f(uniformLoc, val);
}

bool GLShader::SetFloat(const char* uniformName, float v1, float v2)
{
    GLint loc = this->GetUniformLocation(uniformName);
    glUniform2f(loc, v1, v2);
    return (loc >= 0);
}

void GLShader::SetFloat(int uniformLoc, float v1, float v2)
{
    glUniform2f(uniformLoc, v1, v2);
}

bool GLShader::SetFloat(const char* uniformName, float v1, float v2, float v3)
{
    GLint loc = this->GetUniformLocation(uniformName);
    glUniform3f(loc, v1, v2, v3);
    return (loc >= 0);
}

void GLShader::SetFloat(int uniformLoc, float v1, float v2, float v3)
{
    glUniform3f(uniformLoc, v1, v2, v3);
}

bool GLShader::SetFloat(const char* uniformName, float v1, float v2, float v3, float v4)
{
    GLint loc = this->GetUniformLocation(uniformName);
    glUniform4f(loc, v1, v2, v3, v4);
    return (loc >= 0);
}

void GLShader::SetFloat(int uniformLoc, float v1, float v2, float v3, float v4)
{
    glUniform4f(uniformLoc, v1, v2, v3, v4);
}

bool GLShader::SetFloatArray(const char* uniformName, unsigned int dim, unsigned int count, const float* vals)
{
    GLint loc = this->GetUniformLocation(uniformName);
    if (loc >= 0) {
        SetFloatArray(loc, dim, count, vals);
    }
    return (loc >= 0);
}

void GLShader::SetFloatArray(int uniformLoc, unsigned int dim, unsigned int count, const float* vals)
{
    switch (dim) {
        case 1: {
            glUniform1fv(uniformLoc, static_cast<GLsizei>(count), vals);
            break;
        }
        case 2: {
            glUniform2fv(uniformLoc, static_cast<GLsizei>(count), vals);
            break;
        }
        case 3: {
            glUniform3fv(uniformLoc, static_cast<GLsizei>(count), vals);
            break;
        }
        case 4: {
            glUniform4fv(uniformLoc, static_cast<GLsizei>(count), vals);
            break;
        }
    }
}

bool GLShader::SetFloatMatrix(
    const char* uniformName, unsigned int dim, unsigned int count, bool transpose, const float* mat)
{
    GLint loc = this->GetUniformLocation(uniformName);
    if (loc >= 0) {
        SetFloatMatrix(loc, dim, count, transpose, mat);
    }
    return (loc >= 0);
}

void GLShader::SetFloatMatrix(int uniformLoc, unsigned int dim, unsigned int count, bool transpose, const float* mat)
{
    if (mat == nullptr) {
        return;
    }

    switch (dim) {
        case 2: {
            glUniformMatrix2fv(uniformLoc, static_cast<GLsizei>(count), transpose, mat);
            break;
        }
        case 3: {
            glUniformMatrix3fv(uniformLoc, static_cast<GLsizei>(count), transpose, mat);
            break;
        }
        case 4: {
            glUniformMatrix4fv(uniformLoc, static_cast<GLsizei>(count), transpose, mat);
            break;
        }
    }
}

bool GLShader::SetBool(const char* uniformName, bool val)
{
    GLint loc = this->GetUniformLocation(uniformName);
    glUniform1i(loc, (val ? 1 : 0));
    return (loc >= 0);
}

void GLShader::SetBool(int uniformLoc, bool val)
{
    glUniform1i(uniformLoc, (val ? 1 : 0));
}

bool GLShader::SetInt(const char* uniformName, int val)
{
    GLint loc = this->GetUniformLocation(uniformName);
    glUniform1i(loc, val);
    return (loc >= 0);
}

void GLShader::SetInt(int uniformLoc, int val)
{
    glUniform1i(uniformLoc, val);
}

bool GLShader::SetInt(const char* uniformName, int v1, int v2)
{
    GLint loc = this->GetUniformLocation(uniformName);
    glUniform2i(loc, v1, v2);
    return (loc >= 0);
}

void GLShader::SetInt(int uniformLoc, int v1, int v2)
{
    glUniform2i(uniformLoc, v1, v2);
}

bool GLShader::SetInt(const char* uniformName, int v1, int v2, int v3)
{
    GLint loc = this->GetUniformLocation(uniformName);
    glUniform3i(loc, v1, v2, v3);
    return (loc >= 0);
}

void GLShader::SetInt(int uniformLoc, int v1, int v2, int v3)
{
    glUniform3i(uniformLoc, v1, v2, v3);
}

bool GLShader::SetInt(const char* uniformName, int v1, int v2, int v3, int v4)
{
    GLint loc = this->GetUniformLocation(uniformName);
    glUniform4i(loc, v1, v2, v3, v4);
    return (loc >= 0);
}

void GLShader::SetInt(int uniformLoc, int v1, int v2, int v3, int v4)
{
    glUniform4i(uniformLoc, v1, v2, v3, v4);
}

bool GLShader::SetIntArray(const char* uniformName, unsigned int dim, unsigned int count, const int* vals)
{
    GLint loc = this->GetUniformLocation(uniformName);
    if (loc >= 0) {
        SetIntArray(loc, dim, count, vals);
    }
    return (loc >= 0);
}

void GLShader::SetIntArray(int uniformLoc, unsigned int dim, unsigned int count, const int* vals)
{
    switch (dim) {
        case 1: {
            glUniform1iv(uniformLoc, static_cast<GLsizei>(count), vals);
            break;
        }
        case 2: {
            glUniform2iv(uniformLoc, static_cast<GLsizei>(count), vals);
            break;
        }
        case 3: {
            glUniform3iv(uniformLoc, static_cast<GLsizei>(count), vals);
            break;
        }
        case 4: {
            glUniform4iv(uniformLoc, static_cast<GLsizei>(count), vals);
            break;
        }
    }
}

bool GLShader::SetUInt(const char* uniformName, unsigned int val)
{
    GLint loc = this->GetUniformLocation(uniformName);
    glUniform1ui(loc, val);
    return (loc >= 0);
}

void GLShader::SetUInt(int uniformLoc, unsigned int val)
{
    glUniform1ui(uniformLoc, val);
}

bool GLShader::SetUInt(const char* uniformName, unsigned int v1, unsigned int v2)
{
    GLint loc = this->GetUniformLocation(uniformName);
    glUniform2ui(loc, v1, v2);
    return (loc >= 0);
}

void GLShader::SetUInt(int uniformLoc, unsigned int v1, unsigned int v2)
{
    glUniform2ui(uniformLoc, v1, v2);
}

bool GLShader::SetUInt(const char* uniformName, unsigned int v1, unsigned int v2, unsigned int v3)
{
    GLint loc = this->GetUniformLocation(uniformName);
    glUniform3ui(loc, v1, v2, v3);
    return (loc >= 0);
}

void GLShader::SetUInt(int uniformLoc, unsigned int v1, unsigned int v2, unsigned int v3)
{
    glUniform3ui(uniformLoc, v1, v2, v3);
}

bool GLShader::SetUInt(const char* uniformName, unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4)
{
    GLint loc = this->GetUniformLocation(uniformName);
    glUniform4ui(loc, v1, v2, v3, v4);
    return (loc >= 0);
}

void GLShader::SetUInt(int uniformLoc, unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4)
{
    glUniform4ui(uniformLoc, v1, v2, v3, v4);
}

bool GLShader::SetUIntArray(const char* uniformName, unsigned int dim, unsigned int count, const unsigned int* vals)
{
    GLint loc = this->GetUniformLocation(uniformName);
    if (loc >= 0) {
        SetUIntArray(loc, dim, count, vals);
    }
    return (loc >= 0);
}

void GLShader::SetUIntArray(int uniformLoc, unsigned int dim, unsigned int count, const unsigned int* vals)
{
    switch (dim) {
        case 1:
            glUniform1uiv(uniformLoc, static_cast<GLsizei>(count), vals);
            break;
        case 2:
            glUniform2uiv(uniformLoc, static_cast<GLsizei>(count), vals);
            break;
        case 3:
            glUniform3uiv(uniformLoc, static_cast<GLsizei>(count), vals);
            break;
        case 4:
            glUniform4uiv(uniformLoc, static_cast<GLsizei>(count), vals);
            break;
    }
}

void GLShader::SetExePath(const char* exePath)
{
    if (exePath != nullptr) {
        myExePath = std::string(exePath) + "/";
    }
}

void GLShader::SetLocalPath(const char* localPath)
{
    if (localPath != nullptr) {
        myLocalPath = std::string(localPath) + "/";
    }
}

void GLShader::SetFileName(Type type, const char* filename)
{
    switch (type) {
        case Type::None:
            break;
        case Type::Vert: {
            SetVertFileName(filename);
            break;
        }
        case Type::Geom: {
            SetGeomFileName(filename);
            break;
        }
        case Type::TCtrl: {
            SetTCtrlFileName(filename);
            break;
        }
        case Type::TEval: {
            SetTEvalFileName(filename);
            break;
        }
        case Type::Frag: {
            SetFragFileName(filename);
            break;
        }
        case Type::Comp: {
            SetCompFileName(filename);
            break;
        }
    }
}

const char* GLShader::GetFileName(Type type)
{
    switch (type) {
        case Type::None: {
            return nullptr;
        }
        case Type::Vert: {
            return m_vertFileName.c_str();
        }
        case Type::Geom: {
            return m_geomFileName.c_str();
        }
        case Type::TCtrl: {
            return m_tcFileName.c_str();
        }
        case Type::TEval: {
            return m_teFileName.c_str();
        }
        case Type::Frag: {
            return m_fragFileName.c_str();
        }
        case Type::Comp: {
            return m_compFileName.c_str();
        }
    }

    return nullptr;
}

void GLShader::ClearFileNames()
{
    m_vertFileName = std::string();
    m_geomFileName = std::string();
    m_tcFileName = std::string();
    m_teFileName = std::string();
    m_fragFileName = std::string();
    m_compFileName = std::string();
}

void GLShader::SetFileNames(const char* vertFilename, const char* fragFilename)
{
    SetVertFileName(vertFilename);
    SetFragFileName(fragFilename);
}

void GLShader::SetFileNames(const char* vertFilename, const char* geomFilename, const char* fragFilename)
{
    SetVertFileName(vertFilename);
    SetGeomFileName(geomFilename);
    SetFragFileName(fragFilename);
}

void GLShader::SetVertFileName(const char* filename)
{
    m_vertFileName = std::string(filename);
    setFlag(Type::Vert);
}

void GLShader::SetGeomFileName(const char* filename)
{
    m_geomFileName = std::string(filename);
    setFlag(Type::Geom);
}

void GLShader::SetTCtrlFileName(const char* filename)
{
    m_tcFileName = std::string(filename);
    setFlag(Type::TCtrl);
}

void GLShader::SetTEvalFileName(const char* filename)
{
    m_teFileName = std::string(filename);
    setFlag(Type::TEval);
}

void GLShader::SetFragFileName(const char* filename)
{
    m_fragFileName = std::string(filename);
    setFlag(Type::Frag);
}

void GLShader::SetCompFileName(const char* filename)
{
    m_compFileName = std::string(filename);
    setFlag(Type::Comp);
}

