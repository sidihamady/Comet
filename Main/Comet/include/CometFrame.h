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


#ifndef SIGMAFRAME_H
#define SIGMAFRAME_H

#include "ScriptEdit.h"
#include "OutputEdit.h"
#include "ConsoleEdit.h"
#include "CometFileExplorer.h"
#include "EditorConfig.h"
#include "CometConfig.h"
#include "LexerConfig.h"
#include "FindDirDlg.h"
#include "FindFileDlg.h"
#include "BookmarkList.h"

#include <wx/aui/aui.h>
#include <wx/aui/auibook.h>
#include <wx/utils.h>
#include <wx/arrstr.h>
#include <wx/tokenzr.h>
#include <wx/notebook.h>

#include <vector>
#include <algorithm>

#define FRAME_MINSIZE       wxSize(400, 300)
#define NOTEBOOK_MINSIZE    wxSize(200, 200)

#ifdef __WXMSW__
inline int gettimeofday(struct timeval *p, void *tz)
{
    ULARGE_INTEGER ul;
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ul.LowPart = ft.dwLowDateTime;
    ul.HighPart = ft.dwHighDateTime;
    ul.QuadPart /= 10ULL;
    ul.QuadPart -= 11644473600000000ULL;
    p->tv_usec = (long)(ul.QuadPart % 1000000LL);
    p->tv_sec = (long)(ul.QuadPart / 1000000LL);
    return 0;
}
#else
#include <sys/time.h>
#endif

#define TB_DEFAULT_STYLE (wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT)

#define SIGMAFRAME_TIMER_NONE  0
#define SIGMAFRAME_TIMER_SHORT 10000
#define SIGMAFRAME_TIMER_LONG  20000

#define CODESAMPLE_MAXLEN   16384
#define CODESAMPLE_EXAMPLE  1
#define CODESAMPLE_SNIPPET  2
#define CODESAMPLE_TEMPLATE 3

class CodeItem
{
private:
    char_t m_szTitle[LM_STRSIZES]; // Code title
    char_t *m_pszCode;             // Code Content
    int m_iLineCount;              // Total number of line (including empty lines)
    int m_iLinemax;                // Index of the longest line

public:
    CodeItem()
    {
        memset(m_szTitle, 0, LM_STRSIZES * sizeof(char_t));
        m_pszCode = NULL;
        m_iLineCount = 0;
        m_iLinemax = 0;
    }
    ~CodeItem()
    {
        if (m_pszCode) {
            free(m_pszCode);
            m_pszCode = NULL;
        }
        m_iLineCount = 0;
        m_iLinemax = 0;
    }
    bool set(const char_t *pszTitle, int iLineCount, int iLinemax, const char_t *pszCode)
    {
        int iLen = (int)(Tstrlen(pszCode));
        if ((iLen < 7) || (iLen > CODESAMPLE_MAXLEN) || (iLineCount < 1) || (iLinemax < 0)) {
            return false;
        }
        if (m_pszCode) {
            free(m_pszCode);
            m_pszCode = NULL;
        }
        m_pszCode = (char_t *)malloc((iLen + 1) * sizeof(char_t));
        if (m_pszCode == NULL) {
            return false;
        }
        m_pszCode[iLen] = uT('\0');
        memset(m_szTitle, 0, LM_STRSIZES * sizeof(char_t));

        Tstrcpy(m_pszCode, pszCode);
        Tstrcpy(m_szTitle, pszTitle);
        m_iLineCount = iLineCount;
        m_iLinemax = iLinemax;
        return true;
    }

    const char_t *getTitle(void)
    {
        return (const char_t *)(m_szTitle);
    }

    const char_t *getCode(void)
    {
        if (m_pszCode == NULL) {
            return NULL;
        }
        return (const char_t *)(m_pszCode);
    }

    int getLinemax(void)
    {
        return m_iLinemax;
    }
};

class CodeSample
{
private:
    int m_iType;
    int m_iLexer;
    int m_iCount;
    CodeItem *m_pCode;

public:
    CodeSample(int iType = 0, int iLexer = 0)
    {
        m_iType = 0;
        m_iLexer = 0;
        m_iCount = 0;
        m_pCode = NULL;
        create(iType, iLexer);
    }
    ~CodeSample()
    {
        release();
    }
    void release(void)
    {
        m_iType = 0;
        m_iLexer = 0;
        m_iCount = 0;
        if (m_pCode) {
            delete[] m_pCode;
            m_pCode = NULL;
        }
    }

    bool createLua(int iType);
    bool createCPP(int iType);
    bool createPython(int iType);
    bool createLaTeX(int iType);
    bool create(int iType, int iLexer)
    {
        switch (iLexer) {
            case wxSTC_LEX_LUA:
                createLua(iType);
                break;
            case wxSTC_LEX_CPP:
                createCPP(iType);
                break;
            case wxSTC_LEX_PYTHON:
                createPython(iType);
                break;
            case wxSTC_LEX_LATEX:
                createLaTeX(iType);
                break;
            default:
                return false;
        }
        return true;
    }

    int getCount(void)
    {
        if (m_pCode == NULL) {
            return 0;
        }
        return m_iCount;
    }

    const char_t *getTitle(int idx)
    {
        if (m_pCode == NULL) {
            return NULL;
        }
        if ((idx < 0) || (idx >= m_iCount)) {
            return NULL;
        }
        return m_pCode[idx].getTitle();
    }

