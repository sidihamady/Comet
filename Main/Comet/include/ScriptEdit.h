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


#ifndef SCRIPT_EDIT_H
#define SCRIPT_EDIT_H

#include "CodeEdit.h"
#include "FindFileDlg.h"
#include "CometProcess.h"
#include "CodeAnalyzer.h"

#define FIND_ITEMFOUND     0
#define FIND_PARAMERR     -1
#define FIND_LIMITREACHED -2
#define FIND_NOTFOUND     -3

#define FILE_FILTER_LUA      0
#define FILE_FILTER_CPP      1
#define FILE_FILTER_BASH     2
#define FILE_FILTER_PYTHON   3
#define FILE_FILTER_OCTAVE   4
#define FILE_FILTER_FORTRAN  5
#define FILE_FILTER_LATEX    6
#define FILE_FILTER_MAKEFILE 7
#define FILE_FILTER_BATCH    8
#define FILE_FILTER_SOLIS    9
#define FILE_FILTER_SETTINGS 10
#define FILE_FILTER_COMET    11
#define FILE_FILTER_KNOWN    12
#define FILE_FILTER_ALL      13

#define UTF8FROM_ISO8859L1 0
#define UTF8FROM_ISO8859L9 1
#define UTF8FROM_CP1252    2
#define UTF8FROM_LAST      2

#define UNICODE_CHECKLEN 4096

#define UNICODE_BYTEMASK       0xBF
#define UNICODE_BYTEMARK       0x80
#define UNICODE_HALFSHIFT      10
#define UNICODE_HALFBASE       0x0010000UL
#define UNICODE_HALFMASK       0x3FFUL
#define UNICODE_SURR_HIGHSTART 0xD800UL
#define UNICODE_SURR_HIGHEND   0xDBFFUL
#define UNICODE_SURR_LOWSTART  0xDC00UL
#define UNICODE_SURR_LOWEND    0xDFFFUL

#define FileStatsDlg_STYLE    (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
#define FileStatsDlg_TITLE    uT("File Statistics")
#define FileStatsDlg_SIZE     wxDefaultSize
#define FileStatsDlg_POSITION wxDefaultPosition

class FileStatsDlg : public wxDialog
{
public:
    FileStatsDlg(wxWindow *pParent, const wxPoint &ptInit = FileStatsDlg_POSITION, wxWindowID idT = IDD_SCRIPT_STATS,
                 const wxString &strTitle = FileStatsDlg_TITLE, const wxSize &sizeT = FileStatsDlg_SIZE, long styleT = FileStatsDlg_STYLE);
    virtual ~FileStatsDlg();

    void OnOK(wxCommandEvent &tEvent);
    void OnCloseWindow(wxCloseEvent &tEvent);

private:
    wxStaticText *m_txtAA; // Range
    wxStaticText *m_txtAB;
    wxStaticText *m_txtBA; // File type
    wxStaticText *m_txtBB;
    wxStaticText *m_txtCA; // Line count
    wxStaticText *m_txtCB;
    wxStaticText *m_txtDA; // Word count
    wxStaticText *m_txtDB;
    wxStaticText *m_txtEA; // Characters count
    wxStaticText *m_txtEB;

    DECLARE_EVENT_TABLE()

    wxPoint m_ptInit;
    void DoInitControls(void);
    void initInterface();
    static bool ShowToolTips();
    void DoCancel(void);
};

class ScriptEdit : public CodeEdit
{

private:
    static long EDITID;
    long m_Id;

    wxString m_strFilename;
    wxDateTime m_ChangeTime;
    bool m_bAutoReload;

    bool m_bLoading;

    bool m_bLinePrev;
    int m_iLinePrev;

    int m_iMaxLineWidth;

    int m_iBookmarkCount;
    int m_iBreakpointCount;

    wxString m_strDebugTrace;

    wxString m_strErrMsg;

    int m_iFindMarkerCount;
    int m_iFindIndicCount;

