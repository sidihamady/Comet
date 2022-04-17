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

#include <wx/fileconf.h>

WXDLLIMPEXP_CORE size_t wxGetAvailableDrives(wxArrayString &paths, wxArrayString &names, wxArrayInt &icon_ids);

void CometFrame::saveUIperspective(void)
{
    FILE *fp = NULL;

    if ((fp = Tfopen(static_cast<const char_t *>(m_szLayoutConfigFilename), uT("w"))) == NULL) {
        return;
    }

    wxString strHeader = CometApp::getConfigHeader();
    Tfputs(LM_CSTR(strHeader), fp);

    wxString strT(uT("UIperspective="));
    strT += m_auiManager.SavePerspective();
    Tfputs(LM_CSTR(strT), fp);

    fclose(fp);
    fp = NULL;
}

void CometFrame::loadUIperspective(void)
{
    FILE *fp = NULL;

    if ((fp = Tfopen(static_cast<const char_t *>(m_szLayoutConfigFilename), uT("rt"))) == NULL) {
        return;
    }

    char_t szBuffer[LM_STRSIZEL], cT;
    Tmemset(szBuffer, 0, LM_STRSIZEL);

    int ii = 0;
    const int idx = 14;

    char_t *pszT = NULL, *pszTT = NULL;
    wxString strId;

    while (Tfgets(szBuffer, LM_STRSIZEL - 1, fp) != NULL) {

        if (!Tisalpha(szBuffer[0]) || !Tisalpha(szBuffer[1]) || !Tisalpha(szBuffer[2])) {
            continue;
        }

        pszT = Tstrstr((char_t *)(szBuffer), uT("(id"));

        if (pszT != NULL) {
            pszT += 1;
            pszTT = Tstrstr(pszT, uT(")"));
            if (pszTT != NULL) {
                *pszTT = uT('\0');
                strId = static_cast<const char_t *>(pszT);
                if (strId.IsSameAs(COMET_BUILD_STRING) == false) {
                    break;
                }
            }
        }

        cT = szBuffer[idx];
        szBuffer[idx] = uT('\0');
        if (Tstrcmp(static_cast<const char_t *>(szBuffer), uT("UIperspective=")) == 0) {
            szBuffer[idx] = cT;
            m_bPerspectiveLoaded = m_auiManager.LoadPerspective(static_cast<const char_t *>(szBuffer + idx), false);
            break;
        }

        if (++ii > idx) {
            break;
        }
    }

    fclose(fp);
    fp = NULL;
}

SigmaDropTarget::SigmaDropTarget()
{
}

bool SigmaDropTarget::OnDropFiles(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), const wxArrayString &filenames)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return true;
    }
    return pFrame->dropFiles(filenames);
}

#ifdef __WXMSW__

WXLRESULT CometFrame::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if ((wxGetApp().isFirstInstance()) && (nMsg == wxGetApp().getMessageId())) {

        SigmaTempFile *pFile = new (std::nothrow) SigmaTempFile(LM_CSTR(wxGetApp().getMsgFilename()), false, true);
        if ((pFile != NULL) && pFile->ok()) {
            char_t szT[LM_STRSIZE];
            Tmemset(szT, 0, LM_STRSIZE);
            if (pFile->read(szT, LM_STRSIZE - 1)) {
                // always set focus to the main window before opening file to avoid side effects (flickering)
                this->SetFocus();
                this->Raise();
                this->Show(true);
                if (this->IsIconized()) {
                    this->Iconize(false);
                }
                this->fileOpen(static_cast<const char_t *>(szT), false);
            }
        }

        if (pFile != NULL) {
            delete pFile;
            pFile = NULL;
        }

        return 1L;
    }

    return wxFrame::MSWWindowProc(nMsg, wParam, lParam);
}

#endif

wxAuiNotebook *CometFrame::CreateFilesysNotebook(void)
{
    if (m_pNotebookFilesys) {
        return m_pNotebookFilesys;
    }

    m_pFilesysPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, m_FilesysNotebookSize);
    wxBoxSizer *pSizerFilesys = new wxBoxSizer(wxHORIZONTAL);
    m_pFilesysPanel->SetSizer(pSizerFilesys);
    m_pNotebookFilesys = new (std::nothrow) wxAuiNotebook(m_pFilesysPanel, ID_NOTEBOOK_FILESYS, wxDefaultPosition, m_FilesysNotebookSize, wxAUI_NB_TAB_FIXED_WIDTH | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TOP | wxAUI_NB_CLOSE_ON_ALL_TABS | wxNO_BORDER);
    m_pNotebookFilesys->SetArtProvider(new wxAuiDefaultTabArt);
    m_pNotebookFilesys->SetMinSize(NOTEBOOK_MINSIZE);
    pSizerFilesys->Add(m_pNotebookFilesys, 1, wxEXPAND | wxALL | wxALIGN_CENTER, 1);
    pSizerFilesys->Fit(m_pFilesysPanel);
    m_auiManager.AddPane(m_pFilesysPanel, wxAuiPaneInfo().Name(uT("Explorer")).Caption(uT("Explorer Window")).Row(0).Layer(1).PaneBorder(false).CloseButton(true).MaximizeButton(false).MinSize(NOTEBOOK_MINSIZE).DestroyOnClose(true));

    return m_pNotebookFilesys;
}

CometFileExplorer *CometFrame::CreateExplorerTree(void)
{
    if (m_pExplorerTree) {
        return m_pExplorerTree;
    }

    if (m_pNotebookFilesys == NULL) {
        CreateFilesysNotebook();
        m_auiManager.Update();
    }

    if (m_pNotebookFilesys == NULL) {
        return NULL;
    }

    if (m_SigmaCommonPrefs.szLastDir[0] == uT('\0')) {
        wxArrayString paths, names;
        wxArrayInt icons;
        ::wxGetAvailableDrives(paths, names, icons);
        wxStrcpy(m_SigmaCommonPrefs.szLastDir, paths[0]);
    }
    else {
        addToRecentDir(static_cast<const char_t *>(m_SigmaCommonPrefs.szLastDir));
    }

    char_t szFilters[LM_STRSIZEW];
    Tmemset(szFilters, 0, LM_STRSIZEW);
    Tstrcpy(szFilters,
        uT("Lua files|*.lua;*.comet|")
        uT("C/C++ files|*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.rc|")
        uT("Text/Shell files|*.txt;*.dat;*.csv;*.log;*.conf;*.bat;*.sh;*.csh;*.ins;*.bst;*.patch|")
        uT("Python files|*.py;*.pyw;*.sage;*.sagews|")
        uT("Octave files|*.m;*.mat|")
        uT("Fortran files|*.f;*.for;*.f90;*.f95;*.f03;*.f08;*.f15|")
        uT("LaTeX files|*.tex;*.bib;*.cls;*.sty|")
        uT("Makefile|Makefile;makefile|")
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
        uT("*.txt;*.dat;*.csv;*.conf;*.bat;*.sh;*.csh;*.ins;*.bst;")
        uT("*.patch;*.py;*.pyw;*.m;*.mat;*.f;*.for;*.f90;*.f95;*.f03;*.f08;*.f15;*.tex;*.bib;*.cls;*.sty;Makefile;makefile;*.solis;*.cometr;*.cometu;*."));
    Tstrcat(szFilters,
        CodeEdit::getCometLexerExt());
    Tstrcat(szFilters,
        uT("|All files|*"));
    const int iFilterIndex = FILE_FILTER_KNOWN;

    m_pExplorerTree = new (std::nothrow) CometFileExplorer(m_pNotebookFilesys, IDC_EXPLORER, m_SigmaCommonPrefs.szLastDir, uT(""), wxDefaultPosition, m_FilesysNotebookSize,
                                                           wxNO_BORDER | wxDIRCTRL_EDIT_LABELS | wxDIRCTRL_SHOW_FILTERS,
                                                           static_cast<const char_t *>(szFilters), iFilterIndex - 1);

    if (m_pExplorerTree) {
        m_pExplorerTree->SetHelpText(uT("File Explorer"));
        m_pExplorerTree->SetMinSize(NOTEBOOK_MINSIZE);

        long iB = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
        long iF = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
        iB = CodeEdit::normalizeColor(iB, (iB > iF) ? -8L : 8L);
        m_pExplorerTree->SetBackgroundColour(wxColour(iB));
        m_pExplorerTree->SetForegroundColour(wxColour(iF));
        m_pExplorerTree->Refresh();

        if (m_pNotebookFilesys->AddPage(m_pExplorerTree, uT("Explorer"))) {
            int iExplorerIndex = m_pNotebookFilesys->GetPageIndex(m_pExplorerTree);
            m_pNotebookFilesys->SetPageBitmap(iExplorerIndex, wxBitmap(folder_small_xpm));
        }
    }

    return m_pExplorerTree;
}

