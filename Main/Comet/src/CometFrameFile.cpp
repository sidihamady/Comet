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

#include <wx/clipbrd.h>

const int CometFrame::TOOL_LEX[] = {
    wxSTC_LEX_CPP,
    wxSTC_LEX_PYTHON,
    wxSTC_LEX_FORTRAN,
    wxSTC_LEX_LATEX,
    wxSTC_LEX_MAKEFILE,
    wxSTC_LEX_SOLIS,
    wxSTC_LEX_COMET,
    -1
};
const char_t* CometFrame::TOOL_EXT[] = {
    uT("C/C++ (*.c;*.cpp;*.cxx;*.cc)"),
    uT("Python (*.py)"),
    uT("Fortran (*.f;*.for)"),
    uT("LaTeX (*.tex;*.bib;*.cls;*.sty)"),
    uT("Makefile"),
    uT("Solis (*.solis)"),
    uT("User-defined"),
    uT("-1")
};
bool CometFrame::isToolSet(int iLexer)
{
    for (int ii = 0;; ii++) {
        if (-1 == CometFrame::TOOL_LEX[ii]) {
            return false;
        }
        if (CometFrame::TOOL_LEX[ii] == iLexer) {
            return true;
        }
    }
    return false;
}
SigmaToolPrefs CometFrame::TOOL_CONF;
const wxString CometFrame::getToolCmd(int iLexer, const wxString &strExt, const wxString &strShortFilename, bool *pbOut, bool *pbSilent, bool *pbCls)
{
    if (iLexer == wxSTC_LEX_COMET) {
        *pbOut = CometFrame::TOOL_CONF.bOut[TOOLS_MAXCOUNT - 1];
        *pbSilent = CometFrame::TOOL_CONF.bSilent;
        *pbCls = CometFrame::TOOL_CONF.bCls[TOOLS_MAXCOUNT - 1];
        return CometFrame::TOOL_CONF.szCmd[TOOLS_MAXCOUNT - 1];
    }

    for (int ii = 0;; ii++) {
        if (-1 == CometFrame::TOOL_LEX[ii]) {
            return wxEmptyString;
        }
        if (iLexer == CometFrame::TOOL_LEX[ii]) {
            wxString strExtT = uT("*.");
            strExtT += strExt;
            if (Tstrstr(CometFrame::TOOL_EXT[ii], LM_CSTR(strExtT)) != NULL) {
                *pbOut = CometFrame::TOOL_CONF.bOut[ii];
                *pbSilent = CometFrame::TOOL_CONF.bSilent;
                *pbCls = CometFrame::TOOL_CONF.bCls[ii];
                return CometFrame::TOOL_CONF.szCmd[ii];
            }
            else if (strShortFilename.IsSameAs(CometFrame::TOOL_EXT[ii])) {
                *pbOut = CometFrame::TOOL_CONF.bOut[ii];
                *pbSilent = CometFrame::TOOL_CONF.bSilent;
                *pbCls = CometFrame::TOOL_CONF.bCls[ii];
                return CometFrame::TOOL_CONF.szCmd[ii];
            }
            *pbOut = false;
            return wxEmptyString;
        }
    }
    *pbOut = false;
    return wxEmptyString;
}
const bool CometFrame::getToolOut(int iLexer)
{
    for (int ii = 0;; ii++) {
        if (-1 == CometFrame::TOOL_LEX[ii]) {
            return false;
        }
        if (iLexer == CometFrame::TOOL_LEX[ii]) {
            return CometFrame::TOOL_CONF.bOut[ii];
        }
    }
    return false;
}
const bool CometFrame::getToolSilent(int iLexer)
{
    return CometFrame::TOOL_CONF.bSilent;
}

ScriptEdit *CometFrame::fileOpen(const wxString &filenameT /* = wxEmptyString*/, bool bRun /* = false*/,
                                 long iLine /* = -1L*/, const wxString &strFind /* = wxEmptyString*/, const wxString &strReplace /* = wxEmptyString*/,
                                 bool bOpenRecent /* = false*/)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    m_bOpeningFile = true;

    if (iPageCount >= COMET_MAXFILES) {
        SigmaMessageBox(uT("Maximum number of open documents reached"), uT("Comet"), wxOK | wxICON_INFORMATION, this);
        m_bOpeningFile = false;
        return NULL;
    }

    wxString strFilename = filenameT;

    if (filenameT == wxEmptyString) {

        int filterIndex = FILE_FILTER_LUA;

        wxString strDir = wxEmptyString;
        int iSel = m_pNotebookMain->GetSelection();
        if ((iSel >= 0) && (iSel < (int)(m_pNotebookMain->GetPageCount()))) {
            ScriptEdit *pEdit = getEditor(iSel);
            wxFileName fname = pEdit->GetFilename();
            strDir = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
            if (pEdit->GetFilename().IsEmpty() == false) {
                int iLexer = pEdit->GetLexer();
                switch (iLexer) {
                    case wxSTC_LEX_LUA:
                        filterIndex = FILE_FILTER_LUA;
                        break;
                    case wxSTC_LEX_CPP:
                        filterIndex = FILE_FILTER_CPP;
                        break;
                    case wxSTC_LEX_BASH:
                        filterIndex = FILE_FILTER_BASH;
                        break;
                    case wxSTC_LEX_PYTHON:
                        filterIndex = FILE_FILTER_PYTHON;
                        break;
                    case wxSTC_LEX_OCTAVE:
                        filterIndex = FILE_FILTER_OCTAVE;
                        break;
                    case wxSTC_LEX_FORTRAN:
                        filterIndex = FILE_FILTER_FORTRAN;
                        break;
                    case wxSTC_LEX_LATEX:
                        filterIndex = FILE_FILTER_LATEX;
                        break;
                    case wxSTC_LEX_MAKEFILE:
                        filterIndex = FILE_FILTER_MAKEFILE;
                        break;
                    case wxSTC_LEX_BATCH:
                        filterIndex = FILE_FILTER_BATCH;
                        break;
                    case wxSTC_LEX_SOLIS:
                        filterIndex = (LEXERTYPE_FIRST == pEdit->getLexerType()) ? FILE_FILTER_SOLIS : FILE_FILTER_SETTINGS;
                        break;
                    case wxSTC_LEX_COMET:
                        filterIndex = FILE_FILTER_COMET;
                        break;
                    default:
                        filterIndex = FILE_FILTER_KNOWN;
                        break;
                }
            }
        }

        char_t szFilters[LM_STRSIZEW];
        Tmemset(szFilters, 0, LM_STRSIZEW);
        Tstrcpy(szFilters,
            uT("Lua files|*.lua;*.comet|")
            uT("C/C++ files|*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.rc|")
            uT("Text/Shell files|*.txt;*.dat;*.csv;*.log;*.conf;*.cometo;*.bat;*.sh;*.csh;*.ins;*.bst;*.patch|")
            uT("Python files|*.py;*.pyw;*.sage;*.sagews|")
            uT("Octave files|*.m;*.mat|")
            uT("Fortran files|*.f;*.for;*.f90;*.f95;*.f03;*.f08;*.f15|")
            uT("LaTeX files|*.tex;*.bib;*.cls;*.sty|")
            uT("Makefile|Makefile;makefile|"));
        Tstrcat(szFilters,
            uT("Batch files|*.bat|")
            uT("Solis files|*.solis|")
            uT("Comet settings files|*.cometr;*.cometu|"));
        Tstrcat(szFilters,
                uT("User-defined files|*."));
        Tstrcat(szFilters,
                CodeEdit::getCometLexerExt());
        Tstrcat(szFilters,
                uT("|"));
        Tstrcat(szFilters,
            uT("All supported files|*.lua;*.comet;*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.rc;")
            uT("*.txt;*.dat;*.csv;*.conf;*.cometr;*.cometu;*.cometo;*.bat;*.sh;*.csh;*.ins;*.bst;")
            uT("*.bib;*.patch;*.py;*.pyw;*.m;*.mat;*.f;*.for;*.f90;*.f95;*.f03;*.f08;*.f15;*.tex;*.bib;*.cls;*.sty;Makefile;makefile;*.solis;*.cometr;*.cometu;*."));
        Tstrcat(szFilters,
                CodeEdit::getCometLexerExt());
        Tstrcat(szFilters,
                uT("|All files|*"));

        wxFileDialog *pDlg = new (std::nothrow) wxFileDialog(this, uT("Open file"), strDir, wxEmptyString,
                                                             static_cast<const char_t *>(szFilters),
                                                             wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
        if (pDlg == NULL) {
            m_bOpeningFile = false;
            return NULL;
        }
        pDlg->SetFilterIndex(filterIndex);
        if (pDlg->ShowModal() != wxID_OK) {
            pDlg->Destroy();
            m_bOpeningFile = false;
            return NULL;
        }
        strFilename = pDlg->GetPath();
        pDlg->Destroy();
    }

    if ((strFilename == wxEmptyString) || (::wxFileExists(strFilename) == false)) {
        m_bOpeningFile = false;
        return NULL;
    }

    wxFileName fname = strFilename;
    wxString strExtRaw = fname.GetExt();
    if (CometApp::isExecutable(strFilename, strExtRaw)) {
        OutputStatusbar(uT("Cannot load file: executable type"), SIGMAFRAME_TIMER_SHORT);
        m_bOpeningFile = false;
        return NULL;
    }

    long iFileSize = lf_getFileSize(LM_CSTR(strFilename));

    if (iFileSize < 0L) { // Should never happen
        m_bOpeningFile = false;
        return NULL;
    }
    if (iFileSize > LF_SCRIPT_MAXCHARS) {
        OutputStatusbar(uT("Cannot load file: invalid size (max: 256 MB)"), SIGMAFRAME_TIMER_SHORT);
        m_bOpeningFile = false;
        return NULL;
    }

    if (bOpenRecent) {
        if (iFileSize > LF_SCRIPT_MAXCHARS_SM) {
            m_bOpeningFile = false;
            return NULL;
        }
    }

    ScriptEdit *pEdit = NULL;

    if (iPageCount > 0) {

        wxFileName fnameT;

        for (int ii = 0; ii < iPageCount; ii++) {

            pEdit = getEditor(ii);
            if (pEdit == NULL) {
                break;
            }

            // Already open?
            fnameT = pEdit->GetFilename();
            if (fnameT.SameAs(fname)) {
                setActiveEditor(ii);
                if (false == pEdit->autoReload()) {
                    pEdit->DoReload(false, false);
                }
                if (iLine >= 0L) {
                    pEdit->setFindMarker(iLine, strFind, strReplace);
                }
                m_bOpeningFile = false;
                return pEdit;
            }

            // Empty unmodified document?
            if ((pEdit->GetLength() < 1) && (pEdit->isModified() == false)) {
                pEdit->SetLexer(wxSTC_LEX_NULL);
                if (pEdit->DoLoadFile(strFilename, bRun, false, false, true) == true) {
                    updateTitle(pEdit->GetFilename());
                    m_pNotebookMain->SetPageToolTip(ii, strFilename);
                    if (iLine >= 0L) {
                        pEdit->setFindMarker(iLine, strFind, strReplace);
                    }
                    addToRecent(strFilename);
                    setActiveEditor(ii);
                }
                updateCodeSample();
                m_bOpeningFile = false;
                return pEdit;
            }
        }
    }

    wxString strTitle = fname.GetFullName();

    int iSelP = m_pNotebookMain->GetSelection();

    pEdit = getEditor(iSelP);
    wxSize sizeT = pEdit ? (pEdit->GetSize()) : wxSize(300, 200);
    wxPoint posT = pEdit ? (pEdit->GetPosition()) : wxDefaultPosition;

    pEdit = new (std::nothrow) ScriptEdit(m_pNotebookMain, ID_WINDOW_SCRIPT, posT, sizeT, wxNO_BORDER | wxVSCROLL | wxHSCROLL, wxEmptyString, wxSTC_LEX_NULL);
    if (pEdit == NULL) {
        m_bOpeningFile = false;
        return NULL;
    }
    int iIdx = 0;
    addEditor(pEdit, pEdit->GetFilename(), &iIdx, false);
    pEdit->initDrop();

    pEdit->setPrefs(m_ScintillaPrefs, true, true);

    if (pEdit->DoLoadFile(strFilename, bRun, false, bOpenRecent, true) == true) {
        if (iLine >= 0L) {
            pEdit->setFindMarker(iLine, strFind, strReplace);
        }
        addToRecent(strFilename);
        setActiveEditor(iIdx, true, false);
    }
    else {
        pEdit->DoDeleteAllMarkers();
        if (pEdit->Destroy()) {
            pEdit = NULL;
        }
    }

    updateCodeSample();

    m_bOpeningFile = false;
    return pEdit;
}

