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
#include "FindFileDlg.h"
#include "ScriptEdit.h"

#define STATUSCOLOR_RED  wxColour(120, 0, 20)
#define STATUSCOLOR_BLUE wxColour(0, 80, 120)

BEGIN_EVENT_TABLE(FindReplaceComboBox, CometComboBox)
EVT_KEY_DOWN(FindReplaceComboBox::OnKeyDown)
END_EVENT_TABLE()

FindReplaceComboBox::FindReplaceComboBox(wxWindow *pParent, wxWindowID idT,
                                         const wxString &valueT /* = wxEmptyString*/,
                                         const wxPoint &posT /* = wxDefaultPosition*/,
                                         const wxSize &sizeT /* = wxDefaultSize*/,
                                         int nn /* = 0*/, const wxString choicesT[] /* = NULL*/,
                                         long styleT /* = 0*/,
                                         const wxValidator &validatorT /* = wxDefaultValidator*/,
                                         const wxString &nameT /* = wxComboBoxNameStr*/)
    : CometComboBox(pParent, idT, valueT, posT, sizeT, nn, choicesT, styleT | wxTE_PROCESS_ENTER, validatorT, nameT)
{
    m_bReplace = false;
}

void FindReplaceComboBox::OnKeyDown(wxKeyEvent &tEvent)
{
    if (tEvent.GetKeyCode() == WXK_RETURN) {
        FindFileDlg *pParentDlg = (FindFileDlg *)GetParent();
        if (pParentDlg) {
            pParentDlg->DoFindReplace(isReplace());
        }
        return;
    }

    tEvent.Skip();
}

BEGIN_EVENT_TABLE(FindFileDlg, wxDialog)
    EVT_BUTTON(ID_FIND_PREV, FindFileDlg::OnFindPrev)
    EVT_BUTTON(ID_FIND_NEXT, FindFileDlg::OnFindNext)
    EVT_BUTTON(ID_FIND_FINDALL, FindFileDlg::OnFindFindAll)
    EVT_BUTTON(ID_FIND_REPLACE, FindFileDlg::OnFindReplace)
    EVT_BUTTON(ID_FIND_REPLACEALL, FindFileDlg::OnFindReplaceAll)
    EVT_MENU(ID_SEARCH_FIND, FindFileDlg::OnMainAction)
    EVT_MENU(ID_SEARCH_REPLACE, FindFileDlg::OnMainAction)
    EVT_MENU(ID_SEARCH_FINDINFILES, FindFileDlg::OnMainAction)
    EVT_MENU(ID_SEARCH_REPLACEINFILES, FindFileDlg::OnMainAction)
    EVT_BUTTON(ID_FIND_CLOSE, FindFileDlg::OnFindClose)
    EVT_COMBOBOX(IDE_FIND_WHAT, FindFileDlg::OnWhatChange)
    EVT_RADIOBOX(IDO_FIND_WHERE, FindFileDlg::OnWhereChange)
    EVT_TEXT(IDE_FIND_WHAT, FindFileDlg::OnWhatChange)
    EVT_CHECKBOX(IDK_FIND_MARKER, FindFileDlg::OnMarker)
    EVT_CHECKBOX(IDK_FIND_CYCLIC, FindFileDlg::OnCyclic)
    EVT_ACTIVATE(FindFileDlg::OnActivate)
    EVT_CLOSE(FindFileDlg::OnCloseWindow)
END_EVENT_TABLE()

FindFileDlg::FindFileDlg(wxWindow *pParent, wxString strFind /* = wxEmptyString*/, bool bReplace /* = false*/,
                         const wxPoint &ptInit, wxWindowID idT, const wxString &strTitle, const wxSize &sizeT, long styleT)
    : wxDialog(pParent, idT, strTitle, ptInit, sizeT, styleT & ~wxRESIZE_BORDER)
{
    m_strFind = strFind;
    m_iCurLine = -1;
    m_iFindPos = -1;

    m_bReplace = bReplace;

    m_cboFind = NULL;
    m_cboReplace = NULL;

    m_optCase = NULL;
    m_optWord = NULL;
    m_optRegExp = NULL;
    m_optCyclic = NULL;

    m_optMarker = NULL;

    m_txtStatus = NULL;

    m_btnFindPR = NULL;
    m_btnFindNext = NULL;
    m_btnFindAll = NULL;

    m_ptInit = ptInit;

    initInterface();

    wxSize sizeM = GetSize();
    SetMinSize(sizeM);

    // Accelerators
    const int nCount = 8;
    wxAcceleratorEntry accelEntries[nCount];
    accelEntries[0].Set(wxACCEL_NORMAL, WXK_F3, ID_FIND_NEXT);
    accelEntries[1].Set(wxACCEL_ALT, WXK_F3, ID_FIND_PREV);
    accelEntries[2].Set(wxACCEL_CTRL, (int)'F', ID_SEARCH_FIND);
    accelEntries[3].Set(wxACCEL_CTRL, (int)'H', ID_SEARCH_REPLACE);
    accelEntries[4].Set(wxACCEL_CTRL, (int)'R', ID_SEARCH_REPLACE);
    accelEntries[5].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'F', ID_SEARCH_FINDINFILES);
    accelEntries[6].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'H', ID_SEARCH_REPLACEINFILES);
    accelEntries[7].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'R', ID_SEARCH_REPLACEINFILES);
    wxAcceleratorTable accelTable(nCount, accelEntries);
    SetAcceleratorTable(accelTable);
    //
}

