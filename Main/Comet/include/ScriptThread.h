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


#ifndef SCRIPT_THREAD_H
#define SCRIPT_THREAD_H

#include "../../LibLua/LuaExt/include/LuaExt.h"

#include <wx/wx.h>
#include <wx/thread.h>

#define STOPPED_BY_USER  uT("Script stopped by the user")
#define STOPPED_BY_USERA "Script stopped by the user"

class ScriptThread : public wxThread
{
private:
    void *m_pEdit;
    void *m_pMainFrame;
    LuaEngine *m_pLuaEngine;

    int m_iErrLine;

    bool m_bFirstCall;

    char *m_pszBufferA;
    char *m_pszBufferStack;
    int m_iLineCount;

    wxString m_strLastPrint;

    char_t m_szUserDir[LM_STRSIZE];

public:
    ScriptThread() : wxThread(wxTHREAD_DETACHED)
    {
        m_pMainFrame = NULL;
        m_pEdit = NULL;
        m_pLuaEngine = NULL;
        m_pszBufferA = NULL;
        m_pszBufferStack = NULL;
        m_iLineCount = 0;

        m_bFirstCall = true;

        m_iErrLine = -1;

        m_strLastPrint = uT("");

        Tmemset(m_szUserDir, 0, LM_STRSIZE);
    }

    wxThreadError Create(void *pEdit, char *pszBufferA, size_t iBufferSize, int iLineCount);

    LuaEngine *getLuaEngine(void)
    {
        return m_pLuaEngine;
    }

    int getErrLine(void)
    {
        return m_iErrLine;
    }

    const wxString &getLastPrint(void)
    {
        return (const wxString &)m_strLastPrint;
    }

    void setLastPrint(const wxString &strT)
    {
        int iLen = (int)(strT.Length());
        if (iLen > 1) {
            int ii = iLen - 1;
            for (ii = iLen - 1; ii >= 0; ii--) {
                if ((strT[ii] != uT('\t')) && (strT[ii] != uT('\r')) && (strT[ii] != uT('\n')) && (strT[ii] != uT(' '))) {
                    break;
                }
            }
            m_strLastPrint = strT.Mid(0, ii + 1);
        }
        else {
            m_strLastPrint = strT;
        }
    }

    bool breakpointIn(int iLine);
    void highlightLine(int iLine);
    bool stepInto(void);
    bool debugIt(void);
    bool breakIt(lua_Debug *pDebug);
    bool stopIt(void);
    wxString readStr(void);
    int getAnswer(void);

protected:
    virtual ExitCode Entry();
    virtual void OnExit();
};

#endif