BookmarkList *CometFrame::CreateBookmarkWindow(void)
{
    if (m_SigmaGUIPrefs.bViewBookmark == false) {
        return m_pListBookmark;
    }

    if (m_pListBookmark) {
        return m_pListBookmark;
    }

    if (m_pNotebookFilesys == NULL) {
        CreateFilesysNotebook();
        m_auiManager.Update();
    }

    if (m_pNotebookFilesys == NULL) {
        return NULL;
    }

    m_pListBookmark = new (std::nothrow) BookmarkList(m_pNotebookFilesys, m_FilesysNotebookSize, 110, 60, 110);
    if (m_pListBookmark) {
        m_pListBookmark->SetHelpText(uT("Bookmarks"));
        m_pListBookmark->SetMinSize(NOTEBOOK_MINSIZE);

        long iB = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
        long iF = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
        iB = CodeEdit::normalizeColor(iB, (iB > iF) ? -8L : 8L);
        m_pListBookmark->SetBackgroundColour(wxColour(iB));
        m_pListBookmark->SetForegroundColour(wxColour(iF));
        m_pListBookmark->Refresh();

        if (m_pNotebookFilesys->AddPage(m_pListBookmark, uT("Bookmarks"))) {
            int iBookmarkIndex = m_pNotebookFilesys->GetPageIndex(m_pListBookmark);
            m_pNotebookFilesys->SetPageBitmap(iBookmarkIndex, wxBitmap(bookmarkdlg_small_xpm));
            m_pListBookmark->updateList();
        }
    }

    return m_pListBookmark;
}

wxAuiNotebook *CometFrame::CreateMainNotebook(void)
{
    if (m_pNotebookMain) {
        return m_pNotebookMain;
    }

    m_pMainPanel = new wxPanel(this);
    m_pSizerMain = new wxBoxSizer(wxHORIZONTAL);
    m_pMainPanel->SetSizer(m_pSizerMain);

    m_pNotebookMain = new wxAuiNotebook(m_pMainPanel, ID_NOTEBOOK_MAIN, wxDefaultPosition, wxSize(300, 200), wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_TAB_MOVE /*| wxAUI_NB_TAB_SPLIT*/ | wxAUI_NB_TOP | wxNO_BORDER);
    m_pNotebookMain->SetArtProvider(new wxAuiDefaultTabArt);
    m_pNotebookMain->SetMinSize(wxSize(200, 100));
    m_pSizerMain->Add(m_pNotebookMain, 1, wxEXPAND | wxALL, 1);
    m_pInfobarMain = new CodeInfobar(m_pMainPanel, ID_WINDOW_INFOBAR, wxSize(INFOBAR_WIDTH + 2, 200), uT("Infobar"), INFOBAR_TYPE_MAIN, true);
    m_pSizerMain->Add(m_pInfobarMain, 0, wxEXPAND | wxUP | wxDOWN, 0);
    m_pSizerMain->Fit(m_pMainPanel);

    m_auiManager.AddPane(m_pMainPanel, wxAuiPaneInfo().Name(uT("Editor")).CenterPane().PaneBorder(false).CloseButton(false).MaximizeButton(false).MinSize(NOTEBOOK_MINSIZE).DestroyOnClose(true));

    ScriptEdit *pEdit = new ScriptEdit(m_pNotebookMain, ID_WINDOW_SCRIPT, wxDefaultPosition, wxSize(300, 200), wxNO_BORDER | wxVSCROLL | wxHSCROLL, uT("Lua"));
    // call setPrefs before addEditor to avoid flickering
    pEdit->setPrefs(m_ScintillaPrefs, true, true);
    addEditor(pEdit, pEdit->GetFilename());
    pEdit->DoNewFile(m_ScintillaPrefs, wxEmptyString, wxSTC_LEX_LUA, 0);
    pEdit->initDrop();
    pEdit->SetCaretLineVisible(true);

    return m_pNotebookMain;
}

wxAuiNotebook *CometFrame::CreateToolNotebook(void)
{
    if (m_pNotebookToolbox) {
        return m_pNotebookToolbox;
    }

    m_pToolPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, m_ToolNotebookSize);
    m_pToolPanel->SetMinSize(NOTEBOOK_MINSIZE);

    m_pSizerTool = new wxBoxSizer(wxHORIZONTAL);
    m_pSizerTool->SetMinSize(NOTEBOOK_MINSIZE);

    m_pToolPanel->SetSizer(m_pSizerTool);

    m_pNotebookToolbox = new wxAuiNotebook(m_pToolPanel, ID_NOTEBOOK_TOOLBOX, wxDefaultPosition, m_ToolNotebookSize, wxAUI_NB_TAB_FIXED_WIDTH | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TOP | wxAUI_NB_CLOSE_ON_ALL_TABS | wxNO_BORDER);
    m_pNotebookToolbox->SetArtProvider(new wxAuiDefaultTabArt);
    m_pNotebookToolbox->SetMinSize(NOTEBOOK_MINSIZE);

    m_pSizerTool->Add(m_pNotebookToolbox, 1, wxEXPAND | wxALL | wxALIGN_CENTER, 1);

    m_pInfobarTool = new CodeInfobar(m_pToolPanel, ID_WINDOW_INFOBAR, wxSize(INFOBAR_WIDTH + 2, 150), uT("Infobar"), INFOBAR_TYPE_TOOL, false);

    m_pSizerTool->Add(m_pInfobarTool, 0, wxEXPAND | wxUP | wxDOWN | wxALIGN_CENTER, 1);

    m_pSizerTool->Fit(m_pToolPanel);

    m_auiManager.AddPane(m_pToolPanel, wxAuiPaneInfo().Name(uT("Tool")).Caption(uT("Tool Window")).Row(0).Layer(0).Bottom().PaneBorder(false).CloseButton(true).MaximizeButton(false).MinSize(NOTEBOOK_MINSIZE).DestroyOnClose(true));

    return m_pNotebookToolbox;
}

OutputEdit *CometFrame::CreateOutputWindow(void)
{
    if (m_SigmaGUIPrefs.bViewOutput == false) {
        return m_pEditOutput;
    }

    if (m_pEditOutput) {
        return m_pEditOutput;
    }

    if (m_pNotebookToolbox == NULL) {
        CreateToolNotebook();
        m_auiManager.Update();
    }

    if (m_pNotebookToolbox == NULL) {
        return NULL;
    }

    m_pEditOutput = new OutputEdit(m_pNotebookToolbox, ID_WINDOW_OUTPUT, wxDefaultPosition, m_ToolNotebookSize, wxNO_BORDER | wxVSCROLL | wxHSCROLL, uT("Output"));

    m_pEditOutput->setInfobar((wxWindow *)m_pInfobarTool);
    m_pEditOutput->SetHelpText(uT("Output"));
    m_pEditOutput->SetMinSize(NOTEBOOK_MINSIZE);
    m_pEditOutput->setPrefs(m_ScintillaPrefs, true);

    long iB = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
    long iF = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
    iB = CodeEdit::normalizeColor(iB, (iB > iF) ? -8L : 8L);
    m_pEditOutput->SetBackgroundColour(wxColour(iB));
    m_pEditOutput->SetForegroundColour(wxColour(iF));
    m_pEditOutput->Refresh();

    if (m_pNotebookToolbox->AddPage(m_pEditOutput, uT("Output"))) {
        int iOutputIndex = m_pNotebookToolbox->GetPageIndex(m_pEditOutput);
        m_pNotebookToolbox->SetPageBitmap(iOutputIndex, wxBitmap(output_small_xpm));
        m_pNotebookToolbox->SetSelection(iOutputIndex);
    }

    return m_pEditOutput;
}

ConsoleEdit *CometFrame::CreateConsoleWindow(void)
{
    if (m_SigmaGUIPrefs.bViewConsole == false) {
        return m_pEditConsole;
    }

    if (m_pEditConsole) {
        return m_pEditConsole;
    }

    if (m_pNotebookToolbox == NULL) {
        CreateToolNotebook();
        m_auiManager.Update();
    }

    if (m_pNotebookToolbox == NULL) {
        return NULL;
    }

    m_pEditConsole = new (std::nothrow) ConsoleEdit(m_pNotebookToolbox, ID_WINDOW_CONSOLE, wxDefaultPosition, m_ToolNotebookSize, wxNO_BORDER | wxVSCROLL | wxHSCROLL, uT("Console"));

    m_pEditConsole->setInfobar((wxWindow *)m_pInfobarTool);
    m_pEditConsole->SetHelpText(uT("Script Console"));
    m_pEditConsole->SetMinSize(NOTEBOOK_MINSIZE);

    m_pEditConsole->setPrefs(m_ScintillaPrefs, true);

    long iB = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
    long iF = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
    iB = CodeEdit::normalizeColor(iB, (iB > iF) ? -8L : 8L);
    m_pEditConsole->SetBackgroundColour(wxColour(iB));
    m_pEditConsole->SetForegroundColour(wxColour(iF));
    m_pEditConsole->Refresh();

    if (m_pNotebookToolbox->AddPage(m_pEditConsole, uT("Script Console"))) {
        int iConsoleIndex = m_pNotebookToolbox->GetPageIndex(m_pEditConsole);
        m_pNotebookToolbox->SetPageBitmap(iConsoleIndex, wxBitmap(console_small_xpm));
    }

    m_pEditConsole->initOutput();

    return m_pEditConsole;
}