    const char_t *getCode(int idx)
    {
        if (m_pCode == NULL) {
            return NULL;
        }
        if ((idx < 0) || (idx >= m_iCount)) {
            return NULL;
        }
        return m_pCode[idx].getCode();
    }

    int getLinemax(int idx)
    {
        if (m_pCode == NULL) {
            return -1;
        }
        if ((idx < 0) || (idx >= m_iCount)) {
            return -1;
        }
        return m_pCode[idx].getLinemax();
    }

    static wxString getTemplate(int iLexer, int iLexerVar);
};

// up to ~ 1MB of memory for bookmarks
#define BOOKMARKLIST_MAXITEMS  2048
#define BOOKMARKLIST_MAXITEMSL 64
typedef struct _bookmark_t
{
    char_t filename[LM_STRSIZE];
    int line;
    char_t info[LM_STRSIZE];
    long id;
} bookmark_t;
struct bookmarkCompareFilename
{
    inline bool operator()(const bookmark_t *pBookmarkL, const bookmark_t *pBookmarkR)
    {
        return (Tstrcmp(static_cast<const char_t *>(pBookmarkL->filename), static_cast<const char_t *>(pBookmarkR->filename)) < 0);
    }
};
struct bookmarkCompareLine
{
    inline bool operator()(const bookmark_t *pBookmarkL, const bookmark_t *pBookmarkR)
    {
        return (pBookmarkL->line < pBookmarkR->line);
    }
};
struct bookmarkCompareInfo
{
    inline bool operator()(const bookmark_t *pBookmarkL, const bookmark_t *pBookmarkR)
    {
        return (Tstrcmp(static_cast<const char_t *>(pBookmarkL->info), static_cast<const char_t *>(pBookmarkR->info)) < 0);
    }
};
struct bookmarkCompareFilenameDesc
{
    inline bool operator()(const bookmark_t *pBookmarkL, const bookmark_t *pBookmarkR)
    {
        return (Tstrcmp(static_cast<const char_t *>(pBookmarkL->filename), static_cast<const char_t *>(pBookmarkR->filename)) > 0);
    }
};
struct bookmarkCompareLineDesc
{
    inline bool operator()(const bookmark_t *pBookmarkL, const bookmark_t *pBookmarkR)
    {
        return (pBookmarkL->line > pBookmarkR->line);
    }
};
struct bookmarkCompareInfoDesc
{
    inline bool operator()(const bookmark_t *pBookmarkL, const bookmark_t *pBookmarkR)
    {
        return (Tstrcmp(static_cast<const char_t *>(pBookmarkL->info), static_cast<const char_t *>(pBookmarkR->info)) > 0);
    }
};
//

class AnalyzerComboBox : public CometComboBox
{
protected:
    DECLARE_EVENT_TABLE()

public:
    AnalyzerComboBox(wxWindow *pParent, wxWindowID idT,
                     const wxString &valueT = wxEmptyString,
                     const wxPoint &posT = wxDefaultPosition,
                     const wxSize &sizeT = wxDefaultSize,
                     int nn = 0, const wxString choicesT[] = NULL,
                     long styleT = 0,
                     const wxValidator &validatorT = wxDefaultValidator,
                     const wxString &nameT = wxComboBoxNameStr);

    void OnKeyDown(wxKeyEvent &tEvent);
    void OnRightUp(wxMouseEvent &tEvent);
    void OnRightDown(wxMouseEvent &tEvent);
    void OnListReset(wxCommandEvent &tEvent);
};

class SigmaDropTarget : public wxFileDropTarget
{
public:
    SigmaDropTarget();

    virtual bool OnDropFiles(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), const wxArrayString &filenames);

private:
    DECLARE_NO_COPY_CLASS(SigmaDropTarget)
};

class SigmaToolBar : public wxToolBar
{
private:
    SigmaDropTarget *m_pDropTarget;

public:
    SigmaToolBar(wxWindow *parent, wxWindowID id = -1,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = TB_DEFAULT_STYLE & ~wxTB_NO_TOOLTIPS,
                 const wxString &name = wxPanelNameStr)
        : wxToolBar(parent, id, pos, size, style, name)
    {
        m_pDropTarget = new (std::nothrow) SigmaDropTarget();
        SetDropTarget(m_pDropTarget);
    }

    ~SigmaToolBar()
    {
        if (NULL != m_pDropTarget) {
            SetDropTarget(NULL);
            m_pDropTarget = NULL; // deleted in SetDropTarget(NULL)
        }
    }
};

#define INFOBAR_TYPE_NONE 0
#define INFOBAR_TYPE_MAIN 1
#define INFOBAR_TYPE_TOOL 2

class CodeInfobar : public wxWindow
{
private:
    int m_iType;
    bool m_bActive;
    int m_iHeaderSize;
    int m_iScrollSize;

    CodeInfobar();
    CodeInfobar(const CodeInfobar &);

public:
    CodeInfobar(wxPanel *pPanel, int iId, wxSize sizeT, const wxString &strT, int iType, bool bActive);

    void setActive(bool bActive)
    {
        m_bActive = bActive;
    }
    bool isActive(void)
    {
        return m_bActive;
    }

    void OnPaint(wxPaintEvent &evt);
    void OnEraseBackground(wxEraseEvent &evt);
    void OnMouseDown(wxMouseEvent &evt);
    void OnRightContext(wxMouseEvent &evt);
    void LineFromPos(wxPoint pt, int *piLastStart, int *piLineEnd);

