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
#include "../../include/ConsoleEdit.h"
#include "../../include/ConsoleThread.h"

#define LUA_FUNC_PRINT     true
#define LUA_FUNC_IOWRITE   false
#define LUA_PRINT_MAXCOUNT 64

// IO
int consoleDoClear(lua_State *pLua)
{
    if (system(NULL)) {
#ifdef __WXMSW__
        win32ClearConsole();
        //system("cls");
#else
        system("printf \"\\033c\"");
#endif
    }

    return 0;
}

static int luaX_isinteger(lua_State *pLua, int idx)
{
    lua_Number fx = lua_tonumber(pLua, idx);
    lua_Number fxr = (lua_Number)round(fx);
    if (fabs(fx - fxr) <= FLT_EPSILON) {
        return 1;
    }

    return 0;
}

static const char *luaX_tolstring(lua_State *pLua, int idx, size_t *len)
{
    if (!luaL_callmeta(pLua, idx, "__tostring")) { /* no metafield? */
        switch (lua_type(pLua, idx)) {
            case LUA_TNUMBER:
            {
                if (luaX_isinteger(pLua, idx)) {
                    lua_pushfstring(pLua, "%d", lua_tointeger(pLua, idx));
                }
                else {
                    lua_pushfstring(pLua, "%f", lua_tonumber(pLua, idx));
                }
                break;
            }
            case LUA_TSTRING:
                lua_pushvalue(pLua, idx);
                break;
            case LUA_TBOOLEAN:
                lua_pushstring(pLua, (lua_toboolean(pLua, idx) ? "true" : "false"));
                break;
            case LUA_TNIL:
                lua_pushliteral(pLua, "nil");
                break;
            default:
                lua_pushfstring(pLua, "%s: %p", luaL_typename(pLua, idx), lua_topointer(pLua, idx));
                break;
        }
    }
    return lua_tolstring(pLua, -1, len);
}

static int consoleDoWrite(lua_State *pLua, bool bFunc = LUA_FUNC_IOWRITE)
{
    const char *pszNil = "nil";

    int nArgCount = lua_gettop(pLua);
    if ((nArgCount < 0) || (nArgCount > LUA_PRINT_MAXCOUNT)) {
        lua_pushstring(pLua, "");
        return luaL_errorlevel(pLua, "print: invalid argument");
    }
    if (0 == nArgCount) {
        // conformance to the common behavior of print()
        if (bFunc == LUA_FUNC_PRINT) {
            printf("\n");
        }
        return 0;
    }

    char *pszArg;
    int nn, ne, jj, itop;
    for (int ii = 1; ii <= nArgCount; ii++) {
        if (lua_istable(pLua, ii)) {
            nn = (int)(luaL_len(pLua, ii));
            ne = (nn < 10) ? nn : 10;
            if (ne > 0) {
                printf("{ ");
                for (jj = 1; jj <= ne; jj++) {
                    lua_rawgeti(pLua, ii, jj);
                    itop = lua_gettop(pLua);
                    pszArg = (char *)luaX_tolstring(pLua, itop, NULL);
                    if (pszArg != NULL) {
                        printf("%s", (const char *)pszArg);
                        if (jj < ne) {
                            printf(", ");
                        }
                    }
                    lua_pop(pLua, 1);
                }
                if (nn > 10) {
                    printf(" ...");
                }
                printf(" }");
            }
        }
        else {
            if (lua_isnil(pLua, ii)) {
                pszArg = (char *)pszNil;
            }
            else {
                pszArg = (char *)luaX_tolstring(pLua, ii, NULL);
            }
            if (pszArg == NULL) {
                if (lua_isboolean(pLua, ii)) {
                    printf(lua_toboolean(pLua, ii) ? ("true") : ("false"));
                }
                else {
                    const void *ptrT = lua_topointer(pLua, ii);
                    if (ptrT == NULL) {
                        const void *ptrU = lua_touserdata(pLua, ii);
                        if (ptrU == NULL) {
                            printf("nil");
                        }
                        else {
                            printf(("%p"), ptrT);
                        }
                    }
                    else {
                        printf(("%p"), (ptrT));
                    }
                }
            }
            else {
                printf("%s", (const char *)pszArg);
            }
        }

        if (bFunc == LUA_FUNC_PRINT) {
            if (ii < nArgCount) {
                printf("   ");
            }
        }
    }

    // conformance to the common behavior of print()
    if (bFunc == LUA_FUNC_PRINT) {
        printf("\n");
    }

    return 0;
}

int consoleLuaWrite(lua_State *pLua)
{
    return consoleDoWrite(pLua, LUA_FUNC_IOWRITE);
}

