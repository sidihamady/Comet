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
#include "LexerDlg.h"

BEGIN_EVENT_TABLE(LexerDlg, wxDialog)
    EVT_BUTTON(ID_LEXER_LOAD, LexerDlg::OnLexerLoad)
    EVT_BUTTON(ID_LEXER_SAVE, LexerDlg::OnLexerSave)
    EVT_BUTTON(wxID_OK, LexerDlg::OnLexerOK)
    EVT_CLOSE(LexerDlg::OnCloseWindow)
END_EVENT_TABLE()

LexerDlg::LexerDlg(wxWindow *pParent, wxString strFind /* = wxEmptyString*/, bool bReplace /* = false*/,
                   const wxPoint &ptInit, wxWindowID idT, const wxString &strTitle, const wxSize &sizeT, long styleT)
    : wxDialog(pParent, idT, strTitle, ptInit, sizeT, styleT & ~wxRESIZE_BORDER)
{
    (static_cast<CometFrame *>(GetParent()))->setLexerDlg(this);

    m_edtPattern = NULL;
    m_edtKeywords = NULL;

    m_ptInit = ptInit;

    initInterface();

    wxSize sizeM = GetSize();
    SetMinSize(sizeM);
}

LexerDlg::~LexerDlg()
{
}

void LexerDlg::initInterface(void)
{
    SetHelpText(LexerDlg_TITLE);
    if (LexerDlg::ShowToolTips()) {
        SetToolTip(LexerDlg_TITLE);
    }
    SetTitle(LexerDlg_TITLE);

    SetIcon((char **)colors_small_xpm);

    wxBoxSizer *pSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pSizer);

    wxBoxSizer *pSizerAT = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *pSizerAX = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *pSizerA = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerTB = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerB = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerC = new wxBoxSizer(wxHORIZONTAL);

    int iBtnCount = 3;

    wxSize sizeBtn = CometApp::getTextExtent(this, uT(">>KeywordQ<<"));
    int iHedt = sizeBtn.GetHeight();
    int iHbtn = iHedt + 8;
    sizeBtn.SetHeight(iHbtn);

    wxSize sizeEditMulti;
    sizeEditMulti.SetHeight(iHbtn * 5);
    sizeEditMulti.SetWidth((sizeBtn.GetWidth() * (iBtnCount)) + ((iBtnCount - 1) * 4));

    wxSize sizeText;
    sizeText.SetHeight(iHedt);
    sizeText.SetWidth((sizeBtn.GetWidth() * (iBtnCount)) + ((iBtnCount - 1) * 4));

    wxSize sizeEditSm;
    sizeEditSm.SetHeight(iHbtn);
    sizeEditSm.SetWidth(sizeBtn.GetWidth());

    wxSize sizeTextSm;
    sizeTextSm.SetHeight(iHedt);
    sizeTextSm.SetWidth((sizeBtn.GetWidth() * (iBtnCount - 1)) + ((iBtnCount - 2) * 4));

    char_t szName[LM_STRSIZEN];
    Tmemset(szName, 0, LM_STRSIZEN);
    char_t szFilepattern[LM_STRSIZEN];
    Tmemset(szFilepattern, 0, LM_STRSIZEN);
    char_t szKeywords[LEXER_STRSIZE];
    Tmemset(szKeywords, 0, LEXER_STRSIZE);

    CodeEdit::saveCometLexer(szName, szFilepattern, szKeywords);

    m_edtPattern = new wxTextCtrl(this, IDE_LEXER_PATTERN, static_cast<const char_t *>(szFilepattern), wxDefaultPosition, sizeEditSm);
    m_edtPattern->SetMaxLength(LM_STRSIZET - 1);
    m_edtKeywords = new wxTextCtrl(this, IDE_LEXER_KEYWORDS, static_cast<const char_t *>(szKeywords), wxDefaultPosition, sizeEditMulti, wxTE_MULTILINE);
    m_edtKeywords->SetMaxLength(LEXER_STRSIZE - 1);

    pSizerA->AddSpacer(8);
    pSizerAT->Add(new wxStaticText(this, wxID_ANY, uT("User-defined File Extension:"), wxDefaultPosition, sizeTextSm, wxALIGN_RIGHT), 0, wxALL | wxALIGN_CENTER, 0);
    pSizerA->Add(pSizerAT, 0, wxALL | wxALIGN_CENTER, 0);
    pSizerA->AddSpacer(4);
    pSizerAX->Add(m_edtPattern, 0, wxALL | wxALIGN_CENTER, 0);
    pSizerA->Add(pSizerAX, 0, wxALL | wxALIGN_CENTER, 0);
    pSizerA->AddSpacer(8);

    pSizerTB->AddSpacer(8);
    pSizerTB->Add(new wxStaticText(this, wxID_ANY, uT("Keywords (separated by spaces):"), wxDefaultPosition, sizeText, 0), 0, wxALL, 0);
    pSizerTB->AddSpacer(8);

    pSizerB->AddSpacer(8);
    pSizerB->Add(m_edtKeywords, 0, wxALL, 0);
    pSizerB->AddSpacer(8);

    pSizerC->AddSpacer(8);
    pSizerC->Add(new wxButton(this, ID_LEXER_LOAD, uT("&Load..."), wxDefaultPosition, sizeBtn, 0), 0, wxALL, 0);
    pSizerC->AddSpacer(4);
    pSizerC->Add(new wxButton(this, ID_LEXER_SAVE, uT("&Save..."), wxDefaultPosition, sizeBtn, 0), 0, wxALL, 0);
    pSizerC->AddSpacer(4);
    pSizerC->Add(new wxButton(this, wxID_OK, uT("&OK"), wxDefaultPosition, sizeBtn, 0), 0, wxALL, 0);
    pSizerC->AddSpacer(8);

    pSizer->AddSpacer(8);
    pSizer->Add(pSizerA, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerTB, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerB, 0, wxALL, 0);
    pSizer->AddSpacer(16);
    pSizer->Add(pSizerC, 0, wxALL, 0);
    pSizer->AddSpacer(8);

    pSizer->Fit(this);

    Move(m_ptInit);
}

