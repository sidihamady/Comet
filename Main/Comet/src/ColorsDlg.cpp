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
#include "ConsoleEdit.h"
#include "CometFrame.h"
#include "ColorButton.h"
#include "ColorsDlg.h"

BEGIN_EVENT_TABLE(ColorsDlg, wxDialog)
    EVT_CHECKBOX(IDK_SCRIPT_FONTSTYLE_DEF, ColorsDlg::OnBoldDef)
    EVT_CHECKBOX(IDK_SCRIPT_FONTSTYLE_KEY, ColorsDlg::OnBoldKey)
    EVT_CHECKBOX(IDK_SCRIPT_FONTSTYLE_STR, ColorsDlg::OnBoldStr)
    EVT_CHECKBOX(IDK_SCRIPT_FONTSTYLE_NUM, ColorsDlg::OnBoldNum)
    EVT_CHECKBOX(IDK_SCRIPT_FONTSTYLE_COM, ColorsDlg::OnBoldCom)
    EVT_CHECKBOX(IDK_SCRIPT_FONTSTYLE_LIB, ColorsDlg::OnBoldLib)
    EVT_CHECKBOX(IDK_SCRIPT_FONTSTYLE_PREPROC, ColorsDlg::OnBoldPreprocessor)
    EVT_CHECKBOX(IDK_SCRIPT_FONTSTYLE_IDENT, ColorsDlg::OnBoldIdentifier)
    EVT_CHECKBOX(IDK_SCRIPT_FONTSTYLE_LINENUM, ColorsDlg::OnBoldLinenum)
    EVT_RADIOBOX(ID_SCRIPT_THEME, ColorsDlg::OnThemeSelected)
    EVT_BUTTON(ID_OK, ColorsDlg::OnOK)
    EVT_BUTTON(ID_CANCEL, ColorsDlg::OnCancel)
    EVT_CLOSE(ColorsDlg::OnCloseWindow)
END_EVENT_TABLE()

ColorsDlg::ColorsDlg(wxWindow *pParent, const wxPoint &ptInit, wxWindowID idT,
                     const wxString &strTitle, const wxSize &sizeT, long styleT)
    : wxDialog(pParent, idT, strTitle, ptInit, sizeT, styleT & ~wxRESIZE_BORDER)
{
    (static_cast<CometFrame *>(GetParent()))->setColorsDlg(this);

    ScriptEdit *pEditor = ((CometFrame *)pParent)->getActiveEditor();
    (static_cast<ScriptEdit *>(pEditor))->getPrefs(m_ScintillaPrefs);
    EditorConfig::copy(m_ScintillaPrefsBak, m_ScintillaPrefs);

    m_pColorDef = NULL;
    m_pColorKey = NULL;
    m_pColorStr = NULL;
    m_pColorNum = NULL;
    m_pColorCom = NULL;
    m_pColorLib = NULL;
    m_pColorPreprocessor = NULL;
    m_pColorIdentifier = NULL;
    m_pColorLinenum = NULL;
    m_pColorLinenumBack = NULL;
    m_pColorCaretBack = NULL;
    m_pColorDefBack = NULL;
    m_pColorSelBack = NULL;
    m_pColorWsFore = NULL;

    m_pColorMarkerModified = NULL;
    m_pColorMarkerSaved = NULL;
    m_pColorMarkerFind = NULL;
    m_pColorMarkerError = NULL;

    m_pBoldDef = NULL;
    m_pBoldKey = NULL;
    m_pBoldStr = NULL;
    m_pBoldNum = NULL;
    m_pBoldCom = NULL;
    m_pBoldLib = NULL;
    m_pBoldPreprocessor = NULL;
    m_pBoldIdentifier = NULL;
    m_pBoldLinenum = NULL;

    m_pThemesBox = NULL;
    m_iTheme = 0;

    m_ptInit = ptInit;

    initInterface();

    wxSize sizeM = GetSize();
    SetMinSize(sizeM);
}

ColorsDlg::~ColorsDlg()
{
#ifdef __WXMSW__
    try {
        m_Font.FreeResource(true);
    }
    catch (...) {
    }
#endif
}

