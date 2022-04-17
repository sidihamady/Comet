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
#include "ScriptPrint.h"
#include "ScriptThread.h"

#include <wx/file.h>     // raw file io support
#include <wx/textfile.h> // text file io support
#include <wx/colour.h>
#include <wx/textdlg.h>
#include <wx/stdpaths.h>
#include <wx/clipbrd.h>
#include <wx/datetime.h>

#include <vector>

long ScriptEdit::EDITID = 1L;

#define FIND_INDICATOR 2

BEGIN_EVENT_TABLE(ScriptEdit, CodeEdit)

    EVT_SIZE(ScriptEdit::OnSize)

    EVT_SET_FOCUS(ScriptEdit::OnSetFocus)

    EVT_KEY_DOWN(ScriptEdit::OnKeyDown)
    EVT_LEFT_UP(ScriptEdit::OnMouseLeftUp)

    EVT_MENU(wxID_CUT, ScriptEdit::OnEditAction)
    EVT_MENU(wxID_COPY, ScriptEdit::OnEditAction)
    EVT_MENU(wxID_PASTE, ScriptEdit::OnEditAction)
    EVT_MENU(wxID_CLEAR, ScriptEdit::OnEditAction)
    EVT_MENU(wxID_SELECTALL, ScriptEdit::OnEditAction)

    EVT_MENU(ID_SCRIPT_SELECTWORD, ScriptEdit::OnEditAction)
    EVT_MENU(ID_SCRIPT_SELECTLINE, ScriptEdit::OnEditAction)
    EVT_MENU(ID_SCRIPT_UNDO, ScriptEdit::OnEditAction)
    EVT_MENU(ID_SCRIPT_REDO, ScriptEdit::OnEditAction)

    EVT_MENU(ID_ANALYZE_GOTO, ScriptEdit::OnEditAction)
    EVT_MENU(ID_INCLUDE_GOTO, ScriptEdit::OnEditAction)

    EVT_MENU(ID_SCRIPT_INSERT_DATE, ScriptEdit::OnEditAction)
    EVT_MENU(ID_SCRIPT_INSERT_TIME, ScriptEdit::OnEditAction)
    EVT_MENU(ID_SCRIPT_INSERT_DATETIME, ScriptEdit::OnEditAction)

    EVT_UPDATE_UI_RANGE(wxID_CUT, wxID_CLEAR, ScriptEdit::OnUpdateCopyPaste)
    EVT_UPDATE_UI_RANGE(ID_SCRIPT_UNDO, ID_SCRIPT_REDO, ScriptEdit::OnUpdateUndoRedo)

    EVT_STC_MARGINCLICK(wxID_ANY, ScriptEdit::OnMarginClick)
    EVT_STC_CHARADDED(wxID_ANY, ScriptEdit::OnCharAdded)
    EVT_STC_MODIFIED(wxID_ANY, ScriptEdit::OnModified)

    EVT_STC_DWELLSTART(wxID_ANY, ScriptEdit::OnDwellStart)
    EVT_STC_DWELLEND(wxID_ANY, ScriptEdit::OnDwellEnd)

    EVT_MENU(ID_SCRIPT_COMMENTSELECTION, ScriptEdit::OnCommentSelection)
    EVT_MENU(ID_SCRIPT_UNCOMMENTSELECTION, ScriptEdit::OnCommentSelection)

    EVT_MENU(ID_SCRIPT_UPPERCASE, ScriptEdit::OnUpperCase)
    EVT_MENU(ID_SCRIPT_LOWERCASE, ScriptEdit::OnLowerCase)

    // LaTeX stuff
    EVT_MENU(ID_LATEX_TEXTBF, ScriptEdit::OnLaTeXFormat)
    EVT_MENU(ID_LATEX_TEXTIT, ScriptEdit::OnLaTeXFormat)
    EVT_MENU(ID_LATEX_TEXTSC, ScriptEdit::OnLaTeXFormat)
    EVT_MENU(ID_LATEX_TEXTNORMAL, ScriptEdit::OnLaTeXFormat)
    EVT_MENU(ID_LATEX_MATHRM, ScriptEdit::OnLaTeXFormat)
    EVT_MENU(ID_LATEX_EMPH, ScriptEdit::OnLaTeXFormat)
    EVT_MENU(ID_LATEX_TEXTSUPERSCRIPT, ScriptEdit::OnLaTeXFormat)
    EVT_MENU(ID_LATEX_TEXTSUBSCRIPT, ScriptEdit::OnLaTeXFormat)
    //

    EVT_CONTEXT_MENU(ScriptEdit::OnContextMenu)
    EVT_MENU_RANGE(ID_SCRIPT_SNIPPET_FIRST, ID_SCRIPT_SNIPPET_LAST, ScriptEdit::OnInsertCodeSnippet)

    EVT_COMMAND(ID_THREAD_CLEAR, wxEVT_COMMAND_TEXT_UPDATED, ScriptEdit::OnThreadUpdated)
    EVT_COMMAND(ID_THREAD_PRINT, wxEVT_COMMAND_TEXT_UPDATED, ScriptEdit::OnThreadUpdated)
    EVT_COMMAND(ID_THREAD_PRINTERR, wxEVT_COMMAND_TEXT_UPDATED, ScriptEdit::OnThreadUpdated)
    EVT_COMMAND(ID_THREAD_CURLINE, wxEVT_COMMAND_TEXT_UPDATED, ScriptEdit::OnThreadUpdated)
    EVT_COMMAND(ID_THREAD_BREAKPOINT, wxEVT_COMMAND_TEXT_UPDATED, ScriptEdit::OnThreadUpdated)
    EVT_COMMAND(ID_THREAD_STACK, wxEVT_COMMAND_TEXT_UPDATED, ScriptEdit::OnThreadUpdated)
    EVT_COMMAND(ID_THREAD_FINISH, wxEVT_COMMAND_TEXT_UPDATED, ScriptEdit::OnThreadUpdated)

    EVT_TIMER(TIMER_ID_SCRIPTEDIT, ScriptEdit::OnTimer)

    EVT_TIMER(TIMER_ID_SCRIPTEDIT_RELOAD, ScriptEdit::OnTimerReload)

    EVT_MOUSEWHEEL(ScriptEdit::OnMouseWheel)

    EVT_DROP_FILES(ScriptEdit::OnDropFiles)

END_EVENT_TABLE()

ScriptEdit::ScriptEdit(wxWindow *parentT, wxWindowID idT,
                       const wxPoint &posT, const wxSize &sizeT, long styleT,
                       const wxString &strName,
                       int iLexer /* = wxSTC_LEX_NONE*/)
    : CodeEdit(parentT, idT, posT, sizeT, styleT, strName, iLexer)
{
    m_Id = ScriptEdit::EDITID;
    ++(ScriptEdit::EDITID);

    m_strFilename = wxEmptyString;
    m_ChangeTime.ResetTime();
    m_bLoading = false;
    m_bAutoReload = false;
    m_pReloadTimer = NULL;

    m_bLinePrev = false;
    m_iLinePrev = -1;

    if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_bDebugging = false;
        m_bDebugIt = false;
        m_bStopIt = false;
        m_pMutex->Unlock();
    }

    m_MarkerMargin = 14;
    m_FoldingMargin = 12;
    m_StatusMargin = 6;

    m_iMaxLineWidth = 0;

    m_iBookmarkCount = 0;
    m_iBreakpointCount = 0;

    m_strDebugTrace = wxEmptyString;

    m_strErrMsg = wxEmptyString;

    m_iFindMarkerCount = 0;
    m_iFindIndicCount = 0;

    m_bLexerEnforced = false;

    m_pProcess = NULL;
    m_iProcessId = -1L;
    m_pProcessTimer = NULL;

    for (int ii = 0; ii <= UTF8FROM_LAST; ii++) {
        m_bUTF8done[ii] = false;
    }

    m_iErrLine = -1;
    m_iStatusLine = -1;

    m_iDebugCount = 0;
    m_bStepInto = false;

    m_pCodeAnalyzer = new (std::nothrow) CodeAnalyzer();
    m_bEnableCodeAnalyzer = true;
    m_strSelectedFunc = wxEmptyString;
    m_strFileToOpen = wxEmptyString;

    // Accelerators
    const int nCount = 17;
    wxAcceleratorEntry accelEntries[nCount];
    accelEntries[0].Set(wxACCEL_CTRL, (int)'X', wxID_CUT);
    accelEntries[1].Set(wxACCEL_CTRL, (int)'C', wxID_COPY);
    accelEntries[2].Set(wxACCEL_CTRL, (int)'V', wxID_PASTE);
    accelEntries[3].Set(wxACCEL_CTRL, (int)'A', wxID_SELECTALL);
    accelEntries[4].Set(wxACCEL_CTRL, (int)'Y', ID_SCRIPT_REDO);
    accelEntries[5].Set(wxACCEL_CTRL, (int)'Z', ID_SCRIPT_UNDO);
    accelEntries[6].Set(wxACCEL_NORMAL, WXK_DELETE, wxID_CLEAR);
    accelEntries[7].Set(wxACCEL_NORMAL, WXK_NUMPAD_DELETE, wxID_CLEAR);
    accelEntries[8].Set(wxACCEL_CTRL, (int)'B', ID_SCRIPT_BREAKPOINT_MOD);
    accelEntries[9].Set(wxACCEL_CTRL, (int)'K', ID_SCRIPT_BOOKMARK_MOD);
    accelEntries[10].Set(wxACCEL_NORMAL, WXK_F2, ID_SCRIPT_BOOKMARK_NEXT);
    accelEntries[11].Set(wxACCEL_SHIFT, WXK_F2, ID_SCRIPT_BOOKMARK_PREV);

    // LaTeX stuff
    accelEntries[12].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'B', ID_LATEX_TEXTBF);
    accelEntries[13].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'I', ID_LATEX_TEXTIT);
    accelEntries[14].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'E', ID_LATEX_EMPH);
    accelEntries[15].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'N', ID_LATEX_TEXTNORMAL);
    accelEntries[16].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'M', ID_LATEX_MATHRM);
    //

    wxAcceleratorTable accelTable(nCount, accelEntries);
    SetAcceleratorTable(accelTable);
    //

    m_pInfobar = NULL;
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame) {
        m_pInfobar = pFrame->getInfobarMain();
    }

    m_bFindin = false;
    m_bPinned = false;

    m_bCanSetFocus = true;
}

ScriptEdit::~ScriptEdit()
{
    if (m_pMutex) {
        delete m_pMutex;
        m_pMutex = NULL;
    }

    resetTimerReload();

    processKill();

    if (m_pCodeAnalyzer) {
        delete m_pCodeAnalyzer;
        m_pCodeAnalyzer = NULL;
        m_bEnableCodeAnalyzer = false;
    }
}

