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
#include "ToolsDlg.h"

BEGIN_EVENT_TABLE(ToolsDlg, wxDialog)
    EVT_COMBOBOX(IDE_TOOLS_TYPE, ToolsDlg::OnTypeChange)
    EVT_BUTTON(ID_OK, ToolsDlg::OnOK)
    EVT_BUTTON(ID_CANCEL, ToolsDlg::OnCancel)
    EVT_CLOSE(ToolsDlg::OnCloseWindow)
END_EVENT_TABLE()

ToolsDlg::ToolsDlg(wxWindow *pParent, int iLexer /* = 0*/,
                   const wxPoint &ptInit /* = ToolsDlg_POSITION*/, wxWindowID idT /* = IDD_TOOLS*/,
                   const wxString &strTitle /* = ToolsDlg_TITLE*/, const wxSize &sizeT /* = ToolsDlg_SIZE*/,
                   long styleT /* = ToolsDlg_STYLE*/)
    : wxDialog(pParent, idT, strTitle, ptInit, sizeT, styleT)
{
    (static_cast<CometFrame *>(GetParent()))->setToolsDlg(this);

    m_iSel = 0;
    for (int ii = 0;; ii++) {
        if (-1 == CometFrame::TOOL_LEX[ii]) {
            break;
        }
        if (iLexer == CometFrame::TOOL_LEX[ii]) {
            m_iSel = ii;
            break;
        }
    }

    m_cboType = NULL;
    m_edtCmd = NULL;

    m_ptInit = ptInit;

    memset(m_szCmdBak, 0, TOOLS_MAXCOUNT * LM_STRSIZE * sizeof(char_t));
    for (int ii = 0; ii < TOOLS_MAXCOUNT; ii++) {
        Tstrcpy(m_szCmdBak[ii], (const char_t *)(CometFrame::TOOL_CONF.szCmd[ii]));
        m_bOutBak[ii] = CometFrame::TOOL_CONF.bOut[ii];
        m_bClsBak[ii] = CometFrame::TOOL_CONF.bCls[ii];
    }
    m_bLogBak = CometFrame::TOOL_CONF.bLog;
    m_bSilentBak = CometFrame::TOOL_CONF.bSilent;

    initInterface();

    wxSize sizeM = GetSize();
    SetMinSize(sizeM);
}

ToolsDlg::~ToolsDlg()
{
}