bool CometFrame::dropFiles(const wxArrayString &filenames)
{
    int iFileCount = (int)(filenames.GetCount());
    if (iFileCount > 0) {
        int iCount = (iFileCount < CometFrame::MAX_DROPCOUNT) ? iFileCount : CometFrame::MAX_DROPCOUNT;
        int iDropped = 0;
        // always set focus to the main window before opening file to avoid side effects (flickering)
        this->SetFocus();
        for (int ii = 0; ii < iCount; ii++) {
            if (this->fileOpen(filenames[ii])) {
                iDropped += 1;
            }
        }
        if ((iFileCount > CometFrame::MAX_DROPCOUNT) && (iDropped == CometFrame::MAX_DROPCOUNT)) {
            this->OutputStatusbar(uT("Number of files dropped limited to 16"), SIGMAFRAME_TIMER_SHORT);
        }
    }

    return true;
}

bool CometFrame::fileSave(int iPage)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPage >= iPageCount) {
        return false;
    }

    if (iPage < 0) {
        iPage = m_pNotebookMain->GetSelection();
        if ((iPage < 0) || (iPage >= iPageCount)) {
            return false;
        }
    }

    ScriptEdit *pEdit = getEditor(iPage);
    if (pEdit == NULL) {
        return false;
    }

    int iSelStart, iSelEnd;
    pEdit->GetSelection(&iSelStart, &iSelEnd);
    wxString strFilename = pEdit->GetFilename();
    if (pEdit->DoSaveFile(strFilename) == false) {
        return false;
    }
    pEdit->SetFocus();
    pEdit->SetSelection(iSelStart, iSelEnd);

    updateCodeSample();

    return true;
}

bool CometFrame::addEditor(ScriptEdit *pEdit, const wxString &strFilename, int *piIndex /* = NULL*/, bool bFocus /* = true*/)
{
    if (m_pNotebookMain == NULL) {
        return false;
    }

    int iCount = (int)(m_pNotebookMain->GetPageCount());

    if ((iCount < 0) || (iCount >= COMET_MAXFILES)) {
        return false;
    }

    for (int ii = 0; ii < iCount; ii++) {
        if (getEditor(ii) == pEdit) {
            if (piIndex) {
                *piIndex = ii;
            }
            return true;
        }
    }

    wxFileName fname = strFilename;
    wxBitmap *pBmp = getBitmapNone();

    bool bRet = m_pNotebookMain->AddPage((wxWindow *)(pEdit), fname.GetFullName(), false, *pBmp, bFocus);
    if (bRet) {
        if (piIndex) {
            *piIndex = iCount;
        }
    }
    return bRet;
}

ScriptEdit *CometFrame::getEditorByShortFilename(const wxString &shortFilename)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if ((iPageCount < 1) || (iPageCount >= COMET_MAXFILES)) {
        return NULL;
    }

    if (shortFilename == wxEmptyString) {
        return NULL;
    }

    ScriptEdit *pEdit = NULL;

    for (int ii = 0; ii < iPageCount; ii++) {
        pEdit = getEditor(ii);
        if (pEdit == NULL) {
            break;
        }
        if ((pEdit->GetFilename() == shortFilename) || (pEdit->GetFilename().EndsWith(shortFilename))) {
            return pEdit;
        }
    }

    return NULL;
}

bool CometFrame::selectEditor(ScriptEdit *pEdit)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if ((iPageCount < 1) || (iPageCount >= COMET_MAXFILES)) {
        return false;
    }

    ScriptEdit *pEditX = NULL;

    for (int ii = 0; ii < iPageCount; ii++) {
        pEditX = getEditor(ii);
        if (pEditX == NULL) {
            break;
        }
        if (pEditX == pEdit) {
            setActiveEditor(ii, true);
            return true;
        }
    }

    return false;
}

bool CometFrame::isFileOpened(const wxString &filenameT)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if ((iPageCount < 1) || (iPageCount >= COMET_MAXFILES)) {
        return false;
    }

    wxString strFilename = filenameT;

    if (filenameT == wxEmptyString) {
        return false;
    }

    ScriptEdit *pEdit = NULL;

    // Already open ?
    for (int ii = 0; ii < iPageCount; ii++) {
        pEdit = getEditor(ii);
        if (pEdit == NULL) {
            break;
        }
        if (pEdit->GetFilename() == filenameT) {
            return true;
        }
    }

    return false;
}

bool CometFrame::fileSaveAs(void)
{
    int iSel = 0;

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        return false;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        return false;
    }

    pEdit->DoSaveFile(wxEmptyString);

    updateCodeSample();

    return true;
}

bool CometFrame::filePrint(void)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = 0;

    iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        return false;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        return false;
    }

    pEdit->DoPrint();

    return true;
}

bool CometFrame::filePrintPreview(void)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = 0;

    iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        return false;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        return false;
    }

    pEdit->ShowPrintPreviewDialog();

    return true;
}

int CometFrame::fileToClose(int iSel)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());
    if ((iSel < 0) || (iSel >= iPageCount)) {
        return -1;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        return -1;
    }

    if (pEdit->isRunning()) {
        pEdit->SetFocus();
        wxString strT = pEdit->isDebugging() ? uT("Cannot close window: the active script is in debugging session.") : uT("Cannot close window: the active script is running.");
        OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        SigmaMessageBox(strT, uT("Comet"), wxOK | wxICON_INFORMATION, this);
        return -1;
    }

    if (pEdit->isModified()) {
        pEdit->SetFocus();
        int idT = SigmaMessageBox(uT("Script not saved. Close it anyway?"), uT("Comet"),
                                  wxYES | wxNO | wxICON_QUESTION | wxNO_DEFAULT, this, uT("&Close"), uT("&Don't close"));
        if (idT != wxYES) {
            return -1;
        }
    }

    return iSel;
}

bool CometFrame::fileClose(ScriptEdit *pEdit)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount == 1) {
        Tstrcpy(m_szLastClosed, LM_CSTR(pEdit->GetFilename()));
        return (fileNew(wxEmptyString, wxSTC_LEX_LUA, 0) != NULL);
    }

    pEdit->DoDeleteAllMarkers();
    return true;
}

