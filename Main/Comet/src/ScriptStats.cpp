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

BEGIN_EVENT_TABLE(FileStatsDlg, wxDialog)
    EVT_BUTTON(ID_OK, FileStatsDlg::OnOK)
    EVT_CLOSE(FileStatsDlg::OnCloseWindow)
END_EVENT_TABLE()

FileStatsDlg::FileStatsDlg(wxWindow *pParent, const wxPoint &ptInit, wxWindowID idT,
                           const wxString &strTitle, const wxSize &sizeT, long styleT)
    : wxDialog(pParent, idT, strTitle, ptInit, sizeT, styleT & ~wxRESIZE_BORDER)
{
    m_txtAA = NULL; // File name
    m_txtAB = NULL;
    m_txtBA = NULL; // File type
    m_txtBB = NULL;
    m_txtCA = NULL; // Line count
    m_txtCB = NULL;
    m_txtDA = NULL; // Word count
    m_txtDB = NULL;
    m_txtEA = NULL; // Characters count
    m_txtEB = NULL;

    m_ptInit = ptInit;

    initInterface();

    wxSize sizeM = GetSize();
    SetMinSize(sizeM);
}

FileStatsDlg::~FileStatsDlg()
{
}

void FileStatsDlg::DoInitControls(void)
{
    ScriptEdit *pEdit = static_cast<ScriptEdit *>(GetParent());
    if (pEdit == NULL) {
        return;
    }

    int iLineCount = pEdit->GetLineCount();
    if (iLineCount < 1) {
        return;
    }

    wxFileName fname = pEdit->GetFilename();
    wxString strT = uT("'");
    strT += fname.GetFullName();
    strT += uT("' Statistics");
    SetTitle(strT);

    wxFile fileT(pEdit->GetFilename());
    if (fileT.IsOpened() == false) {
        return;
    }
    const wxFileOffset iFileSize = (const wxFileOffset)(fileT.Length());
    if (iFileSize < 0L) { // Should never happen
        fileT.Close();
        return;
    }
    if (iFileSize > LF_SCRIPT_MAXCHARS) {
        fileT.Close();
        return;
    }
    fileT.Close();

    wxBusyCursor waitC;

    int iStartPos = pEdit->GetSelectionStart();
    int iEndPos = pEdit->GetSelectionEnd();

    wxString strFileType = uT(" Text");
    switch (pEdit->GetLexer()) {
        case wxSTC_LEX_LUA:
            strFileType = uT(" Lua");
            break;
        case wxSTC_LEX_CPP:
            strFileType = uT(" C/C++");
            break;
        case wxSTC_LEX_PYTHON:
            strFileType = uT(" Python");
            break;
        case wxSTC_LEX_FORTRAN:
            strFileType = uT(" Fortran");
            break;
        case wxSTC_LEX_MAKEFILE:
            strFileType = uT(" Makefile");
            break;
        case wxSTC_LEX_LATEX:
            strFileType = uT(" LaTeX");
            break;
        case wxSTC_LEX_COMET:
            strFileType = uT(" Comet");
            break;
        default:
            break;
    }

    int iLineStart = 0, iLineEnd = iLineCount - 1;

    wxString strRange = uT(" Whole document");

    if (iStartPos < iEndPos) {
        iLineStart = pEdit->LineFromPosition(iStartPos);
        iLineEnd = pEdit->LineFromPosition(iEndPos);

        if (iLineStart > iLineEnd) {
            return;
        }

        int iStartPosX = pEdit->GetLineEndPosition(iLineStart) - pEdit->LineLength(iLineStart) + 1;
        if (iStartPos == pEdit->GetLineEndPosition(iLineStart)) {
            ++iLineStart;
            iStartPosX = pEdit->GetLineEndPosition(iLineStart) - pEdit->LineLength(iLineStart) + 1;
        }

        int iEndPosX = pEdit->GetLineEndPosition(iLineEnd);
        if (iEndPos == (pEdit->GetLineEndPosition(iLineEnd) - pEdit->LineLength(iLineEnd) + 1)) {
            --iLineEnd;
            iEndPosX = pEdit->GetLineEndPosition(iLineEnd);
        }

        if (iLineStart > iLineEnd) {
            return;
        }

        if (iLineStart == iLineEnd) {
            --iStartPosX;
        }

        pEdit->SetSelection(iStartPosX, iEndPosX);
        strRange = uT(" Selected text");
    }

    wxString strLine;

    wxString strWordCount = uT("");
    wxString strCharCount = uT("");
    wxString strLineCount = uT("");

    int iWordCount = 0, iCharCount = 0;
    iLineCount = 0;

    for (int ii = iLineStart; ii <= iLineEnd; ii++) {

        iLineCount += 1;

        strLine = pEdit->GetLine(ii);
        iCharCount += (int)(strLine.Length());

        strLine.Trim(true);
        strLine.Trim(false);
        strLine.Replace(uT("\t"), uT(" "), true);
        if (strLine.IsEmpty()) {
            continue;
        }

        wxStringTokenizer tokenizerT(strLine, uT(" "));
        if (tokenizerT.HasMoreTokens() == false) {
            continue;
        }
        while (tokenizerT.HasMoreTokens()) {
            strT = tokenizerT.GetNextToken();
            strT.Trim(true);
            strT.Trim(false);
            if (strT.IsEmpty()) {
                continue;
            }
            iWordCount += 1;
        }
    }

    strLineCount = wxString::Format(uT(" %d"), iLineCount);
    strCharCount = wxString::Format(uT(" %d"), iCharCount);
    strWordCount = wxString::Format(uT(" %d"), iWordCount);

    m_txtAB->SetLabel(strRange);
    m_txtBB->SetLabel(strFileType);
    m_txtCB->SetLabel(strLineCount);
    m_txtDB->SetLabel(strWordCount);
    m_txtEB->SetLabel(strCharCount);
}