FindFileDlg::~FindFileDlg()
{
}

void FindFileDlg::updateFindList(const wxString &strFind)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    int ii;

    bool bFound = false;
    wxString strT;
    m_cboFind->Clear();
    if (pFrame->getFindCount() > 0) {
        for (ii = 0; ii < pFrame->getFindCount(); ii++) {
            strT = pFrame->getFindItem(ii);
            m_cboFind->AppendString(pFrame->getFindItem(ii));
            if ((false == bFound) && strT.IsSameAs(strFind, true)) {
                bFound = true;
            }
        }
    }
    if (false == bFound) {
        m_cboFind->Insert(strFind, 0);
    }
    m_cboFind->SetValue(strFind);
    m_cboFind->SelectAll();

    pFrame->updateFindValue(strFind);
}

void FindFileDlg::updateReplaceList(const wxString &strReplace)
{
    if (m_bReplace == false) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    int ii;

    m_cboReplace->Clear();
    if (pFrame->getReplaceCount() > 0) {
        for (ii = 0; ii < pFrame->getReplaceCount(); ii++) {
            m_cboReplace->AppendString(pFrame->getReplaceItem(ii));
        }
    }

    m_cboReplace->SetValue(strReplace);
    m_cboReplace->SelectAll();
}

void FindFileDlg::updateLabel(const wxString &strShort, const wxString &strLong)
{
    m_txtStatus->SetLabel(strShort);
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }
    pFrame->OutputStatusbar(strLong, SIGMAFRAME_TIMER_SHORT);
}

void FindFileDlg::updateWhere(bool bSearchInSel)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    ScriptEdit *pEdit = static_cast<ScriptEdit *>(pFrame->getActiveEditor());
    if (pEdit) {
        bool bEnable = ((pEdit->GetSelectionEnd() - pEdit->GetSelectionStart()) > (int)(m_strFind.Length()));

        if (bSearchInSel && bEnable) {
            m_optWhere->SetSelection(2);
        }

        if ((m_optWhere->GetSelection() == 2) && (bEnable == false)) {
            // Unselect before disabling
            m_optWhere->SetSelection(0);
        }

        m_optWhere->Enable(2, bEnable);
    }
}

void FindFileDlg::updateItems(const wxString &strFind, const wxString &strReplace, bool bSearchInSel /* = false*/)
{
    updateFindList(strFind);
    updateReplaceList(strReplace);

    updateLabel(uT(""), uT(""));

    updateWhere(bSearchInSel);
}

void FindFileDlg::updateSel(void)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    if (false == m_bReplace) {
        m_optWhere->Enable(3, (false == m_bReplace) && pFrame->isOutputVisible());
    }

    ScriptEdit *pEdit = static_cast<ScriptEdit *>(pFrame->getActiveEditor());
    if (pEdit) {
        wxString strFind = m_cboFind->GetValue();
        bool bEnable = ((pEdit->GetSelectionEnd() - pEdit->GetSelectionStart()) > (int)(strFind.Length()));
        if ((m_optWhere->GetSelection() == 2) && (bEnable == false)) {
            // Unselect before disabling
            m_optWhere->SetSelection(0);
        }
        m_optWhere->Enable(2, bEnable);
        m_cboFind->SetFocus();
        m_cboFind->SelectAll();
    }
}