bool ScriptEdit::setPrefs(const ScintillaPrefs &tPrefs, bool bInit, bool bInitIndent)
{

    bool useTab = m_ScintillaPrefs.common.useTab;
    int tabSize = m_ScintillaPrefs.common.tabSize;

    bool bRet = CodeEdit::setPrefs(tPrefs, false);

    if (false == bInitIndent) {
        m_ScintillaPrefs.common.useTab = useTab;
        m_ScintillaPrefs.common.tabSize = tabSize;
    }

    if (bInit) {

        CodeEdit::initPrefs();

        SetMarginLeft(6);
        SetMarginRight(6);

        // Line Number
        calcLinenumberMargin();

        // Status (character) margin
        SetMarginMask(m_StatusID, SCRIPT_MASK_SAVED | SCRIPT_MASK_MODIFIED | SCRIPT_MASK_FIND | SCRIPT_MASK_ERROR | SCRIPT_MASK_DEBUG);
        SetMarginType(m_StatusID, wxSTC_MARGIN_SYMBOL);
        SetMarginWidth(m_StatusID, m_StatusMargin);
        SetMarginSensitive(m_StatusID, false);

        unsigned long lBack = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
        unsigned long lFore = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
        wxColor clrBack = wxColor(lBack);
        wxColor clrFore = wxColor(lFore);

        MarkerDefine(SCRIPT_MASK_ERRORBIT, wxSTC_MARK_CHARACTER + '!', clrFore, clrBack);
        MarkerDefine(SCRIPT_MASK_FINDBIT, wxSTC_MARK_CHARACTER + 'f', clrFore, clrBack);
        MarkerDefine(SCRIPT_MASK_MODIFIEDBIT, wxSTC_MARK_CHARACTER + 'm', clrFore, clrBack);
        MarkerDefine(SCRIPT_MASK_SAVEDBIT, wxSTC_MARK_CHARACTER + 's', clrFore, clrBack);
        MarkerDefineBitmap(SCRIPT_MASK_DEBUGBIT, wxBitmap(startedx_small_xpm));

        this->setMarkersColors(m_ScintillaPrefs.common.marckerColorModified,
                               m_ScintillaPrefs.common.marckerColorSaved,
                               m_ScintillaPrefs.common.marckerColorFind,
                               m_ScintillaPrefs.common.marckerColorError);

        IndicatorSetStyle(FIND_INDICATOR, wxSTC_INDIC_ROUNDBOX);
        IndicatorSetForeground(FIND_INDICATOR, wxColour(m_ScintillaPrefs.common.marckerColorFind));

        SetCaretLineVisible(true);
    }

    return bRet;
}

void ScriptEdit::OnMouseWheel(wxMouseEvent &tEvent)
{
    if (tEvent.ControlDown()) {
        if (tEvent.GetWheelRotation() < 0) {
            DoZoomOut();
        }
        else {
            DoZoomIn();
        }
        return;
    }

    CodeEdit::OnMouseWheel(tEvent);
}

void ScriptEdit::enableStepInto(bool bStepInto)
{
    m_bStepInto = bStepInto;

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    if (isDebugging()) {
        wxFileName fname = this->GetFilename();
        wxString strT = uT("'");
        strT += fname.GetFullName();
#ifdef __WXMSW__
        strT += wxString::Format(uT("' debugging (%s)... step %d"), stepInto() ? uT("step into") : uT("step over"), m_iDebugCount);
#else
        strT += wxString::Format(uT("' debugging (%ls)... step %d"), stepInto() ? uT("step into") : uT("step over"), m_iDebugCount);
#endif
        pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_NONE);
    }
}

void ScriptEdit::OnThreadUpdated(wxCommandEvent &tEvent)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    int idT = tEvent.GetId();
    if (idT == ID_THREAD_CLEAR) {
        pFrame->ClearOutputWindow(GetFilename(), true);
    }

    else if (idT == ID_THREAD_PRINT) {
        pFrame->Output(tEvent.GetString());
    }

    else if (idT == ID_THREAD_PRINTERR) {
        int iErrLine = tEvent.GetInt();
        if (pFrame->isOutputLineEmpty()) {
            m_strErrMsg = tEvent.GetString();
        }
        else {
            m_strErrMsg = uT("\n");
            m_strErrMsg += tEvent.GetString();
        }

        if (iErrLine >= 0) {
            DoAddStatus(iErrLine, SCRIPT_MASK_ERRORBIT);

            // :LUAJIT: unimplemented functions (new in Lua 5.3)
            wxString strLine = this->GetLine(iErrLine);
            if (strLine.IsEmpty() == false) {
                wxString strNotImpl = wxEmptyString;
                if (strLine.Contains(uT("table.move"))) {
                    strNotImpl = uT("table.move");
                }
                else if (strLine.Contains(uT("string.pack"))) {
                    strNotImpl = uT("string.pack");
                }
                else if (strLine.Contains(uT("string.unpack"))) {
                    strNotImpl = uT("string.unpack");
                }
                else if (strLine.Contains(uT("string.packsize"))) {
                    strNotImpl = uT("string.packsize");
                }
                else if (strLine.Contains(uT("package.searchpath"))) {
                    strNotImpl = uT("package.searchpath");
                }
                if (strNotImpl.IsEmpty() == false) {
                    strNotImpl += uT(" is not implemented in this Lua engine");
                    m_strErrMsg += uT("\n");
                    m_strErrMsg += strNotImpl;
                }
            }
            //
        }

        pFrame->Output(m_strErrMsg);
    }

    else if (idT == ID_THREAD_CURLINE) {
        m_iRunningLine = tEvent.GetInt();
    }

    else if (idT == ID_THREAD_BREAKPOINT) {
        EnsureVisible(tEvent.GetInt());
        this->DoAddStatus(tEvent.GetInt(), SCRIPT_MASK_DEBUGBIT);
    }

    else if (idT == ID_THREAD_STACK) {
        if (isDebugging()) {
            m_iDebugCount += 1;
            wxFileName fname = this->GetFilename();
            wxString strT = uT("'");
            strT += fname.GetFullName();
#ifdef __WXMSW__
            strT += wxString::Format(uT("' debugging (%s)... step %d"), stepInto() ? uT("step into") : uT("step over"), m_iDebugCount);
#else
            strT += wxString::Format(uT("' debugging (%ls)... step %d"), stepInto() ? uT("step into") : uT("step over"), m_iDebugCount);
#endif
            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_NONE);
        }
        m_strDebugTrace = tEvent.GetString();
        pFrame->UpdateDebugNotebook(this->GetFilename(), m_strDebugTrace);
    }

    else if (idT == ID_THREAD_FINISH) {
        bool wasDebugging = m_bDebugging;
        setRunning(false);
        MarkerDelete(m_iStatusLine, SCRIPT_MASK_DEBUGBIT);
        wxString strT = tEvent.GetString();
        if (strT.IsEmpty() == false) {
            pFrame->Output(strT);
        }
        if (wasDebugging == false) {
            wxFileName fname = this->GetFilename();
            strT = uT("'");
            strT += fname.GetFullName();
            strT += uT("'");
            if (m_iErrLine >= 0) {
                strT += wxString::Format(uT(" executed with error at line %d"), m_iErrLine + 1);
            }
            else {
                strT += uT(" executed");
            }
            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        }
        else {
            pFrame->OutputStatusbar(uT("Ready"), SIGMAFRAME_TIMER_NONE);
        }
    }
}

void ScriptEdit::OnSize(wxSizeEvent &tEvent)
{
    DoCallTipCancel();
    tEvent.Skip();
}

bool ScriptEdit::DoReload(bool bSilent, bool bUserAction)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return false;
    }

    if (false == pFrame->detectChangeOutside()) {
        return false;
    }

    wxFileName fname = this->GetFilename();
    if (false == fname.FileExists()) {
        return false;
    }

    int reloadDelay = m_ScintillaPrefs.common.reloadDelay;
    if (autoReload() && m_pReloadTimer) {
        m_pReloadTimer->Stop();
    }

    wxDateTime changeTime;
    changeTime.ResetTime();
    fname.GetTimes(NULL, &changeTime, NULL);

    if (changeTime.IsLaterThan(m_ChangeTime)) {

        int iret = (autoReload() || bSilent) ? wxYES : wxNO;
        if (wxNO == iret) {
            wxString strLabel1 = uT("Reload");
            wxString strLabel2 = uT("Don't reload");
            wxString strLabel3 = uT("Always reload");
            wxString strT = uT("'");
            strT += CometFrame::getLabelFromPath(this->GetFilename());
            strT += uT("'\nmodified outside Comet.");
            strT += uT(" Reload it?");
            long iStyle = wxYES | wxNO | wxCANCEL | wxICON_QUESTION;
            if (this->isModified()) {
                iStyle |= wxNO_DEFAULT;
            }
            else {
                iStyle |= wxYES_DEFAULT;
            }
            iret = SigmaMessageBox(strT, uT("Comet"), iStyle, pFrame, strLabel1, strLabel2, strLabel3);
            if (iret == wxCANCEL) {
                try {
                    m_pReloadTimer = new wxTimer(this, TIMER_ID_SCRIPTEDIT_RELOAD);
                }
                catch (...) {
                    m_pReloadTimer = NULL;
                }
                if (m_pReloadTimer) {
                    m_bAutoReload = true;
                    iret = wxYES;
                    if (m_ScintillaPrefs.common.reloadDelay < 3) {
                        m_ScintillaPrefs.common.reloadDelay = 3;
                    }
                    else if (m_ScintillaPrefs.common.reloadDelay > 3600) {
                        m_ScintillaPrefs.common.reloadDelay = 3600;
                    }
                    m_pReloadTimer->Start(m_ScintillaPrefs.common.reloadDelay * 1000);
                }
            }
        }

        if (iret == wxYES) {
            time_t prevT = time(NULL);
            if (DoLoadFile(fname.GetFullPath(), false, true, false, false == autoReload())) {
                if (autoReload()) {
                    this->DoGotoLine(this->GetLineCount() - 1, false);
                    if (m_pReloadTimer) {
                        m_pReloadTimer->Start(m_ScintillaPrefs.common.reloadDelay * 1000);
                    }
                }
                int deltaT = (int)floor(difftime(time(NULL), prevT));
                if (reloadDelay < (deltaT * 2)) {
                    reloadDelay = deltaT * 2;
                }
                pFrame->OutputStatusbar(fname.GetFullName() + uT(" reloaded"), SIGMAFRAME_TIMER_SHORT);
                return true;
            }
        }
    }
    else {
        if (autoReload()) {
            // if no change after a "long" time, disable auto-reload
            wxDateTime timeNow, timePast;
            timeNow = wxDateTime::Now();
            timePast = timeNow;
            int iSec = m_ScintillaPrefs.common.reloadDelay * 12;
            int iH = iSec / 3600;
            int iM = (iSec / 60) % 60;
            int iS = iSec % 60;
            wxTimeSpan dt(iH, iM, iS, 0);
            timePast.Subtract(dt);
            if (changeTime.IsEarlierThan(timePast)) {
                resetTimerReload();
                wxString strT = uT("'");
                strT += CometFrame::getLabelFromPath(this->GetFilename());
                strT += uT("' auto reload ended.");
                pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
                pFrame->updateEditorStatus(this);
            }
        }
        else if (bUserAction) {
            wxString strT = uT("'");
            strT += CometFrame::getLabelFromPath(this->GetFilename());
            strT += uT("' not modified.");
            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        }
    }

    if (autoReload() && m_pReloadTimer) {
        m_pReloadTimer->Start(reloadDelay * 1000);
    }

    return false;
}

void ScriptEdit::OnSetFocus(wxFocusEvent &tEvent)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    if (pFrame->isOpeningFile() || pFrame->isClosingAll()) {
        tEvent.Skip();
        m_bFocused = true;
        return;
    }

    CodeEdit::OnSetFocus(tEvent);

    pFrame->UpdateDebugNotebook(this->GetFilename(), m_strDebugTrace);
}

void ScriptEdit::updatePage(const wxString &strTitle)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    wxAuiNotebook *pNotebook = (wxAuiNotebook *)(GetParent());
    int indexT = pNotebook->GetPageIndex(this);
    if (wxNOT_FOUND == indexT) {
        // should never happen
        return;
    }
    else {
        pNotebook->SetPageText(indexT, strTitle);
    }
    pNotebook->SetPageToolTip(indexT, uT(""));
    pNotebook->SetSelection(indexT);
}