void ColorsDlg::DoInitControls(ScintillaPrefs &tScintillaPrefs)
{
    wxColor clrBack = wxColor(tScintillaPrefs.style[wxSTC_LUA_DEFAULT].background);

    m_pColorDef->SetColorFore(tScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground);
    m_pColorDef->SetColorBack(clrBack);

    m_pColorDefBack->SetColorFore(tScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground);
    m_pColorDefBack->SetColorBack(clrBack);

    m_pColorKey->SetColorFore(tScintillaPrefs.style[wxSTC_LUA_WORD].foreground);
    m_pColorKey->SetColorBack(clrBack);

    m_pColorStr->SetColorFore(tScintillaPrefs.style[STYLEINFO_INDEX_STRING_START].foreground);
    m_pColorStr->SetColorBack(clrBack);

    m_pColorNum->SetColorFore(tScintillaPrefs.style[wxSTC_LUA_NUMBER].foreground);
    m_pColorNum->SetColorBack(clrBack);

    m_pColorCom->SetColorFore(tScintillaPrefs.style[STYLEINFO_INDEX_COMMENT_START].foreground);
    m_pColorCom->SetColorBack(clrBack);

    m_pColorLib->SetColorFore(tScintillaPrefs.style[STYLEINFO_INDEX_LIB_START].foreground);
    m_pColorLib->SetColorBack(clrBack);

    m_pColorPreprocessor->SetColorFore(tScintillaPrefs.style[wxSTC_LUA_PREPROCESSOR].foreground);
    m_pColorPreprocessor->SetColorBack(clrBack);

    m_pColorIdentifier->SetColorFore(tScintillaPrefs.style[wxSTC_LUA_IDENTIFIER].foreground);
    m_pColorIdentifier->SetColorBack(clrBack);

    m_pColorLinenum->SetColorFore(tScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].foreground);
    m_pColorLinenum->SetColorBack(wxColor(tScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].background));

    m_pColorLinenumBack->SetColorFore(tScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].foreground);
    m_pColorLinenumBack->SetColorBack(wxColor(tScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].background));

    m_pColorCaretBack->SetColorFore(tScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground);
    m_pColorCaretBack->SetColorBack(wxColor(tScintillaPrefs.style[STYLEINFO_INDEX_CARET].background));

    m_pColorSelBack->SetColorFore(tScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground);
    m_pColorSelBack->SetColorBack(wxColor(tScintillaPrefs.style[STYLEINFO_INDEX_SELECTION].background));

    m_pColorWsFore->SetColorFore(tScintillaPrefs.style[STYLEINFO_INDEX_WHITESPACE].foreground);
    m_pColorWsFore->SetColorBack(wxColor(tScintillaPrefs.style[wxSTC_LUA_DEFAULT].background));

    m_pColorMarkerModified->SetColorFore(tScintillaPrefs.style[wxSTC_LUA_DEFAULT].background);
    m_pColorMarkerModified->SetColorBack(wxColor(tScintillaPrefs.common.marckerColorModified));
    m_pColorMarkerSaved->SetColorFore(tScintillaPrefs.style[wxSTC_LUA_DEFAULT].background);
    m_pColorMarkerSaved->SetColorBack(wxColor(tScintillaPrefs.common.marckerColorSaved));
    m_pColorMarkerFind->SetColorFore(tScintillaPrefs.style[wxSTC_LUA_DEFAULT].background);
    m_pColorMarkerFind->SetColorBack(wxColor(tScintillaPrefs.common.marckerColorFind));
    m_pColorMarkerError->SetColorFore(tScintillaPrefs.style[wxSTC_LUA_DEFAULT].background);
    m_pColorMarkerError->SetColorBack(wxColor(tScintillaPrefs.common.marckerColorError));

    m_pBoldDef->SetValue((tScintillaPrefs.style[wxSTC_LUA_DEFAULT].fontstyle == SCRIPT_STYLE_BOLD));
    m_pBoldKey->SetValue((tScintillaPrefs.style[wxSTC_LUA_WORD].fontstyle == SCRIPT_STYLE_BOLD));
    m_pBoldStr->SetValue((tScintillaPrefs.style[STYLEINFO_INDEX_STRING_START].fontstyle == SCRIPT_STYLE_BOLD));
    m_pBoldNum->SetValue((tScintillaPrefs.style[wxSTC_LUA_NUMBER].fontstyle == SCRIPT_STYLE_BOLD));
    m_pBoldCom->SetValue((tScintillaPrefs.style[STYLEINFO_INDEX_COMMENT_START].fontstyle == SCRIPT_STYLE_BOLD));
    m_pBoldLib->SetValue((tScintillaPrefs.style[STYLEINFO_INDEX_LIB_START].fontstyle == SCRIPT_STYLE_BOLD));
    m_pBoldPreprocessor->SetValue((tScintillaPrefs.style[wxSTC_LUA_PREPROCESSOR].fontstyle == SCRIPT_STYLE_BOLD));
    m_pBoldIdentifier->SetValue((tScintillaPrefs.style[wxSTC_LUA_IDENTIFIER].fontstyle == SCRIPT_STYLE_BOLD));
    m_pBoldLinenum->SetValue((tScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].fontstyle == SCRIPT_STYLE_BOLD));

    m_Font.SetWeight(m_pBoldDef->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorDef->SetFont(m_Font);
    m_pColorDefBack->SetFont(m_Font);
    m_pColorCaretBack->SetFont(m_Font);

    m_Font.SetWeight(m_pBoldKey->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorKey->SetFont(m_Font);

    m_Font.SetWeight(m_pBoldStr->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorStr->SetFont(m_Font);

    m_Font.SetWeight(m_pBoldNum->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorNum->SetFont(m_Font);

    m_Font.SetWeight(m_pBoldCom->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorCom->SetFont(m_Font);

    m_Font.SetWeight(m_pBoldLib->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorLib->SetFont(m_Font);

    m_Font.SetWeight(m_pBoldLinenum->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorLinenum->SetFont(m_Font);
    m_pColorLinenumBack->SetFont(m_Font);

    m_Font.SetWeight(wxFONTWEIGHT_NORMAL);
    m_pColorSelBack->SetFont(m_Font);
    m_pColorWsFore->SetFont(m_Font);
}

void ColorsDlg::initInterface()
{
    SetHelpText(ColorsDlg_TITLE);
    if (ColorsDlg::ShowToolTips()) {
        SetToolTip(ColorsDlg_TITLE);
    }

    SetIcon((char **)colors_small_xpm);

    wxBoxSizer *pSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pSizer);

    wxBoxSizer *pSizerA1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerA2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerB1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerB2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerC1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerC2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerD1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerD2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerE1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerE2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerF1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerF2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerG1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerG2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerH1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerH2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerI1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerI2 = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer *pSizerY = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerZ = new wxBoxSizer(wxHORIZONTAL);

    wxFlexGridSizer *pGridSizer = new wxFlexGridSizer(9, 2, 8, 8);

    m_Font = this->GetFont();

    wxSize sizeText = CometApp::getTextExtent(this, uT(">> Line Numbery <<"));
    wxSize sizeBtn = sizeText;
    sizeBtn.SetHeight(sizeText.GetHeight() + 10);
    sizeText.SetHeight(sizeText.GetHeight() + 8);
    wxSize sizeCheck = CometApp::getTextExtent(this, uT(">> Boldy"));
    sizeCheck.SetHeight(sizeCheck.GetHeight() + 8);
    wxSize sizeCheckSm = CometApp::getTextExtent(this, uT(">> "));
    sizeCheckSm.SetHeight(sizeCheckSm.GetHeight() + 8);

    wxColor clrBack = wxColor(m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background);

    m_pColorDef = new ColorButton(this, ID_SCRIPT_COLOR_DEF, uT("Default"), wxColor(m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground), clrBack, wxDefaultPosition, sizeText, 0);
    m_pBoldDef = new wxCheckBox(this, IDK_SCRIPT_FONTSTYLE_DEF, uT("Bold"), wxDefaultPosition, sizeCheck);

    m_pColorDefBack = new ColorButton(this, ID_SCRIPT_COLOR_DEFBACK, uT("Background"), wxColor(m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground), clrBack, wxDefaultPosition, sizeText, 1);
    m_pColorDefBack->SetBack(true);

    m_pColorKey = new ColorButton(this, ID_SCRIPT_COLOR_KEY, uT("Keyword"), wxColor(m_ScintillaPrefs.style[wxSTC_LUA_WORD].foreground), clrBack, wxDefaultPosition, sizeText, 2);
    m_pBoldKey = new wxCheckBox(this, IDK_SCRIPT_FONTSTYLE_KEY, uT(" "), wxDefaultPosition, sizeCheck);

    m_pColorStr = new ColorButton(this, ID_SCRIPT_COLOR_STR, uT("String"), wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_STRING_START].foreground), clrBack, wxDefaultPosition, sizeText, 3);
    m_pBoldStr = new wxCheckBox(this, IDK_SCRIPT_FONTSTYLE_STR, uT(" "), wxDefaultPosition, sizeCheckSm);

    m_pColorNum = new ColorButton(this, ID_SCRIPT_COLOR_NUM, uT("Number"), wxColor(m_ScintillaPrefs.style[wxSTC_LUA_NUMBER].foreground), clrBack, wxDefaultPosition, sizeText, 4);
    m_pBoldNum = new wxCheckBox(this, IDK_SCRIPT_FONTSTYLE_NUM, uT(" "), wxDefaultPosition, sizeCheck);

    m_pColorCom = new ColorButton(this, ID_SCRIPT_COLOR_COM, uT("Comment"), wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_COMMENT_START].foreground), clrBack, wxDefaultPosition, sizeText, 5);
    m_pBoldCom = new wxCheckBox(this, IDK_SCRIPT_FONTSTYLE_COM, uT(" "), wxDefaultPosition, sizeCheckSm);

    m_pColorLib = new ColorButton(this, ID_SCRIPT_COLOR_LIB, uT("Library"), wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_LIB_START].foreground), clrBack, wxDefaultPosition, sizeText, 6);
    m_pBoldLib = new wxCheckBox(this, IDK_SCRIPT_FONTSTYLE_LIB, uT(" "), wxDefaultPosition, sizeCheck);

    m_pColorCaretBack = new ColorButton(this, ID_SCRIPT_COLOR_CARETBACK, uT("Caret Back."), wxColor(m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground), wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_CARET].background), wxDefaultPosition, sizeText, 7);
    m_pColorCaretBack->SetBack(true);

    m_pColorPreprocessor = new ColorButton(this, ID_SCRIPT_COLOR_PREPROCESSOR, uT("Preprocessor"), wxColor(m_ScintillaPrefs.style[wxSTC_LUA_PREPROCESSOR].foreground), wxColor(m_ScintillaPrefs.style[wxSTC_LUA_PREPROCESSOR].background), wxDefaultPosition, sizeText, 8);
    m_pBoldPreprocessor = new wxCheckBox(this, IDK_SCRIPT_FONTSTYLE_PREPROC, uT(" "), wxDefaultPosition, sizeCheck);

    m_pColorIdentifier = new ColorButton(this, ID_SCRIPT_COLOR_IDENTIFIER, uT("Identifier"), wxColor(m_ScintillaPrefs.style[wxSTC_LUA_IDENTIFIER].foreground), wxColor(m_ScintillaPrefs.style[wxSTC_LUA_IDENTIFIER].background), wxDefaultPosition, sizeText, 9);
    m_pBoldIdentifier = new wxCheckBox(this, IDK_SCRIPT_FONTSTYLE_IDENT, uT(" "), wxDefaultPosition, sizeCheckSm);

    m_pColorLinenum = new ColorButton(this, ID_SCRIPT_COLOR_LINENUM, uT("Line Number"), wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].foreground), wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].background), wxDefaultPosition, sizeText, 10);
    m_pBoldLinenum = new wxCheckBox(this, IDK_SCRIPT_FONTSTYLE_LINENUM, uT(" "), wxDefaultPosition, sizeCheck);
    m_pColorLinenumBack = new ColorButton(this, ID_SCRIPT_COLOR_LINENUM, uT("Margin Back."), wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].foreground), wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].background), wxDefaultPosition, sizeText, 11);
    m_pColorLinenumBack->SetBack(true);

    m_pColorSelBack = new ColorButton(this, ID_SCRIPT_COLOR_SELBACK, uT("Selection Back."), wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_SELECTION].foreground), wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_SELECTION].background), wxDefaultPosition, sizeText, 12);
    m_pColorSelBack->SetBack(true);

    m_pColorWsFore = new ColorButton(this, ID_SCRIPT_COLOR_WSBACK, uT("Whitespace"), wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_WHITESPACE].foreground), wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_WHITESPACE].background), wxDefaultPosition, sizeText, 13);

    m_pColorMarkerModified = new ColorButton(this, ID_SCRIPT_COLOR_MARKMOD, uT("Marker <Modified>"), wxColor(m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background), wxColor(m_ScintillaPrefs.common.marckerColorModified), wxDefaultPosition, sizeText, 14);
    m_pColorMarkerModified->SetBack(true);
    m_pColorMarkerSaved = new ColorButton(this, ID_SCRIPT_COLOR_MARKSAVED, uT("Marker <Saved>"), wxColor(m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background), wxColor(m_ScintillaPrefs.common.marckerColorSaved), wxDefaultPosition, sizeText, 15);
    m_pColorMarkerSaved->SetBack(true);
    m_pColorMarkerFind = new ColorButton(this, ID_SCRIPT_COLOR_MARKFIND, uT("Marker <Find>"), wxColor(m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background), wxColor(m_ScintillaPrefs.common.marckerColorFind), wxDefaultPosition, sizeText, 16);
    m_pColorMarkerFind->SetBack(true);
    m_pColorMarkerError = new ColorButton(this, ID_SCRIPT_COLOR_MARKERROR, uT("Marker <Error>"), wxColor(m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background), wxColor(m_ScintillaPrefs.common.marckerColorError), wxDefaultPosition, sizeText, 17);
    m_pColorMarkerError->SetBack(true);

    pSizerA1->AddSpacer(8);
    pSizerA1->Add(m_pColorDef, 0, wxALL, 0);
    pSizerA1->AddSpacer(8);
    pSizerA1->Add(m_pBoldDef, 0, wxALL, 0);

    pSizerA2->Add(m_pColorDefBack, 0, wxALL, 0);
    pSizerA2->AddSpacer(8);

    pGridSizer->Add(pSizerA1, 0, wxALL, 0);
    pGridSizer->Add(pSizerA2, 0, wxALL, 0);

    pSizerB1->AddSpacer(8);
    pSizerB1->Add(m_pColorKey, 0, wxALL, 0);
    pSizerB1->AddSpacer(8);
    pSizerB1->Add(m_pBoldKey, 0, wxALL, 0);

    pSizerB2->Add(m_pColorCaretBack, 0, wxALL, 0);
    pSizerB2->AddSpacer(8);

    pGridSizer->Add(pSizerB1, 0, wxALL, 0);
    pGridSizer->Add(pSizerB2, 0, wxALL, 0);

    pSizerC1->AddSpacer(8);
    pSizerC1->Add(m_pColorNum, 0, wxALL, 0);
    pSizerC1->AddSpacer(8);
    pSizerC1->Add(m_pBoldNum, 0, wxALL, 0);

    pSizerC2->Add(m_pColorLinenumBack, 0, wxALL, 0);
    pSizerC2->AddSpacer(8);

    pGridSizer->Add(pSizerC1, 0, wxALL, 0);
    pGridSizer->Add(pSizerC2, 0, wxALL, 0);

    pSizerD1->AddSpacer(8);
    pSizerD1->Add(m_pColorLib, 0, wxALL, 0);
    pSizerD1->AddSpacer(8);
    pSizerD1->Add(m_pBoldLib, 0, wxALL, 0);

    pSizerD2->Add(m_pColorWsFore, 0, wxALL, 0);
    pSizerD2->AddSpacer(8);

    pGridSizer->Add(pSizerD1, 0, wxALL, 0);
    pGridSizer->Add(pSizerD2, 0, wxALL, 0);

    pSizerE1->AddSpacer(8);
    pSizerE1->Add(m_pColorPreprocessor, 0, wxALL, 0);
    pSizerE1->AddSpacer(8);
    pSizerE1->Add(m_pBoldPreprocessor, 0, wxALL, 0);

    pSizerE2->Add(m_pColorSelBack, 0, wxALL, 0);
    pSizerE2->AddSpacer(8);

    pGridSizer->Add(pSizerE1, 0, wxALL, 0);
    pGridSizer->Add(pSizerE2, 0, wxALL, 0);

    pSizerF1->AddSpacer(8);
    pSizerF1->Add(m_pColorLinenum, 0, wxALL, 0);
    pSizerF1->AddSpacer(8);
    pSizerF1->Add(m_pBoldLinenum, 0, wxALL, 0);

    pSizerF2->Add(m_pColorMarkerFind, 0, wxALL, 0);
    pSizerF2->AddSpacer(8);

    pGridSizer->Add(pSizerF1, 0, wxALL, 0);
    pGridSizer->Add(pSizerF2, 0, wxALL, 0);

    pSizerG1->AddSpacer(8);
    pSizerG1->Add(m_pColorStr, 0, wxALL, 0);
    pSizerG1->AddSpacer(8);
    pSizerG1->Add(m_pBoldStr, 0, wxALL, 0);

    pSizerG2->Add(m_pColorMarkerModified, 0, wxALL, 0);
    pSizerG2->AddSpacer(8);

    pGridSizer->Add(pSizerG1, 0, wxALL, 0);
    pGridSizer->Add(pSizerG2, 0, wxALL, 0);

    pSizerH1->AddSpacer(8);
    pSizerH1->Add(m_pColorCom, 0, wxALL, 0);
    pSizerH1->AddSpacer(8);
    pSizerH1->Add(m_pBoldCom, 0, wxALL, 0);

    pSizerH2->Add(m_pColorMarkerSaved, 0, wxALL, 0);
    pSizerH2->AddSpacer(8);

    pGridSizer->Add(pSizerH1, 0, wxALL, 0);
    pGridSizer->Add(pSizerH2, 0, wxALL, 0);

    pSizerI1->AddSpacer(8);
    pSizerI1->Add(m_pColorIdentifier, 0, wxALL, 0);
    pSizerI1->AddSpacer(8);
    pSizerI1->Add(m_pBoldIdentifier, 0, wxALL, 0);

    pSizerI2->Add(m_pColorMarkerError, 0, wxALL, 0);
    pSizerI2->AddSpacer(8);

    pGridSizer->Add(pSizerI1, 0, wxALL, 0);
    pGridSizer->Add(pSizerI2, 0, wxALL, 0);

    wxArrayString arrThemes;
    arrThemes.Add(uT("Custom"));
    arrThemes.Add(uT("Light"));
    arrThemes.Add(uT("Dark"));

    wxSize sizeBox = sizeText;
    sizeBox.SetWidth((sizeText.GetWidth() * 2) + sizeCheck.GetWidth() + sizeCheckSm.GetWidth());
    sizeBox.SetHeight((sizeBox.GetHeight() * 2) + 8);
    m_pThemesBox = new wxRadioBox(this, ID_SCRIPT_THEME, uT("Theme"), wxDefaultPosition, sizeBox, arrThemes, 1, wxRA_SPECIFY_ROWS, wxDefaultValidator, uT("Themes"));
    m_pThemesBox->SetSelection(0);

    pSizerY->AddSpacer(8);
    pSizerY->Add(m_pThemesBox, 0, wxCENTER, 0);
    pSizerY->AddSpacer(8);

    pSizerZ->AddSpacer(8);
    pSizerZ->Add(new wxButton(this, ID_OK, uT("&OK"), wxDefaultPosition, sizeBtn, 0), 0, wxCENTER, 0);
    pSizerZ->AddSpacer(8);
    pSizerZ->Add(new wxButton(this, ID_CANCEL, uT("&Cancel"), wxDefaultPosition, sizeBtn, 0), 0, wxCENTER, 0);
    pSizerZ->AddSpacer(8);

    pSizer->AddSpacer(8);
    pSizer->Add(pGridSizer, 0, wxALL, 0);
    pSizer->AddSpacer(8);
    pSizer->Add(pSizerY, 0, wxALL, 0);
    pSizer->AddSpacer(16);
    pSizer->Add(pSizerZ, 0, wxCENTER, 0);
    pSizer->AddSpacer(8);

    pSizer->Fit(this);

    Move(m_ptInit);

    DoInitControls(m_ScintillaPrefs);
}

