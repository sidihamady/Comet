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

#include <math.h>
#include <float.h>
#include <string.h>
#include <setjmp.h>

#ifdef WIN32
#include <direct.h>
#include <winsock2.h>
#include <eh.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
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

#ifdef __cplusplus
extern "C"
{
#endif
void SharedLibEntry(void)
{
#ifdef _WIN32
    Tprintf(uT("\nLua Library for Comet version %s build %s (C) 2010-2021  Pr. Sidi HAMADY\nhttp://www.hamady.org\n"), LUAEXT_VERSION_STRING, LUAEXT_BUILD_STRING);
#else
    Tprintf(uT("\nLua Library for Comet version %ls build %ls (C) 2010-2021  Pr. Sidi HAMADY\nhttp://www.hamady.org\n"), LUAEXT_VERSION_STRING, LUAEXT_BUILD_STRING);
#endif
    exit(0);
}
#ifdef __cplusplus
}
#endif

#ifndef lua_pushglobaltable
#define lua_pushglobaltable(pLua)        lua_pushvalue(pLua, LUA_GLOBALSINDEX)
#endif

luaexception::luaexception(const char *psz)
{
    if (psz) {
        strcpy(m_szMsg, psz);
    }
}
#if defined(WIN32) || (__GNUC__ < 4) || (__GNUC__ == 4 && __GNUC_MINOR__ < 7)
const char* luaexception::what() const throw()
#else
const char* luaexception::what() const noexcept
#endif
{
    return (const char*) (m_szMsg);
}

extern "C"
{
    static int throwException(lua_State *pLua)
    {
        char szM[LM_STRSIZE];
        szM[0] = '\0';
        const char* pszRet = const_cast<const char*>(lua_tostring(pLua, -1));
        strncpy(szM, pszRet, LM_STRSIZE - 1);
        luaexception excT(const_cast<const char*>(szM));
        lua_gc(pLua, LUA_GCCOLLECT, 0);        // Force a complete garbage collection in case of errors
        throw(excT);

        // Never reached
        return 0;
    }
}

#ifdef WIN32

static void seException(unsigned int, EXCEPTION_POINTERS*)
{
    throw(1);
}

#else

static int wrapExceptions(lua_State *pLua, lua_CFunction lfunc)
{
    try {
        return lfunc(pLua);                // Call wrapped function and return result.
    }
    catch (const char *szT) {            // Catch and convert exceptions.
        lua_pushstring(pLua, szT);
    }
    catch (std::exception& excT) {
        lua_pushstring(pLua, excT.what());
    }
    catch (...) {
        bool bExc = false;
        try {
            throw;
        }
        catch (const luaexception &excT) {
            lua_pushstring(pLua, excT.what());
            bExc = true;
        }
        if (false == bExc) {
            lua_pushliteral(pLua, "! unrecoverable error");
        }
    }

    return lua_error(pLua);                // Rethrow as a Lua error.
}

#endif

LUAEXT_API int gui_directoryexists(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, "directoryexists: invalid number of argument (should be 1)");
    }

    if (!lua_isstring(pLua, 1)) {
        return luaL_errorlevel(pLua, "directoryexists: invalid directory name");
    }
    const char* pszDirname = (const char*) lua_tostring(pLua, 1);
    if (pszDirname == NULL) {
        return luaL_errorlevel(pLua, "directoryexists directory name");
    }

    int iExists = lf_dirExists(pszDirname);
    lua_pushinteger(pLua, iExists);
    return 1;
}

const luaL_Reg LuaEngine::CFUNCTION[] =
{
    // SOCKET
    { "socket_new", socket_new },
    { "socket_setsockopt", socket_setsockopt },
    { "socket_bind", socket_bind },
    { "socket_listen", socket_listen },
    { "socket_connect", socket_connect },
    { "socket_accept", socket_accept },
    { "socket_timeout", socket_timeout },
    { "socket_getsockname", socket_getsockname },
    { "socket_getpeername", socket_getpeername },
    { "socket_gethostname", socket_gethostname },
    { "socket_gethostbyaddr", socket_gethostbyaddr },
    { "socket_gethostbyname", socket_gethostbyname },
    { "socket_send", socket_send },
    { "socket_sendto", socket_sendto },
    { "socket_recv", socket_recv },
    { "socket_recvfrom", socket_recvfrom },
    { "socket_close", socket_close },
    { "socket_delete", socket_delete },
    { "socket_iserr", socket_iserr },
    { "socket_geterr", socket_geterr },
    //

    // TIME
    { "time_tic", time_tic },
    { "time_toc", time_toc },
    { "time_sleep", time_sleep },
    { "time_format", time_format },
    //

    // MISC
    { "ext_beep", ext_beep },
    { "gui_directoryexists", gui_directoryexists },
    //

    { NULL, NULL }
};

FILE* LuaEngine::LUADUMPFIlE = NULL;

extern "C"
{

    LUAEXT_API int luaL_errorlevel(lua_State *pLua, const char *fmt, ...)
    {
        va_list argp;
        va_start(argp, fmt);
        // add prefix with the line number
        lua_pushfstring(pLua, "<!line=%d!>", getCurrentLine(pLua));
        lua_pushvfstring(pLua, fmt, argp);
        va_end(argp);
        lua_concat(pLua, 2);

#ifndef WIN32
        // get error message and throw exception
        throwException(pLua);
#endif

        return lua_error(pLua);
    }
}

void LuaEngine::setInteract(const luaL_Reg *pFuncInteract)
{
    if ((m_pFuncInteract != NULL) || (pFuncInteract == NULL) || (m_pLuaState == NULL)) {
        return;
    }

    m_pFuncInteract = pFuncInteract;

    lua_Hook pHook = lua_gethook(m_pLuaState);
    if (pHook) {
        lua_sethook(m_pLuaState, NULL, LUA_MASKLINE, 0);
    }

    int ii = 0;

    while (pFuncInteract[ii].name != NULL) {
        lua_register(m_pLuaState, pFuncInteract[ii].name, pFuncInteract[ii].func);
        ii += 1;
    }
}
void LuaEngine::create()
{
    m_pFuncInteract = NULL;

    m_bMode = LUA_ENGINE_GUI;
    m_pSigma = NULL;

    m_bOK = true;
    m_pLuaState = NULL;
    Tmemset(m_szMessage, 0, LM_STRSIZE);
    memset(m_szMessageA, 0, LM_STRSIZE * sizeof(char));

    m_iErrLine = -1;

    m_fTic = 0.0;
    m_fToc = 0.0;

    m_SocketMap.clear();
    m_GlobalSet.clear();

    m_iSocketId = 1L;

    m_bDebugging = false;
}

LuaEngine::LuaEngine()
{
    create();

    assignResources(true);
}

LUAEXT_API LuaEngine::LuaEngine(const luaL_Reg *pFuncInteract)
{
    create();

    assignResources(true);
    setInteract(pFuncInteract);
}

LUAEXT_API LuaEngine::LuaEngine(bool bMode, const luaL_Reg *pFuncInteract, bool bOpenLibs/* = true*/)
{
    create();

    m_bMode = bMode;
    assignResources(bOpenLibs);
    setInteract(pFuncInteract);
}

LUAEXT_API LuaEngine::~LuaEngine()
{
    releaseResources();
}

void LuaEngine::assignResources(bool bOpenLibs/* = true*/)
{
    releaseResources();

    m_pSigma = new(std::nothrow) lmSigma();

    m_pLuaState = luaL_newstate();
    if (m_pLuaState == NULL) {
        showMessage(uT("Cannot initialize scripting engine: insufficient memory"));
        return;
    }

    lua_pushliteral(m_pLuaState, "___LuaEngine___");
    lua_pushlightuserdata(m_pLuaState, static_cast<void*>(this));
    lua_settable(m_pLuaState, LUA_REGISTRYINDEX);

#ifdef WIN32
    lua_atpanic(m_pLuaState, throwException);
    _set_se_translator(seException);
#else
    // Define C++ exception wrapper function
    lua_pushlightuserdata(m_pLuaState, (void *) wrapExceptions);
    luaJIT_setmode(m_pLuaState, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_ON);
    lua_pop(m_pLuaState, 1);
#endif

    if (bOpenLibs) {
        openLibs();
    }

    LuaEngine::LUADUMPFIlE = NULL;

    return;
}

void LuaEngine::openLibs(void)
{
    if (m_pLuaState == NULL) {
        return;
    }

#ifndef USE_LUAJIT
    luaL_checkversion(m_pLuaState);
#endif
    lua_gc(m_pLuaState, LUA_GCSTOP, 0);        // stop collector during initialization
    luaL_openlibs(m_pLuaState);                // open libraries
    lua_gc(m_pLuaState, LUA_GCRESTART, 0);

    initModules();

    return;
}