void CometFrame::OnScriptClose(wxAuiNotebookEvent &tEvent)
{
    const int iSel = fileToClose(tEvent.GetSelection());
    if (iSel < 0) {
        tEvent.Veto();
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount == 1) {
        if (iSel != 0) {
            // Should never happen
            return;
        }
        Tstrcpy(m_szLastClosed, LM_CSTR(pEdit->GetFilename()));
        pEdit->DoNewFile(m_ScintillaPrefs, wxEmptyString, wxSTC_LEX_LUA, 0);
        pEdit->DoHighlightSyntax(m_ScintillaPrefs.common.syntaxEnable);
        DoAnalyzerUpdateList(pEdit, true);
        updateCodeSample();
        UpdateDebugNotebook(wxEmptyString, wxEmptyString);

        tEvent.Veto();
        return;
    }

    wxString strFilename = pEdit->GetFilename();

    if (fileClose(pEdit) == false) {
        tEvent.Veto();
        return;
    }

    Tstrcpy(m_szLastClosed, LM_CSTR(strFilename));

    // destroy
    tEvent.Skip();

    int iPrevSel = m_pNotebookMain->GetPrevSelected();
    if (iPrevSel >= iSel) {
        --iPrevSel;
        if (iPrevSel < 0) {
            iPrevSel = 0;
        }
    }
    setActiveEditor(iPrevSel, true);
}

void CometFrame::OnFileNew(wxCommandEvent &WXUNUSED(tEvent))
{
    fileNew(wxEmptyString, wxSTC_LEX_LUA, 0);
}

void CometFrame::OnFileTemplate(wxCommandEvent &tEvent)
{
    int iLexer = wxSTC_LEX_NULL, iLexerVar = 0;
    switch (tEvent.GetId()) {
        case ID_FILE_TEMPLATE01:
            iLexer = wxSTC_LEX_LUA;
            break;
        case ID_FILE_TEMPLATE02:
            iLexer = wxSTC_LEX_CPP;
            iLexerVar = ID_TEMPLATE_C;
            break;
        case ID_FILE_TEMPLATE02MOD:
            iLexer = wxSTC_LEX_CPP;
            iLexerVar = ID_TEMPLATE_CMOD;
            break;
        case ID_FILE_TEMPLATE03:
            iLexer = wxSTC_LEX_CPP;
            iLexerVar = ID_TEMPLATE_CPP;
            break;
        case ID_FILE_TEMPLATE04:
            iLexer = wxSTC_LEX_PYTHON;
            break;
        case ID_FILE_TEMPLATE05:
            iLexer = wxSTC_LEX_FORTRAN;
            break;
        case ID_FILE_TEMPLATE06:
            iLexer = wxSTC_LEX_MAKEFILE;
            break;
        case ID_FILE_TEMPLATE07:
            iLexer = wxSTC_LEX_LATEX;
            break;
        case ID_FILE_TEMPLATE08:
            iLexer = wxSTC_LEX_BATCH;
            break;
        default:
            return;
    }
    wxString strTemplate = CodeSample::getTemplate(iLexer, iLexerVar);
    fileNew(strTemplate, iLexer, iLexerVar);
}

void CometFrame::OnFileDuplicate(wxCommandEvent &tEvent)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount >= COMET_MAXFILES) {
        SigmaMessageBox(uT("Maximum number of open documents reached"), uT("Comet"), wxOK | wxICON_INFORMATION, this);
        tEvent.Skip();
        return;
    }

    int iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        tEvent.Skip();
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        tEvent.Skip();
        return;
    }

    int iLexer = pEdit->GetLexer(), iLexerVar = 0;
    if ((iLexer == wxSTC_LEX_CPP) && (pEdit->GetFilename().IsEmpty() == false)) {
        if (pEdit->GetFilename().EndsWith(uT(".c")) == false) {
            iLexerVar = 1;
        }
    }

    wxBusyCursor waitC;

    wxString strContent = pEdit->GetText();
    pEdit = fileNew(strContent, iLexer, iLexerVar);

    if (pEdit == NULL) {
        tEvent.Skip();
        return;
    }

    pEdit->resetModified();
}

void CometFrame::OnFileOpen(wxCommandEvent &WXUNUSED(tEvent))
{
    fileOpen();
}

void CometFrame::OnFileSave(wxCommandEvent &WXUNUSED(tEvent))
{
    fileSave();
}

void CometFrame::OnFileSaveAs(wxCommandEvent &WXUNUSED(tEvent))
{
    fileSaveAs();
}

void CometFrame::OnFileReload(wxCommandEvent &WXUNUSED(tEvent))
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        return;
    }

    m_SigmaCommonPrefs.bDetectChangeOutside = true;
    bool bAutoReload = pEdit->autoReload();
    pEdit->resetTimerReload();
    pEdit->DoReload(false, true);
    if (bAutoReload) {
        updateEditorStatus(pEdit);
    }
}

ScriptEdit *CometFrame::DoFileNew(wxPoint posT, wxSize sizeT, const wxString &strTemplate, int iLexer, int iLexerVar)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount >= COMET_MAXFILES) {
        SigmaMessageBox(uT("Maximum number of open documents reached"), uT("Comet"), wxOK | wxICON_INFORMATION, this);
        return NULL;
    }

    ScriptEdit *pEdit = new (std::nothrow) ScriptEdit(m_pNotebookMain, ID_WINDOW_SCRIPT, posT, sizeT, wxNO_BORDER | wxVSCROLL | wxHSCROLL, wxEmptyString);
    if (pEdit == NULL) {
        return NULL;
    }
    // call setPrefs before addEditor to avoid flickering
    pEdit->setPrefs(m_ScintillaPrefs, true, true);
    addEditor(pEdit, pEdit->GetFilename());
    pEdit->DoNewFile(m_ScintillaPrefs, strTemplate, iLexer, iLexerVar);
    pEdit->initDrop();
    DoAnalyzerUpdateList(pEdit, true);
    return pEdit;
}

ScriptEdit *CometFrame::fileNew(const wxString &strTemplate, int iLexer, int iLexerVar)
{
    ScriptEdit *pEdit = getEditor(0);
    wxSize sizeT = pEdit ? (pEdit->GetSize()) : wxSize(300, 200);
    wxPoint posT = pEdit ? (pEdit->GetPosition()) : wxDefaultPosition;
    ScriptEdit *pEditNew = DoFileNew(posT, sizeT, strTemplate, iLexer, iLexerVar);
    return pEditNew;
}

void CometFrame::setActiveEditor(int iSel, bool bFocus /* = true*/, bool bMakeVisible /* = true*/)
{
    if (m_pNotebookMain == NULL) {
        return;
    }

    int iCount = (int)(m_pNotebookMain->GetPageCount());

    if ((iSel < 0) || (iSel >= iCount)) {
        return;
    }

    m_pNotebookMain->SetSelection(iSel, bFocus);
    if (bMakeVisible) {
        m_pNotebookMain->MakeSelectionVisible();
    }
    DoScriptChange(iSel);
}

void CometFrame::explorerChdir(void)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount < 1) {
        return;
    }

    int iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        return;
    }

    wxFileName fname = pEdit->GetFilename();

    if (fname.FileExists()) {
        wxString strT = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
        if (strT.Length() > 2) {
            // ::ANSI::    NOT::UNICODE::
            char szT[LM_STRSIZE];
            memset(szT, 0, LM_STRSIZE * sizeof(char));
            strcpy(szT, LM_U8STR(strT));
            chdir(szT);
            //
        }
    }
}

bool CometFrame::explorerIsOpened(const wxString &strOldPath, bool bActivate)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount < 1) {
        return false;
    }

    wxString strFilename;

    ScriptEdit *pEdit = NULL;

    bool bDir = ::wxDirExists(strOldPath);

    for (int ii = 0; ii < iPageCount; ii++) {

        pEdit = getEditor(ii);
        if (pEdit == NULL) {
            continue;
        }

        // File 'strFilename' may not exist since rename could be performed previously
        strFilename = pEdit->GetFilename();

        if (((bDir == false) && strFilename.IsSameAs(strOldPath)) || ((bDir == true) && strFilename.StartsWith(strOldPath))) {
            if (bActivate) {
                setActiveEditor(ii, true);
            }
            return true;
        }
    }

    return false;
}

bool CometFrame::explorerOnRename(bool bDir, const wxString &strOldPath, const wxString &strNewPath)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount < 1) {
        return true;
    }

    wxString strFilename;

    ScriptEdit *pEdit = NULL;

    bool bRenameFile = (bDir == false), bRenameDir = (bDir == true);

    size_t iF, iLen;
    bool bret = true;

    for (int ii = 0; ii < iPageCount; ii++) {

        pEdit = getEditor(ii);
        if (pEdit == NULL) {
            continue;
        }

        // File 'strFilename' may not exist since rename could be performed previously
        strFilename = pEdit->GetFilename();

        bRenameFile = (bDir == false) && strFilename.IsSameAs(strOldPath);
        bRenameDir = (bDir == true) && strFilename.StartsWith(strOldPath);

        if (bRenameFile) {

            if (lf_fileReadonly(LM_CSTR(strFilename))) {
                lf_fileReadWrite(LM_CSTR(strFilename));
            }

            return pEdit->updateFilename(strNewPath);
        }

        else if (bRenameDir) {

            iF = strOldPath.Length();
            iLen = strFilename.Length();

            if ((iF > 1) && (iLen > iF)) {
                if (lf_fileReadonly(LM_CSTR(strFilename))) {
                    lf_fileReadWrite(LM_CSTR(strFilename));
                }
                strFilename.Replace(LM_CSTR(strOldPath), LM_CSTR(strNewPath));
                bret = pEdit->updateFilename(strFilename);
            }
        }
    }

    return bret;
}

