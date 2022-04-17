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
#include "ScriptEdit.h"
#include "CometFrame.h"
#include "TabDlg.h"

BEGIN_EVENT_TABLE(TabDlg, wxDialog)
    EVT_RADIOBOX(ID_SCRIPT_INDENTSIZE, TabDlg::OnTabSelected)
    EVT_RADIOBOX(ID_SCRIPT_INDENTOPT, TabDlg::OnOptSelected)
    EVT_CHECKBOX(ID_SCRIPT_INDENTGUIDE, TabDlg::OnIndentGuide)
    EVT_CHECKBOX(IDK_SCRIPT_LONGLINE, TabDlg::OnLongLine)
    EVT_BUTTON(ID_OK, TabDlg::OnOK)
    EVT_BUTTON(ID_CANCEL, TabDlg::OnCancel)
    EVT_CLOSE(TabDlg::OnCloseWindow)
END_EVENT_TABLE()

TabDlg::TabDlg(wxWindow *pParent, CommonInfo *pCommon, const wxPoint &ptInit, wxWindowID idT,
               const wxString &strTitle, const wxSize &sizeT, long styleT)
    : wxDialog(pParent, idT, strTitle, ptInit, sizeT, styleT & ~wxRESIZE_BORDER)
{
    (static_cast<CometFrame *>(GetParent()))->setTabDlg(this);

    m_pCommon = pCommon;

    m_CommonBak.tabSize = m_pCommon->tabSize;
    m_CommonBak.useTab = m_pCommon->useTab;
    m_CommonBak.indentGuideEnable = m_pCommon->indentGuideEnable;

    m_pIndentGuide = NULL;
    m_pTabOpt = NULL;
    m_pTabSize = NULL;
    m_iTabSize = 0;

    m_ptInit = ptInit;

    initInterface();

    wxSize sizeM = GetSize();
    SetMinSize(sizeM);
}

TabDlg::~TabDlg()
{
}

void TabDlg::initInterface()
{
    SetHelpText(TabDlg_TITLE);
    if (TabDlg::ShowToolTips()) {
        SetToolTip(TabDlg_TITLE);
    }

    SetIcon((char **)indentinc_small_xpm);

    wxBoxSizer *pSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pSizer);

    wxBoxSizer *pSizerA = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerB = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerC = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerD = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerE = new wxBoxSizer(wxHORIZONTAL);

    wxArrayString arrOpt;
    arrOpt.Add(uT("TAB"));
    arrOpt.Add(uT("SPACE"));

    wxSize sizeText = CometApp::getTextExtent(this, uT(">>Long lines indicator, at column:>1234<<<"));
    wxSize sizeBtn = sizeText;
    sizeBtn.SetHeight(sizeText.GetHeight() + 10);
    sizeText.SetHeight(((sizeText.GetHeight() + 8) * 2) + 8);

    wxSize sizeTextSm = CometApp::getTextExtent(this, uT(">1234<"));
    sizeTextSm.SetHeight(sizeTextSm.GetHeight() + 10);

    wxSize sizeCheck = sizeText;
    sizeCheck.SetHeight(sizeBtn.GetHeight());
    sizeCheck.SetWidth(sizeText.GetWidth() - sizeTextSm.GetWidth() - 8);

    m_pTabOpt = new wxRadioBox(this, ID_SCRIPT_INDENTOPT, uT("&Indentation"), wxDefaultPosition, sizeText, arrOpt, 1, wxRA_SPECIFY_ROWS, wxDefaultValidator, uT("Indentation"));
    m_pTabOpt->SetSelection(m_pCommon->useTab ? 0 : 1);

    pSizerA->AddSpacer(8);
    pSizerA->Add(m_pTabOpt, 0, wxCENTER, 0);
    pSizerA->AddSpacer(8);

    wxArrayString arrTab;
    arrTab.Add(uT("3"));
    arrTab.Add(uT("4"));
    arrTab.Add(uT("6"));
    arrTab.Add(uT("8"));

    int iSel = 0;
    if (m_pCommon->tabSize == 4) {
        iSel = 1;
    }
    else if (m_pCommon->tabSize == 6) {
        iSel = 2;
    }
    else if (m_pCommon->tabSize == 8) {
        iSel = 3;
    }
    m_pTabSize = new wxRadioBox(this, ID_SCRIPT_INDENTSIZE, uT("Tab &Size"), wxDefaultPosition, sizeText, arrTab, 1, wxRA_SPECIFY_ROWS, wxDefaultValidator, uT("Tab Size"));
    m_pTabSize->SetSelection(iSel);

    pSizerB->AddSpacer(8);
    pSizerB->Add(m_pTabSize, 0, wxCENTER, 0);
    pSizerB->AddSpacer(8);

    m_pIndentGuide = new wxCheckBox(this, ID_SCRIPT_INDENTGUIDE, uT("Indent Guide"), wxDefaultPosition, sizeBtn);
    m_pIndentGuide->SetValue(m_pCommon->indentGuideEnable);

    m_checkLongLine = new wxCheckBox(this, IDK_SCRIPT_LONGLINE, uT("Long lines indicator, at column:"), wxDefaultPosition, sizeCheck);
    m_checkLongLine->SetValue(m_pCommon->longLineOnEnable);

    wxString strL = wxString::Format(uT("%d"), m_pCommon->longLine);
    m_edtLongLine = new wxTextCtrl(this, IDE_SCRIPT_LONGLINE_COLUMN, LM_CSTR(strL), wxDefaultPosition, sizeTextSm, wxTE_CENTER);
    m_edtLongLine->SetMaxLength(4);

    pSizerC->AddSpacer(8);
    pSizerC->Add(m_pIndentGuide, 0, wxCENTER, 0);
    pSizerC->AddSpacer(8);

    pSizerD->AddSpacer(8);
    pSizerD->Add(m_checkLongLine, 0, wxCENTER, 0);
    pSizerD->AddSpacer(8);
    pSizerD->Add(m_edtLongLine, 0, wxCENTER, 0);
    pSizerD->AddSpacer(8);

    sizeBtn.SetWidth((sizeBtn.GetWidth() - 8) / 2);
    pSizerE->AddSpacer(8);
    pSizerE->Add(new wxButton(this, ID_OK, uT("&OK"), wxDefaultPosition, sizeBtn, 0), 0, wxCENTER, 0);
    pSizerE->AddSpacer(8);
    pSizerE->Add(new wxButton(this, ID_CANCEL, uT("&Cancel"), wxDefaultPosition, sizeBtn, 0), 0, wxCENTER, 0);
    pSizerE->AddSpacer(8);

    pSizer->AddSpacer(8);
    pSizer->Add(pSizerA, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerB, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerC, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerD, 0, wxALL, 0);
    pSizer->AddSpacer(16);
    pSizer->Add(pSizerE, 0, wxCENTER, 0);
    pSizer->AddSpacer(8);

    pSizer->Fit(this);

    Move(m_ptInit);
}