    void draw(wxPaintDC &dc, CodeEdit *pEdit);
    void erase(wxPaintDC &dc);

    DECLARE_EVENT_TABLE()
};

class CometFrame : public wxFrame
{
private:
    bool m_bShellStarted;

    bool m_bSavePerspective;
    bool m_bPerspectiveLoaded;

    SigmaCommonPrefs m_SigmaCommonPrefs;
    SigmaGUIPrefs m_SigmaGUIPrefs;
    ScintillaPrefs m_ScintillaPrefs;

    char_t m_szSigmaConfigDir[LM_STRSIZE];
    char_t m_szSigmaConfigFilename[LM_STRSIZE];
    char_t m_szEditorConfigFilename[LM_STRSIZE];
    char_t m_szLexerConfigFilename[LM_STRSIZE];
    char_t m_szLayoutConfigFilename[LM_STRSIZE];

    char_t m_szLastClosed[LM_STRSIZE];

    wxMenu *m_pFileMenu;
    wxMenu *m_pSearchMenu;

    wxMenu *m_pRecentFileMenu;
    wxMenu *m_pRecentDirMenu;

    wxMenuItem *m_pRecentFileMenuItem;
    wxMenuItem *m_pRecentDirMenuItem;
    wxMenuItem *m_pExamplesMenuItem;
    wxMenuItem *m_pSnippetsMenuItem;
    wxMenuItem *m_pFormatMenuItem;
    wxMenuItem *m_pDateMenuItem;
    wxMenuItem *m_pEOLMenuItem;
    wxMenuItem *m_pUTF8MenuItem;

    SigmaDropTarget *m_pDropTarget;

    wxAuiManager m_auiManager;

    wxMenuBar *m_pMenuBar;
    SigmaToolBar *m_pToolBar;
    wxStatusBar *m_pStatusBar;

    AnalyzerComboBox *m_cboAnalyzer;

    CometFileExplorer *CreateExplorerTree(void);
    wxAuiNotebook *CreateFilesysNotebook(void);

    wxAuiNotebook *CreateMainNotebook(void);

    OutputEdit *CreateOutputWindow(void);
    ConsoleEdit *CreateConsoleWindow(void);
    BookmarkList *CreateBookmarkWindow(void);
    wxAuiNotebook *CreateToolNotebook(void);
    void UpdateToolNotebook(void);

    wxListCtrl *CreateStackWindow(void);
    wxListCtrl *CreateWatchWindow(void);
    wxAuiNotebook *CreateDebugNotebook(void);
    void UpdateDebugNotebook(const wxString &strFilename);

    wxPanel *m_pFilesysPanel;
    wxAuiNotebook *m_pNotebookFilesys;
    CometFileExplorer *m_pExplorerTree;
    BookmarkList *m_pListBookmark;

    wxPanel *m_pMainPanel;
    wxAuiNotebook *m_pNotebookMain;

    wxPanel *m_pToolPanel;
    wxAuiNotebook *m_pNotebookToolbox;
    OutputEdit *m_pEditOutput;
    ConsoleEdit *m_pEditConsole;

    wxPanel *m_pDebugPanel;
    wxAuiNotebook *m_pNotebookDebug;
    wxListCtrl *m_pListStack;
    wxListCtrl *m_pListWatch;

    enum LIST_SORTDIRECTION
    {
        NONE,
        ASC,
        DESC
    };
    int m_iListWatchSorted;
    int m_iListStackSorted;

    std::vector<bookmark_t *> m_arBookmark;

    bool m_bInitialized;

    bool m_bRefreshLayout;

    wxString getTextToFind(ScriptEdit *pEdit);

    FindDirDlg *m_pFindDirDlg;
    FindFileDlg *m_pFindFileDlg;
    void DoFindReplace(bool bFindOnly);

    wxTimer *m_pTimer;
    int m_iTimerCounter;
    int m_iTimerDuration;

    CodeSample *m_pExample;
    CodeSample *m_pSnippet;

    CodeSample *m_pExampleLua;
    CodeSample *m_pSnippetLua;
    CodeSample *m_pSnippetCPP;
    CodeSample *m_pSnippetPython;
    CodeSample *m_pSnippetLaTeX;

    void saveUIperspective(void);
    void loadUIperspective(void);

    bool m_bClosingAll;
    bool m_bClosingSel;

    bool m_bClosed;

    wxBitmap m_bmpNone;
    wxBitmap m_bmpReadOnly;
    wxBitmap m_bmpRunning;
    wxBitmap m_bmpDebugging;
    wxBitmap m_bmpFindin;
    wxBitmap m_bmpPinned;
    wxBitmap m_bmpAutoReload;

    wxDialog *m_dlgTab;
    wxDialog *m_dlgTools;
    wxDialog *m_dlgColors;
    wxDialog *m_dlgLexer;

    CodeInfobar *m_pInfobarMain;
    CodeInfobar *m_pInfobarTool;
    wxBoxSizer *m_pSizerMain;
    wxBoxSizer *m_pSizerTool;

    bool m_bSilent;

    struct timeval m_UItimeval;

    bool m_bOpeningFile;

    wxSize m_ToolNotebookSize;
    wxSize m_FilesysNotebookSize;
    wxSize m_DebugNotebookSize;

public:
    CometFrame(int iWidth = MAINFRAME_SIZEW, int iHeight = MAINFRAME_SIZEH);
    ~CometFrame();

#ifdef __WXMSW__
    WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
#endif