bool CometFrame::explorerOnDelete(const wxString &strPath)
{
    if (::wxFileExists(strPath) == false) {
        return false;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount < 1) {
        return true;
    }

    wxString strFilename, strT;

    ScriptEdit *pEdit = NULL;

    for (int ii = 0; ii < iPageCount; ii++) {

        pEdit = getEditor(ii);
        if (pEdit == NULL) {
            continue;
        }
        strFilename = pEdit->GetFilename();
        if (::wxFileExists(strFilename) == false) {
            continue;
        }

        if (strFilename == strPath) {

            setActiveEditor(ii, true);

            if (lf_fileReadonly(LM_CSTR(strFilename))) {

                strT = uT("Cannot delete '");
                strT += CometFrame::getLabelFromPath(LM_CSTR(strFilename));
                strT += uT("': read-only");

                OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
                SigmaMessageBox(strT, uT("Comet"), wxOK | wxICON_WARNING, this);
                return false;
            }

            strT = uT("Do you want to delete '");
            strT += CometFrame::getLabelFromPath(LM_CSTR(strFilename));
            strT += uT("'?");

            int iret = SigmaMessageBox(strT, uT("Comet"),
                                       wxYES | wxNO | wxICON_QUESTION | wxNO_DEFAULT, this,
                                       uT("&Delete"), uT("D&on't delete"));
            if (iret != wxYES) {
                return false;
            }

            if (fileClose(pEdit)) {
                Tstrcpy(m_szLastClosed, LM_CSTR(strFilename));
                m_pNotebookMain->DeletePage(ii);
                Tunlink(LM_CSTR(strFilename));
                return true;
            }

            return false;
        }
    }

    if (lf_fileReadonly(LM_CSTR(strPath))) {

        strT = uT("Cannot delete '");
        strT += CometFrame::getLabelFromPath(LM_CSTR(strPath));
        strT += uT("': read-only");

        SigmaMessageBox(strT, uT("Comet"), wxOK | wxICON_WARNING, this);
        return false;
    }

    strT = uT("Do you want to delete '");
    strT += CometFrame::getLabelFromPath(LM_CSTR(strPath));
    strT += uT("'?");

    int iret = SigmaMessageBox(strT, uT("Comet"),
                               wxYES | wxNO | wxICON_QUESTION | wxNO_DEFAULT, this,
                               uT("&Delete"), uT("D&on't delete"));
    if (iret != wxYES) {
        return false;
    }

    Tunlink(LM_CSTR(strPath));

    return true;
}

void CometFrame::updateTitle(const wxString &strFilename)
{
    wxString strT, filenameT = strFilename;
    if (strFilename.IsEmpty()) {
        ScriptEdit *pEdit = getActiveEditor();
        if (pEdit != NULL) {
            filenameT = pEdit->GetFilename();
        }
    }

    if (filenameT.IsEmpty() || (::wxFileExists(filenameT) == false)) {
        strT = uT("Comet");
    }
    else {
        strT = uT("Comet - ");
        strT += CometFrame::getLabelFromPath(LM_CSTR(filenameT), false);
    }

    if ((this->GetWindowStyle() & wxSTAY_ON_TOP) != 0) {
        strT += uT(" (Always On Top)");
    }

    this->SetTitle(strT);
}

void CometFrame::updateCodeSample(void)
{
    if (m_pNotebookMain == NULL) {
        m_pExample = NULL;
        m_pSnippet = NULL;
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount < 1) {
        m_pExample = NULL;
        m_pSnippet = NULL;
        return;
    }

    int iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        m_pExample = NULL;
        m_pSnippet = NULL;
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        m_pExample = NULL;
        m_pSnippet = NULL;
        return;
    }

    switch (pEdit->GetLexer()) {
        case wxSTC_LEX_LUA:
            m_pExample = m_pExampleLua;
            m_pSnippet = m_pSnippetLua;
            break;
        case wxSTC_LEX_CPP:
            m_pExample = NULL;
            m_pSnippet = m_pSnippetCPP;
            break;
        case wxSTC_LEX_PYTHON:
            m_pExample = NULL;
            m_pSnippet = m_pSnippetPython;
            break;
        case wxSTC_LEX_LATEX:
            m_pExample = NULL;
            m_pSnippet = m_pSnippetLaTeX;
            break;
        default:
            m_pExample = NULL;
            m_pSnippet = NULL;
            break;
    }
}

ScriptEdit *CometFrame::pageOpen(long iId, long iLine /* = -1L*/, bool bFocus /* = true*/)
{
    if (iId < 1L) {
        return NULL;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if ((iPageCount < 1) || (iPageCount >= COMET_MAXFILES)) {
        return NULL;
    }

    ScriptEdit *pEdit = NULL;
    int idx = 0;

    for (int ii = 0; ii < iPageCount; ii++) {
        pEdit = getEditor(ii);
        if (pEdit == NULL) {
            break;
        }
        if (iId == pEdit->getScriptId()) {
            idx = ii;
            if (iLine >= (long)(pEdit->GetLineCount())) {
                pEdit = NULL;
                idx = 0;
            }
            break;
        }
    }

    if (pEdit) {
        setActiveEditor(idx, bFocus);
        if (false == pEdit->autoReload()) {
            pEdit->DoReload(false, false);
        }
        if (iLine >= 0L) {
            pEdit->SetFocus();
            pEdit->GotoLine(iLine);
        }
    }

    return pEdit;
}

wxString CometFrame::explorerGetCurrentDir(void)
{
    if (m_pNotebookFilesys == NULL) {
        return wxEmptyString;
    }

    wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pFilesysPanel);
    if (paneT.IsShown() && m_pExplorerTree && (m_pExplorerTree->IsShown())) {
        return m_pExplorerTree->doGetCurrentDir();
    }

    return wxEmptyString;
}

void CometFrame::DoUpdateExplorer(void)
{
    if (m_pNotebookFilesys == NULL) {
        return;
    }

    wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pFilesysPanel);
    if (paneT.IsShown() && m_pExplorerTree && (m_pExplorerTree->IsShown())) {
        m_pExplorerTree->doRefreshDir();
    }
}

void CometFrame::OnFileUpdate(wxCommandEvent &WXUNUSED(tEvent))
{
    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit && pEdit->isFocused()) {
        m_SigmaCommonPrefs.bDetectChangeOutside = true;
        bool bAutoReload = pEdit->autoReload();
        pEdit->resetTimerReload();
        pEdit->DoReload(false, true);
        if (bAutoReload) {
            updateEditorStatus(pEdit);
        }
    }
    else {
        DoUpdateExplorer();
    }
}

void CometFrame::OnFileSaveAll(wxCommandEvent &WXUNUSED(tEvent))
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount < 1) {
        return;
    }

    int iSel = 0;

    iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        return;
    }

    int iSelStart, iSelEnd;
    ScriptEdit *pEditSel = getEditor(iSel);
    if (pEditSel == NULL) {
        return;
    }
    pEditSel->GetSelection(&iSelStart, &iSelEnd);

    ScriptEdit *pEdit = NULL;
    wxString strFilename;
    for (int ii = 0; ii < iPageCount; ii++) {
        pEdit = getEditor(ii);
        if (pEdit == NULL) {
            break;
        }
        strFilename = pEdit->GetFilename();
        // Save without selecting
        pEdit->DoSaveFile(strFilename, false);
    }

    pEditSel->SetFocus();
    pEditSel->SetSelection(iSelStart, iSelEnd);
}

void CometFrame::addToRecent(const wxString &strFilename)
{
    if (::wxFileExists(strFilename) == false) {
        return;
    }

    if (m_SigmaCommonPrefs.iFileRecentCount > 0) {
        wxString strTA;
        for (int ii = 0; ii < m_SigmaCommonPrefs.iFileRecentCount; ii++) {
            strTA = wxString(static_cast<const char_t *>(m_SigmaCommonPrefs.szFileRecent[ii]));
            if (strTA.IsSameAs(strFilename)) {
                return;
            }
        }
    }

    if (m_SigmaCommonPrefs.iFileRecentCount < RECENTFILES_MAXCOUNT) {
        wxStrcpy(m_SigmaCommonPrefs.szFileRecent[m_SigmaCommonPrefs.iFileRecentCount], LM_CSTR(strFilename));
        m_SigmaCommonPrefs.iFileRecentLastIndex = m_SigmaCommonPrefs.iFileRecentCount;
        m_SigmaCommonPrefs.iFileRecentCount += 1;
        return;
    }

    if (m_SigmaCommonPrefs.iFileRecentLastIndex >= (RECENTFILES_MAXCOUNT - 1)) {
        m_SigmaCommonPrefs.iFileRecentLastIndex = 0;
    }

    wxStrcpy(m_SigmaCommonPrefs.szFileRecent[m_SigmaCommonPrefs.iFileRecentLastIndex], LM_CSTR(strFilename));
    m_SigmaCommonPrefs.iFileRecentLastIndex += 1;

    return;
}

void CometFrame::addToRecentDir(const wxString &strDirname)
{
    if (::wxDirExists(strDirname) == false) {
        return;
    }

    if (m_SigmaCommonPrefs.iDirRecentCount > 0) {
        const wxString strSep = wxFILE_SEP_PATH;
        wxString strT = strDirname, strTA;
        if (strT.EndsWith(strSep) == false) {
            strT += strSep;
        }

        for (int ii = 0; ii < m_SigmaCommonPrefs.iDirRecentCount; ii++) {
            strTA = wxString(static_cast<const char_t *>(m_SigmaCommonPrefs.szDirRecent[ii]));
            if (strTA.EndsWith(strSep) == false) {
                strTA += strSep;
            }
            if (strTA.IsSameAs(strT)) {
                return;
            }
        }
    }

    if (m_SigmaCommonPrefs.iDirRecentCount < RECENTDIRS_MAXCOUNT) {
        wxStrcpy(m_SigmaCommonPrefs.szDirRecent[m_SigmaCommonPrefs.iDirRecentCount], LM_CSTR(strDirname));
        m_SigmaCommonPrefs.iDirRecentCount += 1;
        return;
    }

    return;
}

