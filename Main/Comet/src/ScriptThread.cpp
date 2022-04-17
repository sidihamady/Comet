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


#include "Identifiers.h"

#include "CometApp.h"
#include "ScriptThread.h"
#include "ScriptEdit.h"
#include "CometFrame.h"

#include <wx/file.h>     // raw file io support
#include <wx/filename.h> // filename support

extern void luaHook(lua_State *pLua, lua_Debug *pDebug); // Hook to force Lua to stop the current script

// IO
extern int luaClear(lua_State *pLua);
extern int luaRead(lua_State *pLua);
extern int luaWrite(lua_State *pLua);
extern int luaPrint(lua_State *pLua);
//

extern int wx_alert(lua_State *pLua);
extern int wx_ask(lua_State *pLua);

static const luaL_Reg CFUNCTION_IO[] = {
    // IO
    { "clear", luaClear },
    { "cls", luaClear },
    { "io_read", luaRead },
    { "io_write", luaWrite },
    { "print", luaPrint },
    { "wx_alert", wx_alert },
    { "wx_ask", wx_ask },
    { NULL, NULL }
};

wxThreadError ScriptThread::Create(void *pEdit, char *pszBufferA, size_t iBufferSize, int iLineCount)
{
    // Should never happen, since Create in called once, after constructor
    if (m_pLuaEngine) {
        delete m_pLuaEngine;
        m_pLuaEngine = NULL;
    }
    if (m_pszBufferA) {
        free(m_pszBufferA);
        m_pszBufferA = NULL;
    }
    if (m_pszBufferStack) {
        free(m_pszBufferStack);
        m_pszBufferStack = NULL;
    }
    //

    m_pLuaEngine = new (std::nothrow) LuaEngine(LUA_ENGINE_GUI, CFUNCTION_IO, true);
    if (m_pLuaEngine == NULL) {
        return wxTHREAD_NO_RESOURCE;
    }
    m_pszBufferA = (char *)malloc((iBufferSize + 1) * sizeof(char));
    if (m_pszBufferA == NULL) {
        delete m_pLuaEngine;
        m_pLuaEngine = NULL;
        return wxTHREAD_NO_RESOURCE;
    }
    memcpy(m_pszBufferA, pszBufferA, iBufferSize * sizeof(char));
    m_pszBufferA[iBufferSize] = '\0';

    m_pszBufferStack = (char *)malloc(iLineCount * LM_STRSIZE * sizeof(char));
    if (m_pszBufferStack == NULL) {
        delete m_pLuaEngine;
        m_pLuaEngine = NULL;
        free(m_pszBufferA);
        m_pszBufferA = NULL;
        return wxTHREAD_NO_RESOURCE;
    }
    memset(m_pszBufferStack, 0, iLineCount * LM_STRSIZE * sizeof(char));

    m_pEdit = pEdit;
    m_pMainFrame = NULL;

    m_iLineCount = iLineCount;

    lua_State *pLuaState = m_pLuaEngine->getLuaState();

    lua_pushliteral(pLuaState, "___SigmaCaller___");
    lua_pushinteger(pLuaState, SIGMACALLER_EDITOR);
    lua_settable(pLuaState, LUA_REGISTRYINDEX);

    lua_pushliteral(pLuaState, "___ScriptEdit___");
    lua_pushlightuserdata(pLuaState, (void *)m_pEdit);
    lua_settable(pLuaState, LUA_REGISTRYINDEX);

    lua_pushliteral(pLuaState, "___SigmaGuierr___");
    lua_pushinteger(pLuaState, 0);
    lua_settable(pLuaState, LUA_REGISTRYINDEX);

    lua_pushliteral(pLuaState, "___SigmaThread___");
    lua_pushlightuserdata(pLuaState, (void *)this);
    lua_settable(pLuaState, LUA_REGISTRYINDEX);

    // Possibility to debug the running script
    if (debugIt()) {
        lua_sethook(pLuaState, &luaHook, LUA_MASKLINE | LUA_MASKCALL | LUA_MASKRET, 0);
        m_pLuaEngine->setDebugging(true);
    }

    ScriptEdit *pScriptEdit = (ScriptEdit *)m_pEdit;
    if (pScriptEdit->m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        pScriptEdit->m_pLuaEngine = m_pLuaEngine;
        pScriptEdit->m_pMutex->Unlock();
    }

    return wxThread::Create();
}

wxThread::ExitCode ScriptThread::Entry()
{
    if ((m_pEdit == NULL) || (m_pszBufferA == NULL)) {
        return 0;
    }
    ScriptEdit *pEdit = (ScriptEdit *)m_pEdit;

    if (m_pLuaEngine == NULL) {
        wxCommandEvent eventT(wxEVT_COMMAND_TEXT_UPDATED, ID_THREAD_PRINT);

        eventT.SetString(uT("! Lua engine not loaded: insufficient memory"));
        pEdit->GetEventHandler()->AddPendingEvent(eventT);
        return 0;
    }

    bool bRet = m_pLuaEngine->runScriptString((const char *)m_pszBufferA, LUA_ENGINE_RUN);

    m_pMainFrame = (void *)(wxGetApp().getMainFrame());

    if ((m_pMainFrame) && (bRet == false)) {
        wxString strT = m_pLuaEngine->getMessage();
        int iErrLine = m_pLuaEngine->getErrLine();
        if (strT.Contains(STOPPED_BY_USER)) {
            iErrLine = -1;
        }
        wxCommandEvent eventT(wxEVT_COMMAND_TEXT_UPDATED, ID_THREAD_PRINTERR);
        eventT.SetInt(iErrLine);
        eventT.SetString(strT);
        pEdit->GetEventHandler()->AddPendingEvent(eventT);
    }

    return 0;
}

