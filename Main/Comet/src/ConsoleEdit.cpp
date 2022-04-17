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
#include "ConsoleEdit.h"
#include "ConsoleThread.h"

#include <wx/clipbrd.h>

BEGIN_EVENT_TABLE(ConsoleEdit, CodeEdit)

EVT_KEY_DOWN(ConsoleEdit::OnKeyDown)
EVT_CHAR(ConsoleEdit::OnChar)

EVT_CONTEXT_MENU(ConsoleEdit::OnContextMenu)

EVT_MENU(wxID_CUT, ConsoleEdit::OnEditAction)
EVT_MENU(wxID_COPY, ConsoleEdit::OnEditAction)
EVT_MENU(wxID_PASTE, ConsoleEdit::OnEditAction)
EVT_MENU(wxID_CLEAR, ConsoleEdit::OnEditAction)
EVT_MENU(ID_CONSOLE_UNDO, ConsoleEdit::OnEditAction)
EVT_MENU(ID_CONSOLE_REDO, ConsoleEdit::OnEditAction)
EVT_MENU(ID_CONSOLE_RESET, ConsoleEdit::OnEditAction)

EVT_MENU(ID_CONSOLE_RUN, ConsoleEdit::OnConsoleAction)

EVT_MENU(ID_CONSOLE_ZOOMIN, ConsoleEdit::OnConsoleAction)
EVT_MENU(ID_CONSOLE_ZOOMOUT, ConsoleEdit::OnConsoleAction)
EVT_MENU(ID_CONSOLE_ZOOMNONE, ConsoleEdit::OnConsoleAction)
EVT_MENU(ID_CONSOLE_SYNTAX, ConsoleEdit::OnConsoleAction)
EVT_MENU(ID_CONSOLE_AUTOCOMPLETE, ConsoleEdit::OnConsoleAction)

EVT_COMMAND(ID_THREAD_CLEAR, wxEVT_COMMAND_TEXT_UPDATED, ConsoleEdit::OnThreadUpdated)
EVT_COMMAND(ID_THREAD_PRINT, wxEVT_COMMAND_TEXT_UPDATED, ConsoleEdit::OnThreadUpdated)
EVT_COMMAND(ID_THREAD_PRINTERR, wxEVT_COMMAND_TEXT_UPDATED, ConsoleEdit::OnThreadUpdated)
EVT_COMMAND(ID_THREAD_FINISH, wxEVT_COMMAND_TEXT_UPDATED, ConsoleEdit::OnThreadUpdated)

EVT_MOUSEWHEEL(ConsoleEdit::OnMouseWheel)

END_EVENT_TABLE()

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

void ConsoleEdit::createEngine(void)
{
    m_pLuaEngine = new (std::nothrow) LuaEngine(LUA_ENGINE_GUI, CFUNCTION_IO, true);
    if (m_pLuaEngine == NULL) {
        return;
    }

    lua_State *pLuaState = m_pLuaEngine->getLuaState();

    lua_pushliteral(pLuaState, "___SigmaCaller___");
    lua_pushinteger(pLuaState, SIGMACALLER_CONSOLE);
    lua_settable(pLuaState, LUA_REGISTRYINDEX);

    lua_pushliteral(pLuaState, "___ScriptEdit___");
    lua_pushlightuserdata(pLuaState, (void *)this);
    lua_settable(pLuaState, LUA_REGISTRYINDEX);

    lua_pushliteral(pLuaState, "___SigmaGuierr___");
    lua_pushinteger(pLuaState, 0);
    lua_settable(pLuaState, LUA_REGISTRYINDEX);
}

ConsoleEdit::ConsoleEdit(wxWindow *parentT, wxWindowID idT,
                         const wxPoint &posT, const wxSize &sizeT, long styleT,
                         const wxString &strName)
    : CodeEdit(parentT, idT, posT, sizeT, styleT, strName)
{
    m_pThread = NULL;
    m_pLuaEngine = NULL;

    if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_iPlotCreatedId = 0L;
        m_bReadDone = false;
        m_bAskDone = false;
        m_strRead = uT("");
        m_iAnswer = -1;
        m_iRunningLine = -1;
        m_bGuierrSet = false;
        m_pMutex->Unlock();
    }

    m_bRunning = false;

    m_nCurCommand = m_nTotalCommand = 0;

    m_bError = false;
    m_bLastError = false;
    m_bNewLine = false;

    m_iMaxLineWidth = 0;

    m_strName = uT("Console");

    setModeScriptConsole(true);

    createEngine();

    // Accelerators
    const int nCount = 16;
    wxAcceleratorEntry accelEntries[nCount];
    accelEntries[0].Set(wxACCEL_CTRL, (int)'X', wxID_CUT);
    accelEntries[1].Set(wxACCEL_CTRL, (int)'C', wxID_COPY);
    accelEntries[2].Set(wxACCEL_CTRL, (int)'V', wxID_PASTE);
    accelEntries[3].Set(wxACCEL_CTRL, (int)'Y', ID_CONSOLE_REDO);
    accelEntries[4].Set(wxACCEL_CTRL, (int)'Z', ID_CONSOLE_UNDO);
    accelEntries[5].Set(wxACCEL_CTRL, (int)'+', ID_CONSOLE_ZOOMIN);
    accelEntries[6].Set(wxACCEL_CTRL, (int)'-', ID_CONSOLE_ZOOMOUT);
    accelEntries[7].Set(wxACCEL_CTRL, (int)'0', ID_CONSOLE_ZOOMNONE);
    accelEntries[8].Set(wxACCEL_CTRL, WXK_NUMPAD_ADD, ID_CONSOLE_ZOOMIN);
    accelEntries[9].Set(wxACCEL_CTRL, WXK_NUMPAD_SUBTRACT, ID_CONSOLE_ZOOMOUT);
    accelEntries[10].Set(wxACCEL_CTRL, WXK_NUMPAD0, ID_CONSOLE_ZOOMNONE);
    accelEntries[11].Set(wxACCEL_NORMAL, WXK_DELETE, wxID_CLEAR);
    accelEntries[12].Set(wxACCEL_NORMAL, WXK_NUMPAD_DELETE, wxID_CLEAR);
    accelEntries[13].Set(wxACCEL_NORMAL, WXK_F12, ID_CONSOLE_RUN);
    accelEntries[14].Set(wxACCEL_ALT, WXK_F12, ID_CONSOLE_STOP);
    accelEntries[15].Set(wxACCEL_NORMAL, WXK_TAB, ID_NOTHING);
    wxAcceleratorTable accelTable(nCount, accelEntries);
    SetAcceleratorTable(accelTable);
    //

    m_iMLineFirst = -1;

    m_iLineRun = -1;
}

