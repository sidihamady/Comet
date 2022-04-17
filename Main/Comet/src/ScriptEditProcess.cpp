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
#include "ScriptThread.h"

bool ScriptEdit::processStart(void)
{
    m_bCanSetFocus = true;

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame == NULL) {
        // should never happen
        return false;
    }

    pFrame->setRunningLexer(GetLexer());
    pFrame->setErrId(getScriptId());

    processKill();

    // Remove previous error and debugging markers
    MarkerDeleteAll(SCRIPT_MASK_ERRORBIT);
    MarkerDeleteAll(SCRIPT_MASK_DEBUGBIT);
    //

    wxFileName fname = this->GetFilename();
    wxString strExt = fname.GetExt();
    wxString strShortFilename = fname.GetName();
    wxString strDirectory = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    if (::wxDirExists(strDirectory) == false) {
        strDirectory = wxEmptyString;
    }
    bool bRedirect = false;
    bool bSilent = false;
    bool bCls = false;
    wxString strCmd;
    if ((wxSTC_LEX_BATCH == m_iLexer) || (wxSTC_LEX_BASH == m_iLexer)) {
        bRedirect = true;
        bCls = true;
        strCmd = this->GetFilename();
    }
    else {
        strCmd = CometFrame::getToolCmd(m_iLexer, strExt, strShortFilename, &bRedirect, &bSilent, &bCls);
    }
    if (strCmd.IsEmpty()) {
        return false;
    }
    if (strCmd.StartsWith(uT("call ")) && bRedirect) {
        // In Windows, command starting with 'call ' (like 'call build.bat') is only suitable for cmd.exe launched tool...
        // ...for redirected mode, start directly batch
        strCmd = strCmd.Mid(5);
    }
    wxString strTT = wxString(uT("%s."));
    strTT += strExt;
    if (strCmd.Find(strTT) >= 0) {
        strCmd.Replace(uT("%s"), LM_CSTR(strShortFilename));
    }
    else {
        strTT = strShortFilename;
        strTT += uT(".");
        strTT += strExt;
        strCmd.Replace(uT("%s"), LM_CSTR(strTT));
    }

    pFrame->ClearOutputWindow(GetFilename(), bCls);

    try {
        // delete itself, in ::wxExecute (if explicitely killed) or in CometProcess::OnTerminate if normally terminated
        m_pProcess = new CometProcess(strCmd, strDirectory, this, bRedirect, this->GetLexer());
    }
    catch (...) {
        m_pProcess = NULL;
        m_iProcessId = -1L;

        wxString strT = uT("Cannot run the external tool: insufficient memory");
        pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        SigmaMessageBox(strT, uT("Comet"), wxOK | wxICON_EXCLAMATION, this);

        return false;
    }

    m_iProcessId = m_pProcess->execute();
    if (m_iProcessId != -1L) {
        if (CometFrame::TOOL_CONF.bLog) {
            CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
            if (pFrame != NULL) {
                char_t cT = strCmd.GetChar(0);
                int iT = (int)(strCmd.find_last_of(wxFILE_SEP_PATH));
                wxString strDisp = wxEmptyString;
                if (iT > 0) {
                    strDisp = (cT == uT('"')) ? uT("\"") : wxEmptyString;
                    strDisp += strCmd.Mid(iT + 1);
                }
                else {
                    strDisp += strCmd;
                }
                strDisp += uT("\n");
                pFrame->Output(strDisp);
            }
        }

        if (bRedirect) {
            try {
                m_pProcessTimer = new wxTimer(this, TIMER_ID_SCRIPTEDIT);
            }
            catch (...) {
                m_pProcessTimer = NULL;
            }
            if (m_pProcessTimer) {
                m_pProcessTimer->Start(100);
            }
        }
    }
    else {
        if (m_pProcessTimer != NULL) {
            m_pProcessTimer->Stop();
            delete m_pProcessTimer;
            m_pProcessTimer = NULL;
        }
        if (m_pProcess != NULL) {
            delete m_pProcess;
            m_pProcess = NULL;
        }

        wxString strT = uT("Cannot run the external tool '");
        strT += strCmd;
        strT += uT("'");
        CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
        if (pFrame != NULL) {
            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        }
        SigmaMessageBox(strT, uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
    }

    bool bRunning = (m_iProcessId != -1L);

    processSetRunning();

    if (bSilent) {
        CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
        if (pFrame != NULL) {
            wxString strX = strCmd;
            strX += uT("\nTo print out all messages, disable the silent mode : Menu/External Build Tools\n");
            pFrame->Output(strX, false, false, NULL, true);
        }
    }

    return bRunning;
}