void ScriptThread::highlightLine(int iLine)
{
    if ((iLine >= 0) && (iLine < m_iLineCount)) {

        ScriptEdit *pEdit = (ScriptEdit *)m_pEdit;

        wxCommandEvent eventT(wxEVT_COMMAND_TEXT_UPDATED, ID_THREAD_BREAKPOINT);

        eventT.SetInt(iLine);
        eventT.SetString(m_bFirstCall ? uT("1") : uT("0"));
        pEdit->GetEventHandler()->AddPendingEvent(eventT);

        m_bFirstCall = false;
    }
}

bool ScriptThread::breakpointIn(int iLine)
{
    ScriptEdit *pEdit = (ScriptEdit *)m_pEdit;

    bool bBreakpoint = false;

    if (pEdit->m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        bBreakpoint = pEdit->breakpointIn(iLine);
        pEdit->m_pMutex->Unlock();
    }

    return bBreakpoint;
}

bool ScriptThread::stepInto(void)
{
    ScriptEdit *pEdit = (ScriptEdit *)m_pEdit;

    bool bStepInto = false;

    if (pEdit->m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        bStepInto = pEdit->stepInto();
        pEdit->m_pMutex->Unlock();
    }

    return bStepInto;
}

bool ScriptThread::debugIt(void)
{
    ScriptEdit *pEdit = (ScriptEdit *)m_pEdit;

    bool bDebug = false;
    bool bDebugging = false;

    if (pEdit->m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        bDebug = pEdit->m_bDebugIt;
        bDebugging = pEdit->m_bDebugging;
        pEdit->m_pMutex->Unlock();
    }

    if (bDebugging == false) {
        lua_sethook(m_pLuaEngine->getLuaState(), &luaHook, 0, 0);
        m_pLuaEngine->setDebugging(false);
    }

    return bDebug;
}

bool ScriptThread::breakIt(lua_Debug *pDebug)
{
    ScriptEdit *pEdit = (ScriptEdit *)m_pEdit;

    bool bDebug = false;

    if (m_pLuaEngine->debugStackTrace(m_pszBufferStack, (m_iLineCount * LM_STRSIZE) - 1, pDebug)) {
        wxCommandEvent eventT(wxEVT_COMMAND_TEXT_UPDATED, ID_THREAD_STACK);

        eventT.SetInt(pDebug->currentline);
        wxString strStack = LM_U8TOWC(m_pszBufferStack);
        eventT.SetString(strStack);
        pEdit->GetEventHandler()->AddPendingEvent(eventT);
    }

    if (pEdit->m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        pEdit->m_bDebugIt = false;
        pEdit->m_pMutex->Unlock();
    }

    return bDebug;
}

bool ScriptThread::stopIt(void)
{
    ScriptEdit *pEdit = (ScriptEdit *)m_pEdit;

    bool bStop = false;

    if (pEdit->m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        bStop = pEdit->m_bStopIt;
        pEdit->m_pMutex->Unlock();
    }

    return bStop;
}

wxString ScriptThread::readStr(void)
{
    ScriptEdit *pEdit = (ScriptEdit *)m_pEdit;

    bool bReadDone = false;

    if (pEdit->m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        bReadDone = pEdit->m_bReadDone;
        pEdit->m_pMutex->Unlock();
    }

    // Wait until the end of the Read function
    if (bReadDone == false) {
        while (true) {
            // Pause 50 ms and try again
            Tsleep(50);

            if (pEdit->m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
                bReadDone = pEdit->m_bReadDone;
                pEdit->m_pMutex->Unlock();
            }

            if (bReadDone) {
                break;
            }
        }
    }

    wxString strT = uT("");

    if (pEdit->m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        strT = pEdit->m_strRead;
        pEdit->m_bReadDone = false;
        pEdit->m_pMutex->Unlock();
    }

    return strT;
}

int ScriptThread::getAnswer(void)
{
    ScriptEdit *pEdit = (ScriptEdit *)m_pEdit;

    bool bAskDone = false;

    if (pEdit->m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        bAskDone = pEdit->m_bAskDone;
        pEdit->m_pMutex->Unlock();
    }

    // Wait until the end of the Ask function
    if (bAskDone == false) {
        while (true) {
            // Pause 50 ms and try again
            Tsleep(50);

            if (pEdit->m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
                bAskDone = pEdit->m_bAskDone;
                pEdit->m_pMutex->Unlock();
            }

            if (bAskDone) {
                break;
            }
        }
    }

    int iAnswer = -1;

    if (pEdit->m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        iAnswer = pEdit->m_iAnswer;
        pEdit->m_pMutex->Unlock();
    }

    return iAnswer;
}

void ScriptThread::OnExit()
{
    if (m_pLuaEngine) {
        delete m_pLuaEngine;
        m_pLuaEngine = NULL;
    }
    if (m_pszBufferA) {
        free(m_pszBufferA);
        m_pszBufferA = NULL;
    }
    if (m_pszBufferStack) {
        free(m_pszBufferStack);
        m_pszBufferStack = NULL;
    }

    m_pMainFrame = (void *)(wxGetApp().getMainFrame());

    if (m_pMainFrame) {
        ScriptEdit *pEdit = (ScriptEdit *)m_pEdit;
        wxCommandEvent eventT(wxEVT_COMMAND_TEXT_UPDATED, ID_THREAD_FINISH);
        pEdit->GetEventHandler()->AddPendingEvent(eventT);
    }
}