void CometFrame::UpdateToolNotebook(void)
{
    if (m_pNotebookToolbox && m_pEditOutput) {
        m_pEditOutput->SetSize(NOTEBOOK_MINSIZE);
        int iOutputIndex = m_pNotebookToolbox->GetPageIndex(m_pEditOutput);
        m_pNotebookToolbox->SetSelection(iOutputIndex);
    }
}

wxAuiNotebook *CometFrame::CreateDebugNotebook(void)
{
    if (m_pNotebookDebug) {
        return m_pNotebookDebug;
    }

    m_pDebugPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, m_DebugNotebookSize);
    wxBoxSizer *pSizerDebug = new wxBoxSizer(wxHORIZONTAL);
    m_pDebugPanel->SetSizer(pSizerDebug);
    m_pNotebookDebug = new (std::nothrow) wxAuiNotebook(m_pDebugPanel, ID_NOTEBOOK_DEBUG, wxDefaultPosition, m_DebugNotebookSize, wxAUI_NB_TAB_FIXED_WIDTH | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TOP | wxNO_BORDER);
    m_pNotebookDebug->SetArtProvider(new wxAuiDefaultTabArt);
    m_pNotebookDebug->SetMinSize(NOTEBOOK_MINSIZE);
    pSizerDebug->Add(m_pNotebookDebug, 1, wxEXPAND | wxALL | wxALIGN_CENTER, 1);
    CodeInfobar *pInfobarDebug = new CodeInfobar(m_pDebugPanel, ID_WINDOW_INFOBAR, wxSize(INFOBAR_WIDTH + 2, 150), uT("Infobar"), INFOBAR_TYPE_TOOL, false);
    pSizerDebug->Add(pInfobarDebug, 0, wxEXPAND | wxUP | wxDOWN, 0);
    pSizerDebug->Fit(m_pDebugPanel);
    m_auiManager.AddPane(m_pDebugPanel, wxAuiPaneInfo().Name(uT("Debug")).Caption(uT("Debug Window")).Direction(3).Layer(0).Row(0).Position(1).PaneBorder(false).CloseButton(true).MaximizeButton(false).MinSize(NOTEBOOK_MINSIZE).DestroyOnClose(true));

    return m_pNotebookDebug;
}

wxListCtrl *CometFrame::CreateStackWindow(void)
{
    if (m_pListStack) {
        return m_pListStack;
    }

    if (m_pNotebookDebug == NULL) {
        CreateDebugNotebook();
        m_auiManager.Update();
    }

    if (m_pNotebookDebug == NULL) {
        return NULL;
    }

    m_pListStack = new (std::nothrow) wxListCtrl(m_pNotebookDebug, IDL_DEBUG_STACK, wxDefaultPosition, m_DebugNotebookSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING | wxLC_SORT_DESCENDING | wxNO_BORDER);
    if (m_pListStack) {
        m_pListStack->SetHelpText(uT("Stack"));
        m_pListStack->SetMinSize(NOTEBOOK_MINSIZE);

        wxSize sizeColumn = CometApp::getTextExtent(this, uT("> Watch Column <"));
        int widthColumn = sizeColumn.GetWidth();

        wxListItem itemA;
        itemA.SetId(0);
        itemA.SetText(uT("Lev"));
        itemA.SetWidth(widthColumn / 2);
        m_pListStack->InsertColumn(0, itemA);
        wxListItem itemB;
        itemB.SetId(1);
        itemB.SetText(uT("Ln"));
        itemB.SetWidth(widthColumn / 2);
        m_pListStack->InsertColumn(1, itemB);
        wxListItem itemC;
        itemC.SetId(2);
        itemC.SetText(uT("Name"));
        itemC.SetWidth(widthColumn + (widthColumn / 2));
        m_pListStack->InsertColumn(2, itemC);
        wxListItem itemD;
        itemD.SetId(3);
        itemD.SetText(uT("What"));
        itemD.SetWidth(widthColumn);
        m_pListStack->InsertColumn(3, itemD);

        long iB = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
        long iF = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
        iB = CodeEdit::normalizeColor(iB, (iB > iF) ? -8L : 8L);
        m_pListStack->SetBackgroundColour(wxColour(iB));
        m_pListStack->SetForegroundColour(wxColour(iF));
        m_pListStack->Refresh();

        m_pNotebookDebug->AddPage(m_pListStack, uT("Stack"));
        int iStackIndex = m_pNotebookDebug->GetPageIndex(m_pListStack);
        m_pNotebookDebug->SetPageBitmap(iStackIndex, wxBitmap(stack_small_xpm));
    }

    return m_pListStack;
}

wxListCtrl *CometFrame::CreateWatchWindow(void)
{
    if (m_pListWatch) {
        return m_pListWatch;
    }

    if (m_pNotebookDebug == NULL) {
        CreateDebugNotebook();
        m_auiManager.Update();
    }

    if (m_pNotebookDebug == NULL) {
        return NULL;
    }

    m_pListWatch = new (std::nothrow) wxListCtrl(m_pNotebookDebug, IDL_DEBUG_WATCH, wxDefaultPosition, m_DebugNotebookSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING | wxLC_SORT_DESCENDING | wxNO_BORDER);
    if (m_pListWatch) {
        m_pListWatch->SetHelpText(uT("Watch"));
        m_pListWatch->SetMinSize(NOTEBOOK_MINSIZE);

        wxSize sizeColumn = CometApp::getTextExtent(this, uT("> Watch Column <"));
        int widthColumn = sizeColumn.GetWidth();

        wxListItem itemAt;
        itemAt.SetId(0);
        itemAt.SetText(uT("Name"));
        itemAt.SetWidth(widthColumn);
        m_pListWatch->InsertColumn(0, itemAt);
        wxListItem itemBt;
        itemBt.SetId(1);
        itemBt.SetText(uT("Value"));
        itemBt.SetWidth(widthColumn + (widthColumn / 2));
        m_pListWatch->InsertColumn(1, itemBt);
        wxListItem itemCt;
        itemCt.SetId(2);
        itemCt.SetText(uT("Type"));
        itemCt.SetWidth(widthColumn);
        m_pListWatch->InsertColumn(2, itemCt);

        long iB = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
        long iF = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
        iB = CodeEdit::normalizeColor(iB, (iB > iF) ? -8L : 8L);
        m_pListWatch->SetBackgroundColour(wxColour(iB));
        m_pListWatch->SetForegroundColour(wxColour(iF));
        m_pListWatch->Refresh();

        m_pNotebookDebug->AddPage(m_pListWatch, uT("Watch"));
        int iWatchIndex = m_pNotebookDebug->GetPageIndex(m_pListWatch);
        m_pNotebookDebug->SetPageBitmap(iWatchIndex, wxBitmap(debug_small_xpm));
    }

    return m_pListWatch;
}

void CometFrame::UpdateDebugNotebook(const wxString &strFilename)
{
    if ((m_pNotebookDebug == NULL) || (m_pListWatch == NULL) || (m_pListStack == NULL)) {
        CreateStackWindow();
        CreateWatchWindow();
    }

    if (m_pNotebookDebug && m_pListWatch && m_pListStack) {
        int iWatchIndex = m_pNotebookDebug->GetPageIndex(m_pListWatch);
        m_pNotebookDebug->SetSelection(iWatchIndex);
    }
}

void CometFrame::loadScript(wxString strFilename)
{
    if (m_pNotebookMain->GetPageCount() == 1) {
        ScriptEdit *pEdit = getEditor(0);
        if (pEdit && (strFilename.IsEmpty() == false)) {
            wxGetApp().resetStartScript();
            pEdit->SetLexer(wxSTC_LEX_NULL);
            if (pEdit->DoLoadFile(strFilename)) {
                m_bShellStarted = true;
                updateTitle(pEdit->GetFilename());
                pEdit->SetFocus();
                // updateStatusbar done CodeEdit::in OnSetFocus
                //pEdit->updateStatusbar(true);
            }
        }
    }
}

