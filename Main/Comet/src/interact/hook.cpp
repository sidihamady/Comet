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


#include "../include/Identifiers.h"

#include <math.h>
#include <float.h>

#include "../../include/CometApp.h"
#include "../../include/ScriptEdit.h"
#include "../../include/ScriptThread.h"

// Debugger
void luaHook(lua_State *pLua, lua_Debug *pDebug)
{
    if (CometApp::COMETCONSOLE) {
        return;
    }

    if (pDebug->event == LUA_HOOKLINE) {

        lua_pushliteral(pLua, "___SigmaThread___");
        lua_gettable(pLua, LUA_REGISTRYINDEX);
        ScriptThread *pThread = (ScriptThread *)lua_touserdata(pLua, -1);

        if (pThread == NULL) {
            return;
        }

        bool bStop = pThread->stopIt();
        if (bStop) {
            luaL_error(pLua, STOPPED_BY_USERA);
            return;
        }

        bool bDebug = pThread->debugIt();
        if (bDebug == false) {
            return;
        }

        bool bBreak = false;

        lua_Debug ldT;
        int iDepth = 0, iLine = -1, ii = 0;
        char cT = '\0';

        bool bStepInto = pThread->stepInto();

        int iDepthbreak = -1;

        for (iDepth = 0; iDepth < 64; iDepth++) {

            if (lua_getstack(pLua, iDepth, &ldT) == 0) {
                if ((iLine >= 0) && (cT != '\0') && (cT != 'C')) {
                    bBreak = true;
                }
                break;
            }

            lua_getinfo(pLua, "nSl", &ldT);
            cT = *(ldT.what);
            ii = ldT.currentline - 1;

            if (pThread->breakpointIn(ii)) {
                iLine = ii;
                iDepthbreak = iDepth;
            }
        }

        // Breakpoint set in the depth 0 (active level)?
        if ((bBreak == false) || ((bStepInto == false) && (iDepthbreak > 0))) {
            return;
        }

        pThread->highlightLine(iLine);
        pThread->breakIt(pDebug);
        while (true) {
            Tsleep(100);
            if (pThread->stopIt()) {
                luaL_error(pLua, STOPPED_BY_USERA);
                return;
            }
            if (pThread->debugIt()) {
                return;
            }
        }
    }
}
//
