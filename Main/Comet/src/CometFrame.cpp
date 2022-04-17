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
#include "ColorButton.h"
#include "ColorsDlg.h"
#include "TabDlg.h"
#include "ToolsDlg.h"
#include "LexerDlg.h"

#include <wx/splitter.h>
#include <wx/aboutdlg.h>
#include <wx/fontdlg.h>
#include <wx/filename.h>
#include <wx/clipbrd.h>
#include <wx/fileconf.h>
#include <wx/wupdlock.h>

#if !defined(__WXMSW__)
#include <sys/sendfile.h>
#include <utime.h>
#endif

BEGIN_EVENT_TABLE(CometFrame, wxFrame)

    EVT_SIZE(CometFrame::OnSize)
    EVT_MOVE(CometFrame::OnMove)
    EVT_KILL_FOCUS(CometFrame::OnKillFocus)

    EVT_MENU(ID_FILE_NEW, CometFrame::OnFileNew)
    EVT_MENU_RANGE(ID_FILE_TEMPLATE01, ID_FILE_TEMPLATELAST, CometFrame::OnFileTemplate)
    EVT_MENU(ID_FILE_OPEN, CometFrame::OnFileOpen)

    EVT_MENU_RANGE(ID_FILE_LEXER01, ID_FILE_LEXERLAST, CometFrame::OnFileEnforceLexer)
    EVT_UPDATE_UI_RANGE(ID_FILE_LEXER01, ID_FILE_LEXERLAST, CometFrame::OnUpdateLexer)

    EVT_UPDATE_UI(ID_FILE_SAVE, CometFrame::OnUpdateFileSave)
    EVT_UPDATE_UI(ID_FILE_SAVEALL, CometFrame::OnUpdateFileSave)
    EVT_MENU(ID_FILE_SAVE, CometFrame::OnFileSave)
    EVT_MENU(ID_FILE_SAVEAS, CometFrame::OnFileSaveAs)
    EVT_MENU(ID_FILE_SAVEALL, CometFrame::OnFileSaveAll)
    EVT_MENU(ID_FILE_FILELIST, CometFrame::OnFileList)
    EVT_UPDATE_UI(ID_FILE_FILELIST, CometFrame::OnUpdateFileList)
    EVT_MENU_RANGE(ID_FILE_LIST01, ID_FILE_LISTLAST, CometFrame::OnFileListSelect)

    EVT_MENU(ID_SCRIPT_BOOKMARK_LIST, CometFrame::OnBookmarkList)
    EVT_UPDATE_UI(ID_SCRIPT_BOOKMARK_LIST, CometFrame::OnUpdateBookmarkList)
    EVT_MENU_RANGE(ID_SCRIPT_BOOKMARK_LIST_FIRST, ID_SCRIPT_BOOKMARK_LIST_LAST, CometFrame::OnBookmarkListGoto)

    EVT_MENU(ID_FILE_RELOAD, CometFrame::OnFileReload)
    EVT_UPDATE_UI(ID_FILE_RELOAD, CometFrame::OnUpdateFileReload)

    EVT_MENU(ID_FILE_UPDATE, CometFrame::OnFileUpdate)

    EVT_MENU(ID_FILE_DUPLICATE, CometFrame::OnFileDuplicate)

    EVT_MENU(wxID_CUT, CometFrame::OnEditAction)
    EVT_MENU(wxID_COPY, CometFrame::OnEditAction)
    EVT_MENU(wxID_PASTE, CometFrame::OnEditAction)
    EVT_MENU(wxID_CLEAR, CometFrame::OnEditAction)
    EVT_MENU(wxID_SELECTALL, CometFrame::OnEditAction)

    EVT_MENU(ID_MAIN_SELECTWORD, CometFrame::OnEditAction)
    EVT_MENU(ID_MAIN_SELECTLINE, CometFrame::OnEditAction)
    EVT_MENU(ID_MAIN_UNDO, CometFrame::OnEditAction)
    EVT_MENU(ID_MAIN_REDO, CometFrame::OnEditAction)
    EVT_MENU(ID_MAIN_INSERT_DATE, CometFrame::OnEditAction)
    EVT_MENU(ID_MAIN_INSERT_TIME, CometFrame::OnEditAction)
    EVT_MENU(ID_MAIN_INSERT_DATETIME, CometFrame::OnEditAction)

    EVT_MENU(ID_CODE_FORMAT, CometFrame::OnCodeFormat)
    EVT_UPDATE_UI(ID_CODE_FORMAT, CometFrame::OnUpdateCodeFormat)

    EVT_MENU(ID_CODE_SNIPPET, CometFrame::OnCodeSnippet)
    EVT_UPDATE_UI(ID_CODE_SNIPPET, CometFrame::OnUpdateCodeSnippet)

    EVT_MENU(ID_MAIN_ANALYZE, CometFrame::OnAnalyzerUpdate)
    EVT_MENU(ID_MAIN_GOTO, CometFrame::OnAnalyzerGoto)
    EVT_COMBOBOX(IDC_ANALYZE_LIST, CometFrame::OnAnalyzerChange)
    EVT_UPDATE_UI(ID_MAIN_ANALYZE, CometFrame::OnAnalyzerToolbarUpdate)

    EVT_UPDATE_UI_RANGE(wxID_CUT, wxID_CLEAR, CometFrame::OnUpdateCopyPaste)
    EVT_UPDATE_UI_RANGE(ID_MAIN_UNDO, ID_MAIN_REDO, CometFrame::OnUpdateUndoRedo)

    EVT_MENU_RANGE(ID_FILE_RECENT01, ID_FILE_RECENTCLEAR, CometFrame::OnFileOpenRecent)
    EVT_UPDATE_UI_RANGE(ID_FILE_RECENT01, ID_FILE_RECENTCLEAR, CometFrame::OnUpdateRecent)

    EVT_MENU_RANGE(ID_DIR_RECENT01, ID_DIR_RECENTCLEAR, CometFrame::OnFileOpenRecentDir)
    EVT_UPDATE_UI_RANGE(ID_DIR_RECENT01, ID_DIR_RECENTCLEAR, CometFrame::OnUpdateRecentDir)

    EVT_MENU(ID_OUTPUT_RESET, CometFrame::OnMainAction)

    EVT_MENU(ID_EXPLORER_BROWSE, CometFrame::OnMainAction)
    EVT_MENU(ID_EXPLORER_REFRESH, CometFrame::OnMainAction)
    EVT_MENU(ID_EXPLORER_OPEN, CometFrame::OnMainAction)
    EVT_MENU(ID_EXPLORER_RENAME, CometFrame::OnMainAction)
    EVT_MENU(ID_EXPLORER_CUT, CometFrame::OnMainAction)
    EVT_MENU(ID_EXPLORER_COPY, CometFrame::OnMainAction)
    EVT_MENU(ID_EXPLORER_PASTE, CometFrame::OnMainAction)
    EVT_MENU(ID_EXPLORER_DELETE, CometFrame::OnMainAction)
    EVT_MENU(ID_EXPLORER_CREATEFILE, CometFrame::OnMainAction)
    EVT_MENU(ID_EXPLORER_CREATEDIR, CometFrame::OnMainAction)
    EVT_MENU(ID_EXPLORER_COPYPATH, CometFrame::OnMainAction)
    EVT_MENU(ID_EXPLORER_CHANGEDIR, CometFrame::OnMainAction)
#ifdef __WXMSW__
    EVT_MENU(ID_EXPLORER_OPENEXT, CometFrame::OnMainAction)
    EVT_MENU(ID_EXPLORER_OPENSHELL, CometFrame::OnMainAction)
    EVT_MENU(ID_EXPLORER_FILEPROPS, CometFrame::OnMainAction)
#endif

    EVT_MENU(ID_FILE_PRINT, CometFrame::OnFilePrint)
    EVT_MENU(ID_FILE_PRINTPREVIEW, CometFrame::OnFilePrintPreview)
    EVT_MENU(ID_FILE_EXIT, CometFrame::OnFileExit)

    EVT_MENU(ID_SEARCH_GOTO, CometFrame::OnScriptAction)

    EVT_MENU(ID_SEARCH_FIND, CometFrame::OnMainAction)
    EVT_MENU(ID_FIND_PREV, CometFrame::OnMainAction)
    EVT_MENU(ID_FIND_NEXT, CometFrame::OnMainAction)
    EVT_MENU(ID_SEARCH_REPLACE, CometFrame::OnMainAction)
    EVT_MENU(ID_SEARCH_RESETMARKERS, CometFrame::OnMainAction)
    EVT_MENU(ID_SEARCH_FINDINFILES, CometFrame::OnMainAction)
    EVT_MENU(ID_SEARCH_REPLACEINFILES, CometFrame::OnMainAction)
    EVT_MENU(ID_FILE_RESETMARKERS, CometFrame::OnMainAction)
    EVT_MENU(ID_FILE_RESETMARKERSB, CometFrame::OnMainAction)
    EVT_UPDATE_UI(ID_SEARCH_RESETMARKERS, CometFrame::OnUpdateMainAction)
    EVT_UPDATE_UI(ID_FILE_RESETMARKERS, CometFrame::OnUpdateMainAction)
    EVT_UPDATE_UI(ID_FILE_RESETMARKERSB, CometFrame::OnUpdateMainAction)

    EVT_MENU(ID_FILE_PIN, CometFrame::OnMainAction)
    EVT_UPDATE_UI(ID_FILE_PIN, CometFrame::OnUpdateMainAction)

    EVT_UPDATE_UI(ID_VIEW_EXPLORER, CometFrame::OnUpdateEditorOptions)
    EVT_MENU(ID_VIEW_EXPLORER, CometFrame::OnView)
    EVT_MENU(ID_VIEW_CONSOLE, CometFrame::OnView)
    EVT_MENU(ID_VIEW_OUTPUT, CometFrame::OnView)
    EVT_MENU(ID_VIEW_BOOKMARK, CometFrame::OnView)
    EVT_MENU(ID_VIEW_TOOLBAR, CometFrame::OnView)
    EVT_MENU(ID_VIEW_STATUSBAR, CometFrame::OnView)
    EVT_MENU(ID_VIEW_SEARCHBAR, CometFrame::OnView)
    EVT_UPDATE_UI_RANGE(ID_VIEW_EXPLORER, ID_VIEW_SEARCHBAR, CometFrame::OnUpdateEditorOptions)

    EVT_MENU(ID_SCRIPT_COLORS, CometFrame::OnScriptColors)

    EVT_MENU(ID_SCRIPT_LEXER, CometFrame::OnScriptLexer)

    EVT_UPDATE_UI_RANGE(ID_SCRIPT_BOOKMARK_MOD, ID_SCRIPT_BOOKMARK_PREV, CometFrame::OnUpdateBookmark)
    EVT_MENU(ID_SCRIPT_BOOKMARK_MOD, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_BOOKMARK_DELALL, CometFrame::OnBookmarkFileDeleteAll)
    EVT_MENU(ID_SCRIPT_BOOKMARK_NEXT, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_BOOKMARK_PREV, CometFrame::OnScriptAction)
    EVT_UPDATE_UI(ID_BOOKMARK_DELETEALL, CometFrame::OnUpdateBookmark)

    EVT_MENU(ID_SCRIPT_INFOBAR_NEXTX, CometFrame::OnInfobarAction)
    EVT_MENU(ID_SCRIPT_INFOBAR_PREVX, CometFrame::OnInfobarAction)

    EVT_UPDATE_UI_RANGE(ID_SCRIPT_OPTION_FIRST, ID_SCRIPT_OPTION_LAST, CometFrame::OnUpdateEditorOptions)
    EVT_MENU(ID_SCRIPT_SYNTAX, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_AUTOCOMPLETE, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_CALLTIP, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_LINENUMBERS, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_MARGINMARKERS, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_FOLDENABLE, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_WRAPMODE, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_SAVENAVIGATION, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_VIEWINFOBAR, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_DETECTCHANGE, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_DISPLAYEOL, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_WHITESPACE, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_MARGINSTATUS, CometFrame::OnScriptAction)

    EVT_MENU(ID_VIEW_TOPMOST, CometFrame::OnViewTopmost)
    EVT_MENU(ID_VIEW_RESETUI, CometFrame::OnViewResetUI)
    EVT_UPDATE_UI(ID_VIEW_TOPMOST, CometFrame::OnUpdateTopmost)

    EVT_MENU(ID_SCRIPT_ZOOMIN, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_ZOOMOUT, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_ZOOMNONE, CometFrame::OnScriptAction)

    EVT_UPDATE_UI_RANGE(ID_SCRIPT_BREAKPOINT_MOD, ID_SCRIPT_BREAKPOINT_DELALL, CometFrame::OnUpdateBreakpoint)
    EVT_MENU(ID_SCRIPT_BREAKPOINT_MOD, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_BREAKPOINT_DELALL, CometFrame::OnScriptAction)

    EVT_MENU(ID_SCRIPT_LINESPACING_100, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_LINESPACING_125, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_LINESPACING_150, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_LINESPACING_175, CometFrame::OnScriptAction)

    EVT_MENU(ID_SCRIPT_CONVERTCR, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_CONVERTCRLF, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_CONVERTLF, CometFrame::OnScriptAction)
    EVT_UPDATE_UI_RANGE(ID_SCRIPT_CONVERTCR, ID_SCRIPT_CONVERTLF, CometFrame::OnUpdateModifyAction)

    EVT_MENU(ID_SCRIPT_SELECTLINE, CometFrame::OnScriptAction)

    EVT_MENU(ID_SCRIPT_UTF8FROM_ISO8859L1, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_UTF8FROM_ISO8859L9, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_UTF8FROM_CP1252, CometFrame::OnScriptAction)

    EVT_MENU(ID_SCRIPT_INDENTINC, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_INDENTDEC, CometFrame::OnScriptAction)

    // Disable edit actions for readonly document
    EVT_UPDATE_UI_RANGE(ID_MAIN_INSERT_DATE, ID_MAIN_UTF8FROM_CP1252, CometFrame::OnUpdateModifyAction)
    EVT_UPDATE_UI_RANGE(ID_SCRIPT_INSERT_DATE, ID_SCRIPT_UTF8FROM_CP1252, CometFrame::OnUpdateModifyAction)

    EVT_MENU(ID_SCRIPT_COMMENTSELECTION, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_UNCOMMENTSELECTION, CometFrame::OnScriptAction)
    EVT_UPDATE_UI_RANGE(ID_SCRIPT_COMMENTSELECTION, ID_SCRIPT_UNCOMMENTSELECTION, CometFrame::OnUpdateEditorAction)

    // LaTeX stuff
    EVT_MENU(ID_LATEX_TEXTBF, CometFrame::OnScriptAction)
    EVT_MENU(ID_LATEX_TEXTIT, CometFrame::OnScriptAction)
    EVT_MENU(ID_LATEX_TEXTSC, CometFrame::OnScriptAction)
    EVT_MENU(ID_LATEX_TEXTNORMAL, CometFrame::OnScriptAction)
    EVT_MENU(ID_LATEX_MATHRM, CometFrame::OnScriptAction)
    EVT_MENU(ID_LATEX_EMPH, CometFrame::OnScriptAction)
    EVT_MENU(ID_LATEX_TEXTSUPERSCRIPT, CometFrame::OnScriptAction)
    EVT_MENU(ID_LATEX_TEXTSUBSCRIPT, CometFrame::OnScriptAction)
    //

    EVT_MENU(ID_SCRIPT_UPPERCASE, CometFrame::OnScriptAction)
    EVT_MENU(ID_SCRIPT_LOWERCASE, CometFrame::OnScriptAction)
    EVT_UPDATE_UI_RANGE(ID_SCRIPT_UPPERCASE, ID_SCRIPT_LOWERCASE, CometFrame::OnUpdateEditorAction)

    EVT_MENU(ID_SCRIPT_FORMATDOC, CometFrame::OnScriptAction)
    EVT_UPDATE_UI(ID_SCRIPT_FORMATDOC, CometFrame::OnUpdateEditorAction)

    EVT_MENU(ID_SCRIPT_INDENT, CometFrame::OnScriptTab)

    EVT_MENU(ID_SCRIPT_FONT, CometFrame::OnFontChoose)

    EVT_UPDATE_UI_RANGE(ID_SCRIPT_START, ID_SCRIPT_STOP, CometFrame::OnUpdateStartStop)
    EVT_MENU(ID_SCRIPT_START, CometFrame::OnScriptStart)
    EVT_MENU(ID_SCRIPT_STEPINTO, CometFrame::OnScriptStepInto)
    EVT_MENU(ID_SCRIPT_STARTDEBUG, CometFrame::OnScriptStartDebug)
    EVT_MENU(ID_SCRIPT_STOP, CometFrame::OnScriptStop)

    EVT_MENU(ID_TOOLS_BUILD, CometFrame::OnToolsBuild)

    EVT_MENU(ID_SCRIPT_KILLTOOL, CometFrame::OnScriptKillTool)
    EVT_UPDATE_UI(ID_SCRIPT_KILLTOOL, CometFrame::OnUpdateKillTool)

    EVT_AUI_PANE_CLOSE(CometFrame::OnPaneClose)

    EVT_AUINOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK_MAIN, CometFrame::OnScriptChange)
    EVT_AUINOTEBOOK_PAGE_CLOSE(ID_NOTEBOOK_MAIN, CometFrame::OnScriptClose)
    EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(ID_NOTEBOOK_MAIN, CometFrame::OnScriptContextMenu)
    EVT_AUINOTEBOOK_MAINFILELIST(ID_NOTEBOOK_MAIN, CometFrame::OnNotebookFileList)

    EVT_AUINOTEBOOK_PAGE_CLOSE(ID_NOTEBOOK_FILESYS, CometFrame::OnFilesysClose)
    EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(ID_NOTEBOOK_FILESYS, CometFrame::OnFilesysContextMenu)

    EVT_AUINOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK_TOOLBOX, CometFrame::OnToolboxChange)
    EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(ID_NOTEBOOK_TOOLBOX, CometFrame::OnToolboxContextMenu)
    EVT_AUINOTEBOOK_PAGE_CLOSE(ID_NOTEBOOK_TOOLBOX, CometFrame::OnToolboxClose)

    EVT_AUINOTEBOOK_PAGE_CLOSE(ID_NOTEBOOK_DEBUG, CometFrame::OnDebugClose)
    EVT_MENU(ID_BOOKMARK_DELETEALL, CometFrame::OnBookmarkDeleteAll)
    EVT_MENU(ID_BOOKMARK_REFRESH, CometFrame::OnBookmarkRefresh)
    EVT_LIST_COL_CLICK(IDL_DEBUG_STACK, CometFrame::OnDebugHeaderClick)
    EVT_LIST_COL_CLICK(IDL_DEBUG_WATCH, CometFrame::OnDebugHeaderClick)

    EVT_MENU(ID_FILE_CLOSE, CometFrame::OnScriptCloseX)
    EVT_MENU(ID_FILE_CLOSEALL, CometFrame::OnScriptCloseAll)
    EVT_MENU(ID_FILE_CLOSEOTHERS, CometFrame::OnScriptCloseOthers)
    EVT_MENU(ID_FILE_CLOSEALLBUTPINNED, CometFrame::OnScriptCloseAllButPinned)
    EVT_MENU(ID_FILE_REOPENLAST, CometFrame::OnScriptReopenLast)
    EVT_MENU(ID_FILE_NEWWINDOW, CometFrame::OnScriptNewWindow)
    EVT_MENU(ID_FILE_COPYPATH, CometFrame::OnScriptCopyPath)
    EVT_MENU(ID_FILE_OPENDIR, CometFrame::OnScriptOpenDir)
    EVT_MENU(ID_FILE_SETDIR, CometFrame::OnScriptSetDir)
    EVT_MENU(ID_FILE_OPENSHELL, CometFrame::OnScriptOpenShell)
    EVT_MENU(ID_FILE_FILEPROPS, CometFrame::OnScriptFileProps)
    EVT_MENU(ID_FILE_FILESTATS, CometFrame::OnScriptFileStats)
    EVT_MENU(ID_FILE_READONLY, CometFrame::OnScriptFileReadOnly)
    EVT_UPDATE_UI(ID_FILE_CLOSE, CometFrame::OnUpdateScriptClose)
    EVT_UPDATE_UI(ID_FILE_CLOSEALL, CometFrame::OnUpdateScriptClose)
    EVT_UPDATE_UI(ID_FILE_CLOSEOTHERS, CometFrame::OnUpdateScriptClose)
    EVT_UPDATE_UI(ID_FILE_CLOSEALLBUTPINNED, CometFrame::OnUpdateScriptClose)
    EVT_UPDATE_UI(ID_FILE_REOPENLAST, CometFrame::OnUpdateScriptReopenLast)
    EVT_UPDATE_UI(ID_FILE_DUPLICATE, CometFrame::OnUpdateScriptPath)
    EVT_UPDATE_UI(ID_FILE_NEWWINDOW, CometFrame::OnUpdateScriptPath)
    EVT_UPDATE_UI(ID_FILE_COPYPATH, CometFrame::OnUpdateScriptPath)
    EVT_UPDATE_UI(ID_FILE_OPENDIR, CometFrame::OnUpdateScriptPath)
    EVT_UPDATE_UI(ID_FILE_SETDIR, CometFrame::OnUpdateScriptPath)
    EVT_UPDATE_UI(ID_FILE_OPENSHELL, CometFrame::OnUpdateScriptPath)
    EVT_UPDATE_UI(ID_FILE_FILEPROPS, CometFrame::OnUpdateScriptPath)
    EVT_UPDATE_UI(ID_FILE_FILESTATS, CometFrame::OnUpdateScriptPath)
    EVT_UPDATE_UI(ID_FILE_READONLY, CometFrame::OnUpdateScriptPath)

    EVT_MENU(ID_FILE_OPENEXAMPLESDIR, CometFrame::OnOpenExamplesDir)

    EVT_LEFT_DCLICK(CometFrame::OnDoubleClick)

    EVT_MENU(ID_HELP_CONTENTS_PDF, CometFrame::OnHelpContentsPdf)
    EVT_MENU_RANGE(ID_SCRIPT_EXAMPLE_FIRST, ID_SCRIPT_EXAMPLE_LAST, CometFrame::OnHelpExample)
    EVT_UPDATE_UI_RANGE(ID_SCRIPT_EXAMPLE_FIRST, ID_SCRIPT_EXAMPLE_LAST, CometFrame::OnUpdateExample)
    EVT_MENU_RANGE(ID_SCRIPT_SNIPPET_FIRST, ID_SCRIPT_SNIPPET_LAST, CometFrame::OnInsertCodeSnippet)
    EVT_UPDATE_UI_RANGE(ID_SCRIPT_SNIPPET_FIRST, ID_SCRIPT_SNIPPET_LAST, CometFrame::OnUpdateSnippet)
    EVT_MENU(ID_HELP_CHECKUPDATE, CometFrame::OnCheckUpdate)
    EVT_MENU(ID_HELP_ABOUT, CometFrame::OnHelpAbout)

    EVT_TIMER(TIMER_ID_SIGMAFRAME, CometFrame::OnTimer)

    EVT_COMMAND(ID_THREAD_CHECKUPDATE, wxEVT_COMMAND_TEXT_UPDATED, CometFrame::OnCheckUpdateEnd)

    EVT_CLOSE(CometFrame::OnClose)