void CometFrame::initInterface(void)
{
    m_auiManager.SetManagedWindow(this);

    this->SetMinSize(FRAME_MINSIZE);

    if (m_SigmaGUIPrefs.bViewExplorer || m_SigmaGUIPrefs.bViewBookmark) {
        CreateFilesysNotebook();
        if (m_SigmaGUIPrefs.bViewExplorer) {
            CreateExplorerTree();
        }
        if (m_SigmaGUIPrefs.bViewBookmark) {
            CreateBookmarkWindow();
        }
    }

    CreateMainNotebook();

    if (m_SigmaGUIPrefs.bViewOutput || m_SigmaGUIPrefs.bViewConsole) {
        CreateToolNotebook();
        if (m_SigmaGUIPrefs.bViewOutput) {
            CreateOutputWindow();
        }
        if (m_SigmaGUIPrefs.bViewConsole) {
            CreateConsoleWindow();
        }
        UpdateToolNotebook();
    }

    SetIcon((char **)app_small_xpm);

    // Set the MainFrame to not display the toolbar buttons tooltip on the statusbar
    wxFrame::SetStatusBarPane(-1);
    wxFrame::enableDispHelp(false);
    //

    if (m_SigmaGUIPrefs.bFrameMaximized) {
        this->Maximize(true);
    }
    else {
        this->SetSize(m_SigmaGUIPrefs.iFrameWidth, m_SigmaGUIPrefs.iFrameHeight);
        this->Layout();
    }

    m_bInitialized = true;

    // initMenu/initToolbar/initStatusbar should be called afer all m_auiManager initializations
    // and before m_auiManager.Update()
    initMenu();
    initToolbar();
    initStatusbar();
    //

    loadUIperspective();

    wxAuiPaneInfo &paneM = m_auiManager.GetPane(m_pMainPanel);
    paneM.Show();

    if (m_pFilesysPanel) {
        wxAuiPaneInfo &paneA = m_auiManager.GetPane(m_pFilesysPanel);
        paneA.Show(m_SigmaGUIPrefs.bViewExplorer || m_SigmaGUIPrefs.bViewBookmark);
    }

    if (m_pToolPanel) {
        wxAuiPaneInfo &paneB = m_auiManager.GetPane(m_pToolPanel);
        paneB.Show(m_SigmaGUIPrefs.bViewConsole || m_SigmaGUIPrefs.bViewOutput);
    }

    if (m_pDebugPanel) {
        wxAuiPaneInfo &paneC = m_auiManager.GetPane(m_pDebugPanel);
        paneC.Hide();
    }

    m_auiManager.Update();

    ScriptEdit *pEdit = getEditor(0);
    if (pEdit) {
        pEdit->SetFocus();
        // updateStatusbar done CodeEdit::in OnSetFocus
    }

    DoAnalyzerUpdate(true, false);

    DoViewInfobar(m_SigmaGUIPrefs.bViewInfobar);

    if (m_pToolPanel && m_SigmaGUIPrefs.bViewOutput) {
        ClearOutputWindow(wxEmptyString, true);
    }

    if (m_pDebugPanel) {
        ClearDebugWindow(wxEmptyString);
    }
}

