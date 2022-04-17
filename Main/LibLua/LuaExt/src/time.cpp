// -------------------------------------------------------------
// LuaExt
//      Copyright(C) 2010-2022 Pr. Sidi HAMADY
//      http://www.hamady.org
//      sidi@hamady.org
//
//      :STABLE:VERSION180:BUILD2104:
//
//      Released under the MIT licence (https://opensource.org/licenses/MIT)
//      See Copyright Notice in COPYRIGHT
// -----------------------------------------------------------------------------------

#define LUAEXT_LIB

#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#endif

#include "LuaExt.h"

// TIME
LUAEXT_API int time_tic(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        return luaL_errorlevel(pLua, "time.tic: invalid number of argument (should be 0)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

#ifdef WIN32
    double fTic = (static_cast<double>(clock()) / static_cast<double>(CLOCKS_PER_SEC)) * 1000.0;
#else
    struct timeval itv;
    long iseconds, iuseconds;
    gettimeofday(&itv, NULL);
    iseconds = itv.tv_sec;
    iuseconds = itv.tv_usec;
    double fTic = ((static_cast<double>(iseconds) * 1000.0) + (static_cast<double>(iuseconds) / 1000.0)) + 0.5;
#endif
    pEngine->setTic(fTic);

    return 0;
}

#define TIME_MAXSLEEP 3600000
LUAEXT_API int time_sleep(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, "time.sleep: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, "time.sleep: invalid argument (not a number)");
    }
    int iT = static_cast<int>(lua_tonumber(pLua, 1));
    if ((iT < 1) || (iT > TIME_MAXSLEEP)) {
        return luaL_errorlevel(pLua, "time.sleep: invalid argument (should be between 1 and 3600000 ms)");
    }

    Tsleep(iT);

    return 0;
}

#define LUAEXT_LIB
LUAEXT_API int time_toc(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        return luaL_errorlevel(pLua, "time.toc: invalid number of argument (should be 0)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    double fTic = pEngine->getTic();

#ifdef WIN32
    double fToc = (static_cast<double>(clock()) / static_cast<double>(CLOCKS_PER_SEC)) * 1000.0;
#else
    struct timeval itv;
    long iseconds, iuseconds;
    gettimeofday(&itv, NULL);
    iseconds = itv.tv_sec;
    iuseconds = itv.tv_usec;
    double fToc = ((static_cast<double>(iseconds) * 1000.0) + (static_cast<double>(iuseconds) / 1000.0)) + 0.5;
#endif
    fToc -= fTic;
    if (fToc < 0.0) {
        fToc = 0.0;
    }

    pEngine->setToc(fToc);

    lua_pushnumber(pLua, fToc);
    return 1;
}

LUAEXT_API int time_format(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, "time.format: invalid number of argument (should be 1)");
    }

    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, "time.format: invalid argument (not a number)");
    }
    double fT = static_cast<double>(lua_tonumber(pLua, 1));
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

    lua_pushstring(pLua, static_cast<const char *>(szT));
    return 1;
}
//

LUAEXT_API int ext_beep(lua_State *pLua)
{
    // :TODO: Linux version to be developed
#ifdef WIN32
    int nArgCount = lua_gettop(pLua);
    if (nArgCount != 2) {
        return luaL_errorlevel(pLua, "beep: invalid number of argument (should be 2)");
    }

    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2)) {
        return luaL_errorlevel(pLua, "beep: invalid window dimension");
    }

    int ifrequency = (int)(lua_tonumber(pLua, 1));
    if ((ifrequency < 100) || (ifrequency > 5000)) {
        return luaL_errorlevel(pLua, "beep: invalid argument #1 (should be between 100 and 5000)");
    }
    int iduration = (int)(lua_tonumber(pLua, 2));
    if ((iduration < 100) || (iduration > 10000)) {
        return luaL_errorlevel(pLua, "beep: invalid argument #1 (should be between 100 and 10000)");
    }

    Beep(ifrequency, iduration);
#endif
    return 0;
}
