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

static long socket_getid(lua_State *pLua)
{
    if (lua_isnumber(pLua, 1)) {
        return (long)lua_tonumber(pLua, 1);
    }

    if (!lua_isstring(pLua, 1)) {
        return luaL_errorlevel(pLua, "socket: invalid identifier");
    }
    const char *pszId = (const char *)lua_tostring(pLua, 1);
    if (pszId == NULL) {
        return luaL_errorlevel(pLua, "socket: invalid identifier");
    }
    return (long)atol(pszId);
}

// SOCKET
LUAEXT_API int socket_new(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 3) {
        return luaL_errorlevel(pLua, "socket.new: invalid number of arguments (should be 3)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = 0;

    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, "socket.new: invalid argument #1 (not a number)");
    }
    if (!lua_isnumber(pLua, 2)) {
        return luaL_errorlevel(pLua, "socket.new: invalid argument #2 (not a number)");
    }
    if (!lua_isnumber(pLua, 3)) {
        return luaL_errorlevel(pLua, "socket.new: invalid argument #3 (not a number)");
    }

    int iFamily = static_cast<int>(lua_tonumber(pLua, 1));
    int iType = static_cast<int>(lua_tonumber(pLua, 2));
    int iProtocol = static_cast<int>(lua_tonumber(pLua, 3));

    iId = pEngine->socketNew(iId, iFamily, iType, iProtocol);
    if (iId > 0L) {
        char szId[LM_STRSIZEN];
        memset(szId, 0, LM_STRSIZEN * sizeof(char));
        const char *pszIdx = Tltoa(iId, szId, 10);
        lua_pushstring(pLua, pszIdx);
    }
    else {
        const char *pszErr = "socket.new: call failed";
        return luaL_errorlevel(pLua, pEngine->socketGeterr(iId, pszErr));
    }

    return 1;
}

LUAEXT_API int socket_setsockopt(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 3) {
        return luaL_errorlevel(pLua, "socket.setsockopt: invalid number of arguments (should be 3)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    if (!lua_isnumber(pLua, 2)) {
        return luaL_errorlevel(pLua, "socket.setsockopt: invalid argument #2");
    }
    int iOpt = static_cast<int>(lua_tonumber(pLua, 2));

    if (!lua_isnumber(pLua, 3)) {
        return luaL_errorlevel(pLua, "socket.setsockopt: invalid argument #3");
    }
    long lBuf = static_cast<long>(lua_tonumber(pLua, 3));

    lua_pushboolean(pLua, (pEngine->socketSetsockopt(iId, iOpt, lBuf) == true) ? 1 : 0);

    return 1;
}

