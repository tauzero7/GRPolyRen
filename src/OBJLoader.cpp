/**
 * File:    OBJLoader.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "OBJLoader.h"
#include "Utilities.h"

const char* const OBJLoader::ObjTextureNames[] = { "none", "disk", "sphere", "col_sphere", "triangle"};

OBJLoader::OBJLoader()
    : m_objOffsets(nullptr)
    , m_numDrawObjects(0)
    , m_numAllObjVertices(0)
    , m_scale(1.0f)
    , m_objTexture(ObjTexture::None)
{
    // blank
}

OBJLoader::OBJLoader(const char* pathname, const char* filename)
    : m_objOffsets(nullptr)
    , m_numDrawObjects(0)
    , m_numAllObjVertices(0)
    , m_scale(1.0f)
{
    m_pathname = std::string(pathname);
    m_filename = std::string(filename);
    ReadObjFile(pathname, filename);
}

OBJLoader::~OBJLoader()
{
    ClearAll();
}

glm::vec4 OBJLoader::CenterOfVertices()
{
    return m_centerOfVertices;
}

void OBJLoader::ClearAll()
{
    m_filename = std::string();
    m_mtlFilename = std::string();

    if (!m_vertices.empty()) {
        m_vertices.clear();
    }

    if (!m_normals.empty()) {
        m_normals.clear();
    }

    if (!m_texCoords.empty()) {
        m_texCoords.clear();
    }

    if (!m_faces.empty()) {
        m_faces.clear();
    }

    if (!m_tags.empty()) {
        m_tags.clear();
    }

    if (!m_material.empty()) {
        for (unsigned int i = 0; i < m_material.size(); i++) {
            SafeDelete<obj_material>(m_material[i]);
        }
        m_material.clear();
    }

    if (!m_materialNames.empty()) {
        m_materialNames.clear();
    }

    if (!m_texNames.empty()) {
        m_texNames.clear();
    }

    if (!m_objList.empty()) {
        m_objList.clear();
    }

    clearObjPointers();
}

bool OBJLoader::GenDrawObjects(float*& vert, float*& norm, float*& tc)
{
    if (!m_objList.empty()) {
        m_objList.clear();
    }

    if (m_faces.size() == 0 || m_vertices.size() == 0) {
        return false;
    }

    SafeDelete<unsigned int>(m_objOffsets);

    bool haveOnlyTriangles = true;

    m_numAllObjVertices = 0;
    for (unsigned int k = 0; k < m_tags.size(); k++) {
        if (m_tags[k].vFaceNums.size() == 0) {
            continue;
        }

        obj_draw obj;
        int numVertices = 0;
        for (unsigned int fNum = 0; fNum < m_tags[k].vFaceNums.size(); fNum++) {
            numVertices += static_cast<int>(m_faces[m_tags[k].vFaceNums[fNum]].size());
        }
        fprintf(stderr, "Tag: %2d --> #vertices: %d\n", k, numVertices);
        m_numAllObjVertices += numVertices;

        for (unsigned int fNum = 0; fNum < m_tags[k].vFaceNums.size(); fNum++) {
            size_t faceNum = static_cast<size_t>(m_tags[k].vFaceNums[fNum]);
            if (!(m_faces[faceNum].size() >= 3 && (m_faces[faceNum].size() % 2 == 1))) {
                haveOnlyTriangles = false;
                continue;
            }

            for (unsigned int j = 1; j < m_faces[faceNum].size(); j += 2) {
                int v1, n1, t1, v2, n2, t2, v3, n3, t3;

                v1 = m_faces[faceNum][0].vID;
                n1 = m_faces[faceNum][0].nID;
                t1 = m_faces[faceNum][0].texID;

                v2 = m_faces[faceNum][j + 0].vID;
                n2 = m_faces[faceNum][j + 0].nID;
                t2 = m_faces[faceNum][j + 0].texID;

                v3 = m_faces[faceNum][j + 1].vID;
                n3 = m_faces[faceNum][j + 1].nID;
                t3 = m_faces[faceNum][j + 1].texID;
                // fprintf(stderr, "NNN: %d %d %d\n", n1, n2, n3);

                if (v1 == 0 || v2 == 0 || v3 == 0 || -v1 > static_cast<int>(m_vertices.size())
                    || -v2 > static_cast<int>(m_vertices.size()) || -v3 > static_cast<int>(m_vertices.size())) {
                    fprintf(stderr, "Vertex coordinates defect!\n");
                    obj.vert.push_back(glm::vec4(0));
                    obj.vert.push_back(glm::vec4(0));
                    obj.vert.push_back(glm::vec4(0));
                }
                else {
                    if (v1 > 0) {
                        obj.vert.push_back(m_vertices[v1 - 1]);
                    }
                    else if (v1 < 0) {
                        obj.vert.push_back(m_vertices[m_vertices.size() + v1]);
                    }

                    if (v2 > 0) {
                        obj.vert.push_back(m_vertices[v2 - 1]);
                    }
                    else if (v2 < 0) {
                        obj.vert.push_back(m_vertices[m_vertices.size() + v2]);
                    }

                    if (v3 > 0) {
                        obj.vert.push_back(m_vertices[v3 - 1]);
                    }
                    else if (v3 < 0) {
                        obj.vert.push_back(m_vertices[m_vertices.size() + v3]);
                    }
                }

                if (m_normals.empty() || n1 == 0 || n2 == 0 || n3 == 0 || -n1 > static_cast<int>(m_normals.size())
                    || -n2 > static_cast<int>(m_normals.size()) || -n3 > static_cast<int>(m_normals.size())) {
                    obj.norm.push_back(glm::vec3(0));
                    obj.norm.push_back(glm::vec3(0));
                    obj.norm.push_back(glm::vec3(0));
                }
                else {
                    if (n1 > 0) {
                        obj.norm.push_back(m_normals[n1 - 1]);
                    }
                    else if (n1 < 0) {
                        obj.norm.push_back(m_normals[m_normals.size() + n1]);
                    }

                    if (n2 > 0) {
                        obj.norm.push_back(m_normals[n2 - 1]);
                    }
                    else if (n2 < 0) {
                        obj.norm.push_back(m_normals[m_normals.size() + n2]);
                    }

                    if (n3 > 0) {
                        obj.norm.push_back(m_normals[n3 - 1]);
                    }
                    else if (n3 < 0) {
                        obj.norm.push_back(m_normals[m_normals.size() + n3]);
                    }
                }

                if (m_texCoords.empty() || t1 == 0 || t2 == 0 || t3 == 0 || -t1 > static_cast<int>(m_texCoords.size())
                    || -t2 > static_cast<int>(m_texCoords.size()) || -t3 > static_cast<int>(m_texCoords.size())) {
                    obj.tc.push_back(glm::vec2(0));
                    obj.tc.push_back(glm::vec2(0));
                    obj.tc.push_back(glm::vec2(0));
                }
                else {
                    if (t1 > 0) {
                        obj.tc.push_back(m_texCoords[t1 - 1]);
                    }
                    else if (t1 < 0) {
                        obj.tc.push_back(m_texCoords[m_texCoords.size() + t1]);
                    }

                    if (t2 > 0) {
                        obj.tc.push_back(m_texCoords[t2 - 1]);
                    }
                    else if (t2 < 0) {
                        obj.tc.push_back(m_texCoords[m_texCoords.size() + t2]);
                    }

                    if (t3 > 0) {
                        obj.tc.push_back(m_texCoords[t3 - 1]);
                    }
                    else if (t3 < 0) {
                        obj.tc.push_back(m_texCoords[m_texCoords.size() + t3]);
                    }
                }
            }
        }

        obj.materialID = m_tags[k].materialID;
        fprintf(stderr, "    #material: %d\n", obj.materialID);
        m_objList.push_back(obj);
    }

    fprintf(stderr, "-----------------\n#All vertices %d\n\n", m_numAllObjVertices);

    if (!haveOnlyTriangles) {
        fprintf(stderr, "NOTE: There are polygon faces other than triangles! These cannot be rendered!\n");
    }

    clearObjPointers();
    m_numDrawObjects = static_cast<unsigned int>(m_objList.size());

    vert = new float[m_numAllObjVertices * 4];
    norm = new float[m_numAllObjVertices * 3];
    tc = new float[m_numAllObjVertices * 2];
    m_objOffsets = new unsigned int[m_numDrawObjects + 1];

    m_objOffsets[0] = 0;

    float* vptr = vert;
    float* nptr = norm;
    float* tptr = tc;
    for (unsigned int i = 0; i < m_objList.size(); i++) {
        for (unsigned int j = 0; j < m_objList[i].vert.size(); j++) {
            *(vptr++) = m_objList[i].vert[j].x;
            *(vptr++) = m_objList[i].vert[j].y;
            *(vptr++) = m_objList[i].vert[j].z;
            *(vptr++) = m_objList[i].vert[j].w;
            *(nptr++) = m_objList[i].norm[j].x;
            *(nptr++) = m_objList[i].norm[j].y;
            *(nptr++) = m_objList[i].norm[j].z;
            *(tptr++) = m_objList[i].tc[j].x;
            *(tptr++) = m_objList[i].tc[j].y;
        }
        m_objOffsets[i + 1] = m_objOffsets[i] + static_cast<unsigned int>(m_objList[i].vert.size());
    }

    return true;
}

unsigned int* OBJLoader::GetDrawOffsets()
{
    return m_objOffsets;
}

bool OBJLoader::GetFacePoint(unsigned int face, unsigned int idx, obj_face_point& fp)
{
    if (face >= m_faces.size()) {
        return false;
    }

    if (idx >= m_faces[face].size()) {
        return false;
    }

    fp = m_faces[face][idx];
    return true;
}

OBJLoader::obj_material* OBJLoader::GetMaterial(const unsigned int objNum)
{
    if (objNum < m_objList.size()) {
        if (m_objList[objNum].materialID >= 0 && m_objList[objNum].materialID < static_cast<int>(m_material.size())) {
            return m_material[m_objList[objNum].materialID];
        }
    }
    return nullptr;
}

unsigned int OBJLoader::GetNumDrawObjects()
{
    return m_numDrawObjects;
}

unsigned int OBJLoader::GetNumDrawVertices()
{
    return m_numAllObjVertices;
}

unsigned int OBJLoader::GetNumFaces()
{
    return static_cast<unsigned int>(m_faces.size());
}

unsigned int OBJLoader::GetNumFaceIndices(unsigned int face)
{
    if (face >= m_faces.size()) {
        return 0;
    }
    return static_cast<unsigned int>(m_faces[face].size());
}

unsigned int OBJLoader::GetNumTextures()
{
    return static_cast<unsigned int>(m_texNames.size());
}

void OBJLoader::SetScale(float scale)
{
    m_scale = scale;
}

float OBJLoader::GetScale()
{
    return m_scale;
}

const char* OBJLoader::GetTextureName(unsigned int num, int& id)
{
    if (num < m_texNames.size()) {
        auto it = m_texNames.begin();
        for (unsigned int i = 0; i < num; i++) {
            it++;
        }
        id = it->second;
        return (it->first).c_str();
    }
    return nullptr;
}

void OBJLoader::PrintAllTags(FILE* fptr)
{
    for (unsigned int i = 0; i < m_tags.size(); i++) {
        fprintf(fptr, "\nTag: #%d:\n", i);
        fprintf(fptr, "    MaterialID: %d\n", m_tags[i].materialID);
        fprintf(fptr, "    Faces:");
        for (auto& f : m_tags[i].vFaceNums) {
            fprintf(fptr, "%d ", f);
        }
        // for(unsigned int j = 0; i < m_tags[i].vFaceNums.size(); j++) {
        //    fprintf(fptr, "%d ", m_tags[i].vFaceNums[j]);
        //}
        fprintf(fptr, "\n");
    }
}

void OBJLoader::PrintFacePoint(obj_face_point& fp, FILE* fptr)
{
    fprintf(fptr, "  %d/%d/%d\n", fp.vID, fp.texID, fp.nID);
    fprintf(fptr, "  %f %f %f %f\n", m_vertices[fp.vID].x, m_vertices[fp.vID].y, m_vertices[fp.vID].z,
        m_vertices[fp.vID].w);
    fprintf(fptr, "  %f %f\n", m_texCoords[fp.texID].x, m_texCoords[fp.texID].y);
}

void OBJLoader::PrintMaterial(int materialID, FILE* fptr)
{
    if (materialID >= 0 && materialID < static_cast<int>(m_material.size())) {
        obj_material* mat = m_material[materialID];
        fprintf(fptr, "\nMaterialID: %d\n", materialID);
        fprintf(fptr, "  Ns: %6.4f\n", mat->Ns);
        fprintf(fptr, "  Ni: %6.4f\n", mat->Ni);
        fprintf(fptr, "   d: %6.4f\n", mat->d);
        fprintf(fptr, "  Ka: %6.4f %6.4f %6.4f\n", mat->Ka[0], mat->Ka[1], mat->Ka[2]);
        fprintf(fptr, "  Kd: %6.4f %6.4f %6.4f\n", mat->Kd[0], mat->Kd[1], mat->Kd[2]);
        fprintf(fptr, "  Ks: %6.4f %6.4f %6.4f\n", mat->Ks[0], mat->Ks[1], mat->Ks[2]);
        fprintf(fptr, "  Ke: %6.4f %6.4f %6.4f\n", mat->Ke[0], mat->Ke[1], mat->Ke[2]);
        // std::cerr << "hier: " << mat->mapID << std::endl;
        if (mat->mapID > 0) {
            fprintf(fptr, "  mapid: %d\n", mat->mapID);
        }
    }
}

bool OBJLoader::ReadObjFile(const char* pathname, const char* filename)
{
    std::string fn = std::string(pathname) + "/" + std::string(filename);

    FileTokenizer ft;
    ft.InsertIgnoreToken('#');

    if (!ft.ReadFile(fn.c_str())) {
        return false;
    }
    // ft.ShowAll(stdout, true);

    ClearAll();
    //fprintf(stderr, "Read %d lines ... \n", ft.GetNumLines());

    obj_tag tag;
    tag.materialID = -1;

    glm::vec4 v;
    glm::vec3 n;
    glm::vec2 tc;
    obj_face f;

    m_centerOfVertices = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    char* buf = nullptr;
    int numCols;
    for (unsigned int i = 0; i < ft.GetNumLines(); i++) {
        // std::cerr << "line: " << i << std::endl;
        if (ft.IsBaseTokenValid("mtllib", i, numCols)) {
            ft.GetSubToken(i, 1, buf);
            m_mtlFilename = std::string(buf);
            SafeDelete<char>(buf);
            ReadMtlfile(pathname, m_mtlFilename.c_str());
        }
        else if (ft.IsBaseTokenValid("usemtl", i, numCols)) {
            m_tags.push_back(tag);
            if (ft.GetSubToken(i, 1, buf)) {
                std::string matname = std::string(buf);
                std::transform(matname.begin(), matname.end(), matname.begin(), ::tolower);
                auto itr = m_materialNames.find(matname);
                if (itr != m_materialNames.end()) {
                    tag.materialID = itr->second;
                }

                if (!tag.vFaceNums.empty()) {
                    tag.vFaceNums.clear();
                }
            }
        }
        else if (ft.IsBaseTokenValid("v", i, numCols) && numCols > 1) {
            if (readVertex(&ft, i, v)) {
                // fprintf(stderr, "V: %8.4f %8.4f %8.4f %8.4f\n", v.x, v.y, v.z, v.w);
                m_centerOfVertices += v;
                m_vertices.push_back(v);
            }
        }
        else if (ft.IsBaseTokenValid("vn", i, numCols) && numCols > 3) {
            if (readNormal(&ft, i, n)) {
                // fprintf(stderr, "N: %8.4f %8.4f %8.4f\n", n.x, n.y, n.z);
                m_normals.push_back(n);
            }
        }
        else if (ft.IsBaseTokenValid("vt", i, numCols) && numCols > 2) {
            if (readTexCoord(&ft, i, tc)) {
                // fprintf(stderr, "T: %8.4f %8.4f\n", tc.x, tc.y);
                m_texCoords.push_back(tc);
            }
        }
        else if (ft.IsBaseTokenValid("f", i, numCols) && numCols > 1) {
            if (readFace(&ft, i, f)) {
                m_faces.push_back(f);
                int numFaces = static_cast<int>(m_faces.size());
                tag.vFaceNums.push_back(numFaces - 1);
            }
        }
    }
    SafeDelete<char>(buf);
    m_tags.push_back(tag);

    fprintf(stderr, "\nOBJ file \'%s\' has the following basic entries:\n", fn.c_str());
    fprintf(stderr, "-----------------------------------\n");
    if (!m_mtlFilename.empty()) {
        fprintf(stderr, "MTLlib      : %s\n", m_mtlFilename.c_str());
    }
    fprintf(stderr, "# Vertices  : %d\n", static_cast<int>(m_vertices.size()));
    fprintf(stderr, "# Normals   : %d\n", static_cast<int>(m_normals.size()));
    fprintf(stderr, "# TexCoords : %d\n", static_cast<int>(m_texCoords.size()));
    fprintf(stderr, "# Faces     : %d\n", static_cast<int>(m_faces.size()));
    fprintf(stderr, "# Tags      : %d\n", static_cast<int>(m_tags.size()));
    fprintf(stderr, "-----------------------------------\n");

    float mm = 1.0f / static_cast<float>(m_vertices.size());
    m_centerOfVertices *= mm;
    fprintf(stderr, "center: %f %f %f\n", m_centerOfVertices.x, m_centerOfVertices.y, m_centerOfVertices.z);
    return true;
}

bool OBJLoader::ReadMtlfile(const char* pathname, const char* filename)
{
    std::string fn = std::string(pathname) + "/" + std::string(filename);

    FileTokenizer ft;
    ft.InsertIgnoreToken('#');

    if (!ft.ReadFile(fn.c_str())) {
        return false;
    }
    // ft.ShowAll(stdout, true);

    obj_material* mat;
    std::string matName = std::string();
    int currMaterialID = -1;
    int currTexID = -1;

    int numCols;
    char* buf = nullptr;
    for (unsigned int i = 0; i < ft.GetNumLines(); i++) {
        if (ft.IsBaseTokenValid("newmtl", i, numCols) && numCols >= 2) {
            ft.GetSubToken(i, 1, buf);
            matName = std::string(buf);
            std::transform(matName.begin(), matName.end(), matName.begin(), ::tolower);

            mat = new obj_material;
            m_material.push_back(mat);
            currMaterialID = static_cast<int>(m_material.size() - 1);
            m_materialNames.insert(std::pair<std::string, int>(matName, currMaterialID));
        }
        else if (ft.IsBaseTokenValid("Ns", i, numCols) && numCols >= 1) {
            if (currMaterialID >= 0 && currMaterialID < static_cast<int>(m_material.size())) {
                ft.GetSubToken<float>(i, 1, m_material[currMaterialID]->Ns);
            }
        }
        else if (ft.IsBaseTokenValid("Ni", i, numCols) && numCols >= 1) {
            if (currMaterialID >= 0 && currMaterialID < static_cast<int>(m_material.size())) {
                ft.GetSubToken<float>(i, 1, m_material[currMaterialID]->Ni);
            }
        }
        else if (ft.IsBaseTokenValid("d", i, numCols) && numCols >= 1) {
            if (currMaterialID >= 0 && currMaterialID < static_cast<int>(m_material.size())) {
                ft.GetSubToken<float>(i, 1, m_material[currMaterialID]->d);
            }
        }
        else if (ft.IsBaseTokenValid("Ka", i, numCols) && numCols >= 3) {
            if (currMaterialID >= 0 && currMaterialID < static_cast<int>(m_material.size())) {
                ft.GetSubTokens<float>(i, 1, 3, m_material[currMaterialID]->Ka);
            }
        }
        else if (ft.IsBaseTokenValid("Kd", i, numCols) && numCols >= 3) {
            if (currMaterialID >= 0 && currMaterialID < static_cast<int>(m_material.size())) {
                ft.GetSubTokens<float>(i, 1, 3, m_material[currMaterialID]->Kd);
            }
        }
        else if (ft.IsBaseTokenValid("Ks", i, numCols) && numCols >= 3) {
            if (currMaterialID >= 0 && currMaterialID < static_cast<int>(m_material.size())) {
                ft.GetSubTokens<float>(i, 1, 3, m_material[currMaterialID]->Ks);
            }
        }
        else if (ft.IsBaseTokenValid("Ke", i, numCols) && numCols >= 3) {
            if (currMaterialID >= 0 && currMaterialID < static_cast<int>(m_material.size())) {
                ft.GetSubTokens<float>(i, 1, 3, m_material[currMaterialID]->Ke);
            }
        }
        else if (ft.IsBaseTokenValid("map_Kd", i, numCols) && numCols >= 1) {
            if (currMaterialID >= 0 && currMaterialID < static_cast<int>(m_material.size())) {
                ft.GetSubToken(i, -1, buf);
                auto itr = m_texNames.find(buf);
                if (itr == m_texNames.end()) {
                    m_texNames.insert(std::pair<std::string, int>(std::string(buf), ++currTexID));
                }
                else {
                    currTexID = itr->second;
                }

                if (currTexID >= 0) {
                    // std::cerr << "TTTTTTT: " << currMaterialID << " " << currTexID << std::endl;
                    m_material[currMaterialID]->mapID = currTexID;

                    int offsetPos = ft.FindTokenInLine(i, "-o");
                    if (offsetPos > 0 && numCols > offsetPos + 3) {
                        float offset[3];
                        if (ft.GetSubTokens<float>(i, offsetPos + 1, 3, offset)) {
                            m_material[currMaterialID]->mapTexOffset[0] = offset[0];
                            m_material[currMaterialID]->mapTexOffset[1] = offset[1];
                            m_material[currMaterialID]->mapTexOffset[2] = offset[2];
                            // fprintf(stderr, "offset: %f %f %f\n", offset[0], offset[1], offset[2]);
                        }
                    }

                    int scalePos = ft.FindTokenInLine(i, "-s");
                    if (scalePos > 0 && numCols > scalePos + 3) {
                        float scale[3];
                        if (ft.GetSubTokens<float>(i, scalePos + 1, 3, scale)) {
                            m_material[currMaterialID]->mapTexScale[0] = scale[0];
                            m_material[currMaterialID]->mapTexScale[1] = scale[1];
                            m_material[currMaterialID]->mapTexScale[2] = scale[2];
                            // fprintf(stderr, "scale: %f %f %f\n", scale[0], scale[1], scale[2]);
                        }
                    }
                }
            }
        }
    }

    for (unsigned int i = 0; i < m_material.size(); i++) {
        PrintMaterial(i);
    }

    return false;
}

void OBJLoader::clearObjPointers()
{
    SafeDelete<unsigned int>(m_objOffsets);
}

bool OBJLoader::readFace(FileTokenizer* ft, int row, obj_face& face)
{
    if (!face.empty()) {
        face.clear();
    }

    std::string::size_type fpos, lpos;

    char* buf = nullptr;
    for (int i = 1; i < ft->GetNumSubTokens(row); i++) {
        obj_face_point p;

        if (ft->GetSubToken(row, i, buf)) {
            std::string sf = std::string(buf);
            fpos = sf.find_first_of("/");

            if (fpos != std::string::npos) {
                lpos = sf.find_last_of("/");

                p.vID = atoi(sf.substr(0, fpos).c_str());
                p.texID = atoi(sf.substr(fpos + 1, lpos - fpos - 1).c_str());
                p.nID = atoi(sf.substr(lpos + 1, sf.length() - lpos - 1).c_str());
                // fprintf(stderr, "%s %s %s : ",
                //         sf.substr(0, fpos).c_str(),
                //         sf.substr(fpos + 1, lpos - fpos - 1).c_str(),
                //         sf.substr(lpos + 1, sf.length() - lpos - 1).c_str());
                // fprintf(stderr, "%d %d %d\n", p.vID, p.texID, p.nID);
            }
            else {
                p.vID = p.texID = p.nID = atoi(sf.c_str());
            }
            face.push_back(p);
        }
    }
    SafeDelete<char>(buf);
    return true;
}

bool OBJLoader::readNormal(FileTokenizer* ft, int row, glm::vec3& n)
{
    n = glm::vec3(0);

    int num = ft->GetNumSubTokens(row);
    if (num != 4) {
        return false;
    }

    float nn[3];
    ft->GetSubTokens<float>(row, 3, nn);
    n = glm::vec3(nn[0], nn[1], nn[2]);
    return true;
}

bool OBJLoader::readVertex(FileTokenizer* ft, int row, glm::vec4& v)
{
    v = glm::vec4(0.0, 0.0, 0.0, 1.0);

    int num = ft->GetNumSubTokens(row);
    int i = 0;
    while (i + 1 < num && i < 4) {
        float c;
        ft->GetSubToken<float>(row, i + 1, c);
        v[i] = c;
        i++;
    }

    return (num != 0);
}

bool OBJLoader::readTexCoord(FileTokenizer* ft, int row, glm::vec2& tc)
{
    tc = glm::vec2(0);

    int num = ft->GetNumSubTokens(row);
    if (num < 2) {
        return false;
    }

    float t[2];
    ft->GetSubTokens<float>(row, 2, t);
    tc = glm::vec2(t[0], t[1]);
    return true;
}

void OBJLoader::UpdateGL(GLShader* shader)
{
    if (shader == nullptr) {
        return;
    }

    shader->SetFloat("scale", m_scale);
    shader->SetInt("obj_texture", static_cast<int>(m_objTexture));
}

void OBJLoader::SetObjTexture(ObjTexture objtex)
{
    m_objTexture = objtex;
}

void OBJLoader::SetObjTextureByName(const char* name) {
    int oid = 0;
    for(const char* oname : OBJLoader::ObjTextureNames) {
        if (strcmp(oname, name) == 0) {
            m_objTexture = static_cast<ObjTexture>(oid);
            return;
        }
        oid++;
    }
    m_objTexture = ObjTexture::None;
}