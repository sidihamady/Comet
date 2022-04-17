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

void CometFrame::createSearchToolbar(void)
{
    if (NULL == m_pToolBar) {
        return;
    }

    bool bShown = (m_cboAnalyzer != NULL);

    if (false == bShown) {

        m_pToolBar->AddTool(ID_SEARCH_FIND, uT("Find"), wxBitmap(find_small_xpm), uT("Find in the current document"));
        m_pToolBar->AddTool(ID_SCRIPT_BOOKMARK_MOD, uT("Add/Remove Bookmark"), wxBitmap(bookmark_add_small_xpm), uT("Add or remove bookmark"));
        m_pToolBar->AddTool(ID_SCRIPT_BOOKMARK_PREV, uT("Previous Bookmark"), wxBitmap(bookmark_prev_small_xpm), uT("Go to the previous bookmark"));
        m_pToolBar->AddTool(ID_SCRIPT_BOOKMARK_NEXT, uT("Next Bookmark"), wxBitmap(bookmark_next_small_xpm), uT("Go to the next bookmark"));
        m_pToolBar->AddTool(ID_SCRIPT_BOOKMARK_DELALL, uT("Delete File Bookmarks"), wxBitmap(bookmark_deleteall_small_xpm), uT("Delete file bookmarks"));
        m_pToolBar->AddTool(ID_SCRIPT_BOOKMARK_LIST, uT("View File Bookmarks"), wxBitmap(bookmark_list_small_xpm), uT("View the file bookmarks list"));

        if (initAnalyze()) {
            m_pToolBar->AddControl(m_cboAnalyzer);
            m_pToolBar->AddTool(ID_MAIN_GOTO, uT("Go To Next"), wxBitmap(analyze_goto_small_xpm), uT("Go to the next item in the document"));
            m_pToolBar->AddTool(ID_MAIN_ANALYZE, uT("Update list"), wxBitmap(analyze_small_xpm), uT("Update the functions/sections list"));

#if defined(__WXMSW__)
            HWND hWnd = (HWND)m_cboAnalyzer->GetHWND();
            ::SetWindowLong(hWnd, GWL_EXSTYLE, ::GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_COMPOSITED);
#endif
        }

        // Realize called later
    }
}

void CometFrame::viewSearchToolbar(void)
{
    if (NULL == m_pToolBar) {
        return;
    }

    bool bShown = (m_pToolBar->FindById(ID_SEARCH_FIND) != NULL);

    if (false == bShown) {
        createSearchToolbar();
        m_pToolBar->Realize();
    }
    else {
        m_pToolBar->DeleteTool(ID_SEARCH_FIND);
        m_pToolBar->DeleteTool(ID_SCRIPT_BOOKMARK_MOD);
        m_pToolBar->DeleteTool(ID_SCRIPT_BOOKMARK_PREV);
        m_pToolBar->DeleteTool(ID_SCRIPT_BOOKMARK_NEXT);
        m_pToolBar->DeleteTool(ID_SCRIPT_BOOKMARK_DELALL);
        m_pToolBar->DeleteTool(ID_SCRIPT_BOOKMARK_LIST);
        if (m_cboAnalyzer) {
            m_pToolBar->DeleteTool(IDC_ANALYZE_LIST);
            m_cboAnalyzer = NULL;
        }
        m_pToolBar->DeleteTool(ID_MAIN_GOTO);
        m_pToolBar->DeleteTool(ID_MAIN_ANALYZE);
        m_pToolBar->Realize();
    }
}

void CometFrame::OnBookmarkList(wxCommandEvent &WXUNUSED(tEvent))
{
    wxMenu popMenu;

    updateBookmarkMenu(&popMenu, NULL);

    wxPoint ptMousePosition = GetMouseNotebookPos(m_pNotebookMain);

    PopupMenu(&popMenu, ptMousePosition);
}