bool ScriptEdit::processIsAlive(void)
{
    return ((m_iProcessId != -1L) && (wxProcess::Exists(m_iProcessId)));
}

void ScriptEdit::processOutput(const wxString &strT, int iExitStatus)
{
    if (m_iProcessId == -1L) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    bool bFocus = false;
    if (0 != iExitStatus) {
        wxFileName fname = this->GetFilename();
        if (fname.FileExists()) {
            wxString strLine = wxString(LM_CSTR(strT));
            strLine.Trim(true);
            strLine.Trim(false);
            wxString strFilename = fname.GetFullName();
            int iErrline = -1;
            int iLexer = this->GetLexer();

            bool bSCC = false;
            wxString strItem = wxString(LM_CSTR(strFilename));
            if (iLexer == wxSTC_LEX_CPP) {
#ifdef __WXMSW__
                if (pFrame->isToolSet(wxSTC_LEX_CPP)) {
                    bool bRedirect = false;
                    bool bSilent = false;
                    bool bCls = false;
                    wxString strExt = fname.GetExt();
                    wxString strShortFilename = fname.GetName();
                    wxString strCmd = CometFrame::getToolCmd(m_iLexer, strExt, strShortFilename, &bRedirect, &bSilent, &bCls);
                    if ((strCmd.IsEmpty() == false) && (strCmd.Contains(uT("scc.exe"))) && (strLine.Contains(uT("error:")))) {
                        strItem << uT(':');
                        bSCC = true;
                    }
                }
#endif
                if (false == bSCC) {
                    strItem << uT('(');
                }
            }
            else if (iLexer == wxSTC_LEX_PYTHON) {
                strItem << uT('"');
            }
            else if (iLexer == wxSTC_LEX_LATEX) {
                strItem << uT(':');
            }

            int iF = strLine.Find(strItem);
            if (iF >= 0) {
                int iLen = (int)(strItem.Length());
                char_t cc;
                wxString strLinenum = uT("");

                if ((iLexer == wxSTC_LEX_CPP) || (iLexer == wxSTC_LEX_LATEX)) {
                    bool bFlag = false;
                    for (int ii = iF + iLen; ii < (iF + iLen + 8); ii++) {
                        cc = strLine.GetChar(ii);
                        if ((cc < uT('0')) || (cc > uT('9'))) {
                            bFlag = ((iLexer == wxSTC_LEX_CPP) && (cc == uT(')'))) || ((iLexer == wxSTC_LEX_LATEX) && (cc == uT(':')))
#ifdef __WXMSW__
                                    || (bSCC && (cc == uT(':')))
#endif
                                ;
                            break;
                        }
                        strLinenum << cc;
                    }
                    if (bFlag && (strLinenum.Length() > 0)) {
                        long iLinenum;
                        if (strLinenum.ToLong(&iLinenum)) {
                            iErrline = (int)(iLinenum - 1);
                        }
                    }
                }

                else if (iLexer == wxSTC_LEX_PYTHON) {
                    int iFl = strLine.Find(uT("line"));
                    if (iFl >= 0) {
                        cc = strLine.GetChar(iFl + 4);
                        if (Tisspace(cc)) {
                            iFl += 1;
                        }
                        for (int ii = iFl + 4; ii < (iFl + 12); ii++) {
                            cc = strLine.GetChar(ii);
                            if ((cc < uT('0')) || (cc > uT('9'))) {
                                break;
                            }
                            strLinenum << cc;
                        }
                        if (strLinenum.Length() > 0) {
                            long iLinenum;
                            if (strLinenum.ToLong(&iLinenum)) {
                                iErrline = (int)(iLinenum - 1);
                            }
                        }
                    }
                }

                if (iErrline >= 0) {
                    DoAddStatus(iErrline, SCRIPT_MASK_ERRORBIT);
                    bFocus = true;
                }
            }
        }
    }

    pFrame->Output(strT);
    if (bFocus && m_bCanSetFocus) {
        this->SetFocus();
        // focus only one time
        m_bCanSetFocus = false;
    }
}