void ScriptEdit::OnMouseLeftUp(wxMouseEvent &tEvent)
{
    CodeEdit::OnMouseLeftUp(tEvent);
}

void ScriptEdit::DoEditRedo(void)
{
    if (!CanRedo()) {
        return;
    }

    Redo();
}

void ScriptEdit::DoEditUndo(void)
{
    if (!CanUndo()) {
        return;
    }

    Undo();
}

void ScriptEdit::DoEditDelete(void)
{
    if (GetReadOnly()) {
        return;
    }
    Clear();
}

void ScriptEdit::DoEditCut(void)
{
    if (GetReadOnly() || ((GetSelectionEnd() - GetSelectionStart()) <= 0)) {
        return;
    }

    Cut();
}

void ScriptEdit::DoEditCopy(void)
{
    if ((GetSelectionEnd() - GetSelectionStart()) <= 0) {
        return;
    }

    Copy();
}

void ScriptEdit::DoEditPaste(void)
{
    if (!CanPaste()) {
        return;
    }

    Paste();
}

void ScriptEdit::DoGoto(void)
{
    int iCount = this->GetLineCount();
    if (iCount < 3) {
        return;
    }
    wxString strL = wxString::Format(uT("Goto line number (1 - %d):"), iCount);
    wxString strT = wxString::Format(uT("%d"), GetCurrentLine() + 1);
    wxTextEntryDialog *pDlg = new (std::nothrow) wxTextEntryDialog(this, strL, uT("Comet"), strT, wxOK | wxCANCEL, wxDefaultPosition, wxSize(160, wxDefaultCoord));
    if (pDlg == NULL) {
        return;
    }
    pDlg->SetTextValidator(wxFILTER_NUMERIC);
    strT = wxString::Format(uT("%d"), iCount + 1);
    unsigned long iLen = static_cast<unsigned long>(strT.Length());
    if (iLen < 2UL) {
        iLen = 2UL;
    }
    pDlg->SetMaxLength(iLen);

    long iRet = pDlg->ShowModal();
    strT = pDlg->GetValue();
    pDlg->Destroy();

    if (iRet == wxID_OK) {
        if (strT.IsNumber()) {
            long iLine = 0;
            strT.ToLong(&iLine, 10);
            DoGotoLine((int)(iLine - 1));
            updateStatusbar();
        }
    }
}

void ScriptEdit::DoEditIndentRed(void)
{
    CmdKeyExecute(wxSTC_CMD_DELETEBACK);
}

void ScriptEdit::DoEditInsertText(const wxString &strT, int iLen /* = -1*/)
{
    int iStart = GetSelectionStart();
    int iEnd = GetSelectionEnd();
    if (iStart == iEnd) {
        InsertText(iStart, strT);
        if (iLen > 0) {
            SetSelection(iStart + iLen, iStart + iLen);
        }
    }
    else {
        ReplaceSelection(strT);
    }
}

void ScriptEdit::DoEditInsertDate(void)
{
    wxDateTime dt = wxDateTime::Now();
    DoEditInsertText(dt.Format(uT("%Y-%m-%d")), 10);
}

void ScriptEdit::DoEditInsertTime(void)
{
    wxDateTime dt = wxDateTime::Now();
    DoEditInsertText(dt.Format(uT("%H:%M:%S")), 8);
}

void ScriptEdit::DoEditInsertDateTime(void)
{
    wxDateTime dt = wxDateTime::Now();
    DoEditInsertText(dt.Format(uT("%Y-%m-%d %H:%M:%S")), 19);
}

int ScriptEdit::CodeAnalyzerGetLine(int ii)
{
    if ((m_bEnableCodeAnalyzer == false) || (m_pCodeAnalyzer == NULL)) {
        return -1;
    }

    return m_pCodeAnalyzer->getLine(ii);
}

int ScriptEdit::CodeAnalyzerGetLine(const char_t *funcName)
{
    if ((m_bEnableCodeAnalyzer == false) || (m_pCodeAnalyzer == NULL)) {
        return -1;
    }

    return m_pCodeAnalyzer->getLine(funcName);
}

void ScriptEdit::CodeAnalyzerSetLine(const char_t *funcName, int iLine)
{
    if ((m_bEnableCodeAnalyzer == false) || (m_pCodeAnalyzer == NULL)) {
        return;
    }

    m_pCodeAnalyzer->setLine(funcName, iLine);
}

int ScriptEdit::CodeAnalyzerGetIndex(const char_t *funcName, bool bComplete /* = true*/)
{
    if ((m_bEnableCodeAnalyzer == false) || (m_pCodeAnalyzer == NULL)) {
        return -1;
    }

    return m_pCodeAnalyzer->getIndex(funcName, bComplete);
}

wxString ScriptEdit::CodeAnalyzerGetName(int ii)
{
    if ((m_bEnableCodeAnalyzer == false) || (m_pCodeAnalyzer == NULL)) {
        return wxEmptyString;
    }

    return wxString(m_pCodeAnalyzer->getName(ii));
}

AnalyzerElement::SCOPE ScriptEdit::CodeAnalyzerGetScope(int ii)
{
    if ((m_bEnableCodeAnalyzer == false) || (m_pCodeAnalyzer == NULL)) {
        return AnalyzerElement::SCOPE_UNKNOWN;
    }

    return m_pCodeAnalyzer->getScope(ii);
}

int ScriptEdit::CodeAnalyzerGetCount(void)
{
    if ((m_bEnableCodeAnalyzer == false) || (m_pCodeAnalyzer == NULL)) {
        return 0;
    }

    return m_pCodeAnalyzer->getCount();
}

void ScriptEdit::updateFindList(wxComboBox *pCombo, int nFunc)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    if (pCombo == NULL) {
        return;
    }

    wxString strT, strT0;

    int ll = pFrame->getFindCount();
    if (ll > 0) {
        if (nFunc > 0) {
            pCombo->AppendString(wxEmptyString);
        }

        for (int ii = 0; ii < ll; ii++) {
            strT = pFrame->getFindItem(ii);
            strT = SigmaTrim(strT);
            if (CodeAnalyzer::isSeparator(strT) == false) {
                pCombo->AppendString(strT);
            }
        }
    }
}

int ScriptEdit::CodeAnalyzerUpdate(wxComboBox *pCombo, bool bRecreate)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return 0;
    }

    if (pCombo == NULL) {
        return 0;
    }

    pCombo->SetValue(wxEmptyString);
    pCombo->Clear();

    if ((m_bEnableCodeAnalyzer == false) || (m_pCodeAnalyzer == NULL)) {
        updateFindList(pCombo, 0);
        return 0;
    }

    if (bRecreate) {
        if (this->isModified()) {
            wxString strFilename = this->GetFilename();
            if (this->DoSaveFile(strFilename) == false) {
                updateFindList(pCombo, 0);
                return 0;
            }
        }

        SigmaBusyCursor waitC;
        if (this->GetLineCount() > 2048) {
            waitC.start();
        }
        m_pCodeAnalyzer->setFilename(this->GetFilename());
        if (m_pCodeAnalyzer->analyze() == false) {
            updateFindList(pCombo, 0);
            return 0;
        }
    }

    int nn = m_pCodeAnalyzer->getCount();

    if (nn > 0) {

        wxString strT;

        wxArrayString arrT;

        for (int ii = 0; ii < nn; ii++) {

            strT = uT("");
            if (this->GetLexer() == wxSTC_LEX_LUA) {
                if (m_pCodeAnalyzer->getScope(ii) == AnalyzerElement::SCOPE_LOCAL) {
                    strT = uT("local ");
                }
            }
            strT += m_pCodeAnalyzer->getName(ii);

            arrT.Add(strT);
        }

        arrT.Sort(false);
        for (int ii = 0; ii < nn; ii++) {
            pCombo->AppendString(arrT[ii]);
        }
    }

    updateFindList(pCombo, nn);

    return nn;
}

void ScriptEdit::DoEditSelectAll(void)
{
    SetSelection(0, GetTextLength());
}

void ScriptEdit::DoEditSelectLine(void)
{
    int currentLine = GetCurrentLine();
    if ((currentLine < 0) || (currentLine >= GetLineCount())) {
        return;
    }
    wxString strLine = GetLine(currentLine);
    if (strLine.IsEmpty()) {
        return;
    }
    int lineStart = PositionFromLine(currentLine);
    int lineEnd = (int)(lineStart + strLine.Length());
    if (lineEnd > lineStart) {
        SetSelection(lineStart, lineEnd);
    }
}

void ScriptEdit::DoEditSelectWord(void)
{
    int iSelStart = this->GetSelectionStart();
    int iSelEnd = this->GetSelectionEnd();
    if (iSelEnd == iSelStart) {
        int iStart = WordStartPosition(iSelStart, true);
        int iEnd = WordEndPosition(iSelStart, true);
        SetSelection(iStart, iEnd);
    }
}

void ScriptEdit::DoLineNumber(bool bEnable)
{
    m_ScintillaPrefs.common.lineNumberEnable = bEnable;
    calcLinenumberMargin(true);
}

void ScriptEdit::DoFoldEnable(bool bEnable)
{
    m_ScintillaPrefs.common.foldEnable = bEnable;

    ShowLines(0, GetLineCount() - 1);
    SetFoldFlags(m_ScintillaPrefs.common.foldEnable ? wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED : 0);
    SetMarginWidth(m_FoldingID, (m_ScintillaPrefs.common.foldEnable) ? m_FoldingMargin : 0);
    SetMarginSensitive(m_FoldingID, m_ScintillaPrefs.common.foldEnable);
}

void ScriptEdit::DoWrapMode(bool bEnable)
{
    m_ScintillaPrefs.common.wrapModeInitial = bEnable;
    SetWrapMode(m_ScintillaPrefs.common.wrapModeInitial ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE);
}

void ScriptEdit::DoConvertEOL(int idT)
{
    int eolMode = GetEOLMode();
    switch (idT) {
        case ID_SCRIPT_CONVERTCR:
        {
            eolMode = wxSTC_EOL_CR;
            break;
        }
        case ID_SCRIPT_CONVERTCRLF:
        {
            eolMode = wxSTC_EOL_CRLF;
            break;
        }
        case ID_SCRIPT_CONVERTLF:
        {
            eolMode = wxSTC_EOL_LF;
            break;
        }
    }
    ConvertEOLs(eolMode);
    SetEOLMode(eolMode);
}

void ScriptEdit::DoEnableEOL(bool bEnable)
{
    m_ScintillaPrefs.common.displayEOLEnable = bEnable;

    SetViewEOL(m_ScintillaPrefs.common.displayEOLEnable);
}

void ScriptEdit::DoEnableWhiteSpace(bool bEnable)
{
    m_ScintillaPrefs.common.whiteSpaceEnable = bEnable;

    SetViewWhiteSpace(m_ScintillaPrefs.common.whiteSpaceEnable ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE);
}

void ScriptEdit::DoEnableLongLine(bool bEnable)
{
    m_ScintillaPrefs.common.longLineOnEnable = bEnable;

    SetEdgeMode(m_ScintillaPrefs.common.longLineOnEnable ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE);
}

void ScriptEdit::DoSetLongLine(int iLen)
{
    if ((iLen >= 10) && (iLen <= 1024)) {
        m_ScintillaPrefs.common.longLine = iLen;
        SetEdgeColumn(m_ScintillaPrefs.common.longLine);
    }
}