LUAEXT_API int socket_bind(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if ((nArgs != 3) && (nArgs != 4)) {
        return luaL_errorlevel(pLua, "socket.bind: invalid number of arguments (should be 3 or 4)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    if (!lua_isnumber(pLua, 2)) {
        return luaL_errorlevel(pLua, "socket.bind: invalid argument #2 (not a number)");
    }
    int iPort = static_cast<int>(lua_tonumber(pLua, 2));

    if (!lua_isnumber(pLua, 3)) {
        return luaL_errorlevel(pLua, "socket.bind: invalid argument #3 (not a number)");
    }
    long iAddr = static_cast<long>(lua_tonumber(pLua, 3));

    lua_pushboolean(pLua, (pEngine->socketBind(iId, iPort, iAddr) == true) ? 1 : 0);

    return 1;
}

LUAEXT_API int socket_listen(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        return luaL_errorlevel(pLua, "socket.listen: invalid number of arguments (should be 2)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    if (!lua_isnumber(pLua, 2)) {
        return luaL_errorlevel(pLua, "socket.listen: invalid argument #2 (not a number)");
    }

    int iBacklog = static_cast<int>(lua_tonumber(pLua, 2));

    lua_pushboolean(pLua, (pEngine->socketListen(iId, iBacklog) == true) ? 1 : 0);

    return 1;
}

LUAEXT_API int socket_connect(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if ((nArgs != 3) && (nArgs != 4)) {
        return luaL_errorlevel(pLua, "socket.connect: invalid number of arguments (should be 3 or 4)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    if (!lua_isstring(pLua, 2)) {
        return luaL_errorlevel(pLua, "socket.connect: invalid argument #2 (not a string)");
    }

    if (!lua_isnumber(pLua, 3)) {
        return luaL_errorlevel(pLua, "socket.connect: invalid argument #3 (not a number)");
    }

    const char *pszServer = static_cast<const char *>(lua_tostring(pLua, 2));
    int iPort = static_cast<int>(lua_tonumber(pLua, 3));

    lua_pushboolean(pLua, (pEngine->socketConnect(iId, pszServer, iPort) == true) ? 1 : 0);

    return 1;
}

LUAEXT_API int socket_accept(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, "socket.accept: invalid argument");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    iId = pEngine->socketAccept(iId);
    char szId[LM_STRSIZEN];
    memset(szId, 0, LM_STRSIZEN * sizeof(char));
    const char *pszIdx = Tltoa(iId, szId, 10);
    lua_pushstring(pLua, pszIdx);

    return 1;
}

LUAEXT_API int socket_timeout(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        return luaL_errorlevel(pLua, "socket.timeout: invalid number of arguments (should be 2)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    if (!lua_isnumber(pLua, 2)) {
        return luaL_errorlevel(pLua, "socket.timeout: invalid argument #2 (not a number)");
    }

    int uiTimeout = static_cast<int>(lua_tonumber(pLua, 2));

    lua_pushboolean(pLua, (pEngine->socketTimeout(iId, uiTimeout) == true) ? 1 : 0);

    return 1;
}

LUAEXT_API int socket_gethostbyaddr(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        return luaL_errorlevel(pLua, "socket.gethostbyaddr: invalid number of arguments (should be 2)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    const char *pszT = static_cast<const char *>(lua_tostring(pLua, 2));
    if (pszT == NULL) {
        return luaL_errorlevel(pLua, "socket.gethostbyaddr: invalid argument");
    }

    char szName[256];
    if (pEngine->socketGethostbyaddr(iId, pszT, szName)) {
        lua_pushstring(pLua, static_cast<const char *>(szName));
    }
    else {
        const char *pszErr = "socket.gethostbyaddr: call failed";
        lua_pushstring(pLua, pEngine->socketGeterr(iId, pszErr));
    }

    return 1;
}

LUAEXT_API int socket_gethostbyname(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        return luaL_errorlevel(pLua, "socket.gethostbyname: invalid number of arguments (should be 2)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    const char *pszT = static_cast<const char *>(lua_tostring(pLua, 2));
    if (pszT == NULL) {
        return luaL_errorlevel(pLua, "socket.gethostbyname: invalid argument");
    }

    char szName[256];
    if (pEngine->socketGethostbyname(iId, pszT, szName)) {
        lua_pushstring(pLua, static_cast<const char *>(szName));
    }
    else {
        const char *pszErr = "socket.gethostbyname: call failed";
        lua_pushstring(pLua, pEngine->socketGeterr(iId, pszErr));
    }

    return 1;
}

LUAEXT_API int socket_gethostname(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, "socket.gethostname: number argument");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    char szName[256];
    if (pEngine->socketGethostname(iId, szName)) {
        lua_pushstring(pLua, static_cast<const char *>(szName));
    }
    else {
        const char *pszErr = "socket.gethostname: call failed";
        lua_pushstring(pLua, pEngine->socketGeterr(iId, pszErr));
    }

    return 1;
}

LUAEXT_API int socket_getsockname(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, "socket.getsockname: number argument");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    char szName[256];
    if (pEngine->socketGetsockname(iId, szName)) {
        lua_pushstring(pLua, static_cast<const char *>(szName));
    }
    else {
        const char *pszErr = "socket.getsockname: call failed";
        lua_pushstring(pLua, pEngine->socketGeterr(iId, pszErr));
    }

    return 1;
}

LUAEXT_API int socket_getpeername(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, "socket.getpeername: invalid argument");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    char szName[256];
    if (pEngine->socketGetpeername(iId, szName)) {
        lua_pushstring(pLua, static_cast<const char *>(szName));
    }
    else {
        const char *pszErr = "socket.getpeername: call failed";
        lua_pushstring(pLua, pEngine->socketGeterr(iId, pszErr));
    }

    return 1;
}

LUAEXT_API int socket_send(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 3) {
        return luaL_errorlevel(pLua, "socket.send: invalid number of arguments (should be 3)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    if (!lua_isstring(pLua, 2)) {
        return luaL_errorlevel(pLua, "socket.send: invalid argument #2 (not a string)");
    }
    const char *pszMsg = static_cast<const char *>(lua_tostring(pLua, 2));

    int iFlags = static_cast<int>(lua_tonumber(pLua, 3));

    lua_pushnumberx(pLua, pEngine->socketSend(iId, pszMsg, iFlags));

    return 1;
}