END_EVENT_TABLE()

static bool isDirectoryEmpty(const char *dirname)
{
#if defined(__WXMSW__)
    WIN32_FIND_DATAA fdFile;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    char szPath[256];
    sprintf(szPath, "%s\\*.*", dirname);
    if ((hFile = FindFirstFileA(szPath, &fdFile)) == INVALID_HANDLE_VALUE) {
        return false;
    }
    int nfiles = 0;
    while (FindNextFileA(hFile, &fdFile)) {
        if (++nfiles > 2) {
            break;
        }
    }
    FindClose(hFile);
    hFile = INVALID_HANDLE_VALUE;
    return (nfiles <= 2) ? true : false;
#else
    struct dirent *d;
    DIR *tdir = opendir(dirname);
    if (tdir == NULL) {
        return true;
    }
    int nfiles = 0;
    while ((d = readdir(tdir)) != NULL) {
        if (++nfiles > 2) {
            break;
        }
    }
    closedir(tdir);
    tdir = NULL;
    return (nfiles <= 2) ? true : false;
#endif
}

#if defined(__WXMSW__)
int copyfile(const char *pszFrom, const char *pszTo)
{
    BOOL bRet = ::CopyFileA(pszFrom, pszTo, FALSE);
    return bRet ? 0 : -1;
}
#else
int copyfile(const char *pszFrom, const char *pszTo)
{
    struct stat stbuf;
    int fdFrom, fdTo;
    off_t ifrom = 0, ito = 0;
    struct utimbuf newtimbuf;

    fdFrom = open(pszFrom, O_RDONLY);
    if (fdFrom < 0) {
        return -1;
    }

    if (fstat(fdFrom, &stbuf) != 0) {
        close(fdFrom);
        return -1;
    }
    ifrom = stbuf.st_size;

    newtimbuf.actime = stbuf.st_atime;
    newtimbuf.modtime = stbuf.st_mtime;

    fdTo = open(pszTo, O_WRONLY | O_CREAT);
    if (fdTo < 0) {
        close(fdFrom);
        return -1;
    }

    sendfile(fdTo, fdFrom, &ito, ifrom);

    close(fdFrom);
    close(fdTo);

    utime(pszTo, &newtimbuf);
    chmod(pszTo, stbuf.st_mode | S_IRUSR | S_IROTH | S_IRGRP | S_IWUSR | S_IWOTH | S_IWGRP);

    return 0;
}
#endif

CometFrame::CometFrame(int iWidth, int iHeight) : wxFrame(NULL, wxID_ANY, uT("Comet"), wxDefaultPosition, wxSize(iWidth, iHeight), wxDEFAULT_FRAME_STYLE)
{
    m_bShellStarted = false;
    m_bInitialized = false;
    m_bClosingAll = false;
    m_bClosingSel = false;
    m_bClosed = false;

    m_bOpeningFile = false;

    m_bSavePerspective = true;
    m_bPerspectiveLoaded = false;

    m_bRefreshLayout = false;

    m_pFilesysPanel = NULL;
    m_pNotebookFilesys = NULL;
    m_pListBookmark = NULL;
    m_pExplorerTree = NULL;

    m_pToolPanel = NULL;
    m_pNotebookToolbox = NULL;
    m_pEditOutput = NULL;
    m_pEditConsole = NULL;

    m_pDebugPanel = NULL;
    m_pNotebookDebug = NULL;
    m_pListStack = NULL;
    m_pListWatch = NULL;
    m_iListWatchSorted = CometFrame::LIST_SORTDIRECTION::NONE;
    m_iListStackSorted = CometFrame::LIST_SORTDIRECTION::NONE;

    m_pMenuBar = NULL;
    m_pToolBar = NULL;
    m_pStatusBar = NULL;

    m_pDropTarget = NULL;

    m_cboAnalyzer = NULL;

    m_pFindDirDlg = NULL;
    m_pFindFileDlg = NULL;

    m_pMainPanel = NULL;
    m_pNotebookMain = NULL;

    wxImage::AddHandler(new wxXPMHandler);
    wxImage::AddHandler(new wxPNGHandler);

    m_pFileMenu = NULL;
    m_pSearchMenu = NULL;

    m_pRecentFileMenu = NULL;
    m_pRecentDirMenu = NULL;

    m_pRecentFileMenuItem = NULL;
    m_pRecentDirMenuItem = NULL;
    m_pExamplesMenuItem = NULL;
    m_pSnippetsMenuItem = NULL;

    m_pFormatMenuItem = NULL;
    m_pDateMenuItem = NULL;
    m_pEOLMenuItem = NULL;
    m_pUTF8MenuItem = NULL;

    m_ToolNotebookSize = NOTEBOOK_MINSIZE;
    m_FilesysNotebookSize = NOTEBOOK_MINSIZE;
    m_DebugNotebookSize = NOTEBOOK_MINSIZE;

    memset(m_szLastClosed, 0, LM_STRSIZE * sizeof(char_t));

    memset(m_szSigmaConfigDir, 0, LM_STRSIZE * sizeof(char_t));
    memset(m_szSigmaConfigFilename, 0, LM_STRSIZE * sizeof(char_t));
    memset(m_szEditorConfigFilename, 0, LM_STRSIZE * sizeof(char_t));
    memset(m_szLexerConfigFilename, 0, LM_STRSIZE * sizeof(char_t));
    memset(m_szLayoutConfigFilename, 0, LM_STRSIZE * sizeof(char_t));

    char_t szCnfUsr[LM_STRSIZE];
    Tmemset(szCnfUsr, 0, LM_STRSIZE);
    Tstrncpy(szCnfUsr, LM_CSTR(CometApp::USRDIR), LM_STRSIZE - 1);
#if defined(__WXMSW__)
    Tstrcat(szCnfUsr, uT("config\\"));
#else
    Tstrcat(szCnfUsr, uT("config/"));
#endif
    if (::wxDirExists(LM_CSTR(CometApp::CNFDIR))) {
        Tstrncpy(m_szSigmaConfigDir, LM_CSTR(CometApp::CNFDIR), LM_STRSIZE - 1);
        // if config empty, check the user directory
        if (::wxDirExists((const char_t *)szCnfUsr)) {
            wxString strFrom((const char_t *)szCnfUsr);
            wxString strTo((const char_t *)m_szSigmaConfigDir);
            if ((isDirectoryEmpty(LM_U8STR(strFrom)) == false) && (isDirectoryEmpty(LM_U8STR(strTo)) == true)) {
                const char_t *arCF[] = { uT("common.cometr"), uT("editor.cometr"), uT("layout.cometr"), uT("user.cometu"), NULL };
                for (int ii = 0; arCF[ii] != NULL; ii++) {
                    strFrom = (const char_t *)szCnfUsr;
                    strFrom += arCF[ii];
                    strTo = (const char_t *)m_szSigmaConfigDir;
                    strTo += arCF[ii];
                    copyfile(LM_U8STR(strFrom), LM_U8STR(strTo));
                }
            }
        }
    }
    else {
        Tstrncpy(m_szSigmaConfigDir, (const char_t *)szCnfUsr, LM_STRSIZE - 1);
        if (::wxDirExists((const char_t *)m_szSigmaConfigDir) == false) {
            wxString strDir((const char_t *)m_szSigmaConfigDir);
            ::wxMkdir(strDir);
        }
    }

    Tstrcpy(m_szSigmaConfigFilename, (const char_t *)m_szSigmaConfigDir);
    Tstrcpy(m_szEditorConfigFilename, (const char_t *)m_szSigmaConfigDir);
    Tstrcpy(m_szLexerConfigFilename, (const char_t *)m_szSigmaConfigDir);
    Tstrcpy(m_szLayoutConfigFilename, (const char_t *)m_szSigmaConfigDir);

    Tstrcat(m_szSigmaConfigFilename, uT("common.cometr"));
    Tstrcat(m_szEditorConfigFilename, uT("editor.cometr"));
    Tstrcat(m_szLayoutConfigFilename, uT("layout.cometr"));
    Tstrcat(m_szLexerConfigFilename, uT("user.cometu"));

    CometConfig *pSigmaConfig = new (std::nothrow) CometConfig(CometConfig::SECTIONCOUNT);
    if (pSigmaConfig) {
        pSigmaConfig->load((const char_t *)m_szSigmaConfigFilename, m_SigmaCommonPrefs, m_SigmaGUIPrefs, TOOL_CONF);
        delete pSigmaConfig;
        pSigmaConfig = NULL;
    }

    EditorConfig *pEditorConfig = new (std::nothrow) EditorConfig(EditorConfig::SECTIONCOUNT);
    if (pEditorConfig) {
        pEditorConfig->load((const char_t *)m_szEditorConfigFilename, m_ScintillaPrefs);
        delete pEditorConfig;
        pEditorConfig = NULL;
    }

    CodeEdit::createCometLexer();
    LexerConfig *pLexerConfig = new (std::nothrow) LexerConfig(LexerConfig::SECTIONCOUNT);
    if (pLexerConfig) {
        pLexerConfig->load((const char_t *)m_szLexerConfigFilename);
        delete pLexerConfig;
        pLexerConfig = NULL;
    }

    m_pTimer = NULL;
    m_iTimerCounter = 0;
    m_iTimerDuration = 20000;

    m_pExample = NULL;
    m_pSnippet = NULL;

    try {
        m_pExampleLua = new CodeSample(CODESAMPLE_EXAMPLE, wxSTC_LEX_LUA);
    }
    catch (...) {
        m_pExampleLua = NULL;
    }

    try {
        m_pSnippetLua = new CodeSample(CODESAMPLE_SNIPPET, wxSTC_LEX_LUA);
    }
    catch (...) {
        m_pSnippetLua = NULL;
    }

    try {
        m_pSnippetCPP = new CodeSample(CODESAMPLE_SNIPPET, wxSTC_LEX_CPP);
    }
    catch (...) {
        m_pSnippetCPP = NULL;
    }

    try {
        m_pSnippetPython = new CodeSample(CODESAMPLE_SNIPPET, wxSTC_LEX_PYTHON);
    }
    catch (...) {
        m_pSnippetPython = NULL;
    }

    try {
        m_pSnippetLaTeX = new CodeSample(CODESAMPLE_SNIPPET, wxSTC_LEX_LATEX);
    }
    catch (...) {
        m_pSnippetLaTeX = NULL;
    }

    m_pExample = m_pExampleLua;
    m_pSnippet = m_pSnippetLua;

    m_bmpNone = wxBitmap(none_small_xpm);
    m_bmpReadOnly = wxBitmap(readonly_small_xpm);
    m_bmpRunning = wxBitmap(started_small_xpm);
    m_bmpDebugging = wxBitmap(starteddebug_small_xpm);
    m_bmpFindin = wxBitmap(findin_small_xpm);
    m_bmpPinned = wxBitmap(pinned_small_xpm);
    m_bmpAutoReload = wxBitmap(autoreload_small_xpm);

    m_dlgTab = NULL;
    m_dlgTools = NULL;
    m_dlgColors = NULL;
    m_dlgLexer = NULL;

    m_pInfobarMain = NULL;
    m_pInfobarTool = NULL;
    m_pSizerMain = NULL;
    m_pSizerTool = NULL;

    m_bSilent = false;

    memset(&m_UItimeval, 0, sizeof(struct timeval));
    gettimeofday(&m_UItimeval, NULL);
}