void FindFileDlg::initInterface(void)
{
    SetHelpText(m_bReplace ? FindFileDlg_TITLEX : FindFileDlg_TITLE);
    if (FindFileDlg::ShowToolTips()) {
        SetToolTip(m_bReplace ? FindFileDlg_TITLEX : FindFileDlg_TITLE);
    }
    SetTitle(m_bReplace ? FindFileDlg_TITLEX : FindFileDlg_TITLE);

    SetIcon((char **)find_small_xpm);

    wxBoxSizer *pSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pSizer);

    wxBoxSizer *pSizerA = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerAx = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerAR = NULL;
    wxBoxSizer *pSizerTAR = NULL;
    if (m_bReplace) {
        pSizerAR = new wxBoxSizer(wxHORIZONTAL);
        pSizerTAR = new wxBoxSizer(wxHORIZONTAL);
    }
    wxBoxSizer *pSizerAM = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerB = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerBB = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerD = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerDD = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer *pSizerTA = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerTS = new wxBoxSizer(wxHORIZONTAL);

    int iBtnCount = 2;

    wxSize sizeBtn = CometApp::getTextExtent(this, uT(">>Match whole word<<"));
    int iHedt = sizeBtn.GetHeight();
    int iHbtn = iHedt + 10;
    sizeBtn.SetHeight(iHbtn);

    wxSize sizeEdit;
    sizeEdit.SetHeight(iHbtn);
    sizeEdit.SetWidth((sizeBtn.GetWidth() * iBtnCount) + ((iBtnCount - 1) * 4));

    wxSize sizeCheck = sizeBtn;
    wxSize sizeCheckSm = CometApp::getTextExtent(this, uT(">>Add marker<<"));
    sizeCheck.SetHeight(sizeCheckSm.GetHeight());

    wxSize sizeText;
    sizeText.SetHeight(iHedt);
    sizeText.SetWidth((sizeBtn.GetWidth() * iBtnCount) + ((iBtnCount - 1) * 4));

    m_cboFind = new FindReplaceComboBox(this, IDE_FIND_WHAT, uT(""), wxDefaultPosition, sizeEdit, 0, NULL, 0);
    m_cboFind->setReplace(false);

    if (m_bReplace) {
        m_cboReplace = new FindReplaceComboBox(this, IDE_FIND_REPLACE, uT(""), wxDefaultPosition, sizeEdit, 0, NULL, 0);
        m_cboReplace->setReplace(true);
    }

    wxArrayString arrOpen;
    arrOpen.Add(uT("Current Document"));
    arrOpen.Add(uT("All Open Documents"));
    arrOpen.Add(uT("Selected Text"));
    if (false == m_bReplace) {
        arrOpen.Add(uT("Output Window"));
    }
    wxSize sizeBox = sizeEdit;
    sizeBox.SetHeight((false == m_bReplace) ? ((iHbtn * 4) + 16) : ((iHbtn * 3) + 16));
    m_optWhere = new wxRadioBox(this, IDO_FIND_WHERE, uT("Look in:"), wxDefaultPosition, sizeBox, arrOpen, 1, wxRA_SPECIFY_COLS, wxDefaultValidator, uT("Look in"));
    m_optWhere->SetSelection(0);

    m_optRegExp = new wxCheckBox(this, IDK_FIND_REGEXP, uT("Regular e&xpression"), wxDefaultPosition, sizeCheck, 0);
    m_optCase = new wxCheckBox(this, IDK_FIND_CASE, uT("Match cas&e"), wxDefaultPosition, sizeCheck, 0);
    m_optCyclic = new wxCheckBox(this, IDK_FIND_CYCLIC, uT("C&yclic search"), wxDefaultPosition, sizeCheck, 0);
    m_optWord = new wxCheckBox(this, IDK_FIND_WORD, uT("Match whole &word"), wxDefaultPosition, sizeCheck, 0);
    m_optMarker = new wxCheckBox(this, IDK_FIND_MARKER, uT("Add &marker"), wxDefaultPosition, sizeCheck, 0);

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame != NULL) {
        m_optMarker->SetValue(pFrame->isFindMarkerEnabled());
        m_optCyclic->SetValue(pFrame->isFindCyclicEnabled());
    }

    m_txtStatus = new wxStaticText(this, wxID_ANY, uT(""), wxDefaultPosition, sizeText, 0);
    m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);

    m_btnFindNext = new wxButton(this, ID_FIND_NEXT, uT("Find &Next"), wxDefaultPosition, sizeBtn, 0);
    if (m_bReplace) {
        m_btnFindPR = new wxButton(this, ID_FIND_REPLACE, uT("&Replace"), wxDefaultPosition, sizeBtn, 0);
    }
    else {
        m_btnFindPR = new wxButton(this, ID_FIND_PREV, uT("Find &Previous"), wxDefaultPosition, sizeBtn, 0);
    }
    if (m_bReplace) {
        m_btnFindAll = new wxButton(this, ID_FIND_REPLACEALL, uT("Rep&lace All"), wxDefaultPosition, sizeBtn, 0);
    }
    else {
        m_btnFindAll = new wxButton(this, ID_FIND_FINDALL, uT("Find &All"), wxDefaultPosition, sizeBtn, 0);
    }
    m_btnClose = new wxButton(this, ID_FIND_CLOSE, uT("&Close"), wxDefaultPosition, sizeBtn, 0);

    pSizerTA->AddSpacer(8);
    pSizerTA->Add(new wxStaticText(this, wxID_ANY, uT("Find what:"), wxDefaultPosition, sizeText, 0), 0, wxALL, 0);
    pSizerTA->AddSpacer(8);

    pSizerA->AddSpacer(8);
    pSizerA->Add(m_cboFind, 0, wxALL, 0);
    pSizerA->AddSpacer(8);

    pSizerAx->AddSpacer(8);
    pSizerAx->Add(m_optRegExp, 0, wxALL, 0);
    pSizerAx->AddSpacer(8);

    if (m_bReplace) {
        pSizerAR->AddSpacer(8);
        pSizerAR->Add(m_cboReplace, 0, wxALL, 0);
        pSizerAR->AddSpacer(8);
        pSizerTAR->AddSpacer(8);
        pSizerTAR->Add(new wxStaticText(this, wxID_ANY, uT("Replace with:"), wxDefaultPosition, sizeText, 0), 0, wxALL, 0);
        pSizerTAR->AddSpacer(8);
    }

    pSizerAM->AddSpacer(8);
    pSizerAM->Add(m_optWhere, 0, wxALL, 0);
    pSizerAM->AddSpacer(8);

    pSizerB->AddSpacer(8);
    pSizerB->Add(m_optCase, 0, wxALL, 0);
    pSizerB->AddSpacer(4);
    pSizerB->Add(m_optCyclic, 0, wxALL, 0);
    pSizerB->AddSpacer(8);

    pSizerBB->AddSpacer(8);
    pSizerBB->Add(m_optWord, 0, wxALL, 0);
    pSizerBB->AddSpacer(4);
    pSizerBB->Add(m_optMarker, 0, wxALL, 0);
    pSizerBB->AddSpacer(8);

    pSizerD->AddSpacer(8);
    if (m_bReplace) {
        pSizerD->Add(m_btnFindNext, 0, wxCENTER, 0);
        pSizerD->AddSpacer(4);
        pSizerD->Add(m_btnFindPR, 0, wxCENTER, 0);
    }
    else {
        pSizerD->Add(m_btnFindPR, 0, wxCENTER, 0);
        pSizerD->AddSpacer(4);
        pSizerD->Add(m_btnFindNext, 0, wxCENTER, 0);
    }
    pSizerDD->AddSpacer(8);
    pSizerDD->Add(m_btnFindAll, 0, wxCENTER, 0);
    pSizerDD->AddSpacer(4);
    pSizerDD->Add(m_btnClose, 0, wxCENTER, 0);
    pSizerDD->AddSpacer(8);

    pSizer->AddSpacer(8);
    pSizer->Add(pSizerTA, 0, wxALL, 0);
    pSizer->AddSpacer(4);
    pSizer->Add(pSizerA, 0, wxALL, 0);
    pSizer->AddSpacer(4);
    pSizer->Add(pSizerAx, 0, wxALL, 0);
    if (m_bReplace) {
        pSizer->AddSpacer(8);
        pSizer->Add(pSizerTAR, 0, wxALL, 0);
        pSizer->AddSpacer(4);
        pSizer->Add(pSizerAR, 0, wxALL, 0);
        pSizer->AddSpacer(8);
    }

    pSizerTS->AddSpacer(8);
    pSizerTS->Add(m_txtStatus, 0, wxALL, 0);
    pSizerTS->AddSpacer(8);

    pSizer->AddSpacer(8);
    pSizer->Add(pSizerAM, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerB, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerBB, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerTS, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerD, 0, wxALL, 0);
    pSizer->AddSpacer(4);
    pSizer->Add(pSizerDD, 0, wxALL, 0);
    pSizer->AddSpacer(8);

    pSizer->Fit(this);

    Move(m_ptInit);

    updateItems(m_strFind, wxEmptyString);
}

