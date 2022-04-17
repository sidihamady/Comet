// -------------------------------------------------------------
// Lua module header
//      Copyright(C) 2010-2022 Pr. Sidi HAMADY
//      sidi@hamady.org
// -------------------------------------------------------------

#if !defined(liblpeg_h)
#define liblpeg_h

#if !defined(LPEG_DEBUG)
#if !defined(NDEBUG)
#define NDEBUG
#endif
#endif

#ifdef __ANDROID__

#include "../core/lua.h"
#include "../core/lualib.h"
#include "../core/lauxlib.h"

#else

#include "../../../LuaJIT/src/lua.h"
#include "../../../LuaJIT/src/lualib.h"
#include "../../../LuaJIT/src/lauxlib.h"

#endif

#ifdef WIN32

#if defined(LIBLPEG_LIB)
#define LIBLPEG_API __declspec(dllexport)
#else
#define LIBLPEG_API __declspec(dllimport)
#endif
#define API_DLLIMPORT __declspec(dllimport)

#else

#define LIBLPEG_API
#define API_DLLIMPORT

#endif

LIBLPEG_API int luaopen_lpeg(lua_State *pLua);

#endif