void ColorsDlg::updateStyle(bool bCustom /* = true*/)
{
    // Bolding
    bool bChecked = m_pBoldDef->IsChecked();
    m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].fontstyle = (bChecked ? SCRIPT_STYLE_BOLD : 0);
    m_ScintillaPrefs.style[wxSTC_LUA_OPERATOR].fontstyle = (bChecked ? SCRIPT_STYLE_BOLD : 0);

    m_ScintillaPrefs.style[wxSTC_LUA_WORD].fontstyle = (m_pBoldKey->IsChecked() ? SCRIPT_STYLE_BOLD : 0);

    m_ScintillaPrefs.style[wxSTC_LUA_PREPROCESSOR].fontstyle = (m_pBoldPreprocessor->IsChecked() ? SCRIPT_STYLE_BOLD : 0);
    m_ScintillaPrefs.style[wxSTC_LUA_IDENTIFIER].fontstyle = (m_pBoldIdentifier->IsChecked() ? SCRIPT_STYLE_BOLD : 0);

    int iStyle = (m_pBoldStr->IsChecked() ? SCRIPT_STYLE_BOLD : 0);
    for (int ii = STYLEINFO_INDEX_STRING_START; ii <= STYLEINFO_INDEX_STRING_END; ii++) {
        m_ScintillaPrefs.style[ii].fontstyle = iStyle;
    }
    m_ScintillaPrefs.style[wxSTC_LUA_STRINGEOL].fontstyle = iStyle;

    m_ScintillaPrefs.style[wxSTC_LUA_NUMBER].fontstyle = (m_pBoldNum->IsChecked() ? SCRIPT_STYLE_BOLD : 0);

    iStyle = (m_pBoldCom->IsChecked() ? SCRIPT_STYLE_BOLD : 0);
    for (int ii = STYLEINFO_INDEX_COMMENT_START; ii <= STYLEINFO_INDEX_COMMENT_END; ii++) {
        m_ScintillaPrefs.style[ii].fontstyle = iStyle;
    }

    iStyle = (m_pBoldLib->IsChecked() ? SCRIPT_STYLE_BOLD : 0);
    for (int ii = STYLEINFO_INDEX_LIB_START; ii <= STYLEINFO_INDEX_LIB_END; ii++) {
        m_ScintillaPrefs.style[ii].fontstyle = iStyle;
    }
    m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].fontstyle = (m_pBoldLinenum->IsChecked() ? SCRIPT_STYLE_BOLD : 0);

    // Colors
    unsigned long iB = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
    unsigned long iF = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;

    wxColor clrBackPrev = m_pColorDef->GetColorBack();

    wxColor clrT = m_pColorDef->GetColorFore();
    unsigned long iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground = iT;
    wxColor clrBack = m_pColorDefBack->GetColorBack();
    iT = LM_COLOR(clrBack.Red(), clrBack.Green(), clrBack.Blue());
    m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background = iT;

    wxColor clrLinenumBackPrev = m_pColorLinenum->GetColorBack();
    wxColor clrLinenumBack = m_pColorLinenumBack->GetColorBack();
    iT = LM_COLOR(clrLinenumBack.Red(), clrLinenumBack.Green(), clrLinenumBack.Blue());
    m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].background = iT;

    clrT = m_pColorCaretBack->GetColorBack();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    m_ScintillaPrefs.style[STYLEINFO_INDEX_CARET].background = iT;

    m_ScintillaPrefs.style[wxSTC_LUA_OPERATOR].foreground = iT;
    m_ScintillaPrefs.style[wxSTC_LUA_IDENTIFIER].foreground = iT;
    if (clrBack != clrBackPrev) {
        m_pColorDef->SetColorBack(clrBack);
        m_pColorDef->Refresh();
    }

    clrT = m_pColorKey->GetColorFore();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    m_ScintillaPrefs.style[wxSTC_LUA_WORD].foreground = iT;
    if (clrBack != clrBackPrev) {
        m_pColorKey->SetColorBack(clrBack);
        m_pColorKey->Refresh();
    }

    clrT = m_pColorStr->GetColorFore();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    for (int ii = STYLEINFO_INDEX_STRING_START; ii <= STYLEINFO_INDEX_STRING_END; ii++) {
        m_ScintillaPrefs.style[ii].foreground = iT;
    }
    m_ScintillaPrefs.style[wxSTC_LUA_STRINGEOL].foreground = iT;
    if (clrBack != clrBackPrev) {
        m_pColorStr->SetColorBack(clrBack);
        m_pColorStr->Refresh();
    }

    clrT = m_pColorNum->GetColorFore();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    m_ScintillaPrefs.style[wxSTC_LUA_NUMBER].foreground = iT;
    if (clrBack != clrBackPrev) {
        m_pColorNum->SetColorBack(clrBack);
        m_pColorNum->Refresh();
    }

    clrT = m_pColorCom->GetColorFore();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    for (int ii = STYLEINFO_INDEX_COMMENT_START; ii <= STYLEINFO_INDEX_COMMENT_END; ii++) {
        m_ScintillaPrefs.style[ii].foreground = iT;
    }
    if (clrBack != clrBackPrev) {
        m_pColorCom->SetColorBack(clrBack);
        m_pColorCom->Refresh();
    }

    clrT = m_pColorLib->GetColorFore();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    for (int ii = STYLEINFO_INDEX_LIB_START; ii <= STYLEINFO_INDEX_LIB_END; ii++) {
        m_ScintillaPrefs.style[ii].foreground = iT;
    }
    if (clrBack != clrBackPrev) {
        m_pColorLib->SetColorBack(clrBack);
        m_pColorLib->Refresh();
    }

    clrT = m_pColorPreprocessor->GetColorFore();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    m_ScintillaPrefs.style[wxSTC_LUA_PREPROCESSOR].foreground = iT;
    if (clrBack != clrBackPrev) {
        m_pColorPreprocessor->SetColorBack(clrBack);
        m_pColorPreprocessor->Refresh();
    }

    clrT = m_pColorIdentifier->GetColorFore();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    m_ScintillaPrefs.style[wxSTC_LUA_IDENTIFIER].foreground = iT;
    if (clrBack != clrBackPrev) {
        m_pColorIdentifier->SetColorBack(clrBack);
        m_pColorIdentifier->Refresh();
    }

    clrT = m_pColorLinenum->GetColorFore();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].foreground = iT;
    if (clrLinenumBackPrev != clrLinenumBack) {
        m_pColorLinenum->SetColorBack(clrLinenumBack);
        m_pColorLinenum->Refresh();
    }

    wxColor clrLinenumFore = m_pColorLinenum->GetColorFore();
    wxColor clrLinenumBackFore = m_pColorLinenumBack->GetColorFore();
    if (clrLinenumBackFore != clrLinenumFore) {
        m_pColorLinenumBack->SetColorFore(clrLinenumFore);
        m_pColorLinenumBack->Refresh();
    }

    clrT = m_pColorSelBack->GetColorBack();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    m_ScintillaPrefs.style[STYLEINFO_INDEX_SELECTION].background = iT;

    clrT = m_pColorWsFore->GetColorFore();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    m_ScintillaPrefs.style[STYLEINFO_INDEX_WHITESPACE].foreground = iT;

    clrT = m_pColorMarkerModified->GetColorBack();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    m_ScintillaPrefs.common.marckerColorModified = iT;

    clrT = m_pColorMarkerSaved->GetColorBack();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    m_ScintillaPrefs.common.marckerColorSaved = iT;

    clrT = m_pColorMarkerFind->GetColorBack();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    m_ScintillaPrefs.common.marckerColorFind = iT;

    clrT = m_pColorMarkerError->GetColorBack();
    iT = LM_COLOR(clrT.Red(), clrT.Green(), clrT.Blue());
    m_ScintillaPrefs.common.marckerColorError = iT;

    if (bCustom) {
        m_pThemesBox->SetSelection(0);
    }

    ScriptEdit *pEditor = ((CometFrame *)(GetParent()))->getActiveEditor();
    (static_cast<ScriptEdit *>(pEditor))->setPrefs(m_ScintillaPrefs, true, false);

    ConsoleEdit *pConsole = (static_cast<CometFrame *>(GetParent()))->getConsole();
    if (pConsole) {
        pConsole->setPrefs(m_ScintillaPrefs, true);
    }
    OutputEdit *pOutput = (static_cast<CometFrame *>(GetParent()))->getOutput();
    if (pOutput) {
        pOutput->setPrefs(m_ScintillaPrefs, true);
    }

    if ((iB != m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background) || (iF != m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground)) {
        (static_cast<CometFrame *>(GetParent()))->initToolPrefs(m_ScintillaPrefs);
    }
}