void ToolsDlg::initInterface()
{
    SetHelpText(ToolsDlg_TITLE);
    if (ToolsDlg::ShowToolTips()) {
        SetToolTip(ToolsDlg_TITLE);
    }
    SetTitle(ToolsDlg_TITLE);

    SetIcon((char **)app_small_xpm);

    wxSize sizeBtn = CometApp::getTextExtent(this, uT(">>Wun Tooly<<"), true);
    int iH = sizeBtn.GetHeight();
    sizeBtn.SetHeight(sizeBtn.GetHeight() + 12);
    wxSize sizeText = CometApp::getTextExtent(this, uT(">>WTerminal Options (title;directory;exec;command):y<<"), true);
    sizeText.SetWidth(sizeText.GetWidth() + 12);
    sizeText.SetHeight(iH);
    wxSize sizeEdit = sizeText;
    sizeEdit.SetHeight(iH + 8);

    m_cboType = new wxComboBox(this, IDE_TOOLS_TYPE, uT(""), wxDefaultPosition, sizeEdit, 0, wxCB_READONLY);
    m_cboType->Clear();
    for (int ii = 0;; ii++) {
        if (-1 == CometFrame::TOOL_LEX[ii]) {
            break;
        }
        m_cboType->AppendString(CometFrame::TOOL_EXT[ii]);
    }
    m_cboType->SetSelection(m_iSel);

    m_edtCmd = new wxTextCtrl(this, IDE_TOOLS_CMD, CometFrame::TOOL_CONF.szCmd[m_iSel], wxDefaultPosition, sizeEdit);
    m_optOut = new wxCheckBox(this, IDE_TOOLS_OUT, uT("Redirect output (stdout) to Comet"), wxDefaultPosition, sizeText);
    m_optOut->SetValue(CometFrame::TOOL_CONF.bOut[m_iSel]);
    m_optSilent = new wxCheckBox(this, IDE_TOOLS_OUT, uT("Silent mode (only if Redirect)"), wxDefaultPosition, sizeText);
    m_optSilent->SetValue(CometFrame::TOOL_CONF.bSilent);
    m_optLog = new wxCheckBox(this, IDE_TOOLS_LOG, uT("Print out the command and termination code"), wxDefaultPosition, sizeText);
    m_optLog->SetValue(CometFrame::TOOL_CONF.bLog);
    m_optCls = new wxCheckBox(this, IDE_TOOLS_CLS, uT("Clear output before each run"), wxDefaultPosition, sizeText);
    m_optCls->SetValue(CometFrame::TOOL_CONF.bCls[m_iSel]);

#if defined(__WXGTK__)
    m_edtTerm = new wxTextCtrl(this, IDE_TOOLS_TERM, CometFrame::TOOL_CONF.szTerm, wxDefaultPosition, sizeEdit);
    m_edtOpt = new wxTextCtrl(this, IDE_TOOLS_OPT, CometFrame::TOOL_CONF.szOpt, wxDefaultPosition, sizeEdit);
    m_edtShell = new wxTextCtrl(this, IDE_TOOLS_SHELL, CometFrame::TOOL_CONF.szShell, wxDefaultPosition, sizeEdit);
#endif

    wxBoxSizer *pSizerMain = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *pSizerAt = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerA = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerBt = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerB = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerBo = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerBp = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerBl = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerBn = new wxBoxSizer(wxHORIZONTAL);

#if defined(__WXGTK__)
    wxBoxSizer *pSizerTt = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerT = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerOt = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerO = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerPt = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerP = new wxBoxSizer(wxHORIZONTAL);
#endif

    wxBoxSizer *pSizerC = new wxBoxSizer(wxHORIZONTAL);

    pSizerAt->AddSpacer(8);
    pSizerAt->Add(new wxStaticText(this, wxID_ANY, uT("File Type:"), wxDefaultPosition, sizeText, 0), 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerAt->AddSpacer(8);

    pSizerA->AddSpacer(8);
    pSizerA->Add(m_cboType, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerA->AddSpacer(8);

    pSizerBt->AddSpacer(8);
    pSizerBt->Add(new wxStaticText(this, wxID_ANY, uT("Command:"), wxDefaultPosition, sizeText, 0), 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerBt->AddSpacer(8);

    pSizerB->AddSpacer(8);
    pSizerB->Add(m_edtCmd, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerB->AddSpacer(8);

    pSizerBo->AddSpacer(8);
    pSizerBo->Add(m_optOut, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerBo->AddSpacer(8);

    pSizerBp->AddSpacer(8);
    pSizerBp->Add(m_optSilent, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerBp->AddSpacer(8);

    pSizerBl->AddSpacer(8);
    pSizerBl->Add(m_optLog, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerBl->AddSpacer(8);

    pSizerBn->AddSpacer(8);
    pSizerBn->Add(m_optCls, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerBn->AddSpacer(8);

#if defined(__WXGTK__)
    pSizerTt->AddSpacer(8);
    pSizerTt->Add(new wxStaticText(this, wxID_ANY, uT("Terminal:"), wxDefaultPosition, sizeText, 0), 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerTt->AddSpacer(8);

    pSizerT->AddSpacer(8);
    pSizerT->Add(m_edtTerm, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerT->AddSpacer(8);

    pSizerOt->AddSpacer(8);
    pSizerOt->Add(new wxStaticText(this, wxID_ANY, uT("Terminal Options (title;directory;exec;command):"), wxDefaultPosition, sizeText, 0), 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerOt->AddSpacer(8);

    pSizerO->AddSpacer(8);
    pSizerO->Add(m_edtOpt, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerO->AddSpacer(8);

    pSizerPt->AddSpacer(8);
    pSizerPt->Add(new wxStaticText(this, wxID_ANY, uT("Shell:"), wxDefaultPosition, sizeText, 0), 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerPt->AddSpacer(8);

    pSizerP->AddSpacer(8);
    pSizerP->Add(m_edtShell, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerP->AddSpacer(8);
#endif

    pSizerC->Add(new wxButton(this, ID_OK, uT("&OK"), wxDefaultPosition, sizeBtn, 0), 0, 0, 0);
    pSizerC->Add(new wxButton(this, ID_CANCEL, uT("&Cancel"), wxDefaultPosition, sizeBtn, 0), 0, 0, 0);

    pSizerMain->AddSpacer(16);
    pSizerMain->Add(pSizerAt, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
    pSizerMain->AddSpacer(4);
    pSizerMain->Add(pSizerA, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
    pSizerMain->AddSpacer(8);
    pSizerMain->Add(pSizerBt, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
    pSizerMain->AddSpacer(4);
    pSizerMain->Add(pSizerB, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
    pSizerMain->AddSpacer(4);
    pSizerMain->Add(pSizerBo, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
    pSizerMain->AddSpacer(4);
    pSizerMain->Add(pSizerBp, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
    pSizerMain->AddSpacer(4);
    pSizerMain->Add(pSizerBl, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
    pSizerMain->AddSpacer(4);
    pSizerMain->Add(pSizerBn, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
#if defined(__WXGTK__)
    pSizerMain->AddSpacer(8);
    pSizerMain->Add(pSizerTt, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
    pSizerMain->AddSpacer(4);
    pSizerMain->Add(pSizerT, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
    pSizerMain->AddSpacer(8);
    pSizerMain->Add(pSizerOt, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
    pSizerMain->AddSpacer(4);
    pSizerMain->Add(pSizerO, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
    pSizerMain->AddSpacer(8);
    pSizerMain->Add(pSizerPt, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
    pSizerMain->AddSpacer(4);
    pSizerMain->Add(pSizerP, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
#endif
    pSizerMain->AddSpacer(16);
    pSizerMain->Add(pSizerC, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 16);

    this->SetSizerAndFit(pSizerMain);

    Move(m_ptInit);
}

void ToolsDlg::OnTypeChange(wxCommandEvent &tEvent)
{
    int iSel = m_cboType->GetSelection();
    if ((iSel < 0) || (iSel >= TOOLS_MAXCOUNT)) {
        tEvent.Skip();
        return;
    }

    wxString strT = m_edtCmd->GetValue();
    Tstrcpy(CometFrame::TOOL_CONF.szCmd[m_iSel], LM_CSTR(strT));
    CometFrame::TOOL_CONF.bOut[m_iSel] = m_optOut->IsChecked();
    CometFrame::TOOL_CONF.bSilent = m_optSilent->IsChecked();
    CometFrame::TOOL_CONF.bCls[m_iSel] = m_optCls->IsChecked();

    m_iSel = iSel;
    m_edtCmd->SetValue(CometFrame::TOOL_CONF.szCmd[m_iSel]);
    m_optOut->SetValue(CometFrame::TOOL_CONF.bOut[m_iSel]);
    m_optSilent->SetValue(CometFrame::TOOL_CONF.bSilent);
    m_optCls->SetValue(CometFrame::TOOL_CONF.bCls[m_iSel]);
}

void ToolsDlg::OnOK(wxCommandEvent &WXUNUSED(tEvent))
{
    wxString strT;

#if defined(__WXGTK__)
    strT = m_edtTerm->GetValue();
    if (::wxFileExists(strT) == false) {
        SigmaMessageBox(uT("Terminal not found"), uT("Comet"), wxICON_WARNING | wxOK, this);
        return;
    }
    Tstrcpy(CometFrame::TOOL_CONF.szTerm, LM_CSTR(strT));
    strT = m_edtOpt->GetValue();
    if (strT.Contains(uT(";")) == false) {
        SigmaMessageBox(uT("Terminal options not valid: example: -T;--working-directory=;-x;-c"), uT("Comet"), wxICON_WARNING | wxOK, this);
        return;
    }
    Tstrcpy(CometFrame::TOOL_CONF.szOpt, LM_CSTR(strT));
    strT = m_edtShell->GetValue();
    if (strT.IsAscii() == false) {
        SigmaMessageBox(uT("Shell not valid: example: bash"), uT("Comet"), wxICON_WARNING | wxOK, this);
        return;
    }
    Tstrcpy(CometFrame::TOOL_CONF.szShell, LM_CSTR(strT));
#endif

    strT = m_edtCmd->GetValue();
    Tstrcpy(CometFrame::TOOL_CONF.szCmd[m_iSel], LM_CSTR(strT));
    CometFrame::TOOL_CONF.bOut[m_iSel] = m_optOut->IsChecked();
    CometFrame::TOOL_CONF.bSilent = m_optSilent->IsChecked();
    CometFrame::TOOL_CONF.bCls[m_iSel] = m_optCls->IsChecked();
    CometFrame::TOOL_CONF.bLog = m_optLog->IsChecked();

    (static_cast<CometFrame *>(GetParent()))->setToolsDlg(NULL);

    this->Destroy();
}

void ToolsDlg::OnCancel(wxCommandEvent &WXUNUSED(tEvent))
{
    // Restore initial values
    for (int ii = 0; ii < TOOLS_MAXCOUNT; ii++) {
        Tstrcpy(CometFrame::TOOL_CONF.szCmd[ii], (const char_t *)(m_szCmdBak[ii]));
        CometFrame::TOOL_CONF.bOut[ii] = m_bOutBak[ii];
        CometFrame::TOOL_CONF.bCls[ii] = m_bClsBak[ii];
    }
    CometFrame::TOOL_CONF.bLog = m_bLogBak;
    CometFrame::TOOL_CONF.bSilent = m_bSilentBak;

    (static_cast<CometFrame *>(GetParent()))->setToolsDlg(NULL);

    this->Destroy();
}

void ToolsDlg::OnCloseWindow(wxCloseEvent &WXUNUSED(tEvent))
{
    (static_cast<CometFrame *>(GetParent()))->setToolsDlg(NULL);

    this->Destroy();
}

bool ToolsDlg::ShowToolTips()
{
    return false;
}