void CometFrame::reopenRecent(void)
{
    if (m_bShellStarted || (m_SigmaGUIPrefs.bReopenRecent == false) || (m_SigmaCommonPrefs.iFileOpenStatus == 0L) || (m_SigmaCommonPrefs.iFileRecentCount < 1)) {
        return;
    }

    wxString strFilename;
    unsigned long iMask = 1L;
    for (int ii = 0; ii < wxMin(m_SigmaCommonPrefs.iFileRecentCount, RECENTFILES_MAXCOUNT); ii++) {

        if ((iMask & m_SigmaCommonPrefs.iFileOpenStatus) == 0L) {
            iMask <<= 1;
            continue;
        }

        strFilename = static_cast<const char_t *>(m_SigmaCommonPrefs.szFileRecent[ii]);
        if (::wxFileExists(strFilename) == false) {
            iMask <<= 1;
            continue;
        }

        fileOpen(strFilename, false, -1L, wxEmptyString, wxEmptyString);
        iMask <<= 1;
    }

    updateBookmark();
}

void CometFrame::selectRecent(void)
{
    if (m_bShellStarted) {
        return;
    }

    // To make active page visible, firstly select page #1
    int iSel = 0;

    if ((m_SigmaCommonPrefs.iFileRecentSel > 0) && (m_SigmaCommonPrefs.iFileRecentSel < (int)m_pNotebookMain->GetPageCount())) {
        iSel = m_SigmaCommonPrefs.iFileRecentSel;
    }

    setActiveEditor(iSel, true);
}

void CometFrame::OnFileOpenRecent(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();

    if (idT == ID_FILE_REOPEN) {
        m_SigmaGUIPrefs.bReopenRecent = (m_SigmaGUIPrefs.bReopenRecent == false);
        return;
    }

    if (m_SigmaCommonPrefs.iFileRecentCount < 1) {
        return;
    }

    if (idT == ID_FILE_RECENTCLEAR) {
        int iret = SigmaMessageBox(uT("Clear the recent files list?"), uT("Comet"),
                                   wxYES | wxNO | wxICON_QUESTION | wxNO_DEFAULT, this,
                                   uT("&Clear"), uT("&Don't clear"));
        if (iret == wxYES) {
            m_SigmaCommonPrefs.iFileRecentCount = 0;
        }
        return;
    }

    int indexT = idT - ID_FILE_RECENT01;
    if ((indexT < 0) || (indexT >= RECENTFILES_MAXCOUNT)) {
        return;
    }

    if (::wxFileExists((const char_t *)(m_SigmaCommonPrefs.szFileRecent[indexT])) == false) {
        wxString strT = uT("File '");
        strT += CometFrame::getLabelFromPath((const char_t *)(m_SigmaCommonPrefs.szFileRecent[indexT]));
        strT += uT("' not found.\nRemove it from the recent files list?");
        int iret = SigmaMessageBox(strT, uT("Comet"),
                                   wxYES | wxNO | wxICON_QUESTION | wxNO_DEFAULT, this,
                                   uT("&Remove"), uT("&Keep"));
        if (iret == wxYES) {
            if (indexT >= (m_SigmaCommonPrefs.iFileRecentCount - 1)) {
                m_SigmaCommonPrefs.iFileRecentCount -= 1;
            }
            else {
                for (int ii = indexT; ii < (m_SigmaCommonPrefs.iFileRecentCount - 1); ii++) {
                    wxStrncpy(m_SigmaCommonPrefs.szFileRecent[ii], (const char_t *)(m_SigmaCommonPrefs.szFileRecent[ii + 1]), LM_STRSIZE - 1);
                }
                m_SigmaCommonPrefs.iFileRecentCount -= 1;
            }
        }
        return;
    }

    fileOpen(m_SigmaCommonPrefs.szFileRecent[indexT]);
}

void CometFrame::OnFileOpenRecentDir(wxCommandEvent &tEvent)
{
    if (NULL == m_pExplorerTree) {
        // should never happen
        return;
    }

    int idT = tEvent.GetId();

    if (m_SigmaCommonPrefs.iDirRecentCount < 1) {
        return;
    }

    if (idT == ID_DIR_RECENTCLEAR) {
        int iret = SigmaMessageBox(uT("Clear the recent directories list?"), uT("Comet"),
                                   wxYES | wxNO | wxICON_QUESTION | wxNO_DEFAULT, this,
                                   uT("&Clear"), uT("&Don't clear"));
        if (iret == wxYES) {
            m_SigmaCommonPrefs.iDirRecentCount = 0;
        }
        return;
    }

    int indexT = idT - ID_DIR_RECENT01;
    if ((indexT < 0) || (indexT >= RECENTDIRS_MAXCOUNT)) {
        return;
    }

    if (::wxDirExists((const char_t *)(m_SigmaCommonPrefs.szDirRecent[indexT])) == false) {
        wxString strT = uT("Directory '");
        strT += CometFrame::getLabelFromPath((const char_t *)(m_SigmaCommonPrefs.szDirRecent[indexT]));
        strT += uT("' not found.\nRemove it from the recent directories list?");
        int iret = SigmaMessageBox(strT, uT("Comet"),
                                   wxYES | wxNO | wxICON_QUESTION | wxNO_DEFAULT, this,
                                   uT("&Remove"), uT("&Keep"));
        if (iret == wxYES) {
            if (indexT >= (m_SigmaCommonPrefs.iDirRecentCount - 1)) {
                m_SigmaCommonPrefs.iDirRecentCount -= 1;
            }
            else {
                for (int ii = indexT; ii < (m_SigmaCommonPrefs.iDirRecentCount - 1); ii++) {
                    wxStrncpy(m_SigmaCommonPrefs.szDirRecent[ii], (const char_t *)(m_SigmaCommonPrefs.szDirRecent[ii + 1]), LM_STRSIZE - 1);
                }
                m_SigmaCommonPrefs.iDirRecentCount -= 1;
            }
        }
        return;
    }

    m_pExplorerTree->doSetDir(static_cast<const char_t *>(m_SigmaCommonPrefs.szDirRecent[idT - ID_DIR_RECENT01]));
}

void CometFrame::OnFilePrint(wxCommandEvent &WXUNUSED(tEvent))
{
    filePrint();
}

void CometFrame::OnFilePrintPreview(wxCommandEvent &WXUNUSED(tEvent))
{
    filePrintPreview();
}

void CometFrame::OnScriptCloseAll(wxCommandEvent &WXUNUSED(tEvent))
{
    DoScriptCloseAll(-1, false);
}

void CometFrame::OnScriptCloseOthers(wxCommandEvent &WXUNUSED(tEvent))
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount < 2) {
        return;
    }

    int iSel = 0;

    iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        return;
    }

    DoScriptCloseAll(iSel, false);
}

void CometFrame::OnScriptCloseAllButPinned(wxCommandEvent &WXUNUSED(tEvent))
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount < 2) {
        return;
    }

    DoScriptCloseAll(-1, true);
}

void CometFrame::OnScriptReopenLast(wxCommandEvent &WXUNUSED(tEvent))
{
    fileOpen(static_cast<const char_t *>(m_szLastClosed));
}

void CometFrame::OnScriptNewWindow(wxCommandEvent &WXUNUSED(tEvent))
{
    if (m_pNotebookMain->GetPageCount() < 1) {
        return;
    }

    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        return;
    }

    wxFileName fname = pEdit->GetFilename();
    wxString strT = uT("\"");
    strT += wxStandardPaths::Get().GetExecutablePath();
    strT += uT("\" \"");
    strT += fname.GetFullPath();
    strT += uT("\"");
    strT += uT(" --new");
    ::wxExecute(strT, wxEXEC_ASYNC, NULL);
}

void CometFrame::OnScriptCopyPath(wxCommandEvent &WXUNUSED(tEvent))
{
    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        return;
    }

    if (wxTheClipboard && wxTheClipboard->Open()) {
        wxTheClipboard->SetData(new wxTextDataObject(pEdit->GetFilename()));
        wxTheClipboard->Flush();
        wxTheClipboard->Close();
        return;
    }
}

void CometFrame::OnScriptOpenDir(wxCommandEvent &WXUNUSED(tEvent))
{
    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        return;
    }

    wxFileName fname = pEdit->GetFilename();

#ifdef __WXMSW__
    wxString strT = uT("explorer /select, ");
    strT += fname.GetFullPath();
    ::wxExecute(strT, wxEXEC_ASYNC, NULL);
#else

#endif
}

void CometFrame::OnScriptSetDir(wxCommandEvent &WXUNUSED(tEvent))
{
    DoViewFileExplorer(true);
    if (NULL == m_pExplorerTree) {
        // should never happen
        return;
    }

    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        return;
    }

    wxFileName fname = pEdit->GetFilename();

    wxString strT = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    m_pExplorerTree->doSetDir(strT);
}

void CometFrame::OnScriptOpenShell(wxCommandEvent &WXUNUSED(tEvent))
{
    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        return;
    }

    wxFileName fname = pEdit->GetFilename();

#ifdef __WXMSW__
    wxString strT = uT("cmd.exe /K \"cd /d ");
    strT += fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    strT += uT("\"");
    ::wxExecute(strT, wxEXEC_ASYNC, NULL);
#else

#endif
}

void CometFrame::OnScriptFileProps(wxCommandEvent &WXUNUSED(tEvent))
{
    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        return;
    }

    wxFileName fname = pEdit->GetFilename();