void LexerDlg::OnLexerLoad(wxCommandEvent &WXUNUSED(tEvent))
{
    wxFileDialog *pDlg = new (std::nothrow) wxFileDialog(this, uT("Open User-defined Syntax File"), wxEmptyString, wxEmptyString,
                                                         uT("User-defined Syntax File|*.cometu"),
                                                         wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
    if (pDlg == NULL) {
        return;
    }
    if (pDlg->ShowModal() != wxID_OK) {
        pDlg->Destroy();
        return;
    }
    wxString strFilename = pDlg->GetPath();
    pDlg->Destroy();

    LexerConfig *pLexerConfig = new (std::nothrow) LexerConfig(LexerConfig::SECTIONCOUNT);
    if (pLexerConfig) {
        pLexerConfig->load(LM_CSTR(strFilename));
        delete pLexerConfig;
        pLexerConfig = NULL;

        char_t szName[LM_STRSIZEN];
        Tmemset(szName, 0, LM_STRSIZEN);
        char_t szFilepattern[LM_STRSIZEN];
        Tmemset(szFilepattern, 0, LM_STRSIZEN);
        char_t szKeywords[LEXER_STRSIZE];
        Tmemset(szKeywords, 0, LEXER_STRSIZE);

        CodeEdit::saveCometLexer(szName, szFilepattern, szKeywords);

        m_edtPattern->SetValue(static_cast<const char_t *>(szFilepattern));
        m_edtKeywords->SetValue(static_cast<const char_t *>(szKeywords));
    }
}

void LexerDlg::OnLexerSave(wxCommandEvent &WXUNUSED(tEvent))
{
    wxString strPattern = m_edtPattern->GetValue();
    wxString strKeywords = m_edtKeywords->GetValue();

    if (ScriptEdit::supportedLexer(LM_CSTR(strPattern)) != 0) {
        SigmaMessageBox(uT("File extension already included in the builtin list"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        return;
    }
    if (strPattern.IsWord() == false) {
        SigmaMessageBox(uT("Invalid file extension"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        return;
    }

    strKeywords.Replace(uT("\r"), uT(""), true);
    strKeywords.Replace(uT("\n"), uT(" "), true);
    strKeywords.MakeLower();

    if (strPattern.IsEmpty() || strKeywords.IsEmpty()) {
        SigmaMessageBox(uT("Empty user-defined keywords list"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        return;
    }

    wxFileDialog *pDlg = new (std::nothrow) wxFileDialog(this, uT("Save User-defined Syntax File"), wxEmptyString, wxEmptyString,
                                                         uT("User-defined Syntax File|*.cometu"),
                                                         wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (pDlg == NULL) {
        return;
    }
    if (pDlg->ShowModal() != wxID_OK) {
        pDlg->Destroy();
        return;
    }
    wxString strFilename = pDlg->GetPath();
    pDlg->Destroy();

    if (strFilename.EndsWith(uT(".cometu")) == false) {
        strFilename += uT(".cometu");
    }

    LexerConfig *pLexerConfig = new (std::nothrow) LexerConfig(LexerConfig::SECTIONCOUNT);
    if (pLexerConfig) {
        pLexerConfig->save(LM_CSTR(strFilename), uT("Comet"), LM_CSTR(strPattern), LM_CSTR(strKeywords));
        delete pLexerConfig;
        pLexerConfig = NULL;
    }
}

void LexerDlg::OnLexerOK(wxCommandEvent &WXUNUSED(tEvent))
{
    wxString strPattern = m_edtPattern->GetValue();
    wxString strKeywords = m_edtKeywords->GetValue();

    if (ScriptEdit::supportedLexer(LM_CSTR(strPattern)) != 0) {
        SigmaMessageBox(uT("File extension already included in the builtin list"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        return;
    }
    if (strPattern.IsWord() == false) {
        SigmaMessageBox(uT("Invalid file extension"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        return;
    }

    if ((strPattern.IsEmpty() == false) && (strKeywords.IsEmpty() == false)) {
        CodeEdit::loadCometLexer(uT("Comet"), LM_CSTR(strPattern), LM_CSTR(strKeywords));
    }

    (static_cast<CometFrame *>(GetParent()))->setLexerDlg(NULL);
    (static_cast<CometFrame *>(GetParent()))->DoScriptLexer();

    this->Destroy();
}

void LexerDlg::OnCloseWindow(wxCloseEvent &WXUNUSED(tEvent))
{
    (static_cast<CometFrame *>(GetParent()))->setLexerDlg(NULL);

    this->Destroy();
}

bool LexerDlg::ShowToolTips()
{
    return false;
}