void CometFrame::OnBookmarkListGoto(wxCommandEvent &tEvent)
{
    ScriptEdit *pEdit = getActiveEditor();

    if (pEdit == NULL) {
        return;
    }

    int idT = tEvent.GetId();

    int ii = idT - ID_SCRIPT_BOOKMARK_LIST_FIRST;
    if ((ii < 0) || (ii >= BOOKMARKLIST_MAXITEMSL)) {
        return;
    }

    wxMenu *pWnd = (wxMenu *)(tEvent.GetEventObject());
    if (pWnd == NULL) {
        return;
    }
    wxMenuItem *pItem = pWnd->FindItem(idT);
    if (pItem == NULL) {
        return;
    }

    wxString strT = pItem->GetHelp();

    if (strT.StartsWith(uT("Ln ")) == false) {
        return;
    }
    int iLen = (int)(strT.Length());

    int iFD = strT.Find(uT(":"));

    if ((iFD < 4) || (iFD >= iLen)) {
        return;
    }

    wxString strN = SigmaTrim(strT.Mid(2, iFD - 2));

    long iLine;

    if (strN.ToLong(&iLine) == false) {
        return;
    }

    int iiLine = (int)(iLine)-1;

    if ((iiLine >= 0) && (iiLine < pEdit->GetLineCount())) {
        pEdit->DoGotoLine(iiLine);
    }
}

void CometFrame::OnUpdateBookmarkList(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    ScriptEdit *pEdit = getActiveEditor();

    if (pEdit == NULL) {
        return;
    }

    CometFrame::enableUIitem(tEvent, pEdit->getBookmarkCount() > 0);
    // :WARNING: Do not update menu item here to avoid flickering
}

void CometFrame::OnUpdateBookmark(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    ScriptEdit *pEdit = this->getActiveEditor();
    if (pEdit == NULL) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    int idT = tEvent.GetId();
    if ((idT == ID_SCRIPT_BOOKMARK_DELALL) || (idT == ID_SCRIPT_BOOKMARK_NEXT) || (idT == ID_SCRIPT_BOOKMARK_PREV)) {
        CometFrame::enableUIitem(tEvent, pEdit->getBookmarkCount() > 0);
    }
    else if (idT == ID_BOOKMARK_DELETEALL) {
        CometFrame::enableUIitem(tEvent, getBookmarkCount() > 0);
    }
}

void CometFrame::updateBookmarkMenu(wxMenu *pMenu, wxMenuItem *pMenuItem)
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

    int idT = ID_SCRIPT_BOOKMARK_LIST_FIRST, ii;

    ScriptEdit *pEdit = getActiveEditor();

    if (pEdit == NULL) {
        return;
    }

    int iCount = pEdit->getBookmarkCount();

    if (pMenuItem != NULL) {
        pMenuItem->Enable(iCount >= 1);
    }

    if (iCount < 1) {
        return;
    }

    if (iCount > BOOKMARKLIST_MAXITEMSL) {
        iCount = BOOKMARKLIST_MAXITEMSL;
    }

    int iLine = 0;
    size_t iLen;

    int iLineCount = pEdit->GetLineCount();
    wxString strT = wxString::Format(uT("%d"), iLineCount + 1);
    wxString strF = uT("Ln %");
    strF += wxString::Format(uT("%d"), strT.Length());
    strF += uT("d:  ");
    wxString strItem;

    iLine = pEdit->MarkerNext(0, SCRIPT_MASK_BOOKMARK);
    if (iLine < 0) {
        // Should never happen
        return;
    }

    strT = pEdit->GetLine(iLine);
    strT = SigmaTrim(strT);
    iLen = strT.Length();

    if (iLen > LM_STRSIZEN) {
        strT = strT.Mid(0, LM_STRSIZEN - 3);
    }

    strItem = wxString::Format(LM_CSTR(strF), iLine + 1);
    strItem += strT;

    wxMenuItem *pItem = new wxMenuItem(pMenu, idT, strItem, strItem, wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(marker_next_small_xpm);
#endif
    pMenu->Append(pItem);

    ++iLine;
    ++idT;

    for (ii = 1; ii < iCount; ii++) {

        iLine = pEdit->MarkerNext(iLine, SCRIPT_MASK_BOOKMARK);
        if (iLine < 0) {
            // Should never happen
            break;
        }

        strT = pEdit->GetLine(iLine);
        strT = SigmaTrim(strT);
        iLen = strT.Length();

        if (iLen > LM_STRSIZEN) {
            strT = strT.Mid(0, LM_STRSIZEN - 3);
            strT += uT("...");
        }

        strItem = wxString::Format(LM_CSTR(strF), iLine + 1);
        strItem += strT;

        pMenu->Append(idT, strItem, strItem, wxITEM_NORMAL);

        ++iLine;
        ++idT;
        if (idT > ID_SCRIPT_BOOKMARK_LIST_LAST) {
            break;
        }
    }
}

