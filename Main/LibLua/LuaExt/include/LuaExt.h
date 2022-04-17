// -------------------------------------------------------------
// LuaExt
//                Copyright(C) 2010-2022 Pr. Sidi HAMADY
//                http://www.hamady.org
//                sidi@hamady.org
//
//      :STABLE:VERSION180:BUILD2104:
// -------------------------------------------------------------

#ifndef LUAEXT_H
#define LUAEXT_H

#define USE_LUAJIT true

#define LUAEXT_VERSION_NUMBER 186
#define LUAEXT_BUILD_NUMBER   2104
#define LUAEXT_VERSION_STRING uT("1.8")
#define LUAEXT_BUILD_STRING   uT("2104") // yymmdd (year, month, day)
#define LUAEXT_HEADER_STRING  uT("# Comet-RC <do not change or remove this header>")

#if defined(LUA_BUILD_AS_DLL)

#if defined(LUAEXT_LIB)
#define LUAEXT_API __declspec(dllexport)
#else
#define LUAEXT_API __declspec(dllimport)
#endif

#else

#define LUAEXT_API

#endif

#define LUA_ENGINE_CONSOLE true
#define LUA_ENGINE_GUI     false

#define LUA_ENGINE_COMPILE true
#define LUA_ENGINE_RUN     false

extern "C" {
#ifdef USE_LUAJIT

#include "../../LuaJIT/src/lua.h"
#include "../../LuaJIT/src/lualib.h"
#include "../../LuaJIT/src/lauxlib.h"
#include "../../LuaJIT/src/luajit.h"

#define luaL_len          lua_objlen
#define LUA_OK            0
#define LUA_ERROR         -1
#define LUA_VERSION_MAJOR "5"
#define LUA_VERSION_MINOR "1"
#define LUA_INITVERSION   LUA_INIT "_" LUA_VERSION_MAJOR "_" LUA_VERSION_MINOR

#else

#include "../LuaCore/lua.h"
#include "../LuaCore/lualib.h"
#include "../LuaCore/lauxlib.h"

#endif
}

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

#include "../../../LibFile/include/File.h"

#include <map>
#include <set>

#ifndef USE_LUAJIT
LUAEXT_API int luaCompile(lua_State *pLua, int argc, char **argv);
#endif

// SOCKET
LUAEXT_API int socket_new(lua_State *pLua);
LUAEXT_API int socket_bind(lua_State *pLua);
LUAEXT_API int socket_listen(lua_State *pLua);
LUAEXT_API int socket_connect(lua_State *pLua);
LUAEXT_API int socket_accept(lua_State *pLua);
LUAEXT_API int socket_timeout(lua_State *pLua);
LUAEXT_API int socket_setsockopt(lua_State *pLua);
LUAEXT_API int socket_getpeername(lua_State *pLua);
LUAEXT_API int socket_gethostname(lua_State *pLua);
LUAEXT_API int socket_gethostbyaddr(lua_State *pLua);
LUAEXT_API int socket_gethostbyname(lua_State *pLua);
LUAEXT_API int socket_getsockname(lua_State *pLua);
LUAEXT_API int socket_send(lua_State *pLua);
LUAEXT_API int socket_sendto(lua_State *pLua);
LUAEXT_API int socket_recv(lua_State *pLua);
LUAEXT_API int socket_recvfrom(lua_State *pLua);
LUAEXT_API int socket_close(lua_State *pLua);
LUAEXT_API int socket_shutdown(lua_State *pLua);
LUAEXT_API int socket_delete(lua_State *pLua);
LUAEXT_API int socket_iserr(lua_State *pLua);
LUAEXT_API int socket_geterr(lua_State *pLua);
//

// TIME
LUAEXT_API int time_tic(lua_State *pLua);
LUAEXT_API int time_toc(lua_State *pLua);
LUAEXT_API int time_sleep(lua_State *pLua);
LUAEXT_API int time_format(lua_State *pLua);
//

