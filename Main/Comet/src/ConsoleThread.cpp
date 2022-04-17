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
#include "ConsoleThread.h"
#include "ConsoleEdit.h"
#include "CometFrame.h"

#include <wx/file.h>     // raw file io support
#include <wx/filename.h> // filename support

wxThreadError ConsoleThread::Create(LuaEngine *pLuaEngine, void *pEdit, char *pszBufferA, size_t iBufferSize)
{
    if (pLuaEngine == NULL) {
        return wxTHREAD_NO_RESOURCE;
    }
    m_pLuaEngine = pLuaEngine;

    m_pszBufferA = (char *)malloc((iBufferSize + 1) * sizeof(char));
    if (m_pszBufferA == NULL) {
        m_pLuaEngine = NULL;
        return wxTHREAD_NO_RESOURCE;
    }
    memcpy(m_pszBufferA, pszBufferA, iBufferSize * sizeof(char));
    m_pszBufferA[iBufferSize] = '\0';

    m_pEdit = pEdit;
    m_pMainFrame = NULL;

    lua_State *pLuaState = m_pLuaEngine->getLuaState();

    lua_pushliteral(pLuaState, "___SigmaThread___");
    lua_pushlightuserdata(pLuaState, (void *)this);
    lua_settable(pLuaState, LUA_REGISTRYINDEX);

    return wxThread::Create();
}

wxThread::ExitCode ConsoleThread::Entry()
{
    if ((m_pEdit == NULL) || (m_pszBufferA == NULL)) {
        return 0;
    }
    ConsoleEdit *pEdit = static_cast<ConsoleEdit *>(m_pEdit);
    m_pMainFrame = (void *)(wxGetApp().getMainFrame());

    if (m_pLuaEngine == NULL) {
        wxCommandEvent eventT(wxEVT_COMMAND_TEXT_UPDATED, ID_THREAD_PRINT);

        eventT.SetString(uT("! Lua engine not loaded: insufficient memory"));
        pEdit->GetEventHandler()->AddPendingEvent(eventT);
        return 0;
    }

    bool bRet = m_pLuaEngine->runScriptString((const char *)m_pszBufferA, LUA_ENGINE_RUN);

    if (bRet == false) {
        wxString strT = m_pLuaEngine->getMessage();
        wxCommandEvent eventT(wxEVT_COMMAND_TEXT_UPDATED, ID_THREAD_PRINTERR);

        eventT.SetString(strT);
        pEdit->GetEventHandler()->AddPendingEvent(eventT);
    }

    return 0;
}

wxString ConsoleThread::readStr(void)
{
    ConsoleEdit *pEdit = static_cast<ConsoleEdit *>(m_pEdit);

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
        pEdit->m_pMutex->Unlock();
    }

    return strT;
}

int ConsoleThread::getAnswer(void)
{
    ConsoleEdit *pEdit = static_cast<ConsoleEdit *>(m_pEdit);

    bool bAskDone = false;

    if (pEdit->m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        bAskDone = pEdit->m_bAskDone;
        pEdit->m_pMutex->Unlock();
    }

    // Wait until the end of the Read function
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

void ConsoleThread::OnExit()
{
    if (m_pLuaEngine) {
        m_pLuaEngine = NULL; // LuaEngine owned by the caller. Do not delete it!
    }
    if (m_pszBufferA) {
        free(m_pszBufferA);
        m_pszBufferA = NULL;
    }

    ConsoleEdit *pEdit = static_cast<ConsoleEdit *>(m_pEdit);

    wxCommandEvent eventT(wxEVT_COMMAND_TEXT_UPDATED, ID_THREAD_FINISH);

    pEdit->GetEventHandler()->AddPendingEvent(eventT);
}