void CometFrame::OnBookmarkFileDeleteAll(wxCommandEvent &tEvent)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = 0, idT = tEvent.GetId();

    iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        return;
    }

    int iret = SigmaMessageBox(uT("Delete file bookmarks?"), uT("Comet"),
                               wxYES | wxNO | wxCANCEL | wxICON_QUESTION | wxNO_DEFAULT, this,
                               uT("&Delete"), uT("D&on't delete"));
    if (iret != wxYES) {
        return;
    }
    pEdit->DoScriptAction(idT);
}

void CometFrame::OnBookmarkDeleteAll(wxCommandEvent &WXUNUSED(tEvent))
{
    deleteBookmarkAll();
}

void CometFrame::OnBookmarkRefresh(wxCommandEvent &WXUNUSED(tEvent))
{
    updateBookmark();
}

bool CometFrame::resetBookmarkArray(void)
{
    bookmark_t *pBookmark = NULL;

    if (m_arBookmark.empty() == false) {
        std::vector<bookmark_t *>::iterator itr, itrbegin, itrend;
        itrbegin = m_arBookmark.begin();
        itrend = m_arBookmark.end();
        for (itr = itrbegin; itr != itrend; ++itr) {
            pBookmark = *itr;
            if (pBookmark) {
                free(pBookmark);
                pBookmark = NULL;
                *itr = NULL;
            }
        }
        m_arBookmark.clear();

        return true;
    }

    return false;
}

bool CometFrame::canAddBookmark(void)
{
    return (m_arBookmark.size() < BOOKMARKLIST_MAXITEMS);
}

bookmark_t *CometFrame::addBookmark(const wxString &strFilename, int iLine, const wxString &strInfo, long iId)
{
    bookmark_t *pBookmark = NULL;

    //It is not necessary to check if already added, since item will be added only once.

    if (m_arBookmark.size() >= BOOKMARKLIST_MAXITEMS) {
        return NULL;
    }

    pBookmark = (bookmark_t *)malloc(sizeof(bookmark_t));
    if (pBookmark == NULL) {
        return NULL;
    }

    pBookmark->id = iId;
    pBookmark->filename[LM_STRSIZE - 1] = uT('\0');
    Tstrncpy(pBookmark->filename, LM_CSTR(strFilename), LM_STRSIZE - 1);
    pBookmark->line = iLine;
    pBookmark->info[LM_STRSIZE - 1] = uT('\0');
    Tstrncpy(pBookmark->info, LM_CSTR(strInfo), LM_STRSIZE - 1);
    int iLineLen = (int)Tstrlen(static_cast<const char_t *>(pBookmark->info));
    if ((iLineLen > 0) && (iLineLen < LM_STRSIZE)) {
        if ((pBookmark->info[iLineLen - 1] == uT('\n')) || (pBookmark->info[iLineLen - 1] == uT('\r'))) {
            pBookmark->info[iLineLen - 1] = uT('\0');
        }
        if (iLineLen > 1) {
            if ((pBookmark->info[iLineLen - 2] == uT('\n')) || (pBookmark->info[iLineLen - 2] == uT('\r'))) {
                pBookmark->info[iLineLen - 2] = uT('\0');
            }
        }
    }
    m_arBookmark.push_back(pBookmark);

    updateBookmark();

    return pBookmark;
}