void FindFileDlg::OnFindPrev(wxCommandEvent &WXUNUSED(tEvent))
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    const bool bFindInOutput = (false == m_bReplace) && (m_optWhere->GetSelection() == 3);

    CodeEdit *pSTC = (false == bFindInOutput) ? static_cast<CodeEdit *>(pFrame->getActiveEditor()) : static_cast<CodeEdit *>(pFrame->getOutput());
    if (pSTC == NULL) {
        return;
    }

    OutputEdit *pOutput = NULL;
    ScriptEdit *pEdit = NULL;
    if (bFindInOutput) {
        pOutput = static_cast<OutputEdit *>(pSTC);
    }
    else {
        pEdit = static_cast<ScriptEdit *>(pSTC);
    }

    updateLabel(uT(""), uT(""));

    int iFound = 0;

    const bool bMulti = (m_optWhere->GetSelection() == 1);

    const int iSelStartx = pSTC->GetSelectionStart();
    const int iSelEndx = pSTC->GetSelectionEnd();
    const int iFirstLinex = pSTC->GetFirstVisibleLine();

    if (bMulti) {
        pSTC->DoFindReset();
    }

    const bool bSel = (m_optWhere->GetSelection() == 2);
    if (bSel == false) {
        m_iFindPos = -1;
    }

    wxString strFind = m_cboFind->GetValue();
    int iStyle = 0;
    if (m_optCase->IsChecked()) {
        iStyle |= wxSTC_FIND_MATCHCASE;
    }
    if (m_optWord->IsChecked()) {
        iStyle |= wxSTC_FIND_WHOLEWORD;
    }
    if (m_optRegExp->IsChecked()) {
        iStyle |= wxSTC_FIND_REGEXP;
    }

    if (bSel) {
        if (m_iFindPos < 0) {
            m_iFindPos = pSTC->GetSelectionEnd();
        }
    }

    const int iFindPosX = pSTC->GetSelectionEnd();

    const bool bCyclic = m_optCyclic->GetValue();

    const int iCount = pFrame->getEditorCount();

    // find in selection or in one file
    if (bSel || (bMulti == false)) {

        int iRet = FIND_NOTFOUND;

        if (bFindInOutput) {
            iRet = pOutput->BasicFind(strFind, iStyle, &m_iFindPos, true);
        }
        else {
            iRet = pEdit->DoFindPrev(strFind, false, iStyle, bSel, &m_iFindPos, m_optMarker->IsChecked());
        }

        if (((iRet == FIND_LIMITREACHED) || (iRet == FIND_NOTFOUND)) && bCyclic) {
            int iEnd = pSTC->GetLineEndPosition(pSTC->GetLineCount() - 1);
            //pSTC->SetSelection(iEnd, iEnd);
            m_iFindPos = iEnd;
            if (bFindInOutput) {
                iRet = pOutput->BasicFind(strFind, iStyle, &m_iFindPos, true);
            }
            else {
                iRet = pEdit->DoFindPrev(strFind, false, iStyle, bSel, &m_iFindPos, m_optMarker->IsChecked());
            }

            if (iRet < FIND_ITEMFOUND) {
                // restore selected text
                pSTC->SetSelection(iSelStartx, iSelEndx);
                const int iFirstLinexNew = pSTC->GetFirstVisibleLine();
                if (iFirstLinex != iFirstLinexNew) {
                    pSTC->LineScroll(0, iFirstLinex - iFirstLinexNew);
                }
            }
        }

        if (iRet == FIND_LIMITREACHED) {
            m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
            wxString strT = bSel ? uT("Find reached the start of selection.") : uT("Find reached the start of document.");
            updateLabel(strT, strT);
            m_iFindPos = -1;
        }
        else if (iRet == FIND_NOTFOUND) {
            wxString strX = pSTC->GetSelectedText();
            wxString strT;
            strT = uT("\'");
            strT += strFind;
            strT += uT("\'");
            if (strX.IsSameAs(strFind)) {
                m_txtStatus->SetForegroundColour(STATUSCOLOR_BLUE);
                strT += uT(": no more occurrence.");
            }
            else {
                m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
                strT += uT(" not found.");
            }
            updateLabel(strT, strT);
            m_iFindPos = -1;
        }
        else if (iRet >= FIND_ITEMFOUND) {
            wxString strT = uT("\'");
            strT += strFind;
            strT += uT("\'");
            wxString strTT = strT;
            wxString strX = pSTC->GetSelectedText();
            if (strX.IsSameAs(strFind) && (pSTC->GetSelectionEnd() == iFindPosX)) {
                strT += uT(": no more occurrence.");
                strTT = strT;
            }
            else {
                int iLine = bSel ? pSTC->LineFromPosition(m_iFindPos) : pSTC->GetCurrentLine();
                strT += uT(" found.");
                if (iLine >= 0) {
                    int iColumn = pSTC->GetColumn(pSTC->GetSelectionStart());
                    if (iColumn < 0) {
                        iColumn = 0;
                    }
                    strTT += wxString::Format(uT(" found in line %d (column %d)."), iLine + 1, iColumn + 1);
                    pFrame->updateStatusbarPos(iLine, iColumn);
                    pSTC->ScrollToColumn(iColumn);
                }
            }
            m_txtStatus->SetForegroundColour(STATUSCOLOR_BLUE);
            updateLabel(strT, strTT);
            updateFindList(strFind);
        }
    }

    // find in all open files
    else {
        int iSel = pFrame->getActiveEditorIndex();
        if (iSel < 0) {
            return;
        }

        int iRet = 0,
            iSelF = pFrame->getActiveEditorIndex();

        ScriptEdit *pEditF = pFrame->getActiveEditor();
        if (pEditF == NULL) {
            return;
        }
        int startPos = 0, endPos = 0;
        pEditF->GetSelection(&startPos, &endPos);

        bool bFindSelected = false;
        if (iStyle & wxSTC_FIND_REGEXP) {
            bFindSelected = (pEditF->GetSelectedText().Length() > 0);
        }
        else {
            if (iStyle & wxSTC_FIND_MATCHCASE) {
                bFindSelected = (pEditF->GetSelectedText().Cmp(strFind) == 0);
            }
            else {
                bFindSelected = (pEditF->GetSelectedText().CmpNoCase(strFind) == 0);
            }
        }

        int ii = iSel, iEnd;

        int jj = iCount;

        pEdit = pFrame->getEditor(ii);
        if (pEdit == NULL) {
            return;
        }

        while (jj >= 0) {
            iRet = pEdit->DoFindPrev(strFind, false, iStyle, false, &m_iFindPos, m_optMarker->IsChecked());
            if ((iRet == FIND_LIMITREACHED) || (iRet == FIND_NOTFOUND)) {
                m_iFindPos = -1;
                // change file
                --jj;
                if (jj < 0) {
                    wxString strT;
                    if (bFindSelected) {
                        strT = uT("Find reached the start of document.");
                    }
                    else {
                        strT = uT("\'");
                        strT += strFind;
                        strT += uT("\' not found.");
                    }
                    m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
                    updateLabel(strT, strT);
                    break;
                }
                --ii;
                if (ii < 0) {
                    if (bCyclic == false) {
                        wxString strT;
                        if (bFindSelected) {
                            strT = uT("Find reached the start of document.");
                        }
                        else {
                            strT = uT("\'");
                            strT += strFind;
                            strT += uT("\' not found.");
                        }
                        m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
                        updateLabel(strT, strT);
                        break;
                    }
                    ii = iCount - 1;
                }
                pEdit = pFrame->getEditor(ii);
                if (pEdit == NULL) {
                    break;
                }
                // go to the end of the next file
                iEnd = pEdit->GetLineEndPosition(pEdit->GetLineCount() - 1);
                pEdit->SetSelection(iEnd, iEnd);
                //
            }
            else if (iRet >= FIND_ITEMFOUND) {
                updateFindList(strFind);
                iSelF = ii;
                iFound += 1;
                break;
            }
        }

        if (iFound > 0) {
            int iLine = pEdit->GetCurrentLine();
            wxString strT = uT("\'");
            strT += strFind;
            strT += uT("\'");
            wxString strTT = strT;
            strT += uT(" found.");
            if (iLine >= 0) {
                int iColumn = pEdit->GetColumn(pEdit->GetSelectionStart());
                if (iColumn < 0) {
                    iColumn = 0;
                }
                strTT += uT(" found");
                if (pEdit->GetFilename().IsEmpty() == false) {
                    strTT += uT(" in \'");
                    strTT += CometFrame::getLabelFromPath(LM_CSTR(pEdit->GetFilename()));
                    strTT += uT("\' ");
                }
                strTT += wxString::Format(uT(" in line %d (column %d)."), iLine + 1, iColumn + 1);
                pFrame->updateStatusbarPos(iLine, iColumn);
                pEdit->ScrollToColumn(iColumn);
            }
            m_txtStatus->SetForegroundColour(STATUSCOLOR_BLUE);
            updateLabel(strT, strTT);

            if (iSelF != pFrame->getActiveEditorIndex()) {
                pFrame->setActiveEditor(iSelF, false);
            }
        }

        int startPosT = 0, endPosT = 0;
        pEditF->GetSelection(&startPosT, &endPosT);
        // restore selection only if no new selection done by the search function
        if (startPosT == endPosT) {
            pEditF->SetSelection(startPos, endPos);
        }
    }
}

