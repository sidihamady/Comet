// -------------------------------------------------------------
//	Lua module header
//				Copyright(C) 2010-2022 Pr. Sidi HAMADY
// 			sidi@hamady.org
// -------------------------------------------------------------

#if !defined(liblmapm_h)
#define liblmapm_h

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

#if defined(LMAPM_LIB)
#define LMAPM_API __declspec(dllexport)
#else
#define LMAPM_API __declspec(dllimport)
#endif
#define API_DLLIMPORT __declspec(dllimport)

#else

#define LMAPM_API
#define API_DLLIMPORT

#endif

LMAPM_API int luaopen_lmapm(lua_State *pLua);

#endif