CometFrame::~CometFrame()
{
    if (m_pDropTarget != NULL) {
        SetDropTarget(NULL);
        m_pDropTarget = NULL; // deleted in SetDropTarget(NULL)
    }

    if (m_pTimer) {
        m_pTimer->Stop();
        delete m_pTimer;
        m_pTimer = NULL;
    }

    if (m_dlgTab != NULL) {
        m_dlgTab->Destroy();
        m_dlgTab = NULL;
    }

    if (m_dlgTools != NULL) {
        m_dlgTools->Destroy();
        m_dlgTools = NULL;
    }

    if (m_dlgColors != NULL) {
        m_dlgColors->Destroy();
        m_dlgColors = NULL;
    }

    if (m_dlgLexer != NULL) {
        m_dlgLexer->Destroy();
        m_dlgLexer = NULL;
    }

    if (m_pFindFileDlg != NULL) {
        // normally never reached (destroyed in fileExit)
        m_pFindFileDlg->Destroy();
        m_pFindFileDlg = NULL;
    }

    if (m_pFindDirDlg != NULL) {
        // normally never reached (destroyed in fileExit)
        m_pFindDirDlg->Destroy();
        m_pFindDirDlg = NULL;
    }

    m_auiManager.UnInit();

    resetBookmarkArray();

    LexerConfig *pLexerConfig = new (std::nothrow) LexerConfig(LexerConfig::SECTIONCOUNT);
    if (pLexerConfig) {
        pLexerConfig->save((const char_t *)m_szLexerConfigFilename);
        delete pLexerConfig;
        pLexerConfig = NULL;
    }

    CodeEdit::deleteCometLexer();
}

void CometFrame::UpdateDebugNotebook(const wxString &strFilename, const wxString &strT)
{
    if (strT.IsEmpty()) {
        ClearDebugWindow(strFilename);
        return;
    }

    if ((m_pNotebookDebug == NULL) || (m_pListWatch == NULL) || (m_pListStack == NULL)) {
        CreateStackWindow();
        CreateWatchWindow();
    }

    if (m_pNotebookDebug && m_pListStack && m_pListWatch) {

        int iPos = (int)(strT.find(uT("\nSTACKTRACE\n")));
        if (iPos < 0) {
            ClearDebugWindow(strFilename);
            return;
        }

        wxString strWatch = strT.Mid(0, iPos);
        wxString strStack = strT.Mid(iPos + 12);

        wxAuiPaneInfo &paneTT = m_auiManager.GetPane(m_pDebugPanel);
        if (paneTT.IsShown() == false) {
            paneTT.Show();
            m_auiManager.Update();
        }

        ClearDebugWindow(strFilename);

        wxStringTokenizer tokenizerT(strWatch, uT("\n"));
        int ii = 0, jj = 0, iLen = 0;
        char_t cT;
        long itemIndex = 0L;
        while (tokenizerT.HasMoreTokens() && (ii < 64)) {

            wxString strToken = tokenizerT.GetNextToken();

            iLen = (int)(strToken.Length());
            if ((iLen < 1) || (iLen >= LM_STRSIZE)) {
                continue;
            }

            jj = iLen - 1;
            while (jj >= 0) {
                cT = strToken.at(jj);
                if ((cT != LM_NEWLINE_CHARA) && (cT != ('\t')) && (cT != (' ')) && (cT != ('*'))) {
                    break;
                }
                jj -= 1;
            }
            if (jj <= 0) {
                continue;
            }

            wxStringTokenizer tokenizerTT(strToken, uT("\t"));
            jj = 0;
            while (tokenizerTT.HasMoreTokens() && (jj < 8)) {
                wxString strTT = tokenizerTT.GetNextToken();
                if (jj == 0) {
                    itemIndex = m_pListWatch->InsertItem(0, strTT);
                }
                else {
                    m_pListWatch->SetItem(itemIndex, jj, strTT);
                }
                jj += 1;
            }

            ii += 1;
        }
        m_pListWatch->Update();

        wxStringTokenizer tokenizerX(strStack, uT("\n"));
        while (tokenizerX.HasMoreTokens() && (ii < 64)) {
            wxString strToken = tokenizerX.GetNextToken();
            wxStringTokenizer tokenizerTT(strToken, uT("\t"));
            jj = 0;
            while (tokenizerTT.HasMoreTokens() && (jj < 8)) {
                wxString strTT = tokenizerTT.GetNextToken();
                if (jj == 0) {
                    itemIndex = m_pListStack->InsertItem(0, strTT);
                }
                else {
                    m_pListStack->SetItem(itemIndex, jj, strTT);
                }
                jj += 1;
            }
            ii += 1;
        }
        m_pListStack->Update();
    }
}

bool CometFrame::GetDebugSymbol(const wxString &strN, wxString &strV)
{
    if ((m_pNotebookDebug == NULL) || (m_pListWatch == NULL) || (m_pListStack == NULL)) {
        return false;
    }

    int iLen = (int)(strN.Length());
    if (iLen < 1) {
        return false;
    }
    if (strN == uT("*")) {
        return false;
    }

    wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pDebugPanel);
    if (paneT.IsShown() == false) {
        paneT.Show();
        m_auiManager.Update();
    }

    int iCount = m_pListWatch->GetItemCount();
    if (iCount < 1) {
        return false;
    }

    wxListItem rowInfo;
    rowInfo.m_col = 0;
    rowInfo.m_mask = wxLIST_MASK_TEXT;
    for (int ii = 0; ii < wxMin(iCount, 64); ii++) {
        rowInfo.m_itemId = ii;
        m_pListWatch->GetItem(rowInfo);
        rowInfo.m_text.Replace(uT("\n"), uT(""));
        rowInfo.m_text.Replace(uT("\r"), uT(""));
        rowInfo.m_text.Replace(uT("\t"), uT(""));
        rowInfo.m_text.Replace(uT(" "), uT(""));
        if (rowInfo.m_text == strN) {
            rowInfo.m_col = 1;
            m_pListWatch->GetItem(rowInfo);
            strV = rowInfo.m_text;
            return true;
        }
    }

    return false;
}

void CometFrame::ClearDebugWindow(const wxString &strFilename)
{
    if (m_pNotebookDebug == NULL) {
        return;
    }

    if (m_pDebugPanel && m_pNotebookDebug && m_pListStack && m_pListWatch) {
        wxString strT = uT("Debug Window");
        if (strFilename.IsEmpty() == false) {
            strT += uT(" - ");
            strT += strFilename;
        }
        wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pDebugPanel);
        if (paneT.IsOk()) {
            paneT.Caption(strT);
            if (paneT.IsShown()) {
                m_auiManager.Update();
            }
        }
    }

    if (m_pListWatch) {
        m_pListWatch->DeleteAllItems();
    }
    if (m_pListStack) {
        m_pListStack->DeleteAllItems();
    }
}

void CometFrame::DoSetStatusbarText(const wxString &strT, int iNum)
{
    if ((m_pStatusBar == NULL) || (m_pStatusBar->IsShown() == false)) {
        return;
    }
    wxString strTO = m_pStatusBar->GetStatusText(iNum);
    if (strTO.IsSameAs(strT) == false) {
        m_pStatusBar->SetStatusText(strT, iNum);
    }
}

void CometFrame::updateStatusbarLexer(const wxString &strT)
{
    DoSetStatusbarText(strT, 1);
}

void CometFrame::updateStatusbarPos(int nLine, int nCol)
{
    if ((m_pStatusBar == NULL) || (m_pStatusBar->IsShown() == false)) {
        return;
    }

    if ((nLine >= 0) && (nCol >= 0)) {
        wxString strT = wxString::Format(uT("Ln %d"), nLine + 1);
        DoSetStatusbarText(strT, 2);
        strT = wxString::Format(uT("Col %d"), nCol + 1);
        DoSetStatusbarText(strT, 3);
    }
    else {
        DoSetStatusbarText(uT(""), 2);
        DoSetStatusbarText(uT(""), 3);
    }
}

void CometFrame::updateStatusbarIns(bool bIns)
{
    DoSetStatusbarText(bIns ? uT("INS") : uT("OVR"), 4);
}

void CometFrame::updateStatusbarEOL(int iEOL)
{
    DoSetStatusbarText((iEOL < 0) ? uT("") : ((iEOL == wxSTC_EOL_CRLF) ? uT("CRLF") : ((iEOL == wxSTC_EOL_LF) ? uT("LF") : uT("CR"))), 5);
}

bool CometFrame::isOutputLineEmpty(void)
{
    if ((m_pNotebookToolbox == NULL) || (m_pEditOutput == NULL)) {
        return true;
    }
    return (m_pEditOutput->isLineEmpty());
}

bool CometFrame::ensureOutputVisible(void)
{
    if (m_pNotebookToolbox == NULL) {
        CreateToolNotebook();
        m_auiManager.Update();
    }

    if (m_pNotebookToolbox == NULL) {
        // Should never happen
        OutputStatusbar(uT("Cannot initialize the output window"), SIGMAFRAME_TIMER_SHORT);
        return false;
    }

    wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pToolPanel);
    if (m_pEditOutput) {
        if (false == paneT.IsShown()) {
            paneT.Show();
            m_auiManager.Update();
        }
        m_SigmaGUIPrefs.bViewOutput = true;
        int iIndexOutput = m_pNotebookToolbox->GetPageIndex(m_pEditOutput);
        m_pNotebookToolbox->SetSelection(iIndexOutput);
    }
    else {
        if (CreateOutputWindow()) {
            int iIndexOutput = m_pNotebookToolbox->GetPageIndex(m_pEditOutput);
            m_pNotebookToolbox->SetSelection(iIndexOutput);
            paneT.Show();
            m_SigmaGUIPrefs.bViewOutput = true;
        }
        else {
            m_SigmaGUIPrefs.bViewOutput = false;
        }
        m_auiManager.Update();
    }

    return m_SigmaGUIPrefs.bViewOutput;
}

void CometFrame::Output(const wxString &strT, bool bAddTime /* = false*/, bool bShowMessage /* = false*/, wxWindow *pParent /* = NULL*/, bool bForce /* = false*/, bool bToErrPos /* = false*/)
{
    bool bSilent = getSilent();

    if ((false == bForce) && bSilent) {
        return;
    }

    if (strT.IsEmpty()) {
        return;
    }

    if (ensureOutputVisible() == false) {
        return;
    }

    wxString strX = strT;
    if (bAddTime == TRUE) {
        strX = uT("[");
        wxDateTime nowT = wxDateTime::Now();
        strX += nowT.Format(uT("%Y-%m-%d %H:%M:%S"));
        strX += uT("] ");
        strX += strT;
    }

    m_pEditOutput->Output(strX, bForce, bToErrPos);

    if (bShowMessage) {
        SigmaMessageBox(strT, uT("Comet"), wxOK | wxICON_EXCLAMATION, pParent);
    }
}

void CometFrame::clearOutput(void)
{
    if ((m_pNotebookToolbox == NULL) || (m_pEditOutput == NULL)) {
        return;
    }

    if (ensureOutputVisible() == false) {
        return;
    }

    m_pEditOutput->Reset();
}

bool CometFrame::scriptStart(void)
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

    OutputStatusbar(uT(""), SIGMAFRAME_TIMER_NONE);

    if (pEdit->isModified() || pEdit->GetFilename().IsEmpty()) {
        if (fileSave(iSel) == false) {
            return false;
        }
    }

    int iLexer = pEdit->GetLexer();

    bool bToolSet = CometFrame::isToolSet(iLexer);
    // for batch or bash files, the tool is the file itself
    bool bBatch = ((wxSTC_LEX_BATCH == iLexer) || (wxSTC_LEX_BASH == iLexer));

    if (bToolSet || bBatch) {
        if (bToolSet) {
            bool bSilent = CometFrame::getToolSilent(iLexer);
            setSilent(bSilent);
        }
        bool bret = pEdit->processStart();
        if (bToolSet) {
            OutputStatusbar(bret ? uT("External build tool started") : uT("Tool not started: check command (Tools/External Build Tools)"), SIGMAFRAME_TIMER_SHORT);
        }
        else {
            OutputStatusbar(bret ? uT("Command script started") : uT("Command script not started: check file content"), SIGMAFRAME_TIMER_SHORT);
        }
        DoUpdateStartButton();
        return bret;
    }

    setSilent(false);
    pEdit->luaRunScript();

    DoUpdateStartButton();

    return true;
}

bool CometFrame::scriptStop(void)
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

    pEdit->luaStopScript();

    return true;
}

bool CometFrame::toolStop(void)
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

    pEdit->processKill();

    return true;
}

void CometFrame::OnSize(wxSizeEvent &tEvent)
{
    CancelCallTip();

    tEvent.Skip();

    if (this->IsMaximized() || this->IsIconized()) {
        return;
    }

    int iWm = this->GetSize().GetX();
    int iHm = this->GetSize().GetY();
    int iWl = 0, iHl = 0;
    wxAuiPaneInfoArray arPane = m_auiManager.GetAllPanes();
    bool bL = false, bC = false, bR = false;
    for (size_t ii = 0; ii < arPane.Count(); ii++) {
        if (arPane[ii].IsDocked() && arPane[ii].IsShown()) {
            if ((bL == false) && (arPane[ii].dock_direction == wxAUI_DOCK_LEFT)) {
                iWl += arPane[ii].window->GetSize().GetX();
                if (arPane[ii].window->GetSize().GetY() > iHl) {
                    iHl = arPane[ii].window->GetSize().GetY();
                }
                bL = true;
            }
            if ((bC == false) && (arPane[ii].dock_direction == wxAUI_DOCK_CENTER)) {
                iWl += arPane[ii].window->GetSize().GetX();
                if (arPane[ii].window->GetSize().GetY() > iHl) {
                    iHl = arPane[ii].window->GetSize().GetY();
                }
                bC = true;
            }
            if ((bR == false) && (arPane[ii].dock_direction == wxAUI_DOCK_RIGHT)) {
                iWl += arPane[ii].window->GetSize().GetX();
                if (arPane[ii].window->GetSize().GetY() > iHl) {
                    iHl = arPane[ii].window->GetSize().GetY();
                }
                bR = true;
            }
        }
        if (bL && bC && bR) {
            break;
        }
    }

    if ((iWl > iWm) || (iHl > iHm) || m_bRefreshLayout) {

        if ((iWl > iWm) || (iHl > iHm)) {
            this->SetSize(wxSize(wxMax(iWl, this->GetMinSize().GetX()), wxMax(iHl, wxMax(this->GetMinSize().GetY(), this->GetSize().GetY()))));
        }

        m_bRefreshLayout = ((iWl > iWm) || (iHl > iHm));

        this->Layout();
        m_auiManager.Update();

        if (m_pNotebookFilesys && m_auiManager.GetPane(m_pFilesysPanel).IsShown() && m_auiManager.GetPane(m_pFilesysPanel).IsDocked()) {
            m_pFilesysPanel->Refresh();
        }
        if (m_pMainPanel && m_auiManager.GetPane(m_pMainPanel).IsShown() && m_auiManager.GetPane(m_pMainPanel).IsDocked()) {
            m_pMainPanel->Refresh();
        }
        if (m_pToolPanel && m_auiManager.GetPane(m_pToolPanel).IsShown() && m_auiManager.GetPane(m_pToolPanel).IsDocked()) {
            m_pToolPanel->Refresh();
        }
        if (m_pNotebookDebug && m_auiManager.GetPane(m_pDebugPanel).IsShown() && m_auiManager.GetPane(m_pDebugPanel).IsDocked()) {
            m_pDebugPanel->Refresh();
        }
    }

    //
}

void CometFrame::OnMove(wxMoveEvent &tEvent)
{
    CancelCallTip();
    tEvent.Skip();
}

void CometFrame::OnKillFocus(wxFocusEvent &tEvent)
{
    CancelCallTip();
    tEvent.Skip();
}

void CometFrame::OnNotebookFileList(wxAuiNotebookEvent &WXUNUSED(tEvent))
{
    wxMenu popMenu;

    int iTextWidth = 0;
    updateOpenFileMenu(&popMenu, NULL, &iTextWidth);
    if (iTextWidth < 20) {
        iTextWidth = 20;
    }

    wxPoint ptMousePosition = GetMouseNotebookPosBottom(m_pNotebookMain);
    ptMousePosition.x -= iTextWidth;

    PopupMenu(&popMenu, ptMousePosition);
}

void CometFrame::OnFileList(wxCommandEvent &WXUNUSED(tEvent))
{
    wxMenu popMenu;

    updateOpenFileMenu(&popMenu, NULL, NULL);

    wxPoint ptMousePosition = GetMouseNotebookPos(m_pNotebookMain);

    PopupMenu(&popMenu, ptMousePosition);
}

wxPoint CometFrame::GetMouseNotebookPosBottom(wxAuiNotebook *pNotebook)
{
    wxPoint ptMousePosition = wxGetMousePosition();
    ptMousePosition = this->ScreenToClient(ptMousePosition);
    ptMousePosition.y = pNotebook->GetPosition().y + pNotebook->GetTabCtrlHeight();

#ifdef __WXMSW__
    wxToolBar *pToolBar = this->GetToolBar();
    // For windows, take into account the toolbar (not necessary for wxGTK)
    if ((pToolBar != NULL) && pToolBar->IsShown()) {
        ptMousePosition.y += pToolBar->GetSize().y;
    }
#endif

    return ptMousePosition;
}

