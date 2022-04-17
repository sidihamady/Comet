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


#ifndef COLORS_DLG_H
#define COLORS_DLG_H

#define ColorsDlg_STYLE    (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
#define ColorsDlg_TITLE    uT("Editor Colors")
#define ColorsDlg_SIZE     wxDefaultSize
#define ColorsDlg_POSITION wxDefaultPosition

class ColorsDlg : public wxDialog
{
public:
    ColorsDlg(wxWindow *pParent, const wxPoint &ptInit = ColorsDlg_POSITION, wxWindowID idT = IDD_SCRIPT_COLORS,
              const wxString &strTitle = ColorsDlg_TITLE, const wxSize &sizeT = ColorsDlg_SIZE, long styleT = ColorsDlg_STYLE);
    virtual ~ColorsDlg();

    void updateStyle(bool bCustom = true);

    void OnBoldDef(wxCommandEvent &tEvent);
    void OnBoldKey(wxCommandEvent &tEvent);
    void OnBoldStr(wxCommandEvent &tEvent);
    void OnBoldNum(wxCommandEvent &tEvent);
    void OnBoldCom(wxCommandEvent &tEvent);
    void OnBoldLib(wxCommandEvent &tEvent);
    void OnBoldPreprocessor(wxCommandEvent &tEvent);
    void OnBoldIdentifier(wxCommandEvent &tEvent);
    void OnBoldLinenum(wxCommandEvent &tEvent);

    void OnThemeSelected(wxCommandEvent &tEvent);

    void OnOK(wxCommandEvent &tEvent);
    void OnCancel(wxCommandEvent &tEvent);
    void OnCloseWindow(wxCloseEvent &tEvent);

private:
    wxFont m_Font;

    DECLARE_EVENT_TABLE()

    ScintillaPrefs m_ScintillaPrefs;
    ScintillaPrefs m_ScintillaPrefsBak;

    ColorButton *m_pColorDef;
    ColorButton *m_pColorKey;
    ColorButton *m_pColorStr;
    ColorButton *m_pColorNum;
    ColorButton *m_pColorCom;
    ColorButton *m_pColorLib;
    ColorButton *m_pColorPreprocessor;
    ColorButton *m_pColorIdentifier;
    ColorButton *m_pColorLinenum;
    ColorButton *m_pColorLinenumBack;
    ColorButton *m_pColorDefBack;
    ColorButton *m_pColorCaretBack;
    ColorButton *m_pColorSelBack;
    ColorButton *m_pColorWsFore;

    ColorButton *m_pColorMarkerModified;
    ColorButton *m_pColorMarkerSaved;
    ColorButton *m_pColorMarkerFind;
    ColorButton *m_pColorMarkerError;

    wxCheckBox *m_pBoldDef;
    wxCheckBox *m_pBoldKey;
    wxCheckBox *m_pBoldStr;
    wxCheckBox *m_pBoldNum;
    wxCheckBox *m_pBoldCom;
    wxCheckBox *m_pBoldLib;
    wxCheckBox *m_pBoldPreprocessor;
    wxCheckBox *m_pBoldIdentifier;
    wxCheckBox *m_pBoldLinenum;

    wxRadioBox *m_pThemesBox;
    int m_iTheme;

    wxPoint m_ptInit;
    void DoInitControls(ScintillaPrefs &tScintillaPrefs);
    void initInterface();
    static bool ShowToolTips();
    void DoCancel(void);
};

#endif