void ConsoleEdit::initOutput(void)
{
    Output(uT("Type Lua statements & press Enter."));
    Output(uT("For multiline statements, use Shift+Enter."));
    Output(uT("Prepend '=' to evaluate expression & display result."));
    MarkerAdd(GetCurrentLine(), SCRIPT_MASK_SAVEDBIT);
    EmptyUndoBuffer();
}

void ConsoleEdit::restoreHistory(const char_t *pszSigmaConfigDir)
{
    // Restore history
    Tmemset(m_szHistoryFile, 0, LM_STRSIZE);
    Tstrncpy(m_szHistoryFile, pszSigmaConfigDir, LM_STRSIZE - 32);
    Tstrcat(m_szHistoryFile, uT("console.cometr"));
    FILE *fp = NULL;

    if ((fp = Tfopen(static_cast<const char_t *>(m_szHistoryFile), uT("rt"))) != NULL) {
        m_nTotalCommand = 0;
        char_t szT[LM_STRSIZE];
        Tmemset(szT, 0, LM_STRSIZE);
        int iLen = 0, ii = 0;
        bool bFound = false;
        while ((Tfgets(szT, LM_STRSIZE - 1, fp) != NULL) && (ii++ < CONSOLE_HISTORY)) {
            lm_trim(szT);
            if (szT[0] == uT('#')) {
                continue;
            }
            iLen = (int)(Tstrlen((const char_t *)szT));
            if (iLen > 1) {
                bFound = false;
                for (int ii = 0; ii < m_nTotalCommand; ii++) {
                    if (Tstricmp(LM_CSTR(m_aCommandHistory[ii]), static_cast<const char_t *>(szT)) == 0) {
                        bFound = true;
                        break;
                    }
                }
                if (bFound == false) {
                    m_aCommandHistory[m_nTotalCommand++] = szT;
                }
                if (m_nTotalCommand >= CONSOLE_HISTORY) {
                    break;
                }
            }
        }
        m_nCurCommand = m_nTotalCommand - 1;
        fclose(fp);
        fp = NULL;
    }
    //
}

void ConsoleEdit::saveHistory(void)
{
    // Save history
    if (m_nTotalCommand >= 1) {
        FILE *fp = NULL;
        if ((fp = Tfopen(static_cast<const char_t *>(m_szHistoryFile), uT("w"))) != NULL) {

            wxString strHeader = CometApp::getConfigHeader();
            Tfputs(LM_CSTR(strHeader), fp);

            if (m_nTotalCommand > 1) {
                for (int ii = 0; ii < (m_nTotalCommand - 1); ii++) {
                    Tfputs(LM_CSTR(m_aCommandHistory[ii]), fp);
                    if (ii < (m_nTotalCommand - 2)) {
                        Tfputs(uT("\n"), fp);
                    }
                }
            }
            if (m_bLastError == false) {
                Tfputs(uT("\n"), fp);
                Tfputs(LM_CSTR(m_aCommandHistory[m_nTotalCommand - 1]), fp);
            }
            fclose(fp);
            fp = NULL;
        }
    }
    else {
        Tunlink(static_cast<const char_t *>(m_szHistoryFile));
    }
    //
}

ConsoleEdit::~ConsoleEdit()
{
    if (m_pMutex) {
        delete m_pMutex;
        m_pMutex = NULL;
    }
    if (m_pLuaEngine) {
        delete m_pLuaEngine;
        m_pLuaEngine = NULL;
    }

    saveHistory();
    m_nCurCommand = m_nTotalCommand = 0;
}