#ifdef __WXMSW__
    SHELLEXECUTEINFO infoT;
    memset(&infoT, 0, sizeof(SHELLEXECUTEINFO));
    infoT.cbSize = sizeof(SHELLEXECUTEINFO);
    infoT.lpVerb = uT("properties");
    infoT.lpFile = LM_CSTR(fname.GetFullPath());
    infoT.nShow = SW_SHOW;
    infoT.fMask = SEE_MASK_INVOKEIDLIST;
    ShellExecuteEx(&infoT);
#else

#endif
}

void CometFrame::OnScriptFileStats(wxCommandEvent &WXUNUSED(tEvent))
{
    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        return;
    }

    pEdit->DoShowStats();
}

void CometFrame::OnScriptFileReadOnly(wxCommandEvent &WXUNUSED(tEvent))
{
    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        return;
    }

    bool bFileReadOnly = lf_fileReadonly(LM_CSTR(pEdit->GetFilename()));
    bool bReadOnly = (pEdit->GetReadOnly() == false);

    if (bFileReadOnly && (bReadOnly == false)) {

        wxString strT = uT("'");
        strT += CometFrame::getLabelFromPath(LM_CSTR(pEdit->GetFilename()));
        strT += uT("' is read-only. Change to read-write?");

        int iret = SigmaMessageBox(strT, uT("Comet"), wxYES_NO | wxICON_QUESTION | wxNO_DEFAULT, this, uT("C&hange"), uT("&Cancel"));
        if (iret != wxYES) {
            return;
        }

        if (lf_fileReadWrite(LM_CSTR(pEdit->GetFilename())) == false) {
            wxString strT = uT("Cannot change file permission to read-write");
            OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
            SigmaMessageBox(strT, uT("Comet"), wxOK | wxICON_WARNING, this);
            return;
        }
    }

    pEdit->SetReadOnly(bReadOnly);

    updateEditorStatus(pEdit);
}

void CometFrame::OnScriptChange(wxAuiNotebookEvent &tEvent)
{
    tEvent.Skip();

    if (m_bClosingAll || m_bOpeningFile) {
        return;
    }

    DoScriptChange(tEvent.GetOldSelection());
}