    bool canSearch(wxString &strFind, wxString strReplace, bool bReplace, int *iFindLen, int *iLineCount, int *iCurLine);

    int lexerFromExtension(const wxString &strExt, const wxString &strShortFilename);

    bool FileLoaded(const wxString &filenameT, int maxLineIndex = -1, bool bReload = false, int iSelStart = 0, int iSelEnd = 0, bool bConverted = false, bool bSelect = true, bool bFocus = true);

    bool m_bLexerEnforced;

    // For external tool
    CometProcess *m_pProcess;
    long m_iProcessId;
    wxTimer *m_pProcessTimer;

    wxTimer *m_pReloadTimer;

    // UTF8
    bool m_bUTF8done[UTF8FROM_LAST + 1];

    static const uint8_t UTF32_BOM_LE[];
    static const uint8_t UTF32_BOM_BE[];
    static const uint8_t UTF16_BOM_LE[];
    static const uint8_t UTF16_BOM_BE[];
    static const uint8_t UTF8_BOM[];
    static const uint8_t UTF16_BYTEMARK[];

    static bool isCP1252(uint8_t *pBufferCP, size_t sizeCP);
    bool convertFromCP1252(uint8_t *pBufferUTF8, size_t sizeUTF8);

    static bool isUTF8(uint8_t *pBufferUTF, size_t sizeUTF, bool *bBOM);
    static bool isUTF8BOM(uint8_t *pBufferUTF, size_t sizeUTF);
    static bool isUTF16(uint8_t *pBufferUTF, size_t sizeUTF, bool *bBigEndian, bool *bBOM);
    static bool isUTF16BOM(uint8_t *pBufferUTF, size_t sizeUTF, bool *bBigEndian);
    static bool isUTF32(uint8_t *pBufferUTF, size_t sizeUTF, bool *bBigEndian, bool *bBOM);
    static bool isUTF32BOM(uint8_t *pBufferUTF, size_t sizeUTF, bool *bBigEndian);
    static bool convertFromUTF16(uint8_t *pBufferUTF8, uint8_t *pBufferUTF16, size_t sizeUTF8, size_t sizeUTF16, bool bBigEndian);
    static bool convertFromUTF32(uint8_t *pBufferUTF8, uint8_t *pBufferUTF32, size_t sizeUTF8, size_t sizeUTF32, bool bBigEndian);
    static int doConvertFromWC(uint8_t *tUTF8, wchar_t tWC, size_t sizeUTF8);
    static bool convertFromWC(uint8_t *pBufferUTF8, wchar_t *pBufferWC, size_t sizeUTF8, size_t sizeWC, size_t *psizeRet, bool *pbCheck);

    int m_iStatusLine;
    int m_iErrLine;

    int m_iDebugCount;
    bool m_bStepInto;

    bool m_bEnableCodeAnalyzer;
    CodeAnalyzer *m_pCodeAnalyzer;
    wxString m_strSelectedFunc;
    wxString m_strFileToOpen;

    bool m_bFindin;
    bool m_bPinned;

    bool m_bCanSetFocus;

    DECLARE_EVENT_TABLE()

public:
    ScriptEdit(wxWindow *pParent, wxWindowID id = wxID_ANY,
               const wxPoint &pos = wxDefaultPosition,
               const wxSize &size = wxDefaultSize,
               long style = wxNO_BORDER | wxVSCROLL | wxHSCROLL,
               const wxString &strName = wxSTCNameStr,
               int iLexer = wxSTC_LEX_LUA);
    ~ScriptEdit();

    long getScriptId(void)
    {
        return m_Id;
    }

    bool setPrefs(const ScintillaPrefs &tPrefs, bool bInit, bool bInitIndent);

    // >> Communication between threads
    bool m_bDebugging;
    bool m_bDebugIt;
    bool m_bStopIt;
    // <<

    bool luaReset(void);
    bool luaRunScript(void);
    bool luaDebugScript(void);
    bool luaStopScript(void);

    bool isExtKnown(void);
    void enforceLexer(int iLexerF);

