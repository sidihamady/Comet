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
#include "OutputEdit.h"

BEGIN_EVENT_TABLE(OutputEdit, CodeEdit)

    EVT_CONTEXT_MENU(OutputEdit::OnContextMenu)

    EVT_MENU(wxID_COPY, OutputEdit::OnEditAction)
    EVT_MENU(wxID_CLEAR, OutputEdit::OnEditAction)
    EVT_MENU(wxID_SELECTALL, OutputEdit::OnEditAction)

    EVT_MENU(ID_OUTPUT_ZOOMIN, OutputEdit::OnOutputAction)
    EVT_MENU(ID_OUTPUT_ZOOMOUT, OutputEdit::OnOutputAction)
    EVT_MENU(ID_OUTPUT_ZOOMNONE, OutputEdit::OnOutputAction)
    EVT_MENU(ID_OUTPUT_CLEARKEY, OutputEdit::OnOutputAction)

    EVT_COMMAND(ID_THREAD_OUTPUT_APPEND, wxEVT_COMMAND_TEXT_UPDATED, OutputEdit::OnUpdated)

    EVT_MOUSEWHEEL(OutputEdit::OnMouseWheel)

END_EVENT_TABLE()

OutputEdit::OutputEdit(wxWindow *parentT, wxWindowID idT,
                       const wxPoint &posT, const wxSize &sizeT, long styleT,
                       const wxString &strName)
    : CodeEdit(parentT, idT, posT, sizeT, styleT, strName)
{
    m_bInitialized = false;

    m_ScintillaPrefs.common.syntaxEnable = false;

    m_iMaxLineWidth = 0;

    m_strName = uT("Output");

    m_iRunningLexer = wxSTC_LEX_NULL;
    m_iErrPos = -1;
    m_ErrLine.line = -1;
    m_ErrLine.id = 0L;
    m_iTotalLength = 0;
    m_strErrInfo.Empty();

    // specific LaTeX stuff
    m_latexWarn = 0;
    m_latexOverfull = 0;
    m_latexOverfullMax = 0.0;
    m_latexUnderfull = 0;
    m_latexFilename.Empty();
    m_latexFullFilename.Empty();
    //

    setModeScriptConsole(true);

    // Accelerators
    const int nCount = 10;
    wxAcceleratorEntry accelEntries[nCount];
    accelEntries[0].Set(wxACCEL_CTRL, (int)'A', wxID_SELECTALL);
    accelEntries[1].Set(wxACCEL_CTRL, (int)'C', wxID_COPY);
    accelEntries[2].Set(wxACCEL_NORMAL, WXK_TAB, ID_NOTHING);
    accelEntries[3].Set(wxACCEL_CTRL, (int)'+', ID_OUTPUT_ZOOMIN);
    accelEntries[4].Set(wxACCEL_CTRL, (int)'-', ID_OUTPUT_ZOOMOUT);
    accelEntries[5].Set(wxACCEL_CTRL, (int)'0', ID_OUTPUT_ZOOMNONE);
    accelEntries[6].Set(wxACCEL_CTRL, WXK_NUMPAD_ADD, ID_OUTPUT_ZOOMIN);
    accelEntries[7].Set(wxACCEL_CTRL, WXK_NUMPAD_SUBTRACT, ID_OUTPUT_ZOOMOUT);
    accelEntries[8].Set(wxACCEL_CTRL, WXK_NUMPAD0, ID_OUTPUT_ZOOMNONE);
    accelEntries[9].Set(wxACCEL_NORMAL, WXK_DELETE, ID_OUTPUT_CLEARKEY);
    wxAcceleratorTable accelTable(nCount, accelEntries);
    SetAcceleratorTable(accelTable);
    //

    SetUndoCollection(false);
    EmptyUndoBuffer();
}

OutputEdit::~OutputEdit()
{
    if (m_pMutex) {
        delete m_pMutex;
        m_pMutex = NULL;
    }
}

bool OutputEdit::reinitPrefs(void)
{
    bool bSolis = (wxSTC_LEX_SOLIS == m_iRunningLexer);

    m_ScintillaPrefs.common.syntaxEnable = bSolis;

    m_iLexer = bSolis ? wxSTC_LEX_SOLIS : wxSTC_LEX_NULL;
    m_iLexerType = bSolis ? LEXERTYPE_SECOND : LEXERTYPE_FIRST;

    initLexer(m_iLexer, m_iLexerType);
    DoHighlightSyntax(m_ScintillaPrefs.common.syntaxEnable);

    return true;
}