void ScriptEdit::OnTimer(wxTimerEvent &tEvent)
{
    if ((m_pProcessTimer == NULL) || (tEvent.GetId() != TIMER_ID_SCRIPTEDIT)) {
        tEvent.Skip();
        return;
    }

    if ((processIsAlive() == false) || (m_pProcess == NULL)) {
        m_pProcessTimer->Stop();
        delete m_pProcessTimer;
        m_pProcessTimer = NULL;
        processSetRunning();
        return;
    }

    wxString strStdout, strStderr;
    m_pProcess->getOutput(strStdout, strStderr, (m_iErrLine >= 0) ? 1 : 0);
}

void ScriptEdit::OnTimerReload(wxTimerEvent &tEvent)
{
    if (tEvent.GetId() != TIMER_ID_SCRIPTEDIT_RELOAD) {
        tEvent.Skip();
        return;
    }

    if ((false == autoReload()) || isRunning()) {
        if (autoReload()) {
            CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
            if (pFrame) {
                pFrame->updateEditorStatus(this);
            }
        }
        resetTimerReload();
        return;
    }

    DoReload(true, false);
}

void ScriptEdit::resetTimerReload(void)
{
    if (m_pReloadTimer) {
        m_pReloadTimer->Stop();
        delete m_pReloadTimer;
        m_pReloadTimer = NULL;
    }
    m_bAutoReload = false;
}

void ScriptEdit::processOnTerminate(int pid, int iExitStatus)
{
    if (m_pProcessTimer) {
        m_pProcessTimer->Stop();
        delete m_pProcessTimer;
        m_pProcessTimer = NULL;
    }

    // delete itself, in ::wxExecute (if explicitely killed) or in CometProcess::OnTerminate if normally terminated
    m_pProcess = NULL;

    m_iProcessId = -1L;

    if (CometFrame::TOOL_CONF.bLog) {
        CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
        if (pFrame != NULL) {
            wxString strX = wxString::Format(uT("\nProcess %d terminated with status code %d\n"), pid, iExitStatus);
            pFrame->Output(strX, false, false, NULL, true, true);

            // LaTeX stuff
            if (GetLexer() == wxSTC_LEX_LATEX) {
                int iErrline = pFrame->getErrLine();
                wxString strFilename = wxEmptyString;
                if (iErrline >= 0) {
                    bool bFound = false;
                    ScriptEdit *pEdit = this;
                    if ((iErrline < pEdit->GetLineCount()) && pEdit->GetLine(iErrline).Contains(pFrame->getErrInfo())) {
                        wxFileName fname = pEdit->GetFilename();
                        strFilename = fname.GetFullName();
                        bFound = true;
                    }
                    else {
                        pEdit = pFrame->getEditorByShortFilename(pFrame->getLaTeXFilename());
                        if ((NULL != pEdit) && (iErrline < pEdit->GetLineCount()) && pEdit->GetLine(iErrline).Contains(pFrame->getErrInfo())) {
                            wxFileName fname = pEdit->GetFilename();
                            strFilename = fname.GetFullName();
                            bFound = true;
                        }
                        else {
                            // search in the opened files
                            for (int ii = 0; ii < pFrame->getEditorCount(); ii++) {
                                pEdit = pFrame->getEditor(ii);
                                if (NULL == pEdit) {
                                    pEdit = this;
                                    break;
                                }
                                if ((iErrline < pEdit->GetLineCount()) && pEdit->GetLine(iErrline).Contains(pFrame->getErrInfo())) {
                                    wxFileName fname = pEdit->GetFilename();
                                    strFilename = fname.GetFullName();
                                    bFound = true;
                                    break;
                                }
                            }
                            if (false == bFound) {
                                pEdit = this;
                                // search in the last file, and only in the first 2048 lines
                                if (iErrline < 2048) {
                                    int lineCount = 0;
                                    char_t *pszLine = (char_t *)malloc(32768 * sizeof(char_t));
                                    if (pszLine) {
                                        pszLine[0] = pszLine[32767] = uT('\0');
                                        wxString strExtFile = pFrame->getLaTeXFullFilename();
                                        FILE *fp = wxFopen(LM_CSTR(strExtFile), uT("r"));
                                        if (fp) {
                                            const char_t *pszF = LM_CSTR(pFrame->getErrInfo());
                                            while (wxFgets(pszLine, 32767, fp) && (lineCount <= iErrline)) {
                                                if ((iErrline == lineCount) && Tstrstr(pszLine, pszF)) {
                                                    strFilename = pFrame->getLaTeXFilename();
                                                    break;
                                                }
                                                ++lineCount;
                                            }
                                            fclose(fp);
                                            fp = NULL;
                                        }
                                        free(pszLine);
                                        pszLine = NULL;
                                    }
                                }
                            }
                        }
                    }
                    if (bFound) {
                        pEdit->resetErrorMarkers();
                        pFrame->selectEditor(pEdit);
                        pEdit->DoAddStatus(iErrline, SCRIPT_MASK_ERRORBIT);
                        pFrame->updateEditorStatus(pEdit);
                    }
                }
                pFrame->printLaTeXReport(iExitStatus, strFilename);
            }
            // END LaTeX stuff

            // Solis stuff
            if (GetLexer() == wxSTC_LEX_SOLIS) {
                int iErrline = pFrame->getErrLine();
                if (iErrline >= 0) {
                    ScriptEdit *pEdit = this;
                    if (pEdit->getScriptId() != pFrame->getErrId())
                        // search in the opened files
                        for (int ii = 0; ii < pFrame->getEditorCount(); ii++) {
                            pEdit = pFrame->getEditor(ii);
                            if (NULL == pEdit) {
                                pEdit = this;
                                break;
                            }
                            if ((iErrline < pEdit->GetLineCount()) && (pEdit->getScriptId() == pFrame->getErrId())) {
                                break;
                            }
                        }
                    pEdit->resetErrorMarkers();
                    pFrame->selectEditor(pEdit);
                    pEdit->DoAddStatus(iErrline, SCRIPT_MASK_ERRORBIT);
                    pFrame->updateEditorStatus(pEdit);
                }
            }
            // END Solis stuff
        }
    }

    processSetRunning();

    return;
}