LUAEXT_API int socket_sendto(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if ((nArgs != 5) && (nArgs != 6)) {
        return luaL_errorlevel(pLua, "socket.sendto: invalid number of arguments (should be 5 or 6)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    const char *pszServer = static_cast<const char *>(lua_tostring(pLua, 2));
    if (pszServer == NULL) {
        return luaL_errorlevel(pLua, "socket.sendto: invalid argument #1");
    }

    if (!lua_isnumber(pLua, 3)) {
        return luaL_errorlevel(pLua, "socket.sendto: invalid argument #3 (not a number)");
    }
    int iPort = static_cast<int>(lua_tonumber(pLua, 3));

    if (!lua_isstring(pLua, 4)) {
        return luaL_errorlevel(pLua, "socket.sendto: invalid argument #4 (not a string)");
    }
    const char *pszMsg = static_cast<const char *>(lua_tostring(pLua, 4));

    if (!lua_isnumber(pLua, 5)) {
        return luaL_errorlevel(pLua, "socket.sendto: invalid argument #5 (not a number)");
    }
    int iFlags = static_cast<int>(lua_tonumber(pLua, 5));

    lua_pushnumberx(pLua, pEngine->socketSendto(iId, pszServer, iPort, pszMsg, iFlags));

    return 1;
}

LUAEXT_API int socket_recv(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 3) {
        return luaL_errorlevel(pLua, "socket.recv: invalid number of arguments (should be 3)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    if (!lua_isnumber(pLua, 2)) {
        return luaL_errorlevel(pLua, "socket.recv: invalid argument #2 (not a number)");
    }

    if (!lua_isnumber(pLua, 3)) {
        return luaL_errorlevel(pLua, "socket.recv: invalid argument #3 (not a number)");
    }

    int iSize = static_cast<int>(lua_tonumber(pLua, 2));
    int iFlags = static_cast<int>(lua_tonumber(pLua, 3));

    const char *pszRet = pEngine->socketRecv(iId, iSize, iFlags);
    if (pszRet) {
        lua_pushstring(pLua, pszRet);
    }
    else {
        const char *pszErr = "socket.recv: call failed";
        lua_pushstring(pLua, pEngine->socketGeterr(iId, pszErr));
    }

    return 1;
}

LUAEXT_API int socket_recvfrom(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if ((nArgs != 5) && (nArgs != 6)) {
        return luaL_errorlevel(pLua, "socket.recvfrom: invalid number of arguments (should be 5 or 6)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    const char *pszServer = static_cast<const char *>(lua_tostring(pLua, 2));
    if (pszServer == NULL) {
        return luaL_errorlevel(pLua, "socket.recvfrom: invalid argument");
    }

    if (!lua_isnumber(pLua, 3)) {
        return luaL_errorlevel(pLua, "socket.recvfrom: invalid argument #3 (not a number)");
    }

    if (!lua_isnumber(pLua, 4)) {
        return luaL_errorlevel(pLua, "socket.recvfrom: invalid argument #4 (not a number)");
    }

    if (!lua_isnumber(pLua, 5)) {
        return luaL_errorlevel(pLua, "socket.recvfrom: invalid argument #5 (not a number)");
    }

    int iPort = static_cast<int>(lua_tonumber(pLua, 3));
    int iSize = static_cast<int>(lua_tonumber(pLua, 4));
    int iFlags = static_cast<int>(lua_tonumber(pLua, 5));

    const char *pszRet = pEngine->socketRecvfrom(iId, pszServer, iPort, iSize, iFlags);
    if (pszRet) {
        lua_pushstring(pLua, pszRet);
    }
    else {
        const char *pszErr = "socket.recvfrom: call failed";
        lua_pushstring(pLua, pEngine->socketGeterr(iId, pszErr));
    }

    return 1;
}

LUAEXT_API int socket_close(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, "socket.close: invalid number of arguments (should be 1)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    lua_pushboolean(pLua, (pEngine->socketClose(iId) == true) ? 1 : 0);

    return 1;
}

LUAEXT_API int socket_shutdown(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        return luaL_errorlevel(pLua, "socket.shutdown: invalid number of arguments (should be 2)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    if (!lua_isnumber(pLua, 2)) {
        return luaL_errorlevel(pLua, "socket.shutdown: invalid argument #2 (not a number)");
    }

    int iHow = static_cast<int>(lua_tonumber(pLua, 2));

    lua_pushboolean(pLua, (pEngine->socketShutdown(iId, iHow) == true) ? 1 : 0);

    return 1;
}

LUAEXT_API int socket_iserr(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        return luaL_errorlevel(pLua, "socket.iserr: invalid number of arguments (should be none)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    lua_pushboolean(pLua, (pEngine->socketIserr(iId) == true) ? 1 : 0);

    return 1;
}

LUAEXT_API int socket_geterr(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, "socket.geterr: invalid number of arguments (should be 1)");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    const char *pszErr = "socket.geterr: call failed";
    const char *pszRet = pEngine->socketGeterr(iId, pszErr);
    lua_pushstring(pLua, pszRet);

    return 1;
}

LUAEXT_API int socket_delete(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, "socket.delete: invalid argument");
    }

    lua_pushliteral(pLua, "___LuaEngine___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    LuaEngine *pEngine = static_cast<LuaEngine *>(lua_touserdata(pLua, -1));

    long iId = socket_getid(pLua);

    lua_pushboolean(pLua, (pEngine->socketDelete(iId) == true) ? 1 : 0);

    return 1;
}
//

#if WIN32
#define SOCKERR_EINTR           WSAEINTR
#define SOCKERR_EWOULDBLOCK     WSAEWOULDBLOCK
#define SOCKERR_EINPROGRESS     WSAEINPROGRESS
#define SOCKERR_EALREADY        WSAEALREADY
#define SOCKERR_ENOTSOCK        WSAENOTSOCK
#define SOCKERR_EINVAL          WSAEINVAL
#define SOCKERR_EDESTADDRREQ    WSAEDESTADDRREQ
#define SOCKERR_EMSGSIZE        WSAEMSGSIZE
#define SOCKERR_EPROTOTYPE      WSAEPROTOTYPE
#define SOCKERR_ENOPROTOOPT     WSAENOPROTOOPT
#define SOCKERR_EPROTONOSUPPORT WSAEPROTONOSUPPORT
#define SOCKERR_ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
#define SOCKERR_EOPNOTSUPP      WSAEOPNOTSUPP
#define SOCKERR_EPFNOSUPPORT    WSAEPFNOSUPPORT
#define SOCKERR_EAFNOSUPPORT    WSAEAFNOSUPPORT
#define SOCKERR_EADDRINUSE      WSAEADDRINUSE
#define SOCKERR_EADDRNOTAVAIL   WSAEADDRNOTAVAIL
#define SOCKERR_ENETDOWN        WSAENETDOWN
#define SOCKERR_ENETUNREACH     WSAENETUNREACH
#define SOCKERR_ENETRESET       WSAENETRESET
#define SOCKERR_ECONNABORTED    WSAECONNABORTED
#define SOCKERR_ECONNRESET      WSAECONNRESET
#define SOCKERR_ENOBUFS         WSAENOBUFS
#define SOCKERR_EISCONN         WSAEISCONN
#define SOCKERR_ENOTCONN        WSAENOTCONN
#define SOCKERR_ESHUTDOWN       WSAESHUTDOWN
#define SOCKERR_ETOOMANYREFS    WSAETOOMANYREFS
#define SOCKERR_ETIMEDOUT       WSAETIMEDOUT
#define SOCKERR_ECONNREFUSED    WSAECONNREFUSED
#define SOCKERR_ELOOP           WSAELOOP
#define SOCKERR_ENAMETOOLONG    WSAENAMETOOLONG
#define SOCKERR_EHOSTDOWN       WSAEHOSTDOWN
#define SOCKERR_EHOSTUNREACH    WSAEHOSTUNREACH
#define SOCKERR_ENOTEMPTY       WSAENOTEMPTY
#define SOCKERR_EPROCLIM        WSAEPROCLIM
#define SOCKERR_EUSERS          WSAEUSERS
#define SOCKERR_EDQUOT          WSAEDQUOT
#define SOCKERR_ESTALE          WSAESTALE
#define SOCKERR_EREMOTE         WSAEREMOTE
#else
#define SOCKERR_EINTR           EINTR
#define SOCKERR_EWOULDBLOCK     EWOULDBLOCK
#define SOCKERR_EINPROGRESS     EINPROGRESS
#define SOCKERR_EALREADY        EALREADY
#define SOCKERR_ENOTSOCK        ENOTSOCK
#define SOCKERR_EINVAL          EINVAL
#define SOCKERR_EDESTADDRREQ    EDESTADDRREQ
#define SOCKERR_EMSGSIZE        EMSGSIZE
#define SOCKERR_EPROTOTYPE      EPROTOTYPE
#define SOCKERR_ENOPROTOOPT     ENOPROTOOPT
#define SOCKERR_EPROTONOSUPPORT EPROTONOSUPPORT
#define SOCKERR_ESOCKTNOSUPPORT ESOCKTNOSUPPORT
#define SOCKERR_EOPNOTSUPP      EOPNOTSUPP
#define SOCKERR_EPFNOSUPPORT    EPFNOSUPPORT
#define SOCKERR_EAFNOSUPPORT    EAFNOSUPPORT
#define SOCKERR_EADDRINUSE      EADDRINUSE
#define SOCKERR_EADDRNOTAVAIL   EADDRNOTAVAIL
#define SOCKERR_ENETDOWN        ENETDOWN
#define SOCKERR_ENETUNREACH     ENETUNREACH
#define SOCKERR_ENETRESET       ENETRESET
#define SOCKERR_ECONNABORTED    ECONNABORTED
#define SOCKERR_ECONNRESET      ECONNRESET
#define SOCKERR_ENOBUFS         ENOBUFS
#define SOCKERR_EISCONN         EISCONN
#define SOCKERR_ENOTCONN        ENOTCONN
#define SOCKERR_ESHUTDOWN       ESHUTDOWN
#define SOCKERR_ETOOMANYREFS    ETOOMANYREFS
#define SOCKERR_ETIMEDOUT       ETIMEDOUT
#define SOCKERR_ECONNREFUSED    ECONNREFUSED
#define SOCKERR_ELOOP           ELOOP
#define SOCKERR_ENAMETOOLONG    ENAMETOOLONG
#define SOCKERR_EHOSTDOWN       EHOSTDOWN
#define SOCKERR_EHOSTUNREACH    EHOSTUNREACH
#define SOCKERR_ENOTEMPTY       ENOTEMPTY
#define SOCKERR_EPROCLIM        77
#define SOCKERR_EUSERS          EUSERS
#define SOCKERR_EDQUOT          EDQUOT
#define SOCKERR_ESTALE          ESTALE
#define SOCKERR_EREMOTE         EREMOTE
#endif

#define SOCKERR_HOSTNOTFOUND HOST_NOT_FOUND
#define SOCKERR_NOADDRESS    NO_ADDRESS
#define SOCKERR_ENONRECOV    -1

const int lmSocket::ERRINFO_COUNT = 42;
const SocketErrorInfo lmSocket::ERRINFO[] = {
    { SOCKERR_EINTR, "Interrupted function call (EINTR)." },
    { SOCKERR_EWOULDBLOCK, "Operation would block (EWOULDBLOCK)." },
    { SOCKERR_EINPROGRESS, "Operation now in progress (EINPROGRESS)." },
    { SOCKERR_EALREADY, "Operation already in progress (EALREADY)." },
    { SOCKERR_ENOTSOCK, "Socket operation on nonsocket (ENOTSOCK)." },
    { SOCKERR_EINVAL, "Invalid argument (input or socket connection status) (EINVAL)." },
    { SOCKERR_EDESTADDRREQ, "Destination address required (EDESTADDRREQ)." },
    { SOCKERR_EMSGSIZE, "Message too long (EMSGSIZE)." },
    { SOCKERR_EPROTOTYPE, "Protocol wrong type for socket (EPROTOTYPE)." },
    { SOCKERR_ENOPROTOOPT, "Protocol not available (ENOPROTOOPT)." },
    { SOCKERR_EPROTONOSUPPORT, "Protocol not supported (EPROTONOSUPPORT)." },
    { SOCKERR_ESOCKTNOSUPPORT, "Socket type not supported (ESOCKTNOSUPPORT)." },
    { SOCKERR_EOPNOTSUPP, "Operation not supported on socket (EOPNOTSUPP)." },
    { SOCKERR_EPFNOSUPPORT, "Protocol family not supported (EPFNOSUPPORT)." },
    { SOCKERR_EAFNOSUPPORT, "Address family not supported by protocol family (EAFNOSUPPORT)." },
    { SOCKERR_EADDRINUSE, "Address already in use (EADDRINUSE)." },
    { SOCKERR_EADDRNOTAVAIL, "Cannot assign requested address (EADDRNOTAVAIL)." },
    { SOCKERR_ENETDOWN, "Network is down (ENETDOWN)." },
    { SOCKERR_ENETUNREACH, "Network is unreachable (ENETUNREACH)." },
    { SOCKERR_ENETRESET, "Network dropped connection on reset (ENETRESET)." },
    { SOCKERR_ECONNABORTED, "Software caused connection abort (ECONNABORTED)." },
    { SOCKERR_ECONNRESET, "Connection reset by peer (ECONNRESET)." },
    { SOCKERR_ENOBUFS, "No buffer space available (ENOBUFS)." },
    { SOCKERR_EISCONN, "Socket is already connected (EISCONN)." },
    { SOCKERR_ENOTCONN, "Socket is not connected (ENOTCONN)." },
    { SOCKERR_ESHUTDOWN, "Cannot send after socket shutdown (ESHUTDOWN)." },
    { SOCKERR_ETOOMANYREFS, "Too many references : cannot splice (ETOOMANYREFS)." },
    { SOCKERR_ETIMEDOUT, "Connection timed out (ETIMEDOUT)." },
    { SOCKERR_ECONNREFUSED, "Connection refused (ECONNREFUSED)." },
    { SOCKERR_ELOOP, " Too many levels of symbolic links (ELOOP)." },
    { SOCKERR_ENAMETOOLONG, "File name too long (ENAMETOOLONG)." },
    { SOCKERR_EHOSTDOWN, "Host is down (EHOSTDOWN)." },
    { SOCKERR_EHOSTUNREACH, "No route to host (EHOSTUNREACH)." },
    { SOCKERR_ENOTEMPTY, "Directory not empty (ENOTEMPTY)." },
    { SOCKERR_EPROCLIM, "Too many processes (EPROCLIM)." },
    { SOCKERR_EUSERS, "Too many users (EUSERS)." },
    { SOCKERR_EDQUOT, "Quota exceeded (EDQUOT)." },
    { SOCKERR_ESTALE, "Stale NFS file handle (ESTALE)." },
    { SOCKERR_EREMOTE, "Remote I/O error (EREMOTE)." },
    { SOCKERR_HOSTNOTFOUND, "Unknown host (HOST_NOT_FOUND)." },
    { SOCKERR_NOADDRESS, "The requested name does not have an IP address (NO_ADDRESS)." },
    { SOCKERR_ENONRECOV, "Call failed." }
};

#ifdef WIN32
#include <Ws2tcpip.h>
typedef int socklen_t;
WSADATA lmSocket::m_wsaData;
void lmSocket::startup(void)
{
    int nret = WSAStartup(MAKEWORD(2, 0), &(m_wsaData));
    if (nret < 0) {
        throw(nret);
    }
}
void lmSocket::shutdown(void)
{
    WSACleanup();
}
#endif

lmSocket::lmSocket(long iId, int iFamily /* = AF_INET*/, int iType /* = SOCK_STREAM*/,
                   int iProtocol /* = IPPROTO_TCP*/)
{
    m_bError = false;

    m_Id = iId;
    m_Addrin.sin_family = iFamily;
    m_pszBuffer = NULL;
    m_iBufferSize = 0;
    memset(m_szError, 0, 256 * sizeof(char));
    m_iAccepted = 0L;

    m_iSocket = (int)(socket(iFamily, iType, iProtocol));
    if (m_iSocket <= 0) {
        this->error("new");
        return;
    }

    long lBuf = 1L;
    this->setsockopt(SO_REUSEADDR, lBuf);
    this->setsockopt(SO_ACCEPTCONN, lBuf);
    this->timeout(10);
}

lmSocket::lmSocket(long iId, int iSocket, bool bFlags)
{
    m_bError = false;

    m_Id = iId;
    m_iSocket = iSocket;
    m_iBufferSize = 0;
    m_pszBuffer = NULL;
    m_iAccepted = 0L;
    memset(m_szError, 0, 256 * sizeof(char));
}

lmSocket::~lmSocket()
{
    m_bError = false;

    this->release();
}

bool lmSocket::setsockopt(int iOpt, long lBuf)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return false;
    }

#ifdef WIN32
    ::setsockopt(m_iSocket, SOL_SOCKET, iOpt, reinterpret_cast<const char *>(&lBuf), sizeof(long));
#else
    ::setsockopt(m_iSocket, SOL_SOCKET, iOpt, &lBuf, sizeof(long));
#endif
    return true;
}

bool lmSocket::bind(int iPort, long lInaddr /* = INADDR_ANY*/)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return false;
    }

    m_Addrin.sin_port = htons(iPort);
    m_Addrin.sin_addr.s_addr = lInaddr;
    memset(m_Addrin.sin_zero, '\0', sizeof(m_Addrin.sin_zero));

    int nret = ::bind(m_iSocket, reinterpret_cast<struct sockaddr *>(&m_Addrin), sizeof(m_Addrin));
    if (nret < 0) {
        this->error("bind");
        return false;
    }
    return true;
}