bool ConsoleEdit::setPrefs(const ScintillaPrefs &tPrefs, bool bInit /* = true*/)
{
    bool bRet = CodeEdit::setPrefs(tPrefs, false);
    m_ScintillaPrefs.common.foldEnable = false;
    m_ScintillaPrefs.common.wrapModeInitial = false;
    m_ScintillaPrefs.common.lineNumberEnable = false;
    m_ScintillaPrefs.common.displayEOLEnable = false;
    m_ScintillaPrefs.common.lineSpacing = 0;

    long iB = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
    long iF = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
    iB = CodeEdit::normalizeColor(iB, (iB > iF) ? -4L : 4L);
    m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background = iB;

    if (bInit) {

        CodeEdit::initPrefs();

        unsigned long lBack = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
        unsigned long lFore = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
        wxColor clrBack = wxColor(lBack);
        wxColor clrFore = wxColor(lFore);

        // Status (character) margin
        SetMarginMask(m_StatusID, SCRIPT_MASK_SAVED | SCRIPT_MASK_ERROR);
        SetMarginType(m_StatusID, wxSTC_MARGIN_SYMBOL);
        SetMarginWidth(m_StatusID, m_ScintillaPrefs.common.statusEnable ? m_StatusMargin : 0);
        SetMarginSensitive(m_StatusID, false);

        MarkerDefine(SCRIPT_MASK_SAVEDBIT, wxSTC_MARK_CHARACTER + '>', clrFore, clrBack);
        MarkerDefineBitmap(SCRIPT_MASK_ERRORBIT, wxBitmap(error_small_xpm));

        SetMarginLeft(6);
        SetMarginRight(6);

        SetMarginWidth(m_FoldingID, 0);
        SetMarginWidth(m_MarkerID, 0);
        SetMarginWidth(m_LinenumberID, 0);

        SetCaretForeground(wxColor(m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground));
        SetCaretLineBackground(wxColor(m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background));
        SetCaretLineVisible(false);

        SetViewEOL(false);
        SetIndentationGuides(false);
        SetEdgeMode(wxSTC_EDGE_NONE);
        SetViewWhiteSpace(wxSTC_WS_INVISIBLE);

        calcStatusMargin();
    }

    return bRet;
}

void ConsoleEdit::doClear(void)
{
    ClearAll();
    if (this->isFocused()) {
        updateStatusbar();
    }
}

void ConsoleEdit::Reset(void)
{
    if (isRunning()) {
        return;
    }

    int idT = SigmaMessageBox(uT("Reset Script Console?"), uT("Comet"),
                              wxYES_NO | wxICON_QUESTION | wxNO_DEFAULT, this, uT("Reset"), uT("Cancel"));
    if (idT != wxYES) {
        return;
    }

    m_iMLineFirst = -1;
    m_bNewLine = false;

    m_nCurCommand = m_nTotalCommand = 0;

    if (m_pLuaEngine) {
        delete m_pLuaEngine;
        m_pLuaEngine = NULL;
    }
    createEngine();

    MarkerDeleteAll(SCRIPT_MASK_SAVEDBIT);
    MarkerDeleteAll(SCRIPT_MASK_ERRORBIT);
    doClear();
    MarkerAdd(GetCurrentLine(), SCRIPT_MASK_SAVEDBIT);
    EmptyUndoBuffer();
}

void ConsoleEdit::AddToHistory(const char_t *pszCommand)
{
    if (m_iMLineFirst >= 0) {
        return;
    }

    if (pszCommand[0] != uT('\0')) {
        if ((m_bError == false) && m_bLastError && (m_nTotalCommand > 0)) {
            bool bFound = false;
            for (int ii = 0; ii < m_nTotalCommand; ii++) {
                if (Tstricmp(LM_CSTR(m_aCommandHistory[ii]), static_cast<const char_t *>(pszCommand)) == 0) {
                    bFound = true;
                    break;
                }
            }
            if (bFound == false) {
                m_aCommandHistory[m_nTotalCommand - 1] = pszCommand;
            }
            m_nCurCommand = m_nTotalCommand - 1;
            m_bLastError = false;
            return;
        }

        if (m_nTotalCommand >= CONSOLE_HISTORY) {
            int is = 5;
            for (int ii = is; ii < CONSOLE_HISTORY; ii++) {
                m_aCommandHistory[ii - is] = m_aCommandHistory[ii];
            }
            m_nTotalCommand -= is;
        }
        bool bFound = false;
        for (int ii = 0; ii < m_nTotalCommand; ii++) {
            if (Tstricmp(LM_CSTR(m_aCommandHistory[ii]), static_cast<const char_t *>(pszCommand)) == 0) {
                bFound = true;
                break;
            }
        }
        if (bFound == false) {
            m_aCommandHistory[m_nTotalCommand++] = pszCommand;
        }
        m_nCurCommand = m_nTotalCommand - 1;
    }
}

int ConsoleEdit::SetCommand(const char_t *pszCommand)
{
    if (m_iMLineFirst >= 0) {
        return 0;
    }

    int nLength = (int)Tstrlen(pszCommand);
    int nLen = nLength;
    if ((nLength < 2) || (nLength >= LM_STRSIZE)) {
        return 0;
    }
    char_t *pszT = (char_t *)pszCommand;
    nLength = (int)Tstrlen(pszT);
    if ((nLength < 2) || (nLength >= LM_STRSIZE)) {
        return 0;
    }
    wxStrncpy(m_szCommand, pszT, LM_STRSIZE - 1);
    if (m_szCommand[nLength - 1] == LM_NEWLINE_CHAR) {
        m_szCommand[nLength - 1] = uT('\0');
        nLen = nLength - 1;
    }
    if (m_szCommand[nLength - 2] == LM_NEWLINE_CHAR) {
        m_szCommand[nLength - 2] = uT('\0');
        nLen = nLength - 2;
    }
    return nLen;
}

