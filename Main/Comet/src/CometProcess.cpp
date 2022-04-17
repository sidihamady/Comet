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
#include "CometFrame.h"
#include "ScriptEdit.h"
#include "CometProcess.h"

#if defined(__WXGTK__)
#include <sys/stat.h>
#endif

#include <wx/txtstrm.h>

void CometProcess::OnTerminate(int pid, int istatus)
{
    if (m_pEdit != NULL) {
        ScriptEdit *pEdit = (ScriptEdit *)m_pEdit;
        if (m_bRedirect) {
            CloseOutput();
            wxString strStdout, strStderr;
            getOutput(strStdout, strStderr, istatus, true);
        }
        pEdit->processOnTerminate(pid, istatus);
        m_pEdit = NULL;
    }

    m_iId = -1L;

    // as OnTerminate event is processed, CometProcess should delete itself
    delete this;
}

void CometProcess::kill(void)
{
    m_bRedirect = false;
    m_pEdit = NULL;

    if (m_iId != -1L) {
        wxProcess::Kill(m_iId, wxSIGKILL, wxKILL_CHILDREN);
        m_iId = -1L;
    }
}

long CometProcess::execute(void)
{
    m_iId = -1L;

    if (m_strCmd.IsEmpty()) {
        return -1L;
    }

    if (m_bRedirect && (m_pEdit != NULL)) {
        if (m_strDirectory.Length() > 2) {
            // ::ANSI::    NOT::UNICODE::
            char szT[LM_STRSIZE];
            memset(szT, 0, LM_STRSIZE * sizeof(char));
            strncpy(szT, LM_U8STR(m_strDirectory), LM_STRSIZE - 1);
            chdir(szT);
            //
        }
#if defined(__WXMSW__)
        wxString strCmdT = uT("cmd /C \"");
        strCmdT += m_strCmd;
        strCmdT += uT("\"");
#else
        wxString strCmdT = m_strCmd;
#endif
        this->Redirect();
        m_iId = ::wxExecute(strCmdT, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER, this);
        // Returns always -1L on failure (::wxExecute returns -1L or 0L on failure)
        if (m_iId <= 0L) {
            m_iId = -1L;
        }
        return m_iId;
    }

#if defined(__WXMSW__)

    wxString strCmdT = uT("cmd /C \"");
    strCmdT += m_strCmd;
    strCmdT += uT("\" & echo. & pause");

#else

    if (::wxFileExists(static_cast<const char_t *>(CometFrame::TOOL_CONF.szTerm)) == false) {
        bool bRet = false;
        const char_t *arTerm[] = {
            uT("/usr/bin/x-terminal-emulator"),
            uT("/usr/bin/urxvt"),
            uT("/usr/bin/rxvt"),
            uT("/usr/bin/termit"),
            uT("/usr/bin/terminator"),
            uT("/usr/bin/eterm"),
            uT("/usr/bin/uxterm"),
            uT("/usr/bin/xterm"),
            uT("/usr/bin/gnome-terminal"),
            uT("/usr/bin/roxterm"),
            uT("/usr/bin/xfce4-terminal"),
            uT("/usr/bin/termite"),
            uT("/usr/bin/lxterminal"),
            uT("/usr/bin/mate-terminal"),
            uT("/usr/bin/terminology"),
            uT("/usr/bin/st"),
            uT("/usr/bin/qterminal"),
            uT("/usr/bin/lilyterm"),
            uT("/usr/bin/tilix"),
            uT("/usr/bin/terminix"),
            uT("/usr/bin/konsole"),
            uT("/usr/bin/kitty"),
            uT("/usr/bin/guake"),
            uT("/usr/bin/tilda"),
            uT("/usr/bin/alacritty"),
            uT("/usr/bin/hyper"),
            NULL
        };
        for (int ii = 0; arTerm[ii] != NULL; ii++) {
            if (::wxFileExists(arTerm[ii])) {
                Tstrcpy(CometFrame::TOOL_CONF.szTerm, arTerm[ii]);
                Tstrcpy(CometFrame::TOOL_CONF.szOpt, uT("-T;--working-directory=;-e;-c"));
                bRet = true;
                break;
            }
        }
        if (false == bRet) {
            SigmaMessageBox(uT("Terminal not found: check in Options/Tools menu"), uT("Comet"), wxICON_WARNING | wxOK, NULL);
            return -1L;
        }
    }

    wxString strCmdT = static_cast<const char_t *>(CometFrame::TOOL_CONF.szTerm);
    wxString strOpt = static_cast<const char_t *>(CometFrame::TOOL_CONF.szOpt);
    wxString strShell = static_cast<const char_t *>(CometFrame::TOOL_CONF.szShell);

    wxArrayString arrT(false);
    int ii = 0;
    wxString strX = wxEmptyString;
    wxStringTokenizer tokenizerT(strOpt, uT(";"));
    while (tokenizerT.HasMoreTokens() && (ii++ < 5)) {
        strX = tokenizerT.GetNextToken().Trim();
        if (false == strX.IsEmpty()) {
            arrT.Add(strX);
        }
    }
    if (arrT.size() != 4) {
        if (wxStrcmp(static_cast<const char_t *>(CometFrame::TOOL_CONF.szTerm), uT("/usr/bin/xfce4-terminal")) == 0) {
            Tstrcpy(CometFrame::TOOL_CONF.szOpt, uT("-T;--working-directory=;-x;-c"));
            arrT[0] = uT("-T");
            arrT[1] = uT("--working-directory=");
            arrT[2] = uT("-x");
            arrT[3] = uT("-c");
        }
        else if (wxStrcmp(static_cast<const char_t *>(CometFrame::TOOL_CONF.szTerm), uT("/usr/bin/gnome-terminal")) == 0) {
            Tstrcpy(CometFrame::TOOL_CONF.szOpt, uT("-t;--working-directory=;-x;-c"));
            arrT[0] = uT("-t");
            arrT[1] = uT("--working-directory=");
            arrT[2] = uT("-x");
            arrT[3] = uT("-c");
        }
        else if (wxStrcmp(static_cast<const char_t *>(CometFrame::TOOL_CONF.szTerm), uT("/usr/bin/konsole")) == 0) {
            Tstrcpy(CometFrame::TOOL_CONF.szOpt, uT(" ;--working-directory=;-e; ;-c"));
            arrT[0] = uT("");
            arrT[1] = uT("--working-directory=");
            arrT[2] = uT("-e");
            arrT[3] = uT("-c");
        }
        else if (wxStrcmp(static_cast<const char_t *>(CometFrame::TOOL_CONF.szTerm), uT("/usr/bin/uxterm")) == 0) {
            Tstrcpy(CometFrame::TOOL_CONF.szOpt, uT("-T;--working-directory=;-e;-c"));
            arrT[0] = uT("-T");
            arrT[1] = uT("--working-directory=");
            arrT[2] = uT("-e");
            arrT[3] = uT("-c");
        }
        else if (wxStrcmp(static_cast<const char_t *>(CometFrame::TOOL_CONF.szTerm), uT("/usr/bin/xterm")) == 0) {
            Tstrcpy(CometFrame::TOOL_CONF.szOpt, uT("-T;--working-directory=;-e;-c"));
            arrT[0] = uT("-T");
            arrT[1] = uT("--working-directory=");
            arrT[2] = uT("-e");
            arrT[3] = uT("-c");
        }
        else if (wxStrcmp(static_cast<const char_t *>(CometFrame::TOOL_CONF.szTerm), uT("/usr/bin/mate-terminal")) == 0) {
            Tstrcpy(CometFrame::TOOL_CONF.szOpt, uT("-T;--working-directory=;--command;-c"));
            arrT[0] = uT("-T");
            arrT[1] = uT("--working-directory=");
            arrT[2] = uT("--command");
            arrT[3] = uT("-c");
        }
        else if (wxStrcmp(static_cast<const char_t *>(CometFrame::TOOL_CONF.szTerm), uT("/usr/bin/x-terminal-emulator")) == 0) {
            Tstrcpy(CometFrame::TOOL_CONF.szOpt, uT("-T;--working-directory=;--command;-c"));
            arrT[0] = uT("-T");
            arrT[1] = uT("--working-directory=");
            arrT[2] = uT("--command");
            arrT[3] = uT("-c");
        }
        else if (wxStrcmp(static_cast<const char_t *>(CometFrame::TOOL_CONF.szTerm), uT("/usr/bin/urxvt")) == 0) {
            Tstrcpy(CometFrame::TOOL_CONF.szOpt, uT("-title;--working-directory=;-e;-c"));
            arrT[0] = uT("-title");
            arrT[1] = uT("--working-directory=");
            arrT[2] = uT("-e");
            arrT[3] = uT("-c");
        }
        else if (wxStrcmp(static_cast<const char_t *>(CometFrame::TOOL_CONF.szTerm), uT("/usr/bin/rxvt")) == 0) {
            Tstrcpy(CometFrame::TOOL_CONF.szOpt, uT("-title;--working-directory=;-e;-c"));
            arrT[0] = uT("-title");
            arrT[1] = uT("--working-directory=");
            arrT[2] = uT("-e");
            arrT[3] = uT("-c");
        }
        else if (wxStrcmp(static_cast<const char_t *>(CometFrame::TOOL_CONF.szTerm), uT("/usr/bin/roxterm")) == 0) {
            Tstrcpy(CometFrame::TOOL_CONF.szOpt, uT("-title;--working-directory=;-e;-c"));
            arrT[0] = uT("-title");
            arrT[1] = uT("--working-directory=");
            arrT[2] = uT("-e");
            arrT[3] = uT("-c");
        }
        else if (wxStrcmp(static_cast<const char_t *>(CometFrame::TOOL_CONF.szTerm), uT("/usr/bin/lxterminal")) == 0) {
            Tstrcpy(CometFrame::TOOL_CONF.szOpt, uT("-title;--working-directory=;-e;-c"));
            arrT[0] = uT("-title");
            arrT[1] = uT("--working-directory=");
            arrT[2] = uT("-e");
            arrT[3] = uT("-c");
        }
        else {
            SigmaMessageBox(uT("Terminal options not valid: check in Options/Tools menu"), uT("Comet"), wxICON_WARNING | wxOK, NULL);
            return -1L;
        }
    }

    strCmdT += uT(" ");
    if (arrT[0].IsEmpty() == false) {
        strCmdT += arrT[0]; // -T
        strCmdT += uT(" \"Comet: ");
        strCmdT += m_strCmd;
        strCmdT += uT("\" ");
    }
    strCmdT += arrT[1] + m_strDirectory; // --working-directory=
    strCmdT += uT(" ");
    strCmdT += arrT[2]; // -x
    strCmdT += uT(" ");
    strCmdT += strShell;
    strCmdT += uT(" ");
    strCmdT += arrT[3]; // -c
    strCmdT += uT(" \"");
    strCmdT += LM_CSTR(m_strCmd);
    strCmdT += uT(" && read -p 'Press any key to continue...'");
    strCmdT += uT("\"");

#endif

    if (m_strDirectory.IsEmpty() == false) {
        // ::ANSI::    NOT::UNICODE::
        char szT[LM_STRSIZE];
        memset(szT, 0, LM_STRSIZE * sizeof(char));
        strncpy(szT, LM_U8STR(m_strDirectory), LM_STRSIZE - 1);
        chdir(szT);
    }

    m_iId = ::wxExecute(strCmdT, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER | wxEXEC_NOHIDE, this);
    if (m_iId <= 0L) {
        SigmaMessageBox(uT("Tool not run: check in Options/Tools menu"), uT("Comet"), wxICON_WARNING | wxOK, NULL);
        m_iId = -1L;
    }
    return m_iId;
}

