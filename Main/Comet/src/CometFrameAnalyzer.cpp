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

#include <wx/wupdlock.h>

const int CometFrame::MAX_DROPCOUNT = 16;

BEGIN_EVENT_TABLE(AnalyzerComboBox, CometComboBox)
    EVT_KEY_DOWN(AnalyzerComboBox::OnKeyDown)
    EVT_RIGHT_DOWN(AnalyzerComboBox::OnRightDown)
    EVT_RIGHT_UP(AnalyzerComboBox::OnRightUp)
    EVT_MENU(ID_ANALYZE_RESET, AnalyzerComboBox::OnListReset)
    EVT_MENU(wxID_REPLACE_ALL, AnalyzerComboBox::OnListReset)
END_EVENT_TABLE()

AnalyzerComboBox::AnalyzerComboBox(wxWindow *pParent, wxWindowID idT,
                                   const wxString &valueT /* = wxEmptyString*/,
                                   const wxPoint &posT /* = wxDefaultPosition*/,
                                   const wxSize &sizeT /* = wxDefaultSize*/,
                                   int nn /* = 0*/, const wxString choicesT[] /* = NULL*/,
                                   long styleT /* = 0*/,
                                   const wxValidator &validatorT /* = wxDefaultValidator*/,
                                   const wxString &nameT /* = wxComboBoxNameStr*/)
    : CometComboBox(pParent, idT, valueT, posT, sizeT, nn, choicesT, (styleT | wxTE_PROCESS_ENTER), validatorT, nameT)
{
    addResetMenu(true);
}

void AnalyzerComboBox::OnKeyDown(wxKeyEvent &tEvent)
{
    if (tEvent.GetKeyCode() == WXK_RETURN) {
        wxToolBar *pToolbar = (wxToolBar *)GetParent();
        if (pToolbar) {
            CometFrame *pFrame = static_cast<CometFrame *>(pToolbar->GetParent());
            if (pFrame) {
                pFrame->DoAnalyzerChange();
            }
        }
        int ilen = (int)(this->GetValue().Length());
        this->SetSelection(ilen, ilen);
        return;
    }

    tEvent.Skip();
}

void AnalyzerComboBox::OnRightUp(wxMouseEvent &tEvent)
{
}