int ConsoleEdit::LineIndex(int nLine /* = -1*/)
{
    if (nLine < 0) {
        nLine = GetCurrentLine();
    }
    if (nLine < 1) {
        return 0;
    }

    return (GetLineEndPosition(nLine) - (int)(GetLine(nLine).Length()));
}

void ConsoleEdit::SetCurCommand(const wxString &strCommand)
{
    if (isRunning()) {
        return;
    }

    int iLine = GetCurrentLine();

    int iLinePos = LineIndex(iLine);
    int iStart = iLinePos;
    int iStop = GetLineEndPosition(iLine);
    SetSelection(iStart, iStop);
    this->ReplaceSelection(strCommand);
    iStop = GetLineEndPosition(iLine);
    SetSelection(iStop, iStop);
}

bool ConsoleEdit::IsEditingLine(void)
{
    int iLine = GetCurrentLine();
    if (m_iMLineFirst >= 0) {
        return (iLine >= m_iMLineFirst);
    }
    return (GetCurrentLine() == (GetLineCount() - 1));
}

void ConsoleEdit::OnKeyDown(wxKeyEvent &tEvent)
{
    int iKey = tEvent.GetKeyCode();

    if (!IsEditingLine()) {

        if ((iKey == WXK_RIGHT) || (iKey == WXK_LEFT) || (iKey == WXK_NUMPAD_RIGHT) || (iKey == WXK_NUMPAD_LEFT)) {

            CodeEdit::OnKeyDown(tEvent);
        }

        else if ((iKey == WXK_UP) || (iKey == WXK_DOWN) || (iKey == WXK_PAGEUP) || (iKey == WXK_PAGEDOWN) || (iKey == WXK_NUMPAD_UP) || (iKey == WXK_NUMPAD_DOWN) || (iKey == WXK_NUMPAD_PAGEUP) || (iKey == WXK_NUMPAD_PAGEDOWN)) {

            int iLineCount = GetLineCount();
            if (iLineCount < 2) {
                return;
            }
            int iLine = GetCurrentLine();
            int iSel = GetSelectionEnd() - LineIndex(iLine);
            if (iSel >= 0) {
                if ((iLine > 0) && ((iKey == WXK_UP) || (iKey == WXK_PAGEUP) || (iKey == WXK_NUMPAD_UP) || (iKey == WXK_NUMPAD_PAGEUP))) {

                    iLine -= ((iKey == WXK_UP) || (iKey == WXK_NUMPAD_UP)) ? 1 : 5;
                    if (iLine < 0) {
                        iLine = 0;
                    }

                    int iSelNew = LineIndex(iLine) + iSel;
                    if (iSelNew > GetLineEndPosition(iLine)) {
                        iSelNew = GetLineEndPosition(iLine);
                    }

                    SetSelection(iSelNew, iSelNew);
                }
                else if ((iLine < (iLineCount - 1)) && ((iKey == WXK_DOWN) || (iKey == WXK_PAGEDOWN) || (iKey == WXK_NUMPAD_DOWN) || (iKey == WXK_NUMPAD_PAGEDOWN))) {

                    iLine += ((iKey == WXK_DOWN) || (iKey == WXK_NUMPAD_DOWN)) ? 1 : 5;
                    if (iLine >= iLineCount) {
                        iLine = iLineCount - 1;
                    }

                    int iSelNew = LineIndex(iLine) + iSel;
                    if (iSelNew > GetLineEndPosition(iLine)) {
                        iSelNew = GetLineEndPosition(iLine);
                    }

                    SetSelection(iSelNew, iSelNew);
                }
            }
        }

        else if (((iKey == ('C')) || (iKey == ('c'))) && (wxGetKeyState(WXK_CONTROL))) {
            Copy();
        }

        return;
    }

    if ((iKey == WXK_RETURN) || (iKey == WXK_NUMPAD_ENTER)) {

        if (this->isRunning()) {
            return;
        }

        if (AutoCompActive()) {
            CodeEdit::OnKeyDown(tEvent);
            return;
        }

        // Multiline statements
        if (wxGetKeyState(WXK_SHIFT)) {
            if (m_iMLineFirst < 0) {
                m_iMLineFirst = GetCurrentLine();
            }
            CodeEdit::OnKeyDown(tEvent);
            MarkerAdd(GetCurrentLine(), SCRIPT_MASK_SAVEDBIT);
            return;
        }
        //

        luaRunScript();
        return;
    }

    else if ((iKey == WXK_DELETE) || (iKey == WXK_NUMPAD_DELETE)) {
        if (!IsEditingLine()) {
            return;
        }
    }

    else if (iKey == WXK_BACK) {

        if (!IsEditingLine()) {
            return;
        }

        int iStart = 0, iStop = 0;
        GetSelection(&iStart, &iStop);

        int iLine = (m_iMLineFirst < 0) ? GetCurrentLine() : m_iMLineFirst;

        if ((iLine > 0) && (iStart <= (GetLineEndPosition(iLine - 1) + 1))) {
            return;
        }
        if (iStart <= LineIndex(iLine)) {
            return;
        }
    }

    else if ((iKey == WXK_UP) || (iKey == WXK_NUMPAD_UP)) {

        if (AutoCompActive() || (m_iMLineFirst >= 0)) {
            CodeEdit::OnKeyDown(tEvent);
            return;
        }

        if (m_nTotalCommand < 1) {
            return;
        }

        if ((m_nCurCommand < 0) || (m_nCurCommand >= m_nTotalCommand)) {
            m_nCurCommand = m_nTotalCommand - 1;
        }
        SetCurCommand(m_aCommandHistory[m_nCurCommand]);
        m_nCurCommand -= 1;

        return;
    }

    else if ((iKey == WXK_DOWN) || (iKey == WXK_NUMPAD_DOWN)) {

        if (AutoCompActive() || (m_iMLineFirst >= 0)) {
            CodeEdit::OnKeyDown(tEvent);
            return;
        }

        if (m_nTotalCommand < 1) {
            return;
        }

        if ((m_nCurCommand >= m_nTotalCommand) || (m_nCurCommand < 0)) {
            m_nCurCommand = 0;
        }
        SetCurCommand(m_aCommandHistory[m_nCurCommand]);
        m_nCurCommand += 1;

        return;
    }

    else if ((iKey == WXK_PAGEUP) || (iKey == WXK_PAGEDOWN) || (iKey == WXK_NUMPAD_PAGEUP) || (iKey == WXK_NUMPAD_PAGEDOWN)) {

        if (AutoCompActive()) {
            CodeEdit::OnKeyDown(tEvent);
            return;
        }

        int iLineCount = GetLineCount();
        if (iLineCount < 2) {
            return;
        }
        int iLine = GetCurrentLine();
        int iSel = GetSelectionEnd() - LineIndex(iLine);
        if (iSel >= 0) {
            if ((iLine > 0) && ((iKey == WXK_PAGEUP) || (iKey == WXK_NUMPAD_PAGEUP))) {
                iLine -= 5;
                if (iLine < 0) {
                    iLine = 0;
                }
                int iSelNew = LineIndex(iLine) + iSel;
                if (iSelNew > GetLineEndPosition(iLine)) {
                    iSelNew = GetLineEndPosition(iLine);
                }
                SetSelection(iSelNew, iSelNew);
            }
            else if ((iLine < (iLineCount - 1)) && ((iKey == WXK_PAGEDOWN) || (iKey == WXK_NUMPAD_PAGEDOWN))) {
                iLine += 5;
                if (iLine >= iLineCount) {
                    iLine = iLineCount - 1;
                }
                int iSelNew = LineIndex(iLine) + iSel;
                if (iSelNew > GetLineEndPosition(iLine)) {
                    iSelNew = GetLineEndPosition(iLine);
                }
                SetSelection(iSelNew, iSelNew);
            }
        }

        return;
    }

    else if (((iKey == ('C')) || (iKey == ('c'))) && (wxGetKeyState(WXK_CONTROL))) {
        Copy();
        return;
    }

    else if (((iKey == ('V')) || (iKey == ('v'))) && (wxGetKeyState(WXK_CONTROL))) {
        DoEditPaste();
        return;
    }

    CodeEdit::OnKeyDown(tEvent);
}