bool lmSocket::listen(int iBacklog)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return false;
    }

    int nret = ::listen(m_iSocket, iBacklog);
    if (nret < 0) {
        this->error("listen");
        return false;
    }
    return true;
}

lmSocket *lmSocket::accept(void)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return NULL;
    }

    int tFd;
    struct sockaddr_in tAddr;

    socklen_t tSize = sizeof(tAddr);

    tFd = (int)(::accept(m_iSocket, reinterpret_cast<struct sockaddr *>(&tAddr), &tSize));
    if (tFd <= 0) {
        this->error("accept");
        return NULL;
    }

    lmSocket *pSocket = NULL;

    try {
        pSocket = new lmSocket(0L, tFd, false);
        m_iAccepted += 1L;
    }
    catch (...) {
        this->error("accept");
        return NULL;
    }

    return pSocket;
}

struct in_addr *lmSocket::DoGethostbyname(const char *pszServer)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return NULL;
    }

    struct hostent *hbn = ::gethostbyname(pszServer);

    if (hbn == 0) {
        return NULL;
    }

    return reinterpret_cast<struct in_addr *>(hbn->h_addr);
}

bool lmSocket::getpeername(char *pszName)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return false;
    }

    struct sockaddr_in peerT;
    socklen_t sizeT = sizeof(peerT);

    int nret = ::getpeername(m_iSocket, reinterpret_cast<struct sockaddr *>(&peerT), &sizeT);
    if (nret < 0) {
        this->error("getpeername");
        return false;
    }

    strcpy(pszName, static_cast<const char *>(inet_ntoa(peerT.sin_addr)));
    return true;
}