    static const int MAX_DROPCOUNT;

    static const int TOOL_LEX[];
    static const char_t *TOOL_EXT[];
    static SigmaToolPrefs TOOL_CONF;
    static bool isToolSet(int iLexer);
    static const wxString getToolCmd(int iLexer, const wxString &strExt, const wxString &strShortFilename, bool *pbOut, bool *pbSilent, bool *pbCls);
    static const bool getToolOut(int iLexer);
    static const bool getToolSilent(int iLexer);

    static wxString getLabelFromPath(const wxString &strFullPath, bool bShort = true)
    {
        const int iLenMax = bShort ? LM_STRSIZEN : LM_STRSIZE;

        wxString strT = strFullPath;
        wxFileName fname = strT;
        wxString strTT = fname.GetFullName();
        int iLen = (int)(strTT.Length());
        if (iLen > iLenMax) {
            strT = uT("...");
            strT += strTT.Mid(iLen - iLenMax);
        }
        else {
            strTT = fname.GetFullPath();
            iLen = (int)(strTT.Length());
            if (iLen > iLenMax) {
                strT = uT("...");
                strT += strTT.Mid(iLen - iLenMax);
            }
            else {
                strT = strTT;
            }
        }
        return strT;
    }

    FindDirDlg *getFindDirDlg(void)
    {
        return m_pFindDirDlg;
    }

    bool isClosingAll(void)
    {
        return m_bClosingAll;
    }

    bool isOpeningFile(void)
    {
        return m_bOpeningFile;
    }

    bool initAnalyze(void);
    void initInterface(void);
    void initMenu(void);
    void initToolbar(void);
    void initStatusbar(void);
    void initDrop(void);
    bool dropFiles(const wxArrayString &filenames);

    void createSearchToolbar(void);
    void viewSearchToolbar(void);

    void updateStatusbarPos(int nLine, int nCol);
    void updateStatusbarIns(bool bIns);
    void updateStatusbarEOL(int iEOL);

    void updateCodeSample(void);

    void loadScript(wxString strFilename);
    void restoreConsoleHistory(void)
    {
        if (m_pEditConsole) {
            m_pEditConsole->restoreHistory(static_cast<const char_t *>(m_szSigmaConfigDir));
        }
    }
    void initConsolePrefs(void)
    {
        if (m_pEditConsole) {
            m_pEditConsole->setPrefs(m_ScintillaPrefs, true);
        }
    }
    void initOutputPrefs(void)
    {
        if (m_pEditOutput) {
            m_pEditOutput->setPrefs(m_ScintillaPrefs, true);
        }
    }
    void initToolXPrefs(void)
    {
        initToolPrefs(m_ScintillaPrefs);
    }
    void initToolPrefs(ScintillaPrefs &tScintillaPrefs);

    CodeInfobar *getInfobarMain(void)
    {
        return m_pInfobarMain;
    }
    CodeInfobar *getInfobarTool(void)
    {
        return m_pInfobarTool;
    }

    bool getSilent(void)
    {
        return m_bSilent;
    }

    void setSilent(bool bSilent)
    {
        m_bSilent = bSilent;
    }

    static void enableUIitem(wxUpdateUIEvent &tEvent, bool bEnable);

    ScriptEdit *getActiveEditor(void)
    {
        if (m_pNotebookMain == NULL) {
            return NULL;
        }

        int iPageCount = (int)(m_pNotebookMain->GetPageCount());

        if (iPageCount < 1) {
            // should never happen
            return NULL;
        }

        int iSel = 0;

        iSel = m_pNotebookMain->GetSelection();
        if ((iSel < 0) || (iSel >= iPageCount)) {
            return NULL;
        }

        ScriptEdit *pEdit = getEditor(iSel);
        if (pEdit == NULL) {
            return NULL;
        }

        return pEdit;
    }

    bool canUpdateUI(double fInterval)
    {
        struct timeval timevalNow;
        gettimeofday(&timevalNow, NULL);
        double fDiff = (((double)(timevalNow.tv_sec - m_UItimeval.tv_sec)) * 1000.0) + (((double)(timevalNow.tv_usec - m_UItimeval.tv_usec)) / 1000.0);
        if (fDiff > fInterval) {
            memcpy(&m_UItimeval, &timevalNow, sizeof(struct timeval));
            return true;
        }
        return false;
    }

    int getActiveEditorIndex(void)
    {
        if (m_pNotebookMain == NULL) {
            return -1;
        }

        int iCount = (int)(m_pNotebookMain->GetPageCount());

        int iSel = m_pNotebookMain->GetSelection();
        if ((iSel < 0) || (iSel >= iCount)) {
            return -1;
        }

        return iSel;
    }

    bool addEditor(ScriptEdit *pEdit, const wxString &strFilename, int *piIndex = NULL, bool bFocus = true);

    ScriptEdit *getEditor(int ii)
    {
        if (m_pNotebookMain == NULL) {
            return NULL;
        }

        int iCount = (int)(m_pNotebookMain->GetPageCount());

        if ((ii < 0) || (ii >= iCount)) {
            return NULL;
        }

        return static_cast<ScriptEdit *>(m_pNotebookMain->GetPage(ii));
    }