void ConsoleEdit::OnChar(wxKeyEvent &tEvent)
{
    if (this->isRunning()) {
        return;
    }

    CodeEdit::OnChar(tEvent);
}

void ConsoleEdit::luaRunScript(void)
{
    if (this->isRunning()) {
        return;
    }

    DoCallTipCancel();

    int imline = m_iMLineFirst;
    m_iMLineFirst = -1;

    if (imline >= 0) {
        m_iLineRun = imline;
    }
    else {
        m_iLineRun = GetCurrentLine();
    }

    this->setRunning(true);

    // set decimal point to... point!
    wxSetlocale(LC_NUMERIC, uT("C"));

    wxString strT = uT("");

    int nLine = GetLineCount() - 1;
    int nPos = GetLineEndPosition(nLine);
    SetSelection(nPos, nPos);

    if (imline >= 0) {

        if (imline > nLine) {
            this->setRunning(false);
            return;
        }

        for (int ii = imline; ii <= nLine; ii++) {
            strT += GetLine(ii);
            if (ii < nLine) {
                strT += uT("\n");
            }
        }
    }
    else {

        strT = GetLine(nLine);
        int nLength = (int)(strT.Length());
        if (nLength < 2) {
            this->setRunning(false);
            return;
        }

        if (SetCommand(LM_CSTR(strT)) < 1) {
            this->setRunning(false);
            return;
        }

        strT = static_cast<const char_t *>(m_szCommand);
    }

    m_bNewLine = true;

    char_t cFirst = strT[0];
    if (cFirst == uT('=')) {
        strT = strT.Mid(1);
        strT.Prepend(uT("print("));
        strT.Append(uT(")"));
    }

    if (strT.CmpNoCase(uT("author")) == 0) {
        endThread(uT("Pr. Sidi HAMADY"));
        return;
    }

    if (strT.CmpNoCase(uT("release")) == 0) {
        endThread(__TDATE__);
        return;
    }

    if (strT.CmpNoCase(uT("version")) == 0) {
        strT = uT("Version ");
        strT += COMET_VERSION_STRING;
        strT += uT(" Build ");
        strT += COMET_BUILD_STRING;
        endThread(strT);
        return;
    }

    if (strT.CmpNoCase(uT("contact")) == 0) {
        endThread(uT("sidi@hamady.org"));
        return;
    }

    char szBufferA[LM_STRSIZE];
    memset(szBufferA, 0, LM_STRSIZE * sizeof(char));

    // Convert from Unicode to ANSI
    strcpy(szBufferA, LM_U8STR(strT));
    int nLengthA = (int)strlen((const char *)szBufferA);
    if (nLengthA < 1) {
        this->setRunning(false);
        return;
    }
    //

    if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_bReadDone = false;
        m_bAskDone = false;
        m_strRead = uT("");
        m_iAnswer = -1;
        m_pMutex->Unlock();
    }

    ConsoleThread *pThread = NULL;
    try {
        pThread = new ConsoleThread();
    }
    catch (...) {
        pThread = NULL;
    }
    if (pThread == NULL) {
        this->endThread(uT("! Cannot start the working thread: insufficient memory"));
        return;
    }

    m_pThread = pThread;

    wxThreadError errT = pThread->Create(m_pLuaEngine, (void *)this, szBufferA, nLengthA);

    if (errT != wxTHREAD_NO_ERROR) {
        if (errT == wxTHREAD_NO_RESOURCE) {
            this->endThread(uT("! Cannot run the script: insufficient memory"));
        }
        else if (errT == wxTHREAD_RUNNING) {
            this->endThread(uT("! Cannot run the script: already running"));
        }
        else {
            this->endThread(uT("! Cannot run the script"));
        }
        return;
    }

    errT = pThread->Run();
    if (errT != wxTHREAD_NO_ERROR) {
        if (errT == wxTHREAD_NO_RESOURCE) {
            this->endThread(uT("! Cannot run the script: insufficient memory"));
        }
        else if (errT == wxTHREAD_RUNNING) {
            this->endThread(uT("! Cannot run the script: already running"));
        }
        else {
            this->endThread(uT("! Cannot run the script"));
        }
        return;
    }
}