void LuaEngine::releaseResources(void)
{
    if (m_pLuaState != NULL) {
        lua_close(m_pLuaState);
        m_pLuaState = NULL;
    }

    if (m_SocketMap.empty() == false) {
        std::map<long, lmSocket*>::iterator itr;
        lmSocket *pSocket = NULL;
        int ii = 0;
        int iSize = (int) (m_SocketMap.size());
        for (itr = m_SocketMap.begin(), ii = 0; (itr != m_SocketMap.end()) && (ii < iSize); ++itr, ii++) {
            pSocket = itr->second;
            if (pSocket) {
                pSocket->release();
                delete pSocket;
                pSocket = NULL;
            }
        }
        m_SocketMap.clear();
    }

    m_GlobalSet.clear();

    if (m_pSigma) {
        delete m_pSigma;
        m_pSigma = NULL;
    }

    return;
}

#define DEBUG_VARSIZE        (LM_STRSIZE >> 1)
#define DEBUG_MAXCOUNT        64

LUAEXT_API bool LuaEngine::debugGetVariable(const char *pszN, char *pszV, char *pszT)
{
    int nn, ne, ilen, jj, itop;
    char *pszArg = NULL, *pszType = NULL;

    int iType = lua_type(m_pLuaState, -1);
    pszType = const_cast<char*>(lua_typename(m_pLuaState, iType));
    bool bFlag = (pszType && (*pszType != '\0'));
    if (bFlag == false) {
        return false;
    }

    switch (iType) {
        case LUA_TSTRING:
            strncpy(pszV, (const char*) lua_tostring(m_pLuaState, -1), DEBUG_VARSIZE - 1);
            snprintf(pszT, LM_STRSIZE - 1, "%s \t%s \t%s \n", static_cast<const char*>(pszN), static_cast<const char*>(pszV), static_cast<const char*>(pszType));
            break;
        case LUA_TBOOLEAN:
            strcpy(pszV, lua_toboolean(m_pLuaState, -1) ? "true" : "false");
            snprintf(pszT, LM_STRSIZE - 1, "%s \t%s \t%s \n", static_cast<const char*>(pszN), static_cast<const char*>(pszV), static_cast<const char*>(pszType));
            break;
        case LUA_TNUMBER:
            if (strcmp(static_cast<const char*>(pszN), "*") == 0) {
                double ft = lua_tonumber(m_pLuaState, -1);
                if (ft < DBL_EPSILON) {
                    snprintf(pszT, LM_STRSIZE - 1, "%s \t0 \t%s \n", static_cast<const char*>(pszN), static_cast<const char*>(pszType));
                }
                else {
                    snprintf(pszV, LM_STRSIZEN - 1, "%g", ft);
                    snprintf(pszT, LM_STRSIZE - 1, "%s \t%s \t%s \n", static_cast<const char*>(pszN), static_cast<const char*>(pszV), static_cast<const char*>(pszType));
                }
            }
            else {
                snprintf(pszV, LM_STRSIZEN - 1, "%g", lua_tonumber(m_pLuaState, -1));
                snprintf(pszT, LM_STRSIZE - 1, "%s \t%s \t%s \n", static_cast<const char*>(pszN), static_cast<const char*>(pszV), static_cast<const char*>(pszType));
            }
            break;
        case LUA_TTABLE:
            nn = (int) luaL_len(m_pLuaState, -1);
            ne = (nn < 10) ? nn : 10;
            if (ne > 0) {
                strcpy(pszV, "{ ");
                for (jj = 1; jj <= ne; jj++) {
                    ilen = (int) strlen((const char*) pszV);
                    if (ilen >= (DEBUG_VARSIZE - 12)) {
                        strcat(pszV, " ...");
                        break;
                    }
                    lua_rawgeti(m_pLuaState, -1, jj);
                    itop = lua_gettop(m_pLuaState);
                    if (itop == 0) {
                        // Empty stack
                        break;
                    }
                    pszArg = (char*) lua_tostring(m_pLuaState, itop);
                    if (pszArg != NULL) {
                        strncat(pszV, (const char*) pszArg, (DEBUG_VARSIZE - 12) - ilen);
                        if (jj < ne) {
                            strcat(pszV, ", ");
                        }
                    }
                    lua_pop(m_pLuaState, 1);
                }
                if ((nn > 10) && (jj >= ne)) {
                    strcat(pszV, " ...");
                }
                strcat(pszV, " }");
            }
            else {
                snprintf(pszV, LM_STRSIZEN - 1, "%lX", reinterpret_cast<unsigned long>(lua_topointer(m_pLuaState, -1)));
            }
            snprintf(pszT, LM_STRSIZE - 1, "%s \t%s \t%s \n", static_cast<const char*>(pszN), static_cast<const char*>(pszV), static_cast<const char*>(pszType));
            break;
        case LUA_TFUNCTION:
            snprintf(pszV, LM_STRSIZEN - 1, "%lX", reinterpret_cast<unsigned long>(lua_topointer(m_pLuaState, -1)));
            snprintf(pszT, LM_STRSIZE - 1, "%s \t%s \t%s \n", static_cast<const char*>(pszN), static_cast<const char*>(pszV), static_cast<const char*>(pszType));
            break;
        case LUA_TLIGHTUSERDATA:
            snprintf(pszV, LM_STRSIZEN - 1, "%lX", reinterpret_cast<unsigned long>(lua_topointer(m_pLuaState, -1)));
            snprintf(pszT, LM_STRSIZE - 1, "%s \t%s \t%s \n", static_cast<const char*>(pszN), static_cast<const char*>(pszV), static_cast<const char*>(pszType));
            break;
        case LUA_TUSERDATA:
            snprintf(pszV, LM_STRSIZEN - 1, "%lX", reinterpret_cast<unsigned long>(lua_topointer(m_pLuaState, -1)));
            snprintf(pszT, LM_STRSIZE - 1, "%s \t%s \t%s \n", static_cast<const char*>(pszN), static_cast<const char*>(pszV), static_cast<const char*>(pszType));
            break;
        case LUA_TTHREAD:
            snprintf(pszV, LM_STRSIZEN - 1, "%lX", reinterpret_cast<unsigned long>(lua_topointer(m_pLuaState, -1)));
            snprintf(pszT, LM_STRSIZE - 1, "%s \t%s \t%s \n", static_cast<const char*>(pszN), static_cast<const char*>(pszV), static_cast<const char*>(pszType));
            break;
        case LUA_TNIL:
            bFlag = false;
            break;
        case LUA_TNONE:
            bFlag = false;
            break;
        default:
            bFlag = false;
            break;
    }

    return bFlag;
}