void FileStatsDlg::initInterface()
{
    SetHelpText(FileStatsDlg_TITLE);
    if (FileStatsDlg::ShowToolTips()) {
        SetToolTip(FileStatsDlg_TITLE);
    }

    wxBoxSizer *pSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pSizer);

    wxBoxSizer *pSizerA = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerB = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerC = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerD = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerE = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerZ = new wxBoxSizer(wxHORIZONTAL);

    wxSize sizeT = CometApp::getTextExtent(this, uT(">Whole document<"));
    sizeT.SetHeight(sizeT.GetHeight() + 10);

    m_txtAA = new wxStaticText(this, wxID_ANY, uT("Range:   "), wxDefaultPosition, sizeT, wxALIGN_RIGHT);
    m_txtAB = new wxStaticText(this, wxID_ANY, uT("..."), wxDefaultPosition, sizeT, 0);

    m_txtBA = new wxStaticText(this, wxID_ANY, uT("File type:   "), wxDefaultPosition, sizeT, wxALIGN_RIGHT);
    m_txtBB = new wxStaticText(this, wxID_ANY, uT("..."), wxDefaultPosition, sizeT, 0);

    m_txtCA = new wxStaticText(this, wxID_ANY, uT("Lines:   "), wxDefaultPosition, sizeT, wxALIGN_RIGHT);
    m_txtCB = new wxStaticText(this, wxID_ANY, uT("..."), wxDefaultPosition, sizeT, 0);

    m_txtDA = new wxStaticText(this, wxID_ANY, uT("Words:   "), wxDefaultPosition, sizeT, wxALIGN_RIGHT);
    m_txtDB = new wxStaticText(this, wxID_ANY, uT("..."), wxDefaultPosition, sizeT, 0);

    m_txtEA = new wxStaticText(this, wxID_ANY, uT("Characters:   "), wxDefaultPosition, sizeT, wxALIGN_RIGHT);
    m_txtEB = new wxStaticText(this, wxID_ANY, uT("..."), wxDefaultPosition, sizeT, 0);

    pSizerA->AddSpacer(8);
    pSizerA->Add(m_txtAA, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerA->AddSpacer(8);
    pSizerA->Add(m_txtAB, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerA->AddSpacer(8);

    pSizerB->AddSpacer(8);
    pSizerB->Add(m_txtBA, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerB->AddSpacer(8);
    pSizerB->Add(m_txtBB, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerB->AddSpacer(8);

    pSizerC->AddSpacer(8);
    pSizerC->Add(m_txtCA, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerC->AddSpacer(8);
    pSizerC->Add(m_txtCB, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerC->AddSpacer(8);

    pSizerD->AddSpacer(8);
    pSizerD->Add(m_txtDA, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerD->AddSpacer(8);
    pSizerD->Add(m_txtDB, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerD->AddSpacer(8);

    pSizerE->AddSpacer(8);
    pSizerE->Add(m_txtEA, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerE->AddSpacer(8);
    pSizerE->Add(m_txtEB, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerE->AddSpacer(8);

    pSizerZ->AddSpacer(8);
    pSizerZ->Add(new wxButton(this, ID_OK, uT("&OK"), wxDefaultPosition, sizeT, 0), 0, wxCENTER, 0);
    pSizerZ->AddSpacer(8);

    pSizer->AddSpacer(8);
    pSizer->Add(pSizerA, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerB, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerC, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerD, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerE, 0, wxALL, 0);
    pSizer->AddSpacer(16);
    pSizer->Add(pSizerZ, 0, wxALIGN_CENTER | wxCENTER | wxBOTTOM, 0);
    pSizer->AddSpacer(8);

    pSizer->Fit(this);

    Move(m_ptInit);

    DoInitControls();
}

void FileStatsDlg::OnOK(wxCommandEvent &WXUNUSED(tEvent))
{
    EndModal(wxID_OK);
}

void FileStatsDlg::DoCancel(void)
{
}

void FileStatsDlg::OnCloseWindow(wxCloseEvent &WXUNUSED(tEvent))
{
    DoCancel();

    EndModal(wxID_CANCEL);
}

bool FileStatsDlg::ShowToolTips()
{
    return false;
}
