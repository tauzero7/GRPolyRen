/**
 * File:    Renderer.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_LUAHANDLE_H
#define GRPR_LUAHANDLE_H

#include <iostream>
#include <cstring>

extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

void LClose();

bool LInit();

void LRunFile(const char* filename);

/**
 * @brief Load object file (wavefront obj file only)  
 *
 * Lua: loadObject(filename)
 */
int loadObject(lua_State* L);

/**
 * @brief Load configure file
 * 
 * Lua: loadSetting(filename)
 */
int loadSetting(lua_State* L);

/**
 * @brief Render image
 *
 * Lua: renderImage()
 */
int renderImage(lua_State* L);

/**
 * @brief Save image to file
 * 
 * Lua: saveImage(filename)
 */
int saveImage(lua_State* L);

/**
 * @brief Set camera's point of interest
 * 
 * Lua: setCamPoI(x, y, z)
 */
int setCamPoI(lua_State* L);

/**
 * @brief Set camera position
 * 
 * Lua: setCamPos(x, y, z)
 */
int setCamPos(lua_State* L);

/**
 * @brief Set camera's field of view [degree]
 *
 * Lua: setCamFoV(fov)
 */
int setCamFoV(lua_State* L);

/**
 * @brief Set scaling factor for object
 * 
 * Lua: setObjScale(sx, sy, sz)
 */
int setObjScale(lua_State* L);

/**
 * @brief Set object translation
 *
 * Lua: setObjTrans(tx, ty, tz)
 */
int setObjTrans(lua_State* L);

int setObjTexture(lua_State* L);

int setPatFreq(lua_State* L);
int setEulerRot(lua_State* L);
int setEulerOrder(lua_State* L);
int setBlackHoleColor(lua_State* L);
int setBlackHoleRadius(lua_State* L);

/**
 * @brief Set view mode ("Flat", "GR", "GRgeom", "GRtess")
 *
 * Lua: setViewMode("mode")
 */
int setViewMode(lua_State* L);

int setMaxTessLevel(lua_State* L);
int setTessFactor(lua_State* L);
int setTessExpon(lua_State* L);
int setLightSourceActive(lua_State* L);
int setClearColor(lua_State* L);
int setWinSize(lua_State* L);


template <typename T> bool getVector(lua_State* L, T* vec, int dim = 3) {    
    int num = lua_gettop(L);

    size_t i = 0;
    if (num == dim) {
        while(num) {
            if (!lua_isnil(L,-num) && lua_isnumber(L,-num)) {
                vec[i++] = static_cast<T>(lua_tonumber(L,-num));
            }
            --num;
        }
        lua_pop(L, lua_gettop(L));
        return true;
    }
    return false;
}


#endif // GRPR_LUAHANDLE_H