// MISC
LUAEXT_API int ext_beep(lua_State *pLua);
//

LUAEXT_API int gui_directoryexists(lua_State *pLua);

class luaexception : public std::exception
{
private:
    char m_szMsg[LM_STRSIZE];

public:
    explicit luaexception(const char *psz);
#if defined(WIN32) || (__GNUC__ < 4) || (__GNUC__ == 4 && __GNUC_MINOR__ < 7)
    const char *what() const throw();
#else
    const char *what() const noexcept;
#endif
};

extern "C" {
LUAEXT_API int luaL_errorlevel(lua_State *pLua, const char *fmt, ...);
}

inline void lua_pushnumberx(lua_State *pLua, lua_Number x)
{
    lua_pushnumber(pLua, lm_isNaN(x) ? 0.0 : x);
}

inline void lua_pushnumberx(lua_State *pLua, int i)
{
    lua_pushnumber(pLua, lm_isNaN(i) ? (lua_Number)0 : (lua_Number)i);
}

/* 1-based index */
inline int lua_toarrayd1(lua_State *pLua, int idx, double *parr, int nn)
{
    int ii, itop, nt;

    if (!lua_istable(pLua, idx)) {
        return -1;
    }

    nt = (int)(luaL_len(pLua, idx));
    if (nt < nn) {
        return -1;
    }
    for (ii = 1; ii <= nn; ii++) {
        lua_rawgeti(pLua, idx, ii); // push table[ii]
        itop = lua_gettop(pLua);
        if (itop == 0) {
            // Empty stack
            break;
        }
        parr[ii] = static_cast<double>(lua_tonumber(pLua, itop));
        lua_pop(pLua, 1);
        if (lm_isNaN(parr[ii])) {
            return -1;
        }
    }

    return 0;
}

inline int lua_toarrayd(lua_State *pLua, int idx, double *parr, int nn)
{
    int ii, itop, nt;

    if (!lua_istable(pLua, idx)) {
        return -1;
    }

    nt = (int)(luaL_len(pLua, idx));
    if (nt < nn) {
        return -1;
    }
    for (ii = 1; ii <= nn; ii++) {
        lua_rawgeti(pLua, idx, ii); // push table[ii]
        itop = lua_gettop(pLua);
        if (itop == 0) {
            // Empty stack
            break;
        }
        parr[ii - 1] = static_cast<double>(lua_tonumber(pLua, itop));
        lua_pop(pLua, 1);
        if (lm_isNaN(parr[ii - 1])) {
            return -1;
        }
    }

    return 0;
}

inline int lua_toarrayi(lua_State *pLua, int idx, int *parr, int nn)
{
    int ii, itop, nt;

    if (!lua_istable(pLua, idx)) {
        return -1;
    }

    nt = (int)(luaL_len(pLua, idx));
    if (nt < nn) {
        return -1;
    }
    for (ii = 1; ii <= nn; ii++) {
        lua_rawgeti(pLua, idx, ii); // push table[ii]
        itop = lua_gettop(pLua);
        if (itop == 0) {
            // Empty stack
            break;
        }
        parr[ii - 1] = static_cast<int>(lua_tonumber(pLua, itop));
        lua_pop(pLua, 1);
        if (lm_isNaN(parr[ii - 1])) {
            return -1;
        }
    }

    return 0;
}

inline void lua_tableinsert(lua_State *pLua, const char *tableT, const char *fieldT, int valueT)
{
    // :DEBUG:
    if (tableT == NULL) {
        return;
    }

    lua_getglobal(pLua, tableT);
    if (lua_istable(pLua, -1)) {
        lua_pushstring(pLua, fieldT);
        lua_pushnumberx(pLua, static_cast<double>(valueT));
        lua_settable(pLua, -3);
    }
    lua_pop(pLua, 1);
}
//

struct SocketErrorInfo
{
    const int id;
    const char *info;
};

