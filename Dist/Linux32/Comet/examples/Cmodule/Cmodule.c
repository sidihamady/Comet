/*
    Comet example
    C Module
*/

/*
    HowTo write and use C module:
    Step 1. Write your C module and register the interface functions as in this example.
    Step 2. Build your C module with your favorite C compiler:
                Visual C++ on Windows or gcc or Clang on Linux, for example. Before compiling, add the Comet include directory (Comet Dir/include) in the compiler include
                    path and set the linker to link against the Comet Lua Core library (luacore.dll under Windows and
                    libluacore.so under Linux). Under Visual C++, you can generate, for your specific VC version, the lib from luacore.dll and
                    libluacore.def by using the lib tool included in Visual C++ as follows:
                        cd C:\Comet\bin\
                        dumpbin /exports luacore.dll > ..\lib\luacore.def
                        cd ..\lib\
                        lib /def:"luacore.def" /out:"luacore.lib" /machine:x64
                Under Linux, you can use a Makefile such as the one located in Comet/examples/Cmodule
                Of course, if your module is already built or purchased you can use it as usual with the Lua ad hoc functions.
    Step 3. When your module was built as dll (under Windows) or so (under Linux), you can load it and use it
                under Comet by using the standard Lua functions:
                    -- Load C module, the Lua standard way
                    cls()
                    local Cmodule = require("Cmodule")
                    print(Cmodule.version())
*/

/* put the Comet include dir in your include path */
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef WIN32
#define CMODULE_API __declspec(dllexport)
#else
#define CMODULE_API
#endif

/* compatibility with Lua 5.1 */
#if (LUA_VERSION_NUM == 501)
#define luaL_newlib(L, f) luaL_register(L, "Cmodule", f)
#endif

static int Cmodule_version(lua_State *pLua)
{
    lua_pushstring(pLua, "Cmodule v0.1");
    return 1;
}

static const luaL_Reg Cmodule[] = {
    { "version", Cmodule_version },
    { NULL, NULL }
};

CMODULE_API int luaopen_Cmodule(lua_State *pLua)
{
    luaL_newlib(pLua, Cmodule);
    return 1;
}