wxPoint CometFrame::GetMouseNotebookPos(wxAuiNotebook *pNotebook)
{
    wxPoint ptMousePosition = wxGetMousePosition();
    ptMousePosition = this->ScreenToClient(ptMousePosition);
    ptMousePosition.y = pNotebook->GetPosition().y;
    ptMousePosition.x -= 8;

#ifdef __WXMSW__
    wxToolBar *pToolBar = this->GetToolBar();
    // For windows, take into account the toolbar (not necessary for wxGTK)
    if ((pToolBar != NULL) && pToolBar->IsShown()) {
        ptMousePosition.y += pToolBar->GetSize().y;
    }
#endif

    return ptMousePosition;
}

wxPoint CometFrame::GetMouseToolbarPos(void)
{
    wxPoint ptMousePosition = wxGetMousePosition();
    ptMousePosition = this->ScreenToClient(ptMousePosition);

#ifdef __WXMSW__
    wxToolBar *pToolBar = this->GetToolBar();
    // For windows, take into account the toolbar (not necessary for wxGTK)
    if ((pToolBar != NULL) && pToolBar->IsShown()) {
        ptMousePosition.y += pToolBar->GetSize().y;
    }
#endif

    return ptMousePosition;
}

void CometFrame::OnCodeFormat(wxCommandEvent &tEvent)
{
    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        return;
    }
    bool bEnable = pEdit->isLexerKnown(pEdit->GetLexer()) && (pEdit->GetReadOnly() == false);
    if (bEnable == false) {
        return;
    }

    wxMenu popMenu;

    wxPoint ptMousePosition = GetMouseNotebookPos(m_pNotebookMain);

    pEdit->CreateMenuFormat(&popMenu, false);

    PopupMenu(&popMenu, ptMousePosition);
}

void CometFrame::OnCodeSnippet(wxCommandEvent &WXUNUSED(tEvent))
{
    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        return;
    }
    bool bEnable = pEdit->isLexerKnown(pEdit->GetLexer()) && (pEdit->GetReadOnly() == false) && getSnippet();
    if (bEnable == false) {
        return;
    }

    wxMenu popMenu;

    wxPoint ptMousePosition = GetMouseNotebookPos(m_pNotebookMain);

    pEdit->CreateMenuSnippet(&popMenu);

    PopupMenu(&popMenu, ptMousePosition);
}

void CometFrame::OnFileListSelect(wxCommandEvent &tEvent)
{
    setActiveEditor(tEvent.GetId() - ID_FILE_LIST01);
}

void CometFrame::OnFileEnforceLexer(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount < 1) {
        return;
    }

    int iSel = 0;

    iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        return;
    }

    int iLexer = CodeEdit::getLexerByIndex(idT - ID_FILE_LEXER01);
    if (iLexer > 0) {
        pEdit->enforceLexer(iLexer);
        updateCodeSample();
    }
}

void CometFrame::DoScriptColors(void)
{
    bool useTab = m_ScintillaPrefs.common.useTab;
    int tabSize = m_ScintillaPrefs.common.tabSize;

    ScriptEdit *pEditor = getActiveEditor();
    pEditor->getPrefs(m_ScintillaPrefs);

    m_ScintillaPrefs.common.useTab = useTab;
    m_ScintillaPrefs.common.tabSize = tabSize;

    ConsoleEdit *pConsole = getConsole();
    if (pConsole) {
        pConsole->setPrefs(m_ScintillaPrefs, true);
    }

    ScriptEdit *pEdit = NULL;
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());
    for (int ii = 0; ii < iPageCount; ii++) {
        pEdit = getEditor(ii);
        if (pEdit == NULL) {
            break;
        }
        pEdit->setPrefs(m_ScintillaPrefs, true, false);
    }

    unsigned long iB = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
    unsigned long iF = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;

    if ((iB != m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background) || (iF != m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground)) {
        initToolPrefs(m_ScintillaPrefs);
    }
}

void CometFrame::DoScriptLexer(void)
{
    ScriptEdit *pEdit = NULL;
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());
    for (int ii = 0; ii < iPageCount; ii++) {
        pEdit = getEditor(ii);
        if (pEdit == NULL) {
            continue;
        }
        int iLexer = pEdit->GetLexer();
        if (iLexer == wxSTC_LEX_COMET) {
            pEdit->initLexer(wxSTC_LEX_COMET, 0);
            pEdit->DoHighlightSyntax(m_ScintillaPrefs.common.syntaxEnable);
            pEdit->updateStatusbar(true);
        }
        else {
            wxFileName fname = pEdit->GetFilename();
            if (fname.IsOk()) {
                wxString strExt = fname.GetExt();
                if ((pEdit->supportedLexer(strExt, NULL) == 0) && (strExt.IsSameAs(CodeEdit::getCometLexerExt()))) {
                    pEdit->initLexer(wxSTC_LEX_COMET, 0);
                    pEdit->DoHighlightSyntax(m_ScintillaPrefs.common.syntaxEnable);
                    pEdit->updateStatusbar(true);
                }
            }
        }
    }
}

void CometFrame::OnScriptLexer(wxCommandEvent &WXUNUSED(tEvent))
{
    if (m_dlgLexer != NULL) {
        // Bring to front
        m_dlgLexer->SetFocus();
        m_dlgLexer->Show();
        m_dlgLexer->Raise();
        return;
    }

    m_dlgLexer = new LexerDlg(this);
    m_dlgLexer->Show();
}

void CometFrame::OnScriptColors(wxCommandEvent &WXUNUSED(tEvent))
{
    if (m_dlgColors != NULL) {
        // Bring to front
        m_dlgColors->SetFocus();
        m_dlgColors->Show();
        m_dlgColors->Raise();
        return;
    }

    m_dlgColors = new ColorsDlg(this);
    m_dlgColors->Show();
}

void CometFrame::DoScriptTab(void)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount > 0) {
        ScriptEdit *pEdit = NULL;
        for (int ii = 0; ii < iPageCount; ii++) {
            pEdit = getEditor(ii);
            if (pEdit == NULL) {
                return;
            }
            pEdit->DoUseTab(m_ScintillaPrefs.common.useTab);
            pEdit->DoSetTabSize(m_ScintillaPrefs.common.tabSize);
            pEdit->DoIndentGuide(m_ScintillaPrefs.common.indentGuideEnable);
            pEdit->DoSetLongLine(m_ScintillaPrefs.common.longLine);
        }
    }
}

void CometFrame::OnScriptTab(wxCommandEvent &WXUNUSED(tEvent))
{
    if (m_dlgTab != NULL) {
        // Bring to front
        m_dlgTab->SetFocus();
        m_dlgTab->Show();
        m_dlgTab->Raise();
        return;
    }

    m_dlgTab = new TabDlg(this, &(m_ScintillaPrefs.common));
    m_dlgTab->Show();
}

void CometFrame::OnMainAction(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();

    DoMainAction(idT);
}

void CometFrame::DoMainAction(int idT)
{
    if (idT == ID_SEARCH_FIND) {
        DoFindReplace(true);
    }

    else if ((idT == ID_FIND_PREV) || (idT == ID_FIND_NEXT)) {
        DoFindPrevNext(idT);
    }

    else if (idT == ID_SEARCH_REPLACE) {
        DoFindReplace(false);
    }

    else if ((idT == ID_SEARCH_FINDINFILES) || (idT == ID_SEARCH_REPLACEINFILES)) {
        ScriptEdit *pEdit = static_cast<ScriptEdit *>(getActiveEditor());
        if (pEdit == NULL) {
            return;
        }

        wxString strFind, strReplace;
        if ((m_pFindFileDlg != NULL) && (m_pFindFileDlg->IsShown())) {
            strFind = m_pFindFileDlg->getFind();
            if (strFind.IsEmpty()) {
                strFind = getTextToFind(pEdit);
            }
            strReplace = m_pFindFileDlg->getFind();
            if (strReplace.IsEmpty()) {
                strReplace = getCurrentReplaceItem();
            }
            updateFindFilePos();
            m_pFindFileDlg->Hide();
        }
        else {
            strFind = getTextToFind(pEdit);
            strReplace = getCurrentReplaceItem();
        }

        wxString strDir = getCurrentDirItem();

        bool bFindOnly = (idT == ID_SEARCH_FINDINFILES);

        if (m_pFindDirDlg != NULL) {

            if (m_pFindDirDlg->findOnly() == bFindOnly) {
                m_pFindDirDlg->updateItems(strFind, strReplace, strDir);

                // Bring to front
                bool bIconized = m_pFindDirDlg->IsIconized();
                if (bIconized) {
                    m_pFindDirDlg->Iconize(false);
                }
                m_pFindDirDlg->SetFocus();
                m_pFindDirDlg->Raise();
                m_pFindDirDlg->Show(true);
                //

                return;
            }

            updateFindDirPos();
            // call Destroy directly, since at this point it is not necessary to handle EVT_CLOSE
            m_pFindDirDlg->Destroy();
            m_pFindDirDlg = NULL;
        }

        int iScreenWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
        int iScreenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
        wxPoint ptInit = wxDefaultPosition;
        if ((m_SigmaGUIPrefs.iFindDirPosX >= 20) && (m_SigmaGUIPrefs.iFindDirPosX <= (iScreenWidth - 200)) && (m_SigmaGUIPrefs.iFindDirPosY >= 20) && (m_SigmaGUIPrefs.iFindDirPosY <= (iScreenHeight - 200)) && (m_SigmaGUIPrefs.iFindDirWidth >= 200) && (m_SigmaGUIPrefs.iFindDirWidth <= iScreenWidth) && (m_SigmaGUIPrefs.iFindDirHeight >= 200) && (m_SigmaGUIPrefs.iFindDirHeight <= iScreenHeight)) {
            ptInit.x = m_SigmaGUIPrefs.iFindDirPosX;
            ptInit.y = m_SigmaGUIPrefs.iFindDirPosY;
        }

        m_pFindDirDlg = new (std::nothrow) FindDirDlg(this, strFind, bFindOnly == false, ptInit);
        if (m_pFindDirDlg == NULL) {
            return;
        }

        m_pFindDirDlg->Show(true);
        m_pFindDirDlg->updateItems(strFind, strReplace, strDir);

        int iMinWidth = m_pFindDirDlg->GetMinSize().GetX();
        int iMinHeight = m_pFindDirDlg->GetMinSize().GetY();
        wxSize sizeInit;
        if ((m_SigmaGUIPrefs.iFindDirWidth >= iMinWidth) && (m_SigmaGUIPrefs.iFindDirWidth <= iScreenWidth) && (m_SigmaGUIPrefs.iFindDirHeight >= iMinHeight) && (m_SigmaGUIPrefs.iFindDirHeight <= iScreenHeight)) {
            sizeInit.Set(m_SigmaGUIPrefs.iFindDirWidth, m_SigmaGUIPrefs.iFindDirHeight);
            m_pFindDirDlg->SetSize(sizeInit);
        }
    }

    else if (idT == ID_SEARCH_RESETMARKERS) {
        const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());
        if ((iPageCount >= 1) && (iPageCount <= COMET_MAXFILES)) {
            ScriptEdit *pEdit = NULL;
            for (int ii = 0; ii < iPageCount; ii++) {
                pEdit = getEditor(ii);
                if (pEdit) {
                    pEdit->resetFindMarkers();
                    updateEditorStatus(pEdit);
                }
            }
        }
    }

    else if (idT == ID_FILE_PIN) {
        ScriptEdit *pEdit = this->getActiveEditor();
        if (pEdit) {
            bool bPinned = pEdit->isPinned();
            pEdit->setPinned(false == bPinned);
            updateEditorStatus(pEdit);
            if (pEdit->isPinned()) {
                const int idx = (int)(m_pNotebookMain->GetPageIndex(pEdit));
                const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());
                if ((idx > 0) && (idx < iPageCount) && (iPageCount > 1) && (iPageCount <= COMET_MAXFILES)) {
                    ScriptEdit *pEditX;
                    for (int ii = 0; ii < idx; ii++) {
                        pEditX = getEditor(ii);
                        if (pEditX && (pEditX->isPinned() == false)) {
                            // move the pinned page to the beginning
                            m_pNotebookMain->DoMovePage(idx, ii);
                            m_pNotebookMain->MakeSelectionVisible();
                            break;
                        }
                    }
                }
            }
        }
    }

    else if ((idT == ID_FILE_RESETMARKERS) || (idT == ID_FILE_RESETMARKERSB)) {
        ScriptEdit *pEdit = this->getActiveEditor();
        if (pEdit && pEdit->containsMarkers()) {
            int iret = SigmaMessageBox(uT("Remove all markers?"), uT("Comet"),
                                       wxYES | wxNO | wxCANCEL | wxICON_QUESTION | wxNO_DEFAULT, this,
                                       uT("&Remove"), uT("D&on't remove"));
            if (iret == wxYES) {
                pEdit->resetAllMarkers();
                updateEditorStatus(pEdit);
            }
        }
    }

    else if (idT == ID_EXPLORER_BROWSE) {
        m_pExplorerTree->doBrowseDir();
    }

    else if (idT == ID_EXPLORER_REFRESH) {
        m_pExplorerTree->doRefreshDir();
    }

    else if (idT == ID_EXPLORER_OPEN) {
        wxString strFilename = m_pExplorerTree->GetPath();
        fileOpen(strFilename, false);
    }

    else if (idT == ID_EXPLORER_RENAME) {
        m_pExplorerTree->doRenameItem();
    }

    else if (idT == ID_EXPLORER_CUT) {
        m_pExplorerTree->doCutItem();
    }

    else if (idT == ID_EXPLORER_COPY) {
        m_pExplorerTree->doCopyItem();
    }

    else if (idT == ID_EXPLORER_PASTE) {
        m_pExplorerTree->doPasteItem();
    }

    else if (idT == ID_EXPLORER_DELETE) {
        m_pExplorerTree->doDeleteItemPrepare();
        m_pExplorerTree->doDeleteItem();
    }

    else if (idT == ID_EXPLORER_CREATEFILE) {
        m_pExplorerTree->doCreateItem(false);
    }

    else if (idT == ID_EXPLORER_CREATEDIR) {
        m_pExplorerTree->doCreateItem(true);
    }

    else if (idT == ID_EXPLORER_COPYPATH) {
        m_pExplorerTree->doCopyPath();
    }

    else if (idT == ID_EXPLORER_CHANGEDIR) {
        m_pExplorerTree->doChangeDir();
    }

#ifdef __WXMSW__
    else if (idT == ID_EXPLORER_OPENEXT) {
        m_pExplorerTree->doOpenExplorer();
    }

    else if (idT == ID_EXPLORER_OPENSHELL) {
        m_pExplorerTree->doOpenShell();
    }

    else if (idT == ID_EXPLORER_FILEPROPS) {
        m_pExplorerTree->doViewProperties();
    }
#endif

    else if (idT == ID_OUTPUT_RESET) {
        ClearOutputWindow(wxEmptyString, true);
    }
}

void CometFrame::OnEditAction(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();

    if ((idT >= wxID_CUT) && (idT <= wxID_SELECTALL) && m_cboAnalyzer && m_cboAnalyzer->isFocused() && m_cboAnalyzer->IsEditable()) {
        m_cboAnalyzer->OnEditAction(tEvent);
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        return;
    }

    if (pEdit->isFocused()) {
        if ((idT >= wxID_CUT) && (idT <= wxID_SELECTALL)) {
            tEvent.SetId(idT);
        }
        else {
            tEvent.SetId(ID_SCRIPT_FIRST + (idT - ID_MAIN_FIRST));
        }
        pEdit->OnEditAction(tEvent);
    }

    else if (m_pEditConsole) {
        if ((idT != ID_NOTHING) && (m_pEditConsole->isFocused())) {
            if ((idT >= wxID_CUT) && (idT <= wxID_CLEAR)) {
                tEvent.SetId(idT);
            }
            else {
                tEvent.SetId(ID_CONSOLE_FIRST + (idT - ID_MAIN_FIRST));
            }
            m_pEditConsole->OnEditAction(tEvent);
        }
    }

    else if (m_pEditOutput) {
        if ((idT != ID_NOTHING) && (m_pEditOutput->isFocused())) {
            if ((idT >= wxID_CUT) && (idT <= wxID_CLEAR)) {
                tEvent.SetId(idT);
            }
            else {
                tEvent.SetId(ID_OUTPUT_FIRST + (idT - ID_MAIN_FIRST));
            }
            m_pEditOutput->OnEditAction(tEvent);
        }
    }
}

void CometFrame::DoViewInfobar(bool bViewInfobar)
{
    m_SigmaGUIPrefs.bViewInfobar = bViewInfobar;
    m_pInfobarMain->setActive(m_SigmaGUIPrefs.bViewInfobar);
    m_pInfobarMain->Refresh();
}

