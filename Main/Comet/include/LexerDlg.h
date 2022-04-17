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


#ifndef LEXER_DLG_H
#define LEXER_DLG_H

#include <wx/textctrl.h>

#include "Identifiers.h"

#define LexerDlg_STYLE    (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
#define LexerDlg_TITLE    uT("User-defined Syntax Highlighting")
#define LexerDlg_SIZE     wxDefaultSize
#define LexerDlg_POSITION wxDefaultPosition

class LexerDlg : public wxDialog
{
public:
    LexerDlg(wxWindow *pParent, wxString strFind = wxEmptyString, bool bReplace = false,
             const wxPoint &ptInit = LexerDlg_POSITION, wxWindowID idT = IDD_LEXER,
             const wxString &strTitle = LexerDlg_TITLE, const wxSize &sizeT = LexerDlg_SIZE,
             long styleT = LexerDlg_STYLE);
    virtual ~LexerDlg();

    void OnLexerLoad(wxCommandEvent &tEvent);
    void OnLexerSave(wxCommandEvent &tEvent);

    void OnLexerOK(wxCommandEvent &tEvent);
    void OnCloseWindow(wxCloseEvent &tEvent);

private:
    DECLARE_EVENT_TABLE()

    wxTextCtrl *m_edtPattern;
    wxTextCtrl *m_edtKeywords;

    wxPoint m_ptInit;
    void initInterface(void);
    static bool ShowToolTips();
};

#endif