void FindFileDlg::DoFindReplace(bool bReplace /* = false*/, bool bAll /* = false*/)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    const bool bFindInOutput = (false == m_bReplace) && (m_optWhere->GetSelection() == 3);
    if (bFindInOutput && bReplace) {
        // should never happen
        return;
    }
    if (bFindInOutput) {
        bAll = false;
    }

    updateLabel(uT(""), uT(""));

    bool bMulti = (m_optWhere->GetSelection() == 1);
    if (bFindInOutput) {
        bMulti = false;
    }

    bool bSel = (m_optWhere->GetSelection() == 2);
    if (bSel == false) {
        m_iFindPos = -1;
    }

    CodeEdit *pSTC = (false == bFindInOutput) ? static_cast<CodeEdit *>(pFrame->getActiveEditor()) : static_cast<CodeEdit *>(pFrame->getOutput());
    if (pSTC == NULL) {
        return;
    }

    OutputEdit *pOutput = NULL;
    ScriptEdit *pEdit = NULL;
    if (bFindInOutput) {
        pOutput = static_cast<OutputEdit *>(pSTC);
    }
    else {
        pEdit = static_cast<ScriptEdit *>(pSTC);
    }

    const int indexT = pFrame->getActiveEditorIndex();

    const int iSelStartx = pSTC->GetSelectionStart();
    const int iSelEndx = pSTC->GetSelectionEnd();
    const int iFirstLinex = pSTC->GetFirstVisibleLine();

    if (bMulti || bAll) {
        pSTC->DoFindReset();
    }

    wxString strFind = m_cboFind->GetValue();
    wxString strReplace = wxEmptyString;

    if (bReplace) {
        strReplace = m_cboReplace->GetValue();
        if (strReplace.IsSameAs(strFind)) {
            m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
            wxString strT = uT("Find and replace items are the same.");
            updateLabel(strT, strT);
            m_iFindPos = -1;
            return;
        }
    }

    int iStyle = 0;
    if (m_optCase->IsChecked()) {
        iStyle |= wxSTC_FIND_MATCHCASE;
    }
    if (m_optWord->IsChecked()) {
        iStyle |= wxSTC_FIND_WHOLEWORD;
    }
    if (m_optRegExp->IsChecked()) {
        iStyle |= wxSTC_FIND_REGEXP;
    }

    int iFound = 0;

    const int iCount = pFrame->getEditorCount();

    const bool bCyclic = m_optCyclic->GetValue();

    int iFindAll = 0;

    if ((m_iFindPos < 0) || bAll) {
        m_iFindPos = pSTC->GetSelectionStart();
    }
    const int iFindPosX = m_iFindPos;

    // find in selection or in one file
    if (bSel || (bMulti == false)) {

        int iFound = FIND_NOTFOUND;
        if (bFindInOutput) {
            iFound = pOutput->BasicFind(strFind, iStyle, &m_iFindPos, false);
        }
        else {
            iFound = pEdit->DoFindReplace(strFind, false, iStyle, strReplace, bReplace, bAll, bAll ? &iFindAll : NULL, bSel, &m_iFindPos, bAll || m_optMarker->IsChecked());
        }

        if (((iFound == FIND_LIMITREACHED) || (iFound == FIND_NOTFOUND)) && (bAll == false) && bCyclic) {
            //pSTC->SetSelection(0, 0);
            m_iFindPos = 0;
            if (bFindInOutput) {
                iFound = pOutput->BasicFind(strFind, iStyle, &m_iFindPos, false);
            }
            else {
                iFound = pEdit->DoFindReplace(strFind, false, iStyle, strReplace, bReplace, bAll, bAll ? &iFindAll : NULL, bSel, &m_iFindPos, bAll || m_optMarker->IsChecked());
            }

            if (iFound < FIND_ITEMFOUND) {
                // restore selected text
                pSTC->SetSelection(iSelStartx, iSelEndx);
                const int iFirstLinexNew = pSTC->GetFirstVisibleLine();
                if (iFirstLinex != iFirstLinexNew) {
                    pSTC->LineScroll(0, iFirstLinex - iFirstLinexNew);
                }
            }
        }

        if (iFound == FIND_LIMITREACHED) {
            m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
            wxString strT = bSel ? uT("Find reached the end of selection.") : uT("Find reached the end of document.");
            updateLabel(strT, strT);
            m_iFindPos = -1;
        }

        else if (iFound == FIND_NOTFOUND) {
            wxString strX = pSTC->GetSelectedText();
            wxString strT = uT("\'");
            strT += strFind;
            strT += uT("\'");
            if (strX.IsSameAs(strFind)) {
                strT += uT(": no more occurrence.");
                m_txtStatus->SetForegroundColour(STATUSCOLOR_BLUE);
            }
            else {
                strT += (iCount == 1) ? uT(" not found.") : (bSel ? uT(" not found in the selection.") : uT(" not found in this document."));
                m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
            }
            updateLabel(strT, strT);
            m_iFindPos = -1;
        }

        else if (iFound >= FIND_ITEMFOUND) {
            wxString strT = uT("\'");
            strT += strFind;
            strT += uT("\'");
            wxString strTT = strT;

            wxString strX = pSTC->GetSelectedText();
            if (strX.IsSameAs(strFind) && (pSTC->GetSelectionStart() == iFindPosX)) {
                strT += uT(": no more occurrence.");
                strTT = strT;
            }
            else {
                int iLine = bSel ? pSTC->LineFromPosition(m_iFindPos - 1) : pSTC->GetCurrentLine();
                strT += uT(" found.");
                if (iLine >= 0) {
                    if (bAll && (iFindAll > 0)) {
                        if (bReplace) {
                            strTT += wxString::Format((iFindAll > 1) ? uT(" replaced %d times.") : uT(" replaced %d time."), iFindAll);
                        }
                        else {
                            strTT += wxString::Format((iFindAll > 1) ? uT(" found %d times.") : uT(" found %d time."), iFindAll);
                        }
                    }
                    else {
                        int iColumn = 0;
                        if (bSel == false) {
                            iColumn = pSTC->GetColumn(pSTC->GetSelectionStart());
                            if (iColumn < 0) {
                                iColumn = 0;
                            }
                        }
                        if (bReplace) {
                            if (bSel == false) {
                                strTT += wxString::Format(uT(" replaced in line %d (column %d)."), iLine + 1, iColumn + 1);
                            }
                            else {
                                strTT += wxString::Format(uT(" replaced in line %d."), iLine + 1);
                            }
                        }
                        else {
                            if (bSel == false) {
                                strTT += wxString::Format(uT(" found in line %d (column %d)."), iLine + 1, iColumn + 1);
                            }
                            else {
                                strTT += wxString::Format(uT(" found in line %d."), iLine + 1);
                            }
                        }
                        pFrame->updateStatusbarPos(iLine, iColumn);
                        pSTC->ScrollToColumn(iColumn);
                    }
                }
            }

            m_txtStatus->SetForegroundColour(STATUSCOLOR_BLUE);
            updateLabel(strT, strTT);

            updateFindList(strFind);

            if (bReplace) {
                if (m_cboReplace->FindString(strReplace, true) == wxNOT_FOUND) {
                    updateReplaceList(strReplace);
                }
            }
        }
    }

    // find in all open files
    else {
        int iF = 0;
        if (bAll == false) {
            if (indexT < 0) {
                return;
            }
        }

        ScriptEdit *pEditF = pFrame->getActiveEditor();
        if (pEditF == NULL) {
            return;
        }
        int startPos = 0, endPos = 0;
        pEditF->GetSelection(&startPos, &endPos);

        bool bFindSelected = false;
        if (iStyle & wxSTC_FIND_REGEXP) {
            bFindSelected = (pEditF->GetSelectedText().Length() > 0);
        }
        else {
            if (iStyle & wxSTC_FIND_MATCHCASE) {
                bFindSelected = (pEditF->GetSelectedText().Cmp(strFind) == 0);
            }
            else {
                bFindSelected = (pEditF->GetSelectedText().CmpNoCase(strFind) == 0);
            }
        }

        int iSelF = indexT;

        int ii = bAll ? 0 : indexT;
        int iFile = 0;

        pEdit = pFrame->getEditor(ii);
        if (pEdit == NULL) {
            return;
        }
        if (bAll) {
            // for the current selected file, search from the beginning
            pEdit->SetSelection(0, 0);
        }

        while (iFile <= iCount) {
            iF = pEdit->DoFindReplace(strFind, false, iStyle, strReplace, bReplace, bAll, NULL, false, NULL, bAll || m_optMarker->IsChecked());
            if ((iF == FIND_LIMITREACHED) || (iF == FIND_NOTFOUND)) {
                // change file
                ++iFile;
                if (iFile > iCount) {
                    wxString strT;
                    if (bFindSelected) {
                        strT = uT("Find reached the end of document.");
                    }
                    else if (iFound < 1) {
                        strT = uT("\'");
                        strT += strFind;
                        strT += uT("\' not found.");
                    }
                    m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
                    updateLabel(strT, strT);
                    break;
                }
                ++ii;
                if ((bAll == false) && (ii >= iCount)) {
                    if (bCyclic == false) {
                        wxString strT;
                        if (bFindSelected) {
                            strT = uT("Find reached the end of document.");
                        }
                        else if (iFound < 1) {
                            strT = uT("\'");
                            strT += strFind;
                            strT += uT("\' not found.");
                        }
                        m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
                        updateLabel(strT, strT);
                        break;
                    }
                    ii = 0;
                }
                // go to the beginning of the next file
                pEdit = pFrame->getEditor(ii);
                if (pEdit == NULL) {
                    break;
                }
                pEdit->SetSelection(0, 0);
                //
            }
            else {
                iFound += iF;
                iSelF = ii;
                if (bAll == false) {
                    break;
                }
                if (bAll && bMulti) {
                    // change file
                    ++iFile;
                    ++ii;
                    if ((iFile >= iCount) || (ii >= iCount)) {
                        break;
                    }
                    // go to the beginning of the next file
                    pEdit = pFrame->getEditor(ii);
                    if (pEdit == NULL) {
                        break;
                    }
                    pEdit->SetSelection(0, 0);
                    //
                }
            }
        }

        if (iFound > 0) {
            wxString strT = uT("\'");
            strT += strFind;
            strT += (bReplace ? uT("\' replaced") : uT("\' found"));
            wxString strTT = strT;
            if ((bMulti == false) || bAll) {
                strT += wxString::Format((iFound > 1) ? uT(" %d times.") : uT(" %d time."), iFound);
                strTT = strT;
            }
            else {
                strT += uT(".");
                pEdit = pFrame->getEditor(iSelF);
                if (pEdit != NULL) {
                    int iLine = pEdit->GetCurrentLine();
                    if (iLine >= 0) {
                        int iColumn = pEdit->GetColumn(pEdit->GetSelectionStart());
                        if (iColumn < 0) {
                            iColumn = 0;
                        }
                        if (pEdit->GetFilename().IsEmpty() == false) {
                            strTT += uT(" in \'");
                            strTT += CometFrame::getLabelFromPath(LM_CSTR(pEdit->GetFilename()));
                            strTT += uT("\'");
                        }
                        strTT += wxString::Format(uT(" in line %d (column %d)."), iLine + 1, iColumn + 1);
                    }
                }
            }
            m_txtStatus->SetForegroundColour(STATUSCOLOR_BLUE);
            updateLabel(strT, strTT);
            if (iSelF != pFrame->getActiveEditorIndex()) {
                pFrame->setActiveEditor(iSelF, true);
            }
            updateFindList(strFind);
        }
        else {
            m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
            wxString strX = pSTC->GetSelectedText();
            wxString strT = uT("\'");
            strT += strFind;
            strT += uT("\'");
            if (strX.IsSameAs(strFind)) {
                strT += uT(": no more occurrence.");
            }
            else {
                strT += uT(" not found.");
            }
            updateLabel(strT, strT);
            m_iFindPos = -1;
        }

        int startPosT = 0, endPosT = 0;
        pEditF->GetSelection(&startPosT, &endPosT);
        // restore selection only if now new selection done by the search function
        if (startPosT == endPosT) {
            pEditF->SetSelection(startPos, endPos);
        }
    }
}