// Called by BookmarkDlg
bool CometFrame::deleteBookmark(int indexT)
{
    bookmark_t *pBookmark = NULL;

    if ((m_arBookmark.empty() == false) && (indexT >= 0) && (indexT < (int)(m_arBookmark.size()))) {

        pBookmark = m_arBookmark[indexT];
        if (pBookmark) {
            wxString strFilename = static_cast<const char_t *>(pBookmark->filename);
            int iLine = pBookmark->line;

            free(pBookmark);
            pBookmark = NULL;
            m_arBookmark.erase(m_arBookmark.begin() + indexT);

            const int iPageCount = (int)(m_pNotebookMain->GetPageCount());
            if (iPageCount > 0) {
                ScriptEdit *pEdit = NULL;
                for (int ii = 0; ii < iPageCount; ii++) {
                    pEdit = getEditor(ii);
                    if (pEdit && (pEdit->GetFilename() == strFilename)) {
                        pEdit->DoDeleteBookmark(iLine);
                        break;
                    }
                }
            }

            return true;
        }
    }

    return false;
}

void CometFrame::updateBookmarks(long iId, const char_t *pszFilename)
{
    int iCount = this->getBookmarkCount();
    if ((NULL == pszFilename) || (uT('\0') == *pszFilename) || (iId < 1L) || (iCount < 1)) {
        return;
    }

    bookmark_t *pBookmark = NULL;
    int iUpdated = 0;
    for (int ii = 0; ii < iCount; ii++) {
        pBookmark = getBookmark(ii);
        if (pBookmark == NULL) {
            return;
        }
        if (iId == pBookmark->id) {
            Tstrcpy(pBookmark->filename, pszFilename);
            ++iUpdated;
        }
    }

    if (iUpdated > 0) {
        updateBookmark();
    }
}

void CometFrame::updateBookmark(void)
{
    if (m_pFilesysPanel) {
        wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pFilesysPanel);
        if (paneT.IsShown()) {
            if (m_pListBookmark) {
                m_pListBookmark->updateList();
            }
        }
    }
}

// Called by BookmarkDlg
bool CometFrame::deleteBookmarkAll(void)
{
    if (m_arBookmark.empty() == false) {

        int iret = SigmaMessageBox(uT("Delete all bookmarks?"), uT("Comet"),
                                   wxYES | wxNO | wxCANCEL | wxICON_QUESTION | wxNO_DEFAULT, this,
                                   uT("&Delete"), uT("D&on't delete"));
        if (iret != wxYES) {
            return false;
        }

        resetBookmarkArray();

        const int iPageCount = (int)(m_pNotebookMain->GetPageCount());
        if (iPageCount > 0) {
            ScriptEdit *pEdit = NULL;
            for (int ii = 0; ii < iPageCount; ii++) {
                pEdit = getEditor(ii);
                if (pEdit) {
                    pEdit->DoDeleteBookmarkAll();
                }
            }
        }

        updateBookmark();
    }

    return false;
}