LUAEXT_API bool LuaEngine::debugStackTrace(char *pszBuffer, int iLen, lua_Debug *pDebug)
{
    if (iLen < LM_STRSIZE) {
        return false;
    }

    int ii = 0, iLenT = 0;
    char szN[LM_STRSIZEN];
    char szT[LM_STRSIZE], szV[LM_STRSIZE >> 1];
    char szPrev[LM_STRSIZE];
    memset(szN, 0, LM_STRSIZEN * sizeof(char));
    memset(szT, 0, LM_STRSIZE * sizeof(char));
    memset(szV, 0, DEBUG_VARSIZE * sizeof(char));
    memset(szPrev, 0, LM_STRSIZE * sizeof(char));
    memset(pszBuffer, 0, iLen * sizeof(char));
    char *pszName = NULL, *pszT = NULL;
    bool bFlag = false;

    ii = 0;

    while (ii < DEBUG_MAXCOUNT) {

        if ((pszName = const_cast<char*>(lua_getlocal(m_pLuaState, pDebug, ii))) != NULL) {

            if (strcmp(static_cast<const char*>(pszName), "(*temporary)") == 0) {
                strcpy(szN, "*");
            }
            else {
                strcpy(szN, static_cast<const char*>(pszName));
            }

            bFlag = debugGetVariable(static_cast<const char*>(szN), static_cast<char*>(szV), static_cast<char*>(szT));

#ifdef WIN32
            if (bFlag && (stricmp(static_cast<const char*>(szPrev), static_cast<const char*>(szT)) != 0)) {
#else
            if (bFlag && (strcasecmp(static_cast<const char*>(szPrev), static_cast<const char*>(szT)) != 0)) {
#endif
                pszT = static_cast<char*>(szT);
                while (*pszT) {
                    if ((*pszT != '\n') && (*pszT != '\t') && (*pszT != ' ') && (*pszT != '*')) {
                        break;
                    }
                    pszT += 1;
                }
                if (*pszT) {
                    iLenT += static_cast<int>(strlen(static_cast<const char*>(szT)));
                    if (iLenT >= iLen) {
                        lua_pop(m_pLuaState, 1);    // remove variable value
                        return false;
                    }
                    strcat(pszBuffer, static_cast<const char*>(szT));
                    strcpy(szPrev, static_cast<const char*>(szT));
                }
            }
            lua_pop(m_pLuaState, 1);    // remove variable value
        }

        if (ii++ >= DEBUG_MAXCOUNT) {
            break;
        }
    }

    iLenT += 20;
    if (iLenT >= iLen) {
        return false;
    }

    // Globals
    ii = 0;

    lua_pushglobaltable(m_pLuaState);
    lua_pushnil(m_pLuaState);

    while (lua_next(m_pLuaState, -2) != 0) {            // pop nil, push name,value

        if (m_GlobalSet.find(lm_hash((const char*) lua_tostring(m_pLuaState, -2))) == m_GlobalSet.end()) {

            pszName = const_cast<char*>(lua_tostring(m_pLuaState, -2));
            if (strcmp(static_cast<const char*>(pszName), "(*temporary)") == 0) {
                strcpy(szN, "*");
            }
            else {
                strcpy(szN, static_cast<const char*>(pszName));
            }

            bFlag = debugGetVariable(static_cast<const char*>(szN), static_cast<char*>(szV), static_cast<char*>(szT));

#ifdef WIN32
            if (bFlag && (stricmp(static_cast<const char*>(szPrev), static_cast<const char*>(szT)) != 0)) {
#else
            if (bFlag && (strcasecmp(static_cast<const char*>(szPrev), static_cast<const char*>(szT)) != 0)) {
#endif
                pszT = static_cast<char*>(szT);
                while (*pszT) {
                    if ((*pszT != '\n') && (*pszT != '\t') && (*pszT != ' ') && (*pszT != '*')) {
                        break;
                    }
                    pszT += 1;
                }
                if (*pszT) {
                    iLenT += static_cast<int>(strlen(static_cast<const char*>(szT)));
                    if (iLenT >= iLen) {
                        lua_pop(m_pLuaState, 1);                                // remove value
                        lua_pop(m_pLuaState, 1);                                // remove global table
                        return false;
                    }
                    strcat(pszBuffer, static_cast<const char*>(szT));
                    strcpy(szPrev, static_cast<const char*>(szT));
                }
            }

            }

        lua_pop(m_pLuaState, 1);                                                // remove value

        if (ii++ >= DEBUG_MAXCOUNT) {
            break;
        }
    }

    lua_pop(m_pLuaState, 1);                                                    // remove global table
    //

    iLenT += 20;
    if (iLenT >= iLen) {
        return false;
    }

    strcat(pszBuffer, "\nSTACKTRACE\n");

    lua_Debug ldT;
    int iDepth = 0;

    while (iDepth <= DEBUG_MAXCOUNT) {
        if (lua_getstack(m_pLuaState, iDepth, &ldT) != 1) {
            break;
        }
        lua_getinfo(m_pLuaState, "nSl", &ldT);
        if (ldT.currentline < 0) {
            ldT.currentline = 0;
        }

        if (ldT.name && (*(ldT.name) != '\0')) {
            strncpy(szV, ldT.name, (LM_STRSIZE >> 1) - 1);
        }
        else if (ldT.namewhat && (*(ldT.namewhat) != '\0')) {
            strncpy(szV, ldT.namewhat, (LM_STRSIZE >> 1) - 1);
        }
        else if (ldT.what) {
            if (*(ldT.what) == 'm') {
                strcpy(szV, "main");
            }
            else if (*(ldT.what) == 'C') {
                strcpy(szV, "C");
            }
            else if (*(ldT.what) == 't') {
                strcpy(szV, "tail");
            }
            else if (ldT.short_src && (*(ldT.short_src) != '\0')) {
                strncpy(szV, ldT.short_src, LM_STRSIZES - 1);
            }
            else {
                strcpy(szV, "?");
            }
        }
        else {
            strcpy(szV, "?");
        }

        snprintf(szT, LM_STRSIZE - 1, "%d \t%d \t%s \t%s \n", iDepth, ldT.currentline, static_cast<const char*>(szV), ldT.what);
        iLenT += static_cast<int>(strlen(static_cast<const char*>(szT)));
        if (iLenT >= iLen) {
            break;
        }
        strcat(pszBuffer, static_cast<const char*>(szT));
        iDepth += 1;
    }

    return true;
}

LUAEXT_API long LuaEngine::socketNew(long iId, int iFamily/* = AF_INET*/, int iType/* = SOCK_STREAM*/,
    int iProtocol/* = IPPROTO_TCP*/)
{
    lmSocket* pSocket = NULL;
    try {
        if (m_SocketMap.empty() == false) {
            std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
            if (itr != m_SocketMap.end()) {
                return itr->first;
            }
        }
        if (m_SocketMap.size() >= 1024) {
            return 0L;
        }
        iId = m_iSocketId;
        pSocket = new(std::nothrow) lmSocket(iId, iFamily, iType, iProtocol);
        if (pSocket) {
            if (pSocket->iserr() == false) {
                m_iSocketId++;
                m_SocketMap.insert(std::pair<long, lmSocket*>(iId, pSocket));
                pSocket->setId(iId);
                return iId;
            }
            pSocket->release();
            delete pSocket;
            pSocket = NULL;
        }

        return 0L;
    }
    catch (...) {
        if (pSocket) {
            pSocket->release();
            delete pSocket;
            pSocket = NULL;
            std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
            if (itr != m_SocketMap.end()) {
                m_SocketMap.erase(iId);
            }
        }
        return 0L;
    }
}

LUAEXT_API bool LuaEngine::socketSetsockopt(long iId, int iOpt, long lBuf)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                pSocket->setsockopt(iOpt, lBuf);
                return true;
            }
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

bool LuaEngine::socketBind(long iId, int iPort, long lInaddr/* = INADDR_ANY*/)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                pSocket->bind(iPort, lInaddr);
                return true;
            }
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

LUAEXT_API bool LuaEngine::socketListen(long iId, int iBacklog/* = 5*/)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                pSocket->listen(iBacklog);
                return true;
            }
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

LUAEXT_API bool LuaEngine::socketConnect(long iId, const char* pszServer, int iPort)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                pSocket->connect(pszServer, iPort);
                return true;
            }
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

LUAEXT_API long LuaEngine::socketAccept(long iId)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                lmSocket *pSocketAccepted = pSocket->accept();
                if (pSocketAccepted) {
                    iId = m_iSocketId++;
                    m_SocketMap.insert(std::pair<long, lmSocket*>(iId, pSocket));
                    pSocketAccepted->setId(iId);
                    return iId;
                }
            }
        }

        return 0L;
    }
    catch (...) {
        return 0L;
    }
}

LUAEXT_API bool LuaEngine::socketTimeout(long iId, int iTimeout)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                pSocket->timeout(iTimeout);
                return true;
            }
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

LUAEXT_API bool LuaEngine::socketGetpeername(long iId, char* pszRetA)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                pSocket->getpeername(pszRetA);
                return true;
            }
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

LUAEXT_API bool LuaEngine::socketGethostname(long iId, char* pszRetA)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                pSocket->gethostname(pszRetA);
                return true;
            }
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

LUAEXT_API bool LuaEngine::socketGethostbyname(long iId, const char* pszT, char* pszRetA)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                pSocket->gethostbyname(pszT, pszRetA);
                return true;
            }
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

LUAEXT_API bool LuaEngine::socketGetsockname(long iId, char* pszRetA)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                pSocket->getsockname(pszRetA);
                return true;
            }
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

LUAEXT_API bool LuaEngine::socketGethostbyaddr(long iId, const char* pszT, char* pszRetA)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                pSocket->gethostbyaddr(pszT, pszRetA);
                return true;
            }
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

LUAEXT_API int LuaEngine::socketSend(long iId, const char* pszMsg, int iFlags)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                return pSocket->send(pszMsg, iFlags);
            }
        }

        return -1;
    }
    catch (...) {
        return -1;
    }
}

LUAEXT_API int LuaEngine::socketSendto(long iId, const char* pszServer, int iPort, const char* pszMsg, int iFlags)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                return pSocket->sendto(pszServer, iPort, pszMsg, iFlags);
            }
        }

        return -1;
    }
    catch (...) {
        return -1;
    }
}

LUAEXT_API const char* LuaEngine::socketRecv(long iId, int iLen/* = 4096*/, int iFlags/* = 0*/)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                return pSocket->recv(iLen, iFlags);
            }
        }

        return NULL;
    }
    catch (...) {
        return NULL;
    }
}

LUAEXT_API const char* LuaEngine::socketRecvfrom(long iId, const char* pszServer, int iPort, int iLen/* = 4096*/, int iFlags/* = 0*/)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                return pSocket->recvfrom(pszServer, iFlags, iLen, iFlags);
            }
        }

        return NULL;
    }
    catch (...) {
        return NULL;
    }
}

LUAEXT_API bool LuaEngine::socketClose(long iId)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                pSocket->close();
                return true;
            }
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