void CometFrame::initMenu(void)
{
    // Accelerators

    const int nCount = 48;
    wxAcceleratorEntry accelEntries[nCount];
    accelEntries[0].Set(wxACCEL_CTRL, (int)'X', wxID_CUT);
    accelEntries[1].Set(wxACCEL_CTRL, (int)'C', wxID_COPY);
    accelEntries[2].Set(wxACCEL_CTRL, (int)'V', wxID_PASTE);
    accelEntries[3].Set(wxACCEL_CTRL, (int)'A', wxID_SELECTALL);
    accelEntries[4].Set(wxACCEL_CTRL, (int)'Y', ID_MAIN_REDO);
    accelEntries[5].Set(wxACCEL_CTRL, (int)'Z', ID_MAIN_UNDO);
    accelEntries[6].Set(wxACCEL_NORMAL, WXK_DELETE, wxID_CLEAR);
    accelEntries[7].Set(wxACCEL_NORMAL, WXK_NUMPAD_DELETE, wxID_CLEAR);
    accelEntries[8].Set(wxACCEL_CTRL, (int)'N', ID_FILE_NEW);
    accelEntries[9].Set(wxACCEL_CTRL, (int)'O', ID_FILE_OPEN);
    accelEntries[10].Set(wxACCEL_CTRL, (int)'S', ID_FILE_SAVE);
    accelEntries[11].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'S', ID_FILE_SAVEALL);
    accelEntries[12].Set(wxACCEL_CTRL, (int)'P', ID_FILE_PRINT);
    accelEntries[13].Set(wxACCEL_NORMAL, WXK_F12, ID_SCRIPT_START);
    accelEntries[14].Set(wxACCEL_SHIFT, WXK_F12, ID_SCRIPT_STARTDEBUG);
    accelEntries[15].Set(wxACCEL_ALT, WXK_F12, ID_SCRIPT_STOP);
    accelEntries[16].Set(wxACCEL_CTRL, (int)'F', ID_SEARCH_FIND);
    accelEntries[17].Set(wxACCEL_NORMAL, WXK_F3, ID_FIND_NEXT);
    accelEntries[18].Set(wxACCEL_SHIFT, WXK_F3, ID_FIND_PREV);
    accelEntries[19].Set(wxACCEL_CTRL, (int)'H', ID_SEARCH_REPLACE);
    accelEntries[20].Set(wxACCEL_CTRL, (int)'R', ID_SEARCH_REPLACE);
    accelEntries[21].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'F', ID_SEARCH_FINDINFILES);
    accelEntries[22].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'H', ID_SEARCH_REPLACEINFILES);
    accelEntries[23].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'R', ID_SEARCH_REPLACEINFILES);
    accelEntries[24].Set(wxACCEL_CTRL, (int)'G', ID_SEARCH_GOTO);
    accelEntries[25].Set(wxACCEL_NORMAL, WXK_F1, ID_HELP_CONTENTS_PDF);
    accelEntries[26].Set(wxACCEL_CTRL, (int)'+', ID_SCRIPT_ZOOMIN);
    accelEntries[27].Set(wxACCEL_CTRL, (int)'-', ID_SCRIPT_ZOOMOUT);
    accelEntries[28].Set(wxACCEL_CTRL, (int)'0', ID_SCRIPT_ZOOMNONE);
    accelEntries[29].Set(wxACCEL_CTRL, WXK_NUMPAD_ADD, ID_SCRIPT_ZOOMIN);
    accelEntries[30].Set(wxACCEL_CTRL, WXK_NUMPAD_SUBTRACT, ID_SCRIPT_ZOOMOUT);
    accelEntries[31].Set(wxACCEL_CTRL, WXK_NUMPAD0, ID_SCRIPT_ZOOMNONE);
    accelEntries[32].Set(wxACCEL_NORMAL, WXK_F5, ID_FILE_UPDATE);
    accelEntries[33].Set(wxACCEL_CTRL, (int)'L', ID_MAIN_SELECTLINE);
    accelEntries[34].Set(wxACCEL_CTRL, (int)'W', ID_MAIN_SELECTWORD);
    accelEntries[35].Set(wxACCEL_NORMAL, WXK_F2, ID_SCRIPT_BOOKMARK_NEXT);
    accelEntries[36].Set(wxACCEL_SHIFT, WXK_F2, ID_SCRIPT_BOOKMARK_PREV);
    accelEntries[37].Set(wxACCEL_CTRL, (int)'K', ID_SCRIPT_BOOKMARK_MOD);
    accelEntries[38].Set(wxACCEL_CTRL, (int)'B', ID_SCRIPT_BREAKPOINT_MOD);
    accelEntries[39].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'C', ID_SCRIPT_COMMENTSELECTION);
    accelEntries[40].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'U', ID_SCRIPT_UNCOMMENTSELECTION);

    // LaTeX stuff
    accelEntries[41].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'B', ID_LATEX_TEXTBF);
    accelEntries[42].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'I', ID_LATEX_TEXTIT);
    accelEntries[43].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'E', ID_LATEX_EMPH);
    accelEntries[44].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'N', ID_LATEX_TEXTNORMAL);
    accelEntries[45].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'M', ID_LATEX_MATHRM);
    //

    accelEntries[46].Set(wxACCEL_NORMAL, WXK_F6, ID_SCRIPT_INFOBAR_NEXTX);
    accelEntries[47].Set(wxACCEL_SHIFT, WXK_F6, ID_SCRIPT_INFOBAR_PREVX);

    wxAcceleratorTable accelTable(nCount, accelEntries);
    SetAcceleratorTable(accelTable);
    //

    m_pFileMenu = new wxMenu();

    wxMenuItem *pItem = new wxMenuItem(m_pFileMenu, ID_FILE_NEW, uT("&New\tCtrl+N"), uT("New script"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(filenew_small_xpm);
#endif
    m_pFileMenu->Append(pItem);

    wxMenu *menuFileTemplate = new wxMenu();

    pItem = new wxMenuItem(menuFileTemplate, ID_FILE_TEMPLATE01, uT("Lua"), uT("Lua Template"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(lua_small_xpm);
#endif
    menuFileTemplate->Append(pItem);
    menuFileTemplate->Append(ID_FILE_TEMPLATE02, uT("C"), uT("C Template"));
    menuFileTemplate->Append(ID_FILE_TEMPLATE02MOD, uT("C module"), uT("C Module Template"));
    menuFileTemplate->Append(ID_FILE_TEMPLATE03, uT("C++"), uT("C++ Template"));
    menuFileTemplate->Append(ID_FILE_TEMPLATE04, uT("Python"), uT("Python Template"));
    menuFileTemplate->Append(ID_FILE_TEMPLATE05, uT("Fortran"), uT("Fortran Template"));
    menuFileTemplate->Append(ID_FILE_TEMPLATE06, uT("Makefile"), uT("Makefile Template"));
    menuFileTemplate->Append(ID_FILE_TEMPLATE07, uT("LaTeX"), uT("LaTeX Template"));
    menuFileTemplate->Append(ID_FILE_TEMPLATE08, uT("Batch"), uT("Batch Template"));
    m_pFileMenu->AppendSubMenu(menuFileTemplate, uT("New <Template>"), uT("New Using Template"));

    m_pFileMenu->AppendSeparator();

    pItem = new wxMenuItem(m_pFileMenu, ID_FILE_OPEN, uT("&Open File...\tCtrl+O"), uT("Open script"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(fileopen_small_xpm);
#endif
    m_pFileMenu->Append(pItem);

    pItem = new wxMenuItem(m_pFileMenu, ID_EXPLORER_BROWSE, uT("Change Directory..."), uT("Change the working directory"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(folder_small_xpm);
#endif
    m_pFileMenu->Append(pItem);

    m_pFileMenu->AppendSeparator();

    pItem = new wxMenuItem(m_pFileMenu, ID_FILE_SAVE, uT("&Save\tCtrl+S"), uT("Save file"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(filesave_small_xpm);
#ifdef __WXMSW__
    pItem->SetDisabledBitmap(filesave_disabled_small_xpm);
#endif
#endif
    m_pFileMenu->Append(pItem);

    pItem = new wxMenuItem(m_pFileMenu, ID_FILE_SAVEAS, uT("Save As..."), uT("Save file in another location"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(filesaveas_small_xpm);
#endif
    m_pFileMenu->Append(pItem);

    pItem = new wxMenuItem(m_pFileMenu, ID_FILE_SAVEALL, uT("Save All"), uT("Save all files"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(filesaveall_small_xpm);
#ifdef __WXMSW__
    pItem->SetDisabledBitmap(filesaveall_disabled_small_xpm);
#endif
#endif
    m_pFileMenu->Append(pItem);

    m_pFileMenu->AppendSeparator();

    wxMenu *menuFileCurrent = new wxMenu();

    updateCurrentFileMenu(menuFileCurrent, -1);

    m_pFileMenu->AppendSubMenu(menuFileCurrent, uT("Current File"), uT("Current File Actions"));

    m_pFileMenu->AppendSeparator();

    pItem = new wxMenuItem(m_pFileMenu, ID_FILE_PRINT, uT("&Print...\tCtrl+P"), uT("Print"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(print_small_xpm);
#endif
    m_pFileMenu->Append(pItem);
    m_pFileMenu->Append(ID_FILE_PRINTPREVIEW, uT("Print Preview..."), uT("Print Preview"));
    m_pFileMenu->AppendSeparator();

    m_pRecentFileMenu = new wxMenu();
    m_pRecentFileMenuItem = m_pFileMenu->AppendSubMenu(m_pRecentFileMenu, uT("Recent Files"), uT("Recent Files"));
    updateRecentFileMenu(m_pRecentFileMenu, m_pRecentFileMenuItem);

    m_pRecentDirMenu = new wxMenu();
    m_pRecentDirMenuItem = m_pFileMenu->AppendSubMenu(m_pRecentDirMenu, uT("Recent Directories"), uT("Recent Directories"));
    updateRecentDirMenu(m_pRecentDirMenu, m_pRecentDirMenuItem);

    m_pFileMenu->AppendSeparator();

    m_pFileMenu->Append(ID_FILE_CLOSE, uT("Close"), uT("Close"));
    m_pFileMenu->Append(ID_FILE_CLOSEALL, uT("Close All"), uT("Close All"));
    m_pFileMenu->Append(ID_FILE_CLOSEOTHERS, uT("Close Others"), uT("Close Others"));
    m_pFileMenu->Append(ID_FILE_CLOSEALLBUTPINNED, uT("Close All But Pinned"), uT("Close All But Pinned"));

    m_pFileMenu->AppendSeparator();

    m_pFileMenu->Append(ID_FILE_REOPENLAST, uT("Reopen Last Closed File"), uT("Reopen Last Closed File"));

    m_pFileMenu->AppendSeparator();

    pItem = new wxMenuItem(m_pFileMenu, ID_FILE_EXIT, uT("E&xit\tAlt+F4"), uT("Quit Comet"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(quit_small_xpm);
#endif
    m_pFileMenu->Append(pItem);

    wxMenu *menuEdit = new wxMenu();

    pItem = new wxMenuItem(menuEdit, ID_MAIN_UNDO, uT("&Undo\tCtrl+Z"), uT("Undo the last action (Ctrl+Z)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(undo_small_xpm);
#endif
    menuEdit->Append(pItem);
    pItem = new wxMenuItem(menuEdit, ID_MAIN_REDO, uT("&Redo\tCtrl+Y"), uT("Repeat the last action (Ctrl+Y)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(redo_small_xpm);
#endif
    menuEdit->Append(pItem);
    menuEdit->AppendSeparator();
    pItem = new wxMenuItem(menuEdit, wxID_CUT, uT("Cu&t\tCtrl+X"), uT("Cut the selected text (Ctrl+X)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(cut_small_xpm);
#endif
    menuEdit->Append(pItem);
    pItem = new wxMenuItem(menuEdit, wxID_COPY, uT("&Copy\tCtrl+C"), uT("Copy the selected text (Ctrl+C)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(copy_small_xpm);
#endif
    menuEdit->Append(pItem);

    pItem = new wxMenuItem(menuEdit, wxID_PASTE, uT("&Paste\tCtrl+V"), uT("Paste text (Ctrl+V)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(paste_small_xpm);
#endif
    menuEdit->Append(pItem);

    pItem = new wxMenuItem(menuEdit, wxID_CLEAR, uT("&Delete\tDel"), uT("Delete the selected text"));
    menuEdit->Append(pItem);

    wxMenu *menuSelect = new wxMenu();
    pItem = new wxMenuItem(menuSelect, wxID_SELECTALL, uT("Select All\tCtrl+A"), uT("Select the whole text"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(selectall_small_xpm);
#endif
    menuSelect->Append(pItem);
    menuSelect->Append(ID_MAIN_SELECTLINE, uT("Select Line\tCtrl+L"), uT("Select current line"));
    menuSelect->Append(ID_MAIN_SELECTWORD, uT("Select Word\tCtrl+W"), uT("Select current word"));
    menuEdit->AppendSubMenu(menuSelect, uT("Select"));

    wxMenu *menuFormat = new wxMenu();
    pItem = new wxMenuItem(menuFormat, ID_SCRIPT_COMMENTSELECTION, uT("Comment\tShift+Ctrl+C"), uT("Comment out the selected text (Shift+Ctrl+C)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(comment_small_xpm);
#endif
    menuFormat->Append(pItem);
    pItem = new wxMenuItem(menuFormat, ID_SCRIPT_UNCOMMENTSELECTION, uT("Uncomment\tShift+Ctrl+U"), uT("Uncomment the selected text (Shift+Ctrl+U)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(uncomment_small_xpm);
#endif
    menuFormat->Append(pItem);
    menuFormat->AppendSeparator();

    menuFormat->Append(ID_SCRIPT_UPPERCASE, uT("Upper Case"), uT("Convert the selected text to upper case"), wxITEM_NORMAL);
    menuFormat->Append(ID_SCRIPT_LOWERCASE, uT("Lower Case"), uT("Convert the selected text to lower case"), wxITEM_NORMAL);

    menuFormat->AppendSeparator();

    pItem = new wxMenuItem(menuFormat, ID_SCRIPT_INDENTINC, uT("Increase Indent"), uT("Increase the indentation of the text (move to right)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(indentinc_small_xpm);
#endif
    menuFormat->Append(pItem);

    pItem = new wxMenuItem(menuFormat, ID_SCRIPT_INDENTDEC, uT("Decrease Indent"), uT("Decrease the indentation of the text (move to left)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(indentdec_small_xpm);
#endif
    menuFormat->Append(pItem);

    menuFormat->AppendSeparator();

    menuFormat->Append(ID_SCRIPT_FORMATDOC, uT("Format Document"), uT("Format document using the current indentation style"), wxITEM_NORMAL);

    m_pFormatMenuItem = menuEdit->AppendSubMenu(menuFormat, uT("Format"));

    menuEdit->AppendSeparator();
    menuEdit->Append(ID_FILE_PIN, uT("Pin Document"), uT("Pin Document"), wxITEM_NORMAL);
    menuEdit->Append(ID_FILE_RESETMARKERSB, uT("Remove Markers"), uT("Remove all markers"), wxITEM_NORMAL);

    menuEdit->AppendSeparator();

    wxMenu *menuInsert = new wxMenu();
    pItem = new wxMenuItem(menuInsert, ID_MAIN_INSERT_DATE, uT("Insert Date"), uT("Insert current date"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(insertdate_small_xpm);
#endif
    menuInsert->Append(pItem);
    menuInsert->Append(ID_MAIN_INSERT_TIME, uT("Insert Time"), uT("Insert current time"));
    menuInsert->Append(ID_MAIN_INSERT_DATETIME, uT("Insert Date/Time"), uT("Insert current date and time"));

    m_pDateMenuItem = menuEdit->AppendSubMenu(menuInsert, uT("Date/Time"));

    m_pSnippetsMenuItem = NULL;

    wxMenu *menuSnippets = new wxMenu();
    pItem = new wxMenuItem(menuSnippets, ID_SCRIPT_SNIPPET_FIRST, (m_pSnippet != NULL) ? m_pSnippet->getTitle(0) : uT("Snippet"),
                           (m_pSnippet != NULL) ? m_pSnippet->getTitle(0) : uT("Snippet"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(code_small_xpm);
#endif
    menuSnippets->Append(pItem);

    for (int idT = ID_SCRIPT_SNIPPET_FIRST + 1; idT <= ID_SCRIPT_SNIPPET_LASTMENU; idT++) {
        const char_t *pszT = (m_pSnippet != NULL) ? m_pSnippet->getTitle(idT - ID_SCRIPT_SNIPPET_FIRST) : uT("Snippet");
        menuSnippets->Append(idT, pszT, pszT);
    }

    m_pSnippetsMenuItem = menuEdit->AppendSubMenu(menuSnippets, uT("Insert Snippet"), uT("Insert code snippet"));

    menuEdit->AppendSeparator();

    wxMenu *menuEditEOL = new wxMenu();
    pItem = new wxMenuItem(menuEditEOL, ID_SCRIPT_CONVERTLF, uT("LF (Unix)"), uT("Unix Line Ending (\\n)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(lf_small_xpm);
#endif
    menuEditEOL->Append(pItem);
    menuEditEOL->Append(ID_SCRIPT_CONVERTCR, uT("CR (Mac)"), uT("Mac OS Line Ending (\\r)"));
    menuEditEOL->Append(ID_SCRIPT_CONVERTCRLF, uT("CRLF (Windows)"), uT("Windows Line Ending (\\r\\n)"));

    m_pEOLMenuItem = menuEdit->AppendSubMenu(menuEditEOL, uT("Convert EOL To"), uT("Convert EOL To (Unix, Mac or Windows)"));

    wxMenu *menuUTF8Encode = new wxMenu();
    pItem = new wxMenuItem(menuUTF8Encode, ID_SCRIPT_UTF8FROM_ISO8859L1, uT("From Latin-1 Western European"), uT("From Latin-1 Western European (ISO-8859-1)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(iso8859l1_small_xpm);
#endif
    menuUTF8Encode->Append(pItem);

    pItem = new wxMenuItem(menuUTF8Encode, ID_SCRIPT_UTF8FROM_ISO8859L9, uT("From Latin-9 Western European"), uT("From Latin-9 Western European (ISO-8859-15)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(iso8859l9_small_xpm);
#endif
    menuUTF8Encode->Append(pItem);

    menuUTF8Encode->Append(ID_SCRIPT_UTF8FROM_CP1252, uT("From CP-1252 (Windows ANSI)"), uT("From CP-1252 (Windows ANSI)"));

    m_pUTF8MenuItem = menuEdit->AppendSubMenu(menuUTF8Encode, uT("Convert To UTF-8"), uT("Convert document encoding to UTF-8 (Comet internal document format)"));

    wxMenu *menuView = new wxMenu();
    pItem = new wxMenuItem(menuView, ID_SCRIPT_ZOOMIN, uT("&Zoom In\tCtrl++"), uT("Increase the editor's font size"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(zoomin_small_xpm);
#endif
    menuView->Append(pItem);
    pItem = new wxMenuItem(menuView, ID_SCRIPT_ZOOMOUT, uT("Zoom &Out\tCtrl+-"), uT("Decrease the editor's font size"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(zoomout_small_xpm);
#endif
    menuView->Append(pItem);
    menuView->Append(ID_SCRIPT_ZOOMNONE, uT("Normal Viewing\tCtrl+0"), uT("Set the editor's font size to the default one"));
    menuView->AppendSeparator();

    menuView->Append(ID_SCRIPT_DISPLAYEOL, uT("End Of Line"), uT("Display the end of line"), wxITEM_CHECK);
    menuView->Append(ID_SCRIPT_WHITESPACE, uT("White Spaces"), uT("Display white spaces"), wxITEM_CHECK);

    menuView->AppendSeparator();
    menuView->Append(ID_VIEW_EXPLORER, uT("File &Explorer"), uT("View the file explorer"), wxITEM_CHECK)->Check(m_SigmaGUIPrefs.bViewExplorer);

    menuView->Append(ID_VIEW_OUTPUT, uT("Output Window"), uT("View the output window"), wxITEM_CHECK)->Check(m_pEditOutput != NULL);
    menuView->Append(ID_VIEW_CONSOLE, uT("Script Console"), uT("View the script console window"), wxITEM_CHECK)->Check(m_pEditConsole != NULL);
    menuView->Append(ID_VIEW_BOOKMARK, uT("Bookmarks"), uT("View the bookmark window"), wxITEM_CHECK)->Check(m_pListBookmark != NULL);

    menuView->AppendSeparator();
    menuView->Append(ID_VIEW_TOOLBAR, uT("Toolbar"), uT("View the toolbar"), wxITEM_CHECK)->Check(m_SigmaGUIPrefs.bViewToolbar);
    menuView->Append(ID_VIEW_STATUSBAR, uT("Statusbar"), uT("View the statusbar"), wxITEM_CHECK)->Check(m_SigmaGUIPrefs.bViewStatusbar);
    menuView->Append(ID_VIEW_SEARCHBAR, uT("Searchbar"), uT("View the searchbar"), wxITEM_CHECK)->Check(m_SigmaGUIPrefs.bViewSearchbar);

    menuView->AppendSeparator();
    menuView->Append(ID_VIEW_TOPMOST, uT("Always On Top"), uT("Keep on top of the all other windows"), wxITEM_CHECK);

    menuView->AppendSeparator();
    menuView->Append(ID_VIEW_RESETUI, uT("Reset UI Perspective"), uT("Reset UI Perspective"));

    m_pSearchMenu = new wxMenu();
    pItem = new wxMenuItem(m_pSearchMenu, ID_SEARCH_FIND, uT("&Find\tCtrl+F"), uT("Find in the current document"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(find_small_xpm);
#endif
    m_pSearchMenu->Append(pItem);

    pItem = new wxMenuItem(m_pSearchMenu, ID_FIND_NEXT, uT("Find Next\tF3"), uT("Find the next item in the current document"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(find_next_small_xpm);
#endif
    m_pSearchMenu->Append(pItem);

    pItem = new wxMenuItem(m_pSearchMenu, ID_FIND_PREV, uT("Find Previous\tShift+F3"), uT("Find the previous item in the current document"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(find_prev_small_xpm);
#endif
    m_pSearchMenu->Append(pItem);

    m_pSearchMenu->Append(ID_SEARCH_REPLACE, uT("&Replace\tCtrl+H"), uT("Find/Replace in the current document"));
    m_pSearchMenu->AppendSeparator();

    pItem = new wxMenuItem(m_pSearchMenu, ID_SEARCH_FINDINFILES, uT("Find in Files"), uT("Find in files"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(find_files_small_xpm);
#endif
    m_pSearchMenu->Append(pItem);

    m_pSearchMenu->Append(ID_SEARCH_REPLACEINFILES, uT("Replace in Files"), uT("Replace in files"));
    m_pSearchMenu->AppendSeparator();
    m_pSearchMenu->Append(ID_SEARCH_RESETMARKERS, uT("Remove Find Markers"), uT("Remove find markers"));
    m_pSearchMenu->AppendSeparator();
    m_pSearchMenu->Append(ID_SEARCH_GOTO, uT("&Goto\tCtrl+G"), uT("Go to specific line"));
    m_pSearchMenu->AppendSeparator();
    pItem = new wxMenuItem(m_pSearchMenu, ID_SCRIPT_BOOKMARK_MOD, uT("Add &Bookmark\tCtrl+F2"), uT("Add or remove bookmark"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(bookmark_add_small_xpm);
#endif
    m_pSearchMenu->Append(pItem);
    pItem = new wxMenuItem(m_pSearchMenu, ID_SCRIPT_BOOKMARK_NEXT, uT("Next Bookmark\tF2"), uT("Go to the next bookmark"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(bookmark_next_small_xpm);
#endif
    m_pSearchMenu->Append(pItem);
    pItem = new wxMenuItem(m_pSearchMenu, ID_SCRIPT_BOOKMARK_PREV, uT("Previous Bookmark\tShift+F2"), uT("Go to the previous bookmark"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(bookmark_prev_small_xpm);
#endif
    m_pSearchMenu->Append(pItem);
    pItem = new wxMenuItem(m_pSearchMenu, ID_SCRIPT_BOOKMARK_DELALL, uT("Delete File Bookmarks"), uT("Delete current file bookmarks"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(bookmark_deleteall_small_xpm);
#endif
    m_pSearchMenu->Append(pItem);
    m_pSearchMenu->Append(ID_BOOKMARK_DELETEALL, uT("Delete All Bookmarks"), uT("Delete all bookmarks"));

    wxMenu *menuRun = new wxMenu();
    pItem = new wxMenuItem(menuRun, ID_SCRIPT_START, uT("&Execute\tF12"), uT("Execute the script"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(start_small_xpm);
#endif
    menuRun->Append(pItem);

    menuRun->AppendSeparator();

    pItem = new wxMenuItem(menuRun, ID_SCRIPT_BREAKPOINT_MOD, uT("Add &Breakpoint\tCtrl+F12"), uT("Add or remove breakpoint"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(breakpoint_add_small_xpm);
#endif
    menuRun->Append(pItem);

    menuRun->Append(ID_SCRIPT_BREAKPOINT_DELALL, uT("Reset Breakpoints"), uT("Remove all breakpoints"));

    menuRun->Append(ID_SCRIPT_STEPINTO, uT("Step Into"), uT("Step into or Step over"), wxITEM_CHECK);

    pItem = new wxMenuItem(menuRun, ID_SCRIPT_STARTDEBUG, uT("&Debug\tShift+F12"), uT("Start/continue script debugging"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(startdebug_small_xpm);
#endif
    menuRun->Append(pItem);

    pItem = new wxMenuItem(menuRun, ID_SCRIPT_STOP, uT("S&top\tAlt+F12"), uT("Stop script debugging"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(stop_small_xpm);
#endif
    menuRun->Append(pItem);

    menuRun->AppendSeparator();
    menuRun->Append(ID_SCRIPT_KILLTOOL, uT("Stop External Tool"), uT("Stop the running external tool"), wxITEM_NORMAL);

    wxMenu *menuOptions = new wxMenu();
    menuOptions->Append(ID_SCRIPT_FONT, uT("&Font..."), uT("Set the editor font"));
    pItem = new wxMenuItem(menuOptions, ID_SCRIPT_COLORS, uT("&Colors and Style..."), uT("Customize the editor colors"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(colors_small_xpm);
#endif
    menuOptions->Append(pItem);
    menuOptions->Append(ID_SCRIPT_LEXER, uT("&User-defined Syntax"), uT("Set the editor font"));

    pItem = new wxMenuItem(menuOptions, ID_SCRIPT_INDENT, uT("&Indentation..."), uT("Set the editor indentation options"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(indentinc_small_xpm);
#endif
    menuOptions->Append(pItem);
    menuOptions->AppendSeparator();

    menuOptions->Append(ID_SCRIPT_SYNTAX, uT("Syntax &Highlighting"), uT("Enable or disable syntax highlighting"), wxITEM_CHECK);
    menuOptions->Append(ID_SCRIPT_AUTOCOMPLETE, uT("&Auto Completion"), uT("Enable or disable auto completion"), wxITEM_CHECK);
    menuOptions->Append(ID_SCRIPT_CALLTIP, uT("Call &Tips"), uT("Enable or disable call tips"), wxITEM_CHECK);
    menuOptions->AppendSeparator();

    menuOptions->Append(ID_SCRIPT_LINENUMBERS, uT("&Line Numbers"), uT("View or hide line numbers"), wxITEM_CHECK);
    menuOptions->Append(ID_SCRIPT_MARGINMARKERS, uT("&Markers"), uT("View or hide markers (bookmarks and breakpoints)"), wxITEM_CHECK);
    menuOptions->Append(ID_SCRIPT_MARGINSTATUS, uT("&Status"), uT("View or hide status margin (annotations)"), wxITEM_CHECK);
    menuOptions->Append(ID_SCRIPT_FOLDENABLE, uT("&Folding"), uT("Enable or disable code folding"), wxITEM_CHECK);
    menuOptions->Append(ID_SCRIPT_WRAPMODE, uT("&Wrap"), uT("Enable or disable wrap mode"), wxITEM_CHECK);
    menuOptions->Append(ID_SCRIPT_SAVENAVIGATION, uT("Save Navigation Info"), uT("Save file navigation info (bookmarks, find, modification...)"), wxITEM_CHECK);
    menuOptions->Append(ID_SCRIPT_VIEWINFOBAR, uT("View Infobar Markers"), uT("View the infobar markers (modification, find, bookmarks...)"), wxITEM_CHECK);
    menuOptions->AppendSeparator();

    wxMenu *menuOptionsLinespacing = new wxMenu();
    pItem = new wxMenuItem(menuOptionsLinespacing, ID_SCRIPT_LINESPACING_100, uT("1.00"), uT("Set the line spacing to 1"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(linespacing100_small_xpm);
#endif
    menuOptionsLinespacing->Append(pItem);
    menuOptionsLinespacing->Append(ID_SCRIPT_LINESPACING_125, uT("1.25"), uT("Set the line spacing to 1.25"));
    menuOptionsLinespacing->Append(ID_SCRIPT_LINESPACING_150, uT("1.50"), uT("Set the line spacing to 1.5"));
    pItem = new wxMenuItem(menuOptionsLinespacing, ID_SCRIPT_LINESPACING_175, uT("1.75"), uT("Set the line spacing to 1.75"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(linespacing175_small_xpm);
#endif
    menuOptionsLinespacing->Append(pItem);
    menuOptions->AppendSubMenu(menuOptionsLinespacing, uT("Line Spacing"), uT("Set the line spacing"));

    menuOptions->AppendSeparator();
    menuOptions->Append(ID_SCRIPT_DETECTCHANGE, uT("&Detect Changes from Outside"), uT("Detect changes that occur outside Comet"), wxITEM_CHECK);

    wxMenu *menuTools = new wxMenu();
    menuTools->Append(ID_TOOLS_BUILD, uT("E&xternal Build Tools..."), uT("External build tools"));

    wxMenu *menuHelp = new wxMenu(0L);
    menuHelp->Append(ID_HELP_CONTENTS_PDF, uT("&Contents (PDF)"), uT("Display Comet Help File (PDF)"));

    m_pExamplesMenuItem = NULL;

    wxMenu *menuHelpSamples = new wxMenu();
    pItem = new wxMenuItem(menuHelpSamples, ID_SCRIPT_EXAMPLE_01, (m_pExample != NULL) ? m_pExample->getTitle(0) : uT("Example"),
                           (m_pExample != NULL) ? m_pExample->getTitle(0) : uT("Example"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(code_small_xpm);
#endif
    menuHelpSamples->Append(pItem);

    for (int idT = ID_SCRIPT_EXAMPLE_FIRST + 1; idT <= ID_SCRIPT_EXAMPLE_LAST; idT++) {
        menuHelpSamples->Append(idT, (m_pExample != NULL) ? m_pExample->getTitle(idT - ID_SCRIPT_EXAMPLE_FIRST) : uT("Example"),
                                (m_pExample != NULL) ? m_pExample->getTitle(idT - ID_SCRIPT_EXAMPLE_FIRST) : uT("Example"));
    }

#ifdef __WXMSW__
    menuHelpSamples->AppendSeparator();
    pItem = new wxMenuItem(menuHelpSamples, ID_FILE_OPENEXAMPLESDIR, uT("Open Examples"), uT("Open the examples directory (copy the content to your own directory)"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(folder_small_xpm);
#endif
    menuHelpSamples->Append(pItem);
#else

#endif

    m_pExamplesMenuItem = menuHelp->AppendSubMenu(menuHelpSamples, uT("Examples"), uT("Code examples"));

    menuHelp->AppendSeparator();
    pItem = new wxMenuItem(menuHelp, ID_HELP_CHECKUPDATE, uT("&Check for Update..."), uT("Check for Update"));
    menuHelp->Append(pItem);

    menuHelp->AppendSeparator();

    pItem = new wxMenuItem(menuHelp, ID_HELP_ABOUT, uT("&About..."), uT("About Comet"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(app_small_xpm);
#endif
    menuHelp->Append(pItem);

    m_pMenuBar = new wxMenuBar();
    m_pMenuBar->Append(m_pFileMenu, uT("&File"));
    m_pMenuBar->Append(menuEdit, uT("&Edit"));
    m_pMenuBar->Append(menuView, uT("&View"));
    m_pMenuBar->Append(m_pSearchMenu, uT("&Search"));
    m_pMenuBar->Append(menuRun, uT("&Run"));
    m_pMenuBar->Append(menuOptions, uT("&Options"));
    m_pMenuBar->Append(menuTools, uT("&Tools"));
    m_pMenuBar->Append(menuHelp, uT("&Help"));

    this->SetMenuBar(m_pMenuBar);
}

bool CometFrame::initAnalyze(void)
{
    if (m_cboAnalyzer == NULL) {
        wxSize sizeEdit = CometApp::getTextExtent(this, uT("function doCalculation(paramA, paramB, paramC)"));
        m_cboAnalyzer = new (std::nothrow) AnalyzerComboBox(m_pToolBar, IDC_ANALYZE_LIST, wxEmptyString, wxDefaultPosition, wxSize(sizeEdit.GetWidth(), -1), 0, NULL, 0);
    }

    return (m_cboAnalyzer != NULL);
}

void CometFrame::initToolbar(void)
{

    m_pToolBar = new SigmaToolBar(this);
    SetToolBar(m_pToolBar);

    m_pToolBar->SetToolBitmapSize(wxSize(22, 22));

    m_pToolBar->AddTool(ID_FILE_NEW, uT("New"), wxBitmap(filenew_small_xpm), uT("New file (Ctrl+N)"));
    m_pToolBar->AddTool(ID_FILE_OPEN, uT("Open"), wxBitmap(fileopen_small_xpm), uT("Open file (Ctrl+O)"));
    m_pToolBar->AddTool(ID_FILE_SAVE, uT("Save"), wxBitmap(filesave_small_xpm), uT("Save file (Ctrl+S)"));
    m_pToolBar->AddTool(ID_FILE_SAVEALL, uT("Save All"), wxBitmap(filesaveall_small_xpm), uT("Save all files (Ctrl+Shift+S)"));
    m_pToolBar->AddSeparator();
    m_pToolBar->AddTool(ID_FILE_FILELIST, uT("Documents"), wxBitmap(filelist_small_xpm), uT("View the documents list"));

    m_pToolBar->AddSeparator();

    m_pToolBar->AddTool(ID_MAIN_UNDO, uT("Undo"), wxBitmap(undo_small_xpm), uT("Undo the last action (Ctrl+Z)"));
    m_pToolBar->AddTool(ID_MAIN_REDO, uT("Redo"), wxBitmap(redo_small_xpm), uT("Repeat the last action (Ctrl+Y)"));
    m_pToolBar->AddTool(wxID_COPY, uT("Copy"), wxBitmap(copy_small_xpm), uT("Copy the selected text (Ctrl+C)"));
    m_pToolBar->AddTool(wxID_PASTE, uT("Paste"), wxBitmap(paste_small_xpm), uT("Paste text (Ctrl+V)"));
    m_pToolBar->AddTool(ID_SCRIPT_COMMENTSELECTION, uT("Comment"), wxBitmap(comment_small_xpm), uT("Comment the selected text or line\tShift+Ctrl+C"));
    m_pToolBar->AddTool(ID_SCRIPT_UNCOMMENTSELECTION, uT("Uncomment"), wxBitmap(uncomment_small_xpm), uT("Uncomment the selected text or line\tShift+Ctrl+U"));
    m_pToolBar->AddTool(ID_CODE_FORMAT, uT("Format"), wxBitmap(format_small_xpm), uT("Format text"));
    m_pToolBar->AddTool(ID_CODE_SNIPPET, uT("Snippet"), wxBitmap(snippet_small_xpm), uT("Insert code snippet"));

    m_pToolBar->AddSeparator();

    m_pToolBar->AddTool(ID_SCRIPT_START, uT("Execute"), wxBitmap(start_small_xpm), uT("Execute the script (F12)"));
    m_pToolBar->AddSeparator();
    m_pToolBar->AddTool(ID_SCRIPT_BREAKPOINT_MOD, uT("Add/Remove Breakpoint"), wxBitmap(breakpoint_add_small_xpm), uT("Add or remove breakpoint (Ctrl+F12)"));
    m_pToolBar->AddTool(ID_SCRIPT_STARTDEBUG, uT("Debug / Continue"), wxBitmap(startdebug_small_xpm), uT("Start/continue script debugging (Shift+F12)"));
    m_pToolBar->AddTool(ID_SCRIPT_STOP, uT("Stop"), wxBitmap(stop_small_xpm), uT("Stop script debugging (Alt+F12)"));

    m_pToolBar->AddSeparator();

    if (m_SigmaGUIPrefs.bViewSearchbar) {
        createSearchToolbar();
    }

    m_pToolBar->Realize();

    if (m_SigmaGUIPrefs.bViewToolbar == false) {
        m_pToolBar->Show(false);
        m_auiManager.Update();
    }
}

void CometFrame::initStatusbar(void)
{
    const int iCount = 6;
    m_pStatusBar = CreateStatusBar(iCount);
    int widthsT[] = { -1, 100, 100, 100, 50, 50 };
    m_pStatusBar->SetStatusWidths(iCount, widthsT);

    if (m_SigmaGUIPrefs.bViewStatusbar == false) {
        m_pStatusBar->Show(false);
        m_auiManager.Update();
    }
}


void CometFrame::initToolPrefs(ScintillaPrefs &tScintillaPrefs)
{
    long iB = tScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
    long iF = tScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
    iB = CodeEdit::normalizeColor(iB, (iB > iF) ? -8L : 8L);

    if (m_pExplorerTree) {
        m_pExplorerTree->SetBackgroundColour(wxColour(iB));
        m_pExplorerTree->GetTreeCtrl()->SetBackgroundColour(wxColour(iB));
        m_pExplorerTree->GetTreeCtrl()->SetForegroundColour(wxColour(iF));
        m_pExplorerTree->Refresh();
    }

    if (m_pNotebookDebug) {
        m_pNotebookDebug->SetBackgroundColour(wxColour(iB));
        m_pNotebookDebug->SetForegroundColour(wxColour(iF));
        m_pNotebookDebug->Refresh();
    }

    if (m_pListWatch) {
        m_pListWatch->SetBackgroundColour(wxColour(iB));
        m_pListWatch->SetForegroundColour(wxColour(iF));
        m_pListWatch->Refresh();
    }

    if (m_pListStack) {
        m_pListStack->SetBackgroundColour(wxColour(iB));
        m_pListStack->SetForegroundColour(wxColour(iF));
        m_pListStack->Refresh();
    }

    if (m_pNotebookToolbox) {
        m_pNotebookToolbox->SetBackgroundColour(wxColour(iB));
        m_pNotebookToolbox->SetForegroundColour(wxColour(iF));
        m_pNotebookToolbox->Refresh();
    }

    if (m_pEditOutput) {
        m_pEditOutput->SetBackgroundColour(wxColour(iB));
        m_pEditOutput->SetForegroundColour(wxColour(iF));
        m_pEditOutput->Refresh();
    }

    if (m_pEditConsole) {
        m_pEditConsole->SetBackgroundColour(wxColour(iB));
        m_pEditConsole->SetForegroundColour(wxColour(iF));
        m_pEditConsole->Refresh();
    }

    if (m_pListBookmark) {
        m_pListBookmark->SetBackgroundColour(wxColour(iB));
        m_pListBookmark->SetForegroundColour(wxColour(iF));
        m_pListBookmark->Refresh();
    }
}

void CometFrame::CancelCallTip(void)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    if (iPageCount > 0) {
        ScriptEdit *pEdit = NULL;
        for (int ii = 0; ii < iPageCount; ii++) {
            pEdit = getEditor(ii);
            if (pEdit == NULL) {
                break;
            }
            pEdit->DoCallTipCancel();
        }
    }
}

void CometFrame::initDrop(void)
{
    if (m_pDropTarget == NULL) {
        DragAcceptFiles(true);
        m_pDropTarget = new (std::nothrow) SigmaDropTarget();
        SetDropTarget(m_pDropTarget);
    }
}