    void DoAddStatus(int iLine, int iStatusBit, bool bFocus = true);

    void OnMouseWheel(wxMouseEvent &tEvent);

    void OnThreadUpdated(wxCommandEvent &tEvent);

    bool isFromCP1252done(void)
    {
        return m_bUTF8done[UTF8FROM_CP1252];
    }
    bool isFromISO8859L1done(void)
    {
        return m_bUTF8done[UTF8FROM_ISO8859L1];
    }
    bool isFromISO8859L9done(void)
    {
        return m_bUTF8done[UTF8FROM_ISO8859L9];
    }

    bool stepInto(void)
    {
        return m_bStepInto;
    }

    void enableStepInto(bool bStepInto);

    bool breakpointIn(int iLine)
    {
        if ((iLine >= 0) && (iLine < this->GetLineCount())) {
            return ((MarkerGet(iLine) & SCRIPT_MASK_BREAKPOINT) != 0);
        }
        return false;
    }

    bool isDebugging(void)
    {
        return (m_bRunning && m_bDebugging);
    }
    bool isRunning(bool *pbProcess = NULL);
    void setRunning(bool bRunning);

    int getMaxLineWidth(void)
    {
        return m_iMaxLineWidth;
    }
    void setMaxLineWidth(int iMaxLineWidth)
    {
        m_iMaxLineWidth = iMaxLineWidth;
    }

    int getBookmarkCount(void)
    {
        return m_iBookmarkCount;
    }

    int getBreakpointCount(void)
    {
        return m_iBreakpointCount;
    }

    void initDrop(void);

    bool processStart(void);
    void processKill(bool bSilent = false);
    bool processIsAlive(void);
    void processOnTerminate(int pid, int iExitStatus);
    void processOutput(const wxString &strT, int iExitStatus);
    void processSetRunning(void);
    void OnTimer(wxTimerEvent &tEvent);

    bool DoReload(bool bSilent, bool bUserAction);
    void OnTimerReload(wxTimerEvent &tEvent);
    void resetTimerReload(void);
    bool autoReload(void)
    {
        return (m_bAutoReload && (m_pReloadTimer != NULL));
    }

    void calcLinenumberMargin(bool bRecalc = false);

    void DoFindReset(void);
    void DoFindHighlight(int iStartPos, int iEndPos);
    int DoFindPrev(wxString &strFind, bool bVerbose = false, int iStyle = 0,
                   bool bSel = false, int *iFindEnd = NULL, bool bAddMarker = true);
    int DoFindReplace(wxString &strFind, bool bVerbose = false, int iStyle = 0,
                      wxString strReplace = wxEmptyString, bool bReplace = false, bool bAll = false,
                      int *iCount = NULL, bool bSel = false, int *iFindStart = NULL, bool bAddMarker = true);

    void OnSize(wxSizeEvent &tEvent);
    void OnSetFocus(wxFocusEvent &tEvent);

    bool updateFilename(const wxString &filenameT, bool bSelect = true, bool bFocus = true);

    void updatePage(const wxString &strTitle);

    void updateFindList(wxComboBox *pCombo, int nFunc);

    int CodeAnalyzerGetCount(void);
    int CodeAnalyzerUpdate(wxComboBox *pCombo, bool bRecreate);
    int CodeAnalyzerGetLine(int ii);
    int CodeAnalyzerGetLine(const char_t *funcName);
    void CodeAnalyzerSetLine(const char_t *funcName, int iLine);
    int CodeAnalyzerGetIndex(const char_t *funcName, bool bComplete = true);
    wxString CodeAnalyzerGetName(int ii);
    AnalyzerElement::SCOPE CodeAnalyzerGetScope(int ii);

    void OnMouseLeftUp(wxMouseEvent &tEvent);

    void CreateMenuFormat(wxMenu *menuFormat, bool bIncludeComment);
    void CreateMenuSnippet(wxMenu *menuFormat);