void CometFrame::OnScriptContextMenu(wxAuiNotebookEvent &tEvent)
{
    tEvent.Skip();

    int iSel = tEvent.GetSelection();

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        return;
    }

    wxMenu popMenu;

    wxMenuItem *pItem = new wxMenuItem(&popMenu, ID_FILE_SAVE, uT("Save"), uT("Save"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(filesave_small_xpm);
#ifdef __WXMSW__
    pItem->SetDisabledBitmap(filesave_disabled_small_xpm);
#endif
#endif
    popMenu.Append(pItem);

    popMenu.Append(ID_FILE_SAVEAS, uT("Save As..."), uT("Save As"));

    pItem = new wxMenuItem(&popMenu, ID_FILE_SAVEALL, uT("Save All"), uT("Save all files"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(filesaveall_small_xpm);
#ifdef __WXMSW__
    pItem->SetDisabledBitmap(filesaveall_disabled_small_xpm);
#endif
#endif
    popMenu.Append(pItem);

    popMenu.AppendSeparator();

    popMenu.Append(ID_FILE_CLOSE, uT("Close"), uT("Close"));
    popMenu.Append(ID_FILE_CLOSEALL, uT("Close All"), uT("Close All"));
    popMenu.Append(ID_FILE_CLOSEOTHERS, uT("Close Others"), uT("Close Others"));
    popMenu.Append(ID_FILE_CLOSEALLBUTPINNED, uT("Close All But Pinned"), uT("Close All But Pinned"));

    popMenu.AppendSeparator();

    popMenu.Append(ID_FILE_REOPENLAST, uT("Reopen Last Closed File"), uT("Reopen Last Closed File"));

    popMenu.AppendSeparator();

    updateCurrentFileMenu(&popMenu, iSel);

    wxPoint ptMousePosition = GetMouseNotebookPos(m_pNotebookMain);
    ptMousePosition.y += m_pNotebookMain->GetTabCtrlHeight();

    PopupMenu(&popMenu, ptMousePosition);
}

void CometFrame::updateRecentFileMenu(wxMenu *pMenu, wxMenuItem *pMenuItem)
{
    if (pMenu == NULL) {
        return;
    }

    int ii;

    int iSize = (int)(pMenu->GetMenuItemCount());
    if (iSize > 0) {
        // :WARNING: separator not destroyed
        wxMenuItemList tItems = pMenu->GetMenuItems();
        for (wxMenuItemList::Node *pNode = tItems.GetFirst(); pNode != NULL; pNode = pNode->GetNext()) {
            wxMenuItem *pItem = pNode->GetData();
            if (pItem) {
                pMenu->Destroy(pItem);
            }
        }
    }

    if (m_SigmaCommonPrefs.iFileRecentCount < 1) {
        if (pMenuItem) {
            pMenuItem->Enable(false);
        }
        return;
    }

    int idT = ID_FILE_RECENT01;

    wxString strT = CometFrame::getLabelFromPath(m_SigmaCommonPrefs.szFileRecent[0]);

    wxMenuItem *pItem = new wxMenuItem(pMenu, idT, strT, strT);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(fileopen_small_xpm);
#ifdef __WXMSW__
    pItem->SetDisabledBitmap(fileopen_disabled_small_xpm);
#endif
#endif
    pMenu->Append(pItem);

    if (m_SigmaCommonPrefs.iFileRecentCount > 1) {
        idT += 1;
        for (ii = 1; ii < m_SigmaCommonPrefs.iFileRecentCount; ii++) {
            strT = CometFrame::getLabelFromPath(m_SigmaCommonPrefs.szFileRecent[ii]);
            pMenu->Append(idT, strT, strT);
            idT += 1;
        }
    }

    pMenu->AppendSeparator();
    pMenu->Append(ID_FILE_REOPEN, uT("Reopen at startup"), uT("Reopen recent files at startup"), wxITEM_CHECK)->Check(m_SigmaGUIPrefs.bReopenRecent);
    pMenu->AppendSeparator();
    pItem = new wxMenuItem(pMenu, ID_FILE_RECENTCLEAR, uT("Clear"), uT("Clear the recent files list"));
    pMenu->Append(pItem);
}

void CometFrame::updateRecentDirMenu(wxMenu *pMenu, wxMenuItem *pMenuItem)
{
    if (pMenu == NULL) {
        return;
    }

    int ii;

    int iSize = (int)(pMenu->GetMenuItemCount());
    if (iSize > 0) {
        // :WARNING: separator not destroyed
        wxMenuItemList tItems = pMenu->GetMenuItems();
        for (wxMenuItemList::Node *pNode = tItems.GetFirst(); pNode != NULL; pNode = pNode->GetNext()) {
            wxMenuItem *pItem = pNode->GetData();
            if (pItem) {
                pMenu->Destroy(pItem);
            }
        }
    }

    if (m_SigmaCommonPrefs.iDirRecentCount < 1) {
        if (pMenuItem) {
            pMenuItem->Enable(false);
        }
        return;
    }

    int idT = ID_DIR_RECENT01;

    wxMenuItem *pItem = NULL;

    wxString strT = CometFrame::getLabelFromPath(m_SigmaCommonPrefs.szDirRecent[0]);

    pItem = new wxMenuItem(pMenu, idT, strT, strT);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(folder_small_xpm);
#ifdef __WXMSW__
    pItem->SetDisabledBitmap(folder_disabled_small_xpm);
#endif
#endif
    pMenu->Append(pItem);

    if (m_SigmaCommonPrefs.iDirRecentCount > 1) {
        idT += 1;
        for (ii = 1; ii < m_SigmaCommonPrefs.iDirRecentCount; ii++) {
            strT = CometFrame::getLabelFromPath(m_SigmaCommonPrefs.szDirRecent[ii]);
            pMenu->Append(idT, strT, strT);
            idT += 1;
        }
    }

    pMenu->AppendSeparator();

    pItem = new wxMenuItem(pMenu, ID_DIR_RECENTCLEAR, uT("Clear"), uT("Clear the recent directories list"));
    pMenu->Append(pItem);
}

void CometFrame::updateOpenFileMenu(wxMenu *pMenu, wxMenuItem *pMenuItem, int *piTextWidth)
{
    if (pMenu == NULL) {
        return;
    }

    int iSize = (int)(pMenu->GetMenuItemCount());
    if (iSize > 0) {
        wxMenuItemList tItems = pMenu->GetMenuItems();
        for (wxMenuItemList::Node *pNode = tItems.GetFirst(); pNode != NULL; pNode = pNode->GetNext()) {
            wxMenuItem *pItem = pNode->GetData();
            if (pItem) {
                pMenu->Destroy(pItem);
            }
        }
    }

    int idT = ID_FILE_LIST01, ii;

    int iPageCount = (int)(m_pNotebookMain->GetPageCount());

    if (pMenuItem != NULL) {
        pMenuItem->Enable(iPageCount >= 2);
    }

    if (iPageCount < 2) {
        return;
    }

#ifdef __WXMSW__
    int iSel = m_pNotebookMain->GetSelection();
#endif
    wxMenuItem *pItem = NULL;
    ScriptEdit *pEdit = NULL;
    wxString strT, strTH;
    wxFileName fname;
    const int iMax = ID_FILE_LISTLAST - ID_FILE_LIST01 + 1;
    if (iPageCount > iMax) {
        iPageCount = iMax;
    }

    wxFont fontT;

    if (piTextWidth) {
        *piTextWidth = 0;
    }
    int iYc = 0;

    for (ii = 0; ii < iPageCount; ii++, idT += 1) {

        pEdit = getEditor(ii);
        if (pEdit == NULL) {
            continue;
        }

        bool bModified = pEdit->isModified();

        fname = pEdit->GetFilename();

        strT = fname.GetFullName();
        if (strT.IsEmpty()) {
            strT = m_pNotebookMain->GetPageText(ii);
            strTH = strT;
        }
        else {
            if (bModified) {
                strT += uT("*");
            }
            strTH = fname.GetFullPath();
        }

        pItem = new wxMenuItem(pMenu, idT, strT, strTH);
#ifdef __WXMSW__
        if (ii == 0) {
            fontT = pItem->GetFont();
        }
        if (ii == iSel) {
            fontT.SetWeight(wxFONTWEIGHT_BOLD);
            pItem->SetFont(fontT);
        }
        else {
            fontT.SetWeight(wxFONTWEIGHT_NORMAL);
            pItem->SetFont(fontT);
        }
        if (bModified) {
            pItem->SetTextColour(wxColour(180, 110, 5));
        }
#else
        if (ii == 0) {
            fontT = m_pNotebookMain->GetFont();
        }
#endif
        if (piTextWidth) {
            int iX = 0, iY = 0;
            m_pNotebookMain->GetTextExtent(strT, &iX, &iY, NULL, NULL, &fontT);
            if (iX > *piTextWidth) {
                *piTextWidth = iX;
            }
            if (iY > iYc) {
                iYc = iY;
            }
        }

#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
        if (pEdit->isFindin()) {
            pItem->SetBitmap(find_small_xpm);
        }
        else if (pEdit->GetReadOnly()) {
            pItem->SetBitmap(readonly_small_xpm);
        }
        else if (pEdit->isPinned()) {
            pItem->SetBitmap(pinned_small_xpm);
        }
        else if (pEdit->autoReload()) {
            pItem->SetBitmap(autoreload_small_xpm);
        }
#endif

        pMenu->Append(pItem);
    }

    if (piTextWidth) {
        *piTextWidth += (2 * iYc);
#ifdef __WXMSW__
        *piTextWidth += (iYc + 8);
#else
        *piTextWidth += 4;
#endif
    }
}

void CometFrame::updateCurrentFileMenu(wxMenu *pMenu, int iSel)
{
    if (pMenu == NULL) {
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount < 1) {
        return;
    }

    if ((iSel >= 0) && (iSel < iPageCount)) {
        setActiveEditor(iSel, true, false);
    }

    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        return;
    }

    wxString strN = m_pNotebookMain->GetPageText(iSel);

    wxMenuItem *pItem = NULL;

    if (pEdit->isExtKnown() == false) {
        wxMenu *menuFileLexer = new wxMenu();

        int idT = ID_FILE_LEXER01;
        wxString strT = CodeEdit::KNOWN_LEXER_DESCRIPTION[0];
        pItem = new wxMenuItem(menuFileLexer, idT, strT, strT);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
        pItem->SetBitmap(lua_small_xpm);
#endif
        menuFileLexer->Append(pItem);

        idT += 1;
        for (int ii = 1;; ii++) {
            if (-1 == CodeEdit::KNOWN_LEXER[ii]) {
                break;
            }
            menuFileLexer->Append(idT, CodeEdit::KNOWN_LEXER_DESCRIPTION[ii], CodeEdit::KNOWN_LEXER_DESCRIPTION[ii], wxITEM_CHECK);
            idT += 1;
        }

        pMenu->AppendSubMenu(menuFileLexer, uT("File Type"), uT("File Type"));
        pMenu->AppendSeparator();
    }

    pMenu->Append(ID_FILE_PIN, uT("Pin Document"), uT("Pin Document"));
    pMenu->Append(ID_FILE_RESETMARKERS, uT("Remove Markers"), uT("Remove Markers"));
    pMenu->Append(ID_FILE_DUPLICATE, uT("Duplicate Document"), uT("Duplicate Document"));
    pMenu->Append(ID_FILE_NEWWINDOW, uT("Open in New Window"), uT("Open in New Window"));
    pMenu->Append(ID_FILE_COPYPATH, uT("Copy Full Path"), uT("Copy Full Path"));

    pItem = new wxMenuItem(pMenu, ID_FILE_SETDIR, uT("Select Containing Directory"), uT("Select Containing Directory"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(folder_small_xpm);
#endif
    pMenu->Append(pItem);

#ifdef __WXMSW__
    pMenu->Append(ID_FILE_OPENDIR, uT("Open Containing Directory"), uT("Open Directory"));
    pMenu->AppendSeparator();
    pItem = new wxMenuItem(pMenu, ID_FILE_OPENSHELL, uT("Command Prompt Here"), uT("Open command prompt in this location"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(prompt_small_xpm);
#endif
    pMenu->Append(pItem);
    pMenu->Append(ID_FILE_FILEPROPS, uT("File Properties..."), uT("View File Properties"), wxITEM_NORMAL);
#else

#endif

    pMenu->Append(ID_FILE_FILESTATS, uT("File Statistics..."), uT("View File Statistics"), wxITEM_NORMAL);
    pMenu->Append(ID_FILE_READONLY, uT("Read Only"), uT("Set As Read Only"), wxITEM_CHECK);

    pMenu->AppendSeparator();
    pMenu->Append(ID_FILE_RELOAD, uT("Reload From Disk"), uT("Reload the active file from disk"), wxITEM_NORMAL);
}

void CometFrame::OnScriptCloseX(wxCommandEvent &tEvent)
{
    int iSel = fileToClose(m_pNotebookMain->GetSelection());
    if (iSel < 0) {
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    wxString strFilename = pEdit->GetFilename();

    if (fileClose(pEdit)) {
        iSel = m_pNotebookMain->GetPageIndex(pEdit);
        if ((iSel >= 0) && (iSel < (int)(m_pNotebookMain->GetPageCount()))) {
            Tstrcpy(m_szLastClosed, LM_CSTR(strFilename));
            m_pNotebookMain->DeletePage(iSel);
            int iPrevSel = m_pNotebookMain->GetPrevSelected();
            if (iPrevSel >= iSel) {
                --iPrevSel;
                if (iPrevSel < 0) {
                    iPrevSel = 0;
                }
            }
            setActiveEditor(iPrevSel, true);
        }
    }

    return;
}

void CometFrame::DoScriptCloseAll(int iExcept, bool bAllButPinned)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());
    if (iPageCount < 1) {
        return;
    }

    m_bClosingAll = true;

    ScriptEdit *pEdit = NULL;

    ScriptEdit *pEditToSelect = NULL;

    if (1 == iPageCount) {

        if (0 == iExcept) {
            m_bClosingAll = false;
            return;
        }

        pEdit = getEditor(0);
        if (NULL == pEdit) {
            // should never happen
            m_bClosingAll = false;
            return;
        }

        if (pEdit->isRunning()) {
            wxFileName fname = pEdit->GetFilename();
#ifdef __WXMSW__
            wxString strT = wxString::Format(pEdit->isDebugging() ? uT("Cannot close '%s': it is in debugging session.") : uT("Cannot close '%s': it is running."), LM_CSTR(fname.GetFullName()));
#else
            wxString strT = wxString::Format(pEdit->isDebugging() ? uT("Cannot close '%ls': it is in debugging session.") : uT("Cannot close '%ls': it is running."), LM_CSTR(fname.GetFullName()));
#endif
            OutputStatusbar(strT);
            m_bClosingAll = false;
            return;
        }

        if (pEdit->isModified()) {
            wxString strT = m_pNotebookMain->GetPageText(0);
            strT += uT(" not saved. Save it before closing?");
            int iret = SigmaMessageBox(strT, uT("Comet"),
                                       wxYES | wxNO | wxCANCEL | wxICON_QUESTION, this,
                                       uT("&Save"), uT("&Don't save"));
            if (iret == wxYES) {
                fileSave(0);
            }
            else if (iret == wxCANCEL) {
                m_bClosingAll = false;
                return;
            }
        }

        pEdit->DoNewFile(m_ScintillaPrefs, wxEmptyString, wxSTC_LEX_LUA, 0);
        pEdit->DoHighlightSyntax(m_ScintillaPrefs.common.syntaxEnable);
        DoAnalyzerUpdateList(pEdit, true);
        setActiveEditor(0);

        m_bClosingAll = false;
        return;
    }

    ScriptEdit *arEdit[COMET_MAXFILES];
    memset(arEdit, 0, COMET_MAXFILES * sizeof(ScriptEdit *));
    int ii, iC = 0, iNC = 0;

    for (ii = 0; ii < iPageCount; ii++) {

        pEdit = getEditor(ii);
        if (pEdit == NULL) {
            break;
        }

        if (ii == iExcept) {
            if (NULL == pEditToSelect) {
                pEditToSelect = pEdit;
            }
            arEdit[iC] = NULL;
            ++iC;
            ++iNC;
            continue;
        }

        if (bAllButPinned && pEdit->isPinned()) {
            if (NULL == pEditToSelect) {
                pEditToSelect = pEdit;
            }
            arEdit[iC] = NULL;
            ++iC;
            ++iNC;
            continue;
        }

        if (pEdit->isRunning()) {
            wxFileName fname = pEdit->GetFilename();
#ifdef __WXMSW__
            wxString strT = wxString::Format(pEdit->isDebugging() ? uT("Cannot close '%s': it is in debugging session.") : uT("Cannot close '%s': it is running."), LM_CSTR(fname.GetFullName()));
#else
            wxString strT = wxString::Format(pEdit->isDebugging() ? uT("Cannot close '%ls': it is in debugging session.") : uT("Cannot close '%ls': it is running."), LM_CSTR(fname.GetFullName()));
#endif
            OutputStatusbar(strT);
            if (NULL == pEditToSelect) {
                pEditToSelect = pEdit;
            }
            arEdit[iC] = NULL;
            ++iC;
            ++iNC;
            continue;
        }

        if (pEdit->isModified()) {
            wxString strT = m_pNotebookMain->GetPageText(ii);
            strT += uT(" not saved. Save it before closing?");
            int iret = SigmaMessageBox(strT, uT("Comet"),
                                       wxYES | wxNO | wxCANCEL | wxICON_QUESTION, this,
                                       uT("&Save"), uT("&Don't save"));
            if (iret == wxYES) {
                fileSave(ii);
            }
            else if (iret == wxCANCEL) {
                m_bClosingAll = false;
                setActiveEditor(m_pNotebookMain->GetPrevSelected(), true);
                return;
            }
        }

        arEdit[iC] = pEdit;
        ++iC;
    }

    if ((iC < 1) || (iNC == iC)) {
        m_bClosingAll = false;
        setActiveEditor(m_pNotebookMain->GetPrevSelected(), true);
        return;
    }

    m_pNotebookMain->SetDeletingAll(true);
    int idx = 0;
    int idxl = (0 == iNC) ? 1 : 0;
    for (ii = iPageCount - 1; ii >= idxl; ii--) {
        if (arEdit[ii] != NULL) {
            idx = m_pNotebookMain->GetPageIndex(arEdit[ii]);
            if ((idx >= 0) && (idx < (int)(m_pNotebookMain->GetPageCount()))) {
                arEdit[ii]->DoDeleteAllMarkers();
                m_pNotebookMain->DeletePage(idx);
            }
        }
    }
    m_pNotebookMain->SetDeletingAll(false);

    m_bClosingAll = false;

    if (0 == iNC) {
        pEdit = getEditor(0);
        pEdit->setPinned(false);
        wxString strFilename = pEdit->GetFilename();
        Tstrcpy(m_szLastClosed, LM_CSTR(strFilename));
        pEdit->DoNewFile(m_ScintillaPrefs, wxEmptyString, wxSTC_LEX_LUA, 0);
        pEdit->DoHighlightSyntax(m_ScintillaPrefs.common.syntaxEnable);
        DoAnalyzerUpdateList(pEdit, true);
        setActiveEditor(0, true);
    }
    else if (pEditToSelect) {
        int iToSelect = m_pNotebookMain->GetPageIndex(pEditToSelect);
        setActiveEditor(iToSelect, true);
    }
}

void CometFrame::OnOpenExamplesDir(wxCommandEvent &WXUNUSED(tEvent))
{
#ifdef __WXMSW__

    if (::wxDirExists(CometApp::EXPDIR) == false) {
        wxString strT = uT("Examples directory not found. Please reinstall Comet from http://www.hamady.org");
        OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        SigmaMessageBox(strT, uT("Comet"), wxOK | wxICON_WARNING, this);
        return;
    }

    wxString strT = uT("explorer ");
    strT += CometApp::EXPDIR;
    ::wxExecute(strT, wxEXEC_ASYNC, NULL);

#endif
}

void CometFrame::OnClose(wxCloseEvent &tEvent)
{
    bool bClose = fileExit();
    if (bClose) {
        tEvent.Skip();
    }
    else {
        tEvent.Veto();
    }
}

bool CometFrame::fileExit(void)
{
    if (m_bClosed) {
        return true;
    }

    wxGetApp().setMainFrame(NULL);

    if (m_pNotebookMain->GetPageCount() > 0) {
        m_SigmaCommonPrefs.iFileRecentSel = m_pNotebookMain->GetSelection();
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    ScriptEdit *pEdit = NULL;

    if (iPageCount > 0) {

        bool bFlag = false;
        wxString strT;
        bool bProcessIsAlive = false;

        for (int ii = 0; ii < iPageCount; ii++) {

            pEdit = getEditor(ii);
            if (pEdit == NULL) {
                break;
            }
            bProcessIsAlive = false;
            if (pEdit->isRunning(&bProcessIsAlive)) {
                if (bFlag == false) {
                    setActiveEditor(ii, true);
                    int idT = SigmaMessageBox(pEdit->isDebugging() ? uT("Script in debugging session. Exit anyway?") : uT("Script running. Exit anyway?"), uT("Comet"),
                                              wxYES | wxNO | wxICON_QUESTION | wxNO_DEFAULT, this, uT("&Exit"), uT("&Don't exit"));
                    if (idT != wxYES) {
                        wxGetApp().setMainFrame(this);
                        return false;
                    }
                    bFlag = true;
                }
                if (bProcessIsAlive) {
                    pEdit->processKill(true);
                }
                else {
                    pEdit->luaStopScript();
                }
                // Pause 50 ms to let working thread stopping (only in debugging mode)
                Tsleep(50);
            }

            if (m_SigmaCommonPrefs.iFileRecentCount > 0) {
                if (ii == 0) {
                    m_SigmaCommonPrefs.iFileOpenStatus = 0L;
                }
                for (int jj = 0; jj < m_SigmaCommonPrefs.iFileRecentCount; jj++) {
                    strT = pEdit->GetFilename();
                    if (strT.IsEmpty() == false) {
                        if (Tstrcmp(LM_CSTR(strT), static_cast<const char_t *>(m_SigmaCommonPrefs.szFileRecent[jj])) == 0) {
                            m_SigmaCommonPrefs.iFileOpenStatus |= ((jj > 0) ? (1L << jj) : 1L);
                            break;
                        }
                    }
                }
            }

            if (pEdit->isModified() && (lf_fileReadonly(LM_CSTR(pEdit->GetFilename())) == false)) {
                setActiveEditor(ii, true);
                wxString strT = m_pNotebookMain->GetPageText(ii);
                if (strT.EndsWith(uT("*"))) {
                    strT = strT.Mid(0, strT.Length() - 1);
                }
                strT += uT(" not saved. Save it before exit?");
                int iret = SigmaMessageBox(strT, uT("Comet"),
                                           wxYES | wxNO | wxCANCEL | wxICON_QUESTION | wxYES_DEFAULT, this,
                                           uT("&Save"), uT("&Don't save"));
                if (iret == wxCANCEL) {
                    wxGetApp().setMainFrame(this);
                    return false;
                }
                if (iret == wxYES) {
                    wxGetApp().setMainFrame(this);
                    fileSave(ii);
                    wxGetApp().setMainFrame(NULL);
                }
            }
        }
    }

    bool bFilesys = m_pFilesysPanel && m_auiManager.GetPane(m_pFilesysPanel).IsShown();
    m_SigmaGUIPrefs.bViewExplorer = (bFilesys && (m_pExplorerTree != NULL) && (m_pExplorerTree->IsShown()));
    m_SigmaGUIPrefs.bViewBookmark = bFilesys && (m_pListBookmark != NULL);
    bool bTool = m_pToolPanel && m_auiManager.GetPane(m_pToolPanel).IsShown();
    m_SigmaGUIPrefs.bViewConsole = bTool && (m_pEditConsole != NULL);
    m_SigmaGUIPrefs.bViewOutput = bTool && (m_pEditOutput != NULL);

    m_SigmaGUIPrefs.bViewToolbar = this->GetToolBar()->IsShown();
    m_SigmaGUIPrefs.bViewStatusbar = this->GetStatusBar()->IsShown();
    m_SigmaGUIPrefs.bViewSearchbar = (m_cboAnalyzer != NULL);

    m_SigmaGUIPrefs.bFrameMaximized = this->IsMaximized();
    wxSize sizeT = this->GetSize();
    m_SigmaGUIPrefs.iFrameHeight = sizeT.GetHeight();
    m_SigmaGUIPrefs.iFrameWidth = sizeT.GetWidth();

    if (m_bSavePerspective && (::wxFileExists(static_cast<const char_t *>(m_szLayoutConfigFilename)) || (m_bPerspectiveLoaded == false))) {
        saveUIperspective();
    }
    else if (m_bSavePerspective == false) {
        Tunlink(m_szLayoutConfigFilename);
    }

    if (m_pFindFileDlg) {
        updateFindFilePos();
        // call Destroy directly, since at this point it is not necessary to handle EVT_CLOSE
        m_pFindFileDlg->Destroy();
        m_pFindFileDlg = NULL;
    }

    if (m_pFindDirDlg) {
        updateFindDirPos();
        // call Destroy directly, since at this point it is not necessary to handle EVT_CLOSE
        m_pFindDirDlg->Destroy();
        m_pFindDirDlg = NULL;
    }

    CometConfig *pSigmaConfig = new (std::nothrow) CometConfig(CometConfig::SECTIONCOUNT);
    if (pSigmaConfig) {
        pSigmaConfig->save((const char_t *)m_szSigmaConfigFilename, m_SigmaCommonPrefs, m_SigmaGUIPrefs, TOOL_CONF);
        delete pSigmaConfig;
        pSigmaConfig = NULL;
    }

    EditorConfig *pEditorConfig = new (std::nothrow) EditorConfig(EditorConfig::SECTIONCOUNT);
    if (pEditorConfig) {
        pEditorConfig->save((const char_t *)m_szEditorConfigFilename, m_ScintillaPrefs);
        delete pEditorConfig;
        pEditorConfig = NULL;
    }

    // Keep copied text in the clipboard after exit
    if (wxTheClipboard && wxTheClipboard->Open()) {
        if (wxTheClipboard->IsSupported(wxDF_TEXT)) {
            wxTheClipboard->Flush();
        }
        wxTheClipboard->Close();
    }

    m_bClosed = true;
    return true;
}

void CometFrame::OnFileExit(wxCommandEvent &WXUNUSED(tEvent))
{
    bool bClose = fileExit();
    if (bClose) {
        Close(true);
    }
}