LUAEXT_API bool LuaEngine::socketShutdown(long iId, int iHow)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                pSocket->shutdown(iHow);
                return true;
            }
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

LUAEXT_API bool LuaEngine::socketDelete(long iId)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                pSocket->release();
                delete pSocket;
                pSocket = NULL;
            }
            m_SocketMap.erase(itr->first);
            return true;
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

LUAEXT_API bool LuaEngine::socketIserr(long iId)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                return pSocket->iserr();
            }
            return false;
        }

        return false;
    }
    catch (...) {
        return false;
    }
}

LUAEXT_API const char* LuaEngine::socketGeterr(long iId, const char *pszDefaultMsg)
{
    try {
        std::map<long, lmSocket*>::iterator itr = m_SocketMap.find(iId);
        if (itr != m_SocketMap.end()) {
            lmSocket *pSocket = itr->second;
            if (pSocket) {
                return pSocket->geterr();
            }
            return pszDefaultMsg;
        }

        return pszDefaultMsg;
    }
    catch (...) {
        return pszDefaultMsg;
    }
}

inline static int igetLine(FILE* pf, char* pszBufferA, int nCount, int *ieof)
{
    int ii;
    char cr;
    *ieof = 0;

    for (ii = 0; ii < nCount; ii++) {
        cr = (char) fgetc(pf);

        if (feof(pf) != 0) {
            if (ii > 0) {
                if (pszBufferA[ii - 1] != ('\n')) {
                    pszBufferA[ii] = ('\n');
                    pszBufferA[ii + 1] = ('\0');
                    return (ii + 1);
                }
            }
            pszBufferA[ii] = ('\0');
            *ieof = 1;
            return ii;
        }
        pszBufferA[ii] = cr;
        if (pszBufferA[ii] == ('\n')) {
            if (ii > 0) {
                if (pszBufferA[ii - 1] == ('\r')) {
                    pszBufferA[ii - 1] = ('\n');
                    pszBufferA[ii] = ('\0');
                    return ii;
                }
            }
            pszBufferA[ii + 1] = ('\0');
            return (ii + 1);
        }
    }

    return 0;
}

LUAEXT_API bool LuaEngine::runScriptFunction(const char* pszScriptA, const char* pszFuncNameA,
    const double *pa, const double *pb, const double *pc, const double *pd, const double *pe, const double *pf,
    double *pra, double *prb, double *prc, double *prd, double *pre, double *prf, 
    int *pstatus)
{
    if (pstatus == NULL) {
        // should never happen
        updateMessage(uT("invalid status argument"), false);
        return false;
    }

    lua_State *pLua = this->getLuaState();
    if (pLua == NULL) {
        // should never happen
        updateMessage(uT("Lua engine not loaded"), false);
        return false;
    }

    int iRet = -1;

    try {

        // load the script containing the function...
        iRet = luaL_loadstring(pLua, pszScriptA);
        if (iRet != LUA_OK) {
            updateMessage(uT("script not loaded"), true);
            return false;
        }
        // ...and run it
        iRet = lua_pcall(pLua, 0, 0, 0);
        if (iRet != LUA_OK) {
            updateMessage(uT("function not loaded"), true);
            return false;
        }

        int nresultsRet = lua_gettop(pLua);

        // Push the Lua function on the stack
        lua_getglobal(pLua, pszFuncNameA);
        if (!lua_isfunction(pLua, -1)) {
            updateMessage(uT("function not found"), false);
            return false;
        }

        int nargs = 0, nresults = 0;

        // Push the Lua function arguments on the stack
        if (pa != NULL) {
            lua_pushnumberx(pLua, *pa);
            ++nargs;
        }
        if (pb != NULL) {
            lua_pushnumberx(pLua, *pb);
            ++nargs;
        }
        if (pc != NULL) {
            lua_pushnumberx(pLua, *pc);
            ++nargs;
        }
        if (pd != NULL) {
            lua_pushnumberx(pLua, *pd);
            ++nargs;
        }
        if (pe != NULL) {
            lua_pushnumberx(pLua, *pe);
            ++nargs;
        }
        if (pf != NULL) {
            lua_pushnumberx(pLua, *pf);
            ++nargs;
        }

        if (pra != NULL) {
            ++nresults;
        }
        if (prb != NULL) {
            ++nresults;
        }
        if (prc != NULL) {
            ++nresults;
        }
        if (prd != NULL) {
            ++nresults;
        }
        if (pre != NULL) {
            ++nresults;
        }
        if (prf != NULL) {
            ++nresults;
        }

        // add the last returned value, the boolean flag (status)
        // nresults is at least equal to one
        ++nresults;

        // Call the Lua function (this call is slow on Windows, while fast on Android.. why?)
        // lua_pcall always removes the function and its arguments from the stack
        iRet = lua_pcall(pLua, nargs, nresults, 0);
        if (iRet != LUA_OK) {
            updateMessage(uT("function not loaded"), true);
            return false;
        }

        nresultsRet = lua_gettop(pLua) - nresultsRet;
        if (nresultsRet != nresults) {
            updateMessage(uT("function returned values not valid"), false);
            return false;
        }

        // returned values
        int itop = lua_gettop(pLua);
        if (itop != 0) {
            // the boolean flag (status)
            if (lua_isboolean(pLua, itop)) {
                *pstatus = lua_toboolean(pLua, itop);
                lua_pop(pLua, 1);
            }
        }
        if (nresults >= 2) {
            // a,b,c,d,e,f,st            nresults = 7            idx = 5
            // a,b,c,d,e,st            nresults = 6            idx = 4
            // a,b,c,d,st                nresults = 5            idx = 3
            // a,b,c,st                    nresults = 4            idx = 2
            // a,b,st                    nresults = 3            idx = 1
            // a,st                        nresults = 2            idx = 0
            double *pp[] = { pra, prb, prc, prd, pre, prf };
            const int idim = 6;
            int idx = nresults - 2;
            for (int ii = 0; ii < (nresults - 1); ii++) {
                if ((idx < 0) || (idx >= idim) || (pp[idx] == NULL)) {
                    // Empty stack
                    break;
                }
                itop = lua_gettop(pLua);
                if (itop == 0) {
                    // Empty stack
                    break;
                }
                if (!lua_isnumber(pLua, itop)) {
                    updateMessage(uT("invalid function returned value"), false);
                    return false;
                }
                *pp[idx] = (double) lua_tonumber(pLua, itop);
                --idx;
                lua_pop(pLua, 1);
            }
        }
        
    }
    catch (...) {

        m_iErrLine = -1;

        char szMsg[LM_STRSIZE];
        szMsg[0] = szMsg[LM_STRSIZE - 1] = '\0';

        try {
            throw;
        }
        catch (const luaexception &excT) {
            char szL[LM_STRSIZE];
            szL[0] = szL[LM_STRSIZE - 1] = '\0';
            strncpy(szL, excT.what(), LM_STRSIZE - 1);
            int iErrLine = lineNumber(static_cast<const char*>(szL), szMsg);
            if (iErrLine >= 0) {
                m_iErrLine = iErrLine;
            }
        }
        if (szMsg[0] != '\0') {
            char *pszT = strchr(static_cast<char*>(szMsg), '\n');
            if (NULL != pszT) {
                if ((pszT > static_cast<char*>(szMsg)) && (':' == *(pszT - 1))) {
                    *(pszT - 1) = '\0';
                }
                *pszT = '\0';
            }
            strncpy(m_szMessageA, static_cast<const char*>(szMsg), LM_STRSIZE - 1);
            mbstowcs(static_cast<char_t*>(m_szMessage), static_cast<const char*>(szMsg), LM_STRSIZE - 1);
        }
        else {
            strcpy(m_szMessageA, "! unrecoverable error");
            Tstrcpy(m_szMessage, uT("! unrecoverable error"));
        }
        m_bOK = false;
        iRet = LUA_ERROR;
    }

    return (iRet == LUA_OK);
}

