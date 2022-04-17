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


#ifndef FINDFILE_DLG_H
#define FINDFILE_DLG_H

#include <wx/listctrl.h>

#include "CometComboBox.h"

#define FindFileDlg_STYLE    (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
#define FindFileDlg_TITLE    uT("Find")
#define FindFileDlg_TITLEX   uT("Find/Replace")
#define FindFileDlg_SIZE     wxDefaultSize
#define FindFileDlg_POSITION wxDefaultPosition

#define FINDFILE_MAXWND 12

class FindReplaceComboBox : public CometComboBox
{
private:
    bool m_bReplace;

protected:
    DECLARE_EVENT_TABLE()

public:
    FindReplaceComboBox(wxWindow *pParent, wxWindowID idT,
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

class FindFileDlg : public wxDialog
{
public:
    FindFileDlg(wxWindow *pParent, wxString strFind = wxEmptyString, bool bReplace = false,
                const wxPoint &ptInit = FindFileDlg_POSITION, wxWindowID idT = IDD_FIND,
                const wxString &strTitle = FindFileDlg_TITLE, const wxSize &sizeT = FindFileDlg_SIZE,
                long styleT = FindFileDlg_STYLE);
    virtual ~FindFileDlg();

    bool findOnly(void)
    {
        return (m_bReplace == false);
    }

    void updateWhere(bool bSearchInSel);

    void updateLabel(const wxString &strShort, const wxString &strLong);
    void updateFindList(const wxString &strFind);
    void updateReplaceList(const wxString &strReplace);
    void updateItems(const wxString &strFind, const wxString &strReplace, bool bSearchInSel = false);
    void updateSel(void);

    void DoFindReplace(bool bReplace = false, bool bAll = false);

    void OnFindPrev(wxCommandEvent &tEvent);
    void OnFindNext(wxCommandEvent &tEvent);
    void OnFindFindAll(wxCommandEvent &tEvent);
    void OnFindReplace(wxCommandEvent &tEvent);
    void OnFindReplaceAll(wxCommandEvent &tEvent);
    void OnWhatChange(wxCommandEvent &tEvent);
    void OnWhereChange(wxCommandEvent &tEvent);
    void OnRightClick(wxListEvent &tEvent);
    void OnListDoubleClick(wxListEvent &tEvent);
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

    void OnActivate(wxActivateEvent &tEvent);

    void OnMarker(wxCommandEvent &tEvent);
    void OnCyclic(wxCommandEvent &tEvent);

    void OnFindClose(wxCommandEvent &tEvent);
    void OnCloseWindow(wxCloseEvent &tEvent);

private:
    DECLARE_EVENT_TABLE()

    wxString m_strFind;
    int m_iCurLine;

    int m_iFindPos;

    bool m_bReplace;

    FindReplaceComboBox *m_cboFind;
    FindReplaceComboBox *m_cboReplace;

    wxRadioBox *m_optWhere;

    wxCheckBox *m_optCase;
    wxCheckBox *m_optWord;
    wxCheckBox *m_optRegExp;
    wxCheckBox *m_optCyclic;

    wxCheckBox *m_optMarker;

    wxStaticText *m_txtStatus;

    wxButton *m_btnFindPR;
    wxButton *m_btnFindNext;
    wxButton *m_btnFindAll;
    wxButton *m_btnClose;

    wxPoint m_ptInit;
    void initInterface(void);
    static bool ShowToolTips();
};

#endif
