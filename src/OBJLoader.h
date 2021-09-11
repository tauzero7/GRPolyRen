/**
 * File:    OBJLoader.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_IO_WAVEFRONT_OBJ_H
#define GRPR_IO_WAVEFRONT_OBJ_H

#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <vector>

#include "GLShader.h"
#include "FileTokenizer.h"

class OBJLoader
{
public:
    typedef struct obj_face_point_t {
        int vID; //!< Vertex ID
        int texID; //!< Texture ID
        int nID; //!< Normal ID
        obj_face_point_t()
        {
            this->vID = -1;
            this->texID = -1;
            this->nID = -1;
        }
    } obj_face_point;

    typedef std::vector<obj_face_point> obj_face;

    typedef struct obj_tag_t {
        int materialID;
        std::vector<int> vFaceNums;
        obj_tag_t() { materialID = -1; }
    } obj_tag;

    typedef struct obj_draw_t {
        std::vector<glm::vec4> vert;
        std::vector<glm::vec3> norm;
        std::vector<glm::vec2> tc;
        int materialID;
        obj_draw_t() { materialID = -1; }
    } obj_draw;

    typedef struct obj_material_t {
        float Ns;
        float Ni;
        float d;
        float Tr;
        float Tf[3];
        int illum;
        float Ka[3];
        float Kd[3];
        float Ks[3];
        float Ke[3];

        float mapTexOffset[3];
        float mapTexScale[3];

        int mapID;
        unsigned int bumpMapID;

        obj_material_t()
        {
            Ns = Ni = d = Tr = 0.0f;
            Tf[0] = Tf[1] = Tf[2] = 0.0f;
            illum = 0;
            Ka[0] = Ka[1] = Ka[2] = 1.0f;
            Kd[0] = Kd[1] = Kd[2] = 1.0f;
            Ks[0] = Ks[1] = Ks[2] = 0.0f;
            Ke[0] = Ke[1] = Ke[2] = 0.0f;
            mapTexOffset[0] = mapTexOffset[1] = mapTexOffset[2] = 0.0f;
            mapTexScale[0] = mapTexScale[1] = mapTexScale[2] = 1.0f;
            mapID = -1;
            bumpMapID = 0;
        }
    } obj_material;

    enum class ObjTexture : int { None = 0, Disk, Sphere, ColSphere, Triangle };
    static const char* const ObjTextureNames[];

public:
    OBJLoader();
    OBJLoader(const char* pathname, const char* filename);
    virtual ~OBJLoader();

    glm::vec4 CenterOfVertices();

    void ClearAll();

    bool GenDrawObjects(float*& vert, float*& norm, float*& tc);

    unsigned int* GetDrawOffsets();

    bool GetFacePoint(unsigned int face, unsigned int idx, obj_face_point& fp);

    obj_material* GetMaterial(const unsigned int objNum);

    unsigned int GetNumDrawObjects();

    unsigned int GetNumDrawVertices();

    unsigned int GetNumFaces();

    unsigned int GetNumFaceIndices(unsigned int face);

    unsigned int GetNumTextures();

    void SetScale(float scale);
    float GetScale();

    const char* GetTextureName(unsigned int num, int& id);

    void PrintAllTags(FILE* fptr = stdout);

    void PrintFacePoint(obj_face_point& fp, FILE* fptr = stdout);

    void PrintMaterial(int materialID, FILE* fptr = stdout);

    bool ReadObjFile(const char* pathname, const char* filename);

    bool ReadMtlfile(const char* pathname, const char* filename);

    void UpdateGL(GLShader* shader);

    void SetObjTexture(ObjTexture objtex);
    void SetObjTextureByName(const char* name);

protected:
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

    void clearObjPointers();

    bool readFace(FileTokenizer* ft, int row, obj_face& face);
    bool readNormal(FileTokenizer* ft, int row, glm::vec3& n);
    bool readVertex(FileTokenizer* ft, int row, glm::vec4& v);
    bool readTexCoord(FileTokenizer* ft, int row, glm::vec2& tc);

protected:
    std::string m_pathname;
    std::string m_filename;
    std::string m_mtlFilename;
    long m_numVertices;
    long m_numNormals;
    long m_numTexCoords;

    std::vector<glm::vec4> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_texCoords;
    std::vector<obj_face> m_faces;
    std::vector<obj_tag> m_tags;

    std::vector<obj_material*> m_material;
    std::map<std::string, int> m_materialNames;
    std::map<std::string, int> m_texNames;

    glm::vec4 m_centerOfVertices;

    unsigned int* m_objOffsets;
    unsigned int m_numDrawObjects;
    unsigned int m_numAllObjVertices;

    std::vector<obj_draw> m_objList;

    float m_scale;

    ObjTexture m_objTexture;
};

#endif // GRPR_IO_WAVEFRONT_OBJ_H