bool lmSocket::gethostbyaddr(const char *pszT, char *pszName)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return false;
    }

    struct hostent *hbn = NULL;

    hbn = ::gethostbyaddr(pszT, static_cast<int>(strlen(pszT)), m_Addrin.sin_family);
    if (hbn == 0) {
        this->error("gethostbyaddr");
        return false;
    }

    strcpy(pszName, static_cast<const char *>(hbn->h_name));
    return true;
}

bool lmSocket::gethostbyname(const char *pszT, char *pszName)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return false;
    }

    struct in_addr *pAddr = this->DoGethostbyname(pszT);
    if (pAddr == NULL) {
        this->error("gethostbyname");
        return false;
    }

    strcpy(pszName, static_cast<const char *>(inet_ntoa(*pAddr)));

    return true;
}

bool lmSocket::gethostname(char *pszName)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return false;
    }

    char szName[256];

    int nret = ::gethostname(szName, 256 * sizeof(char));
    if (nret < 0) {
        this->error("gethostname");
        return false;
    }

    strcpy(pszName, static_cast<const char *>(szName));
    return true;
}

bool lmSocket::getsockname(char *pszName)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return false;
    }

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    int nret = ::getsockname(m_iSocket, reinterpret_cast<struct sockaddr *>(&addr), &addrlen);
    if (nret < 0) {
        this->error("getsockname");
        return false;
    }

    sprintf(pszName, "%s:%d", static_cast<const char *>(inet_ntoa(addr.sin_addr)), addr.sin_port);

    return true;
}