void CometFrame::OnScriptAction(wxCommandEvent &tEvent)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());
    if ((iPageCount < 1) || (iPageCount > COMET_MAXFILES)) {
        tEvent.Skip();
        return;
    }

    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        tEvent.Skip();
        return;
    }

    int idT = tEvent.GetId();

    if (idT == ID_SCRIPT_LINENUMBERS) {
        m_ScintillaPrefs.common.lineNumberEnable = (m_ScintillaPrefs.common.lineNumberEnable == false);
        for (int ii = 0; ii < iPageCount; ii++) {
            pEdit = getEditor(ii);
            if (pEdit) {
                pEdit->DoLineNumber(m_ScintillaPrefs.common.lineNumberEnable);
            }
        }
    }

    else if (idT == ID_SCRIPT_FOLDENABLE) {
        m_ScintillaPrefs.common.foldEnable = (m_ScintillaPrefs.common.foldEnable == false);
        for (int ii = 0; ii < iPageCount; ii++) {
            pEdit = getEditor(ii);
            if (pEdit) {
                pEdit->DoFoldEnable(m_ScintillaPrefs.common.foldEnable);
            }
        }
    }

    else if (idT == ID_SCRIPT_WRAPMODE) {
        m_ScintillaPrefs.common.wrapModeInitial = (m_ScintillaPrefs.common.wrapModeInitial == false);
        for (int ii = 0; ii < iPageCount; ii++) {
            pEdit = getEditor(ii);
            if (pEdit) {
                pEdit->DoWrapMode(m_ScintillaPrefs.common.wrapModeInitial);
                m_ScintillaPrefs.loaded = true;
            }
        }
    }

    else if (idT == ID_SCRIPT_SAVENAVIGATION) {
        m_SigmaGUIPrefs.bSaveNavigation = (m_SigmaGUIPrefs.bSaveNavigation == false);
    }

    else if (idT == ID_SCRIPT_VIEWINFOBAR) {
        DoViewInfobar(m_SigmaGUIPrefs.bViewInfobar == false);
    }

    else if (idT == ID_SCRIPT_MARGINMARKERS) {
        m_ScintillaPrefs.common.markerEnable = (m_ScintillaPrefs.common.markerEnable == false);
        for (int ii = 0; ii < iPageCount; ii++) {
            pEdit = getEditor(ii);
            if (pEdit) {
                pEdit->DoShowMarker(m_ScintillaPrefs.common.markerEnable);
            }
        }
    }

    else if (idT == ID_SCRIPT_MARGINSTATUS) {
        m_ScintillaPrefs.common.statusEnable = (m_ScintillaPrefs.common.statusEnable == false);
        for (int ii = 0; ii < iPageCount; ii++) {
            pEdit = getEditor(ii);
            if (pEdit) {
                pEdit->DoShowStatus(m_ScintillaPrefs.common.statusEnable);
            }
        }
    }

    else if (idT == ID_SCRIPT_SYNTAX) {
        m_ScintillaPrefs.common.syntaxEnable = (m_ScintillaPrefs.common.syntaxEnable == false);
        for (int ii = 0; ii < iPageCount; ii++) {
            pEdit = getEditor(ii);
            if (pEdit) {
                pEdit->DoHighlightSyntax(m_ScintillaPrefs.common.syntaxEnable);
            }
        }
    }

    else if (idT == ID_SCRIPT_AUTOCOMPLETE) {
        m_ScintillaPrefs.common.autocompleteEnable = (m_ScintillaPrefs.common.autocompleteEnable == false);
        for (int ii = 0; ii < iPageCount; ii++) {
            pEdit = getEditor(ii);
            if (pEdit) {
                pEdit->DoEnableAutocomplete(m_ScintillaPrefs.common.autocompleteEnable);
            }
        }
    }

    else if (idT == ID_SCRIPT_CALLTIP) {
        m_ScintillaPrefs.common.calltipEnable = (m_ScintillaPrefs.common.calltipEnable == false);
        if (iPageCount > 0) {
            for (int ii = 0; ii < iPageCount; ii++) {
                pEdit = getEditor(ii);
                if (pEdit) {
                    pEdit->DoEnableCalltip(m_ScintillaPrefs.common.calltipEnable);
                }
            }
        }
    }

    else if ((idT >= ID_SCRIPT_LINESPACING_100) && (idT <= ID_SCRIPT_LINESPACING_175)) {
        int iT = m_ScintillaPrefs.common.lineSpacing;
        if (idT == ID_SCRIPT_LINESPACING_100) {
            m_ScintillaPrefs.common.lineSpacing = 0;
        }
        else if (idT == ID_SCRIPT_LINESPACING_125) {
            m_ScintillaPrefs.common.lineSpacing = 1;
        }
        else if (idT == ID_SCRIPT_LINESPACING_150) {
            m_ScintillaPrefs.common.lineSpacing = 2;
        }
        else if (idT == ID_SCRIPT_LINESPACING_175) {
            m_ScintillaPrefs.common.lineSpacing = 3;
        }

        if (iT != m_ScintillaPrefs.common.lineSpacing) {
            for (int ii = 0; ii < iPageCount; ii++) {
                pEdit = getEditor(ii);
                if (pEdit) {
                    pEdit->initCaret(m_ScintillaPrefs.common.lineSpacing);
                }
            }
        }
    }

    else if (idT == ID_SCRIPT_DISPLAYEOL) {
        m_ScintillaPrefs.common.displayEOLEnable = (m_ScintillaPrefs.common.displayEOLEnable == false);
        if (iPageCount > 0) {
            for (int ii = 0; ii < iPageCount; ii++) {
                pEdit = getEditor(ii);
                if (pEdit) {
                    pEdit->DoEnableEOL(m_ScintillaPrefs.common.displayEOLEnable);
                }
            }
        }
    }

    else if (idT == ID_SCRIPT_WHITESPACE) {
        m_ScintillaPrefs.common.whiteSpaceEnable = (m_ScintillaPrefs.common.whiteSpaceEnable == false);
        for (int ii = 0; ii < iPageCount; ii++) {
            pEdit = getEditor(ii);
            if (pEdit) {
                pEdit->DoEnableWhiteSpace(m_ScintillaPrefs.common.whiteSpaceEnable);
            }
        }
    }

    else if ((idT == ID_SCRIPT_INDENTINC) || (idT == ID_SCRIPT_INDENTDEC)) {
        int iStart = 1, iEnd = 0;
        int iLineStart = 1, iLineEnd = 0;
        pEdit->GetSelection(&iStart, &iEnd);
        iLineStart = pEdit->LineFromPosition(iStart);
        iLineEnd = pEdit->LineFromPosition(iEnd);
        if (iLineEnd > iLineStart) {
            if (pEdit->PositionFromLine(iLineEnd) == iEnd) {
                --iLineEnd;
            }
            if (pEdit->GetLineEndPosition(iLineStart) == iStart) {
                ++iLineStart;
            }
        }
        if (iLineEnd >= iLineStart) {
            pEdit->BeginUndoAction();
            for (int iLine = iLineStart; iLine <= iLineEnd; iLine++) {
                int iIndent = pEdit->GetLineIndentation(iLine);
                wxString strT = pEdit->GetLine(iLine);
                strT = strT.Trim();
                if (strT.IsEmpty() && (0 == iIndent)) {
                    continue;
                }
                if (idT == ID_SCRIPT_INDENTINC) {
                    iIndent += pEdit->GetIndent();
                }
                else {
                    iIndent -= pEdit->GetIndent();
                }
                if (iIndent >= 0) {
                    pEdit->SetLineIndentation(iLine, iIndent);
                }
            }
            pEdit->EndUndoAction();
        }
    }

    else if (idT == ID_SCRIPT_FORMATDOC) {
        pEdit->DoFormatDoc();
    }

    else if (idT == ID_SCRIPT_COMMENTSELECTION) {
        pEdit->DoCommentSelection(true);
    }

    else if (idT == ID_SCRIPT_UNCOMMENTSELECTION) {
        pEdit->DoCommentSelection(false);
    }

    else if ((idT == ID_LATEX_TEXTBF) || (idT == ID_LATEX_TEXTIT) || (idT == ID_LATEX_TEXTSC) || (idT == ID_LATEX_TEXTNORMAL) || (idT == ID_LATEX_MATHRM) || (idT == ID_LATEX_EMPH) || (idT == ID_LATEX_TEXTSUPERSCRIPT) || (idT == ID_LATEX_TEXTSUBSCRIPT)) {
        pEdit->DoLaTeXFormat(idT);
    }

    else if ((idT == ID_SCRIPT_UPPERCASE) || (idT == ID_SCRIPT_LOWERCASE)) {
        pEdit->DoChangeCase(idT);
    }

    else if (idT == ID_SCRIPT_DETECTCHANGE) {
        m_SigmaCommonPrefs.bDetectChangeOutside = (m_SigmaCommonPrefs.bDetectChangeOutside == false);
    }

    else if (idT == ID_SCRIPT_UTF8FROM_ISO8859L1) {
        pEdit->DoUTF8Encode(UTF8FROM_ISO8859L1);
    }

    else if (idT == ID_SCRIPT_UTF8FROM_ISO8859L9) {
        pEdit->DoUTF8Encode(UTF8FROM_ISO8859L9);
    }

    else if (idT == ID_SCRIPT_UTF8FROM_CP1252) {
        pEdit->DoUTF8Encode(UTF8FROM_CP1252);
    }

    else {
        if (idT == ID_SCRIPT_BOOKMARK_DELALL) {
            int iret = SigmaMessageBox(uT("Delete file bookmarks?"), uT("Comet"),
                                       wxYES | wxNO | wxCANCEL | wxICON_QUESTION | wxNO_DEFAULT, this,
                                       uT("&Delete"), uT("D&on't delete"));
            if (iret != wxYES) {
                return;
            }
        }
        pEdit->DoScriptAction(idT);
    }
}

void CometFrame::OnInfobarAction(wxCommandEvent &tEvent)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());
    if ((iPageCount < 1) || (iPageCount > COMET_MAXFILES)) {
        tEvent.Skip();
        return;
    }

    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        tEvent.Skip();
        return;
    }

    int idT = tEvent.GetId();

    int iLineCount = pEdit->GetLineCount();
    int iCurrentLine = pEdit->GetCurrentLine();
    const int iMaskX = SCRIPT_MASK_MODIFIED | SCRIPT_MASK_SAVED;
    int iMarkerLine = -1;

    if (idT == ID_SCRIPT_INFOBAR_NEXTX) {
        iMarkerLine = pEdit->MarkerNext(iCurrentLine + 1, iMaskX);
        if ((iMarkerLine < iCurrentLine) || (iMarkerLine >= iLineCount)) {
            iMarkerLine = -1;
        }
    }

    else if (idT == ID_SCRIPT_INFOBAR_PREVX) {
        iMarkerLine = pEdit->MarkerPrevious(iCurrentLine - 1, iMaskX);
        if ((iMarkerLine < 0) || (iMarkerLine > iCurrentLine)) {
            iMarkerLine = -1;
        }
    }

    if (iMarkerLine >= 0) {
        int iMask = pEdit->MarkerGet(iMarkerLine);
        if (0 != iMask) {
            if (pEdit->isFocused() == false) {
                pEdit->SetFocus();
            }
            pEdit->GotoLine(iMarkerLine);
        }
    }
}

void CometFrame::OnToolsBuild(wxCommandEvent &tEvent)
{
    if (m_dlgTools != NULL) {
        // Bring to front
        m_dlgTools->SetFocus();
        m_dlgTools->Show();
        m_dlgTools->Raise();
        return;
    }

    if (tEvent.GetId() == ID_TOOLS_BUILD) {
        const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

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

        m_dlgTools = new ToolsDlg(this, pEdit->GetLexer());
        m_dlgTools->Show();
    }
}

void CometFrame::OnViewTopmost(wxCommandEvent &tEvent)
{
    if (tEvent.GetId() == ID_VIEW_TOPMOST) {
        long iStyle = this->GetWindowStyle();
        if ((iStyle & wxSTAY_ON_TOP) != 0) {
            iStyle &= ~wxSTAY_ON_TOP;
        }
        else {
            iStyle |= wxSTAY_ON_TOP;
        }
        this->SetWindowStyle(iStyle);

        updateTitle(wxEmptyString);
    }
}

void CometFrame::DoViewFileExplorer(bool bViewOnly)
{
    if (m_pNotebookFilesys == NULL) {
        CreateFilesysNotebook();
        m_auiManager.Update();
    }

    if (m_pNotebookFilesys == NULL) {
        // Should never happen
        return;
    }

    wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pFilesysPanel);
    if (m_pExplorerTree) {
        if (paneT.IsShown()) {
            if (false == bViewOnly) {
                int iIndexExplorer = m_pNotebookFilesys->GetPageIndex(m_pExplorerTree);
                if (m_pNotebookFilesys->DeletePage(iIndexExplorer)) {
                    m_pExplorerTree = NULL;
                    m_SigmaGUIPrefs.bViewExplorer = false;
                }
            }
            if ((m_pExplorerTree == NULL) && (m_pListBookmark == NULL)) {
                paneT.Hide();
                m_auiManager.Update();
            }
        }
        else {
            if ((false == m_SigmaGUIPrefs.bViewBookmark) && m_pListBookmark) {
                int iIndexBookmark = m_pNotebookFilesys->GetPageIndex(m_pListBookmark);
                if (m_pNotebookFilesys->DeletePage(iIndexBookmark)) {
                    m_pListBookmark = NULL;
                }
            }
            paneT.Show();
            m_auiManager.Update();
            m_SigmaGUIPrefs.bViewExplorer = true;
        }
    }
    else {
        if (paneT.IsShown() == false) {
            if ((false == m_SigmaGUIPrefs.bViewBookmark) && m_pListBookmark) {
                int iIndexBookmark = m_pNotebookFilesys->GetPageIndex(m_pListBookmark);
                if (m_pNotebookFilesys->DeletePage(iIndexBookmark)) {
                    m_pListBookmark = NULL;
                }
            }
            paneT.Show();
            m_auiManager.Update();
        }
        m_SigmaGUIPrefs.bViewExplorer = true;
        if (CreateExplorerTree()) {
            int iIndexExplorer = m_pNotebookFilesys->GetPageIndex(m_pExplorerTree);
            m_pNotebookFilesys->SetSelection(iIndexExplorer);
        }
        else {
            OutputStatusbar(uT("Cannot initialize the explorer window"), SIGMAFRAME_TIMER_SHORT);
            m_SigmaGUIPrefs.bViewExplorer = false;
        }
    }
}

void CometFrame::OnView(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();

    if (idT == ID_VIEW_EXPLORER) {
        DoViewFileExplorer(false);
    }

    else if (idT == ID_VIEW_BOOKMARK) {

        if (m_pNotebookFilesys == NULL) {
            CreateFilesysNotebook();
            m_auiManager.Update();
        }

        if (m_pNotebookFilesys == NULL) {
            // Should never happen
            return;
        }

        wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pFilesysPanel);
        if (m_pListBookmark) {
            if (paneT.IsShown()) {
                int iIndexBookmark = m_pNotebookFilesys->GetPageIndex(m_pListBookmark);
                if (m_pNotebookFilesys->DeletePage(iIndexBookmark)) {
                    m_pListBookmark = NULL;
                    m_SigmaGUIPrefs.bViewBookmark = false;
                }
                if ((m_pExplorerTree == NULL) && (m_pListBookmark == NULL)) {
                    paneT.Hide();
                    m_auiManager.Update();
                }
            }
            else {
                if ((false == m_SigmaGUIPrefs.bViewExplorer) && m_pExplorerTree) {
                    int iIndexExplorer = m_pNotebookFilesys->GetPageIndex(m_pExplorerTree);
                    if (m_pNotebookFilesys->DeletePage(iIndexExplorer)) {
                        m_pExplorerTree = NULL;
                    }
                }
                paneT.Show();
                m_auiManager.Update();
                m_SigmaGUIPrefs.bViewBookmark = true;
            }
        }
        else {
            if (paneT.IsShown() == false) {
                if ((false == m_SigmaGUIPrefs.bViewExplorer) && m_pExplorerTree) {
                    int iIndexExplorer = m_pNotebookFilesys->GetPageIndex(m_pExplorerTree);
                    if (m_pNotebookFilesys->DeletePage(iIndexExplorer)) {
                        m_pExplorerTree = NULL;
                    }
                }
                paneT.Show();
                m_auiManager.Update();
            }
            m_SigmaGUIPrefs.bViewBookmark = true;
            if (CreateBookmarkWindow()) {
                int iIndexBookmark = m_pNotebookFilesys->GetPageIndex(m_pListBookmark);
                m_pNotebookFilesys->SetSelection(iIndexBookmark);
            }
            else {
                m_SigmaGUIPrefs.bViewBookmark = false;
            }
        }
    }

    else if (idT == ID_VIEW_CONSOLE) {

        if (m_pNotebookToolbox == NULL) {
            CreateToolNotebook();
            m_auiManager.Update();
        }

        if (m_pNotebookToolbox == NULL) {
            // Should never happen
            return;
        }

        wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pToolPanel);
        if (m_pEditConsole) {
            if (paneT.IsShown()) {
                int iIndexConsole = m_pNotebookToolbox->GetPageIndex(m_pEditConsole);
                if (m_pNotebookToolbox->DeletePage(iIndexConsole)) {
                    m_pEditConsole = NULL;
                    m_SigmaGUIPrefs.bViewConsole = false;
                }
                if ((m_pEditConsole == NULL) && (m_pEditOutput == NULL)) {
                    paneT.Hide();
                    m_auiManager.Update();
                }
            }
            else {
                if ((false == m_SigmaGUIPrefs.bViewOutput) && m_pEditOutput) {
                    int iIndexOutput = m_pNotebookToolbox->GetPageIndex(m_pEditOutput);
                    if (m_pNotebookToolbox->DeletePage(iIndexOutput)) {
                        m_pEditOutput = NULL;
                    }
                }
                paneT.Show();
                m_auiManager.Update();
                m_SigmaGUIPrefs.bViewConsole = true;
            }
        }
        else {
            if (paneT.IsShown() == false) {
                if ((false == m_SigmaGUIPrefs.bViewOutput) && m_pEditOutput) {
                    int iIndexOutput = m_pNotebookToolbox->GetPageIndex(m_pEditOutput);
                    if (m_pNotebookToolbox->DeletePage(iIndexOutput)) {
                        m_pEditOutput = NULL;
                    }
                }
                paneT.Show();
                m_auiManager.Update();
            }
            m_SigmaGUIPrefs.bViewConsole = true;
            if (CreateConsoleWindow()) {
                int iIndexConsole = m_pNotebookToolbox->GetPageIndex(m_pEditConsole);
                m_pNotebookToolbox->SetSelection(iIndexConsole);
            }
            else {
                OutputStatusbar(uT("Cannot initialize the output window"), SIGMAFRAME_TIMER_SHORT);
                m_SigmaGUIPrefs.bViewConsole = false;
            }
        }
    }

    else if (idT == ID_VIEW_OUTPUT) {

        if (m_pNotebookToolbox == NULL) {
            CreateToolNotebook();
            m_auiManager.Update();
        }

        if (m_pNotebookToolbox == NULL) {
            // Should never happen
            OutputStatusbar(uT("Cannot initialize the output window"), SIGMAFRAME_TIMER_SHORT);
            return;
        }

        wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pToolPanel);
        if (m_pEditOutput) {
            if (paneT.IsShown()) {
                int iIndexOutput = m_pNotebookToolbox->GetPageIndex(m_pEditOutput);
                if (m_pNotebookToolbox->DeletePage(iIndexOutput)) {
                    m_pEditOutput = NULL;
                    m_SigmaGUIPrefs.bViewOutput = false;
                }
                if ((m_pEditConsole == NULL) && (m_pEditOutput == NULL)) {
                    paneT.Hide();
                    m_auiManager.Update();
                }
            }
            else {
                if ((false == m_SigmaGUIPrefs.bViewConsole) && m_pEditConsole) {
                    int iIndexConsole = m_pNotebookToolbox->GetPageIndex(m_pEditConsole);
                    if (m_pNotebookToolbox->DeletePage(iIndexConsole)) {
                        m_pEditConsole = NULL;
                    }
                }
                paneT.Show();
                m_auiManager.Update();
                m_SigmaGUIPrefs.bViewOutput = true;
            }
        }
        else {
            if (paneT.IsShown() == false) {
                if ((false == m_SigmaGUIPrefs.bViewConsole) && m_pEditConsole) {
                    int iIndexConsole = m_pNotebookToolbox->GetPageIndex(m_pEditConsole);
                    if (m_pNotebookToolbox->DeletePage(iIndexConsole)) {
                        m_pEditConsole = NULL;
                    }
                }
                paneT.Show();
                m_auiManager.Update();
            }
            m_SigmaGUIPrefs.bViewOutput = true;
            if (CreateOutputWindow()) {
                int iIndexOutput = m_pNotebookToolbox->GetPageIndex(m_pEditOutput);
                m_pNotebookToolbox->SetSelection(iIndexOutput);
            }
            else {
                m_SigmaGUIPrefs.bViewOutput = false;
            }
        }
    }

    else if (idT == ID_VIEW_TOOLBAR) {
        m_SigmaGUIPrefs.bViewToolbar = (GetToolBar()->IsShown() == false);
        GetToolBar()->Show(m_SigmaGUIPrefs.bViewToolbar);
        this->SendSizeEvent();
    }

    else if (idT == ID_VIEW_STATUSBAR) {
        m_SigmaGUIPrefs.bViewStatusbar = (GetStatusBar()->IsShown() == false);
        GetStatusBar()->Show(m_SigmaGUIPrefs.bViewStatusbar);
        this->SendSizeEvent();
    }

    else if (idT == ID_VIEW_SEARCHBAR) {
        this->viewSearchToolbar();
    }
}