// Called by ScriptEdit
bool CometFrame::deleteBookmark(const wxString &strFilename, int iLine, long iId)
{
    if (m_arBookmark.empty()) {
        return false;
    }
    bool fEmptyFilename = strFilename.IsEmpty();
    bool bTest;

    std::vector<bookmark_t *>::iterator itr;
    bookmark_t *pBookmark = NULL;
    bool bFound = false, bErased = false;

    itr = m_arBookmark.begin();

    while (itr != m_arBookmark.end()) {

        pBookmark = *itr;
        if (pBookmark == NULL) {
            itr = m_arBookmark.erase(itr);
            continue;
        }

        bErased = false;

        if ((iLine >= 0) && (pBookmark->line == iLine)) {
            if (fEmptyFilename) {
                bTest = (pBookmark->id == iId);
            }
            else {
                bTest = (Tstrcmp(LM_CSTR(strFilename), static_cast<const char_t *>(pBookmark->filename)) == 0);
            }
            if (bTest) {
                free(pBookmark);
                pBookmark = NULL;

                itr = m_arBookmark.erase(itr);

                bFound = true;
                bErased = true;
                break;
            }
        }

        else if (iLine < 0) {
            if (fEmptyFilename) {
                bTest = (pBookmark->id == iId);
            }
            else {
                bTest = (Tstrcmp(LM_CSTR(strFilename), static_cast<const char_t *>(pBookmark->filename)) == 0);
            }
            if (bTest) {
                free(pBookmark);
                pBookmark = NULL;

                itr = m_arBookmark.erase(itr);

                bFound = true;
                bErased = true;
            }
        }

        if (bErased == false) {
            ++itr;
        }
    }

    if (bFound) {
        updateBookmark();
    }

    return bFound;
}

bookmark_t *CometFrame::getBookmark(int posT)
{
    bookmark_t *pBookmark = NULL;

    if ((m_arBookmark.empty() == false) && (posT >= 0) && (posT < (int)(m_arBookmark.size()))) {
        pBookmark = m_arBookmark[posT];
    }

    return pBookmark;
}

int CometFrame::getBookmarkCount(void)
{
    return ((m_arBookmark.empty() == false) ? (int)(m_arBookmark.size()) : 0);
}

void CometFrame::sortBookmark(int iColumn, bool bAsc /* = true*/)
{
    bool bSorted = false;
    if ((m_arBookmark.empty() == false) && (m_arBookmark.size() > 1)) {
        if (iColumn == 0) {

            std::vector<bookmark_t *>::iterator itr, itrprev, itrend;
            itrprev = m_arBookmark.begin();
            itrend = m_arBookmark.end();

            if (bAsc) {
                std::sort(itrprev, itrend, bookmarkCompareFilename());
            }
            else {
                std::sort(itrprev, itrend, bookmarkCompareFilenameDesc());
            }

            // >> SubSort
            bookmark_t *pBookmark = *itrprev;
            if (pBookmark) {
                wxString strFilename = static_cast<const char_t *>(pBookmark->filename),
                         strFilenameT;
                int icount;
                int iSize = (int)(m_arBookmark.size());
                itr = itrprev + 1;
                for (icount = 0; icount < iSize; icount++) {
                    pBookmark = *itr;
                    if (pBookmark == NULL) {
                        break;
                    }
                    strFilenameT = static_cast<const char_t *>(pBookmark->filename);
                    if (strFilenameT.IsSameAs(strFilename) == false) {
                        if (itr != (itrprev + 1)) {
                            std::sort(itrprev, itr, bookmarkCompareLine());
                        }

                        // Root
                        itrprev = itr;
                        strFilename = strFilenameT;
                        //
                    }
                    if (itrprev == itrend) {
                        break;
                    }
                    ++itr;
                    if (itr == itrend) {
                        if (itr != itrprev) {
                            std::sort(itrprev, itr, bookmarkCompareLine());
                        }
                        break;
                    }
                }
            }
            // <<

            bSorted = true;
        }
        else if (iColumn == 1) {
            if (bAsc) {
                std::sort(m_arBookmark.begin(), m_arBookmark.end(), bookmarkCompareLine());
            }
            else {
                std::sort(m_arBookmark.begin(), m_arBookmark.end(), bookmarkCompareLineDesc());
            }
            bSorted = true;
        }
        else if (iColumn == 2) {
            if (bAsc) {
                std::sort(m_arBookmark.begin(), m_arBookmark.end(), bookmarkCompareInfo());
            }
            else {
                std::sort(m_arBookmark.begin(), m_arBookmark.end(), bookmarkCompareInfoDesc());
            }
            bSorted = true;
        }
    }
    if (bSorted) {
        updateBookmark();
    }
}