bool lmSocket::connect(const char *pszServer, int iPort)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return false;
    }

    struct in_addr *pAddr = this->DoGethostbyname(pszServer);
    if (pAddr == NULL) {
        this->error("connect");
        return false;
    }
    m_Addrin.sin_port = htons(iPort);
    m_Addrin.sin_addr = *pAddr;
    memset(m_Addrin.sin_zero, '\0', sizeof(m_Addrin.sin_zero));

    int nret = ::connect(m_iSocket, reinterpret_cast<struct sockaddr *>(&m_Addrin), sizeof(m_Addrin));
    if (nret < 0) {
        this->error("connect");
        return false;
    }
    return true;
}

bool lmSocket::timeout(int iTimeout)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return false;
    }

#ifdef WIN32
    long lT = 1000L * static_cast<long>(iTimeout);
    ::setsockopt(m_iSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&lT), sizeof(long));
    ::setsockopt(m_iSocket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char *>(&lT), sizeof(long));
#else
    struct timeval tv;
    tv.tv_sec = (long)iTimeout;
    tv.tv_usec = 0L;
    ::setsockopt(m_iSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
    ::setsockopt(m_iSocket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(struct timeval));
#endif
    return true;
}

int lmSocket::send(const char *pszMsg, int iFlags /* = 0*/)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return 0;
    }

    int iLen = (int)(strlen(pszMsg));
    int nret = ::send(m_iSocket, pszMsg, iLen, iFlags);
    if (nret < 0) {
        this->error("send");
        return nret;
    }

    return nret;
}
int lmSocket::sendto(const char *pszServer, int iPort, const char *pszMsg, int iFlags /* = 0*/)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return 0;
    }

    int iLen = (int)(strlen(pszMsg));
    int nret = -1;

    struct in_addr *pAddr = this->DoGethostbyname(pszServer);
    if (pAddr == NULL) {
        this->error("sendto");
        return 0;
    }

    m_Addrin.sin_port = htons(iPort);
    m_Addrin.sin_addr = *pAddr;
    memset(m_Addrin.sin_zero, '\0', sizeof(m_Addrin.sin_zero));

    nret = ::sendto(m_iSocket, pszMsg, iLen, iFlags, reinterpret_cast<struct sockaddr *>(&m_Addrin), sizeof(m_Addrin));
    if (nret < 0) {
        this->error("sendto");
        return nret;
    }

    return nret;
}

