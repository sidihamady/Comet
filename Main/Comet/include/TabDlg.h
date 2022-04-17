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


#ifndef TAB_DLG_H
#define TAB_DLG_H

#define TabDlg_STYLE    (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
#define TabDlg_TITLE    uT("Indentation")
#define TabDlg_SIZE     wxDefaultSize
#define TabDlg_POSITION wxDefaultPosition

class TabDlg : public wxDialog
{
private:
    DECLARE_EVENT_TABLE()

public:
    TabDlg(wxWindow *pParent, CommonInfo *pCommon, const wxPoint &ptInit = TabDlg_POSITION, wxWindowID idT = IDD_SCRIPT_INDENT,
           const wxString &strTitle = TabDlg_TITLE, const wxSize &sizeT = TabDlg_SIZE, long styleT = TabDlg_STYLE);
    virtual ~TabDlg();

    void setCommon(CommonInfo *pCommon)
    {
        m_pCommon = pCommon;
    }

    void OnTabSelected(wxCommandEvent &tEvent);
    void OnOptSelected(wxCommandEvent &tEvent);
    void OnIndentGuide(wxCommandEvent &tEvent);
    void OnLongLine(wxCommandEvent &tEvent);

    void OnOK(wxCommandEvent &tEvent);
    void OnCancel(wxCommandEvent &tEvent);
    void OnCloseWindow(wxCloseEvent &tEvent);

private:
    CommonInfo *m_pCommon;
    CommonInfo m_CommonBak;

    wxCheckBox *m_pIndentGuide;
    wxCheckBox *m_checkLongLine;
    wxTextCtrl *m_edtLongLine;

    wxRadioBox *m_pTabOpt;
    wxRadioBox *m_pTabSize;
    int m_iTabSize;

    wxPoint m_ptInit;
    void initInterface();
    static bool ShowToolTips();
};

#endif