    void OnContextMenu(wxContextMenuEvent &tEvent);
    void OnInsertCodeSnippet(wxCommandEvent &tEvent);
    void DoInsertCodeSnippet(int idT);

    void OnCommentSelection(wxCommandEvent &tEvent);
    void DoCommentSelection(bool bComment);

    void OnUpperCase(wxCommandEvent &tEvent);
    void OnLowerCase(wxCommandEvent &tEvent);
    void DoChangeCase(int iCase);

    void DoLaTeXFormat(int idT);
    void OnLaTeXFormat(wxCommandEvent &tEvent);

    void OnUpdateUndoRedo(wxUpdateUIEvent &tEvent);
    void OnUpdateCopyPaste(wxUpdateUIEvent &tEvent);

    void OnEditAction(wxCommandEvent &tEvent);

    void DoEditRedo(void);
    void DoEditUndo(void);
    void DoEditDelete(void);
    void DoEditCut(void);
    void DoEditCopy(void);
    void DoEditPaste(void);

    inline void DoGotoLine(int iLine, bool bFocus = true)
    {
        if ((iLine < 0) || (iLine >= this->GetLineCount())) {
            return;
        }
        if (bFocus) {
            this->SetFocus();
        }
        this->GotoLine(iLine);
        this->SetCaretLineVisible(true);

        // Ensure that the line is visible (if wrap mode enabled or disabled)
        int iH = this->TextHeight(0);
        wxPoint ptT = this->PointFromPosition(this->GetCurrentPos());
        int iLineW = ptT.y / iH;
        this->LineScroll(0, iLineW - (this->LinesOnScreen() / 2));
    }

    void DoGoto(void);
    void DoEditIndentRed(void);
    void DoEditSelectAll(void);
    void DoEditSelectLine(void);
    void DoEditSelectWord(void);
    void DoEditInsertText(const wxString &strT, int iLen = -1);
    void DoEditInsertDate(void);
    void DoEditInsertTime(void);
    void DoEditInsertDateTime(void);

    void DoLineNumber(bool bEnable);
    void DoShowMarker(bool bEnable);
    void DoShowStatus(bool bEnable);
    void DoFoldEnable(bool bEnable);
    void DoWrapMode(bool bEnable);
    void DoUseTab(bool bEnable);
    void DoIndentGuide(bool bEnable);
    void DoSetTabSize(int iTabSize);
    void DoFormatDoc(void);
    void DoZoomIn(void);
    void DoZoomOut(void);
    void DoZoomNone(void);

    void DoConvertEOL(int idT);
    void DoEnableEOL(bool bEnable);
    void DoEnableWhiteSpace(bool bEnable);

    void DoEnableLongLine(bool bEnable);
    void DoSetLongLine(int iLen);

    void DoModifyMarker(int iMarkerBit, int nPos = -1);
    void DoModifyFolding(int nPos = -1);

    void DoDeleteBookmark(int iLine);
    void DoDeleteBookmarkAll(void);

    void DoDeleteAllMarkers(void);
    void DoDeleteAllBookmarks(void);
    void DoFindNextBookmark(void);
    void DoFindPrevBookmark(void);

    void DoDeleteAllBreakpoints(void);

    void DoShowStats(void);

    void OnMarginClick(wxStyledTextEvent &tEvent);
    void OnCharAdded(wxStyledTextEvent &tEvent);
    void OnModified(wxStyledTextEvent &tEvent);
    void OnDwellStart(wxStyledTextEvent &tEvent);
    void OnDwellEnd(wxStyledTextEvent &tEvent);

    void DoSetModified(int iLine = -1);
    void DoSetModifiedLean(int iLine);

    bool GetUserSettings(const wxString &strFilename);