void CometFrame::OnFontChoose(wxCommandEvent &WXUNUSED(tEvent))
{
    int iFontWeight = (m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].fontstyle == SCRIPT_STYLE_BOLD) ? wxBOLD : wxNORMAL;
    int iFontFamily = wxMODERN;
    int iFontStyle = wxNORMAL;
    wxFontEncoding fontEncod = wxFONTENCODING_DEFAULT;

    char_t szFontName[LM_STRSIZES];
    memset(szFontName, 0, LM_STRSIZES * sizeof(char_t));
    wxStrcpy(szFontName, static_cast<const char_t *>(m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].fontname));
    int iFontSize = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].fontsize;
    wxFont *pFont = new (std::nothrow) wxFont(iFontSize, iFontFamily, iFontStyle, iFontWeight, false, szFontName, fontEncod);

    if (pFont == NULL) {
        return;
    }

    wxFont fontT(*pFont);

#ifdef __WXMSW__
    pFont->FreeResource(true);
#endif
    delete pFont;
    pFont = NULL;

    wxFontData dataT;
    dataT.SetInitialFont(fontT);

    wxFontDialog dlgT(this, dataT);
    if (dlgT.ShowModal() == wxID_OK) {

        dataT = dlgT.GetFontData();
        fontT = dataT.GetChosenFont();

        int ii;

        for (ii = 0; ii < STYLEINFO_COUNT; ii++) {
            wxStrcpy(m_ScintillaPrefs.style[ii].fontname, LM_CSTR(fontT.GetFaceName()));
            m_ScintillaPrefs.style[ii].fontsize = fontT.GetPointSize();
            if (fontT.GetWeight() == wxFONTWEIGHT_BOLD) {
                m_ScintillaPrefs.style[ii].fontstyle = SCRIPT_STYLE_BOLD;
            }
            else {
                m_ScintillaPrefs.style[ii].fontstyle = SCRIPT_STYLE_NORMAL;
            }
        }

        const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

        if (iPageCount > 0) {
            ScriptEdit *pEdit = NULL;
            for (ii = 0; ii < iPageCount; ii++) {
                pEdit = getEditor(ii);
                if (pEdit == NULL) {
                    break;
                }
                pEdit->setPrefs(m_ScintillaPrefs, true, false);
            }
        }

        ConsoleEdit *pConsole = getConsole();
        if (pConsole) {
            pConsole->setPrefs(m_ScintillaPrefs, true);
        }

        OutputEdit *pOutput = getOutput();
        if (pOutput) {
            pOutput->setPrefs(m_ScintillaPrefs, true);
        }
    }
}

void CometFrame::OnScriptStart(wxCommandEvent &WXUNUSED(tEvent))
{
    scriptStart();
}

void CometFrame::OnScriptStop(wxCommandEvent &WXUNUSED(tEvent))
{
    scriptStop();
}

void CometFrame::OnScriptKillTool(wxCommandEvent &WXUNUSED(tEvent))
{
    toolStop();
}

void CometFrame::OnScriptStepInto(wxCommandEvent &WXUNUSED(tEvent))
{
    ScriptEdit *pEdit = this->getActiveEditor();
    if (pEdit == NULL) {
        return;
    }
    pEdit->enableStepInto(pEdit->stepInto() == false);
}

void CometFrame::OnScriptStartDebug(wxCommandEvent &WXUNUSED(tEvent))
{
    ScriptEdit *pEdit = this->getActiveEditor();
    if (pEdit == NULL) {
        return;
    }

    if (pEdit->isRunning()) {
        pEdit->luaDebugScript();
        return;
    }

    pEdit->luaDebugScript();
    scriptStart();
}

bool CometFrame::DoScriptChange(int iPrevSel)
{
    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        return false;
    }

    pEdit->updateStatusbar(true);

    DoAnalyzerUpdateList(pEdit, false);

    updateTitle(pEdit->GetFilename());

    if (pEdit->isRunning()) {
        if (m_pInfobarMain) {
            m_pInfobarMain->Refresh();
        }
        return false;
    }

    // Change current dir
    wxFileName fname = pEdit->GetFilename();
    wxString strT = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    if (strT.Length() > 2) {
        // ::ANSI::    NOT::UNICODE::
        char szT[LM_STRSIZE];
        memset(szT, 0, LM_STRSIZE * sizeof(char));
        strcpy(szT, LM_U8STR(strT));
        chdir(szT);
        //
    }
    //

    updateCodeSample();

    if (m_pInfobarMain) {
        m_pInfobarMain->Refresh();
    }

    return true;
}

void CometFrame::OnDoubleClick(wxMouseEvent &tEvent)
{
    tEvent.Skip();

    if ((m_pStatusBar == NULL) || (m_pStatusBar->IsShown() == false)) {
        return;
    }

    wxPoint tPoint = tEvent.GetPosition();

    wxRect tRect;
    m_pStatusBar->GetFieldRect(1, tRect);

    bool bFlag = false;
    if (tRect.Contains(tPoint)) {
        bFlag = true;
    }
    else {
        m_pStatusBar->GetFieldRect(2, tRect);
        if (tRect.Contains(tPoint)) {
            bFlag = true;
        }
    }

    if (bFlag) {
        ScriptEdit *pEdit = getActiveEditor();
        if (pEdit == NULL) {
            return;
        }
        pEdit->DoScriptAction(ID_SEARCH_GOTO);
    }
}

