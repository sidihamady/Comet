// -----------------------------------------------------------------------------------
// Comet <Programming Environment for Lua>
//      Copyright(C) 2010-2022 Pr. Sidi HAMADY
//      http://www.hamady.org
//      sidi@hamady.org
//
//      :STABLE:VERSION180:BUILD2104:
//
//      Released under the MIT licence (https://opensource.org/licenses/MIT)
//      See Copyright Notice in COPYRIGHT
// -----------------------------------------------------------------------------------


#include "../Identifiers.h"

#include <math.h>
#include <float.h>

#include "../ScriptEdit.h"

#include "../SigmaThread.h"

// TIME
int time_tic(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        return luaL_errorlevel(pLua, 2, "time.tic: invalid argument");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = (LuaEngine *)lua_touserdata(pLua, -1);

    double fTic = (((double)clock()) / (double)CLOCKS_PER_SEC) * 1000.0;
    pEngine->setTic(fTic);

    return 0;
}

#define TIME_MAXSLEEP 3600000
int time_sleep(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "time.sleep: invalid number of arguments (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "time.sleep: invalid argument (not a number)");
    }
    int iSleep = (int)lua_tonumber(pLua, 1);
    if ((iSleep < 5) || (iSleep > TIME_MAXSLEEP)) {
        return luaL_errorlevel(pLua, 2, "time.sleep: invalid argument (should be between 5 and 3600000 ms)");
    }

    int iCount = iSleep / 100;
    int iRemain = iSleep % 100;
    int iPeriod = 100, iTime = 0;

    lua_pushliteral(pLua, "LuaThread");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    SigmaThread *pThread = (SigmaThread *)lua_touserdata(pLua, -1);

    if (iCount > 0) {
        for (int ii = 0; ii < iCount; ii++) {
            Tsleep(iPeriod);

            iTime += iPeriod;
            if (iTime >= iSleep) {
                break;
            }

            if (pThread != NULL) {
                if (pThread->stopIt()) {
                    break;
                }
            }
        }
    }

    if (iRemain > 0) {
        Tsleep(iRemain);
    }

    return 0;
}

int time_toc(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        return luaL_errorlevel(pLua, 2, "time.toc: invalid argument");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = (LuaEngine *)lua_touserdata(pLua, -1);

    double fTic = pEngine->getTic();

    double fToc = (((double)clock()) / (double)CLOCKS_PER_SEC) * 1000.0;
    fToc -= fTic;
    if (fToc < 0.0) {
        fToc = 0.0;
    }
    pEngine->setToc(fToc);

    lua_pushnumber(pLua, fToc);
    return 1;
}

int time_format(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "time.format: invalid number of argument (should be 1)");
    }

    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "time.format: invalid argument (not a number)");
    }
    double fT = (double)lua_tonumber(pLua, 1);
    if (fT < 0.0) {
        fT = 0.0;
    }

    char szT[LM_STRSIZEN];
    memset(szT, 0, LM_STRSIZEN * sizeof(char));
    if (fT < 1.0) { // < 1 ms.
        strcpy(szT, "< 1 ms");
    }
    else if ((fT >= 1.0) && (fT < 10000.0)) { // 10000 ms.
        sprintf(szT, "%.0f ms", fT);
    }
    else if ((fT >= 10000.0) && (fT < 600000.0)) { // entre 10 sec. 600 sec.
        sprintf(szT, "%.3f seconds", fT / 1000.0);
    }
    else if ((fT >= 600000.0) && (fT < 36000000.0)) { // entre 10 min. et 600 min.
        sprintf(szT, "%.3f minutes", fT / 60000.0);
    }
    else if (fT >= 36000000.0) { // plus de 10 heures
        sprintf(szT, "%.3f hours", fT / 3600000.0);
    }

    lua_pushstring(pLua, (const char *)szT);
    return 1;
}
//
