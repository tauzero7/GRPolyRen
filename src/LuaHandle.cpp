
#include "LuaHandle.h"
#include "Renderer.h"

static lua_State* m_luaInstance = nullptr;    

extern Renderer* renderer;

extern void draw();
extern bool saveImageToFile(const char* filename);
extern void setWindowSize(int width, int height);

int loadObject(lua_State* L) {
    const char* filename = lua_tostring(L, -1);
    renderer->LoadObject(filename);
    return 0;
}

int loadSetting(lua_State* L) {
    const char* filename = lua_tostring(L, -1);
    renderer->LoadSetting(filename);
    return 0;
}

int renderImage(lua_State* L) {
    draw();
    return 0;
}

int saveImage(lua_State* L) {
    const char* filename = lua_tostring(L, -1);
    if (filename != nullptr && strcmp(filename,"") != 0) {
        saveImageToFile(filename);
    }
    return 0;
}

int setCamPoI(lua_State* L) {
    double poi[3];
    if (getVector<double>(L, poi)) {
        fprintf(stderr, "lua: set camera poi: %f %f %f\n", poi[0], poi[1], poi[2]);
        renderer->m_camera.SetPoI(poi);
    }
    return 0;
}

int setCamPos(lua_State* L) {
    double pos[3];
    if (getVector<double>(L, pos)) {
        fprintf(stderr, "lua: set camera position: %f %f %f\n", pos[0], pos[1], pos[2]);
        renderer->m_camera.SetPosition(pos);
    }
    return 0;
}

int setCamFoV(lua_State* L) {
    if (lua_isnumber(L,-1)) {
        double val = static_cast<double>(lua_tonumber(L,-1));
        fprintf(stderr, "lua: set camera fov: %f\n", val);
        renderer->m_camera.SetFoVy(val);
    }
    return 0;
}

int setObjScale(lua_State* L) {
    float scale[3];
    if (getVector<float>(L, scale)) {
        fprintf(stderr, "lua: set object scale: %f %f %f\n", scale[0], scale[1], scale[2]);
        renderer->m_transScale.SetScale(scale);
    }
    return 0;
}

int setObjTrans(lua_State* L) {
    float trans[3];
    if (getVector<float>(L, trans)) {
        fprintf(stderr, "lua: set object trans: %f %f %f\n", trans[0], trans[1], trans[2]);
        renderer->m_transScale.SetTrans(trans);
    }
    return 0;
}

int setObjTexture(lua_State* L) {
     const char* name =  lua_tostring(L, -1);
    if (name != nullptr && strcmp(name,"") != 0) {
        fprintf(stderr, "lua: set object texture: %s\n", name);
        renderer->m_obj.SetObjTextureByName(name);
    }
    return 0;
}

int setPatFreq(lua_State* L) {
    int freq[2];
    if (getVector<int>(L, freq, 2)) {
        fprintf(stderr, "lua: set pattern freq: %d %d\n", freq[0], freq[1]);
        renderer->m_patFreq[0] = freq[0];
        renderer->m_patFreq[1] = freq[1];
    }
    return 0;
}

int setEulerRot(lua_State* L) {
    float angle[3];
    if (getVector<float>(L, angle)) {
        fprintf(stderr, "lua: set euler rotation: %f %f %f\n", angle[0], angle[1], angle[2]);
        renderer->m_eulerRot.Set(angle);
    }
    return 0;
}

int setEulerOrder(lua_State* L) {
    const char* eulerOrder = lua_tostring(L, -1);
    if (eulerOrder != nullptr && strcmp(eulerOrder,"") != 0) {
        fprintf(stderr, "lua: set euler order: %s\n", eulerOrder);
        renderer->m_eulerRot.SetOrderByName(eulerOrder);
    }
    return 0;
}

int setBlackHoleColor(lua_State* L) {
    float rgba[4];
    if (getVector<float>(L, rgba, 4)) {
        fprintf(stderr, "lua: set blackhole color: %5.3f %5.3f %5.3f %5.3f\n", rgba[0], rgba[1], rgba[2], rgba[3]);
        renderer->m_blackhole.SetColor(rgba);
    }
    return 0;
}

int setBlackHoleRadius(lua_State* L) {
     if (lua_isnumber(L,-1)) {
        float radius = static_cast<float>(lua_tonumber(L,-1));
        fprintf(stderr, "lua: set blackhole radius: %f\n", radius);
        renderer->m_blackhole.SetRadius(radius);
    }
    return 0;
}