void ConsoleEdit::luaStopScript(void)
{
    if (isRunning() && m_pThread) {
        endThread(uT("! Execution aborted"));
        return;
    }
}

void ConsoleEdit::OnContextMenu(wxContextMenuEvent &tEvent)
{
    DoCallTipCancel();

    wxPoint ptMousePosition = tEvent.GetPosition();
    if ((ptMousePosition.x == -1) && (ptMousePosition.y == -1)) {
        wxSize sizeT = GetSize();
        ptMousePosition.x = sizeT.x / 2;
        ptMousePosition.y = sizeT.y / 2;
    }
    else {
        ptMousePosition = ScreenToClient(ptMousePosition);
    }

    wxMenu popMenu;

    wxMenuItem *pMenu = NULL;

    bool bEditingLine = IsEditingLine();

    int ur = 0;
    if (bEditingLine && CanUndo()) {
        popMenu.Append(ID_CONSOLE_UNDO, uT("Undo"), uT("Undo the last action"), wxITEM_NORMAL);
        ur += 1;
    }
    if (bEditingLine && CanRedo()) {
        popMenu.Append(ID_CONSOLE_REDO, uT("Redo"), uT("Redo the last action"), wxITEM_NORMAL);
        ur += 1;
    }
    if (ur > 0) {
        popMenu.AppendSeparator();
    }

    int iStart, iEnd;
    bool bPasteAdded = false;
    GetSelection(&iStart, &iEnd);
    if ((iStart >= 0) && (iEnd >= 0) && (iStart < iEnd)) {
        if (!isRunning() && bEditingLine) {
            pMenu = popMenu.Append(wxID_CUT, uT("Cut"), uT("Cut the selected text"), wxITEM_NORMAL);
        }
        pMenu = popMenu.Append(wxID_COPY, uT("Copy"), uT("Copy the selected text"), wxITEM_NORMAL);
        if (!isRunning() && bEditingLine && CanPaste()) {
            pMenu = popMenu.Append(wxID_PASTE, uT("Paste"), uT("Paste text"), wxITEM_NORMAL);
            bPasteAdded = true;
        }
        popMenu.AppendSeparator();
    }

    if (bEditingLine) {
        if (!isRunning() && (bPasteAdded == false) && CanPaste()) {
            pMenu = popMenu.Append(wxID_PASTE, uT("Paste"), uT("Paste the selected text"), wxITEM_NORMAL);
            popMenu.AppendSeparator();
        }

        if (isRunning()) {
            popMenu.Append(ID_CONSOLE_STOP, uT("Stop"), uT("Stop Script"), wxITEM_NORMAL);
            popMenu.AppendSeparator();
        }
        else {
            int nLine = GetCurrentLine();
            wxString strT = GetLine(nLine);
            int nLength = (int)(strT.Length());
            if (nLength > 1) {
                popMenu.Append(ID_CONSOLE_RUN, uT("Execute"), uT("Execute Script"), wxITEM_NORMAL);
                popMenu.AppendSeparator();
            }
        }
    }

    wxMenu *menuView = new wxMenu();
    wxMenuItem *pItem = new wxMenuItem(menuView, ID_CONSOLE_ZOOMIN, uT("Zoom In"), uT("Increase the console's font size"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(zoomin_small_xpm);
#endif
    menuView->Append(pItem);
    pItem = new wxMenuItem(menuView, ID_CONSOLE_ZOOMOUT, uT("Zoom Out"), uT("Decrease the console's font size"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(zoomout_small_xpm);
#endif
    menuView->Append(pItem);
    menuView->Append(ID_CONSOLE_ZOOMNONE, uT("Normal Viewing"), uT("Set the console's font size to the default one"), wxITEM_NORMAL);

    popMenu.AppendSubMenu(menuView, uT("View"), uT("View"));

    wxMenu *menuOptions = new wxMenu();
    pMenu = menuOptions->Append(ID_CONSOLE_SYNTAX, uT("Syntax highlighting"), uT("Enable or disable syntax highlighting"), wxITEM_CHECK);
    pMenu->Check(m_ScintillaPrefs.common.syntaxEnable);
    pMenu = menuOptions->Append(ID_CONSOLE_AUTOCOMPLETE, uT("AutoCompletion"), uT("Enable or disable auto completion"), wxITEM_CHECK);
    pMenu->Check(m_ScintillaPrefs.common.autocompleteEnable);
    pMenu = menuOptions->Append(ID_CONSOLE_CALLTIP, uT("CallTip"), uT("Enable or disable call tips"), wxITEM_CHECK);
    pMenu->Check(m_ScintillaPrefs.common.calltipEnable);
    menuOptions->AppendSeparator();
    pItem = new wxMenuItem(menuOptions, ID_SCRIPT_COLORS, uT("Colors and Style..."), uT("Customize the editor colors"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(colors_small_xpm);
#endif
    menuOptions->Append(pItem);
    popMenu.AppendSubMenu(menuOptions, uT("Options"), uT("Options"));
    popMenu.AppendSeparator();

    pItem = new wxMenuItem(&popMenu, ID_CONSOLE_RESET, uT("Reset"), uT("Clear the console window"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(reset_small_xpm);
#endif
    popMenu.Append(pItem);

    int iPos = PositionFromPoint(ptMousePosition);
    if ((iPos < this->GetSelectionStart()) || (iPos > this->GetSelectionEnd())) {
        this->SetSelection(iPos, iPos);
    }

    PopupMenu(&popMenu, ptMousePosition);
}

void ConsoleEdit::OnScriptRun(wxCommandEvent &WXUNUSED(tEvent))
{
    luaRunScript();
}

void ConsoleEdit::endThread(const wxString &strT)
{
    this->setRunning(false);
    m_pThread = NULL;
    if (m_bError) {
        m_bLastError = true;
        AddToHistory(static_cast<const char_t *>(m_szCommand));
        MarkerDelete(m_iLineRun, SCRIPT_MASK_SAVEDBIT);
        MarkerAdd(m_iLineRun, SCRIPT_MASK_ERRORBIT);
        m_iLineRun = -1;
    }
    this->Output(strT);
    if (m_bError == false) {
        AddToHistory(static_cast<const char_t *>(m_szCommand));
        m_bLastError = false;
    }
    EmptyUndoBuffer();
    m_iMLineFirst = -1;
    m_bError = false;
    m_bNewLine = false;
    MarkerAdd(GetCurrentLine(), SCRIPT_MASK_SAVEDBIT);
}

void ConsoleEdit::getLine(wxString &strT, int &iLen)
{
    int nLine = GetCurrentLine();
    strT = GetLine(nLine);
    iLen = (int)(strT.Length());
}

void ConsoleEdit::Output(const wxString &strT)
{
    wxString strX = strT;
    strX.Replace(uT("\t"), uT(" "), true);
    strX.Replace(uT("\r"), uT(""), true);
    strX.Replace(uT("\n"), uT(" "), true);

    if (m_bNewLine) {
        m_bNewLine = false;
        if (GetCurLine().IsEmpty() == false) {
            wxString strU = strX;
            strX = SCRIPT_NEWLINE;
            strX += strU;
        }
    }

    if (strX.Length() < 1) {
        return;
    }

    if (strX.EndsWith(SCRIPT_NEWLINE) == false) {
        strX += SCRIPT_NEWLINE;
    }

    int iStop = GetSelectionEnd();
    SetSelection(iStop, iStop);
    ReplaceSelection(strX);
    iStop = GetSelectionEnd();
    SetSelection(iStop, iStop);

    int iW = this->TextWidth(wxSTC_STYLE_DEFAULT, strT);
    if (iW > m_iMaxLineWidth) {
        m_iMaxLineWidth = iW;
        this->SetScrollWidth(m_iMaxLineWidth);
        this->Refresh();
    }

    if (this->isFocused()) {
        updateStatusbar();
    }
}

void ConsoleEdit::OnThreadUpdated(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();
    if (idT == ID_THREAD_CLEAR) {
        this->doClear();
    }

    else if (idT == ID_THREAD_PRINT) {
        this->Output(tEvent.GetString());
    }

    else if (idT == ID_THREAD_PRINTERR) {
        m_bError = true;
        this->Output(tEvent.GetString());
    }

    else if (idT == ID_THREAD_FINISH) {
        endThread(tEvent.GetString());
    }
}

void ConsoleEdit::DoEditCut(void)
{
    if (GetReadOnly() || ((GetSelectionEnd() - GetSelectionStart()) <= 0)) {
        return;
    }

    if (!IsEditingLine()) {
        return;
    }

    int iStart = 0, iStop = 0;
    GetSelection(&iStart, &iStop);

    int iLine = (m_iMLineFirst < 0) ? GetCurrentLine() : m_iMLineFirst;
    if (iStart < LineIndex(iLine)) {
        return;
    }

    CodeEdit::Cut();
}

void ConsoleEdit::DoEditPaste(void)
{
    if (!IsEditingLine()) {
        return;
    }

    wxTextDataObject tData;
    bool isDataAvail = false;

    if (wxTheClipboard && wxTheClipboard->Open()) {
        wxTheClipboard->UsePrimarySelection(false);
        isDataAvail = wxTheClipboard->GetData(tData);
        wxTheClipboard->Close();
    }
    if (isDataAvail) {
        wxString strT = tData.GetText();

        wxString strL = uT("");
        int iLen = 0;
        getLine(strL, iLen);
        int iStart, iEnd;
        GetSelection(&iStart, &iEnd);
        iLen -= (iEnd - iStart);

        int iLenT = (int)(strT.Length());
        if (iLenT > (LM_STRSIZE - iLen - 1)) {
            return;
        }

        strT.Replace(uT("\t"), uT(" "), true);
        strT.Replace(uT("\r"), uT(""), true);
        strT.Replace(uT("\n"), uT(" "), true);
        this->ReplaceSelection(strT);
    }
}

void ConsoleEdit::DoEditCopy(void)
{
    if ((GetSelectionEnd() - GetSelectionStart()) <= 0) {
        return;
    }

    CodeEdit::Copy();
}

void ConsoleEdit::DoEditDelete(void)
{
    if (!IsEditingLine()) {
        return;
    }

    int iLine = GetCurrentLine();
    int iStart = 0, iStop = 0;
    GetSelection(&iStart, &iStop);
    if (iStop > iStart) {
        ReplaceSelection(uT(""));
    }
    else if ((iStop == iStart) && (iStart >= LineIndex(iLine))) {
        int iEnd = GetLineEndPosition(iLine);
        if ((iEnd > 0) && (iStart < iEnd)) {
            SetSelection(iStart, iStart + 1);
            ReplaceSelection(uT(""));
        }
    }
}

void ConsoleEdit::DoEditReset(void)
{
    if (isRunning()) {
        return;
    }

    Reset();
}

void ConsoleEdit::OnEditAction(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();

    switch (idT) {
        case wxID_CUT:
            DoEditCut();
            break;
        case wxID_COPY:
            DoEditCopy();
            break;
        case wxID_PASTE:
            DoEditPaste();
            break;
        case ID_CONSOLE_REDO:
            DoEditRedo();
            break;
        case ID_CONSOLE_UNDO:
            DoEditUndo();
            break;
        case wxID_CLEAR:
            DoEditDelete();
            break;
        case ID_CONSOLE_RESET:
            DoEditReset();
            break;
        default:
            break;
    }
}

void ConsoleEdit::OnMouseWheel(wxMouseEvent &tEvent)
{
    if (tEvent.ControlDown()) {
        if (tEvent.GetWheelRotation() < 0) {
            DoZoomIn();
        }
        else {
            DoZoomOut();
        }
        return;
    }

    CodeEdit::OnMouseWheel(tEvent);
}

void ConsoleEdit::OnConsoleAction(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();

    if (idT == ID_CONSOLE_RUN) {
        if (this->isRunning() || AutoCompActive()) {
            return;
        }
        luaRunScript();
    }
    else if (idT == ID_CONSOLE_ZOOMIN) {
        DoZoomIn();
    }
    else if (idT == ID_CONSOLE_ZOOMOUT) {
        DoZoomOut();
    }
    else if (idT == ID_CONSOLE_ZOOMNONE) {
        DoZoomNone();
    }
    else if (idT == ID_CONSOLE_SYNTAX) {
        m_ScintillaPrefs.common.syntaxEnable = (m_ScintillaPrefs.common.syntaxEnable == false);
        DoHighlightSyntax(m_ScintillaPrefs.common.syntaxEnable);
    }
    else if (idT == ID_CONSOLE_AUTOCOMPLETE) {
        m_ScintillaPrefs.common.autocompleteEnable = (m_ScintillaPrefs.common.autocompleteEnable == false);
        DoEnableAutocomplete(m_ScintillaPrefs.common.autocompleteEnable);
    }
    else if (idT == ID_CONSOLE_CALLTIP) {
        m_ScintillaPrefs.common.calltipEnable = (m_ScintillaPrefs.common.calltipEnable == false);
        DoEnableCalltip(m_ScintillaPrefs.common.calltipEnable);
    }
}

void ConsoleEdit::DoEditRedo(void)
{
    if (!CanRedo()) {
        return;
    }
    Redo();
}

void ConsoleEdit::DoEditUndo(void)
{
    if (!CanUndo()) {
        return;
    }
    Undo();
}

void ConsoleEdit::DoZoomIn(void)
{
    int iF = this->GetFont().GetPointSize();
    if (iF >= 80) {
        return;
    }
    ZoomIn();
    calcStatusMargin();
}

void ConsoleEdit::DoZoomOut(void)
{
    int iF = this->GetFont().GetPointSize();
    if (iF <= 4) {
        return;
    }
    ZoomOut();
    calcStatusMargin();
}

void ConsoleEdit::DoZoomNone(void)
{
    SetZoom(0);
    calcStatusMargin();
}
