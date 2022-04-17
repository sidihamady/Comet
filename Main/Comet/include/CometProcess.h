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


#ifndef SIGMA_PROCESS_H
#define SIGMA_PROCESS_H

#include "Identifiers.h"

#include "../../LibLua/LuaExt/include/LuaExt.h"

#include <wx/process.h>

class CometProcess : public wxProcess
{
private:
    long m_iId;
    wxString m_strCmd;
    wxString m_strDirectory;
    bool m_bRedirect;
    wxWindow *m_pEdit;
    bool m_bInStdin;
    bool m_bInGetOut;
    int m_iLexer;

public:
    CometProcess(const wxString &strCmd, const wxString strDirectory, wxWindow *pEdit = NULL, bool bRedirect = false, int iLexer = wxSTC_LEX_NULL)
    {
        m_iId = -1L;
        m_strCmd = wxString(LM_CSTR(strCmd));
        m_strDirectory = wxString(LM_CSTR(strDirectory));
        m_bRedirect = bRedirect;
        m_pEdit = pEdit;
        m_bInStdin = false;
        m_bInGetOut = false;
        m_iLexer = iLexer;
    }

    ~CometProcess()
    {
        m_pEdit = NULL;
        if (m_iId != -1L) {
            wxProcess::Kill(m_iId, wxSIGKILL, wxKILL_CHILDREN);
            m_iId = -1L;
        }
    }

    int getLexer(void)
    {
        return m_iLexer;
    }
    int setLexer(int iLexer)
    {
        m_iLexer = iLexer;
        return m_iLexer;
    }

    long execute(void);
    bool getOutput(wxString &strStdout, wxString &strStderr, int iExitStatus, bool bEnded = false);
    void sendOutput(const wxString &strT);
    void kill(void);

    virtual void OnTerminate(int pid, int istatus);
};

#endif // SIGMA_PROCESS_H