    ScriptEdit *getEditor(const wxString &strT)
    {
        if (m_pNotebookMain == NULL) {
            return NULL;
        }

        int iCount = (int)(m_pNotebookMain->GetPageCount());

        if (iCount < 1) {
            return NULL;
        }

        int iSel = m_pNotebookMain->GetSelection();

        if ((iSel < 0) || (iSel >= iCount)) {
            return NULL;
        }

        // Firstly check the active editor
        if (m_pNotebookMain->GetPageText(iSel).IsSameAs(strT)) {
            return getEditor(iSel);
        }

        for (int ii = 0; ii < iCount; ii++) {
            if (ii == iSel) {
                continue;
            }
            if (m_pNotebookMain->GetPageText(ii).IsSameAs(strT)) {
                // NB: two pages can have the same title (same filename but, of course, different directories).
                return getEditor(ii);
            }
        }

        return NULL;
    }

    void setActiveEditor(int iSel, bool bFocus = true, bool bMakeVisible = true);

    int getEditorCount(void)
    {
        if (m_pNotebookMain == NULL) {
            return 0;
        }

        return (int)(m_pNotebookMain->GetPageCount());
    }

    wxAuiNotebook *getNotebookMain(void)
    {
        return m_pNotebookMain;
    }

    wxBitmap *getBitmapNone(void)
    {
        return &m_bmpNone;
    }

    void updateEditorStatus(ScriptEdit *pEdit)
    {
        if ((m_pNotebookMain == NULL) || (pEdit == NULL)) {
            return;
        }

        int iCount = (int)(m_pNotebookMain->GetPageCount());
        int indexT = m_pNotebookMain->GetPageIndex(pEdit);
        if ((indexT < 0) || (indexT >= iCount)) {
            return;
        }

        wxBitmap *pBmp = NULL;

        if (pEdit->isRunning()) {
            pBmp = pEdit->isDebugging() ? &m_bmpDebugging : &m_bmpRunning;
        }
        else {
            if (pEdit->isFindin()) {
                pBmp = &m_bmpFindin;
                pEdit->setFindin(false);
            }
            else if (pEdit->GetReadOnly()) {
                pBmp = &m_bmpReadOnly;
            }
            else if (pEdit->isPinned()) {
                pBmp = &m_bmpPinned;
            }
            else if (pEdit->autoReload()) {
                pBmp = &m_bmpAutoReload;
            }
            else {
                pBmp = &m_bmpNone;
            }
        }

        if (pBmp != NULL) {
            if (m_pNotebookMain->GetPageBitmap(indexT).IsSameAs(*pBmp) == false) {
                m_pNotebookMain->SetPageBitmap(indexT, *pBmp);
            }
        }
    }

    bool saveNavigation(void)
    {
        return m_SigmaGUIPrefs.bSaveNavigation;
    }
    bool viewInfobar(void)
    {
        return m_SigmaGUIPrefs.bViewInfobar;
    }

    SigmaGUIPrefs *getSigmaGUIPrefs(void)
    {
        return &m_SigmaGUIPrefs;
    }

    bool isConsoleVisible(void)
    {
        if (NULL == m_pNotebookToolbox) {
            return false;
        }

        wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pNotebookToolbox);
        if ((NULL == m_pEditConsole) || (false == paneT.IsShown())) {
            return false;
        }