LUAEXT_API bool LuaEngine::runScriptFile(char* pszFilename, int iLen, const char* pszCurDir, bool bAction, const char *pszOutputFilename)
{

#ifdef WIN32
    const int nFilenameSize = LM_STRSIZEW;
#else
    const int nFilenameSize = PATH_MAX;
#endif

    if (iLen < 0) {
        iLen = (int) strlen((const char*) (pszFilename));
    }
    if ((iLen < 5) || (iLen >= nFilenameSize)) {
        showMessage(uT("Cannot run script: invalid file"));
        return false;
    }

    if (m_pLuaState == NULL) {
        assignResources();
        if (m_pLuaState == NULL) {
            showMessage(uT("Cannot run script: insufficient memory"));
            return false;
        }
    }

    // Set the current directory
    bool bDirChanged = false;
    int is = 0;
    char cs = '\0';
    if (pszCurDir) {
        for (int ii = (iLen - 1); ii >= 0; ii--) {
            if ((pszFilename[ii] == '/') || (pszFilename[ii] == '\\')) {
                is = ii + 1;
                cs = pszFilename[is];
                pszFilename[is] = '\0';
                bDirChanged = true;
                break;
            }
        }
        if (bDirChanged) {
            chdir(pszFilename);
            pszFilename[is] = cs;
        }
    }
    //

    // read in binary mode --> "rb" (by default windows api read in text mode)
    FILE *fpScript = fopen(pszFilename, "rb");
    if (fpScript == NULL) {
        if (pszCurDir && bDirChanged) {
            chdir(pszCurDir);
        }
        showMessage(uT("Cannot run script: invalid file"));
        return false;
    }

    fseek(fpScript, 0L, SEEK_END);
    long fileSize = ftell(fpScript);
    fseek(fpScript, 0L, SEEK_SET);
    if ((fileSize < 1L) || (fileSize > LF_SCRIPT_MAXCHARS)) {
        fclose(fpScript);
        fpScript = NULL;
        if (pszCurDir && bDirChanged) {
            chdir(pszCurDir);
        }
        showMessage((fileSize < 1L) ? uT("Cannot run script: empty file") : uT("Cannot run script: file size > 256 MB"));
        return false;
    }

    char *pszBufferA = (char*) malloc((fileSize + 1) * sizeof(char));
    if (pszBufferA == NULL) {
        fclose(fpScript);
        fpScript = NULL;
        if (pszCurDir && bDirChanged) {
            chdir(pszCurDir);
        }
        showMessage(uT("Cannot run script: insufficient memory"));
        return false;
    }

    pszBufferA[0] = pszBufferA[fileSize] = '\0';
    size_t iret = fread(pszBufferA, sizeof(char), fileSize, fpScript);

    fclose(fpScript);
    fpScript = NULL;

    bool bRet = false;
    if ((iret * sizeof(char)) == static_cast<size_t>(fileSize)) {
        pszBufferA[iret] = '\0';

        char szOutputFilenameA[LM_STRSIZE];
        szOutputFilenameA[0] = szOutputFilenameA[LM_STRSIZE - 1] = '\0';

        if (LUA_ENGINE_COMPILE == bAction) {
            if (LuaEngine::LUADUMPFIlE) {
                fclose(LuaEngine::LUADUMPFIlE);
                LuaEngine::LUADUMPFIlE = NULL;
            }
            char szDumpFilename[LM_STRSIZE];
            szDumpFilename[0] = szDumpFilename[LM_STRSIZE - 1] = '\0';
            if (pszOutputFilename && (*pszOutputFilename != ('\0'))) {
                strncpy(szDumpFilename, static_cast<const char*>(pszOutputFilename), LM_STRSIZE - 5);
#ifdef WIN32
                if (stricmp(static_cast<const char*>(szDumpFilename), static_cast<const char*>(pszFilename)) == 0) {
#else
                if (strcasecmp(static_cast<const char*>(szDumpFilename), static_cast<const char*>(pszFilename)) == 0) {
#endif
                    szDumpFilename[0] = '\0';
                }
            }
            if (('\0') == szDumpFilename[0]) {
                strncpy(szDumpFilename, static_cast<const char*>(pszFilename), LM_STRSIZE - 5);
                char* pszDext = strstr(static_cast<char*>(szDumpFilename), (".lua"));
                if (pszDext) {
                    *pszDext = ('\0');
                }
                strcat(szDumpFilename, (".luac"));
            }
            LuaEngine::LUADUMPFIlE = fopen(static_cast<const char*>(szDumpFilename), ("wb"));
        }

        bRet = runScriptString(static_cast<const char*>(pszBufferA), bAction);

        if (LUA_ENGINE_COMPILE == bAction) {
            if (LuaEngine::LUADUMPFIlE) {
                fclose(LuaEngine::LUADUMPFIlE);
                LuaEngine::LUADUMPFIlE = NULL;
            }
        }
    }
    else {
        free(pszBufferA);
        pszBufferA = NULL;
        if (pszCurDir && bDirChanged) {
            chdir(pszCurDir);
        }
        showMessage(uT("Cannot run script: invalid file content"));
        return false;
    }

    free(pszBufferA);
    pszBufferA = NULL;

    if (pszCurDir && bDirChanged) {
        chdir(pszCurDir);
    }

    return bRet;
}

LUAEXT_API bool LuaEngine::runScriptString(const char *pszBufferA, bool bAction)
{
    int iRet = -1;

    try {
        if (m_pLuaState == NULL) {
            assignResources();
            if (m_pLuaState == NULL) {
                showMessage(uT("Cannot run script: insufficient memory"));
                return false;
            }
        }

        if (isDebugging()) {
            m_GlobalSet.clear();
            lua_pushglobaltable(m_pLuaState);
            lua_pushnil(m_pLuaState);
            while (lua_next(m_pLuaState, -2) != 0) {                                                    // pop nil, push name,value
                m_GlobalSet.insert(lm_hash((const char*) lua_tostring(m_pLuaState, -2)));               // insert into hash table
                lua_pop(m_pLuaState, 1);                                                                    // remove value
            }
            lua_pop(m_pLuaState, 1);                                                                        // Remove global table
        }

        // m_iErrLine could be modified during execution (in UI calls for example)
        m_iErrLine = -1;

        if (LUA_ENGINE_RUN == bAction) {
            iRet = luaL_dostring(m_pLuaState, pszBufferA);
        }
        else {
            // LUA_ENGINE_COMPILE
            iRet = luaL_loadstring(m_pLuaState, pszBufferA);
            if (0 == iRet) {
                iRet = lua_dump(m_pLuaState, LuaEngine::LUADUMP, LuaEngine::LUADUMPFIlE, 1);
            }
        }

        if (iRet != 0) {
            const char* pszRet = const_cast<const char*>(lua_tostring(m_pLuaState, -1));
            if (pszRet && (*pszRet != '\0')) {
                char szL[LM_STRSIZE];
                szL[0] = szL[LM_STRSIZE - 1] = '\0';
                strncpy(szL, pszRet, LM_STRSIZE - 1);
                luaexception excT(szL);
                throw(excT);
            }
            else {
                luaexception excT("! unrecoverable error");
                throw(excT);
            }
        }
    }
    catch (...) {

        m_iErrLine = -1;

        char szMsg[LM_STRSIZE];
        szMsg[0] = szMsg[LM_STRSIZE - 1] = '\0';

        try {
            throw;
        }
        catch (const luaexception &excT) {
            char szL[LM_STRSIZE];
            szL[0] = szL[LM_STRSIZE - 1] = '\0';
            strncpy(szL, excT.what(), LM_STRSIZE - 1);
            int iErrLine = lineNumber(const_cast<const char*>(szL), szMsg);
            if (iErrLine >= 0) {
                m_iErrLine = iErrLine;
            }
        }

        if (szMsg[0] != '\0') {
            char *pszT = strchr(static_cast<char*>(szMsg), '\n');
            if (NULL != pszT) {
                if ((pszT > static_cast<char*>(szMsg)) && (':' == *(pszT - 1))) {
                    *(pszT - 1) = '\0';
                }
                *pszT = '\0';
            }
            mbstowcs(static_cast<char_t*>(m_szMessage), static_cast<const char*>(szMsg), LM_STRSIZE - 1);
            showMessageA(static_cast<const char*>(szMsg));
        }
        else {
            Tstrcpy(m_szMessage, uT("! unrecoverable error"));
            showMessageA("! unrecoverable error");
        }
        m_bOK = false;
        iRet = LUA_ERROR;
    }

    return (iRet == LUA_OK);
}

LUAEXT_API int LuaEngine::lineNumber(const char* pszMessageA, char *pszT)
{
    int iLine = -1;
    char cT;
    char *pszTA = NULL, *pszTTA = NULL;

    if (pszMessageA == NULL) {
        return 0;
    }

    // LuaExt error?
    pszTA = const_cast<char*>(strstr(pszMessageA, "<!line="));
    if (pszTA != NULL) {
        const int iPrefix = 7;
        const int iSuffix = 2;
        pszTTA = const_cast<char*>(strstr(pszTA + iPrefix, "!>"));
        if ((pszTTA != NULL) && (pszTTA > (pszTA + iPrefix)) && (pszTTA < (pszTA + iPrefix + LM_STRSIZET))) {
            cT = *pszTTA;
            *pszTTA = '\0';
            // index already 0-based
            iLine = atoi(pszTA + iPrefix);
            *pszTTA = cT;
            snprintf(pszT, LM_STRSIZE - 1, "%d: %s", iLine + 1, pszTTA + iSuffix);
        }
        else {
            strcpy(pszT, pszMessageA);
        }
        return iLine;
    }

    // Lua error?
    pszTA = const_cast<char*>(strstr(pszMessageA, ":"));
    if (pszTA != NULL) {
        if ((*(pszTA + 1) == '\\') || (*(pszTA + 1) == '/')) {
            pszTA = const_cast<char*>(strstr(static_cast<const char*>(pszTA + 1), ":"));
        }
    }
    if (pszTA != NULL) {
        pszTA += 1;
        pszTTA = static_cast<char*>(strstr(pszTA, ":"));
        if ((pszTTA != NULL) && (pszTTA >= (pszTA + 1)) && (pszTTA < (pszTA + 1 + LM_STRSIZET))) {
            cT = *pszTTA;
            *pszTTA = '\0';
            iLine = atoi(pszTA) - 1;
            *pszTTA = cT;
        }
        strcpy(pszT, pszTA);
    }
    else {
        strcpy(pszT, pszMessageA);
    }

    if (iLine < 0) {
        // Lua error?
        pszTA = const_cast<char*>(strstr(pszMessageA, "at line"));
        if (pszTA != NULL) {
            pszTA += 7;
            pszTTA = static_cast<char*>(strstr(pszTA, ")"));
            if ((pszTTA != NULL) && (pszTTA >(pszTA + 1)) && (pszTTA < (pszTA + 1 + LM_STRSIZET))) {
                cT = *pszTTA;
                *pszTTA = '\0';
                iLine = atoi(pszTA) - 1;
                *pszTTA = cT;
            }
        }
    }

    return iLine;
}

LUAEXT_API void LuaEngine::initModules(void)
{
    if (m_pLuaState == NULL) {
        return;
    }

    lua_Hook pHook = lua_gethook(m_pLuaState);
    if (pHook) {
        lua_sethook(m_pLuaState, NULL, LUA_MASKLINE, 0);
    }

    int ii = 0;

    ii = 0;
    while (LuaEngine::CFUNCTION[ii].name != NULL) {
        lua_register(m_pLuaState, LuaEngine::CFUNCTION[ii].name, LuaEngine::CFUNCTION[ii].func);
        ii += 1;
    }

    const int iSize = 131072;

    char* pszBuffer = (char*) malloc(iSize * sizeof(char));
    if (pszBuffer == NULL) {
        return;
    }
    pszBuffer[0] = pszBuffer[iSize - 1] = '\0';

    strcpy(pszBuffer,
        "\n-- Comet \n"
        "\n-- (C) Pr. Sidi HAMADY \n"
        "\n-- http://www.hamady.org \n");

#ifdef WIN32

    char szLDpathA[512];
    memset(szLDpathA, 0, 512 * sizeof(char));
    char szExepath[256];
    memset(szExepath, 0, 256 * sizeof(char));
    HMODULE hModule = ::GetModuleHandle(NULL);
    DWORD ires = ::GetModuleFileNameA(hModule, szExepath, 255);
    if (ires > 1) {
        strcpy(szLDpathA, "\npackage.cpath = package.cpath .. ';");
        size_t isl = strlen((const char*) (szLDpathA));
        DWORD isep = 0;
        for (DWORD ii = 0; ii < ires; ii++) {
            szLDpathA[isl++] = szExepath[ii];
            if (szExepath[ii] == '\\') {
                szLDpathA[isl++] = '\\';
                isep = (DWORD) isl;
            }
        }
        if (isep > 1) {
            szLDpathA[isep] = '\0';
        }
        else {
            szLDpathA[0] = '\0';
        }
    }
    if (szLDpathA[0] != '\0') {
        strcat(szLDpathA, "?.dll' \n");
        strcat(pszBuffer, (const char*) (szLDpathA));
    }

#else

    char szLDpathA[512];
    memset(szLDpathA, 0, 512 * sizeof(char));
    char szExepath[256];
    memset(szExepath, 0, 256 * sizeof(char));
    int ires = readlink("/proc/self/exe", szExepath, 254);
    if (ires > 1) {
        szExepath[ires] = '\0';
        if (strcmp(szExepath, "(deleted)") != 0) {
            for (int ii = ires - 1; ii >= 0; ii--) {
                if (szExepath[ii] == '/') {
                    szExepath[ii + 1] = '\0';
                    break;
                }
            }
            snprintf(szLDpathA, 511, "\npackage.cpath = package.cpath .. ';%slib?.so;%s?.so' \n", szExepath, szExepath);
        }
    }

    char szLDpathB[512];
    memset(szLDpathB, 0, 512 * sizeof(char));
    const char* pszLDpath = (const char*) getenv("LD_LIBRARY_PATH");
    if (pszLDpath != NULL) {
        const int iLDlen = (const int) (strlen(pszLDpath));
        if (iLDlen > 0) {
            if (pszLDpath[iLDlen - 1] == '/') {
                snprintf(szLDpathB, 511, "\npackage.cpath = package.cpath .. ';%slib?.so;%s?.so' \n", pszLDpath, pszLDpath);
            }
            else {
                snprintf(szLDpathB, 511, "\npackage.cpath = package.cpath .. ';%s/lib?.so;%s/?.so' \n", pszLDpath, pszLDpath);
            }
        }
    }

    strcat(pszBuffer, "\npackage.cpath = package.cpath .. ';../lib/lib?.so;../lib/?.so;./lib?.so;./?.so' \n");
    if (szLDpathA[0] != '\0') {
        strcat(pszBuffer, (const char*) (szLDpathA));
    }
    if (szLDpathB[0] != '\0') {
        strcat(pszBuffer, (const char*) (szLDpathB));
    }

#endif

    strcat(pszBuffer,
        "function split(str, delim)\n"
        "    arr = { }\n"
        "    for elem in (str .. delim):gmatch(\"(.-)\" .. delim) do\n"
        "        table.insert(arr, elem)\n"
        "    end\n"
        "    return arr\n"
        "end\n"
        "\n"
        "\n"
        "function osname()\n"
        "    if string.sub(package.config, 1, 1) == \"/\" then\n"
        "        return \"linux\"\n"
        "    end\n"
        "    return \"windows\"\n"
        "end\n"
        "\n"
        "function readOnly(builtinTable)\n"
        "    local proxy = {}\n"
        "    local mt = {\n"
        "        __index = builtinTable,\n"
        "        __newindex = function (builtinTable,k,v)\n"
        "            error(\"Cannot modify Comet builtin modules\", 2)\n"
        "        end\n"
        "    }\n"
        "    setmetatable(proxy, mt)\n"
        "    return proxy\n"
        "end\n"
        "\n");

    strcat(pszBuffer,
        "\n"
        "if abs == nil then \n"
        "    abs = function(x) \n"
        "        return math.abs(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if acos == nil then \n"
        "    acos = function(x) \n"
        "        return math.acos(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if asin == nil then \n"
        "    asin = function(x) \n"
        "        return math.asin(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if atan == nil then \n"
        "    atan = function(x) \n"
        "        return math.atan(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if atan2 == nil then \n"
        "    atan2 = function(y,x) \n"
        "        return math.atan2(y,x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if ceil == nil then \n"
        "    ceil = function(x) \n"
        "        return math.ceil(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if cos == nil then \n"
        "    cos = function(x) \n"
        "        return math.cos(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if cosh == nil then \n"
        "    cosh = function(x) \n"
        "        return math.cosh(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if deg == nil then \n"
        "    deg = function(x) \n"
        "        return math.deg(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if exp == nil then \n"
        "    exp = function(x) \n"
        "        return math.exp(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if floor == nil then \n"
        "    floor = function(x) \n"
        "        return math.floor(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if fmod == nil then \n"
        "    fmod = function(x) \n"
        "        return math.fmod(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if frexp == nil then \n"
        "    frexp = function(x) \n"
        "        return math.frexp(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if huge == nil then \n"
        "    huge = math.huge \n"
        "end \n"
        "\n"
        "if ldexp == nil then \n"
        "    ldexp = function(x,y) \n"
        "        return math.ldexp(x,y) \n"
        "    end \n"
        "end \n"
        "\n"
        "if log == nil then \n"
        "    log = function(x) \n"
        "        return math.log(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if log10 == nil then \n"
        "    log10 = function(x) \n"
        "        return math.log10(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if max == nil then \n"
        "    max = function(...) \n"
        "        return math.max(...) \n"
        "    end \n"
        "end \n"
        "\n"
        "if min == nil then \n"
        "    min = function(...) \n"
        "        return math.min(...) \n"
        "    end \n"
        "end \n"
        "\n"
        "if modf == nil then \n"
        "    modf = function(x) \n"
        "        return math.modf(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if pi == nil then \n"
        "    pi = math.pi \n"
        "end \n"
        "\n"
        "if pow == nil then \n"
        "    pow = function(x,y) \n"
        "        return math.pow(x,y) \n"
        "    end \n"
        "end \n"
        "\n"
        "if rad == nil then \n"
        "    rad = function(x) \n"
        "        return math.rad(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if random == nil then \n"
        "    random = function(...) \n"
        "        return math.random(...) \n"
        "    end \n"
        "end \n"
        "\n"
        "if randomseed == nil then \n"
        "    randomseed = function(x) \n"
        "        return math.randomseed(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if sin == nil then \n"
        "    sin = function(x) \n"
        "        return math.sin(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if sinh == nil then \n"
        "    sinh = function(x) \n"
        "        return math.sinh(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if sqrt == nil then \n"
        "    sqrt = function(x) \n"
        "        return math.sqrt(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if tan == nil then \n"
        "    tan = function(x) \n"
        "        return math.tan(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if tanh == nil then \n"
        "    tanh = function(x) \n"
        "        return math.tanh(x) \n"
        "    end \n"
        "end \n"
        "\n");

    strcat(pszBuffer,
        "\n"
        "time = time or {}\n"
        "\n"
        "function time.tic()\n"
        "    time_tic()\n"
        "end\n"
        "\n"
        "function time.toc()\n"
        "    return time_toc()\n"
        "end"
        "\n"
        "function time.sleep(millis)\n"
        "    time_sleep(millis)\n"
        "end\n"
        "\n"
        "function time.pause(millis)\n"
        "    time_sleep(millis)\n"
        "end\n"
        "\n"
        "function time.format(millis)\n"
        "    return time_format(millis)\n"
        "end\n"
        "time = readOnly(time)\n"
        "tic = time.tic\n"
        "toc = time.toc\n"
        "sleep = time.sleep\n"
        "pause = time.sleep\n"
        "package.preload.time = function()\n"
        "    return time\n"
        "end\n"
        "\n");

    strcat(pszBuffer,
        "\n"
        "function beep(ifrequency,iduration)\n"
        "    ifrequency = ifrequency or 500\n"
        "    iduration = iduration or 500\n"
        "    ext_beep(ifrequency,iduration)\n"
        "end\n");

    strcat(pszBuffer,
        "\n"
        "io = io or {}\n"
        "io_write_def = io.write\n"
        "function io.write(...)\n"
        "    -- redefine only for std input\n"
        "    if io.output() ~= io.stdout then\n"
        "        return io_write_def(...)\n"
        "    end\n"
        "    io_write(...)\n"
        "end\n"
        "\n");

    if (getMode() == LUA_ENGINE_GUI) {

        // Window
        strcat(pszBuffer,
            "\n"
            "io = io or {}\n"
            "io_read_def = io.read\n"
            "function io.read(...)\n"
            "    -- redefine only for std input\n"
            "    if io.input() ~= io.stdin then\n"
            "        return io_read_def(...)\n"
            "    end\n"
            "    return io_read(...)\n"
            "end\n"
            "\n"
            "io = readOnly(io)\n"
            "\n");
    }

    // Window
    strcat(pszBuffer,
        (getMode() == LUA_ENGINE_CONSOLE) ?

        ("\n"
        "gui = gui or {}\n"
        "\n"

#ifdef WIN32
        "gui.os = \"windows\"\n"
#else
        "gui.os = \"linux\"\n"
#endif

        "function gui.write(...)\n"
        "	io.write(msg)\n"
        "end\n"

        "function gui.print(...)\n"
        "	print(...)\n"
        "end\n"

        "function gui.log(...)\n"
        "	return\n"
        "end\n"

        "function gui.read(...)\n"
        "	return io.read(...)\n"
        "end\n"

        "function gui.input(...)\n"
        "	return io.read(...)\n"
        "end\n"

        "function gui.cpuinfo()\n"
        "	-- :TODO:\n"
        "end\n"

        "function gui.primaryvolume()\n"
#ifdef WIN32
        "   return \"C:\"\n"
#else
        "   return \"/\"\n"
#endif
        "end\n"

        "function gui.secondaryvolume()\n"
        "   return gui.primaryvolume()\n"
        "end\n"

        "function gui.createdirectory(dirname)\n"
        "	os.system(\"mkdir \" .. dirname)\n"
		"end\n"

		"function gui.directoryexists(dirname)\n"
        "	return gui_directoryexists(dirname)\n"
		"end\n"

		"function gui.meminfo()\n"
		"	-- :TODO:\n"
		"end\n"

		"function gui.memusage()\n"
		"	-- :TODO:\n"
		"end\n"

		"function gui.play(snd, duration)\n"
		"	-- :TODO:\n"
		"end\n"

        "function gui.alert(msg)\n"
        "    io.write(msg)\n"
        "end"
        "\n"
        "function gui.toast(msg)\n"
        "    io.write(msg)\n"
        "end"
        "\n"
        "function gui.disp(msg)\n"
        "    io.write(msg)\n"
        "end"
        "\n"
        "function gui.ask(msg)\n"
        "    io.write(msg, \" (yes/no): \")\n"
        "    answ = io.read()\n"
        "    if answ == \"yes\" or answ == \"y\" then\n"
        "        return 1\n"
        "    end\n"
        "    return 0\n"
        "end\n"
        "\n"
        "function gui.log(msg)\n"
        "    return\n"
        "end\n"
        "\n"
        "function gui.notify(msg,snd)\n"
        "    beep(500,500)\n"
        "end\n"
        "\n"
        "function gui.vibrate(ms)\n"
        "    beep(500,500)\n"
        "end\n"
        "\n"
        "gui = readOnly(gui)\n"
        "package.preload.gui = function()\n"
        "    return gui\n"
        "end\n"
        "\n"
        "win = gui\n"
        "win = readOnly(win)\n"
        "package.preload.win = function()\n"
        "    return win\n"
        "end\n"
        "droid = gui\n"
        "droid = readOnly(droid)\n"
        "package.preload.droid = function()\n"
        "    return droid\n"
        "end\n"
        "lwin = gui\n"
        "lwin = readOnly(lwin)\n"
        "package.preload.lwin = function()\n"
        "    return lwin\n"
        "end\n"
        "\n")

        :

        ("\n"
        "gui = gui or {}\n"
        "\n"

#ifdef WIN32
        "gui.os = \"windows\"\n"
#else
        "gui.os = \"linux\"\n"
#endif

        "function gui.write(...)\n"
        "	io.write(msg)\n"
        "end\n"

        "function gui.print(...)\n"
        "	print(...)\n"
        "end\n"

        "function gui.log(...)\n"
        "	return\n"
        "end\n"

        "function gui.read(...)\n"
        "	return io.read(...)\n"
        "end\n"

        "function gui.input(...)\n"
        "	return io.read(...)\n"
        "end\n"

		"function gui.cpuinfo()\n"
		"	-- :TODO:\n"
		"end\n"

        "function gui.primaryvolume()\n"
#ifdef WIN32
        "   return \"C:\"\n"
#else
        "   return \"/\"\n"
#endif
        "end\n"

        "function gui.secondaryvolume()\n"
        "   return gui.primaryvolume()\n"
        "end\n"

        "function gui.createdirectory(dirname)\n"
        "	os.system(\"mkdir \" .. dirname)\n"
        "end\n"

        "function gui.directoryexists(dirname)\n"
        "	return gui_directoryexists(dirname)\n"
        "end\n"

		"function gui.meminfo()\n"
		"	-- :TODO:\n"
		"end\n"

		"function gui.memusage()\n"
		"	-- :TODO:\n"
		"end\n"

		"function gui.play(snd, duration)\n"
		"	-- :TODO:\n"
		"end\n"
 
        "function gui.alert(msg)\n"
        "    return wx_alert(msg)\n"
        "end"
        "\n"
        "function gui.toast(msg)\n"
        "    return wx_alert(msg)\n"
        "end"
        "\n"
        "function gui.disp(msg)\n"
        "    return wx_alert(msg)\n"
        "end"
        "\n"
        "function gui.ask(msg)\n"
        "    return wx_ask(msg)\n"
        "end\n"
        "\n"
        "function gui.log(msg)\n"
        "    return\n"
        "end\n"
        "\n"
        "function gui.notify(msg,snd)\n"
        "    beep(500,500)\n"
        "end\n"
        "\n"
        "function gui.vibrate(ms)\n"
        "    beep(500,500)\n"
        "end\n"
        "\n"
        "gui = readOnly(gui)\n"
        "package.preload.gui = function()\n"
        "    return gui\n"
        "end\n"
        "\n"
        "win = gui\n"
        "win = readOnly(win)\n"
        "package.preload.win = function()\n"
        "    return win\n"
        "end\n"
        "droid = gui\n"
        "droid = readOnly(droid)\n"
        "package.preload.droid = function()\n"
        "    return droid\n"
        "end\n"
        "lwin = gui\n"
        "lwin = readOnly(lwin)\n"
        "package.preload.lwin = function()\n"
        "    return lwin\n"
        "end\n"
        "\n"));

    strcat(pszBuffer,
        "\n"
        "socket = socket or {}\n"
        "\n"

        "function socket.new(iFamily, iType, iProtocol)\n"
        "    iFamily = iFamily or socket.AF_INET\n"
        "    iType = iType or socket.SOCK_STREAM\n"
        "    iProtocol = iProtocol or socket.IPPROTO_TCP\n"
        "    return socket_new(iFamily, iType, iProtocol)\n"
        "end\n"

        "function socket.accept(id)\n"
        "    return socket_accept(id)\n"
        "end\n"

        "function socket.setsockopt(id, iOpt, lBuf)\n"
        "    return socket_setsockopt(id, iOpt, lBuf)\n"
        "end\n"

        "function socket.bind(id, iAddr, iPort, iFamily)\n"
        "    iAddr = iAddr or socket.INADDR_ANY\n"
        "    iFamily = iFamily or socket.AF_INET\n"
        "    return socket_bind(id, iPort, iAddr, iFamily)\n"
        "end\n"

        "function socket.listen(id, iBacklog)\n"
        "    iBacklog = iBacklog or 5\n"
        "    return socket_listen(id, iBacklog)\n"
        "end\n"

        "function socket.connect(id, szServer, iPort, iFamily)\n"
        "    iFamily = iFamily or socket.AF_INET\n"
        "    return socket_connect(id, szServer, iPort, iFamily)\n"
        "end\n"

        "function socket.timeout(id, uiTimeout)\n"
        "    return socket_timeout(id, uiTimeout)\n"
        "end\n"

        "function socket.getpeername(id)\n"
        "    return socket_getpeername(id)\n"
        "end\n"
        "function socket.gethostname(id)\n"
        "    return socket_gethostname(id)\n"
        "end\n"

        "function socket.getsockname(id)\n"
        "    return socket_getsockname(id)\n"
        "end\n"

        "function socket.gethostbyaddr(id, taddr)\n"
        "    return socket_gethostbyaddr(id, taddr)\n"
        "end\n"
        "function socket.gethostbyname(id, tname)\n"
        "    return socket_gethostbyname(id, tname)\n"
        "end\n"

        "function socket.send(id, pszMsg, iFlags)\n"
        "    iFlags = iFlags or 0\n"
        "    return socket_send(id, pszMsg, iFlags)\n"
        "end\n"

        "function socket.sendto(id, szServer, iPort, pszMsg, iFlags, iFamily)\n"
        "    iFlags = iFlags or 0\n"
        "    iFamily = iFamily or socket.AF_INET\n"
        "    return socket_sendto(id, szServer, iPort, pszMsg, iFlags, iFamily)\n"
        "end\n"

        "function socket.write(id, pszMsg)\n"
        "    return socket_send(id, pszMsg, 0)\n"
        "end\n"

        "function socket.recv(id, iSize, iFlags)\n"
        "    iSize = iSize or 4096\n"
        "    iFlags = iFlags or 0\n"
        "    return socket_recv(id, iSize, iFlags)\n"
        "end\n"

        "function socket.recvfrom(id, szServer, iPort, iSize, iFlags, iFamily)\n"
        "    iSize = iSize or 4096\n"
        "    iFlags = iFlags or 0\n"
        "    iFamily = iFamily or socket.AF_INET\n"
        "    return socket_recvfrom(id, szServer, iPort, iSize, iFlags, iFamily)\n"
        "end\n"

        "function socket.read(id, iSize)\n"
        "    iSize = iSize or 4096\n"
        "    return socket_recv(id, iSize, 0)\n"
        "end\n"

        "function socket.shutdown(id, iHow)\n"
        "    iHow = iHow or 2\n"
        "    return socket_close(id, iHow)\n"
        "end\n"

        "function socket.close(id)\n"
        "    return socket_close(id)\n"
        "end\n"

        "function socket.delete(id)\n"
        "    return socket_delete(id)\n"
        "end\n"

        "function socket.iserr(id)\n"
        "    return socket_iserr(id)\n"
        "end\n"

        "function socket.geterr(id)\n"
        "    return socket_geterr(id)\n"
        "end\n"

        "\n"
        "package.path = './?.luac;' .. package.path\n"
        "\n"

        "\n"
        "package.path = './?.luac;' .. package.path\n"
        "\n"
        );

    int iRet = luaL_dostring(m_pLuaState, static_cast<const char*>(pszBuffer));
    if (iRet != 0) {
        char* pszRet = const_cast<char*>(lua_tostring(m_pLuaState, -1));
        showMessageA(static_cast<const char*>(pszRet));
        lua_pop(m_pLuaState, 1);
        lua_gc(m_pLuaState, LUA_GCCOLLECT, 0);        // Force a complete garbage collection in case of errors
        free(pszBuffer);
        pszBuffer = NULL;
        return;
    }
    free(pszBuffer);
    pszBuffer = NULL;

    lua_tableinsert(m_pLuaState, "socket", "INADDR_ANY", INADDR_ANY);
    lua_tableinsert(m_pLuaState, "socket", "INADDR_LOOPBACK", INADDR_LOOPBACK);
    lua_tableinsert(m_pLuaState, "socket", "INADDR_BROADCAST", INADDR_BROADCAST);
    lua_tableinsert(m_pLuaState, "socket", "INADDR_NONE", INADDR_NONE);

    lua_tableinsert(m_pLuaState, "socket", "AF_UNIX", AF_UNIX);
    lua_tableinsert(m_pLuaState, "socket", "AF_UNSPEC", AF_UNSPEC);
    lua_tableinsert(m_pLuaState, "socket", "AF_INET", AF_INET);
    lua_tableinsert(m_pLuaState, "socket", "AF_INET6", AF_INET6);
#ifdef WIN32
    lua_tableinsert(m_pLuaState, "socket", "AF_ECMA", AF_ECMA);
    lua_tableinsert(m_pLuaState, "socket", "AF_IRDA", AF_IRDA);
#else
    lua_tableinsert(m_pLuaState, "socket", "AF_ECMA", 8);
    lua_tableinsert(m_pLuaState, "socket", "AF_IRDA", 23);
#endif

    lua_tableinsert(m_pLuaState, "socket", "IPPROTO_TCP", IPPROTO_TCP);
    lua_tableinsert(m_pLuaState, "socket", "IPPROTO_UDP", IPPROTO_UDP);

    lua_tableinsert(m_pLuaState, "socket", "SO_REUSEADDR", SO_REUSEADDR);
    lua_tableinsert(m_pLuaState, "socket", "SO_KEEPALIVE", SO_KEEPALIVE);
    lua_tableinsert(m_pLuaState, "socket", "SO_DONTROUTE", SO_DONTROUTE);
    lua_tableinsert(m_pLuaState, "socket", "SO_LINGER", SO_LINGER);
    lua_tableinsert(m_pLuaState, "socket", "SO_ACCEPTCONN", SO_ACCEPTCONN);
    lua_tableinsert(m_pLuaState, "socket", "SO_SNDTIMEO", SO_SNDTIMEO);
    lua_tableinsert(m_pLuaState, "socket", "SO_RCVTIMEO", SO_RCVTIMEO);

    lua_tableinsert(m_pLuaState, "socket", "SOCK_DGRAM", SOCK_DGRAM);
    lua_tableinsert(m_pLuaState, "socket", "SOCK_STREAM", SOCK_STREAM);
    lua_tableinsert(m_pLuaState, "socket", "SOCK_SEQPACKET", SOCK_SEQPACKET);
    lua_tableinsert(m_pLuaState, "socket", "SOCK_RAW", SOCK_RAW);

    lua_tableinsert(m_pLuaState, "socket", "MSG_CTRUNC", MSG_CTRUNC);
    lua_tableinsert(m_pLuaState, "socket", "MSG_DONTROUTE", MSG_DONTROUTE);
    lua_tableinsert(m_pLuaState, "socket", "MSG_OOB", MSG_OOB);
    lua_tableinsert(m_pLuaState, "socket", "MSG_PEEK", MSG_PEEK);
    lua_tableinsert(m_pLuaState, "socket", "MSG_TRUNC", MSG_TRUNC);
    lua_tableinsert(m_pLuaState, "socket", "MSG_WAITALL", MSG_WAITALL);

    iRet = luaL_dostring(m_pLuaState, 
        "\nsocket = readOnly(socket)\n"
        "package.preload.socket = function()\n"
        "    return socket\n"
        "end\n"
        );
    if (iRet != 0) {
        char* pszRet = const_cast<char*>(lua_tostring(m_pLuaState, -1));
        showMessageA(static_cast<const char*>(pszRet));
        lua_pop(m_pLuaState, 1);
        lua_gc(m_pLuaState, LUA_GCCOLLECT, 0);        // Force a complete garbage collection in case of errors
        return;
    }

    if (pHook) {
        lua_sethook(m_pLuaState, pHook, LUA_MASKLINE, 0);
    }
}
