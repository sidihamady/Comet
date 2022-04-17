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


#ifndef TOOLS_DLG_H
#define TOOLS_DLG_H

#define ToolsDlg_STYLE    (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
#define ToolsDlg_TITLE    uT("External Build Tools")
#define ToolsDlg_SIZE     wxDefaultSize
#define ToolsDlg_POSITION wxDefaultPosition

class ToolsDlg : public wxDialog
{
public:
    ToolsDlg(wxWindow *pParent, int iLexer = 0,
             const wxPoint &ptInit = ToolsDlg_POSITION, wxWindowID idT = IDD_TOOLS,
             const wxString &strTitle = ToolsDlg_TITLE, const wxSize &sizeT = ToolsDlg_SIZE,
             long styleT = ToolsDlg_STYLE);
    virtual ~ToolsDlg();

    void OnTypeChange(wxCommandEvent &tEvent);
    void OnOK(wxCommandEvent &tEvent);
    void OnCancel(wxCommandEvent &tEvent);
    void OnCloseWindow(wxCloseEvent &tEvent);

private:
    DECLARE_EVENT_TABLE()

    char_t m_szCmdBak[TOOLS_MAXCOUNT][LM_STRSIZE];
    bool m_bOutBak[TOOLS_MAXCOUNT];
    bool m_bClsBak[TOOLS_MAXCOUNT];
    bool m_bLogBak;
    bool m_bSilentBak;

    int m_iSel;
    wxComboBox *m_cboType;
    wxTextCtrl *m_edtCmd;
    wxCheckBox *m_optOut;
    wxCheckBox *m_optSilent;
    wxCheckBox *m_optLog;
    wxCheckBox *m_optCls;

#if defined(__WXGTK__)
    wxTextCtrl *m_edtTerm;
    wxTextCtrl *m_edtOpt;
    wxTextCtrl *m_edtShell;
#endif

    wxPoint m_ptInit;
    void initInterface();
    static bool ShowToolTips();
};

#endif