        return (1 == m_pNotebookToolbox->GetSelection());
    }

    ConsoleEdit *getConsole(void)
    {
        return m_pEditConsole;
    }

    OutputEdit *getOutput(void)
    {
        return m_pEditOutput;
    }

    bool isOutputVisible(void)
    {
        if ((NULL == m_pNotebookToolbox) || (NULL == m_pEditOutput)) {
            return false;
        }

        wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pNotebookToolbox);
        if (false == paneT.IsShown()) {
            return false;
        }

        return (0 == m_pNotebookToolbox->GetSelection());
    }

    void setTabDlg(wxDialog *dlgTab)
    {
        m_dlgTab = dlgTab;
    }

    void setToolsDlg(wxDialog *dlgTools)
    {
        m_dlgTools = dlgTools;
    }

    void setColorsDlg(wxDialog *dlgColors)
    {
        m_dlgColors = dlgColors;
    }

    void setLexerDlg(wxDialog *dlgLexer)
    {
        m_dlgLexer = dlgLexer;
    }

    bool resetBookmarkArray(void);
    bool deleteBookmark(int posT);
    bool deleteBookmark(const wxString &strFilename, int iLine, long iId);
    bool deleteBookmarkAll(void);
    bool canAddBookmark(void);
    void updateBookmark(void);
    void updateBookmarks(long iId, const char_t *pszFilename);
    bookmark_t *addBookmark(const wxString &strFilename, int iLine, const wxString &strInfo, long iId);
    bookmark_t *getBookmark(int posT);
    int getBookmarkCount(void);
    void sortBookmark(int iColumn, bool bAsc = true);

    bool ensureOutputVisible(void);
    void Output(const wxString &strT, bool bAddTime = false, bool bShowMessage = false, wxWindow *pParent = NULL, bool bForce = false, bool bToErrPos = false);
    void setRunningLexer(int iRunningLexer)
    {
        if (m_pEditOutput) {
            m_pEditOutput->setRunningLexer(iRunningLexer);
        }
    }
    int getErrLine(void)
    {
        if (m_pEditOutput) {
            return m_pEditOutput->getErrLine();
        }
        return -1;
    }
    long getErrId(void)
    {
        if (m_pEditOutput) {
            return m_pEditOutput->getErrId();
        }
        return 0L;
    }
    void setErrId(long iId)
    {
        if (m_pEditOutput) {
            m_pEditOutput->setErrId(iId);
        }
    }
    wxString getErrInfo(void)
    {
        if (m_pEditOutput) {
            return m_pEditOutput->getErrInfo();
        }
        return wxEmptyString;
    }
    wxString getLaTeXFilename(void)
    {
        if (m_pEditOutput) {
            return m_pEditOutput->getLaTeXFilename();
        }
        return wxEmptyString;
    }
    wxString getLaTeXFullFilename(void)
    {
        if (m_pEditOutput) {
            return m_pEditOutput->getLaTeXFullFilename();
        }
        return wxEmptyString;
    }
    void gotoErrPos(void)
    {
        if (m_pEditOutput) {
            m_pEditOutput->gotoErrPos();
        }
    }
    void printLaTeXReport(int iExitStatus, wxString strFilename)
    {
        if (m_pEditOutput) {
            m_pEditOutput->printLaTeXReport(iExitStatus, strFilename);
        }
    }
    void clearOutput(void);
    bool isOutputLineEmpty(void);

    void OutputStatusbar(const wxString &strT, int iMilliseconds = SIGMAFRAME_TIMER_SHORT);
    void OnTimer(wxTimerEvent &tEvent);
    bool isStatusbarEmpty(void);

    void OnCheckUpdate(wxCommandEvent &tEvent);
    void OnCheckUpdateEnd(wxCommandEvent &tEvent);

    void DoSetStatusbarText(const wxString &strT, int iNum);
    void updateStatusbarLexer(const wxString &strT);

    void UpdateDebugNotebook(const wxString &strFilename, const wxString &strT);
    bool GetDebugSymbol(const wxString &strN, wxString &strV);
    void ClearDebugWindow(const wxString &strFilename);

    void ClearOutputWindow(const wxString &strFilename, bool bCls)
    {
        if (m_pNotebookToolbox == NULL) {
            return;
        }

        if (m_pToolPanel && m_pNotebookToolbox && m_pEditOutput) {
            wxString strT = uT("Tool Window");
            if (strFilename.IsEmpty() == false) {
                strT += uT(" - ");
                strT += strFilename;
            }
            wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pToolPanel);
            if (paneT.IsOk()) {
                paneT.Caption(strT);
                m_auiManager.Update();
            }
        }

        if (bCls) {
            clearOutput();
        }
    }

    void setLastDir(const char_t *szLastDir)
    {
        wxStrcpy(m_SigmaCommonPrefs.szLastDir, szLastDir);
        addToRecentDir(szLastDir);
    }

    const char_t *getSigmaConfigDir(void) const
    {
        return static_cast<const char_t *>(m_szSigmaConfigDir);
    }

    bool detectChangeOutside(void)
    {
        return m_SigmaCommonPrefs.bDetectChangeOutside;
    }

    bool isFindMarkerEnabled(void)
    {
        return m_SigmaCommonPrefs.bFindMarker;
    }

    bool isFindCyclicEnabled(void)
    {
        return m_SigmaCommonPrefs.bFindCyclic;
    }

    void enableFindMarker(bool bEnable)
    {
        m_SigmaCommonPrefs.bFindMarker = bEnable;
    }

    void enableFindCyclic(bool bEnable)
    {
        m_SigmaCommonPrefs.bFindCyclic = bEnable;
    }

    CodeSample *getExample(void)
    {
        return m_pExample;
    }

    CodeSample *getSnippet(void)
    {
        return m_pSnippet;
    }

    void updateFindValue(const wxString &strFind)
    {
        if ((m_cboAnalyzer != NULL) && (strFind.IsEmpty() == false)) {
            m_cboAnalyzer->SetValue(strFind);
        }
    }

    void updateFindFilePos(void)
    {
        if (m_pFindFileDlg) {
            m_SigmaGUIPrefs.iFindFilePosX = m_pFindFileDlg->GetPosition().x;
            m_SigmaGUIPrefs.iFindFilePosY = m_pFindFileDlg->GetPosition().y;
        }
    }

    void updateFindDirPos(void)
    {
        if (m_pFindDirDlg) {
            m_SigmaGUIPrefs.iFindDirPosX = m_pFindDirDlg->GetPosition().x;
            m_SigmaGUIPrefs.iFindDirPosY = m_pFindDirDlg->GetPosition().y;
            m_SigmaGUIPrefs.iFindDirWidth = m_pFindDirDlg->GetSize().GetX();
            m_SigmaGUIPrefs.iFindDirHeight = m_pFindDirDlg->GetSize().GetY();
        }
    }

    int getFindCount(void);
    void resetFind(void);
    wxString getFindItem(int iItem);
    wxString getCurrentFindItem(void);
    int addFindItem(const wxString &strFind);
    int getReplaceCount(void);
    wxString getReplaceItem(int iItem);
    wxString getCurrentReplaceItem(void);
    int addReplaceItem(const wxString &strReplace);
    int getDirCount(void);
    wxString getDirItem(int iItem);
    wxString getCurrentDirItem(void);
    int addDirItem(const wxString &strDir);

    void updateTitle(const wxString &strFilename);

    ScriptEdit *getEditorByShortFilename(const wxString &shortFilename);
    bool selectEditor(ScriptEdit *pEdit);

    bool isFileOpened(const wxString &filenameT);

    int fileToClose(int iSel);

    ScriptEdit *DoFileNew(wxPoint posT, wxSize sizeT, const wxString &strTemplate, int iLexer, int iLexerVar);
    ScriptEdit *fileNew(const wxString &strTemplate, int iLexer, int iLexerVar);
    ScriptEdit *fileOpen(const wxString &filenameT = wxEmptyString, bool bRun = false,
                         long iLine = -1L, const wxString &strFind = wxEmptyString, const wxString &strReplace = wxEmptyString,
                         bool bOpenRecent = false);
    ScriptEdit *pageOpen(long iId, long iLine = -1L, bool bFocus = true);
    bool fileSave(int iPage = -1);
    bool fileSaveAs(void);
    bool filePrint(void);
    bool filePrintPreview(void);
    bool fileClose(ScriptEdit *pEdit);
    bool fileExit(void);

    bool scriptStart(void);
    bool scriptStop(void);
    bool scriptOptions(void);

    bool toolStop(void);

    void CancelCallTip(void);

    void addToRecent(const wxString &strFilename);
    void reopenRecent(void);
    void selectRecent(void);
    void addToRecentDir(const wxString &strDirname);

    void explorerChdir(void);
    bool explorerIsOpened(const wxString &strOldPath, bool bActivate);
    bool explorerOnRename(bool bDir, const wxString &strOldPath, const wxString &strNewPath);
    bool explorerOnDelete(const wxString &strPath);
    wxString explorerGetCurrentDir(void);

    void DoFindPrevNext(int idT);
    void DoUpdateExplorer(void);

    void OnSize(wxSizeEvent &tEvent);
    void OnMove(wxMoveEvent &tEvent);
    void OnKillFocus(wxFocusEvent &tEvent);

    void OnFileNew(wxCommandEvent &tEvent);
    void OnFileTemplate(wxCommandEvent &tEvent);
    void OnFileDuplicate(wxCommandEvent &tEvent);
    void OnFileOpen(wxCommandEvent &tEvent);
    void OnFileSave(wxCommandEvent &tEvent);
    void OnFileSaveAs(wxCommandEvent &tEvent);
    void OnFileSaveAll(wxCommandEvent &tEvent);
    void OnFileList(wxCommandEvent &tEvent);
    void OnFileListSelect(wxCommandEvent &tEvent);
    void OnFileReload(wxCommandEvent &tEvent);
    void OnFileUpdate(wxCommandEvent &tEvent);
    void OnFileOpenRecent(wxCommandEvent &tEvent);
    void OnFileOpenRecentDir(wxCommandEvent &tEvent);
    void OnFilePrint(wxCommandEvent &tEvent);
    void OnFilePrintPreview(wxCommandEvent &tEvent);
    void OnFileExit(wxCommandEvent &tEvent);

    void OnNotebookFileList(wxAuiNotebookEvent &tEvent);

    wxPoint GetMouseNotebookPos(wxAuiNotebook *pNotebook);
    wxPoint GetMouseNotebookPosBottom(wxAuiNotebook *pNotebook);
    wxPoint GetMouseToolbarPos(void);

    void OnCodeFormat(wxCommandEvent &tEvent);
    void OnUpdateCodeFormat(wxUpdateUIEvent &tEvent);

    void OnCodeSnippet(wxCommandEvent &tEvent);
    void OnUpdateCodeSnippet(wxUpdateUIEvent &tEvent);

    void OnBookmarkList(wxCommandEvent &tEvent);
    void OnUpdateBookmarkList(wxUpdateUIEvent &tEvent);
    void OnBookmarkListGoto(wxCommandEvent &tEvent);

    void OnFileEnforceLexer(wxCommandEvent &tEvent);
    void OnUpdateLexer(wxUpdateUIEvent &tEvent);

    wxMenuItem *rebuildMenuItem(wxMenuItem *pMnuItem, const wxBitmap &tBmp, const wxBitmap &tBmpD);
    void OnUpdateRecent(wxUpdateUIEvent &tEvent);
    void OnUpdateRecentDir(wxUpdateUIEvent &tEvent);
    void OnUpdateExample(wxUpdateUIEvent &tEvent);
    void OnUpdateSnippet(wxUpdateUIEvent &tEvent);

    void DoScriptColors(void);
    void OnScriptColors(wxCommandEvent &tEvent);

    void DoScriptLexer(void);
    void OnScriptLexer(wxCommandEvent &tEvent);

    void DoScriptTab(void);
    void OnScriptTab(wxCommandEvent &tEvent);

    void DoViewInfobar(bool bViewInfobar);

    void DoMainAction(int idT);
    void OnMainAction(wxCommandEvent &tEvent);
    void OnUpdateMainAction(wxUpdateUIEvent &tEvent);
    void OnEditAction(wxCommandEvent &tEvent);
    void OnScriptAction(wxCommandEvent &tEvent);

    void OnInfobarAction(wxCommandEvent &tEvent);

    void OnViewTopmost(wxCommandEvent &tEvent);
    void OnUpdateTopmost(wxUpdateUIEvent &tEvent);
    void OnViewResetUI(wxCommandEvent &tEvent);

    void DoViewFileExplorer(bool bViewOnly);
    void OnView(wxCommandEvent &tEvent);

    void OnFilesysContextMenu(wxAuiNotebookEvent &tEvent);
    void OnFilesysClose(wxAuiNotebookEvent &tEvent);

    void OnToolboxChange(wxAuiNotebookEvent &tEvent);
    void OnToolboxContextMenu(wxAuiNotebookEvent &tEvent);
    void OnToolboxClose(wxAuiNotebookEvent &tEvent);

    void OnDebugClose(wxAuiNotebookEvent &tEvent);

    void OnDebugHeaderClick(wxListEvent &tEvent);

    void OnBookmarkDeleteAll(wxCommandEvent &tEvent);
    void OnBookmarkRefresh(wxCommandEvent &tEvent);
    void OnBookmarkFileDeleteAll(wxCommandEvent &tEvent);

    void OnFontChoose(wxCommandEvent &tEvent);

    void OnScriptStart(wxCommandEvent &tEvent);
    void OnScriptStop(wxCommandEvent &tEvent);
    void OnScriptStepInto(wxCommandEvent &tEvent);
    void OnScriptStartDebug(wxCommandEvent &tEvent);

    void OnScriptKillTool(wxCommandEvent &tEvent);

    void OnScriptOptions(wxCommandEvent &tEvent);

    void OnToolsBuild(wxCommandEvent &tEvent);

    void updateRecentFileMenu(wxMenu *pMenu, wxMenuItem *pMenuItem);
    void updateRecentDirMenu(wxMenu *pMenu, wxMenuItem *pMenuItem);

    void updateOpenFileMenu(wxMenu *pMenu, wxMenuItem *pMenuItem, int *piTextWidth);
    void updateBookmarkMenu(wxMenu *pMenu, wxMenuItem *pMenuItem);
    void updateCurrentFileMenu(wxMenu *pMenu, int iSel);

    bool DoScriptChange(int iPrevSel);

    void DoScriptCloseAll(int iExcept, bool bAllButPinned);

    void OnPaneClose(wxAuiManagerEvent &tEvent);
    void OnScriptChange(wxAuiNotebookEvent &tEvent);
    void OnScriptClose(wxAuiNotebookEvent &tEvent);
    void OnScriptContextMenu(wxAuiNotebookEvent &tEvent);
    void OnScriptCloseAll(wxCommandEvent &tEvent);
    void OnScriptCloseX(wxCommandEvent &tEvent);
    void OnScriptCloseOthers(wxCommandEvent &tEvent);
    void OnScriptCloseAllButPinned(wxCommandEvent &tEvent);
    void OnScriptReopenLast(wxCommandEvent &tEvent);
    void OnScriptNewWindow(wxCommandEvent &tEvent);
    void OnScriptCopyPath(wxCommandEvent &tEvent);
    void OnScriptOpenDir(wxCommandEvent &tEvent);
    void OnScriptSetDir(wxCommandEvent &tEvent);
    void OnScriptOpenShell(wxCommandEvent &tEvent);
    void OnScriptFileProps(wxCommandEvent &tEvent);
    void OnScriptFileStats(wxCommandEvent &tEvent);
    void OnScriptFileReadOnly(wxCommandEvent &tEvent);

    void OnOpenExamplesDir(wxCommandEvent &tEvent);

    void OnHelpContentsPdf(wxCommandEvent &tEvent);
    void OnHelpExample(wxCommandEvent &tEvent);
    void OnInsertCodeSnippet(wxCommandEvent &tEvent);
    void OnHelpAbout(wxCommandEvent &tEvent);

    bool AnalyzerIsFindSelected(void);
    int DoAnalyzerUpdateList(ScriptEdit *pEdit, bool bRecreate);
    int DoAnalyzerUpdate(bool bRecreate, bool bVerbose = true);
    void DoAnalyzerChange(void);
    void DoAnalyzerChange(const wxString &strFunc, bool bSelect = false);
    void OnAnalyzerUpdate(wxCommandEvent &tEvent);
    void OnAnalyzerGoto(wxCommandEvent &tEvent);
    void OnAnalyzerReset(wxCommandEvent &tEvent);
    void OnAnalyzerToolbarUpdate(wxUpdateUIEvent &tEvent);
    void OnAnalyzerChange(wxCommandEvent &tEvent);

    void OnUpdateBookmark(wxUpdateUIEvent &tEvent);
    void OnUpdateBreakpoint(wxUpdateUIEvent &tEvent);
    void OnUpdateEditorOptions(wxUpdateUIEvent &tEvent);
    void OnUpdateEditorAction(wxUpdateUIEvent &tEvent);
    void OnUpdateModifyAction(wxUpdateUIEvent &tEvent);

    void OnUpdateFileSave(wxUpdateUIEvent &tEvent);
    void OnUpdateUndoRedo(wxUpdateUIEvent &tEvent);
    void OnUpdateCopyPaste(wxUpdateUIEvent &tEvent);
    void DoUpdateStartButton(void);
    void OnUpdateStartStop(wxUpdateUIEvent &tEvent);
    void OnUpdateFileList(wxUpdateUIEvent &tEvent);

    void OnUpdateKillTool(wxUpdateUIEvent &tEvent);

    void OnUpdateFileReload(wxUpdateUIEvent &tEvent);

    void OnUpdateScriptClose(wxUpdateUIEvent &tEvent);
    void OnUpdateScriptPath(wxUpdateUIEvent &tEvent);

    void OnUpdateScriptReopenLast(wxUpdateUIEvent &tEvent);

    void OnDoubleClick(wxMouseEvent &tEvent);

    void OnClose(wxCloseEvent &tEvent);

    DECLARE_EVENT_TABLE()
};

#endif