void ScriptEdit::processKill(bool bSilent /* = false*/)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame == NULL) {
        // should never happen
        return;
    }

    if (m_pProcessTimer != NULL) {
        m_pProcessTimer->Stop();
        delete m_pProcessTimer;
        m_pProcessTimer = NULL;
    }

    if (processIsAlive()) {
        if (m_pProcess != NULL) {
            m_pProcess->kill();
            // delete itself, in ::wxExecute (if explicitely killed) or in CometProcess::OnTerminate if normally terminated
            m_pProcess = NULL;
        }
        else {
            wxProcess::Kill(m_iProcessId, wxSIGKILL, wxKILL_CHILDREN);
        }
        m_iProcessId = -1L;
        if (false == bSilent) {
            if (processIsAlive() == false) {
                wxString strDisp = uT("External tool stopped");
                pFrame->OutputStatusbar(strDisp, SIGMAFRAME_TIMER_SHORT);
                strDisp += uT("\n");
                pFrame->Output(strDisp);
            }
        }
    }

    processSetRunning();

    // delete itself, in ::wxExecute (if explicitely killed) or in CometProcess::OnTerminate if normally terminated
    m_pProcess = NULL;
}

bool ScriptEdit::luaRunScript(void)
{
    if (m_iLexer != wxSTC_LEX_LUA) {
        return false;
    }

    m_bCanSetFocus = true;

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return false;
    }

    pFrame->setRunningLexer(wxSTC_LEX_NULL);

    // Set script read-only during buffering
    bool bReadWriteStatus = this->GetReadOnly();
    this->SetReadOnly(true);

    m_strErrMsg = wxEmptyString;

    if (isRunning()) {
        if (isDebugging()) {
            if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
                m_bDebugging = false;
                m_bDebugIt = true;
                m_pMutex->Unlock();
            }
            this->SetReadOnly(bReadWriteStatus);
            return true;
        }
        this->SetReadOnly(bReadWriteStatus);
        return false;
    }

    if (m_strFilename.IsEmpty()) {
        if (DoSaveFile() == false) {
            this->SetReadOnly(bReadWriteStatus);
            return false;
        }
    }

    setRunning(true);
    pFrame->ClearDebugWindow(GetFilename());

    wxString strT;

    int iLineCount = GetLineCount();
    if (iLineCount < 1) {
        setRunning(false);
        this->SetReadOnly(bReadWriteStatus);
        return false;
    }
    if (iLineCount > LF_SCRIPT_MAXLINES) {
        strT = uT("Cannot run script: maximum lines count reached (maximum = 32768)");
        pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        SigmaMessageBox(strT, uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        setRunning(false);
        this->SetReadOnly(bReadWriteStatus);
        return false;
    }

    if (iLineCount == 1) {
        strT = GetLine(0);
        int iScriptLen = (int)(strT.Length());
        if (iScriptLen < 12) {
            strT = strT.Trim(false);
            if (strT.CmpNoCase(uT("author")) == 0) {
                pFrame->Output(uT("Pr. Sidi HAMADY"));
                setRunning(false);
                this->SetReadOnly(bReadWriteStatus);
                return true;
            }
            if (strT.CmpNoCase(uT("version")) == 0) {
                strT = uT("Version ");
                strT += COMET_VERSION_STRING;
                strT += uT(" Build ");
                strT += COMET_BUILD_STRING;
                pFrame->Output(strT);
                setRunning(false);
                this->SetReadOnly(bReadWriteStatus);
                return true;
            }
            if (strT.CmpNoCase(uT("release")) == 0) {
                pFrame->Output(__TDATE__);
                setRunning(false);
                this->SetReadOnly(bReadWriteStatus);
                return true;
            }
        }
    }

    const size_t iLineMaxLen = LM_STRSIZEW;
    size_t iBufferSize = (size_t)(iLineCount * iLineMaxLen);
    char *pszBufferA = (char *)malloc(iBufferSize * sizeof(char));
    if (pszBufferA == NULL) {
        pFrame->OutputStatusbar(uT("Cannot run script: insufficient memory"), SIGMAFRAME_TIMER_SHORT);
        SigmaMessageBox(uT("Cannot run script: insufficient memory"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        setRunning(false);
        this->SetReadOnly(bReadWriteStatus);
        return false;
    }

    pszBufferA[0] = pszBufferA[iBufferSize - 1] = 0;

    size_t nLenLineA, nLengthA = 0, nLengthW;
    wxString strBufferW;
    strBufferW.Empty();

    // set decimal point to... point!
    wxSetlocale(LC_NUMERIC, uT("C"));

    SigmaBusyCursor waitC;
    if (iLineCount > 2048) {
        waitC.start();
    }

    if (m_bEnableCodeAnalyzer && (m_pCodeAnalyzer != NULL)) {
        m_pCodeAnalyzer->reset();
        m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_LUA);
        m_pCodeAnalyzer->setFilename(this->GetFilename());
    }

    char_t szLine[LM_STRSIZEL];
    Tmemset(szLine, 0, LM_STRSIZEL);

    int iLine = 0, iId = 0, iBalanceFunc = 0, iBalanceLoop = 0, iBalanceComment = 0, iLen;

    nLengthA = 0;

    // Remove previous error and debugging markers
    MarkerDeleteAll(SCRIPT_MASK_ERRORBIT);
    MarkerDeleteAll(SCRIPT_MASK_DEBUGBIT);
    //

    for (int ii = 0; ii < iLineCount; ii++) {

        strBufferW = GetLine(ii);
        nLengthW = strBufferW.Length();
        if (nLengthW < 1) {
            // Empty line (keep all lines to make the link with breakpoints)
            pszBufferA[nLengthA] = LM_NEWLINE_CHARA;
            pszBufferA[nLengthA + 1] = '\0';
            nLengthA += 1;
            continue;
        }

        // Too long line?
        if (nLengthW >= (iLineMaxLen - 2)) {
            free(pszBufferA);
            pszBufferA = NULL;

            setRunning(false);
            this->SetReadOnly(bReadWriteStatus);

            wxString strT = wxString::Format(uT("Cannot run script: line %d too long (maximum: %d characters)."), ii + 1, iLineMaxLen - 2);
            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
            SigmaMessageBox(strT, uT("Comet"), wxOK | wxICON_EXCLAMATION, this);

            return false;
        }
        //

        // Convert from Unicode to ANSI
        nLenLineA = strlen(LM_U8STR(strBufferW));
        strcat(pszBufferA, LM_U8STR(strBufferW));
        nLengthA += nLenLineA;
        //

        // Add newline if necessary
        if (pszBufferA[nLengthA - 1] != LM_NEWLINE_CHARA) {
            pszBufferA[nLengthA] = LM_NEWLINE_CHARA;
            pszBufferA[nLengthA + 1] = '\0';
            nLengthA += 1;
        }
        //

        // Analyse every line of the code
        if (m_bEnableCodeAnalyzer && (m_pCodeAnalyzer != NULL) && (m_pCodeAnalyzer->getCount() < CodeAnalyzer::MAXCOUNT)) {

            Tstrncpy(szLine, LM_CSTR(strBufferW), LM_STRSIZEL - 1);

            iLen = lm_trim(szLine);

            if (iLen >= 4) {
                // Comment block?
                if ((szLine[0] == uT('-')) && (szLine[1] == uT('-')) && (szLine[2] == uT('[')) && (szLine[3] == uT('['))) {
                    iBalanceComment += 1;
                    iLine += 1;
                    continue;
                }
                else if ((szLine[0] == uT('-')) && (szLine[1] == uT('-')) && (szLine[2] == uT(']')) && (szLine[3] == uT(']'))) {
                    iBalanceComment -= 1;
                    iLine += 1;
                    continue;
                }
                else if ((szLine[iLen - 2] == uT(']')) && (szLine[iLen - 1] == uT(']'))) {
                    iBalanceComment -= 1;
                    iLine += 1;
                    continue;
                }
            }
            else if (iLen >= 2) {
                // Comment block?
                if ((szLine[iLen - 2] == uT(']')) && (szLine[iLen - 1] == uT(']'))) {
                    iBalanceComment -= 1;
                    iLine += 1;
                    continue;
                }
            }

            if (iBalanceComment > 0) {
                iLine += 1;
                continue;
            }

            m_pCodeAnalyzer->analyzeLine(szLine, iLen, &iId, &iLine, &iBalanceFunc, &iBalanceLoop);
        }
        //
    }

    // Update the multifunction toolbar combobox, if necessary
    if (m_bEnableCodeAnalyzer && (m_pCodeAnalyzer != NULL)) {
        pFrame->DoAnalyzerUpdate(false, false);
    }
    //

    luaReset();

    ScriptThread *pThread = NULL;
    try {
        pThread = new ScriptThread();
    }
    catch (...) {
        pThread = NULL;
    }
    if (pThread == NULL) {
        free(pszBufferA);
        pszBufferA = NULL;

        setRunning(false);
        this->SetReadOnly(bReadWriteStatus);

        pFrame->OutputStatusbar(uT("Cannot start the working thread: insufficient memory"), SIGMAFRAME_TIMER_SHORT);
        SigmaMessageBox(uT("Cannot start the working thread: insufficient memory"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);

        return false;
    }

    wxFileName fname = this->GetFilename();
    wxString strPath = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    if (strPath.Length() > 2) {
        // ::ANSI::    NOT::UNICODE::
        char szT[LM_STRSIZE];
        memset(szT, 0, LM_STRSIZE * sizeof(char));
        strncpy(szT, LM_U8STR(strPath), LM_STRSIZE - 1);
        chdir(szT);
        //
    }

    wxThreadError errT = pThread->Create((void *)this, pszBufferA, iBufferSize, iLineCount);

    free(pszBufferA);
    pszBufferA = NULL;

    // Restore script read-write status
    this->SetReadOnly(bReadWriteStatus);

    if (errT != wxTHREAD_NO_ERROR) {
        setRunning(false);

        if (errT == wxTHREAD_NO_RESOURCE) {
            pFrame->OutputStatusbar(uT("Cannot run the script: insufficient memory"), SIGMAFRAME_TIMER_SHORT);
            SigmaMessageBox(uT("Cannot run the script: insufficient memory"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        }
        else if (errT == wxTHREAD_RUNNING) {
            pFrame->OutputStatusbar(uT("Cannot run the script: already running"), SIGMAFRAME_TIMER_SHORT);
            SigmaMessageBox(uT("Cannot run the script: already running"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        }

        return false;
    }

    errT = pThread->Run();
    if (errT != wxTHREAD_NO_ERROR) {
        setRunning(false);

        if (errT == wxTHREAD_NO_RESOURCE) {
            pFrame->OutputStatusbar(uT("Cannot run the script: insufficient memory"), SIGMAFRAME_TIMER_SHORT);
            SigmaMessageBox(uT("Cannot run the script: insufficient memory"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        }
        else if (errT == wxTHREAD_RUNNING) {
            pFrame->OutputStatusbar(uT("Cannot run the script: already running"), SIGMAFRAME_TIMER_SHORT);
            SigmaMessageBox(uT("Cannot run the script: already running"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        }

        return false;
    }

    return true;
}

bool ScriptEdit::luaReset(void)
{
    if (NULL == m_pMutex) {
        // should never happen
        return false;
    }

    if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_bStopIt = false;
        m_bReadDone = false;
        m_strRead = uT("");
        m_bAskDone = false;
        m_iAnswer = -1;
        m_pMutex->Unlock();
    }

    return true;
}

bool ScriptEdit::luaStopScript(void)
{
    if (NULL == m_pMutex) {
        // should never happen
        return false;
    }

    bool bRet = false;
    if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_bDebugIt = false;
        m_bStopIt = true;
        bRet = m_bStopIt;
        m_pMutex->Unlock();
    }

    return bRet;
}

bool ScriptEdit::luaDebugScript(void)
{
    if (NULL == m_pMutex) {
        // should never happen
        return false;
    }

    bool bRet = false;
    if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_bDebugIt = true;
        m_bStopIt = false;
        bRet = m_bDebugIt;
        m_pMutex->Unlock();
    }

    return bRet;
}

bool ScriptEdit::isRunning(bool *pbProcess /* = NULL*/)
{
    bool bProcessIsAlive = processIsAlive();
    if (pbProcess) {
        *pbProcess = bProcessIsAlive;
    }
    return (m_bRunning || bProcessIsAlive);
}

void ScriptEdit::processSetRunning(void)
{
    wxAuiNotebook *pNotebook = (wxAuiNotebook *)(GetParent());

    int indexT = pNotebook->GetPageIndex(this);

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame) {
        pFrame->updateEditorStatus(this);
    }

    if (indexT == pNotebook->GetSelection()) {
        if (pFrame) {
            wxString strT = uT("");
            if (processIsAlive()) {
                wxFileName fname = this->GetFilename();
                strT = uT("'");
                strT += fname.GetFullName();
                strT += uT("' running...");
            }
            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_NONE);
        }
    }
}

void ScriptEdit::setRunning(bool bRunning)
{
    m_bRunning = bRunning;

    // >> Lock
    if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_bStopIt = false;
        if (m_bRunning == false) {
            m_bDebugIt = false;
        }
        m_bReadDone = false;
        m_bAskDone = false;
        m_strRead = uT("");
        m_iAnswer = -1;
        m_pLuaEngine = NULL; // allocated in ScriptThread
        m_iRunningLine = -1;
        m_bGuierrSet = false;
        m_pMutex->Unlock();
    }

    m_bDebugging = m_bDebugIt;

    SetReadOnly(m_bDebugging);

    if (bRunning) {
        m_strDebugTrace.Empty();
        struct timeval timevalNow;
        gettimeofday(&timevalNow, NULL);
        m_fStartTime = (((double)(timevalNow.tv_sec)) * 1000.0) + (((double)(timevalNow.tv_usec)) / 1000.0);
        m_iErrLine = -1;
        m_iDebugCount = 0;
    }
    else {
        struct timeval timevalNow;
        gettimeofday(&timevalNow, NULL);
        m_fEndTime = (((double)(timevalNow.tv_sec)) * 1000.0) + (((double)(timevalNow.tv_usec)) / 1000.0);
    }

    wxAuiNotebook *pNotebook = (wxAuiNotebook *)(GetParent());

    int indexT = pNotebook->GetPageIndex(this);

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame) {
        pFrame->updateEditorStatus(this);
    }

    if (indexT == pNotebook->GetSelection()) {
        if (pFrame) {
            wxString strT = uT("");
            if (isRunning()) {
                wxFileName fname = this->GetFilename();
                strT = uT("'");
                strT += fname.GetFullName();
#ifdef __WXMSW__
                strT += m_bDebugging ? wxString::Format(uT("' debugging (%s)..."), stepInto() ? uT("step into") : uT("step over")) : uT("' running...");
#else
                strT += m_bDebugging ? wxString::Format(uT("' debugging (%ls)..."), stepInto() ? uT("step into") : uT("step over")) : uT("' running...");
#endif
            }
            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_NONE);
        }
    }
}