class lmSocketException : public std::exception
{
private:
    int m_iCode;
    char m_szInfo[256];

public:
    lmSocketException(int iCode, const char *szInfo) : exception()
    {
        memset(m_szInfo, 0, 256 * sizeof(char));
        strcpy(m_szInfo, szInfo);
        m_iCode = iCode;
    }
    lmSocketException() : exception()
    {
        memset(m_szInfo, 0, 256 * sizeof(char));
        m_iCode = -1;
    }

#if defined(WIN32) || (__GNUC__ < 4) || (__GNUC__ == 4 && __GNUC_MINOR__ < 7)
    const char *what() const throw()
#else
    const char *what() const throw() override
#endif
    {
        return static_cast<const char *>(m_szInfo);
    }
};

class lmSocket
{
public:
#ifdef WIN32
    LUAEXT_API static WSADATA m_wsaData;
    LUAEXT_API static void startup(void);
    LUAEXT_API static void shutdown(void);
#endif

    lmSocket(long iId, int iFamily = AF_INET, int iType = SOCK_STREAM, int iProtocol = IPPROTO_TCP);
    lmSocket(long iId, int iSocket, bool bFlags);

    virtual ~lmSocket();

    static const int ERRINFO_COUNT;
    static const SocketErrorInfo ERRINFO[];

    bool setsockopt(int iOpt, long lBuf);
    bool bind(int iPort, long lInaddr = INADDR_ANY);
    bool listen(int iBacklog = 5);
    bool connect(const char *pszServer, int iPort);
    lmSocket *accept(void);
    int send(const char *pszMsg, int iFlags = 0);
    int sendto(const char *pszServer, int iPort, const char *pszMsg, int iFlags = 0);
    int write(const char *pszMsg);
    const char *recv(int iSize = 4096, int iFlags = 0);
    const char *recvfrom(const char *pszServer, int iPort, int iSize = 4096, int iFlags = 0);
    const char *read(int iSize = 4096);
    bool timeout(int iTimeout);
    bool getpeername(char *pszName);
    bool gethostname(char *pszName);
    bool gethostbyaddr(const char *pszT, char *pszName);
    bool gethostbyname(const char *pszT, char *pszName);
    bool getsockname(char *pszName);
    bool shutdown(int iHow);
    bool close();

    void release();

    bool iserr(void) const
    {
        return m_bError;
    }
    const char *geterr(void) const
    {
        return static_cast<const char *>(m_szError);
    }

    long getId(void) const
    {
        return m_Id;
    }
    void setId(long iId)
    {
        if (iId > 0L) {
            m_Id = iId;
        }
    }

private:
    long m_Id;

    struct in_addr *DoGethostbyname(const char *pszServer);

    int m_iSocket;
    struct sockaddr_in m_Addrin;
    char *m_pszBuffer;
    int m_iBufferSize;

    long m_iAccepted;

    bool m_bError;
    void error(const char *pszSource = NULL);
    char m_szError[256];
};

class LuaEngine
{

private:
    lua_State *m_pLuaState;
    static const luaL_Reg CFUNCTION[];

    lmSigma *m_pSigma;

    bool m_bOK;
    char_t m_szMessage[LM_STRSIZE];
    char m_szMessageA[LM_STRSIZE];

    int m_iErrLine;

    double m_fTic;
    double m_fToc;

    bool m_bMode;

    std::map<long, lmSocket *> m_SocketMap;
    long m_iSocketId;

    void create();

    const luaL_Reg *m_pFuncInteract;

    bool m_bDebugging;
    std::set<unsigned int> m_GlobalSet;

    LUAEXT_API bool debugGetVariable(const char *pszN, char *pszV, char *pszT);