void CometFrame::OnFilesysContextMenu(wxAuiNotebookEvent &tEvent)
{
    tEvent.Skip();

    if (m_pNotebookFilesys == NULL) {
        return;
    }

    const int iPageCount = (const int)(m_pNotebookFilesys->GetPageCount());

    if (iPageCount < 1) {
        return;
    }

    int iSel = tEvent.GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        return;
    }

    m_pNotebookFilesys->SetSelection(iSel);

    int iExplorerIndex = (m_pExplorerTree) ? m_pNotebookFilesys->GetPageIndex(m_pExplorerTree) : -1;
    int iBookmarkIndex = (m_pListBookmark && (getBookmarkCount() > 0)) ? m_pNotebookFilesys->GetPageIndex(m_pListBookmark) : -1;
    if ((iSel != iBookmarkIndex) && (iSel != iExplorerIndex)) {
        return;
    }

    wxMenu popMenu;
    wxMenuItem *pItem = (iSel == iBookmarkIndex) ? new wxMenuItem(&popMenu, ID_BOOKMARK_DELETEALL, uT("Delete All Bookmarks"), uT("Delete all bookmarks"), wxITEM_NORMAL) : new wxMenuItem(&popMenu, ID_EXPLORER_BROWSE, uT("Change Directory..."), uT("Change the working directory"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap((iSel == iBookmarkIndex) ? reset_small_xpm : folder_small_xpm);
#endif
    popMenu.Append(pItem);
    wxPoint ptMousePosition = GetMouseToolbarPos();
    PopupMenu(&popMenu, ptMousePosition);
}

void CometFrame::OnToolboxChange(wxAuiNotebookEvent &tEvent)
{
    tEvent.Skip();
}

void CometFrame::OnToolboxContextMenu(wxAuiNotebookEvent &tEvent)
{
    tEvent.Skip();

    int iOutputIndex = (m_pNotebookToolbox && m_pEditOutput && (m_pEditOutput->GetLength() > 0)) ? m_pNotebookToolbox->GetPageIndex(m_pEditOutput) : -1;
    if (iOutputIndex < 0) {
        return;
    }

    wxMenu popMenu;
    wxMenuItem *pItem = new wxMenuItem(&popMenu, ID_OUTPUT_RESET, uT("Reset"), uT("Clear the output window"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(reset_small_xpm);
#endif
    popMenu.Append(pItem);
    wxPoint ptMousePosition = GetMouseToolbarPos();
    PopupMenu(&popMenu, ptMousePosition);
}

void CometFrame::OnPaneClose(wxAuiManagerEvent &tEvent)
{
    if (m_bClosed) {
        // On app exit, close all panes normally
        tEvent.Skip();
        return;
    }

    if (tEvent.pane == NULL) {
        // Should never happen
        return;
    }

    if (m_pMainPanel != NULL) {
        wxAuiPaneInfo &paneMain = m_auiManager.GetPane(m_pMainPanel);
        if (paneMain.name == tEvent.pane->name) {
            // Never close notebook window, only hide it
            tEvent.Veto();
            return;
        }
    }

    if (m_pFilesysPanel != NULL) {
        wxAuiPaneInfo &paneA = m_auiManager.GetPane(m_pFilesysPanel);
        if (paneA.name == tEvent.pane->name) {
            // Never close notebook window, only hide it
            tEvent.Veto();
            tEvent.pane->Hide();
            m_auiManager.Update();
            m_SigmaGUIPrefs.bViewBookmark = false;
            m_SigmaGUIPrefs.bViewExplorer = false;
            return;
        }
    }

    if (m_pToolPanel != NULL) {
        wxAuiPaneInfo &paneB = m_auiManager.GetPane(m_pToolPanel);
        if (paneB.name == tEvent.pane->name) {
            // Never close notebook window, only hide it
            tEvent.Veto();
            tEvent.pane->Hide();
            m_auiManager.Update();
            m_SigmaGUIPrefs.bViewOutput = false;
            m_SigmaGUIPrefs.bViewConsole = false;
            return;
        }
    }

    if (m_pDebugPanel != NULL) {
        wxAuiPaneInfo &paneC = m_auiManager.GetPane(m_pDebugPanel);
        if (paneC.name == tEvent.pane->name) {
            // Never close notebook window, only hide it
            tEvent.Veto();
            tEvent.pane->Hide();
            m_auiManager.Update();
            return;
        }
    }
}

void CometFrame::OnFilesysClose(wxAuiNotebookEvent &tEvent)
{
    if (m_pNotebookFilesys == NULL) {
        return;
    }

    int iExplorerIndex = (m_pExplorerTree) ? m_pNotebookFilesys->GetPageIndex(m_pExplorerTree) : -1;
    int iBookmarkIndex = (m_pListBookmark) ? m_pNotebookFilesys->GetPageIndex(m_pListBookmark) : -1;

    int iSel = m_pNotebookFilesys->GetSelection();
    if ((iSel < 0) || (iSel >= (int)(m_pNotebookFilesys->GetPageCount()))) {
        tEvent.Veto();
        return;
    }
    if ((iSel != iExplorerIndex) && (iSel != iBookmarkIndex)) {
        tEvent.Veto();
        return;
    }

    if (m_pExplorerTree && (iSel == iExplorerIndex)) {

        tEvent.Skip();
        m_pExplorerTree = NULL;
        m_SigmaGUIPrefs.bViewExplorer = false;
    }

    else if (m_pListBookmark && (iSel == iBookmarkIndex)) {

        tEvent.Skip();
        m_pListBookmark = NULL;
        m_SigmaGUIPrefs.bViewBookmark = false;
    }

    if ((m_pExplorerTree == NULL) && (m_pListBookmark == NULL)) {
        wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pFilesysPanel);
        paneT.Hide();
        m_auiManager.Update();
    }
}

void CometFrame::OnToolboxClose(wxAuiNotebookEvent &tEvent)
{
    int iConsoleIndex = (m_pEditConsole) ? m_pNotebookToolbox->GetPageIndex(m_pEditConsole) : -1;
    int iOutputIndex = (m_pEditOutput) ? m_pNotebookToolbox->GetPageIndex(m_pEditOutput) : -1;

    int iSel = m_pNotebookToolbox->GetSelection();
    if ((iSel < 0) || (iSel >= (int)(m_pNotebookToolbox->GetPageCount()))) {
        tEvent.Veto();
        return;
    }
    if ((iSel != iConsoleIndex) && (iSel != iOutputIndex)) {
        tEvent.Veto();
        return;
    }

    if (m_pEditConsole && (iSel == iConsoleIndex)) {
        if (m_pEditConsole->isRunning()) {
            tEvent.Veto();
            return;
        }

        tEvent.Skip();
        m_pEditConsole = NULL;
        m_SigmaGUIPrefs.bViewConsole = false;
    }

    else if (m_pEditOutput && (iSel == iOutputIndex)) {

        tEvent.Skip();
        m_pEditOutput = NULL;
        m_SigmaGUIPrefs.bViewOutput = false;
    }

    if ((m_pEditConsole == NULL) && (m_pEditOutput == NULL)) {
        wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pToolPanel);
        paneT.Hide();
        m_auiManager.Update();
    }
}

// Sort wxListCtrl
int listDoCompare(long itemL, long itemR, long listPtr, int iCol, bool bAsc)
{
    if (itemL == itemR) {
        return 0;
    }

    wxListCtrl *pListCtrl = (wxListCtrl *)listPtr;
    if (pListCtrl == NULL) {
        return 0;
    }

    wxListItem listItemL, listItemR;
    listItemL.m_itemId = itemL;
    listItemL.m_col = iCol;
    listItemR.m_itemId = itemR;
    listItemR.m_col = iCol;
    listItemL.m_mask = listItemR.m_mask = wxLIST_MASK_TEXT;
    if (pListCtrl->GetItem(listItemL) && pListCtrl->GetItem(listItemR)) {
        wxString strL = (bAsc) ? listItemL.m_text : listItemR.m_text;
        wxString strR = (bAsc) ? listItemR.m_text : listItemL.m_text;
        int iC = strL.Cmp(strR);
        return (iC < 0) ? -1 : ((iC > 0) ? 1 : 0);
    }
    return 0;
}
int wxCALLBACK listCompareCol0Asc(long itemL, long itemR, long listPtr)
{
    return listDoCompare(itemL, itemR, listPtr, 0, true);
}
int wxCALLBACK listCompareCol1Asc(long itemL, long itemR, long listPtr)
{
    return listDoCompare(itemL, itemR, listPtr, 1, true);
}
int wxCALLBACK listCompareCol2Asc(long itemL, long itemR, long listPtr)
{
    return listDoCompare(itemL, itemR, listPtr, 2, true);
}
int wxCALLBACK listCompareCol3Asc(long itemL, long itemR, long listPtr)
{
    return listDoCompare(itemL, itemR, listPtr, 3, true);
}
int wxCALLBACK listCompareCol0Desc(long itemL, long itemR, long listPtr)
{
    return listDoCompare(itemL, itemR, listPtr, 0, false);
}
int wxCALLBACK listCompareCol1Desc(long itemL, long itemR, long listPtr)
{
    return listDoCompare(itemL, itemR, listPtr, 1, false);
}
int wxCALLBACK listCompareCol2Desc(long itemL, long itemR, long listPtr)
{
    return listDoCompare(itemL, itemR, listPtr, 2, false);
}
int wxCALLBACK listCompareCol3Desc(long itemL, long itemR, long listPtr)
{
    return listDoCompare(itemL, itemR, listPtr, 3, false);
}
//

void CometFrame::OnDebugHeaderClick(wxListEvent &tEvent)
{
    wxListCtrl *pList = NULL;
    int iSorted = CometFrame::LIST_SORTDIRECTION::ASC;
    if ((tEvent.GetId() == IDL_DEBUG_WATCH) && m_pListWatch) {
        pList = m_pListWatch;
        if (m_iListWatchSorted == CometFrame::LIST_SORTDIRECTION::ASC) {
            m_iListWatchSorted = CometFrame::LIST_SORTDIRECTION::DESC;
        }
        else {
            m_iListWatchSorted = CometFrame::LIST_SORTDIRECTION::ASC;
        }
        iSorted = m_iListWatchSorted;
    }
    else if ((tEvent.GetId() == IDL_DEBUG_STACK) && m_pListStack) {
        pList = m_pListStack;
        if (m_iListStackSorted == CometFrame::LIST_SORTDIRECTION::ASC) {
            m_iListStackSorted = CometFrame::LIST_SORTDIRECTION::DESC;
        }
        else {
            m_iListStackSorted = CometFrame::LIST_SORTDIRECTION::ASC;
        }
        iSorted = m_iListStackSorted;
    }
    if (pList == NULL) {
        return;
    }

    long itemT = -1;
    bool bFlag = false;
    for (int ii = 0; ii < 128; ii++) {
        itemT = pList->GetNextItem(itemT);
        if (itemT == -1) {
            bFlag = true;
            break;
        }
        pList->SetItemData(itemT, itemT);
    }
    if (bFlag == false) {
        return;
    }

    int iCol = tEvent.GetColumn();
    if (iCol == 0) {
        pList->SortItems((iSorted == CometFrame::LIST_SORTDIRECTION::ASC) ? listCompareCol0Asc : listCompareCol0Desc, (long)pList);
    }
    else if (iCol == 1) {
        pList->SortItems((iSorted == CometFrame::LIST_SORTDIRECTION::ASC) ? listCompareCol1Asc : listCompareCol1Desc, (long)pList);
    }
    else if (iCol == 2) {
        pList->SortItems((iSorted == CometFrame::LIST_SORTDIRECTION::ASC) ? listCompareCol2Asc : listCompareCol2Desc, (long)pList);
    }
    else if (iCol == 3) {
        pList->SortItems((iSorted == CometFrame::LIST_SORTDIRECTION::ASC) ? listCompareCol3Asc : listCompareCol3Desc, (long)pList);
    }
}

void CometFrame::OnDebugClose(wxAuiNotebookEvent &tEvent)
{
    tEvent.Veto();
    return;
}

void CometFrame::OnHelpContentsPdf(wxCommandEvent &WXUNUSED(tEvent))
{
    wxGetApp().displayHelp();
}

void CometFrame::OnHelpExample(wxCommandEvent &tEvent)
{
    if ((m_pExample != m_pExampleLua) || (m_pExampleLua == NULL)) {
        return;
    }

    int idT = tEvent.GetId();

    const char_t *pszExample = m_pExample->getCode(idT - ID_SCRIPT_EXAMPLE_FIRST);
    const char_t *pszTitle = m_pExample->getTitle(idT - ID_SCRIPT_EXAMPLE_FIRST);
    if ((pszExample == NULL) || (pszTitle == NULL)) {
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if ((idT < ID_SCRIPT_EXAMPLE_FIRST) || (idT > ID_SCRIPT_EXAMPLE_LAST)) {
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

    if (pEdit->isRunning()) {
        pEdit->SetFocus();
        wxString strT = pEdit->isDebugging() ? uT("Cannot load example: the active script is in debugging session.") : uT("Cannot load example: the active script is running.");
        OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        SigmaMessageBox(strT, uT("Comet"), wxOK | wxICON_INFORMATION, this);
        return;
    }

    if (pEdit->isModified()) {
        wxString strT = m_pNotebookMain->GetPageText(iSel);
        strT += uT(" not saved. Save it before loading example?");
        int iret = SigmaMessageBox(strT, uT("Comet"),
                                   wxYES | wxNO | wxCANCEL | wxICON_QUESTION, this,
                                   uT("&Save"), uT("&Don't save"));
        if (iret == wxCANCEL) {
            tEvent.Skip();
            return;
        }
        if (iret == wxYES) {
            fileSave(iSel);
        }
    }

    wxString strExample(pszExample);

    if (pEdit->DoNewFile(m_ScintillaPrefs, strExample, wxSTC_LEX_LUA, 0) == false) {
        tEvent.Skip();
        return;
    }
    this->DoAnalyzerUpdateList(pEdit, false);

    m_pNotebookMain->SetPageText(iSel, pszTitle);
}

void CometFrame::OnInsertCodeSnippet(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();
    if ((idT < ID_SCRIPT_SNIPPET_FIRST) || (idT > ID_SCRIPT_SNIPPET_LAST)) {
        tEvent.Skip();
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

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

    pEdit->DoInsertCodeSnippet(idT);
}

bool CometFrame::isStatusbarEmpty(void)
{
    if ((m_pStatusBar == NULL) || (m_pStatusBar->IsShown() == false)) {
        return false;
    }

    wxString strT = m_pStatusBar->GetStatusText(0);

    return (strT.IsEmpty() || strT.IsSameAs(uT("Ready")));
}

void CometFrame::OutputStatusbar(const wxString &strT, int iMilliseconds /* = SIGMAFRAME_TIMER_SHORT*/)
{
    static const int iInterval = 1000;

    if ((m_pStatusBar == NULL) || (m_pStatusBar->IsShown() == false)) {
        return;
    }

    if (m_pTimer) {
        m_pTimer->Stop();
        delete m_pTimer;
        m_pTimer = NULL;
        m_iTimerCounter = 0;
    }

    m_pStatusBar->SetStatusText(strT, 0);

    if ((strT.IsEmpty() == false) && (strT.IsSameAs(uT("Ready")) == false) && (iMilliseconds > iInterval)) {
        if (iMilliseconds < 5000) {
            iMilliseconds = 5000;
        }
        if (iMilliseconds > 120000) {
            iMilliseconds = 120000;
        }
        m_iTimerDuration = iMilliseconds;
        try {
            m_pTimer = new wxTimer(this, TIMER_ID_SIGMAFRAME);
        }
        catch (...) {
            m_pTimer = NULL;
        }
        if (m_pTimer) {
            m_iTimerCounter = 0;
            m_pTimer->Start(iInterval);
        }
    }
}

void CometFrame::OnTimer(wxTimerEvent &tEvent)
{
    static const int iInterval = 1000;

    if ((m_pTimer == NULL) || (tEvent.GetId() != TIMER_ID_SIGMAFRAME)) {
        tEvent.Skip();
        return;
    }

    m_iTimerCounter += iInterval;
    if (m_iTimerCounter >= m_iTimerDuration) {
        m_pTimer->Stop();
        delete m_pTimer;
        m_pTimer = NULL;
        m_iTimerCounter = 0;
        m_pStatusBar->SetStatusText(uT("Ready"), 0);
        return;
    }
}

void CometFrame::OnHelpAbout(wxCommandEvent &WXUNUSED(tEvent))
{
    wxAboutDialogInfo infoT;
    infoT.SetName(uT("Comet"));
    wxString strVersion = wxString(uT("Version "));
    strVersion += wxString(COMET_VERSION_STRING);
    strVersion += wxString(uT(" Build "));
    strVersion += wxString(COMET_BUILD_STRING);
    infoT.SetVersion(strVersion);
    infoT.SetDescription(uT("Programming Environment for Lua"));
    infoT.SetCopyright(COMET_BUILD_COPYRIGHT);
    infoT.SetWebSite(uT("http://www.hamady.org"));
    infoT.SetLicence(uT("Comet:\n")
        uT("* is free of charge only for non-commercial use ;\n")
        uT("* is provided \'As Is\' without any warranty of any kind ;\n")
        uT("* is protected by copyright laws and international copyright treaties ;\n")
        uT("* may not be redistributed without authorization of the author.\n")
        uT("\nComet uses:\n")
        uT("* the Lua Programming Language, (C) 1994-2014 Tecgraf, PUC-Rio.\n")
        uT("* the Lua Just-In-Time Compiler, (C) 2005-2017 Mike Pall.\n")
        uT("* the Scintilla Component, (C) 1998-2004 Neil Hodgson.\n")
        uT("* the wxWidgets GUI toolkit, (C) 1992-2013 the wxWidgets team.\n"));
    wxAboutBox(infoT, this);
}

// --------------------------------------------------------------------
// Backport from Solis

static void CometCheckUpdate(CometFrame *pFrame, const char_t *pszName);

void CometFrame::OnCheckUpdate(wxCommandEvent &WXUNUSED(tEvent))
{
    CometCheckUpdate(this, uT("Comet"));
}

static void checkupdateShowWindow(void);

void CometFrame::OnCheckUpdateEnd(wxCommandEvent &tEvent)
{
    checkupdateShowWindow();
}

#define SEC_PER_MIN  60UL
#define SEC_PER_HOUR 3600UL
#define SEC_PER_DAY  86400UL
#define SEC_PER_YEAR (SEC_PER_DAY * 365)

#define CONV_STR2DEC_1(str, i) ((str[i] > '0') ? (str[i] - '0') : 0)
#define CONV_STR2DEC_2(str, i) ((CONV_STR2DEC_1(str, i) * 10) + str[i + 1] - '0')
#define CONV_STR2DEC_3(str, i) ((CONV_STR2DEC_2(str, i) * 10) + str[i + 2] - '0')
#define CONV_STR2DEC_4(str, i) ((CONV_STR2DEC_3(str, i) * 10) + str[i + 3] - '0')

#define GET_MONTH(str, i) (str[i] == 'J' && str[i + 1] == 'a' && str[i + 2] == 'n' ? 1 : str[i] == 'F' && str[i + 1] == 'e' && str[i + 2] == 'b' ? 2  \
                                                                                       : str[i] == 'M' && str[i + 1] == 'a' && str[i + 2] == 'r'   ? 3  \
                                                                                       : str[i] == 'A' && str[i + 1] == 'p' && str[i + 2] == 'r'   ? 4  \
                                                                                       : str[i] == 'M' && str[i + 1] == 'a' && str[i + 2] == 'y'   ? 5  \
                                                                                       : str[i] == 'J' && str[i + 1] == 'u' && str[i + 2] == 'n'   ? 6  \
                                                                                       : str[i] == 'J' && str[i + 1] == 'u' && str[i + 2] == 'l'   ? 7  \
                                                                                       : str[i] == 'A' && str[i + 1] == 'u' && str[i + 2] == 'g'   ? 8  \
                                                                                       : str[i] == 'S' && str[i + 1] == 'e' && str[i + 2] == 'p'   ? 9  \
                                                                                       : str[i] == 'O' && str[i + 1] == 'c' && str[i + 2] == 't'   ? 10 \
                                                                                       : str[i] == 'N' && str[i + 1] == 'o' && str[i + 2] == 'v'   ? 11 \
                                                                                       : str[i] == 'D' && str[i + 1] == 'e' && str[i + 2] == 'c'   ? 12 \
                                                                                                                                                 : 0)

#define __TIME_SECONDS__ CONV_STR2DEC_2(__TIME__, 6)
#define __TIME_MINUTES__ CONV_STR2DEC_2(__TIME__, 3)
#define __TIME_HOURS__   CONV_STR2DEC_2(__TIME__, 0)
#define __TIME_DAYS__    CONV_STR2DEC_2(__DATE__, 4)
#define __TIME_MONTH__   GET_MONTH(__DATE__, 0)
#define __TIME_YEARS__   CONV_STR2DEC_4(__DATE__, 7)

#define _UNIX_TIMESTAMP_FDAY(year) \
    (((year) % 400) == 0UL ? 29UL : (((year) % 100) == 0UL ? 28UL : (((year) % 4) == 0UL ? 29UL : 28UL)))

#define _UNIX_TIMESTAMP_YDAY(year, month, day) \
    (                                          \
        day + (month >= 2 ? 31UL : 0UL) + (month >= 3 ? _UNIX_TIMESTAMP_FDAY(year) : 0UL) + (month >= 4 ? 31UL : 0UL) + (month >= 5 ? 30UL : 0UL) + (month >= 6 ? 31UL : 0UL) + (month >= 7 ? 30UL : 0UL) + (month >= 8 ? 31UL : 0UL) + (month >= 9 ? 31UL : 0UL) + (month >= 10 ? 30UL : 0UL) + (month >= 11 ? 31UL : 0UL) + (month >= 12 ? 30UL : 0UL))

#define _UNIX_TIMESTAMP(year, month, day, hour, minute, second) \
    (second + minute * SEC_PER_MIN + hour * SEC_PER_HOUR + (_UNIX_TIMESTAMP_YDAY(year, month, day) - 1) * SEC_PER_DAY + (year - 1970UL) * SEC_PER_YEAR + ((year - 1969UL) / 4UL) * SEC_PER_DAY - ((year - 1901UL) / 100UL) * SEC_PER_DAY + ((year - 1601UL) / 400UL) * SEC_PER_DAY)

#define UNIX_TIMESTAMP (_UNIX_TIMESTAMP(__TIME_YEARS__, __TIME_MONTH__, __TIME_DAYS__, __TIME_HOURS__, __TIME_MINUTES__, __TIME_SECONDS__))

#ifndef WIN32
#include <glib.h>
#endif

typedef struct _checkupdate_t
{
    CometFrame *frame;
    char_t name[LM_STRSIZEN];
    char_t timestamp[LM_STRSIZEM];
    bool running;
#ifdef WIN32
    uintptr_t thread;
#else
    GThread *thread;
#endif
} checkupdate_t;
checkupdate_t g_checkupdate = {
    NULL,  // frame
    { 0 }, // name
    { 0 }, // timestamp
    false, // running
#ifdef WIN32
    0UL // thread
#else
    NULL // thread
#endif
};

static void checkupdateShowWindow(void)
{
#ifdef WIN32
    g_checkupdate.thread = 0UL;
#else
    g_checkupdate.thread = NULL;
#endif

    if (g_checkupdate.frame) {
        g_checkupdate.frame->OutputStatusbar(uT(""), SIGMAFRAME_TIMER_NONE);
    }

    size_t iLen = Tstrlen((const char_t *)(g_checkupdate.timestamp));
    if ((iLen < 5) || (iLen >= LM_STRSIZEM) || (g_checkupdate.timestamp[0] < uT('0')) || (g_checkupdate.timestamp[0] > uT('9')) || (g_checkupdate.timestamp[1] < uT('0')) || (g_checkupdate.timestamp[1] > uT('9')) || (g_checkupdate.timestamp[2] < uT('0')) || (g_checkupdate.timestamp[2] > uT('9')) || (g_checkupdate.timestamp[3] < uT('0')) || (g_checkupdate.timestamp[3] > uT('9')) || (g_checkupdate.timestamp[4] < uT('0')) || (g_checkupdate.timestamp[4] > uT('9'))) {
        SigmaMessageBox(uT("Unable to check for update.\nTo get the latest news: http://www.hamady.org"), uT("Comet"), wxOK, g_checkupdate.frame);
        g_checkupdate.running = FALSE;
        return;
    }
    size_t ii = iLen - 1;
    while (((uT(' ') == g_checkupdate.timestamp[ii]) || (uT('\t') == g_checkupdate.timestamp[ii]) || (uT('\r') == g_checkupdate.timestamp[ii]) || (uT('\n') == g_checkupdate.timestamp[ii])) && (ii >= 0)) {
        g_checkupdate.timestamp[ii] = uT('\0');
        --ii;
    }
    if (ii < 1) {
        SigmaMessageBox(uT("Unable to check for update.\nTo get the latest news: http://www.hamady.org"), uT("Comet"), wxOK, g_checkupdate.frame);
        g_checkupdate.running = FALSE;
        return;
    }
    iLen = ii + 1;
    uint64_t lStampNew = 0UL, lStampCurrent = 0UL;
    lStampNew = (uint64_t)wcstoll((const char_t *)(g_checkupdate.timestamp), NULL, 10);
    lStampCurrent = UNIX_TIMESTAMP;
    const uint64_t lStampMin = 1262304000UL;
    const uint64_t lStampMax = 2524608000UL;

    if ((lStampNew < lStampMin) || (lStampNew > lStampMax) || (lStampCurrent < lStampMin) || (lStampCurrent > lStampMax)) {
        SigmaMessageBox(uT("Unable to check for update.\nTo get the latest news: http://www.hamady.org"), uT("Comet"), wxOK, g_checkupdate.frame);
        g_checkupdate.running = FALSE;
        return;
    }

    if (lStampNew <= (lStampCurrent + 86400UL)) {
        SigmaMessageBox(uT("The Comet software is up-to-date."), uT("Comet"), wxOK, g_checkupdate.frame);
        g_checkupdate.running = FALSE;
        return;
    }

    char_t szTm[LM_STRSIZE + LM_STRSIZEM];
    Tmemset(szTm, 0, LM_STRSIZE + LM_STRSIZEM);
    time_t tmT = (time_t)lStampNew;
    struct tm stmT = { 0 };
    stmT = *localtime(&tmT);
    Tstrftime(szTm, LM_STRSIZE + LM_STRSIZEM - 1, uT("Programming Environment for Lua\nA new Comet version is available, released: %a %Y-%m-%d %H:%M:%S\nClick the link below to download, extract and overwrite the installed version"), &stmT);

    wxAboutDialogInfo infoT;
    infoT.SetName(uT("Comet"));
    wxString strVersion = wxString(uT("Version "));
    strVersion += wxString(COMET_VERSION_STRING);
    strVersion += wxString(uT(" Build "));
    strVersion += wxString(COMET_BUILD_STRING);
    infoT.SetVersion(strVersion);
    infoT.SetDescription(szTm);
    infoT.SetCopyright(COMET_BUILD_COPYRIGHT);
#ifdef _WIN32
#ifdef _AMD64_
    infoT.SetWebSite(uT("http://www.hamady.org/download/comet_windows_64bit.zip"));
#else
    infoT.SetWebSite(uT("http://www.hamady.org/download/comet_windows_32bit.zip"));
#endif
#else
#if defined(__LP64__) || defined(_LP64)
    infoT.SetWebSite(uT("http://www.hamady.org/download/comet_linux_64bit.tgz"));
#else
    infoT.SetWebSite(uT("http://www.hamady.org/download/comet_linux_32bit.tgz"));
#endif
#endif
    infoT.SetLicence(uT("Comet:\n")
        uT("* is free of charge only for non-commercial use ;\n")
        uT("* is provided \'As Is\' without any warranty of any kind ;\n")
        uT("* is protected by copyright laws and international copyright treaties ;\n")
        uT("* may not be redistributed without authorization of the author.\n")
        uT("\nComet uses:\n")
        uT("* the Lua Programming Language, (C) 1994-2014 Tecgraf, PUC-Rio.\n")
        uT("* the Lua Just-In-Time Compiler, (C) 2005-2017 Mike Pall.\n")
        uT("* the Scintilla Component, (C) 1998-2004 Neil Hodgson.\n")
        uT("* the wxWidgets GUI toolkit, (C) 1992-2013 the wxWidgets team.\n"));
    wxAboutBox(infoT, g_checkupdate.frame);

    g_checkupdate.running = FALSE;
    return;
}

#ifdef WIN32

#include <process.h>

#ifndef HINTERNET
// from winhttp.h
// no need to include the whole stuff
// I just need few basic functions
typedef LPVOID HINTERNET;
typedef HINTERNET *LPHINTERNET;
typedef WORD INTERNET_PORT;
typedef INTERNET_PORT *LPINTERNET_PORT;
#define WINHTTP_ACCESS_TYPE_DEFAULT_PROXY 0
#define WINHTTP_ACCESS_TYPE_NO_PROXY      1
#define WINHTTP_ACCESS_TYPE_NAMED_PROXY   3
#define WINHTTP_NO_PROXY_NAME             NULL
#define WINHTTP_NO_PROXY_BYPASS           NULL
#define INTERNET_DEFAULT_PORT             0   // use the protocol-specific default
#define INTERNET_DEFAULT_HTTP_PORT        80  //    "     "  HTTP   "
#define INTERNET_DEFAULT_HTTPS_PORT       443 //    "     "  HTTPS  "
#define WINHTTP_NO_REFERER                NULL
#define WINHTTP_DEFAULT_ACCEPT_TYPES      NULL
#define WINHTTP_NO_ADDITIONAL_HEADERS     NULL
#define WINHTTP_NO_REQUEST_DATA           NULL
#endif

static unsigned int __stdcall checkupdateThreadRun(void *data)
{
    checkupdate_t *pcheckupdate = (struct _checkupdate_t *)data;
    if (NULL == pcheckupdate) {
        // should never happen
        return FALSE;
    }
    const size_t bufferSize = (size_t)LM_STRSIZEM;
    Tmemset(pcheckupdate->timestamp, 0, bufferSize);

    if ((bufferSize < LM_STRSIZET) || (bufferSize > (LM_STRSIZE * LM_STRSIZE))) {
        return FALSE;
    }

    HMODULE winhttpDLL = LoadLibrary(uT("winhttp.dll"));
    if (NULL == winhttpDLL) {
        return FALSE;
    }

    DWORD dwBytesAvailable = 0;
    DWORD dwBytesRead = 0;
    BOOL bRet = FALSE;
    HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;

    typedef HINTERNET(WINAPI * PF_WINHTTPOPEN)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD);
    typedef HINTERNET(WINAPI * PF_WINHTTPCONNECT)(HINTERNET, LPCWSTR, INTERNET_PORT, DWORD);
    typedef HINTERNET(WINAPI * PF_WINHTTPOPENREQUEST)(HINTERNET, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, DWORD);
    typedef BOOL(WINAPI * PF_WINHTTPSENDREQUEST)(HINTERNET, LPCWSTR, DWORD, LPVOID, DWORD, DWORD, DWORD);
    typedef BOOL(WINAPI * PF_WINHTTPRECEIVERESPONSE)(HINTERNET, LPVOID);
    typedef BOOL(WINAPI * PF_WINHTTPQUERYDATAAVAILABLE)(HINTERNET, LPDWORD);
    typedef BOOL(WINAPI * PF_WINHTTPREADDATA)(HINTERNET, LPVOID, DWORD, LPDWORD);
    typedef BOOL(WINAPI * PF_WINHTTPCLOSEHANDLE)(HINTERNET);
    PF_WINHTTPOPEN pfWinHttpOpen = (PF_WINHTTPOPEN)GetProcAddress(winhttpDLL, "WinHttpOpen");
    PF_WINHTTPCONNECT pfWinHttpConnect = (PF_WINHTTPCONNECT)GetProcAddress(winhttpDLL, "WinHttpConnect");
    PF_WINHTTPOPENREQUEST pfWinHttpOpenRequest = (PF_WINHTTPOPENREQUEST)GetProcAddress(winhttpDLL, "WinHttpOpenRequest");
    PF_WINHTTPSENDREQUEST pfWinHttpSendRequest = (PF_WINHTTPSENDREQUEST)GetProcAddress(winhttpDLL, "WinHttpSendRequest");
    PF_WINHTTPRECEIVERESPONSE pfWinHttpReceiveResponse = (PF_WINHTTPRECEIVERESPONSE)GetProcAddress(winhttpDLL, "WinHttpReceiveResponse");
    PF_WINHTTPQUERYDATAAVAILABLE pfWinHttpQueryDataAvailable = (PF_WINHTTPQUERYDATAAVAILABLE)GetProcAddress(winhttpDLL, "WinHttpQueryDataAvailable");
    PF_WINHTTPREADDATA pfWinHttpReadData = (PF_WINHTTPREADDATA)GetProcAddress(winhttpDLL, "WinHttpReadData");
    PF_WINHTTPCLOSEHANDLE pfWinHttpCloseHandle = (PF_WINHTTPCLOSEHANDLE)GetProcAddress(winhttpDLL, "WinHttpCloseHandle");
    if ((NULL == pfWinHttpOpen) || (NULL == pfWinHttpConnect) || (NULL == pfWinHttpOpenRequest) || (NULL == pfWinHttpSendRequest) || (NULL == pfWinHttpReceiveResponse) || (NULL == pfWinHttpQueryDataAvailable) || (NULL == pfWinHttpReadData) || (NULL == pfWinHttpCloseHandle)) {
        FreeLibrary(winhttpDLL);
        winhttpDLL = NULL;
        return FALSE;
    }

    hSession = (*pfWinHttpOpen)((LPCWSTR)L"CometCheckUpdate", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (NULL == hSession) {
        FreeLibrary(winhttpDLL);
        winhttpDLL = NULL;
        return FALSE;
    }

    hConnect = (*pfWinHttpConnect)(hSession, L"solis.hamady.org", INTERNET_DEFAULT_HTTP_PORT, 0);
    if (NULL == hConnect) {
        (*pfWinHttpCloseHandle)(hSession);
        hSession = NULL;
        FreeLibrary(winhttpDLL);
        winhttpDLL = NULL;
        return FALSE;
    }

    hRequest = (*pfWinHttpOpenRequest)(hConnect, L"GET", L"/comet_release_date.txt", (LPCWSTR)NULL, (LPCWSTR)WINHTTP_NO_REFERER, (LPCWSTR)NULL, 0);
    if (NULL == hRequest) {
        (*pfWinHttpCloseHandle)(hConnect);
        hConnect = NULL;
        (*pfWinHttpCloseHandle)(hSession);
        hSession = NULL;
        FreeLibrary(winhttpDLL);
        winhttpDLL = NULL;
        return FALSE;
    }

    bRet = (*pfWinHttpSendRequest)(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    if (TRUE == bRet) {
        bRet = (*pfWinHttpReceiveResponse)(hRequest, NULL);
    }
    dwBytesAvailable = 0;
    if (TRUE == bRet) {
        bRet = (*pfWinHttpQueryDataAvailable)(hRequest, &dwBytesAvailable);
        if (dwBytesAvailable >= (DWORD)bufferSize) {
            dwBytesAvailable = (DWORD)(bufferSize - 1);
        }
    }
    if ((TRUE == bRet) && (dwBytesAvailable > 5)) {
        char *pszBuffer = (char *)malloc(dwBytesAvailable * sizeof(char));
        if (pszBuffer) {
            ZeroMemory(pszBuffer, dwBytesAvailable);
            dwBytesRead = 0;
            bRet = (*pfWinHttpReadData)(hRequest, (LPVOID)(pszBuffer), dwBytesAvailable, &dwBytesRead);
            if ((TRUE == bRet) && (dwBytesRead > 0) && (dwBytesRead <= dwBytesAvailable)) {
                pszBuffer[dwBytesRead] = uT('\0');
                mbstowcs(pcheckupdate->timestamp, (const char *)pszBuffer, dwBytesRead);
            }
            else {
                bRet = FALSE;
            }
            free(pszBuffer);
            pszBuffer = NULL;
        }
        else {
            bRet = FALSE;
        }
    }
    if ((FALSE == bRet) || (dwBytesAvailable <= 5)) {
        (*pfWinHttpCloseHandle)(hRequest);
        hRequest = NULL;
        (*pfWinHttpCloseHandle)(hConnect);
        hConnect = NULL;
        (*pfWinHttpCloseHandle)(hSession);
        hSession = NULL;
        FreeLibrary(winhttpDLL);
        winhttpDLL = NULL;
        return FALSE;
    }
    if (dwBytesAvailable > (DWORD)bufferSize) {
        dwBytesAvailable = (DWORD)bufferSize;
    }

    (*pfWinHttpCloseHandle)(hRequest);
    hRequest = NULL;
    (*pfWinHttpCloseHandle)(hConnect);
    hConnect = NULL;
    (*pfWinHttpCloseHandle)(hSession);
    hSession = NULL;
    FreeLibrary(winhttpDLL);
    winhttpDLL = NULL;

    size_t iLen = 0;
    iLen = Tstrlen((const char_t *)(pcheckupdate->timestamp));
    if (iLen > 5) {
        char_t *pszX = Tstrrchr((char_t *)(pcheckupdate->timestamp), uT('\r'));
        if (NULL == pszX) {
            pszX = Tstrrchr((char_t *)(pcheckupdate->timestamp), uT('\n'));
        }
        if (pszX) {
            *pszX = '\0';
            iLen = Tstrlen((const char_t *)(pcheckupdate->timestamp));
        }
    }

    wxCommandEvent eventCheckUpdate(wxEVT_COMMAND_TEXT_UPDATED, ID_THREAD_CHECKUPDATE);
    pcheckupdate->frame->GetEventHandler()->AddPendingEvent(eventCheckUpdate);

    return (iLen > 5) ? TRUE : FALSE;
}

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>

static gboolean checkupdateThreadFinish(gpointer data)
{
    checkupdate_t *pcheckupdate = (struct _checkupdate_t *)data;
    if (NULL == pcheckupdate) {
        // should never happen
        return FALSE;
    }
    pcheckupdate->thread = NULL;

    checkupdateShowWindow();

    return FALSE;
}

static gpointer checkupdateThreadRun(gpointer data)
{
    checkupdate_t *pcheckupdate = (struct _checkupdate_t *)data;
    if (NULL == pcheckupdate) {
        // should never happen
        return NULL;
    }
    const size_t bufferSize = (size_t)LM_STRSIZEM;
    Tmemset(pcheckupdate->timestamp, 0, bufferSize);

    if ((bufferSize < LM_STRSIZET) || (bufferSize > (LM_STRSIZE * LM_STRSIZE))) {
        g_idle_add(checkupdateThreadFinish, pcheckupdate);
        return NULL;
    }

    char tdomain[] = ("solis.hamady.org"),
           tpath[] = ("/comet_release_date.txt");

    int tsock = -1, bytesReceived = 0;
    struct sockaddr_in serverAddr;
    struct hostent *pHostent;

    pHostent = gethostbyname(tdomain);
    if (NULL == pHostent) {
        g_idle_add(checkupdateThreadFinish, pcheckupdate);
        return NULL;
    }

    tsock = socket(AF_INET, SOCK_STREAM, 0);
    if (tsock < 0) {
        g_idle_add(checkupdateThreadFinish, pcheckupdate);
        return NULL;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(80);
    serverAddr.sin_addr = *((struct in_addr *)pHostent->h_addr);
    bzero(&(serverAddr.sin_zero), 8);

    int iret = connect(tsock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr));
    if (iret < 0) {
        close(tsock);
        tsock = -1;
        g_idle_add(checkupdateThreadFinish, pcheckupdate);
        return NULL;
    }

    char szWrite[LM_STRSIZE];
    memset(szWrite, 0, LM_STRSIZE);
    snprintf(szWrite, LM_STRSIZE - 1, ("GET %s HTTP/1.1\r\nHost: %s\r\n\r\n"), tpath, tdomain);
    iret = send(tsock, szWrite, strlen((const char *)szWrite), 0);
    if (iret < 0) {
        close(tsock);
        tsock = -1;
        g_idle_add(checkupdateThreadFinish, pcheckupdate);
        return NULL;
    }

    char szBufferT[LM_STRSIZEL];
    memset(szBufferT, 0, LM_STRSIZEL);
    bytesReceived = recv(tsock, szBufferT, LM_STRSIZEL - 1, 0);
    if ((bytesReceived <= 0) || (strstr((const char *)szBufferT, ("HTTP/1.1 200 OK")) == NULL)) {
        close(tsock);
        tsock = -1;
        g_idle_add(checkupdateThreadFinish, pcheckupdate);
        return NULL;
    }

    size_t iLen = 0;
    const char *pszT = strstr((const char *)szBufferT, ("\r\n\r\n"));
    if (pszT) {
        pszT += strlen(("\r\n\r\n"));
        while ((*pszT == ('\n')) || (*pszT == ('\r')) || (*pszT == ('\t')) || (*pszT == (' '))) {
            ++pszT;
        }
        iLen = strlen(pszT);
        if (iLen >= bufferSize) {
            iLen = bufferSize - 1;
        }
        if (iLen > 5) {
            mbstowcs(pcheckupdate->timestamp, pszT, iLen);
            char_t *pszX = Tstrrchr((char_t *)(pcheckupdate->timestamp), uT('\r'));
            if (NULL == pszX) {
                pszX = Tstrrchr((char_t *)(pcheckupdate->timestamp), uT('\n'));
            }
            if (pszX) {
                *pszX = '\0';
                iLen = Tstrlen((const char_t *)(pcheckupdate->timestamp));
            }
        }
    }

    close(tsock);
    tsock = -1;

    g_idle_add(checkupdateThreadFinish, pcheckupdate);
    return NULL;
}

#endif

static void CometCheckUpdate(CometFrame *pFrame, const char_t *pszName)
{
    if (NULL == pFrame) {
        return;
    }

#ifdef WIN32
    if ((true == g_checkupdate.running) || (0UL != g_checkupdate.thread)) {
#else
    if ((true == g_checkupdate.running) || (NULL != g_checkupdate.thread)) {
#endif
        return;
    }

    memset(&g_checkupdate, 0, sizeof(struct _checkupdate_t));
    g_checkupdate.frame = pFrame;
    g_checkupdate.running = true;
    Tstrcpy(g_checkupdate.name, pszName);

    g_checkupdate.frame->OutputStatusbar(uT("Checking for Update..."), SIGMAFRAME_TIMER_LONG);

#ifdef WIN32
    g_checkupdate.thread = _beginthreadex(NULL, 0, &checkupdateThreadRun, (void *)&g_checkupdate, 0, 0);
#else
    g_checkupdate.thread = g_thread_try_new("SOL_WindowMain_checkUpdate", checkupdateThreadRun, &g_checkupdate, NULL);
    if (g_checkupdate.thread) {
        g_thread_unref(g_checkupdate.thread);
    }
#endif

    return;
}