bool CometProcess::getOutput(wxString &strStdout, wxString &strStderr, int iExitStatus, bool bEnded /* = false*/)
{
    if (m_bInStdin || m_bInGetOut) {
        return false;
    }

    if ((m_bRedirect == false) || (m_pEdit == NULL) || (wxProcess::Exists(m_iId) == false)) {
        return false;
    }

    bool bRet = false;
    int ii = 0;
    bool bHuge = false;

    m_bInGetOut = true;

    if ((false == bEnded) && (GetOutputStream() != NULL) && (GetOutputStream()->IsOk())) {
        // disable stdin
        m_bInStdin = true;
        wxTextOutputStream tStreamStdin(*GetOutputStream());
        tStreamStdin << uT("\n");
        CloseOutput();
        m_bInStdin = false;
    }

    wxInputStream *pStream = GetInputStream();
    if ((pStream != NULL) && (pStream->IsOk())) {
        // do not use wxTextInputStream: sometimes, it hangs with no-ascii character
        ii = 0;
        wxChar cT = 0;
        while (IsInputAvailable() && (++ii < LM_STRSIZEL)) {
            cT = (wxChar)(pStream->GetC());
            if ((0 == cT) || (wxEOF == cT)) {
                break;
            }
            strStdout << cT;
            bRet = true;
        }
        bHuge = (ii >= LM_STRSIZEL);
    }

    pStream = GetErrorStream();
    if ((pStream != NULL) && (pStream->IsOk())) {
        // do not use wxTextInputStream: sometimes, it hangs with no-ascii character
        ii = 0;
        wxChar cT = 0;
        while (IsErrorAvailable() && (++ii < LM_STRSIZEL)) {
            cT = (wxChar)(pStream->GetC());
            if ((0 == cT) || (wxEOF == cT)) {
                break;
            }
            strStdout << cT;
            bRet = true;
        }
        bHuge = (ii >= LM_STRSIZEL);
    }

    if (bHuge) {
        strStdout += uT("\n...");
    }

    ScriptEdit *pEdit = (ScriptEdit *)m_pEdit;
    pEdit->processOutput(strStdout, iExitStatus);
    m_bInGetOut = false;
    return bRet;
}