void ScriptEdit::OnMarginClick(wxStyledTextEvent &tEvent)
{
    if (tEvent.GetMargin() == m_MarkerID) {

        wxPoint ptMousePosition = ::wxGetMousePosition();
        ptMousePosition = this->ScreenToClient(ptMousePosition);
        const int iPos = this->PositionFromPoint(ptMousePosition);
        if (iPos < 0) {
            return;
        }

        const int iLine = this->LineFromPosition(iPos);

        if ((MarkerGet(iLine) & SCRIPT_MASK_ERROR) && (m_strErrMsg.IsEmpty() == false)) {
            wxString strMsg;
            if (m_strErrMsg.StartsWith(uT("\n"))) {
                strMsg = m_strErrMsg.Mid(1);
            }
            else {
                strMsg = m_strErrMsg;
            }
            CallTipShow(iPos, strMsg);
            return;
        }

        DoModifyMarker(SCRIPT_MASK_BOOKMARKBIT, iPos);
    }
    else if (tEvent.GetMargin() == m_FoldingID) {
        DoModifyFolding(tEvent.GetPosition());
    }
}

void ScriptEdit::calcLinenumberMargin(bool bRecalc /* = false*/)
{
    if (m_ScintillaPrefs.common.lineNumberEnable == false) {
        SetMarginWidth(m_LinenumberID, 0);
        return;
    }

    int iCount = GetLineCount();

    if ((bRecalc == false) && (iCount <= 999)) {
        return;
    }

    if (iCount <= 999) {
        SetMarginWidth(m_LinenumberID, TextWidth(wxSTC_STYLE_LINENUMBER, uT("-0000")));
        return;
    }

    char_t szBuffer[LM_STRSIZET];
    memset(szBuffer, 0, LM_STRSIZET * sizeof(char_t));
    int iT = (int)floor(log10(fabs((double)(iCount))));
    iT += 1;
    if ((iT > 3) && (iT < (LM_STRSIZET - 1))) {
        szBuffer[0] = uT('-');
        szBuffer[1] = uT('0');
        for (int ii = 2; ii <= iT; ii++) {
            szBuffer[ii] = uT('0');
        }
        SetMarginWidth(m_LinenumberID, TextWidth(wxSTC_STYLE_LINENUMBER, (const char_t *)szBuffer));
    }
}

void ScriptEdit::OnCharAdded(wxStyledTextEvent &tEvent)
{
    int currentLine = GetCurrentLine();
    if ((currentLine < 0) || (currentLine >= GetLineCount())) {
        return;
    }

    // remove indicator, if any
    int iCurrentPos = GetCurrentPos();
    if (this->GetStyleAt(iCurrentPos) & wxSTC_INDIC2_MASK) {
        int iStart = iCurrentPos, iEnd = iCurrentPos;
        const int iLineEnd = this->GetLineEndPosition(currentLine);
        const int iLineStart = (iLineEnd - (int)(this->GetLine(currentLine).Length()));
        if (iCurrentPos < (iLineStart + 2)) {
            iStart = iLineStart;
        }
        else {
            for (int ii = iCurrentPos - 2; ii >= iLineStart; ii--) {
                if ((this->GetStyleAt(ii) & wxSTC_INDIC2_MASK) == 0) {
                    iStart = ii + 1;
                    break;
                }
            }
        }
        if (iCurrentPos > (iLineEnd - 2)) {
            iEnd = iLineEnd;
        }
        else {
            for (int ii = iCurrentPos + 2; ii <= iLineEnd; ii++) {
                if ((this->GetStyleAt(ii) & wxSTC_INDIC2_MASK) == 0) {
                    iEnd = ii - 1;
                    break;
                }
            }
        }
        StartStyling(iStart, wxSTC_INDIC2_MASK);
        SetStyling((iEnd - iStart + 1), 0);
    }
    //

    CodeEdit::OnCharAdded(tEvent);

    if (m_bWordEnclosed) {
        m_bWordEnclosed = false;
        return;
    }

    char cT = (char)(tEvent.GetKey());

    bool bAutoCompActive = AutoCompActive();
    bool bCallTipActive = CallTipActive();

    if ((cT == ('(')) && (bAutoCompActive == false) && (bCallTipActive == false) && ((m_iLexer == wxSTC_LEX_LUA) || (m_iLexer == wxSTC_LEX_PYTHON))) {

        const int nStartSel = GetSelectionStart();
        const int nEndSel = GetSelectionEnd();
        char cTp = (char)GetCharAt(nStartSel - 2);

        if ((nStartSel == nEndSel) && (nStartSel > 2) && (isalnum(cTp) || (cTp == ('_')))) {

            int nWordStart = nStartSel;
            int nWordEnd = nStartSel - 1;
            for (int ii = nWordEnd - 1; ii >= (nWordEnd - LM_STRSIZEN); ii--) {
                cTp = (char)GetCharAt(ii);
                if (!isalnum(cTp) && (cTp != ('_'))) {
                    nWordStart = ii + 1;
                    break;
                }
            }

            if (nWordEnd > (nWordStart + 2)) {
                wxString strN = GetTextRange(nWordStart, nWordEnd);
                int iLen = (int)(strN.Length());
                if ((iLen > 2) && (iLen <= LM_STRSIZEN)) {
                    wxString strF = strN + uT("(");
                    int indexT = this->CodeAnalyzerGetIndex(LM_CSTR(strF), false);
                    if ((indexT >= 0) && (indexT < this->CodeAnalyzerGetCount())) {
                        int iLineF = this->CodeAnalyzerGetLine(indexT);
                        if (currentLine != iLineF) {
                            strF = this->CodeAnalyzerGetName(indexT);
                            CallTipShow(nStartSel, strF);
                            return;
                        }
                    }
                }
            }
        }
    }

    wxString strT = this->GetLine(currentLine);
    int iW = TextWidth(wxSTC_STYLE_DEFAULT, strT);

    if (iW > m_iMaxLineWidth) {
        m_iMaxLineWidth = iW;
        SetScrollWidth(m_iMaxLineWidth);
    }

    if (cT == LM_NEWLINE_CHARA) {
        int lineInd = 0;
        if (currentLine > 0) {
            lineInd = this->GetLineIndentation(currentLine - 1);
            int iLexer = this->GetLexer();
            if ((iLexer == wxSTC_LEX_LUA) || (iLexer == wxSTC_LEX_CPP) || (iLexer == wxSTC_LEX_PYTHON) || (iLexer == wxSTC_LEX_OCTAVE) || (iLexer == wxSTC_LEX_LATEX) || ((iLexer == wxSTC_LEX_SOLIS) && (LEXERTYPE_FIRST == m_iLexerType))) {
                wxString strLine = this->GetLine(currentLine - 1).Trim(true);

                if (strLine.IsEmpty() == false) {
                    if (iLexer == wxSTC_LEX_LUA) {
                        if (strLine.EndsWith(uT("then")) || strLine.EndsWith(uT("else")) || strLine.EndsWith(uT("do")) || (strLine.Find(uT("function")) == 0)) {
                            lineInd += this->GetIndent();
                        }
                    }
                    else if (iLexer == wxSTC_LEX_CPP) {
                        if (strLine.EndsWith(uT("{"))) {
                            lineInd += this->GetIndent();
                        }
                    }
                    else if (iLexer == wxSTC_LEX_PYTHON) {
                        if (strLine.EndsWith(uT(":"))) {
                            lineInd += this->GetIndent();
                        }
                    }
                    else if (iLexer == wxSTC_LEX_OCTAVE) {
                        if (strLine.EndsWith(uT("then")) || strLine.EndsWith(uT("else")) || strLine.EndsWith(uT("do")) || (strLine.Find(uT("function")) == 0)) {
                            lineInd += this->GetIndent();
                        }
                    }
                    else if (iLexer == wxSTC_LEX_LATEX) {
                        if (strLine.Find(uT("\\begin")) == 0) {
                            lineInd += this->GetIndent();
                        }
                    }
                    else if (iLexer == wxSTC_LEX_SOLIS) {
                        if (strLine.StartsWith(uT("[")) && strLine.EndsWith(uT("]"))) {
                            lineInd += this->GetIndent();
                        }
                    }
                }
            }
        }

        if (lineInd == 0) {
            return;
        }
        SetLineIndentation(currentLine, lineInd);
        GotoPos(GetLineIndentPosition(currentLine));
    }
}

void ScriptEdit::DoScriptAction(int idT)
{
    switch (idT) {
        case ID_SCRIPT_BOOKMARK_MOD:
            DoModifyMarker(SCRIPT_MASK_BOOKMARKBIT);
            break;
        case ID_SCRIPT_BOOKMARK_NEXT:
            DoFindNextBookmark();
            break;
        case ID_SCRIPT_BOOKMARK_PREV:
            DoFindPrevBookmark();
            break;
        case ID_SCRIPT_BOOKMARK_DELALL:
            DoDeleteAllBookmarks();
            break;
        case ID_SCRIPT_BREAKPOINT_MOD:
            DoModifyMarker(SCRIPT_MASK_BREAKPOINTBIT);
            break;
        case ID_SCRIPT_BREAKPOINT_DELALL:
            DoDeleteAllBreakpoints();
            break;
        case ID_SCRIPT_SELECTWORD:
            DoEditSelectWord();
            break;
        case ID_SCRIPT_SELECTLINE:
            DoEditSelectLine();
            break;
        case wxID_SELECTALL:
            DoEditSelectAll();
            break;
        case ID_SCRIPT_INSERT_DATE:
            DoEditInsertDate();
            break;
        case ID_SCRIPT_INSERT_TIME:
            DoEditInsertTime();
            break;
        case ID_SCRIPT_INSERT_DATETIME:
            DoEditInsertDateTime();
            break;
        case ID_SEARCH_GOTO:
            DoGoto();
            break;
        case ID_SCRIPT_CONVERTCR:
            DoConvertEOL(ID_SCRIPT_CONVERTCR);
            break;
        case ID_SCRIPT_CONVERTCRLF:
            DoConvertEOL(ID_SCRIPT_CONVERTCRLF);
            break;
        case ID_SCRIPT_CONVERTLF:
            DoConvertEOL(ID_SCRIPT_CONVERTLF);
            break;
        case ID_SCRIPT_ZOOMIN:
            DoZoomIn();
            break;
        case ID_SCRIPT_ZOOMOUT:
            DoZoomOut();
            break;
        case ID_SCRIPT_ZOOMNONE:
            DoZoomNone();
            break;
        default:
            break;
    }
}

void ScriptEdit::DoModifyFolding(int nPos /* = -1*/)
{
    int iLineCount = GetLineCount();
    if (iLineCount < 1) {
        return;
    }

    if (nPos < 0) {
        nPos = GetCurrentPos();
    }

    int nLine = LineFromPosition(nPos);
    if ((nLine < 0) || (nLine >= iLineCount)) {
        return;
    }

    ToggleFold(nLine);
}

void ScriptEdit::DoDeleteAllBreakpoints(void)
{
    MarkerDeleteAll(SCRIPT_MASK_BREAKPOINTBIT);

    m_iBreakpointCount = 0;
}

void ScriptEdit::DoShowStats(void)
{
    FileStatsDlg *pDlg = new FileStatsDlg(this);
    pDlg->ShowModal();
    pDlg->Destroy();
}

void ScriptEdit::DoUseTab(bool bEnable)
{
    m_ScintillaPrefs.common.useTab = bEnable;
    SetUseTabs(bEnable);
    SetTabIndents(bEnable);
    SetBackSpaceUnIndents(bEnable);
}

void ScriptEdit::DoIndentGuide(bool bEnable)
{
    SetIndentationGuides(bEnable);
}