void ColorsDlg::OnBoldDef(wxCommandEvent &WXUNUSED(tEvent))
{
    updateStyle();

    m_Font.SetWeight(m_pBoldDef->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorDef->SetFont(m_Font);
    m_pColorDefBack->SetFont(m_Font);
    m_pColorCaretBack->SetFont(m_Font);

    m_pColorDef->Refresh();
    m_pColorDefBack->Refresh();
    m_pColorCaretBack->Refresh();
}
void ColorsDlg::OnBoldKey(wxCommandEvent &WXUNUSED(tEvent))
{
    updateStyle();

    m_Font.SetWeight(m_pBoldKey->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorKey->SetFont(m_Font);

    m_pColorKey->Refresh();
}
void ColorsDlg::OnBoldStr(wxCommandEvent &WXUNUSED(tEvent))
{
    updateStyle();

    m_Font.SetWeight(m_pBoldStr->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorStr->SetFont(m_Font);

    m_pColorStr->Refresh();
}
void ColorsDlg::OnBoldNum(wxCommandEvent &WXUNUSED(tEvent))
{
    updateStyle();

    m_Font.SetWeight(m_pBoldNum->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorNum->SetFont(m_Font);

    m_pColorNum->Refresh();
}
void ColorsDlg::OnBoldCom(wxCommandEvent &WXUNUSED(tEvent))
{
    updateStyle();

    m_Font.SetWeight(m_pBoldCom->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorCom->SetFont(m_Font);

    m_pColorCom->Refresh();
}
void ColorsDlg::OnBoldLib(wxCommandEvent &WXUNUSED(tEvent))
{
    updateStyle();

    m_Font.SetWeight(m_pBoldLib->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorLib->SetFont(m_Font);

    m_pColorLib->Refresh();
}
void ColorsDlg::OnBoldPreprocessor(wxCommandEvent &WXUNUSED(tEvent))
{
    updateStyle();

    m_Font.SetWeight(m_pBoldPreprocessor->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorPreprocessor->SetFont(m_Font);

    m_pColorPreprocessor->Refresh();
}
void ColorsDlg::OnBoldIdentifier(wxCommandEvent &WXUNUSED(tEvent))
{
    updateStyle();

    m_Font.SetWeight(m_pBoldIdentifier->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorIdentifier->SetFont(m_Font);

    m_pColorIdentifier->Refresh();
}
void ColorsDlg::OnBoldLinenum(wxCommandEvent &WXUNUSED(tEvent))
{
    updateStyle();

    m_Font.SetWeight(m_pBoldLinenum->IsChecked() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    m_pColorLinenum->SetFont(m_Font);
    m_pColorLinenumBack->SetFont(m_Font);

    m_pColorLinenum->Refresh();
    m_pColorLinenumBack->Refresh();
}

void ColorsDlg::OnThemeSelected(wxCommandEvent &WXUNUSED(tEvent))
{
    int iThemePrev = m_iTheme;
    m_iTheme = m_pThemesBox->GetSelection();
    if (m_iTheme != iThemePrev) {
        if (m_iTheme == 0) {
            DoInitControls(m_ScintillaPrefs);
            Refresh();
            updateStyle(false);
        }
        else if (m_iTheme == 1) {
            ScintillaPrefs tScintillaPrefs;
            for (int ii = 0; ii < STYLEINFO_COUNT; ii++) {
                tScintillaPrefs.style[ii].id = ScriptEdit::STYLE_LIGHT[ii].id;
                tScintillaPrefs.style[ii].idl = ScriptEdit::STYLE_LIGHT[ii].idl;
                tScintillaPrefs.style[ii].foreground = ScriptEdit::STYLE_LIGHT[ii].foreground;
                tScintillaPrefs.style[ii].background = ScriptEdit::STYLE_LIGHT[ii].background;
                wxStrcpy(tScintillaPrefs.style[ii].fontname, ScriptEdit::STYLE_LIGHT[ii].fontname);
                tScintillaPrefs.style[ii].fontsize = ScriptEdit::STYLE_LIGHT[ii].fontsize;
                tScintillaPrefs.style[ii].fontstyle = ScriptEdit::STYLE_LIGHT[ii].fontstyle;
                tScintillaPrefs.style[ii].lettercase = ScriptEdit::STYLE_LIGHT[ii].lettercase;
            }
            tScintillaPrefs.common.marckerColorModified = 0x00B1E6;
            tScintillaPrefs.common.marckerColorSaved = 0x009700;
            tScintillaPrefs.common.marckerColorFind = 0xC24230;
            tScintillaPrefs.common.marckerColorError = 0x0000FF;
            DoInitControls(tScintillaPrefs);
            Refresh();
            updateStyle(false);
        }
        else if (m_iTheme == 2) {
            ScintillaPrefs tScintillaPrefs;
            for (int ii = 0; ii < STYLEINFO_COUNT; ii++) {
                tScintillaPrefs.style[ii].id = ScriptEdit::STYLE_DARK[ii].id;
                tScintillaPrefs.style[ii].idl = ScriptEdit::STYLE_DARK[ii].idl;
                tScintillaPrefs.style[ii].foreground = ScriptEdit::STYLE_DARK[ii].foreground;
                tScintillaPrefs.style[ii].background = ScriptEdit::STYLE_DARK[ii].background;
                wxStrcpy(tScintillaPrefs.style[ii].fontname, ScriptEdit::STYLE_DARK[ii].fontname);
                tScintillaPrefs.style[ii].fontsize = ScriptEdit::STYLE_DARK[ii].fontsize;
                tScintillaPrefs.style[ii].fontstyle = ScriptEdit::STYLE_DARK[ii].fontstyle;
                tScintillaPrefs.style[ii].lettercase = ScriptEdit::STYLE_DARK[ii].lettercase;
            }
            tScintillaPrefs.common.marckerColorModified = 0x46B5F0;
            tScintillaPrefs.common.marckerColorSaved = 0x36C12B;
            tScintillaPrefs.common.marckerColorFind = 0xF4998E;
            tScintillaPrefs.common.marckerColorError = 0x7171FF;
            DoInitControls(tScintillaPrefs);
            Refresh();
            updateStyle(false);
        }
    }
}

void ColorsDlg::DoCancel(void)
{
    ScriptEdit *pEditor = ((CometFrame *)(GetParent()))->getActiveEditor();
    (static_cast<ScriptEdit *>(pEditor))->setPrefs(m_ScintillaPrefsBak, true, false);
    ConsoleEdit *pConsole = (static_cast<CometFrame *>(GetParent()))->getConsole();
    if (pConsole) {
        pConsole->setPrefs(m_ScintillaPrefsBak, true);
    }
    OutputEdit *pOutput = (static_cast<CometFrame *>(GetParent()))->getOutput();
    if (pOutput) {
        pOutput->setPrefs(m_ScintillaPrefsBak, true);
    }
    if ((m_ScintillaPrefsBak.style[wxSTC_LUA_DEFAULT].background != m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background) || (m_ScintillaPrefsBak.style[wxSTC_LUA_DEFAULT].foreground != m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground)) {
        (static_cast<CometFrame *>(GetParent()))->initToolPrefs(m_ScintillaPrefsBak);
    }
}

void ColorsDlg::OnOK(wxCommandEvent &WXUNUSED(tEvent))
{
    (static_cast<CometFrame *>(GetParent()))->setColorsDlg(NULL);
    (static_cast<CometFrame *>(GetParent()))->DoScriptColors();

    this->Destroy();
}

void ColorsDlg::OnCancel(wxCommandEvent &WXUNUSED(tEvent))
{
    DoCancel();

    (static_cast<CometFrame *>(GetParent()))->setColorsDlg(NULL);

    this->Destroy();
}

void ColorsDlg::OnCloseWindow(wxCloseEvent &WXUNUSED(tEvent))
{
    DoCancel();

    (static_cast<CometFrame *>(GetParent()))->setColorsDlg(NULL);

    this->Destroy();
}

bool ColorsDlg::ShowToolTips()
{
    return false;
}