    static FILE *LUADUMPFIlE;
    static int LUADUMP(lua_State *pLua, const void *pSource, size_t iSize, void *pTarget)
    {
        if (pTarget && pSource && (iSize > 0)) {
            size_t iCount = fwrite(pSource, iSize, 1, (FILE *)pTarget);
            return (1 == iCount) ? 0 : 1;
        }
        return 1;
    }

public:
    LUAEXT_API LuaEngine();
    LUAEXT_API LuaEngine(const luaL_Reg *pFuncInteract);
    LUAEXT_API LuaEngine(bool bMode, const luaL_Reg *pFuncInteract, bool bOpenLibs = true);
    LUAEXT_API ~LuaEngine();

    LUAEXT_API void assignResources(bool bOpenLibs = true);

    LUAEXT_API void openLibs(void);

    LUAEXT_API void setInteract(const luaL_Reg *pFuncInteract);

    LUAEXT_API bool debugStackTrace(char *pszBuffer, int iLen, lua_Debug *pDebug);

    LUAEXT_API void releaseResources(void);

    LUAEXT_API lua_State *getLuaState(void)
    {
        return m_pLuaState;
    }

    LUAEXT_API lmSigma *getSigma(void)
    {
        return m_pSigma;
    }

    LUAEXT_API long socketNew(long iId, int iFamily = AF_INET, int iType = SOCK_STREAM, int iProtocol = IPPROTO_TCP);
    LUAEXT_API bool socketSetsockopt(long iId, int iOpt, long lBuf);
    LUAEXT_API bool socketBind(long iId, int iPort, long lInaddr = INADDR_ANY);
    LUAEXT_API bool socketListen(long iId, int iBacklog = 5);
    LUAEXT_API bool socketConnect(long iId, const char *pszServer, int iPort);
    LUAEXT_API long socketAccept(long iId);
    LUAEXT_API bool socketTimeout(long iId, int iTimeout);
    LUAEXT_API bool socketGetpeername(long iId, char *pszRetA);
    LUAEXT_API bool socketGethostname(long iId, char *pszRetA);
    LUAEXT_API bool socketGethostbyaddr(long iId, const char *pszT, char *pszRetA);
    LUAEXT_API bool socketGethostbyname(long iId, const char *pszT, char *pszRetA);
    LUAEXT_API bool socketGetsockname(long iId, char *pszRetA);
    LUAEXT_API int socketSend(long iId, const char *pszMsg, int iFlags = 0);
    LUAEXT_API int socketSendto(long iId, const char *pszServer, int iPort, const char *pszMsg, int iFlags = 0);
    LUAEXT_API const char *socketRecv(long iId, int iLen = 4096, int iFlags = 0);
    LUAEXT_API const char *socketRecvfrom(long iId, const char *pszServer, int iPort, int iLen = 4096, int iFlags = 0);
    LUAEXT_API bool socketClose(long iId);
    LUAEXT_API bool socketShutdown(long iId, int iHow);
    LUAEXT_API bool socketDelete(long iId);
    LUAEXT_API bool socketIserr(long iId);
    LUAEXT_API const char *socketGeterr(long iId, const char *pszDefaultMsg);

    LUAEXT_API void setTic(double fTic)
    {
        m_fTic = fTic;
    }
    LUAEXT_API double getTic(void) const
    {
        return m_fTic;
    }
    LUAEXT_API double getToc(void) const
    {
        return m_fToc;
    }
    LUAEXT_API void setToc(double fToc)
    {
        m_fToc = fToc;
    }

    LUAEXT_API bool getMode(void) const
    {
        return m_bMode;
    }