void FindFileDlg::OnFindNext(wxCommandEvent &WXUNUSED(tEvent))
{
    DoFindReplace(false);
}

void FindFileDlg::OnFindFindAll(wxCommandEvent &WXUNUSED(tEvent))
{
    DoFindReplace(false, true);
}

void FindFileDlg::OnFindReplace(wxCommandEvent &WXUNUSED(tEvent))
{
    DoFindReplace(true);
}

void FindFileDlg::OnFindReplaceAll(wxCommandEvent &WXUNUSED(tEvent))
{
    DoFindReplace(true, true);
}

void FindFileDlg::OnMainAction(wxCommandEvent &tEvent)
{
    if (((tEvent.GetId() == ID_SEARCH_FIND) && findOnly()) || ((tEvent.GetId() == ID_SEARCH_REPLACE) && (findOnly() == false))) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }
    pFrame->DoMainAction(tEvent.GetId());
}

void FindFileDlg::OnWhatChange(wxCommandEvent &WXUNUSED(tEvent))
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    ScriptEdit *pEdit = static_cast<ScriptEdit *>(pFrame->getActiveEditor());
    if (pEdit == NULL) {
        return;
    }

    wxString strFind = m_cboFind->GetValue();
    bool bEnable = (strFind.Length() < pEdit->GetSelectedText().Length());
    if ((m_optWhere->GetSelection() == 2) && (bEnable == false)) {
        // Unselect before disabling
        m_optWhere->SetSelection(0);
    }
    m_optWhere->Enable(2, bEnable);
}

void FindFileDlg::OnWhereChange(wxCommandEvent &WXUNUSED(tEvent))
{
    if (m_optWhere->GetSelection() != 2) {
        m_iFindPos = -1;
    }
}

void FindFileDlg::OnActivate(wxActivateEvent &tEvent)
{
    wxDialog::OnActivate(tEvent);

    updateSel();
}

void FindFileDlg::OnMarker(wxCommandEvent &WXUNUSED(tEvent))
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame != NULL) {
        pFrame->enableFindMarker(m_optMarker->IsChecked());
    }
}

void FindFileDlg::OnCyclic(wxCommandEvent &WXUNUSED(tEvent))
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame != NULL) {
        pFrame->enableFindCyclic(m_optCyclic->IsChecked());
    }
}

void FindFileDlg::OnFindClose(wxCommandEvent &WXUNUSED(tEvent))
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame != NULL) {
        pFrame->updateFindFilePos();
    }
    this->Hide();
}

void FindFileDlg::OnCloseWindow(wxCloseEvent &WXUNUSED(tEvent))
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame != NULL) {
        pFrame->updateFindFilePos();
    }
    this->Hide();
}

bool FindFileDlg::ShowToolTips()
{
    return false;
}