int setBlackHoleFlatShading(lua_State* L) {
    if (lua_isboolean(L,-1)) {
        int isflat = static_cast<int>(lua_toboolean(L,-1));
        fprintf(stderr, "lua: set blackhole flat shading: %d\n", isflat);
        renderer->m_blackhole.SetFlatShading(isflat == 1);
    }
    return 0;
}

int setViewMode(lua_State* L) {
    const char* viewMode = lua_tostring(L, -1);
    if (viewMode != nullptr && strcmp(viewMode,"") != 0) {
        fprintf(stderr, "lua: set view mode: %s\n", viewMode);
        renderer->SetViewModeByName(viewMode);
    }
    return 0;
}

int setMaxTessLevel(lua_State* L) {
    if (lua_isnumber(L,-1)) {
        int mtl = static_cast<int>(lua_tonumber(L,-1));
        fprintf(stderr, "lua: set max tess level: %d\n", mtl);
        renderer->m_maxTessLevel = mtl;
    }
    return 0;
}

int setTessFactor(lua_State* L) {
    if (lua_isnumber(L,-1)) {
        float factor = static_cast<float>(lua_tonumber(L,-1));
        fprintf(stderr, "lua: set tess factor: %f\n", factor);
        renderer->m_tessFactor = factor;
    }
    return 0;
}

int setTessExpon(lua_State* L) {
    if (lua_isnumber(L,-1)) {
        float expon = static_cast<float>(lua_tonumber(L,-1));
        fprintf(stderr, "lua: set tess expon: %f\n", expon);
        renderer->m_tessExpon = expon;
    }
    return 0;
}

int setTessDistRelation(lua_State* L) {
    if (lua_isnumber(L,-1)) {
        float rel = static_cast<float>(lua_tonumber(L,-1));
        fprintf(stderr, "lua: set tess dist relation: %f\n", rel);
        renderer->m_distRelation = rel;
    }
    return 0;
}

int setLightSourceActive(lua_State* L) {
    if (lua_isboolean(L,-1)) {
        int active = static_cast<int>(lua_toboolean(L,-1));
        fprintf(stderr, "lua: set light source active: %d\n", active);
        renderer->m_lights[0].SetActive(active == 1);
    }
    return 0;
}

int setLightSourcePos(lua_State* L) {
    float pos[2];
    if (getVector<float>(L, pos, 2)) {
        fprintf(stderr, "lua: set light source pos: %7.2f %7.2f\n", pos[0], pos[1]);
        renderer->m_lights[0].Set(pos[0], pos[1]);
    }
    return 0;
}

int setLightSourceFactor(lua_State* L) {
    if (lua_isnumber(L,-1)) {
        float fac = static_cast<float>(lua_tonumber(L,-1));
        fprintf(stderr, "lua: set light source factor: %f\n", fac);
        renderer->m_lights[0].SetFactor(fac);
    }
    return 0;
}

int setClearColor(lua_State* L) {
    float rgb[3];
    if (getVector<float>(L, rgb, 3)) {
        fprintf(stderr, "lua: set blackhole color: %5.3f %5.3f %5.3f\n", rgb[0], rgb[1], rgb[2]);
        renderer->SetClearColor(rgb[0], rgb[1], rgb[2]);
    }
    return 0;
}

int setWinSize(lua_State* L) {    
    int num = lua_gettop(L);
    if (num == 2) {
        int wsize[2];
        if (getVector<int>(L, wsize, 2)) {
            fprintf(stderr, "lua: set window size: %d %d\n", wsize[0], wsize[1]);
            setWindowSize(wsize[0], wsize[1]);
        }
    }
    else if (num == 3) {
        double wsize[3];
        if (getVector<double>(L, wsize, 3)) {
            int w = static_cast<int>(wsize[0] * wsize[2]);
            int h = static_cast<int>(wsize[1] * wsize[2]);
            fprintf(stderr, "lua: set scaled window size: %d %d\n", w, h);
            setWindowSize(w, h);
        }
    }
    return 0;
}

int setCrossHairsVisible(lua_State* L) {
    if (lua_isboolean(L,-1)) {
        int visible = static_cast<int>(lua_toboolean(L,-1));
        renderer->m_crossHairs.Show(visible == 1);
    }
    return 0;
}

int setCoordSysVisible(lua_State* L) {
    if (lua_isboolean(L,-1)) {
        int visible = static_cast<int>(lua_toboolean(L,-1));
        renderer->m_coordSystem.Show(visible == 1);
    }
    return 0;
}