void ScriptEdit::DoSetTabSize(int iTabSize)
{
    m_ScintillaPrefs.common.tabSize = iTabSize;
    SetTabWidth(iTabSize);
    SetIndent(m_ScintillaPrefs.common.indentEnable ? iTabSize : 0);
}

void ScriptEdit::DoFormatDoc(void)
{
    int iLineCount = GetLineCount();
    if (iLineCount <= 1) {
        return;
    }

    wxBusyCursor waitC;

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    bool bTabToSpaces = (m_ScintillaPrefs.common.useTab == false);

    int iTab = 0, iSpace = 0, iTabSize = GetTabWidth(), iTabSizeSp = 0;
    wxString strLineO, strLine, strLineTrimmed;
    wxString strText = uT("");
    const wxString strEOL((this->GetEOLMode() == wxSTC_EOL_LF) ? uT("\n") : ((this->GetEOLMode() == wxSTC_EOL_CR) ? uT("\r") : uT("\r\n")));
    char_t cT;
    int ii, jj, ll, nn = 0;

    if (iTabSize < 2) {
        iTabSize = 2;
    }

    iTabSizeSp = iTabSize;

    lmByte *pbMarkers = (lmByte *)malloc(iLineCount * sizeof(lmByte));

    int iMarkerCount = 0;

    pFrame->OutputStatusbar(uT("Formatting document..."), SIGMAFRAME_TIMER_SHORT);

    for (ii = 0; ii < iLineCount; ii++) {

        if (pbMarkers != NULL) {
            pbMarkers[ii] = 0x00;
            if (MarkerGet(ii) & SCRIPT_MASK_BOOKMARK) {
                pbMarkers[ii] |= 0x01;
                iMarkerCount += 1;
            }
            if (MarkerGet(ii) & SCRIPT_MASK_BREAKPOINT) {
                pbMarkers[ii] |= 0x02;
                iMarkerCount += 1;
            }
            if ((GetFoldLevel(ii) & wxSTC_FOLDLEVELHEADERFLAG) && (GetFoldExpanded(ii) == false)) {
                pbMarkers[ii] |= 0x04;
                iMarkerCount += 1;
            }
            if ((MarkerGet(ii) & SCRIPT_MASK_SAVED) != 0) {
                pbMarkers[ii] |= 0x08;
                iMarkerCount += 1;
            }
            if ((MarkerGet(ii) & SCRIPT_MASK_FIND) != 0) {
                pbMarkers[ii] |= 0x10;
                iMarkerCount += 1;
            }
        }

        strLineO = GetLine(ii);

        strLine = strLineO;
        strLineTrimmed = strLineO;
        strLineTrimmed.Trim(true);
        strLineTrimmed.Trim(false);
        if (strLineTrimmed.IsEmpty()) {
            strLine = strEOL;
            if (ii < (iLineCount - 1)) {
                strText += strLine;
                if (strLine.IsSameAs(strLineO) == false) {
                    nn += 1;
                }
            }
            continue;
        }

        cT = strLine.GetChar(0);

        if ((cT != uT(' ')) && (cT != uT('\t'))) {
            strLine = strLineTrimmed + strEOL;
            strText += strLine;
            if (strLine.IsSameAs(strLineO) == false) {
                nn += 1;
            }
            continue;
        }

        iTab = 0;
        iSpace = 0;
        for (jj = 0; jj < (int)(strLine.Length()); jj++) {
            cT = strLine.GetChar(jj);
            if ((cT != uT(' ')) && (cT != uT('\t'))) {
                break;
            }
            if (cT == uT(' ')) {
                iSpace += 1;
            }
            else if (cT == uT('\t')) {
                iTab += 1;
            }
        }

        if ((iSpace >= 1) && (iSpace < iTabSize)) {
            iTabSizeSp = iSpace;
        }

        if (bTabToSpaces) {
            strLine = uT("");

            if (iTab > 0) {
                for (ll = 0; ll < (iTab * iTabSize); ll++) {
                    strLine += uT(" ");
                }
                if (iSpace > 0) {
                    for (ll = 0; ll < iSpace; ll++) {
                        strLine += uT(" ");
                    }
                }
            }

            else if (iSpace > 0) {
                for (ll = 0; ll < iSpace; ll++) {
                    strLine += uT(" ");
                }
            }

            strLine += (strLineTrimmed + strEOL);
        }

        else {
            strLine = uT("");

            if (iSpace > 0) {
                if (iSpace >= iTabSizeSp) {
                    ll = iSpace / iTabSizeSp;
                    for (jj = 0; jj < ll; jj++) {
                        strLine += uT("\t");
                    }
                    if ((iSpace % iTabSizeSp) != 0) {
                        strLine += uT("\t");
                    }
                }
                else {
                    strLine += uT("\t");
                }

                if (iTab > 0) {
                    for (ll = 0; ll < iTab; ll++) {
                        strLine += uT("\t");
                    }
                }
            }

            else if (iTab > 0) {
                for (int ll = 0; ll < iTab; ll++) {
                    strLine += uT("\t");
                }
            }

            strLine += (strLineTrimmed + strEOL);
        }

        if (strLine.IsSameAs(strLineO) == false) {
            nn += 1;
        }

        strText += strLine;
    }

    if (nn > 0) {

        DoDeleteAllMarkers();

        SetText(strText);

        if ((pbMarkers != NULL) && (iMarkerCount > 0)) {
            for (ii = 0; ii < iLineCount; ii++) {
                if ((pbMarkers[ii] & 0x01) == 0x01) {
                    MarkerAdd(ii, SCRIPT_MASK_BOOKMARKBIT);
                    if (pFrame->canAddBookmark()) {
                        strLine = this->GetLine(ii).Trim(false);
                        pFrame->addBookmark(this->GetFilename(), ii, strLine, m_Id);
                    }
                    m_iBookmarkCount += 1;
                }
                if ((pbMarkers[ii] & 0x02) == 0x02) {
                    MarkerAdd(ii, SCRIPT_MASK_BREAKPOINTBIT);
                    m_iBreakpointCount += 1;
                }
                if ((pbMarkers[ii] & 0x04) == 0x04) {
                    ToggleFold(ii);
                }
                if ((pbMarkers[ii] & 0x08) == 0x08) {
                    MarkerAdd(ii, SCRIPT_MASK_SAVEDBIT);
                }
                if ((pbMarkers[ii] & 0x10) == 0x10) {
                    DoAddStatus(ii, SCRIPT_MASK_FINDBIT);
                }
            }
        }

        pFrame->OutputStatusbar(uT("Document formatting done"), SIGMAFRAME_TIMER_SHORT);
    }
    else {
        pFrame->OutputStatusbar(uT("Document formatting already done"), SIGMAFRAME_TIMER_SHORT);
    }

    if (pbMarkers != NULL) {
        free(pbMarkers);
        pbMarkers = NULL;
    }
}

void ScriptEdit::DoZoomIn(void)
{
    int iF = this->GetFont().GetPointSize();
    if (iF >= 96) {
        return;
    }
    ZoomIn();
    calcLinenumberMargin(true);
}

void ScriptEdit::DoZoomOut(void)
{
    int iF = this->GetFont().GetPointSize();
    if (iF <= 4) {
        return;
    }
    ZoomOut();
    calcLinenumberMargin(true);
}

void ScriptEdit::DoZoomNone(void)
{
    SetZoom(0);
    calcLinenumberMargin(true);
}