    void setFindMarker(int iLine, const wxString &strFind, const wxString &strReplace = wxEmptyString, bool bFocus = true);
    int getFindMarkerCount(void)
    {
        return m_iFindMarkerCount;
    }
    void resetFindMarkers(void)
    {
        DoFindReset();
        MarkerDeleteAll(SCRIPT_MASK_FINDBIT);
        m_iFindMarkerCount = 0;
        m_bFindin = false;
    }
    void resetAllMarkers(void)
    {
        MarkerDeleteAll(SCRIPT_MASK_MODIFIEDBIT);
        MarkerDeleteAll(SCRIPT_MASK_SAVEDBIT);
        MarkerDeleteAll(SCRIPT_MASK_ERRORBIT);
        MarkerDeleteAll(SCRIPT_MASK_DEBUGBIT);
        MarkerDeleteAll(SCRIPT_MASK_BREAKPOINTBIT);
        DoDeleteAllBookmarks();
        resetFindMarkers();
    }
    void resetErrorMarkers(void)
    {
        MarkerDeleteAll(SCRIPT_MASK_ERRORBIT);
    }
    bool containsMarkers(void)
    {
        const int arM[] = { SCRIPT_MASK_MODIFIED, SCRIPT_MASK_SAVED, SCRIPT_MASK_FIND,
                            SCRIPT_MASK_BOOKMARK, SCRIPT_MASK_ERROR, SCRIPT_MASK_DEBUG, SCRIPT_MASK_BREAKPOINT, -1 };
        for (int ii = 0; arM[ii] > 0; ii++) {
            int nFoundLine = MarkerNext(0, arM[ii]);
            if (nFoundLine >= 0) {
                return true;
            }
        }
        return false;
    }
    bool containsMarkers(int markerMask)
    {
        int nFoundLine = MarkerNext(0, markerMask);
        if (nFoundLine >= 0) {
            return true;
        }
        return false;
    }
    bool containsMarkersModified(void)
    {
        return containsMarkers(SCRIPT_MASK_MODIFIED);
    }
    bool containsMarkersSaved(void)
    {
        return containsMarkers(SCRIPT_MASK_SAVED);
    }
    bool containsMarkersFind(void)
    {
        return containsMarkers(SCRIPT_MASK_FIND);
    }
    bool containsMarkersBookmark(void)
    {
        return containsMarkers(SCRIPT_MASK_BOOKMARK);
    }
    bool containsMarkersError(void)
    {
        return containsMarkers(SCRIPT_MASK_ERROR);
    }
    bool containsMarkersDebug(void)
    {
        return containsMarkers(SCRIPT_MASK_DEBUG);
    }
    bool containsMarkersBreakpoint(void)
    {
        return containsMarkers(SCRIPT_MASK_BREAKPOINT);
    }
    void resetModified(void)
    {
        resetAllMarkers();
        EmptyUndoBuffer();
    }
    int getFindIndicCount(void)
    {
        return m_iFindIndicCount;
    }

    bool isFindin(void)
    {
        return m_bFindin;
    }

    void setFindin(bool bFindin)
    {
        m_bFindin = bFindin;
    }

    bool isPinned(void)
    {
        return m_bPinned;
    }

    void setPinned(bool bPinned)
    {
        m_bPinned = bPinned;
    }

    bool DoNewFile(ScintillaPrefs &tScintillaPrefs, const wxString &strTemplate, int iLexer, int iLexerVar);
    bool DoLoadFile(const wxString &filenameT = wxEmptyString, bool bRun = false, bool bReload = false, bool bOpenRecent = false, bool bSelect = true, bool bFocus = true);
    bool DoUTF8Encode(int iFrom);
    bool DoSaveFile(const wxString &filenameT = wxEmptyString, bool bSelect = true);
    bool isModified(void);
    wxString GetFilename() { return m_strFilename; };
    void SetFilename(const wxString &strFilename)
    {
        m_strFilename = strFilename;
    }

    void DoScriptAction(int idT);
    void DoEditAction(int idT);
    void DoPrint(void);
    bool ShowPrintPreviewDialog(void);

    static int supportedLexer(const wxString &strExt, int *piLexerType = NULL);

    void OnDropFiles(wxDropFilesEvent &);
};

#endif
