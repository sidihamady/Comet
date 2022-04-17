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


#ifndef OUTPUT_EDIT_H
#define OUTPUT_EDIT_H

#include "CodeEdit.h"

#include <wx/process.h>

typedef struct _errline_t
{
    int line;
    long id;
} errline_t;

class OutputEdit : public CodeEdit
{
private:
    DECLARE_EVENT_TABLE();

private:
    bool m_bInitialized;

    int m_iMaxLineWidth;

    int m_iRunningLexer;
    int m_iErrPos;
    errline_t m_ErrLine;
    wxString m_strErrInfo;
    size_t m_iTotalLength;

    // specific LaTeX stuff
    int m_latexWarn;
    int m_latexOverfull;
    double m_latexOverfullMax;
    int m_latexUnderfull;
    wxString m_latexFilename;
    wxString m_latexFullFilename;
    //

public:
    OutputEdit(wxWindow *pParent, wxWindowID id = wxID_ANY,
               const wxPoint &pos = wxDefaultPosition,
               const wxSize &size = wxDefaultSize,
               long styleT = wxNO_BORDER | wxVSCROLL | wxHSCROLL,
               const wxString &strName = wxSTCNameStr);
    virtual ~OutputEdit();

    bool setPrefs(const ScintillaPrefs &tPrefs, bool bInit = true);
    bool reinitPrefs(void);

    void Output(const wxString &strT, bool bForce = false, bool bToErrPos = false);
    bool isLineEmpty(void);

    void setRunningLexer(int iRunningLexer)
    {
        m_iRunningLexer = iRunningLexer;
        m_iErrPos = -1;
        m_ErrLine.line = -1;
        m_ErrLine.id = 0L;
        m_strErrInfo.Empty();

        // specific LaTeX stuff
        m_latexWarn = 0;
        m_latexOverfull = 0;
        m_latexOverfullMax = 0.0;
        m_latexUnderfull = 0;
        m_latexFilename.Empty();
        m_latexFullFilename.Empty();
        //

        reinitPrefs();
    }
    void gotoErrPos(void)
    {
        if (m_iErrPos > 0) {
            if (this->isFocused()) {
                this->SetSelection(m_iErrPos, m_iErrPos);
                this->EnsureCaretVisible();
            }
            else {
                this->ScrollToLine(this->LineFromPosition(m_iErrPos));
            }
            m_iErrPos = -1;
        }
    }
    int getErrLine(void)
    {
        return m_ErrLine.line;
    }
    long getErrId(void)
    {
        return m_ErrLine.id;
    }
    void setErrId(long iId)
    {
        m_ErrLine.id = iId;
    }
    wxString getErrInfo(void)
    {
        return m_strErrInfo;
    }
    wxString getLaTeXFilename(void)
    {
        return m_latexFilename;
    }
    wxString getLaTeXFullFilename(void)
    {
        return m_latexFullFilename;
    }
    void printLaTeXReport(int iExitStatus, wxString strFilename)
    {
        wxString strT = wxString::Format(
            uT("\n")
            uT("================ LaTeX report by Comet ================\n")
#ifdef __WXMSW__
            uT(" status code:   %d (%s) \n")
#else
            uT(" status code:   %d (%ls) \n")
#endif
            uT("       error:   %d \n")
            uT("     warning:   %d \n")
            uT("    overfull:   %d (max: %.2fpt) \n")
            uT("   underfull:   %d \n")
#ifdef __WXMSW__
            uT("   last file:   %s \n"),
#else
            uT("   last file:   %ls \n"),
#endif
           (m_ErrLine.line >= 0) ? 1 : iExitStatus, (iExitStatus == 0) ? ((m_ErrLine.line >= 0) ? uT("ERROR") : uT("OK")) : uT("ERROR"),
           (m_ErrLine.line >= 0) ? 1 : 0,
            m_latexWarn,
            m_latexOverfull, m_latexOverfullMax,
            m_latexUnderfull,
            LM_CSTR(m_latexFilename));

        if (m_ErrLine.line >= 0) {
            strT += wxString::Format(
#ifdef __WXMSW__
            uT("  error line:   %s %s\n")
            uT("          in:   %s \n"),
#else
            uT("  error line:   %ls %ls \n")
            uT("          in:   %ls \n"),
#endif
                (m_ErrLine.line >= 0) ? LM_CSTR(wxString::Format(uT("%d"), m_ErrLine.line + 1)) : uT("None"),
                ((m_ErrLine.line >= 0) && (m_strErrInfo.IsEmpty() == false)) ? LM_CSTR(m_strErrInfo) : uT(" "),
                (strFilename.IsEmpty() == false) ? LM_CSTR(strFilename) : ((m_ErrLine.line >= 0) ? uT("?") : uT("None")));
        }

        strT += uT("=======================================================\n");

        Output(strT);
    }

    void OnEditAction(wxCommandEvent &tEvent);
    void OnOutputAction(wxCommandEvent &tEvent);

    void OnUpdated(wxCommandEvent &tEvent);

    void OnMouseWheel(wxMouseEvent &tEvent);

    void DoEditCopy(void);
    void DoEditReset(void);

    void DoZoomIn(void);
    void DoZoomOut(void);
    void DoZoomNone(void);

    void Reset(void);

    void DoFindReset(void)
    {
        // nothing
    }

    void OnContextMenu(wxContextMenuEvent &tEvent);
};

#endif