int lmSocket::write(const char *pszMsg)
{
    return this->send(pszMsg, 0);
}

const char *lmSocket::recv(int iSize /* = 4096*/, int iFlags /* = 0*/)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return NULL;
    }

    int nret = 0;

    if (m_pszBuffer == NULL) {
        m_pszBuffer = static_cast<char *>(malloc((iSize + 1) * sizeof(char)));
        if (m_pszBuffer) {
            m_iBufferSize = (iSize + 1);
            m_pszBuffer[0] = '\0';
        }
        else {
            return NULL;
        }
    }
    else if (iSize >= m_iBufferSize) {
        char *pszBufferT = static_cast<char *>(realloc(m_pszBuffer, (iSize + 1) * sizeof(char)));
        if (pszBufferT) {
            m_pszBuffer = pszBufferT;
            m_iBufferSize = (iSize + 1);
        }
        else {
            return NULL;
        }
    }

    nret = ::recv(m_iSocket, m_pszBuffer, iSize, 0);
    if ((nret >= 0) && (nret < m_iBufferSize)) {
        m_pszBuffer[nret] = '\0';
    }

    return static_cast<const char *>(m_pszBuffer);
}
const char *lmSocket::recvfrom(const char *pszServer, int iPort, int iSize /* = 4096*/, int iFlags /* = 0*/)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return NULL;
    }

    int nret = 0;

    struct in_addr *pAddr = this->DoGethostbyname(pszServer);
    if (pAddr == NULL) {
        this->error("recvfrom");
        return NULL;
    }

    if (m_pszBuffer == NULL) {
        m_pszBuffer = static_cast<char *>(malloc((iSize + 1) * sizeof(char)));
        if (m_pszBuffer) {
            m_iBufferSize = (iSize + 1);
            m_pszBuffer[0] = '\0';
        }
        else {
            return NULL;
        }
    }
    else if (iSize >= m_iBufferSize) {
        char *pszBufferT = static_cast<char *>(realloc(m_pszBuffer, (iSize + 1) * sizeof(char)));
        m_iBufferSize = (iSize + 1);
        if (pszBufferT) {
            m_pszBuffer = pszBufferT;
            m_iBufferSize = (iSize + 1);
        }
        else {
            return NULL;
        }
    }

    m_Addrin.sin_port = htons(iPort);
    m_Addrin.sin_addr = *pAddr;
    memset(m_Addrin.sin_zero, '\0', sizeof(m_Addrin.sin_zero));

    int iT = sizeof(m_Addrin);

    nret = ::recvfrom(m_iSocket, m_pszBuffer, iSize, iFlags, reinterpret_cast<struct sockaddr *>(&m_Addrin), (socklen_t *)(&iT));
    if ((nret >= 0) && (nret < m_iBufferSize)) {
        m_pszBuffer[nret] = '\0';
    }

    return static_cast<const char *>(m_pszBuffer);
}
const char *lmSocket::read(int iSize /* = 4096*/)
{
    return this->recv(iSize, 0);
}