bool OutputEdit::setPrefs(const ScintillaPrefs &tPrefs, bool bInit /* = true*/)
{
    bool bRet = CodeEdit::setPrefs(tPrefs, false);
    m_ScintillaPrefs.common.autocompleteEnable = false;
    m_ScintillaPrefs.common.calltipEnable = false;
    m_ScintillaPrefs.common.foldEnable = false;
    m_ScintillaPrefs.common.wrapModeInitial = false;
    m_ScintillaPrefs.common.lineNumberEnable = false;
    m_ScintillaPrefs.common.displayEOLEnable = false;
    m_ScintillaPrefs.common.lineSpacing = 0;

    long iB = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
    long iF = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
    iB = CodeEdit::normalizeColor(iB, (iB > iF) ? -12L : 12L);
    m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background = iB;

    if (bInit) {

        CodeEdit::initPrefs();

        unsigned long lBack = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
        unsigned long lFore = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
        wxColor clrBack = wxColor(lBack);
        wxColor clrFore = wxColor(lFore);

        // Status (character) margin
        SetMarginMask(m_StatusID, SCRIPT_MASK_SAVED);
        SetMarginType(m_StatusID, wxSTC_MARGIN_SYMBOL);
        SetMarginWidth(m_StatusID, m_ScintillaPrefs.common.statusEnable ? m_StatusMargin : 0);
        SetMarginSensitive(m_StatusID, false);

        MarkerDefine(SCRIPT_MASK_SAVEDBIT, wxSTC_MARK_CHARACTER + '#', clrFore, clrBack);

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

    SetReadOnly(true);

    return bRet;
}

void OutputEdit::Reset(void)
{
    SetReadOnly(false);
    ClearAll();
    MarkerDeleteAll(SCRIPT_MASK_SAVEDBIT);

    // Always call EmptyUndoBuffer to avoid any unnecessary increasing memory usage
    EmptyUndoBuffer();
    SetReadOnly(true);

    if (this->isFocused()) {
        updateStatusbar();
    }

    m_iMaxLineWidth = 20;
    this->SetScrollWidth(m_iMaxLineWidth);

    m_iTotalLength = 0;
}

void OutputEdit::OnContextMenu(wxContextMenuEvent &tEvent)
{
    if (this->GetTextLength() < 1) {
        return;
    }

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

    int iStart, iEnd;
    GetSelection(&iStart, &iEnd);
    if ((iStart >= 0) && (iEnd >= 0) && (iStart < iEnd)) {
        popMenu.Append(wxID_COPY, uT("Copy"), uT("Copy the selected text"), wxITEM_NORMAL);
        popMenu.AppendSeparator();
    }

    popMenu.Append(wxID_SELECTALL, uT("Select All"), uT("Select All"), wxITEM_NORMAL);
    popMenu.AppendSeparator();

    wxMenu *menuView = new wxMenu();
    wxMenuItem *pItem = new wxMenuItem(menuView, ID_OUTPUT_ZOOMIN, uT("Zoom In"), uT("Increase the output's font size"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(zoomin_small_xpm);
#endif
    menuView->Append(pItem);
    pItem = new wxMenuItem(menuView, ID_OUTPUT_ZOOMOUT, uT("Zoom Out"), uT("Decrease the output's font size"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(zoomout_small_xpm);
#endif
    menuView->Append(pItem);
    menuView->Append(ID_OUTPUT_ZOOMNONE, uT("Normal Viewing"), uT("Set the output's font size to the default one"), wxITEM_NORMAL);
    popMenu.AppendSubMenu(menuView, uT("View"), uT("View"));

    popMenu.AppendSeparator();

    pItem = new wxMenuItem(&popMenu, wxID_CLEAR, uT("Reset"), uT("Clear the output window"), wxITEM_NORMAL);
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

void OutputEdit::OnUpdated(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();
    if (idT == ID_THREAD_OUTPUT_APPEND) {
        this->Output(tEvent.GetString(), tEvent.GetInt() == 1);
    }

    else if (idT == ID_THREAD_OUTPUT_CLEAR) {
        this->DoEditReset();
    }
}

bool OutputEdit::isLineEmpty(void)
{
    return GetCurLine().IsEmpty();
}
void OutputEdit::Output(const wxString &strT, bool bForce /* = false*/, bool bToErrPos /* = false*/)
{
    if (strT.Length() < 1) {
        return;
    }

    SetReadOnly(false);

    int iStartLine = GetCurrentLine();
    if (strT.StartsWith(SCRIPT_NEWLINE)) {
        iStartLine += 1;
    }

    // Avoid very long line
    if (GetLine(iStartLine).Length() >= LM_STRSIZE) {
        AppendText(SCRIPT_NEWLINE);
        m_iTotalLength += Tstrlen(SCRIPT_NEWLINE);
    }

    AppendText(strT);
    size_t iStrLen = strT.Length();
    m_iTotalLength += iStrLen;

    if (m_iRunningLexer == wxSTC_LEX_LATEX) {

        wxString strTW;
        int iLaTeXPos = strT.Find(uT(".tex"));
        if (iLaTeXPos > 0) {
            strTW = strT.Mid(0, iLaTeXPos + 4);
            int iLaTeXPosX = (int)(strTW.rfind(uT("(")));
            if ((iLaTeXPosX > 0) && (iLaTeXPosX < iLaTeXPos)) {
                m_latexFilename = strTW.Mid(iLaTeXPosX + 1);
                m_latexFullFilename = m_latexFilename;
                iLaTeXPosX = (int)(m_latexFilename.rfind(uT("/")));
                if (iLaTeXPosX > 0) {
                    m_latexFilename = m_latexFilename.Mid(iLaTeXPosX + 1);
                }
            }
        }

        // specific LaTeX stuff
        wxString strTi = strT.Lower();
        strTW = strTi;
        iLaTeXPos = strTW.Find(uT("warning:"));
        if (iLaTeXPos > 0) {
            ++m_latexWarn;
            for (int ii = 0; ii < 12; ii++) {
                strTW = strTW.Mid(iLaTeXPos + 7);
                iLaTeXPos = strTW.Find(uT("warning:"));
                if (iLaTeXPos < 1) {
                    break;
                }
                ++m_latexWarn;
            }
        }

        strTW = strTi;
        iLaTeXPos = strTW.Find(uT("overfull"));
        if (iLaTeXPos > 0) {
            ++m_latexOverfull;
            //
            wxString strPt;
            int iLaTeXPosX = strTW.Find(uT("\\hbox ("));
            if (iLaTeXPosX > 0) {
                strPt = strTW.Mid(iLaTeXPosX + 7);
                iLaTeXPosX = strPt.Find(uT("pt"));
                if (iLaTeXPosX > 0) {
                    strPt = strPt.Mid(0, iLaTeXPosX);
                    double fT;
                    if (strPt.ToDouble(&fT)) {
                        if (fT > m_latexOverfullMax) {
                            m_latexOverfullMax = fT;
                        }
                    }
                }
            }
            //
            for (int ii = 0; ii < 12; ii++) {
                strTW = strTW.Mid(iLaTeXPos + 7);
                iLaTeXPos = strTW.Find(uT("overfull"));
                if (iLaTeXPos < 1) {
                    break;
                }
                ++m_latexOverfull;
                //
                iLaTeXPosX = strTW.Find(uT("\\hbox ("));
                if (iLaTeXPosX > 0) {
                    strPt = strTW.Mid(iLaTeXPosX + 7);
                    iLaTeXPosX = strPt.Find(uT("pt"));
                    if (iLaTeXPosX > 0) {
                        strPt = strPt.Mid(0, iLaTeXPosX);
                        double fT;
                        if (strPt.ToDouble(&fT)) {
                            if (fT > m_latexOverfullMax) {
                                m_latexOverfullMax = fT;
                            }
                        }
                    }
                }
                //
            }
        }

        strTW = strTi;
        iLaTeXPos = strTW.Find(uT("underfull"));
        if (iLaTeXPos > 0) {
            ++m_latexUnderfull;
            for (int ii = 0; ii < 12; ii++) {
                strTW = strTW.Mid(iLaTeXPos + 8);
                iLaTeXPos = strTW.Find(uT("underfull"));
                if (iLaTeXPos < 1) {
                    break;
                }
                ++m_latexUnderfull;
            }
        }
        //

        if (m_iErrPos < 0) {
            int iErrPos = strT.Find(uT("! "));
            if (iErrPos > 0) {
                wxString strErr = strT.Mid(iErrPos);
                m_iErrPos = (int)(GetLength() - iStrLen + iErrPos);
                for (int ii = 0; ii < 12; ii++) {
                    int iStart = strErr.Find(SCRIPT_NEWLINE);
                    if (iStart < 1) {
                        break;
                    }
                    strErr = strErr.Mid(iStart + 1);
                    iStart = strErr.Find(uT("l."));
                    if ((iStart >= 0) && (iStart <= 2)) {
                        strErr = strErr.Mid(iStart + 2);
                        int iEnd = strErr.Find(uT(" "));
                        if ((iEnd > 0) && (iEnd < 12)) {
                            wxString strN = strErr.Mid(0, iEnd);
                            long iL = -1L;
                            if (strN.ToLong(&iL, 10)) {
                                m_ErrLine.line = ((int)iL) - 1;
                                int iEndX = strErr.Find(uT("\n"));
                                if (iEndX > (iEnd + 1)) {
                                    m_strErrInfo = strErr.Mid(iEnd + 1, iEndX - 1 - (iEnd + 1));
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    else if (m_iRunningLexer == wxSTC_LEX_SOLIS) {

        // specific Solis stuff
        int iErr = strT.Find(uT("[ERROR]"));
        if (iErr > 0) {
            int iErrline = strT.Find(uT(" at line "));
            if (iErrline > (iErr + 7)) {
                wxString strTW = strT.Mid(iErrline + 9);
                int iLen = (int)(strTW.Length());
                if ((iLen > 0) && (iLen < 128)) {
                    for (int ii = 0; ii < iLen; ii++) {
                        if ((strTW[ii] < uT('0')) || (strTW[ii] > uT('9'))) {
                            strTW = strTW.Mid(0, ii);
                            break;
                        }
                    }
                    long iL = -1L;
                    if (strTW.ToLong(&iL, 10)) {
                        m_ErrLine.line = ((int)iL) - 1;
                    }
                }
            }
        }
    }

    int iW = this->TextWidth(wxSTC_STYLE_DEFAULT, strT);
    if (iW > m_iMaxLineWidth) {
        m_iMaxLineWidth = iW;
        this->SetScrollWidth(m_iMaxLineWidth);
        this->Refresh();
    }

    // Always call EmptyUndoBuffer to avoid any unnecessary increasing memory usage
    EmptyUndoBuffer();
    SetReadOnly(true);

    if ((m_iErrPos < 0) && bToErrPos) {
        bToErrPos = false;
    }
    if (this->isFocused()) {
        int iSel = bToErrPos ? m_iErrPos : GetLength();
        SetSelection(iSel, iSel);
        EnsureCaretVisible();
        updateStatusbar();
    }
    else {
        ScrollToLine(bToErrPos ? LineFromPosition(m_iErrPos) : (GetLineCount() - 1));
    }
    if (bToErrPos) {
        m_iErrPos = -1;
    }
}

void OutputEdit::DoEditCopy(void)
{
    if ((GetSelectionEnd() - GetSelectionStart()) <= 0) {
        return;
    }

    CodeEdit::Copy();
}

void OutputEdit::DoEditReset(void)
{
    Reset();
}

void OutputEdit::OnEditAction(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();

    switch (idT) {
        case wxID_COPY:
            DoEditCopy();
            break;
        case wxID_SELECTALL:
            SelectAll();
            break;
        case wxID_CLEAR:
        {
            CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
            if (pFrame != NULL) {
                pFrame->ClearOutputWindow(wxEmptyString, true);
            }
        } break;
        default:
            break;
    }
}

void OutputEdit::OnOutputAction(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();

    if (idT == ID_OUTPUT_ZOOMIN) {
        DoZoomIn();
    }
    else if (idT == ID_OUTPUT_ZOOMOUT) {
        DoZoomOut();
    }
    else if (idT == ID_OUTPUT_ZOOMNONE) {
        DoZoomNone();
    }
    else if (idT == ID_OUTPUT_CLEARKEY) {
        ; // do nothing
    }
}

void OutputEdit::OnMouseWheel(wxMouseEvent &tEvent)
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

void OutputEdit::DoZoomIn(void)
{
    int iF = this->GetFont().GetPointSize();
    if (iF >= 80) {
        return;
    }
    ZoomIn();
    calcStatusMargin();
}

void OutputEdit::DoZoomOut(void)
{
    int iF = this->GetFont().GetPointSize();
    if (iF <= 4) {
        return;
    }
    ZoomOut();
    calcStatusMargin();
}

void OutputEdit::DoZoomNone(void)
{
    SetZoom(0);
    calcStatusMargin();
}