void LClose() {
    lua_close(m_luaInstance);
}

bool LInit() {
    m_luaInstance = luaL_newstate();
    luaL_openlibs(m_luaInstance);

    lua_pushcfunction(m_luaInstance, loadObject);
    lua_setglobal(m_luaInstance, "loadObject");

    lua_pushcfunction(m_luaInstance, loadSetting);
    lua_setglobal(m_luaInstance, "loadSetting");

    lua_pushcfunction(m_luaInstance, renderImage);
    lua_setglobal(m_luaInstance, "renderImage");

    lua_pushcfunction(m_luaInstance, saveImage);
    lua_setglobal(m_luaInstance, "saveImage");

    lua_pushcfunction(m_luaInstance, setCamPoI); 
    lua_setglobal(m_luaInstance, "setCamPoI");

    lua_pushcfunction(m_luaInstance, setCamPos); 
    lua_setglobal(m_luaInstance, "setCamPos");

    lua_pushcfunction(m_luaInstance, setCamFoV);
    lua_setglobal(m_luaInstance, "setCamFoV");

    lua_pushcfunction(m_luaInstance, setObjTrans);
    lua_setglobal(m_luaInstance, "setObjTrans");

    lua_pushcfunction(m_luaInstance, setObjScale);
    lua_setglobal(m_luaInstance, "setObjScale");

    lua_pushcfunction(m_luaInstance, setObjTexture);
    lua_setglobal(m_luaInstance, "setObjTexture");

    lua_pushcfunction(m_luaInstance, setPatFreq);
    lua_setglobal(m_luaInstance, "setPatFreq");

    lua_pushcfunction(m_luaInstance, setEulerRot);
    lua_setglobal(m_luaInstance, "setEulerRot");

    lua_pushcfunction(m_luaInstance, setEulerOrder);
    lua_setglobal(m_luaInstance, "setEulerOrder");

    lua_pushcfunction(m_luaInstance, setBlackHoleRadius);
    lua_setglobal(m_luaInstance, "setBlackHoleRadius");

    lua_pushcfunction(m_luaInstance, setBlackHoleColor);
    lua_setglobal(m_luaInstance, "setBlackHoleColor");

    lua_pushcfunction(m_luaInstance, setBlackHoleFlatShading);
    lua_setglobal(m_luaInstance, "setBlackHoleFlatShading");
    
    lua_pushcfunction(m_luaInstance, setViewMode);
    lua_setglobal(m_luaInstance, "setViewMode");

    lua_pushcfunction(m_luaInstance, setMaxTessLevel);
    lua_setglobal(m_luaInstance, "setMaxTessLevel");

    lua_pushcfunction(m_luaInstance, setTessFactor);
    lua_setglobal(m_luaInstance, "setTessFactor");

    lua_pushcfunction(m_luaInstance, setTessExpon);
    lua_setglobal(m_luaInstance, "setTessExpon");

    lua_pushcfunction(m_luaInstance, setTessDistRelation);
    lua_setglobal(m_luaInstance, "setTessDistRelation");

    lua_pushcfunction(m_luaInstance, setLightSourceActive);
    lua_setglobal(m_luaInstance, "setLightSourceActive");

    lua_pushcfunction(m_luaInstance, setLightSourcePos);
    lua_setglobal(m_luaInstance, "setLightSourcePos");

    lua_pushcfunction(m_luaInstance, setLightSourceFactor);
    lua_setglobal(m_luaInstance, "setLightSourceFactor");

    lua_pushcfunction(m_luaInstance, setClearColor);
    lua_setglobal(m_luaInstance, "setClearColor");

    lua_pushcfunction(m_luaInstance, setWinSize);
    lua_setglobal(m_luaInstance, "setWindowSize");

    lua_pushcfunction(m_luaInstance, setCoordSysVisible);
    lua_setglobal(m_luaInstance, "setCoordSysVisible");

    lua_pushcfunction(m_luaInstance, setCrossHairsVisible);
    lua_setglobal(m_luaInstance, "setCrossHairsVisible");
    
    return true;
}

void LRunFile(const char* filename)
{
    int res = luaL_dofile(m_luaInstance, filename);
    if (res != 0) {
        fprintf(stderr, "Lua Error: 0x%x\n", res);
        fprintf(stderr, "  %s:\n", lua_tostring(m_luaInstance, -1));
    }
}