bool lmSocket::close()
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return false;
    }

#ifdef WIN32
    int nret = closesocket(m_iSocket);
#else
    int nret = ::close(m_iSocket);
#endif

    if (nret != 0) {
        this->error("close");
        return false;
    }

    m_iSocket = 0;
    return true;
}

bool lmSocket::shutdown(int iHow)
{
    m_bError = false;

    if (m_iSocket <= 0) {
        return false;
    }

#ifdef WIN32
    int nret = ::shutdown(m_iSocket, iHow);
#else
    int nret = ::shutdown(m_iSocket, iHow);
#endif

    if (nret != 0) {
        this->error("shutdown");
        return false;
    }

    return true;
}

void lmSocket::release()
{
    m_bError = false;

    this->close();

    if (m_pszBuffer) {
        free(m_pszBuffer);
        m_pszBuffer = NULL;
    }

    m_iAccepted = 0L;
}

void lmSocket::error(const char *pszSource /* = NULL*/)
{
    m_bError = true;

    if (pszSource) {
        strcpy(m_szError, pszSource);
        strcat(m_szError, ": ");
    }
    else {
        memset(m_szError, 0, 256 * sizeof(char));
    }

#ifdef WIN32
    int iErr = WSAGetLastError();
#else
    int iErr = errno;
#endif

    if (iErr > 0) {
        for (int ii = 0; ii < ERRINFO_COUNT; ii++) {
            if (ERRINFO[ii].id == iErr) {
                strcat(m_szError, ERRINFO[ii].info);
                return;
            }
        }
    }
    strcat(m_szError, ERRINFO[ERRINFO_COUNT - 1].info);
}