void TabDlg::OnOptSelected(wxCommandEvent &tEvent)
{
    int iOpt = m_pTabOpt->GetSelection();
    if (iOpt == 0) {
        m_pCommon->useTab = true;
    }
    else if (iOpt == 1) {
        m_pCommon->useTab = false;
    }

    ScriptEdit *pEditor = ((CometFrame *)(GetParent()))->getActiveEditor();
    (static_cast<ScriptEdit *>(pEditor))->DoUseTab(m_pCommon->useTab);
}

void TabDlg::OnIndentGuide(wxCommandEvent &tEvent)
{
    m_pCommon->indentGuideEnable = m_pIndentGuide->IsChecked();

    ScriptEdit *pEditor = ((CometFrame *)(GetParent()))->getActiveEditor();
    (static_cast<ScriptEdit *>(pEditor))->DoIndentGuide(m_pCommon->indentGuideEnable);
}

void TabDlg::OnLongLine(wxCommandEvent &tEvent)
{
    m_pCommon->longLineOnEnable = m_checkLongLine->IsChecked();

    long iLen = 0L;
    bool bRet = m_edtLongLine->GetValue().ToLong(&iLen, 10);
    if (bRet && (iLen >= 10L) && (iLen <= 1024L)) {
        m_pCommon->longLine = static_cast<int>(iLen);
    }

    ScriptEdit *pEditor = ((CometFrame *)(GetParent()))->getActiveEditor();
    (static_cast<ScriptEdit *>(pEditor))->DoSetLongLine(m_pCommon->longLine);
    (static_cast<ScriptEdit *>(pEditor))->DoEnableLongLine(m_pCommon->longLineOnEnable);
}

void TabDlg::OnTabSelected(wxCommandEvent &WXUNUSED(tEvent))
{
    int iTab = m_pTabSize->GetSelection();
    if (iTab == 0) {
        m_pCommon->tabSize = 3;
    }
    else if (iTab == 1) {
        m_pCommon->tabSize = 4;
    }
    else if (iTab == 2) {
        m_pCommon->tabSize = 6;
    }
    else if (iTab == 3) {
        m_pCommon->tabSize = 8;
    }

    ScriptEdit *pEditor = ((CometFrame *)(GetParent()))->getActiveEditor();
    (static_cast<ScriptEdit *>(pEditor))->DoSetTabSize(m_pCommon->tabSize);
}

void TabDlg::OnOK(wxCommandEvent &WXUNUSED(tEvent))
{
    long iLen = 0L;
    bool bRet = m_edtLongLine->GetValue().ToLong(&iLen, 10);
    if (bRet && (iLen >= 10L) && (iLen <= 1024L)) {
        m_pCommon->longLine = static_cast<int>(iLen);
    }

    (static_cast<CometFrame *>(GetParent()))->setTabDlg(NULL);
    (static_cast<CometFrame *>(GetParent()))->DoScriptTab();

    this->Destroy();
}

void TabDlg::OnCancel(wxCommandEvent &WXUNUSED(tEvent))
{
    m_pCommon->tabSize = m_CommonBak.tabSize;
    m_pCommon->useTab = m_CommonBak.useTab;
    m_pCommon->indentGuideEnable = m_CommonBak.indentGuideEnable;

    (static_cast<CometFrame *>(GetParent()))->setTabDlg(NULL);

    this->Destroy();
}

void TabDlg::OnCloseWindow(wxCloseEvent &WXUNUSED(tEvent))
{
    (static_cast<CometFrame *>(GetParent()))->setTabDlg(NULL);

    this->Destroy();
}

bool TabDlg::ShowToolTips()
{
    return false;
}