    LUAEXT_API bool isOK(void) const
    {
        return m_bOK;
    }
    LUAEXT_API const char_t *getMessage(void) const
    {
        return static_cast<const char_t *>(m_szMessage);
    }
    LUAEXT_API const char *getMessageA(void) const
    {
        return static_cast<const char *>(m_szMessageA);
    }
    LUAEXT_API void showMessage(const char_t *pszMessage, bool bOK = false)
    {
        Tstrcpy(m_szMessage, pszMessage);
#ifdef __WXMSW__
        Tprintf(uT("\n%s\n"), pszMessage);
#else
        //In Linux, for unicode text, it is necessary to specify %ls
        Tprintf(uT("\n%ls\n"), pszMessage);
#endif
        m_bOK = bOK;
    }
    LUAEXT_API void showMessageA(const char *pszMessage, bool bOK = false)
    {
        strncpy(m_szMessageA, pszMessage, LM_STRSIZE - 1);
        mbstowcs(static_cast<char_t *>(m_szMessage), pszMessage, LM_STRSIZE - 1);

#ifdef __WXMSW__
        Tprintf(uT("\n%s\n"), static_cast<const char_t *>(m_szMessage));
#else
        //In Linux, for unicode text, it is necessary to specify %ls
        Tprintf(uT("\n%ls\n"), static_cast<const char_t *>(m_szMessage));
#endif

        m_bOK = bOK;
    }

    LUAEXT_API void updateMessage(const char_t *pszDefault, bool bStack)
    {
        lua_State *pLua = this->getLuaState();
        if (pLua == NULL) {
            Tstrcpy(static_cast<char_t *>(m_szMessage), pszDefault);
            wcstombs(static_cast<char *>(m_szMessageA), static_cast<const char_t *>(m_szMessage), LM_STRSIZE - 1);
            m_bOK = false;
            return;
        }

        if (bStack) {
            const char *pszRet = const_cast<const char *>(lua_tostring(pLua, -1));
            if (pszRet != NULL) {
                int iErrLine = lineNumber(pszRet, static_cast<char *>(m_szMessageA));
                if (iErrLine >= 0) {
                    m_iErrLine = iErrLine;
                }
                mbstowcs(static_cast<char_t *>(m_szMessage), static_cast<const char *>(m_szMessageA), LM_STRSIZE - 1);
            }
            else {
                Tstrcpy(static_cast<char_t *>(m_szMessage), pszDefault);
                wcstombs(static_cast<char *>(m_szMessageA), static_cast<const char_t *>(m_szMessage), LM_STRSIZE - 1);
            }
        }
        else {
            Tstrcpy(static_cast<char_t *>(m_szMessage), pszDefault);
            wcstombs(static_cast<char *>(m_szMessageA), static_cast<const char_t *>(m_szMessage), LM_STRSIZE - 1);
        }
        m_bOK = false;
    }

    LUAEXT_API bool isDebugging(void) const
    {
        return m_bDebugging;
    }

    LUAEXT_API void setDebugging(bool bDebugging)
    {
        m_bDebugging = bDebugging;
    }

    LUAEXT_API bool runScriptFunction(const char *pszScriptA, const char *pszFuncNameA,
                                      const double *pa, const double *pb, const double *pc, const double *pd, const double *pe, const double *pf,
                                      double *pra, double *prb, double *prc, double *prd, double *pre, double *prf,
                                      int *pstatus);

    LUAEXT_API bool runScriptFile(char *pszFilename, int iLen, const char *pszCurDir, bool bAction, const char *pszOutputFilename);

    LUAEXT_API bool runScriptString(const char *pszBufferA, bool bAction);

    LUAEXT_API int getErrLine(void) const
    {
        return m_iErrLine;
    }
    LUAEXT_API void setErrLine(int iErrLine)
    {
        m_iErrLine = iErrLine;
    }

    LUAEXT_API int lineNumber(const char *pszMessageA, char *pszT);

private:
    LUAEXT_API void initModules(void);
};

inline int getCurrentLine(lua_State *pLua)
{
    int iErrLine = -1;
    char cT;
    lua_Debug ldT;
    for (int iDepth = 0; iDepth < 32; iDepth++) {
        if (lua_getstack(pLua, iDepth, &ldT) == 0) {
            break;
        }
        lua_getinfo(pLua, "nSl", &ldT);
        cT = *(ldT.what);
        if ((cT != '\0') && (cT != 'C')) {
            iErrLine = ldT.currentline - 1;
        }
    }
    return iErrLine;
}

#endif
