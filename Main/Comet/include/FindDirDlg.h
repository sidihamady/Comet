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


#ifndef FINDDIR_DLG_H
#define FINDDIR_DLG_H

#include <wx/listctrl.h>
#include <wx/tokenzr.h>

#include "CometComboBox.h"

#define FindDirDlg_STYLE    (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxRESIZE_BORDER)
#define FindDirDlg_TITLE    uT("Find in Files")
#define FindDirDlg_TITLEX   uT("Replace in Files")
#define FindDirDlg_SIZE     wxDefaultSize
#define FindDirDlg_POSITION wxDefaultPosition

#define FINDLIST_MAXITEMS 16383

class DirFindReplaceComboBox : public CometComboBox
{
private:
    bool m_bReplace;

protected:
    DECLARE_EVENT_TABLE()

public:
    DirFindReplaceComboBox(wxWindow *pParent, wxWindowID idT,
                           const wxString &valueT = wxEmptyString,
                           const wxPoint &posT = wxDefaultPosition,
                           const wxSize &sizeT = wxDefaultSize,
                           int nn = 0, const wxString choicesT[] = NULL,
                           long styleT = 0,
                           const wxValidator &validatorT = wxDefaultValidator,
                           const wxString &nameT = wxComboBoxNameStr);

    void OnKeyDown(wxKeyEvent &tEvent);

    void setReplace(bool bReplace)
    {
        m_bReplace = bReplace;
    }
    bool isReplace(void)
    {
        return m_bReplace;
    }
};

class FindDirDlg : public wxDialog
{

public:
    FindDirDlg(wxWindow *pParent, wxString strFind = wxEmptyString, bool bReplace = false,
               const wxPoint &ptInit = FindDirDlg_POSITION, wxWindowID idT = IDD_FINDDIR,
               const wxString &strTitle = FindDirDlg_TITLE, const wxSize &sizeT = FindDirDlg_SIZE,
               long styleT = FindDirDlg_STYLE);
    virtual ~FindDirDlg();

    // >> Communication between threads
    wxMutex *m_pThreadMutex;
    bool m_bThreadRunning;
    bool m_bThreadStop;
    wxArrayString m_arrBuffer;
    wxString m_strFilename;
    // <<

    bool findOnly(void)
    {
        return (m_bReplace == false);
    }

    void updateLabel(const wxString &strT);
    void updateTypeList(void);
    void updateDirList(const wxString &strDir);
    void updateFindList(const wxString &strFind);
    void updateReplaceList(const wxString &strReplace);
    void updateItems(const wxString &strFind, const wxString &strReplace, const wxString &strDir);

    void endTask(const wxString &strT);
    void updateList(void);

    void DoFindAll(void);
    void DoReplaceAll(void);

    void DoFindReplace(bool bReplace = false);

    void OnTimer(wxTimerEvent &tEvent);

    void gotoItem(long iSel);
    void OnGoto(wxCommandEvent &tEvent);
    void OnDelete(wxCommandEvent &tEvent);
    void OnDeleteAll(wxCommandEvent &tEvent);
    void OnMainAction(wxCommandEvent &tEvent);

    wxString getFind(void)
    {
        if (m_cboFind) {
            return m_cboFind->GetValue();
        }
        return wxEmptyString;
    }
    wxString getReplace(void)
    {
        if (m_cboReplace) {
            return m_cboReplace->GetValue();
        }
        return wxEmptyString;
    }

    void OnBrowseDir(wxCommandEvent &tEvent);
    void OnFindAll(wxCommandEvent &tEvent);
    void OnReplaceAll(wxCommandEvent &tEvent);
    void OnListRightClick(wxListEvent &tEvent);
    void OnListDoubleClick(wxListEvent &tEvent);
    void OnFindClose(wxCommandEvent &tEvent);

    void OnCloseWindow(wxCloseEvent &tEvent);

private:
    DECLARE_EVENT_TABLE()

    wxString m_strFind;
    bool m_bReplace;
    bool m_bReplacing;
    wxString m_strReplace;
    wxString m_strDir;
    int m_iFind;

    wxTimer *m_pTimer;

    wxString m_strFilenamePrev;

    DirFindReplaceComboBox *m_cboFind;
    DirFindReplaceComboBox *m_cboReplace;

    wxComboBox *m_cboType;

    CometComboBox *m_cboDir;

    wxButton *m_btnBrowseDir;

    wxCheckBox *m_optSubDir;
    wxCheckBox *m_optCase;
    wxCheckBox *m_optWord;

    wxButton *m_btnFindAll;
    wxButton *m_btnReplaceAll;
    wxButton *m_btnClose;

    wxStaticText *m_txtStatus;

    wxListCtrl *m_listFind;

    inline bool setRunning(bool bRunning);
    inline bool isRunning(void);
    inline bool stopIt(bool bStopIt);

    bool doUpdateList(const wxString &strT);

    wxPoint m_ptInit;
    void initInterface();
    static bool ShowToolTips();
};

#endif