int consoleLuaPrint(lua_State *pLua)
{
    return consoleDoWrite(pLua, LUA_FUNC_PRINT);
}

//

static int doWrite(lua_State *pLua, bool bFunc = LUA_FUNC_IOWRITE)
{
    const char *pszNil = "nil";

    if (CometApp::COMETCONSOLE) {
        return consoleDoWrite(pLua, bFunc);
    }

    wxString strT = uT("");

    int nArgCount = lua_gettop(pLua);
    if ((nArgCount < 0) || (nArgCount > LUA_PRINT_MAXCOUNT)) {
        lua_pushstring(pLua, "");
        return luaL_errorlevel(pLua, "print: invalid argument");
    }

    if (0 == nArgCount) {
        // conformance to the common behavior of print()
        if (bFunc != LUA_FUNC_PRINT) {
            return 0;
        }
    }
    else {
        char *pszArg;
        int nn, ne, jj, itop;
        bool bNL = false;
        for (int ii = 1; ii <= nArgCount; ii++) {
            bNL = false;
            if (lua_istable(pLua, ii)) {
                nn = (int)(luaL_len(pLua, ii));
                ne = (nn < 10) ? nn : 10;
                if (ne > 0) {
                    strT += uT("{ ");
                    for (jj = 1; jj <= ne; jj++) {
                        lua_rawgeti(pLua, ii, jj);
                        itop = lua_gettop(pLua);
                        pszArg = (char *)luaX_tolstring(pLua, itop, NULL);
                        if (pszArg != NULL) {
                            // Convert ANSI to Unicode
                            strT += LM_U8TOWC(pszArg);
                            if (jj < ne) {
                                strT += uT(", ");
                            }
                        }
                        lua_pop(pLua, 1);
                    }
                    if (nn > 10) {
                        strT += uT(" ...");
                    }
                    strT += uT(" }");
                }
            }
            else {
                if (lua_isnil(pLua, ii)) {
                    pszArg = (char *)pszNil;
                }
                else {
                    pszArg = (char *)luaX_tolstring(pLua, ii, NULL);
                }
                if (pszArg == NULL) {
                    if (lua_isboolean(pLua, ii)) {
                        strT += lua_toboolean(pLua, ii) ? uT("true") : uT("false");
                    }
                    else {
                        const void *ptrT = lua_topointer(pLua, ii);
                        if (ptrT == NULL) {
                            const void *ptrU = lua_touserdata(pLua, ii);
                            if (ptrU == NULL) {
                                strT += uT("nil");
                            }
                            else {
                                strT += wxString::Format(uT("%p"), (ptrT));
                            }
                        }
                        else {
                            strT += wxString::Format(uT("%p"), (ptrT));
                        }
                    }
                }
                else {
                    // Convert ANSI to Unicode
                    strT += LM_U8TOWC(pszArg);
                    bNL = (*pszArg == LM_NEWLINE_CHARA);
                }
            }

            if (bFunc == LUA_FUNC_PRINT) {
                if ((bNL == false) && (ii < nArgCount)) {
                    strT += uT("   ");
                }
            }
        }
    }

    // conformance to the common behavior of print()
    if (bFunc == LUA_FUNC_PRINT) {
        strT += uT("\n");
    }

    lua_pushliteral(pLua, "___SigmaCaller___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    int iSigmaCaller = (int)lua_tointeger(pLua, -1);
    if ((iSigmaCaller != SIGMACALLER_EDITOR) && (iSigmaCaller != SIGMACALLER_CONSOLE)) {
        return luaL_errorlevel(pLua, "print: internal error"); // Should never happen
    }

    lua_pushliteral(pLua, "___ScriptEdit___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    ScriptEdit *pScriptEdit = (iSigmaCaller == SIGMACALLER_EDITOR) ? (ScriptEdit *)lua_touserdata(pLua, -1) : NULL;
    ConsoleEdit *pConsoleEdit = (iSigmaCaller == SIGMACALLER_CONSOLE) ? (ConsoleEdit *)lua_touserdata(pLua, -1) : NULL;

    lua_pushliteral(pLua, "___SigmaThread___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    ScriptThread *pScriptThread = (iSigmaCaller == SIGMACALLER_EDITOR) ? (ScriptThread *)lua_touserdata(pLua, -1) : NULL;
    ConsoleThread *pConsoleThread = (iSigmaCaller == SIGMACALLER_CONSOLE) ? (ConsoleThread *)lua_touserdata(pLua, -1) : NULL;

    if (iSigmaCaller == SIGMACALLER_EDITOR) {
        pScriptThread->setLastPrint(strT);
    }
    else if (iSigmaCaller == SIGMACALLER_CONSOLE) {
        pConsoleThread->setLastPrint(strT);
    }

    wxCommandEvent eventT(wxEVT_COMMAND_TEXT_UPDATED, ID_THREAD_PRINT);
    eventT.SetString(strT);
    if (iSigmaCaller == SIGMACALLER_EDITOR) {
        pScriptEdit->GetEventHandler()->AddPendingEvent(eventT);
    }
    else if (iSigmaCaller == SIGMACALLER_CONSOLE) {
        pConsoleEdit->GetEventHandler()->AddPendingEvent(eventT);
    }

    return 0;
}

int luaWrite(lua_State *pLua)
{
    if (CometApp::COMETCONSOLE) {
        return consoleDoWrite(pLua, LUA_FUNC_IOWRITE);
    }

    return doWrite(pLua, LUA_FUNC_IOWRITE);
}

int luaRead(lua_State *pLua)
{
    if (CometApp::COMETCONSOLE) {
        return consoleDoWrite(pLua, LUA_FUNC_IOWRITE);
    }

    int nArgCount = 0;

    nArgCount = lua_gettop(pLua);

    if ((nArgCount != 0) && (nArgCount != 1)) {
        lua_pushstring(pLua, "");
        return luaL_errorlevel(pLua, "io.read: invalid argument");
    }

    lua_pushliteral(pLua, "___SigmaCaller___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    int iSigmaCaller = (int)lua_tointeger(pLua, -1);
    if ((iSigmaCaller != SIGMACALLER_EDITOR) && (iSigmaCaller != SIGMACALLER_CONSOLE)) {
        return luaL_errorlevel(pLua, "io.read: internal error"); // Should never happen
    }

    lua_pushliteral(pLua, "___ScriptEdit___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    ScriptEdit *pScriptEdit = (iSigmaCaller == SIGMACALLER_EDITOR) ? (ScriptEdit *)lua_touserdata(pLua, -1) : NULL;
    ConsoleEdit *pConsoleEdit = (iSigmaCaller == SIGMACALLER_CONSOLE) ? (ConsoleEdit *)lua_touserdata(pLua, -1) : NULL;

    lua_pushliteral(pLua, "___SigmaThread___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    ScriptThread *pScriptThread = (iSigmaCaller == SIGMACALLER_EDITOR) ? (ScriptThread *)lua_touserdata(pLua, -1) : NULL;
    ConsoleThread *pConsoleThread = (iSigmaCaller == SIGMACALLER_CONSOLE) ? (ConsoleThread *)lua_touserdata(pLua, -1) : NULL;

    char *pszArg = NULL;
    char szRet[LM_STRSIZE];
    int iNum = 0;

    memset(szRet, 0, LM_STRSIZE * sizeof(char));

    if (nArgCount == 1) {
        pszArg = (char *)luaX_tolstring(pLua, 1, NULL);
        if (pszArg == NULL) {
            lua_pushstring(pLua, "");
            return luaL_errorlevel(pLua, "input: invalid argument");
        }
        if (strstr((const char *)pszArg, "*n")) {
            iNum = 1;
        }
    }

    wxString strT = uT("");

    wxCommandEvent eventT(wxEVT_COMMAND_TEXT_UPDATED, ID_THREAD_READ);
    if (iSigmaCaller == SIGMACALLER_EDITOR) {
        strT = pScriptThread->getLastPrint();
        eventT.SetString(strT.EndsWith(uT(":")) ? strT : uT(""));
    }
    else if (iSigmaCaller == SIGMACALLER_CONSOLE) {
        strT = pConsoleThread->getLastPrint();
        eventT.SetString(strT.EndsWith(uT(":")) ? strT : uT(""));
    }

    eventT.SetInt(iNum);

    if (iSigmaCaller == SIGMACALLER_EDITOR) {
        pScriptEdit->GetEventHandler()->AddPendingEvent(eventT);
    }
    else if (iSigmaCaller == SIGMACALLER_CONSOLE) {
        pConsoleEdit->GetEventHandler()->AddPendingEvent(eventT);
    }

    // Pause 50 ms to let text entry dialog to be shown
    Tsleep(50);

    strT = uT("");
    if (iSigmaCaller == SIGMACALLER_EDITOR) {
        strT = pScriptThread->readStr();
    }
    else if (iSigmaCaller == SIGMACALLER_CONSOLE) {
        strT = pConsoleThread->readStr();
    }

    // Convert from Unicode to ANSI
    strcpy(szRet, LM_U8STR(strT));

    if (iNum == 1) {
        lua_pushnumber(pLua, strtod((const char *)szRet, NULL));
    }
    else {
        lua_pushstring(pLua, (const char *)szRet);
    }

    return 1;
}

int luaClear(lua_State *pLua)
{
    if (CometApp::COMETCONSOLE) {
        return consoleDoClear(pLua);
    }

    lua_pushliteral(pLua, "___SigmaCaller___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    int iSigmaCaller = (int)lua_tointeger(pLua, -1);
    if ((iSigmaCaller != SIGMACALLER_EDITOR) && (iSigmaCaller != SIGMACALLER_CONSOLE)) {
        return luaL_errorlevel(pLua, "cls: internal error"); // Should never happen
    }

    lua_pushliteral(pLua, "___ScriptEdit___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    ScriptEdit *pScriptEdit = (iSigmaCaller == SIGMACALLER_EDITOR) ? (ScriptEdit *)lua_touserdata(pLua, -1) : NULL;
    ConsoleEdit *pConsoleEdit = (iSigmaCaller == SIGMACALLER_CONSOLE) ? (ConsoleEdit *)lua_touserdata(pLua, -1) : NULL;

    wxCommandEvent eventT(wxEVT_COMMAND_TEXT_UPDATED, ID_THREAD_CLEAR);
    if (iSigmaCaller == SIGMACALLER_EDITOR) {
        pScriptEdit->GetEventHandler()->AddPendingEvent(eventT);
    }
    else if (iSigmaCaller == SIGMACALLER_CONSOLE) {
        pConsoleEdit->GetEventHandler()->AddPendingEvent(eventT);
    }

    return 0;
}

int luaPrint(lua_State *pLua)
{
    return doWrite(pLua, LUA_FUNC_PRINT);
}

int wx_message(lua_State *pLua, bool bAsk)
{
    if (CometApp::COMETCONSOLE) {
        return luaL_errorlevel(pLua, "gui: not available in console mode");
    }

    int nArgCount = lua_gettop(pLua);
    if (nArgCount != 1) {
        return luaL_errorlevel(pLua, "gui: invalid number of argument (should be 1)");
    }

    const char *pszArg = (const char *)luaX_tolstring(pLua, 1, NULL);
    if (pszArg == NULL) {
        return luaL_errorlevel(pLua, "gui: invalid argument");
    }
    wxString strT = LM_U8TOWC(pszArg);

    lua_pushliteral(pLua, "___SigmaCaller___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    int iSigmaCaller = (int)lua_tointeger(pLua, -1);
    if ((iSigmaCaller != SIGMACALLER_EDITOR) && (iSigmaCaller != SIGMACALLER_CONSOLE)) {
        return luaL_errorlevel(pLua, "gui: internal error"); // Should never happen
    }

    lua_pushliteral(pLua, "___ScriptEdit___");
    lua_gettable(pLua, LUA_REGISTRYINDEX);
    ScriptEdit *pScriptEdit = (iSigmaCaller == SIGMACALLER_EDITOR) ? (ScriptEdit *)lua_touserdata(pLua, -1) : NULL;
    ConsoleEdit *pConsoleEdit = (iSigmaCaller == SIGMACALLER_CONSOLE) ? (ConsoleEdit *)lua_touserdata(pLua, -1) : NULL;

    wxCommandEvent eventT(wxEVT_COMMAND_TEXT_UPDATED, ID_THREAD_MESSAGE);
    eventT.SetString(strT);
    eventT.SetInt(bAsk ? 1 : 0);
    if (iSigmaCaller == SIGMACALLER_EDITOR) {
        pScriptEdit->GetEventHandler()->AddPendingEvent(eventT);
    }
    else if (iSigmaCaller == SIGMACALLER_CONSOLE) {
        pConsoleEdit->GetEventHandler()->AddPendingEvent(eventT);
    }

    if (bAsk) {
        lua_pushliteral(pLua, "___SigmaThread___");
        lua_gettable(pLua, LUA_REGISTRYINDEX);
        ScriptThread *pScriptThread = (iSigmaCaller == SIGMACALLER_EDITOR) ? (ScriptThread *)lua_touserdata(pLua, -1) : NULL;
        ConsoleThread *pConsoleThread = (iSigmaCaller == SIGMACALLER_CONSOLE) ? (ConsoleThread *)lua_touserdata(pLua, -1) : NULL;

        int iret = -1;
        if (iSigmaCaller == SIGMACALLER_EDITOR) {
            iret = pScriptThread->getAnswer();
        }
        else if (iSigmaCaller == SIGMACALLER_CONSOLE) {
            iret = pConsoleThread->getAnswer();
        }
        // answ = gui.ask(msg,ttl)
        lua_pushinteger(pLua, iret);
        return 1;
    }
    return 0;
}

int wx_alert(lua_State *pLua)
{
    return wx_message(pLua, false);
}

int wx_ask(lua_State *pLua)
{
    return wx_message(pLua, true);
}

//