void ScriptEdit::CreateMenuFormat(wxMenu *menuFormat, bool bIncludeComment)
{
    if (GetReadOnly() || (isLexerKnown(GetLexer()) == false)) {
        return;
    }

    int iLexer = GetLexer();
    bool bTextSelected = ((GetSelectionEnd() - GetSelectionStart()) > 0);
    wxMenuItem *pItem;

    if (bTextSelected) {
        if ((iLexer != wxSTC_LEX_NULL) && bIncludeComment) {
            pItem = new wxMenuItem(menuFormat, ID_SCRIPT_COMMENTSELECTION, uT("Comment\tShift+Ctrl+C"), uT("Comment out the selected text (Shift+Ctrl+C)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
            pItem->SetBitmap(comment_small_xpm);
#endif
            menuFormat->Append(pItem);
            pItem = new wxMenuItem(menuFormat, ID_SCRIPT_UNCOMMENTSELECTION, uT("Uncomment\tShift+Ctrl+U"), uT("Uncomment the selected text (Shift+Ctrl+U)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
            pItem->SetBitmap(uncomment_small_xpm);
#endif
            menuFormat->Append(pItem);
            menuFormat->AppendSeparator();
        }

        if (GetLexer() == wxSTC_LEX_LATEX) {
            menuFormat->Append(ID_LATEX_TEXTBF, uT("Bold\tShift+Ctrl+B"), uT("Convert the selected text to bold"), wxITEM_NORMAL);
            menuFormat->Append(ID_LATEX_TEXTIT, uT("Italic\tShift+Ctrl+I"), uT("Convert the selected text to italic"), wxITEM_NORMAL);
            menuFormat->Append(ID_LATEX_TEXTSC, uT("Small Caps"), uT("Convert the selected text to small caps"), wxITEM_NORMAL);
            menuFormat->Append(ID_LATEX_EMPH, uT("Emphasize\tShift+Ctrl+E"), uT("Emphasize the selected text"), wxITEM_NORMAL);
            menuFormat->Append(ID_LATEX_MATHRM, uT("Upright mode\tShift+Ctrl+M"), uT("Convert the selected text to upright roman in math mode"), wxITEM_NORMAL);
            menuFormat->Append(ID_LATEX_TEXTNORMAL, uT("Normal style\tShift+Ctrl+N"), uT("Convert the selected text to normal style"), wxITEM_NORMAL);
            menuFormat->Append(ID_LATEX_TEXTSUPERSCRIPT, uT("Superscript"), uT("Make the selected text superscript"), wxITEM_NORMAL);
            menuFormat->Append(ID_LATEX_TEXTSUBSCRIPT, uT("Subscript"), uT("Make the selected text subscript"), wxITEM_NORMAL);
            menuFormat->AppendSeparator();
        }

        menuFormat->Append(ID_SCRIPT_UPPERCASE, uT("Upper Case"), uT("Convert the selected text to upper case"), wxITEM_NORMAL);
        menuFormat->Append(ID_SCRIPT_LOWERCASE, uT("Lower Case"), uT("Convert the selected text to lower case"), wxITEM_NORMAL);

        menuFormat->AppendSeparator();
    }

    pItem = new wxMenuItem(menuFormat, ID_SCRIPT_INDENTINC, uT("Increase Indent"), uT("Increase the indentation of the text (move to right)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(indentinc_small_xpm);
#endif
    menuFormat->Append(pItem);

    pItem = new wxMenuItem(menuFormat, ID_SCRIPT_INDENTDEC, uT("Decrease Indent"), uT("Decrease the indentation of the text (move to left)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(indentdec_small_xpm);
#endif
    menuFormat->Append(pItem);

    return;
}

void ScriptEdit::CreateMenuSnippet(wxMenu *menuSnippet)
{
    if (GetReadOnly() || (isLexerKnown(GetLexer()) == false)) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame != NULL) {
        CodeSample *pSnippet = pFrame->getSnippet();
        if (pSnippet != NULL) {

            wxMenuItem *pItem = new wxMenuItem(menuSnippet, ID_SCRIPT_SNIPPET_01, pSnippet->getTitle(ID_SCRIPT_SNIPPET_01 - ID_SCRIPT_SNIPPET_FIRST));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
            pItem->SetBitmap(code_small_xpm);
#endif
            menuSnippet->Append(pItem);

            for (int idT = ID_SCRIPT_SNIPPET_FIRST + 1; idT <= ID_SCRIPT_SNIPPET_LAST; idT++) {
                const char_t *pszT = pSnippet->getTitle(idT - ID_SCRIPT_SNIPPET_FIRST);
                if (NULL == pszT) {
                    break;
                }
                menuSnippet->Append(idT, pszT);
            }
        }
    }
}

void ScriptEdit::OnContextMenu(wxContextMenuEvent &tEvent)
{
    DoCallTipCancel();

    m_bPopmenuShown = true;

    wxPoint ptMousePosition = tEvent.GetPosition();
    if ((ptMousePosition.x == -1) && (ptMousePosition.y == -1)) {
        wxSize sizeT = GetSize();
        ptMousePosition.x = sizeT.x / 2;
        ptMousePosition.y = sizeT.y / 2;
    }
    else {
        ptMousePosition = ScreenToClient(ptMousePosition);
    }

    const int iPos = PositionFromPoint(ptMousePosition);
    if ((iPos < this->GetSelectionStart()) || (iPos > this->GetSelectionEnd())) {
        // if context point outside selection, update selection
        this->SetSelection(iPos, iPos);
    }

    wxMenu popMenu;

    wxMenuItem *pItem = NULL;

    int iMarginL = GetMarginWidth(m_LinenumberID);
    int iMarginR = iMarginL + GetMarginWidth(m_MarkerID);
    int iMarginRR = iMarginR + GetMarginWidth(m_FoldingID);
    int iMarginRRR = iMarginRR + GetMarginWidth(m_StatusID);

    if ((ptMousePosition.x >= iMarginL) && (ptMousePosition.x <= iMarginR)) {

        pItem = new wxMenuItem(&popMenu, ID_SCRIPT_BOOKMARK_MOD, uT("Add &Bookmark"), uT("Add or remove bookmark"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
        pItem->SetBitmap(bookmark_add_small_xpm);
#endif
        popMenu.Append(pItem);

        if (getBookmarkCount() > 0) {
            pItem = new wxMenuItem(&popMenu, ID_SCRIPT_BOOKMARK_NEXT, uT("Next Bookmark"), uT("Go to the next bookmark"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
            pItem->SetBitmap(bookmark_next_small_xpm);
#endif
            popMenu.Append(pItem);

            pItem = new wxMenuItem(&popMenu, ID_SCRIPT_BOOKMARK_PREV, uT("Previous Bookmark"), uT("Go to the previous bookmark"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
            pItem->SetBitmap(bookmark_prev_small_xpm);
#endif
            popMenu.Append(pItem);

            pItem = new wxMenuItem(&popMenu, ID_SCRIPT_BOOKMARK_DELALL, uT("Delete File Bookmarks"), uT("Delete current file bookmarks"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
            pItem->SetBitmap(bookmark_deleteall_small_xpm);
#endif
            popMenu.Append(pItem);
        }

        if (GetLexer() == wxSTC_LEX_LUA) {
            popMenu.AppendSeparator();

            pItem = new wxMenuItem(&popMenu, ID_SCRIPT_BREAKPOINT_MOD, uT("Add &Breakpoint"), uT("Add or remove breakpoint"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
            pItem->SetBitmap(breakpoint_add_small_xpm);
#endif
            popMenu.Append(pItem);

            if (getBreakpointCount() > 0) {
                popMenu.Append(ID_SCRIPT_BREAKPOINT_DELALL, uT("Reset Breakpoints"), uT("Remove all breakpoints"));
            }
        }

        this->PopupMenu(&popMenu, ptMousePosition);

        return;
    }
    else if ((ptMousePosition.x >= iMarginRR) && (ptMousePosition.x <= iMarginRRR)) {
        if (containsMarkers()) {
            popMenu.Append(ID_FILE_RESETMARKERS, uT("Remove Markers"), uT("Remove all markers"));
            this->PopupMenu(&popMenu, ptMousePosition);
        }
        return;
    }
    else if (ptMousePosition.x <= iMarginRRR) {
        return;
    }

    wxString strWord = wxEmptyString;
    int iLexer = this->GetLexer();
    bool bOpenIncluded = ((iLexer == wxSTC_LEX_LUA) || (iLexer == wxSTC_LEX_CPP) || (iLexer == wxSTC_LEX_PYTHON) || (iLexer == wxSTC_LEX_LATEX));
    int iLineCurrent = 0;
    int iSelStart = 0, iSelEnd = 0, iWordStart = 0, iWordEnd = 0;

    // >> 'goto definition' or (include, require, import)

    if ((iLexer == wxSTC_LEX_LUA) || (iLexer == wxSTC_LEX_CPP) || (iLexer == wxSTC_LEX_PYTHON) || (iLexer == wxSTC_LEX_LATEX)) {
        iSelStart = this->GetSelectionStart();
        iSelEnd = this->GetSelectionEnd();
        iLineCurrent = this->LineFromPosition(iSelStart);
        const int iLineCurrentx = this->LineFromPosition(iSelEnd);
        if (iSelEnd == iSelStart) {
            iWordStart = WordStartPosition(iSelStart, true);
            iWordEnd = WordEndPosition(iSelStart, true);
            if (iWordEnd >= (iWordStart + 2)) {
                strWord = this->GetTextRange(iWordStart, iWordEnd);
            }
        }
        else if ((iSelEnd >= (iSelStart + 2)) && (iSelEnd < (iSelStart + LM_STRSIZES)) && (iLineCurrentx == iLineCurrent)) {
            iWordStart = iSelStart;
            iWordEnd = iSelEnd;
            strWord = this->GetTextRange(iWordStart, iWordEnd);
            wxString strW = wxString(LM_CSTR(strWord));
            strW.Replace(uT("\t"), uT(""), true);
            strW.Replace(uT(" "), uT(""), true);
            if (strW.IsEmpty()) {
                strWord = wxEmptyString;
            }
        }
    }

    // <<

    // >> 'goto definition'

    int iFuncCount = this->CodeAnalyzerGetCount();
    if ((strWord.IsEmpty() == false) && (iFuncCount > 0) && ((iLexer == wxSTC_LEX_LUA) || (iLexer == wxSTC_LEX_PYTHON))) {
        wxString strT = wxString(LM_CSTR(strWord));
        wxString strNx = uT("Go To '");
        if (iWordEnd > (iWordStart + LM_STRSIZET)) {
            strNx += strT.Mid(0, LM_STRSIZET);
            strNx += uT("...");
        }
        else {
            strNx += strT;
        }
        strNx += uT("' Definition");
        strT += uT("(");
        int indexT = this->CodeAnalyzerGetIndex(LM_CSTR(strT), false);
        if ((indexT >= 0) && (indexT < iFuncCount)) {
            int iLine = this->CodeAnalyzerGetLine(indexT);
            if (iLine != iLineCurrent) {
                m_strSelectedFunc = this->CodeAnalyzerGetName(indexT);
                SetSelection(iWordStart, iWordEnd);
                popMenu.Append(ID_ANALYZE_GOTO, strNx, uT("Go to function definition"));
                popMenu.AppendSeparator();
                int idd = (this->TextHeight(GetCurrentLine()) / 4);
                if (idd < 2) {
                    idd = 2;
                }
                else if (idd > 16) {
                    idd = 16;
                }
                ptMousePosition.y += idd;
                bOpenIncluded = false;
            }
        }
    }

    // <<

    // >> include, require, import

    wxFileName fname = this->GetFilename();

    if (bOpenIncluded && (strWord.IsEmpty() == false) && fname.FileExists()) {

        bOpenIncluded = false;
        wxString strPath = wxEmptyString;

        const wxString strLine = this->GetCurLine();
        const int iLineLen = (int)(strLine.Length());

        wxString strLineStart = strLine.Mid(0, (iLineLen > LM_STRSIZES) ? LM_STRSIZES : iLineLen);
        strLineStart.Replace(uT("\t"), uT(""), true);
        strLineStart.Replace(uT(" "), uT(""), true);

        const bool bCInc = ((iLexer == wxSTC_LEX_CPP) && strLineStart.StartsWith(uT("#include")));
        bool bLuaInc = ((iLexer == wxSTC_LEX_LUA) && strLineStart.StartsWith(uT("require")));
        bool bPythonInc = ((iLexer == wxSTC_LEX_PYTHON) && strLineStart.StartsWith(uT("import")));
        const bool bLaTeXInp = ((iLexer == wxSTC_LEX_LATEX) && strLineStart.StartsWith(uT("\\input")));
        const bool bLaTeXInc = bLaTeXInp || ((iLexer == wxSTC_LEX_LATEX) && strLineStart.StartsWith(uT("\\include")));

        const int iLineEnd = GetLineEndPosition(iLineCurrent);
        const int iLineStart = iLineEnd - iLineLen;
        int iWs = (iLexer == wxSTC_LEX_LUA) ? 7 : (((iLexer == wxSTC_LEX_CPP) || (iLexer == wxSTC_LEX_LATEX)) ? 8 : 6);
        if (bLaTeXInp) {
            iWs = 6;
        }

        if ((bLuaInc == false) && (iLexer == wxSTC_LEX_LUA)) {
            bLuaInc = strLineStart.Contains(uT("=require"));
        }
        if (bLuaInc) {
            const int iF = strLine.Find(uT("require"));
            if ((iF >= 0) && (iF < (iLineLen - (iWs + 2)))) {
                const char_t cc = strLine.GetChar(iF + iWs);
                bLuaInc = (Tisspace(cc) || (cc == uT('(')));
            }
            else {
                bLuaInc = false;
            }
        }

        if (iLexer == wxSTC_LEX_PYTHON) {
            if (bPythonInc) {
                // import modulename
                const int iF = strLine.Find(uT("import"));
                if ((iF >= 0) && (iF < (iLineLen - (iWs + 2)))) {
                    const char_t cc = strLine.GetChar(iF + iWs);
                    bPythonInc = (Tisspace(cc) || (cc == uT('(')));
                }
                else {
                    bPythonInc = false;
                }
            }
            else {
                bPythonInc = strLineStart.StartsWith(uT("from"));
                if (bPythonInc) {
                    // from modulename import ...
                    const int iB = strLine.Find(uT("from"));
                    const int iF = strLine.Find(uT("import"));
                    const int iWordIndex = strLine.Find(LM_CSTR(strWord));
                    const int iWordLen = (int)(strWord.Length());
                    bPythonInc = ((iB >= 0) && (iB < (iWordIndex - 4)) && (iF > (iB + 4)) && (iF > (iWordIndex + iWordLen)));
                    iWs = 4;
                }
            }
        }

        if ((iWordStart > (iLineStart + iWs)) && (bCInc || bLuaInc || bPythonInc || bLaTeXInc)) {

            strPath = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

            if ((iLexer == wxSTC_LEX_LUA) || (iLexer == wxSTC_LEX_PYTHON)) {
                strPath += strWord;
                if ((strPath.EndsWith(uT(".lua")) == false) && (strPath.EndsWith(uT(".py")) == false)) {
                    strPath += (iLexer == wxSTC_LEX_LUA) ? uT(".lua") : uT(".py");
                }
                bOpenIncluded = ::wxFileExists(strPath);

                if ((iLexer == wxSTC_LEX_LUA) && (bOpenIncluded == false)) {
                    strPath = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
                    strPath += strWord;
                    strPath += uT(".comet");
                    bOpenIncluded = ::wxFileExists(strPath);
                }
            }

            else if (iLexer == wxSTC_LEX_CPP) {

                const int iWordIndex = strLine.Find(LM_CSTR(strWord));
                const int iWordLen = (int)(strWord.Length());

                char_t cc = uT('<');

                int iB = strLine.Find(cc, false);
                if ((iB < iWs) || (iB >= iWordIndex)) {
                    cc = uT('"');
                    iB = strLine.Find(cc, false);
                }
                else {
                    cc = uT('>');
                }

                if ((iB >= iWs) && (iB < iWordIndex)) {
                    const int iF = strLine.Find(cc, true);
                    if ((iF >= ((iWordIndex + iWordLen) + 2)) && (iF < iLineLen)) {
                        iWordStart -= (iWordIndex - (iB + 1));
                        // Take into account '.h' (or '.c') --> only C (not C++)
                        iWordEnd += (iF - (iWordIndex + iWordLen));
                        // could contain path like sys/unistd.h
                        strWord = strLine.Mid(iB + 1, iF - iB - 1);
                        int ii = 0;
                        const int iLen = (int)(strWord.Length());
                        while (Tisspace(strWord.GetChar(ii++)) && (ii < iLen)) {
                            iWordStart += 1;
                        }
                        ii = iLen - 1;
                        if (ii > 1) {
                            while (Tisspace(strWord.GetChar(ii--)) && (ii >= 0)) {
                                iWordEnd -= 1;
                            }
                        }
                        if (iWordEnd > (iWordStart + 1)) {
                            strWord.Replace(uT("\t"), uT(""), true);
                            strWord.Replace(uT(" "), uT(""), true);
                            if (strWord.IsEmpty() == false) {
                                strPath += strWord;
                                bOpenIncluded = ::wxFileExists(strPath);
                                if (bOpenIncluded == false) {
                                    strPath = CometApp::INCDIR;
                                    strPath += strWord;
                                    bOpenIncluded = ::wxFileExists(strPath);
                                }
                            }
                        }
                    }
                }
            }

            else if (iLexer == wxSTC_LEX_LATEX) {
                strWord.Replace(uT("\t"), uT(""), true);
                strWord.Replace(uT(" "), uT(""), true);
                if (strWord.IsEmpty() == false) {
                    strPath += strWord;
                    if (strPath.EndsWith(uT(".tex")) == false) {
                        strPath += uT(".tex");
                    }
                    bOpenIncluded = ::wxFileExists(strPath);
                }
            }

            if (bOpenIncluded) {
                bOpenIncluded = false;
                if (strPath.IsSameAs(this->GetFilename())) {
                    m_strFileToOpen = wxEmptyString;
                }
                else {
                    m_strFileToOpen = strPath;
                    SetSelection(iWordStart, iWordEnd);
                    wxString strNx = uT("Open '");
                    strNx += wxString(LM_CSTR(strWord));
                    strNx += uT("'");
                    popMenu.Append(ID_INCLUDE_GOTO, strNx, uT("Open the included file"));
                    popMenu.AppendSeparator();
                    int idd = (this->TextHeight(GetCurrentLine()) / 4);
                    if (idd < 2) {
                        idd = 2;
                    }
                    else if (idd > 16) {
                        idd = 16;
                    }
                    ptMousePosition.y += idd;
                }
            }
        }
    }

    // <<

    pItem = new wxMenuItem(&popMenu, ID_SCRIPT_UNDO, uT("Undo"), uT("Undo the last action"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(undo_small_xpm);
#endif
    popMenu.Append(pItem);
    pItem = new wxMenuItem(&popMenu, ID_SCRIPT_REDO, uT("Redo"), uT("Redo the last action"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(redo_small_xpm);
#endif
    popMenu.Append(pItem);
    popMenu.AppendSeparator();

    if (GetReadOnly() == false) {
        pItem = popMenu.Append(wxID_CUT, uT("Cut\tCtrl+X"), uT("Cut the selected text"), wxITEM_NORMAL);
    }
    pItem = new wxMenuItem(&popMenu, wxID_COPY, uT("Copy\tCtrl+C"), uT("Copy the selected text"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(copy_small_xpm);
#endif
    popMenu.Append(pItem);

    if (GetReadOnly() == false) {
        pItem = new wxMenuItem(&popMenu, wxID_PASTE, uT("Paste\tCtrl+V"), uT("Paste the copied text"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
        pItem->SetBitmap(paste_small_xpm);
#endif
        popMenu.Append(pItem);
    }

    wxMenu *menuSelect = new wxMenu();
    pItem = new wxMenuItem(menuSelect, wxID_SELECTALL, uT("Select All\tCtrl+A"), uT("Select the whole text"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(selectall_small_xpm);
#endif
    menuSelect->Append(pItem);
    menuSelect->Append(ID_SCRIPT_SELECTLINE, uT("Select Line\tCtrl+L"), uT("Select current line"));
    menuSelect->Append(ID_SCRIPT_SELECTWORD, uT("Select Word\tCtrl+W"), uT("Select current word"));
    popMenu.AppendSubMenu(menuSelect, uT("Select"));

    if (GetReadOnly() == false) {

        bool bEnable = isLexerKnown(GetLexer());

        if (bEnable) {
            wxMenu *menuFormat = new wxMenu();

            CreateMenuFormat(menuFormat, true);

            popMenu.AppendSubMenu(menuFormat, uT("Format"));

            popMenu.AppendSeparator();

            if (containsMarkers()) {
                popMenu.Append(ID_FILE_RESETMARKERS, uT("Remove Markers"), uT("Remove all markers"));
                popMenu.AppendSeparator();
            }

            CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
            if (pFrame && pFrame->getSnippet()) {
                wxMenu *menuSnippet = new wxMenu();
                CreateMenuSnippet(menuSnippet);
                popMenu.AppendSubMenu(menuSnippet, uT("Insert Snippet"), uT("Insert code snippet"));
                popMenu.AppendSeparator();
            }
        }
    }

    wxMenu *menuView = new wxMenu();

    pItem = new wxMenuItem(menuView, ID_SCRIPT_ZOOMIN, uT("Zoom In"), uT("Increase the editor's font size"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(zoomin_small_xpm);
#endif
    menuView->Append(pItem);

    pItem = new wxMenuItem(menuView, ID_SCRIPT_ZOOMOUT, uT("Zoom Out"), uT("Decrease the editor's font size"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(zoomout_small_xpm);
#endif
    menuView->Append(pItem);

    menuView->Append(ID_SCRIPT_ZOOMNONE, uT("Normal Viewing"), uT("Set the editor's font size to the default one"), wxITEM_NORMAL);

    menuView->AppendSeparator();

    menuView->Append(ID_SCRIPT_DISPLAYEOL, uT("End Of Line"), uT("Display the end of line"), wxITEM_CHECK);
    menuView->Append(ID_SCRIPT_WHITESPACE, uT("White Spaces"), uT("Display white spaces"), wxITEM_CHECK);

    popMenu.AppendSubMenu(menuView, uT("View"), uT("View"));

    this->PopupMenu(&popMenu, ptMousePosition);
}

void ScriptEdit::DoCommentSelection(bool bComment)
{
    int iLineCount = this->GetLineCount();
    if (iLineCount < 1) {
        return;
    }

    int iStart = 0, iEnd = 0;
    GetSelection(&iStart, &iEnd);
    if (iEnd < iStart) {
        // should never happen
        return;
    }

    const int iSelBak = iEnd;

    const bool bTextSelected = ((iEnd - iStart) > 0);
    if (bTextSelected == false) {
        // (un)comment the current line
        int iLineEnd = this->LineFromPosition(iEnd);
        if ((iLineEnd < 0) || (iLineEnd >= iLineCount)) {
            return;
        }
        iStart = this->PositionFromLine(iLineEnd);
        iEnd = this->GetLineEndPosition(iLineEnd);
        this->SetSelection(iStart, iEnd);
    }

    int iLineEnd = LineFromPosition(iEnd);
    if ((iLineEnd < 0) || (iLineEnd >= iLineCount)) {
        return;
    }
    int iLineStart = LineFromPosition(iStart);
    if ((iLineStart < 0) || (iLineStart >= iLineCount) || (iLineStart > iLineEnd)) {
        if (bTextSelected == false) {
            this->SetSelection(iSelBak, iSelBak);
        }
        return;
    }

    bool bSelect = false;
    if (iLineStart < iLineEnd) {
        // Selection from the beginning of the first line ...
        int iPosT = this->PositionFromLine(iLineStart);
        if (iPosT != iStart) {
            iStart = iPosT;
            bSelect = true;
        }

        // ... to the end of the last line
        iPosT = this->PositionFromLine(iLineEnd);
        if (iPosT == iEnd) {
            iLineEnd--;
            iEnd = this->GetLineEndPosition(iLineEnd);
            bSelect = true;
        }
        else {
            iPosT = this->GetLineEndPosition(iLineEnd);
            if (iEnd != iPosT) {
                iEnd = iPosT;
                bSelect = true;
            }
        }
    }
    if (bSelect) {
        SetSelection(iStart, iEnd);
    }

    wxString strSel = this->GetSelectedText();
    wxString strSelTrimmed = strSel;
    strSelTrimmed.Trim();
    if (strSelTrimmed.IsEmpty()) {
        if (bTextSelected == false) {
            this->SetSelection(iSelBak, iSelBak);
        }
        return;
    }

    wxString strT = strSel;
    wxString strTT = uT(""), strLine = uT("");
    wxString strCmt;
    int iCmtLen;
    int iLexer = GetLexer();
    switch (iLexer) {
        case wxSTC_LEX_LUA:
            strCmt = uT("--");
            iCmtLen = 2;
            break;
        case wxSTC_LEX_CPP:
            strCmt = uT("//");
            iCmtLen = 2;
            break;
        case wxSTC_LEX_BASH:
            strCmt = uT("#");
            iCmtLen = 1;
            break;
        case wxSTC_LEX_PYTHON:
            strCmt = uT("#");
            iCmtLen = 1;
            break;
        case wxSTC_LEX_OCTAVE:
            strCmt = uT("%");
            iCmtLen = 1;
            break;
        case wxSTC_LEX_FORTRAN:
            strCmt = uT("!");
            iCmtLen = 1;
            break;
        case wxSTC_LEX_LATEX:
            strCmt = uT("%");
            iCmtLen = 1;
            break;
        case wxSTC_LEX_MAKEFILE:
            strCmt = uT("#");
            iCmtLen = 1;
            break;
        case wxSTC_LEX_COMET:
            strCmt = uT("#");
            iCmtLen = 1;
            break;
        case wxSTC_LEX_BATCH:
            strCmt = uT("rem ");
            iCmtLen = 4;
            break;
        case wxSTC_LEX_SOLIS:
            strCmt = uT("#");
            iCmtLen = 1;
            break;
        default:
            return;
    }

    bool bReplace = false;

    if (bComment) { // Comment
        if (iLineStart == iLineEnd) {
            strT = strCmt;
            strT += strSel;
            bReplace = true;
        }
        else {
            strT = uT("");
            for (int ii = iLineStart; ii <= iLineEnd; ii++) {
                strLine = this->GetLine(ii);
                strTT = strLine;
                strTT = strTT.Trim(false);
                if (strTT.IsEmpty() == false) {
                    strT += strCmt;
                    strLine.Replace(uT("\n"), uT(""));
                    strLine.Replace(uT("\r"), uT(""));
                    strT += strLine;
                    if (ii < iLineEnd) {
                        strT += uT("\n");
                    }
                    bReplace = true;
                }
                else if (ii < iLineEnd) { // Preserve empty lines
                    strT += uT("\n");
                }
            }
        }
    }
    else { // Uncomment
        if (iLineStart == iLineEnd) {
            int iF = strSel.Find(strCmt);
            if (iF >= 0) {
                if (iF > 0) {
                    strT = strSel.Mid(0, iF);
                    strT += strSel.Mid(iF + iCmtLen);
                }
                else {
                    strT = strSel.Mid(iF + iCmtLen);
                }
                bReplace = true;
            }
        }
        else {
            wxString strLine;
            int iF = -1;
            for (int ii = iLineStart; ii <= iLineEnd; ii++) {
                strLine = this->GetLine(ii);
                strLine.Replace(uT("\n"), uT(""));
                strLine.Replace(uT("\r"), uT(""));
                iF = strLine.Find(strCmt);
                if (iF >= 0) {
                    if (iF > 0) {
                        if (ii == iLineStart) {
                            strT = strLine.Mid(0, iF);
                        }
                        else {
                            strT += strLine.Mid(0, iF);
                        }
                        strT += strLine.Mid(iF + iCmtLen);
                    }
                    else {
                        if (ii == iLineStart) {
                            strT = strLine.Mid(iF + iCmtLen);
                        }
                        else {
                            strT += strLine.Mid(iF + iCmtLen);
                        }
                    }
                    if (ii < iLineEnd) {
                        strT += uT("\n");
                    }
                    bReplace = true;
                }
                else {
                    if (ii == iLineStart) {
                        strT = strLine;
                    }
                    else {
                        strT += strLine;
                    }
                    if (ii < iLineEnd) {
                        strT += uT("\n");
                    }
                }
            }
        }
    }

    if (bReplace) {
        this->ReplaceSelection(strT);
    }

    if (bTextSelected == false) {
        if (bComment) {
            this->SetSelection(iSelBak + iCmtLen, iSelBak + iCmtLen);
        }
        else {
            this->SetSelection(iSelBak - iCmtLen, iSelBak - iCmtLen);
        }
    }
}

void ScriptEdit::DoInsertCodeSnippet(int idT)
{
    if ((idT < ID_SCRIPT_SNIPPET_FIRST) || (idT > ID_SCRIPT_SNIPPET_LAST)) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }
    CodeSample *pSnippet = pFrame->getSnippet();
    if (pSnippet == NULL) {
        return;
    }

    int idx = idT - ID_SCRIPT_SNIPPET_FIRST;
    const char_t *pszT = pSnippet->getTitle(idx);
    if (NULL == pszT) {
        return;
    }

    int nPos = this->GetCurrentPos();
    int nLine = this->LineFromPosition(nPos);
    if ((nLine < 0) || (nLine >= this->GetLineCount())) {
        return;
    }

    wxString strT(pSnippet->getCode(idx));
    if (strT.IsEmpty()) {
        return;
    }
    int iSelPos = pSnippet->getLinemax(idx);
    nPos = this->GetLineEndPosition(nLine);
    if (iSelPos > 0) {
        this->InsertText(this->GetLineEndPosition(nLine), strT);
        this->SetSelection(nPos + iSelPos, nPos + iSelPos);
    }
    else {
        this->SetSelection(nPos, nPos);
        this->ReplaceSelection(strT);
    }
}

void ScriptEdit::OnInsertCodeSnippet(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();
    if ((idT < ID_SCRIPT_SNIPPET_FIRST) || (idT > ID_SCRIPT_SNIPPET_LAST)) {
        tEvent.Skip();
        return;
    }

    DoInsertCodeSnippet(idT);
}

void ScriptEdit::OnCommentSelection(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();
    if ((idT != ID_SCRIPT_COMMENTSELECTION) && (idT != ID_SCRIPT_UNCOMMENTSELECTION)) {
        tEvent.Skip();
        return;
    }

    DoCommentSelection(idT == ID_SCRIPT_COMMENTSELECTION);
}

void ScriptEdit::DoChangeCase(int iCase)
{
    if (iCase == ID_SCRIPT_UPPERCASE) {
        this->UpperCase();
    }
    else if (iCase == ID_SCRIPT_LOWERCASE) {
        this->LowerCase();
    }
}

void ScriptEdit::OnUpperCase(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();
    if (idT != ID_SCRIPT_UPPERCASE) {
        tEvent.Skip();
        return;
    }

    this->UpperCase();
}

void ScriptEdit::OnLowerCase(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();
    if (idT != ID_SCRIPT_LOWERCASE) {
        tEvent.Skip();
        return;
    }

    this->LowerCase();
}

void ScriptEdit::DoLaTeXFormat(int idT)
{
    if (GetLexer() != wxSTC_LEX_LATEX) {
        return;
    }

    wxString strT = this->GetSelectedText();
    if (strT.IsEmpty()) {
        return;
    }

    const int arId[] = {
        ID_LATEX_TEXTBF, ID_LATEX_TEXTIT, ID_LATEX_TEXTSC, ID_LATEX_MATHRM, ID_LATEX_TEXTNORMAL, ID_LATEX_EMPH, ID_LATEX_TEXTSUPERSCRIPT, ID_LATEX_TEXTSUBSCRIPT, -1
    };
    const char_t *arT[] = {
        uT("\\textbf{"), uT("\\textit{"), uT("\\textsc{"), uT("\\mathrm{"), uT("{"), uT("\\emph{"), uT("\\textsuperscript{"), uT("\\textsubscript{"), NULL
    };
    const char_t *pszPrepend = NULL;
    for (int ii = 0; arId[ii] > 0; ii++) {
        if (arId[ii] == idT) {
            pszPrepend = arT[ii];
            break;
        }
    }
    if (NULL == pszPrepend) {
        // should never happen
        return;
    }

    bool bStartswith = false, bEndswith = false;
    int idx = 0, iOffset = 0;
    for (int ii = 0; arT[ii] != NULL; ii++) {
        if (strT.StartsWith(arT[ii])) {
            bStartswith = true;
            bEndswith = strT.EndsWith(uT("}"));
            idx = ii;
            iOffset = (int)Tstrlen(arT[ii]);
            break;
        }
    }
    if (bStartswith != bEndswith) {
        // inconsistent selection
        return;
    }

    wxString strX = strT.Mid(iOffset);

    for (int ii = 0; arT[ii] != NULL; ii++) {
        if (strX.Contains(arT[ii])) {
            // inconsistent selection (imbricated formats)
            return;
        }
    }

    if (bStartswith) {
        if (*pszPrepend == uT('{')) {
            strX = strT.Mid(iOffset, strT.Length() - iOffset - 1);
        }
        else {
            strX = strT.Mid(iOffset);
            strX = strX.Prepend(pszPrepend);
        }
    }
    else {
        if (*pszPrepend == uT('{')) {
            strX = strT;
        }
        else {
            strX = strT.Prepend(pszPrepend);
            strX = strX.Append(uT("}"));
        }
    }
    this->ReplaceSelection(strX);
}

void ScriptEdit::OnLaTeXFormat(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();
    DoLaTeXFormat(idT);
}

void ScriptEdit::OnEditAction(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();

    DoCallTipCancel();

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
        case wxID_SELECTALL:
            DoEditSelectAll();
            break;
        case ID_SCRIPT_REDO:
            DoEditRedo();
            break;
        case ID_SCRIPT_UNDO:
            DoEditUndo();
            break;
        case wxID_CLEAR:
            DoEditDelete();
            break;
        case ID_SCRIPT_SELECTWORD:
            DoEditSelectWord();
            break;
        case ID_SCRIPT_SELECTLINE:
            DoEditSelectLine();
            break;
        case ID_SCRIPT_INSERT_DATE:
            DoEditInsertDate();
            break;
        case ID_SCRIPT_INSERT_TIME:
            DoEditInsertTime();
            break;
        case ID_SCRIPT_INSERT_DATETIME:
            DoEditInsertDateTime();
            break;
        case ID_ANALYZE_GOTO:
            if (m_strSelectedFunc.IsEmpty() == false) {
                CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
                if (pFrame != NULL) {
                    pFrame->DoAnalyzerChange(m_strSelectedFunc, true);
                }
                m_strSelectedFunc = wxEmptyString;
            }
            break;
        case ID_INCLUDE_GOTO:
            if (m_strFileToOpen.IsEmpty() == false) {
                CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
                if (pFrame) {
                    pFrame->fileOpen(m_strFileToOpen);
                }
                m_strFileToOpen = wxEmptyString;
            }
            break;
        default:
            break;
    }
}

void ScriptEdit::OnUpdateUndoRedo(wxUpdateUIEvent &tEvent)
{
    int idT = tEvent.GetId();

    if (idT == ID_SCRIPT_UNDO) {
        CometFrame::enableUIitem(tEvent, CanUndo());
    }
    else if (idT == ID_SCRIPT_REDO) {
        CometFrame::enableUIitem(tEvent, CanRedo());
    }
}

void ScriptEdit::OnUpdateCopyPaste(wxUpdateUIEvent &tEvent)
{
    if (GetReadOnly()) {
        CometFrame::enableUIitem(tEvent, false);
    }

    int idT = tEvent.GetId();

    if ((idT == wxID_CUT) || (idT == wxID_COPY) || (idT == wxID_CLEAR)) {
        int iStart = 0, iEnd = 0;
        GetSelection(&iStart, &iEnd);
        CometFrame::enableUIitem(tEvent, iEnd > iStart);
    }
    else if (idT == wxID_PASTE) {
        bool bEnable = false;
        if (wxTheClipboard && wxTheClipboard->Open()) {
            bEnable = wxTheClipboard->IsSupported(wxDF_TEXT);
            wxTheClipboard->Close();
        }
        CometFrame::enableUIitem(tEvent, bEnable);
    }
}

// Call tips
void ScriptEdit::OnDwellStart(wxStyledTextEvent &tEvent)
{
    if (m_bPopmenuShown) {
        m_bPopmenuShown = false;
        return;
    }

    if ((m_ScintillaPrefs.common.calltipEnable == false) || ((m_iLexer != wxSTC_LEX_LUA) && (m_iLexer != wxSTC_LEX_PYTHON))) {
        return;
    }

    if (GetSTCFocus() == false) {
        DoCallTipCancel();
        return;
    }

    int iPos = tEvent.GetPosition();
    if (iPos < 0) {
        return;
    }

    int iLine = this->LineFromPosition(iPos);

    if ((MarkerGet(iLine) & SCRIPT_MASK_ERROR) && (m_strErrMsg.IsEmpty() == false)) {
        wxString strMsg;
        if (m_strErrMsg.StartsWith(uT("\n"))) {
            strMsg = m_strErrMsg.Mid(1);
        }
        else {
            strMsg = m_strErrMsg;
        }
        CallTipShow(iPos, strMsg);
        return;
    }

    if (CodeEdit::DoDwellStart(iPos)) {
        return;
    }

    if (m_bWordEnclosed) {
        m_bWordEnclosed = false;
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    int iStart = WordStartPosition(iPos, true);
    int iEnd = WordEndPosition(iPos, true);
    if (iEnd <= iStart) {
        return;
    }
    wxString strN = GetTextRange(iStart, iEnd);
    int iLen = (int)(strN.Length());
    if ((iLen < 1) || (iLen >= LM_STRSIZEN)) {
        return;
    }

    if (m_iLexer != wxSTC_LEX_LUA) {
        return;
    }

    if (isDebugging()) {

        wxString strV = uT("");

        if (pFrame->GetDebugSymbol(strN, strV)) {
            strN += uT(" = ");
            strN += strV;
            CallTipShow(iPos, strN);
            return;
        }
    }
}

void ScriptEdit::OnDwellEnd(wxStyledTextEvent &tEvent)
{
    CodeEdit::OnDwellEnd(tEvent);
}