void AnalyzerComboBox::OnRightDown(wxMouseEvent &tEvent)
{
    wxPoint ptMousePosition = tEvent.GetPosition();

    wxMenu popMenu;

    wxMenuItem *pMenu = NULL;

    pMenu = new wxMenuItem(&popMenu, wxID_CUT, uT("Cut"), uT("Cut the selected text"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pMenu->SetBitmap(cut_small_xpm);
#endif
    popMenu.Append(pMenu);
    long iStart = 0L, iEnd = 0L;
    GetSelection(&iStart, &iEnd);
    pMenu->Enable((iStart >= 0) && (iEnd >= 0) && (iStart < iEnd));

    pMenu = new wxMenuItem(&popMenu, wxID_COPY, uT("Copy"), uT("Copy the selected text"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pMenu->SetBitmap(copy_small_xpm);
#endif
    popMenu.Append(pMenu);
    pMenu->Enable((iStart >= 0) && (iEnd >= 0) && (iStart < iEnd));

    pMenu = new wxMenuItem(&popMenu, wxID_PASTE, uT("Paste"), uT("Paste text"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pMenu->SetBitmap(paste_small_xpm);
#endif
    popMenu.Append(pMenu);

    pMenu = new wxMenuItem(&popMenu, wxID_CLEAR, uT("Delete"), uT("Delete the selected text"), wxITEM_NORMAL);
    popMenu.Append(pMenu);
    pMenu->Enable((iStart >= 0) && (iEnd >= 0) && (iStart < iEnd));

    popMenu.AppendSeparator();

    pMenu = new wxMenuItem(&popMenu, ID_ANALYZE_RESET, uT("Reset"), uT("Reset the list"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pMenu->SetBitmap(reset_small_xpm);
#endif
    popMenu.Append(pMenu);
    pMenu->Enable(GetCount() > 1);

    PopupMenu(&popMenu, ptMousePosition);
}

void AnalyzerComboBox::OnListReset(wxCommandEvent &tEvent)
{
    int iret = SigmaMessageBox(uT("Reset Find History?"), uT("Comet"),
                               wxYES | wxNO | wxICON_QUESTION | wxNO_DEFAULT, this,
                               uT("&Reset"), uT("D&on't reset"));
    if (iret != wxYES) {
        return;
    }

    SetValue(wxEmptyString);
    Clear();
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame) {
        pFrame->resetFind();
    }
}


int CometFrame::DoAnalyzerUpdateList(ScriptEdit *pEdit, bool bRecreate)
{
    if ((pEdit == NULL) || (m_cboAnalyzer == NULL)) {
        return 0;
    }

    int nn = 0;
    // Freeze --> Update --> Thaw, to avoid flickering
    wxWindowUpdateLocker flickerFree(m_cboAnalyzer);
    nn = pEdit->CodeAnalyzerUpdate(m_cboAnalyzer, bRecreate);
    m_cboAnalyzer->Select(0);
    m_cboAnalyzer->SetToolTip((wxToolTip *)NULL);
    wxString strTT = m_cboAnalyzer->GetValue();
    int tx = 0, ty = 0, txd;
    m_cboAnalyzer->GetTextExtent(strTT, &tx, &ty);
    m_cboAnalyzer->GetTextExtent(uT("Q"), &txd, &ty);
    tx += txd;
    if (tx > m_cboAnalyzer->GetSize().x) {
        m_cboAnalyzer->SetToolTip(strTT);
    }

    return nn;
}

int CometFrame::DoAnalyzerUpdate(bool bRecreate, bool bVerbose /* = true*/)
{
    if (m_cboAnalyzer == NULL) {
        return 0;
    }

    ScriptEdit *pEdit = this->getActiveEditor();
    if (pEdit == NULL) {
        return 0;
    }

    int nn = DoAnalyzerUpdateList(pEdit, bRecreate);

    int iLexer = pEdit->GetLexer();
    if ((iLexer != wxSTC_LEX_LUA) && (iLexer != wxSTC_LEX_PYTHON) && (iLexer != wxSTC_LEX_LATEX) && (iLexer != wxSTC_LEX_CPP) && ((iLexer != wxSTC_LEX_SOLIS) || (LEXERTYPE_FIRST != pEdit->getLexerType()))) {
        return 0;
    }

    wxFileName fnameT = pEdit->GetFilename();

    if (bVerbose && fnameT.FileExists()) {

        wxString strT = uT("'");
        strT += fnameT.GetFullName();
        strT += uT("' listing done");

        if (nn > 0) {
            wxString strF = uT(" with %d ");
            strF += ((iLexer == wxSTC_LEX_LATEX) || (iLexer == wxSTC_LEX_SOLIS)) ? uT("section") : uT("function");
            if (nn > 1) {
                strF += uT('s');
            }
            strF += uT(" found.");

            strT += wxString::Format(LM_CSTR(strF), nn);
        }
        else {
            strT += ((iLexer == wxSTC_LEX_LATEX) || (iLexer == wxSTC_LEX_SOLIS)) ? uT(": no section found.") : uT(": no function found.");
        }

        OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
    }

    return nn;
}

void CometFrame::OnAnalyzerUpdate(wxCommandEvent &WXUNUSED(tEvent))
{
    if (m_cboAnalyzer) {
        DoAnalyzerUpdate(true, true);
    }
}

void CometFrame::OnAnalyzerGoto(wxCommandEvent &WXUNUSED(tEvent))
{
    if (m_cboAnalyzer) {
        // If m_cboAnalyzer not focused, search will be performed from current editor selection
        m_cboAnalyzer->SetFocus();
        DoAnalyzerChange();
    }
}

void CometFrame::OnAnalyzerReset(wxCommandEvent &WXUNUSED(tEvent))
{
    if (m_cboAnalyzer) {
        m_cboAnalyzer->SetValue(wxEmptyString);
        m_cboAnalyzer->Clear();
    }
}

void CometFrame::OnAnalyzerToolbarUpdate(wxUpdateUIEvent &tEvent)
{
    int idT = tEvent.GetId();
    if (idT != ID_MAIN_ANALYZE) {
        tEvent.Skip();
        return;
    }

    ScriptEdit *pEdit = this->getActiveEditor();
    if (pEdit == NULL) {
        CometFrame::enableUIitem(tEvent, false);
        return;
    }

    bool bEnable = false;
    int iLexer = pEdit->GetLexer();
    switch (iLexer) {
        case wxSTC_LEX_LUA:
        case wxSTC_LEX_PYTHON:
        case wxSTC_LEX_LATEX:
        case wxSTC_LEX_CPP:
        case wxSTC_LEX_SOLIS:
            bEnable = (LEXERTYPE_FIRST == pEdit->getLexerType());
            break;
        default:
            bEnable = false;
            break;
    }

    CometFrame::enableUIitem(tEvent, bEnable);
    // :WARNING: Do not update m_cboAnalyzer here
    // :WARNING: In general do not update any UI inside OnUpdateUIxxx function
}

bool CometFrame::AnalyzerIsFindSelected(void)
{
    if (m_cboAnalyzer == NULL) {
        return false;
    }

    ScriptEdit *pEdit = this->getActiveEditor();
    if (pEdit == NULL) {
        return false;
    }

    wxString strFind = m_cboAnalyzer->GetValue();
    if (strFind == wxEmptyString) {
        return false;
    }

    if (CodeAnalyzer::isSeparator(strFind)) {
        return false;
    }

    int indexT = pEdit->CodeAnalyzerGetIndex(LM_CSTR(strFind));

    return ((indexT < 0) || (indexT >= pEdit->CodeAnalyzerGetCount()));
}

void CometFrame::DoAnalyzerChange(const wxString &strFunc, bool bSelect /* = false*/)
{
    if (m_cboAnalyzer == NULL) {
        return;
    }

    ScriptEdit *pEdit = this->getActiveEditor();
    if (pEdit == NULL) {
        return;
    }

    size_t iFuncCount = pEdit->CodeAnalyzerGetCount();

    const int indexT = pEdit->CodeAnalyzerGetIndex(LM_CSTR(strFunc));

    // Local function?
    if ((indexT < 0) || (indexT >= (int)(iFuncCount))) {
        return;
    }
    //

    // Function
    int iLexer = pEdit->GetLexer();
    if ((iLexer != wxSTC_LEX_LUA) && (iLexer != wxSTC_LEX_PYTHON) && (iLexer != wxSTC_LEX_LATEX) && (iLexer != wxSTC_LEX_CPP) && ((iLexer != wxSTC_LEX_SOLIS) || (LEXERTYPE_FIRST != pEdit->getLexerType()))) {
        return;
    }

    int iLine = pEdit->CodeAnalyzerGetLine(indexT);
    wxString strLine = pEdit->GetLine(iLine);
    strLine = SigmaTrim(strLine);

    const AnalyzerElement::SCOPE iScope = pEdit->CodeAnalyzerGetScope(indexT);

    bool bFound = false;

    if (iLexer == wxSTC_LEX_LUA) {
        if (iScope == AnalyzerElement::SCOPE_LOCAL) {
            bFound = (strLine.StartsWith(uT("local")) && strLine.Contains(uT("function")) && strLine.Contains(strFunc));
        }
        else {
            bFound = (strLine.StartsWith(uT("function")) && strLine.Contains(strFunc)) || (strLine.StartsWith(strFunc) && strLine.Contains(uT("function")));
        }
    }
    else if (iLexer == wxSTC_LEX_PYTHON) {
        bFound = (strLine.StartsWith(uT("def")) && strLine.Contains(strFunc) && strLine.EndsWith(uT(":")));
    }
    else if (iLexer == wxSTC_LEX_LATEX) {
        bFound = ((strLine.StartsWith(uT("\\section")) || strLine.StartsWith(uT("\\subsection")) || strLine.StartsWith(uT("\\subsubsection"))) && strLine.Contains(strFunc) && strLine.EndsWith(uT("}")));
    }
    else if (iLexer == wxSTC_LEX_CPP) {
        bFound = (strLine.StartsWith(strFunc) && strLine.Contains(uT("(")));
    }
    else if (iLexer == wxSTC_LEX_SOLIS) {
        bFound = (strLine.StartsWith(uT("[")) && strLine.EndsWith(uT("]")));
    }

    if (bFound == false) {
        // Retry
        int iMin = iLine - 12;
        int iMax = iLine + 12;
        int iLines = pEdit->GetLineCount();
        if (iMin < 0) {
            iMin = 0;
        }
        if (iMax >= iLines) {
            iMax = iLines - 1;
        }
        for (int ii = iMin; ii <= iMax; ii++) {
            strLine = pEdit->GetLine(ii);
            strLine = SigmaTrim(strLine);

            if (iLexer == wxSTC_LEX_LUA) {
                if (iScope == AnalyzerElement::SCOPE_LOCAL) {
                    bFound = (strLine.StartsWith(uT("local")) && strLine.Contains(uT("function")) && strLine.Contains(strFunc));
                }
                else {
                    bFound = (strLine.StartsWith(uT("function")) && strLine.Contains(strFunc)) || (strLine.StartsWith(strFunc) && strLine.Contains(uT("function")));
                }
            }
            else if (iLexer == wxSTC_LEX_PYTHON) {
                bFound = (strLine.StartsWith(uT("def")) && strLine.Contains(strFunc) && strLine.EndsWith(uT(":")));
            }
            else if (iLexer == wxSTC_LEX_LATEX) {
                bFound = ((strLine.StartsWith(uT("\\section")) || strLine.StartsWith(uT("\\subsection"))) && strLine.Contains(strFunc) && strLine.EndsWith(uT("}")));
            }
            else if (iLexer == wxSTC_LEX_CPP) {
                bFound = (strLine.StartsWith(strFunc) && strLine.Contains(uT("(")));
            }
            else if (iLexer == wxSTC_LEX_SOLIS) {
                bFound = (strLine.StartsWith(uT("[")) && strLine.EndsWith(uT("]")));
            }

            if (bFound) {
                pEdit->CodeAnalyzerSetLine(LM_CSTR(strFunc), ii);
                iLine = ii;
                break;
            }
        }
    }

    if (bFound) {

        pEdit->DoGotoLine(iLine, bSelect);

        m_cboAnalyzer->SetToolTip((wxToolTip *)NULL);

        wxString strT = uT("");
        if (iLexer == wxSTC_LEX_LUA) {
            if (iScope == AnalyzerElement::SCOPE_LOCAL) {
                strT = uT("local ");
            }
        }
        strT += strFunc;

        if (bSelect) {
            // select item (when called from 'goto function')
            for (int ii = 0; ii < pEdit->CodeAnalyzerGetCount(); ii++) {
                if (strT.IsSameAs(m_cboAnalyzer->GetString(static_cast<size_t>(ii)))) {
                    m_cboAnalyzer->SetSelection(ii);
                    m_cboAnalyzer->SetToolTip(strT);
                    break;
                }
            }
        }
        else {
            // already selected
            m_cboAnalyzer->SetToolTip(strT);
        }

        return;
    }

    wxString strT = uT("'");
    strT += strFunc;
    strT += uT("' not found. Please update the functions/sections list.");

    OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
    return;
}

void CometFrame::DoAnalyzerChange(void)
{
    if (m_cboAnalyzer == NULL) {
        return;
    }

    ScriptEdit *pEdit = this->getActiveEditor();
    if (pEdit == NULL) {
        return;
    }

    wxString strFind = m_cboAnalyzer->GetValue();

    if (CodeAnalyzer::isSeparator(strFind)) {
        int iSel = m_cboAnalyzer->GetSelection();
        wxString strT = m_cboAnalyzer->GetString(iSel + 1);
        m_cboAnalyzer->SetSelection(iSel + 1);
        m_cboAnalyzer->SetToolTip((wxToolTip *)NULL);
        m_cboAnalyzer->SetToolTip(strT);
        return;
    }

    size_t iLen = strFind.Length();

    // Go To Line
    long iT;
    if ((iLen > 0) && (iLen <= 6) && strFind.ToLong(&iT)) {

        int iLine = (int)(iT)-1;

        if ((iLine >= 0) && (iLine < pEdit->GetLineCount())) {

            pEdit->DoGotoLine(iLine);

            return;
        }
    }
    //

    size_t iTotal = m_cboAnalyzer->GetCount();
    size_t iFuncCount = pEdit->CodeAnalyzerGetCount();
    size_t iFindFirst = iFuncCount + 1;

    int indexT = pEdit->CodeAnalyzerGetIndex(LM_CSTR(strFind));

    // Local function?
    if ((indexT < 0) || (indexT >= (int)(iFuncCount))) {
        if (strFind.StartsWith(uT("local"))) {
            wxString strFunc = SigmaTrim(strFind.Mid(6));
            indexT = pEdit->CodeAnalyzerGetIndex(LM_CSTR(strFunc));
        }
    }
    //

    // Find
    if ((indexT < 0) || (indexT >= (int)(iFuncCount))) {
        m_cboAnalyzer->SetToolTip((wxToolTip *)NULL);
        if (iTotal > iFindFirst) {
            for (size_t ii = iFindFirst; ii < iTotal; ii++) {
                if (strFind.IsSameAs(m_cboAnalyzer->GetString((unsigned int)ii))) {
                    m_cboAnalyzer->SetSelection((int)ii);
                    m_cboAnalyzer->SetToolTip(strFind);
                    break;
                }
            }
        }
        DoFindPrevNext(ID_FIND_NEXT);
        return;
    }
    //

    // Function
    int iLexer = pEdit->GetLexer();
    if ((iLexer != wxSTC_LEX_LUA) && (iLexer != wxSTC_LEX_PYTHON) && (iLexer != wxSTC_LEX_LATEX) && (iLexer != wxSTC_LEX_CPP) && ((iLexer != wxSTC_LEX_SOLIS) || (LEXERTYPE_FIRST != pEdit->getLexerType()))) {
        return;
    }

    wxString strFunc = pEdit->CodeAnalyzerGetName(indexT);
    if (strFunc == wxEmptyString) {
        int nn = DoAnalyzerUpdate(true, true);
        if (nn > 0) {
            strFunc = pEdit->CodeAnalyzerGetName(indexT);
            if (strFunc == wxEmptyString) {
                return;
            }
        }
        else {
            return;
        }
    }

    DoAnalyzerChange(strFunc, true);
}

void CometFrame::OnAnalyzerChange(wxCommandEvent &tEvent)
{
    tEvent.Skip();

    DoAnalyzerChange();
}

BEGIN_EVENT_TABLE(CodeInfobar, wxWindow)
    EVT_RIGHT_DOWN(CodeInfobar::OnRightContext)
    EVT_LEFT_DOWN(CodeInfobar::OnMouseDown)
    EVT_PAINT(CodeInfobar::OnPaint)
    EVT_ERASE_BACKGROUND(CodeInfobar::OnEraseBackground)
END_EVENT_TABLE()

CodeInfobar::CodeInfobar(wxPanel *pPanel, int iId, wxSize sizeT, const wxString &strT, int iType, bool bActive) : wxWindow(pPanel, iId, wxDefaultPosition, sizeT, wxNO_BORDER, strT)
{
    m_iType = iType;
    m_bActive = bActive;
    m_iHeaderSize = 0;
    m_iScrollSize = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
}

void CodeInfobar::OnPaint(wxPaintEvent &evt)
{
    wxPaintDC dc(this);

    if (false == m_bActive) {
        erase(dc);
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        erase(dc);
        return;
    }

    CodeEdit *pEdit = NULL;
    if (INFOBAR_TYPE_TOOL == m_iType) {
        CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
        if (false == pFrame->isConsoleVisible()) {
            erase(dc);
            return;
        }
        pEdit = (CodeEdit *)(pFrame->getConsole());
    }
    else {
        pEdit = (CodeEdit *)(pFrame->getActiveEditor());
    }

    if (NULL == pEdit) {
        erase(dc);
        return;
    }

    draw(dc, pEdit);
}

void CodeInfobar::draw(wxPaintDC &dc, CodeEdit *pEdit)
{
    if ((false == m_bActive) || (NULL == pEdit)) {
        erase(dc);
        return;
    }

    if (INFOBAR_TYPE_TOOL == m_iType) {
        CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
        if (false == pFrame->isConsoleVisible()) {
            erase(dc);
            return;
        }
    }

    if (0 == m_iHeaderSize) {
        CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
        wxAuiNotebook *pNotebook = pFrame->getNotebookMain();
        if (pNotebook) {
            m_iHeaderSize = pNotebook->GetTabCtrlHeight();
        }
    }

    int lineHeight = (int)(pEdit->TextHeight(0));
    int iLineCount = (int)(pEdit->GetLineCount());
    int iLinesOnScreen = (int)(pEdit->LinesOnScreen());
    int iScrollSize = 0;

    double fLineHeight = 0.0, fDH = 0.0;

    if (iLinesOnScreen >= iLineCount) {
        fLineHeight = (double)(lineHeight);
    }
    else {
        iScrollSize = m_iScrollSize;
        double fHfull = (double)(lineHeight * iLineCount);
        double fHvis = (double)(lineHeight * iLinesOnScreen);
        double fRatio = (fHvis - (double)(2 * iScrollSize)) / fHfull;
        fLineHeight = fRatio * (double)(lineHeight);
    }
    fDH = 0.5 * fLineHeight;

    unsigned long lBackModified, lBackSaved, lBackFind, lBackError, lBackBreakpoint = 0x88A1E0;
    pEdit->getMarkersColors(&lBackModified, &lBackSaved, &lBackFind, &lBackError);

    wxColour clrBack(pEdit->GetBackgroundColour());
    wxColour clrError(lBackError);
    wxColour clrBreakpoint(lBackBreakpoint);
    wxColour clrMarker(lBackFind);
    wxColour clrBookmark(lBackFind);
    wxColour clrModified(lBackModified);
    wxColour clrSaved(lBackSaved);
    wxBrush brushBack(clrBack);
    wxBrush brushError(clrError);
    wxBrush brushDebug(wxColour(0x006EDE));
    wxBrush brushBreakpoint(clrBreakpoint);
    wxBrush brushBookmark(clrBookmark);
    wxBrush brushMarker(clrMarker);
    wxBrush brushModified(clrModified);
    wxBrush brushSaved(clrSaved);

    wxRect rcInfobar = GetClientRect();

    wxBitmap bitmapT(rcInfobar.width, rcInfobar.height);
    wxMemoryDC memDC(bitmapT);

    memDC.SetPen(*wxTRANSPARENT_PEN);
    memDC.SetBrush(brushBack);
    memDC.DrawRectangle(0, 0, rcInfobar.width, rcInfobar.height);

    int iDD = 0;
    int iMid = INFOBAR_WIDTH / 2;

    int iLine = 0;
    int iMask = 0;
    double fPosY, fPosYD = (double)(m_iHeaderSize + iScrollSize) + fDH,
                  fYm = (double)(rcInfobar.GetBottom()) - (2.0 * fDH);
    int iDH = 1 + (int)(round(fDH));
    if (iDH < 2) {
        iDH = 2;
    }
    if (0 != (iDH % 2)) {
        iDH += 1;
    }
    int iPosY, iPosYs, iLH = 2 * iDH, iPosYl = -1;

    const int iMaskX = (INFOBAR_TYPE_MAIN == m_iType) ? (SCRIPT_MASK_ERROR | SCRIPT_MASK_DEBUG | SCRIPT_MASK_BREAKPOINT | SCRIPT_MASK_FIND | SCRIPT_MASK_BOOKMARK | SCRIPT_MASK_MODIFIED | SCRIPT_MASK_SAVED) : (SCRIPT_MASK_SAVED);

    int iLinePrev = 0;

    while (iLine < iLineCount) {

        iLine = pEdit->MarkerNext(iLinePrev, iMaskX);
        if (iLine < iLinePrev) {
            break;
        }
        iMask = pEdit->MarkerGet(iLine);
        if (0 == iMask) {
            break;
        }

        fPosY = fPosYD + (fLineHeight * (double)(iLine));
        if (fPosY > fYm) {
            break;
        }

        iPosY = 1 + (int)(round(fPosY));
        iPosYs = iPosY - iDH;
        if (iPosYs < (iPosYl - iDH)) {
            // no overlapped markers
            iLinePrev = iLine + 1;
            continue;
        }
        iPosYl = iPosYs + iLH;

        if ((iMask & SCRIPT_MASK_MODIFIED) != 0) {
            memDC.SetBrush(brushModified);
            memDC.DrawRectangle(iDD, iPosYs, iMid, iLH);
        }
        else if ((iMask & SCRIPT_MASK_SAVED) != 0) {
            memDC.SetBrush(brushSaved);
            memDC.DrawRectangle(iDD, iPosYs, iMid, iLH);
        }

        if ((iMask & SCRIPT_MASK_ERROR) != 0) {
            memDC.SetBrush(brushError);
            memDC.DrawRectangle(iDD + iMid, iPosYs, iMid, iLH);
        }
        else if ((iMask & SCRIPT_MASK_DEBUG) != 0) {
            memDC.SetBrush(brushDebug);
            memDC.DrawRectangle(iDD + iMid, iPosYs, iMid, iLH);
        }
        else if ((iMask & SCRIPT_MASK_BREAKPOINT) != 0) {
            memDC.SetBrush(brushBreakpoint);
            memDC.DrawRectangle(iDD + iMid, iPosYs, iMid, iLH);
        }
        else if ((iMask & SCRIPT_MASK_FIND) != 0) {
            memDC.SetBrush(brushMarker);
            memDC.DrawRectangle(iDD + iMid, iPosYs, iMid, iLH);
        }
        else if ((iMask & SCRIPT_MASK_BOOKMARK) != 0) {
            memDC.SetBrush(brushBookmark);
            memDC.DrawRectangle(iDD + iMid, iPosYs, iMid, iLH);
        }

        iLinePrev = iLine + 1;
    }

    dc.Blit(rcInfobar.x, rcInfobar.y, rcInfobar.width, rcInfobar.height, &memDC, 0, 0, wxCOPY);

    memDC.SetPen(wxNullPen);
    memDC.SetFont(wxNullFont);
    memDC.SetBrush(wxNullBrush);
    memDC.SelectObject(wxNullBitmap);

#ifdef __WXMSW__
    bitmapT.FreeResource(true);
    brushBack.FreeResource(true);
    brushError.FreeResource(true);
    brushDebug.FreeResource(true);
    brushBreakpoint.FreeResource(true);
    brushMarker.FreeResource(true);
    brushBookmark.FreeResource(true);
    brushModified.FreeResource(true);
    brushSaved.FreeResource(true);
#endif
}

void CodeInfobar::erase(wxPaintDC &dc)
{
    wxRect rcInfobar = GetClientRect();

    wxColour clrBack(GetBackgroundColour());
    wxBrush brushBack(clrBack);

    wxBitmap bitmapT(rcInfobar.width, rcInfobar.height);
    wxMemoryDC memDC(bitmapT);

    memDC.SetPen(*wxTRANSPARENT_PEN);
    memDC.SetBrush(brushBack);
    memDC.DrawRectangle(0, 0, rcInfobar.width, rcInfobar.height);

    dc.Blit(rcInfobar.x, rcInfobar.y, rcInfobar.width, rcInfobar.height, &memDC, 0, 0, wxCOPY);

    memDC.SetPen(wxNullPen);
    memDC.SetBrush(wxNullBrush);
    memDC.SelectObject(wxNullBitmap);

#ifdef __WXMSW__
    bitmapT.FreeResource(true);
    brushBack.FreeResource(true);
#endif
}

void CodeInfobar::OnEraseBackground(wxEraseEvent &WXUNUSED(evt))
{
    // do nothing to help avoid flashing
}

void CodeInfobar::OnRightContext(wxMouseEvent &evt)
{
    if (false == m_bActive) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }
    ScriptEdit *pEdit = pFrame->getActiveEditor();
    if (NULL == pEdit) {
        return;
    }

    if (pEdit->containsMarkers()) {
        wxPoint ptMousePosition = evt.GetPosition();
        wxMenu popMenu;

        wxMenuItem *pItem = new wxMenuItem(&popMenu, ID_FILE_RESETMARKERS, uT("Remove Markers"), uT("Remove all markers"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
        pItem->SetBitmap(reset_small_xpm);
#endif
        popMenu.Append(pItem);

        this->PopupMenu(&popMenu, ptMousePosition);
    }
}

void CodeInfobar::LineFromPos(wxPoint pt, int *piLastStart, int *piLineEnd)
{
    if (false == m_bActive) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }
    ScriptEdit *pEdit = pFrame->getActiveEditor();
    if (NULL == pEdit) {
        return;
    }

    int iLineCount = (int)(pEdit->GetLineCount());
    int iLinesOnScreen = (int)(pEdit->LinesOnScreen());
    int lineHeight = (int)(pEdit->TextHeight(0));

    wxRect rcInfobar = GetClientRect();

    int iW = rcInfobar.GetWidth();
    int iH = rcInfobar.GetHeight();

    int iX = pt.x;
    int iY = pt.y;

    double fLineHeight = (double)(lineHeight);

    if ((iW >= 2) && (iH >= 16) && (iX >= rcInfobar.GetLeft()) && (iX <= rcInfobar.GetRight()) && (iY >= rcInfobar.GetTop()) && (iY <= rcInfobar.GetBottom())) {

        iY -= m_iHeaderSize;
        iH -= m_iHeaderSize;

        int iDH = 0;
        if (iLinesOnScreen < iLineCount) {
            double fHfull = (double)(lineHeight * iLineCount);
            double fHvis = (double)(lineHeight * iLinesOnScreen);
            fLineHeight *= ((fHvis - (double)(2 * m_iScrollSize)) / fHfull);
            lineHeight = (int)(fLineHeight);
            iY -= m_iScrollSize;
            iH -= m_iScrollSize;

            if (fLineHeight <= 4.0) {
                iDH = 1 + (int)(round(fLineHeight * 0.5));
                if (iDH < 2) {
                    iDH = 2;
                }
                if (0 != (iDH % 2)) {
                    iDH += 1;
                }
            }
        }

        *piLastStart = (int)(floor((double)(iY - iDH) / fLineHeight));
        *piLineEnd = (int)(floor((double)(iY + iDH) / fLineHeight));
    }
}

void CodeInfobar::OnMouseDown(wxMouseEvent &evt)
{
    if (false == m_bActive) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }
    ScriptEdit *pEdit = pFrame->getActiveEditor();
    if (NULL == pEdit) {
        return;
    }

    int iLineCount = (int)(pEdit->GetLineCount());

    wxRect rcInfobar = GetClientRect();

    wxPoint pt = evt.GetPosition();

    int iLastStart = 0, iLineEnd = 0;
    LineFromPos(pt, &iLastStart, &iLineEnd);

    const int iMaskX = (pt.x <= (rcInfobar.GetLeft() + (rcInfobar.GetWidth() / 2))) ? (SCRIPT_MASK_MODIFIED | SCRIPT_MASK_SAVED) : (SCRIPT_MASK_ERROR | SCRIPT_MASK_DEBUG | SCRIPT_MASK_BREAKPOINT | SCRIPT_MASK_FIND | SCRIPT_MASK_BOOKMARK);
    int iNextLine = pEdit->MarkerNext(iLastStart, iMaskX);
    if ((iNextLine >= iLastStart) && (iNextLine <= iLineEnd)) {
        int iMask = pEdit->MarkerGet(iNextLine);
        if (0 != iMask) {
            if ((iNextLine >= 0) && (iNextLine < iLineCount)) {
                if (pEdit->isFocused() == false) {
                    pEdit->SetFocus();
                }
                pEdit->GotoLine(iNextLine);
            }
        }
    }
}
