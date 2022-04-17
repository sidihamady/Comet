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
#include "CometFileExplorer.h"

#include <wx/clipbrd.h>
#include <wx/utils.h>
#include <wx/dir.h>
#include <wx/wupdlock.h>

void CometFileExplorer::OnItemRightClick(wxTreeEvent &tEvent)
{
    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();
    pTreeCtrl->SelectItem(tEvent.GetItem());

    tEvent.Skip();
}

void CometFileExplorer::OnItemDoubleClick(wxTreeEvent &tEvent)
{
    tEvent.Skip();

    wxTreeItemId treeId = (wxTreeItemId)tEvent.GetItem();
    if (m_RootId == treeId) {
        doBrowseDir();
        return;
    }

    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();
    wxDirItemData *pData = (wxDirItemData *)(pTreeCtrl->GetItemData(treeId));
    if (pData != NULL) {
        doOpenFile(pData->m_path);
        return;
    }
}

void CometFileExplorer::OnItemMenu(wxTreeEvent &tEvent)
{
    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();

    const wxTreeItemId treeId = (const wxTreeItemId)tEvent.GetItem();

    wxDirItemData *pData = (wxDirItemData *)(pTreeCtrl->GetItemData(treeId));
    if (pData == NULL) {
        tEvent.Veto();
        return;
    }

    const wxString filenameT = pData->m_path;
    bool bDir = ::wxDirExists(filenameT);
    bool bFile = bDir ? false : ::wxFileExists(filenameT);

    if ((bDir == false) && (bFile == false)) {

        if ((bDir == false) && (m_RootId == treeId)) {
            wxMenu popMenu;
            wxMenuItem *pItem;
            pItem = new wxMenuItem(&popMenu, ID_EXPLORER_BROWSE, uT("Change Directory"), uT("Change the working directory"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
            pItem->SetBitmap(folder_small_xpm);
#endif
            popMenu.Append(pItem);
            wxPoint ptMousePosition = ::wxGetMousePosition();
            ptMousePosition = this->ScreenToClient(ptMousePosition);
            PopupMenu(&popMenu, ptMousePosition);

            return;
        }

        tEvent.Veto();
        return;
    }

    tEvent.Skip();

    wxMenu popMenu;
    wxMenuItem *pItem;

    bool bIsExpanded = pTreeCtrl->IsExpanded(treeId);
    bool bDoExpand = (bIsExpanded == false);

    const wxFileName fname = filenameT;
    bool bWritable = bDir ? fname.IsDirWritable() : fname.IsFileWritable();

    if (m_RootId != treeId) {

        if (bDir && bIsExpanded) {
            popMenu.Append(ID_EXPLORER_REFRESH, uT("Refresh\tF5"), uT("Refresh the current directory"), wxITEM_NORMAL);
        }

        popMenu.Append(ID_EXPLORER_OPEN, bDir ? (bDoExpand ? uT("Expand") : uT("Collapse")) : uT("Open"), bDir ? (bDoExpand ? uT("Expand the selected directory") : uT("Collapse the selected directory")) : uT("Open the selected file"), wxITEM_NORMAL);
        popMenu.AppendSeparator();

        if (bWritable) {
            if (bDir) {
                popMenu.Append(ID_EXPLORER_CREATEFILE, uT("New File"), uT("New File"), wxITEM_NORMAL);
                popMenu.Append(ID_EXPLORER_CREATEDIR, uT("New Directory"), uT("New Directory"), wxITEM_NORMAL);
                popMenu.AppendSeparator();
                if (this->canPaste(false)) {
                    pItem = popMenu.Append(ID_EXPLORER_PASTE, uT("Paste"), uT("Paste File"), wxITEM_NORMAL);
                    popMenu.AppendSeparator();
                }
            }

            popMenu.Append(ID_EXPLORER_RENAME, uT("Rename"), bDir ? uT("Rename the selected directory") : uT("Rename the selected file"), wxITEM_NORMAL);

            if (bFile) {
                popMenu.Append(ID_EXPLORER_CUT, uT("Cut"), uT("Cut File"), wxITEM_NORMAL);
                popMenu.Append(ID_EXPLORER_COPY, uT("Copy"), uT("Copy File"), wxITEM_NORMAL);
                popMenu.Append(ID_EXPLORER_DELETE, uT("Delete"), uT("Delete the selected file"), wxITEM_NORMAL);
            }

            popMenu.AppendSeparator();
        }
        else {
            if (bFile) {
                popMenu.Append(ID_EXPLORER_COPY, uT("Copy"), uT("Copy File"), wxITEM_NORMAL);
                popMenu.AppendSeparator();
            }
        }
    }
    else { // Root Dir
        pItem = new wxMenuItem(&popMenu, ID_EXPLORER_BROWSE, uT("Change Directory"), uT("Change the working directory"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
        pItem->SetBitmap(folder_small_xpm);
#endif
        popMenu.Append(pItem);
        popMenu.AppendSeparator();
        popMenu.Append(ID_EXPLORER_REFRESH, uT("Refresh\tF5"), uT("Refresh the current directory"), wxITEM_NORMAL);
        popMenu.AppendSeparator();
        if (bWritable) {
            popMenu.Append(ID_EXPLORER_CREATEFILE, uT("New File"), uT("New File"), wxITEM_NORMAL);
            popMenu.Append(ID_EXPLORER_CREATEDIR, uT("New Directory"), uT("New Directory"), wxITEM_NORMAL);
            popMenu.AppendSeparator();
            if (this->canPaste(false)) {
                pItem = popMenu.Append(ID_EXPLORER_PASTE, uT("Paste"), uT("Paste File"), wxITEM_NORMAL);
                popMenu.AppendSeparator();
            }
        }
    }

#ifdef __WXMSW__
    popMenu.Append(ID_EXPLORER_OPENEXT, bDir ? uT("Open in Explorer") : uT("Open Containing Folder"), bDir ? uT("Open in Explorer") : uT("Open Containing Folder"), wxITEM_NORMAL);
#endif

    popMenu.Append(ID_EXPLORER_COPYPATH, uT("Copy Path"), uT("Copy Full Path"), wxITEM_NORMAL);

    if (bDir && (m_RootId != treeId)) {
        pItem = new wxMenuItem(&popMenu, ID_EXPLORER_CHANGEDIR, uT("Set as Root Directory"), uT("Set as Root Directory"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
        pItem->SetBitmap(folder_small_xpm);
#endif
        popMenu.Append(pItem);
    }

#ifdef __WXMSW__
    popMenu.AppendSeparator();
    if (bDir) {
        pItem = new wxMenuItem(&popMenu, ID_EXPLORER_OPENSHELL, uT("Command Prompt Here"), uT("Open command prompt in this location"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
        pItem->SetBitmap(prompt_small_xpm);
#endif
        popMenu.Append(pItem);
    }
    popMenu.Append(ID_EXPLORER_FILEPROPS, uT("File Properties"), uT("View File Properties"), wxITEM_NORMAL);
#endif

    wxPoint ptMousePosition = ::wxGetMousePosition();
    ptMousePosition = this->ScreenToClient(ptMousePosition);

    PopupMenu(&popMenu, ptMousePosition);
}

void CometFileExplorer::OnItemBeginRename(wxTreeEvent &tEvent)
{
    wxTreeItemId treeId = (wxTreeItemId)tEvent.GetItem();
    if (m_RootId == treeId) {
        tEvent.Veto();
        return;
    }

    if (m_bNewItem == false) {
        wxFileName fname = this->GetPath();
        bool bCanRename = fname.IsDirWritable() || fname.IsFileWritable();
        if (bCanRename == false) {
            tEvent.Veto();
            return;
        }
    }

    tEvent.Skip();
}

void CometFileExplorer::OnItemEndRename(wxTreeEvent &tEvent)
{
    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();
    const wxTreeItemId treeId = (const wxTreeItemId)(tEvent.GetItem());

    if (tEvent.IsEditCancelled()) {
        if (m_bNewItem) {
            pTreeCtrl->Delete(treeId);
        }
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        if (m_bNewItem) {
            pTreeCtrl->Delete(treeId);
        }
        tEvent.Veto();
        return;
    }

    pFrame->OutputStatusbar(uT("Ready"), SIGMAFRAME_TIMER_NONE);

    wxDirItemData *pData = (wxDirItemData *)(pTreeCtrl->GetItemData(treeId));
    if (pData == NULL) {
        if (m_bNewItem) {
            pTreeCtrl->Delete(treeId);
        }
        tEvent.Veto();
        return;
    }

    if (m_bNewItem == false) {
        wxFileName fname = pData->m_path;
        bool bCanRename = fname.IsDirWritable() || fname.IsFileWritable();
        if (bCanRename == false) {
            wxString strT = uT("'");
            strT += pData->m_path;
            strT += uT("' cannot be renamed: permission denied");
            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
            tEvent.Veto();
            return;
        }
    }

    const wxString strSelLabel = tEvent.GetLabel();

    if (strSelLabel.IsEmpty() ||
        (strSelLabel == uT(".")) ||
        (strSelLabel == uT("..")) ||
        (strSelLabel.Find(uT('/')) >= 0) ||
        (strSelLabel.Find(uT('\\')) >= 0) ||
        (strSelLabel.Find(uT('|')) >= 0)) {

        if (m_bNewItem) {
            pTreeCtrl->Delete(treeId);
        }
        tEvent.Veto();
        return;
    }

    wxString strInvalidChars = wxFileName::GetForbiddenChars();

    for (size_t ii = 0; ii < strInvalidChars.Length(); ii++) {
        if (strSelLabel.Find(strInvalidChars.GetChar(ii)) >= 0) {
            if (m_bNewItem) {
                pTreeCtrl->Delete(treeId);
            }
            tEvent.Veto();
            return;
        }
    }

    const wxString strOldPath(pData->m_path);
    wxString strNewPath(wxPathOnly(pData->m_path));
    strNewPath += wxString(wxFILE_SEP_PATH);
    strNewPath += strSelLabel;

    if (strNewPath.IsSameAs(strOldPath) || ::wxFileExists(strNewPath)) {
        if (m_bNewItem) {
            pTreeCtrl->Delete(treeId);
        }
        tEvent.Veto();
        return;
    }

    if (m_bNewItem == false) {

        wxDirItemData *pDataRoot = (wxDirItemData *)(pTreeCtrl->GetItemData(m_RootId));
        if (pDataRoot == NULL) {
            tEvent.Veto();
            return;
        }

        // To be able to rename dir, change current dir to root
        wxString strRootDir = pDataRoot->m_path;
        if (strRootDir.Length() > 2) {
            // ::ANSI::    NOT::UNICODE::
            char szT[LM_STRSIZE];
            memset(szT, 0, LM_STRSIZE * sizeof(char));
            strcpy(szT, LM_U8STR(strRootDir));
            chdir(szT);
            //
        }

        bool bDir = ::wxDirExists(strOldPath);

        if (wxRenameFile(strOldPath, strNewPath, false)) {
            if (pFrame->explorerOnRename(bDir, strOldPath, strNewPath) == false) {
                wxRenameFile(strNewPath, strOldPath, false);
                tEvent.Veto();
                wxString strT = uT("'");
                strT += strOldPath;
                strT += uT("' cannot be renamed");
                pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
                return;
            }
            pData->SetNewDirName(strNewPath);
        }
        else {
            tEvent.Veto();
            if (pFrame->explorerIsOpened(strOldPath, true)) {
                wxString strT = uT("'");
                strT += strOldPath;
                strT += uT("' cannot be renamed: document open");
                pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
            }
        }

        pFrame->explorerChdir();
    }
    else {
        if (pData->m_isDir) {
            if (wxMkdir(strNewPath)) {
                pData->SetNewDirName(strNewPath);
            }
            else {
                pTreeCtrl->Delete(treeId);
                tEvent.Veto();
            }
        }
        else {
            wxFile fnNew(strNewPath, wxFile::write);
            if (fnNew.IsOpened()) {
                fnNew.Close();
                pData->SetNewDirName(strNewPath);
            }
            else {
                pTreeCtrl->Delete(treeId);
                tEvent.Veto();
            }
        }

        wxTreeItemId itemT = (wxTreeItemId)(pTreeCtrl->GetItemParent(treeId));

        if (itemT != m_RootId) {
            this->CollapseDir(itemT);
            this->ExpandDir(itemT);
        }
        else {
            this->ReCreateTree();
        }
        this->ExpandPath(strNewPath);

        m_bNewItem = false;
    }
}

void CometFileExplorer::OnItemDelete(wxTreeEvent &tEvent)
{
    if (m_bDeleteItem == false) {
        tEvent.Veto();
        return;
    }

    m_bDeleteItem = false;

    if (m_bNewItem) {
        m_bNewItem = false;
        tEvent.Veto();
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        tEvent.Veto();
        return;
    }

    pFrame->OutputStatusbar(uT("Ready"), SIGMAFRAME_TIMER_NONE);

    const wxTreeItemId treeId = (const wxTreeItemId)(tEvent.GetItem());

    if (treeId.IsOk() == false) {
        tEvent.Veto();
        return;
    }

    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();

    wxDirItemData *pData = (wxDirItemData *)(pTreeCtrl->GetItemData(treeId));
    if (pData == NULL) {
        tEvent.Veto();
        return;
    }

    if (::wxDirExists(pData->m_path) || (::wxFileExists(pData->m_path) == false)) {
        tEvent.Veto();
        return;
    }

    wxFileName fname = pData->m_path;

    wxString strT;

    if (fname.IsFileWritable() == false) {
        tEvent.Veto();
        strT = uT("'");
        strT += pData->m_path;
        strT += uT("' cannot be deleted: permission denied");
        pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        return;
    }

    if (pFrame->explorerOnDelete(pData->m_path) == false) {
        tEvent.Veto();
        strT = uT("'");
        strT += pData->m_path;
        strT += uT("' not deleted");
        pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        return;
    }

    strT = uT("'");
    strT += pData->m_path;
    strT += uT("' deleted");
    pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);

    return;
}

void CometFileExplorer::OnItemDragBegin(wxTreeEvent &tEvent)
{
    m_SourceId = tEvent.GetItem();

    if (m_SourceId.IsOk() == false) {
        tEvent.Veto();
        return;
    }

    if (m_RootId == m_SourceId) {
        m_SourceId.Unset();
        tEvent.Veto();
        return;
    }

    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();

    wxDirItemData *pDataSource = (wxDirItemData *)(pTreeCtrl->GetItemData(m_SourceId));
    if (pDataSource == NULL) {
        m_SourceId.Unset();
        tEvent.Veto();
        return;
    }

    if ((::wxFileExists(pDataSource->m_path) == false) && (::wxDirExists(pDataSource->m_path) == false)) {
        m_SourceId.Unset();
        tEvent.Veto();
        return;
    }

    wxTreeItemId treeId = pTreeCtrl->GetItemParent(m_SourceId);

    if (treeId.IsOk() == false) {
        m_SourceId.Unset();
        tEvent.Veto();
        return;
    }

    tEvent.Allow();
}

void CometFileExplorer::OnItemDragEnd(wxTreeEvent &tEvent)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    if (m_SourceId.IsOk() == false) {
        tEvent.Veto();
        return;
    }

    wxTreeItemId treeId = tEvent.GetItem();

    if (treeId.IsOk() == false) {
        m_SourceId.Unset();
        tEvent.Veto();
        return;
    }

    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();

    wxDirItemData *pDataDest = (wxDirItemData *)(pTreeCtrl->GetItemData(treeId));
    if (pDataDest == NULL) {
        m_SourceId.Unset();
        tEvent.Veto();
        return;
    }

    const wxString strPathDest = pDataDest->m_path;
    if (::wxDirExists(strPathDest) == false) {
        m_SourceId.Unset();
        tEvent.Veto();
        return;
    }

    wxDirItemData *pDataSource = (wxDirItemData *)(pTreeCtrl->GetItemData(m_SourceId));
    if (pDataSource == NULL) {
        m_SourceId.Unset();
        tEvent.Veto();
        return;
    }

    const wxString strPathSource = pDataSource->m_path;
    if ((::wxFileExists(strPathSource) == false) && (::wxDirExists(strPathSource) == false)) {
        m_SourceId.Unset();
        tEvent.Veto();
        return;
    }
    const wxString strNameSource = pDataSource->m_name;

    wxString strNewPath(strPathDest), strSep = wxString(wxFILE_SEP_PATH);
    if (strNewPath.EndsWith(strSep) == false) {
        strNewPath += strSep;
    }
    strNewPath += strNameSource;

    wxString strT = uT("'");
    strT += strPathSource;

    if (::wxFileExists(strNewPath) || ::wxDirExists(strNewPath)) {
        m_SourceId.Unset();
        tEvent.Veto();
        strT += uT("' cannot be moved: found in destination");
        pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        return;
    }

    bool bDir = ::wxDirExists(strPathSource), bFile = ::wxFileExists(strPathSource);
    if (((bDir == false) && (bFile == false)) || (bDir && strNewPath.StartsWith(strPathSource))) {
        m_SourceId.Unset();
        tEvent.Veto();
        strT += uT("' cannot be moved");
        pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        return;
    }

    // get confirmation and check if all files are copied
    wxString strTT = uT("Do you want to move '");
    strTT += CometFrame::getLabelFromPath(LM_CSTR(strPathSource));
    strTT += uT("'\nto '");
    strTT += CometFrame::getLabelFromPath(LM_CSTR(strNewPath));
    strTT += uT("'?");
    int iret = SigmaMessageBox(strTT, uT("Comet"),
                               wxYES | wxNO | wxICON_QUESTION | wxNO_DEFAULT, this,
                               uT("&Move"), uT("D&on't move"));
    if (iret != wxYES) {
        m_SourceId.Unset();
        tEvent.Veto();
        strT += uT("' cannot be moved");
        pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        return;
    }
    //

    bool bOK = false;
    int iDestCount = 0, iSrcCount = 0;
    if (this->doCopyFile(strPathSource, strNewPath, &iSrcCount, &iDestCount)) {
        bOK = doDeleteFile(strPathSource);
        if (bOK) {
            if (bDir) {
                ::wxRmdir(strPathSource);
            }
            strT += uT("' successfully moved");

            this->CollapsePath(strNewPath);
            this->ExpandPath(strNewPath);
        }
    }

    if (bOK == false) {
        // check if all files are well copied
        if (doDeleteFile(strNewPath)) {
            if (bDir) {
                ::wxRmdir(strNewPath);
            }
        }
        m_SourceId.Unset();
        tEvent.Veto();
        strT += uT("' cannot be moved");
        pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        return;
        //
    }

    pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);

    m_SourceId.Unset();
    return;
}

void CometFileExplorer::doSetDir(const wxString &strDir)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }
    if (::wxDirExists(strDir) == false) {
        return;
    }

    m_strRoot = strDir;
    m_strRootLabel = m_strRoot;
    pFrame->setLastDir(LM_CSTR(m_strRoot));
    this->ReCreateTree();
    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();
    pTreeCtrl->SetScrollPos(wxHORIZONTAL, 0);
}

void CometFileExplorer::doBrowseDir(void)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    wxDirDialog *dirDlg = new (std::nothrow) wxDirDialog(this, uT("Choose a directory"), m_strRoot, 0, wxDefaultPosition);
    if (dirDlg && (dirDlg->ShowModal() == wxID_OK)) {
        doSetDir(dirDlg->GetPath());
    }
}

void CometFileExplorer::doRefreshDir(void)
{
    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();

    this->ReCreateTree();

    pTreeCtrl->SetScrollPos(wxHORIZONTAL, 0);

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame) {
        pFrame->OutputStatusbar(uT("Explorer refresh done"), SIGMAFRAME_TIMER_SHORT);
    }
}

bool CometFileExplorer::doOpenFile(const wxString &strFilename)
{
    if (::wxDirExists(strFilename)) {
        return true;
    }

    if (::wxFileExists(strFilename)) {
        CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
        if (NULL == pFrame) {
            // should never happen
            return false;
        }
        ScriptEdit *pEdit = pFrame->fileOpen(strFilename, false);
        return (pEdit != NULL);
    }
    else {
        wxFileName fname = strFilename;
        wxString strPath = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
        this->CollapsePath(strPath);
        this->ExpandPath(strPath);
        return false;
    }
}

bool CometFileExplorer::doRenameItem(void)
{
    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();
    wxTreeItemId itemT = pTreeCtrl->GetSelection();
    (void)(pTreeCtrl->EditLabel(itemT));
    return true;
}

bool CometFileExplorer::doDeleteItem(void)
{
    if (m_bDeleteItem == false) {
        return false;
    }

    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();

    const wxTreeItemId treeId = (const wxTreeItemId)(pTreeCtrl->GetSelection());
    if (treeId.IsOk() == false) {
        m_bDeleteItem = false;
        return false;
    }

    wxDirItemData *pData = (wxDirItemData *)(pTreeCtrl->GetItemData(treeId));
    if (pData == NULL) {
        m_bDeleteItem = false;
        return false;
    }

    const wxString filenameT = pData->m_path;

    if (::wxDirExists(filenameT) || (::wxFileExists(filenameT) == false)) {
        m_bDeleteItem = false;
        return false;
    }

    wxFileName fname = filenameT;
    wxString strPath = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    pTreeCtrl->Delete(treeId);

    this->CollapsePath(strPath);
    this->ExpandPath(strPath);

    m_bDeleteItem = false;
    return true;
}

bool CometFileExplorer::doCreateItem(bool bDir)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return false;
    }

    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();

    wxString strRootDir;

    wxTreeItemId itemT = pTreeCtrl->GetSelection();

    wxDirItemData *pData = (wxDirItemData *)(pTreeCtrl->GetItemData(itemT));
    if (pData == NULL) {
        return false;
    }

    const wxString filenameT = pData->m_path;
    if (::wxDirExists(filenameT)) {
        strRootDir = filenameT;
    }
    else {
        return false;
    }

    if ((strRootDir.EndsWith(uT("\\")) == false) && (strRootDir.EndsWith(uT("/")) == false)) {
#ifdef __WXMSW__
        strRootDir += uT("\\");
#else
        strRootDir += uT("/");
#endif
    }

    wxDirItemData *itemNew = new (std::nothrow) wxDirItemData(strRootDir, uT(""), bDir);
    if (itemNew) {
        wxTreeItemId itemNewId = this->AppendItem(itemT, uT(""), -1, -1, itemNew);
        pTreeCtrl->SelectItem(itemNewId);
        m_bNewItem = true;
        doRenameItem();
    }

    return false;
}

bool CometFileExplorer::canPaste(bool bGet, wxString *pstrFilename /* = NULL*/)
{
    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();

    if (bGet) {
        m_CopyId = pTreeCtrl->GetSelection();
    }

    if (m_CopyId.IsOk() == false) {
        return false;
    }

    wxDirItemData *pData = (wxDirItemData *)(pTreeCtrl->GetItemData(m_CopyId));
    if (pData == NULL) {
        m_CopyId.Unset();
        return false;
    }

    if (pstrFilename) {
        *pstrFilename = pData->m_path;
    }

    if (::wxFileExists(pData->m_path) == false) {
        m_CopyId.Unset();
        return false;
    }

    return true;
}

bool CometFileExplorer::doCutCopyItem(bool bCut)
{
    if (canPaste(true) == false) {
        m_CopyId.Unset();
        return false;
    }

    m_bCutItem = bCut;
    return true;
}

bool CometFileExplorer::doCutItem(void)
{
    return doCutCopyItem(true);
}

bool CometFileExplorer::doCopyItem(void)
{
    return doCutCopyItem(false);
}

bool CometFileExplorer::doPasteItem(void)
{
    wxString fileSource;
    wxString strSep = wxString(wxFILE_SEP_PATH);

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        if (m_CopyId.IsOk()) {
            m_CopyId.Unset();
        }
        m_bCutItem = false;
        return false;
    }

    if (canPaste(false, &fileSource) == false) {
        m_CopyId.Unset();
        m_bCutItem = false;
        pFrame->OutputStatusbar(uT("No file to paste"), SIGMAFRAME_TIMER_SHORT);
        return false;
    }

    wxTreeCtrl *pTreeCtrl = GetTreeCtrl();
    const wxTreeItemId treeId = (const wxTreeItemId)(pTreeCtrl->GetSelection());

    if (treeId.IsOk() == false) {
        m_CopyId.Unset();
        m_bCutItem = false;
        return false;
    }

    wxDirItemData *pDataDest = (wxDirItemData *)(pTreeCtrl->GetItemData(treeId));
    if (pDataDest == NULL) {
        m_CopyId.Unset();
        m_bCutItem = false;
        return false;
    }

    wxString fileDest = pDataDest->m_path;

    if (::wxDirExists(fileDest) == false) {
        m_CopyId.Unset();
        m_bCutItem = false;
        pFrame->OutputStatusbar(uT("Can paste only to a directory"), SIGMAFRAME_TIMER_SHORT);
        return false;
    }

    wxFileName fnameSource = fileSource;
    if (fileDest.EndsWith(strSep) == false) {
        fileDest += strSep;
    }
    fileDest += fnameSource.GetFullName();

    wxString strT;

    bool bOK = false;

    int iDestCount = 0, iSrcCount = 0;

    if (::wxFileExists(fileDest)) {

        wxString fileDestCopy = pDataDest->m_path;
        if (fileDestCopy.EndsWith(strSep) == false) {
            fileDestCopy += strSep;
        }
        fileDestCopy += fnameSource.GetName();
        fileDestCopy += uT("_copy");
        if (fnameSource.GetExt().IsEmpty() == false) {
            fileDestCopy += uT(".");
            fileDestCopy += fnameSource.GetExt();
        }

        if (::wxFileExists(fileDestCopy)) {
            m_CopyId.Unset();
            m_bCutItem = false;
            strT = uT("Cannot paste: '");
            strT += CometFrame::getLabelFromPath(fileDestCopy);
            strT += uT("' already exists");
            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
            return false;
        }

        fileDest = fileDestCopy;
        bOK = ::wxCopyFile(fileSource, fileDestCopy, false);

        // check if all files are well copied
        if (bOK) {
            iSrcCount += 1;
            const wxFileOffset iFileSizeSrc = getFileSize(fileSource);
            const wxFileOffset iFileSizeDest = getFileSize(fileDestCopy);
            bOK = (iFileSizeSrc >= 0L) && (iFileSizeSrc == iFileSizeDest);
            if (bOK) {
                iDestCount += 1;
            }
        }
        //
    }
    else {
        bOK = this->doCopyFile(fileSource, fileDest, &iSrcCount, &iDestCount);
    }

    if (bOK == false) {
        m_CopyId.Unset();
        m_bCutItem = false;
        strT = uT("Cannot paste to '");
        strT += CometFrame::getLabelFromPath(fileDest);
        strT += uT("'");
        pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        return false;
    }

    wxFileName fnameDest = fileDest;

    wxString strPathSource = fnameSource.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    wxString strPathDest = fnameDest.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    if (m_bCutItem) {
        m_bCutItem = false;
        this->doDeleteFile(fileSource);
    }

    m_CopyId.Unset();
    m_bCutItem = false;

    this->CollapsePath(strPathSource);
    this->CollapsePath(strPathDest);
    this->ExpandPath(fileDest);

    strT = uT("Paste to '");
    strT += CometFrame::getLabelFromPath(fileDest);
    strT += uT("' successfully done");
    pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);

    return true;
}

bool CometFileExplorer::doOpenExplorer(void)
{
#ifdef __WXMSW__
    wxString filenameT = this->GetPath();
    wxFileName fname = filenameT;
    wxString strT = uT("explorer ");
    strT += (::wxDirExists(filenameT)) ? uT("/e, ") : uT("/select, ");
    strT += fname.GetFullPath();
    ::wxExecute(strT, wxEXEC_ASYNC, NULL);
#endif
    return true;
}

bool CometFileExplorer::doCopyPath(void)
{
    if (wxTheClipboard && wxTheClipboard->Open()) {
        wxTheClipboard->SetData(new wxTextDataObject(this->GetPath()));
        wxTheClipboard->Flush();
        wxTheClipboard->Close();
        return true;
    }

    return false;
}

bool CometFileExplorer::doChangeDir(void)
{
    wxString filenameT = this->GetPath();
    if (::wxDirExists(filenameT)) {
        doSetDir(filenameT);
        return true;
    }

    return false;
}

bool CometFileExplorer::doOpenShell(void)
{
#ifdef __WXMSW__
    wxString filenameT = this->GetPath();
    wxFileName fname = filenameT;
    wxString strT = uT("cmd.exe /K \"cd /d ");
    strT += (::wxDirExists(filenameT) ? fname.GetFullPath() : fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
    strT += uT("\"");
    ::wxExecute(strT, wxEXEC_ASYNC, NULL);
    return true;
#endif
    return false;
}

bool CometFileExplorer::doViewProperties(void)
{
#ifdef __WXMSW__
    wxString filenameT = this->GetPath();
    wxFileName fname = filenameT;
    SHELLEXECUTEINFO infoT;
    memset(&infoT, 0, sizeof(SHELLEXECUTEINFO));
    infoT.cbSize = sizeof(SHELLEXECUTEINFO);
    infoT.lpVerb = uT("properties");
    infoT.lpFile = LM_CSTR(fname.GetFullPath());
    infoT.nShow = SW_SHOW;
    infoT.fMask = SEE_MASK_INVOKEIDLIST;
    ShellExecuteEx(&infoT);
    return true;
#endif
    return false;
}

bool CometFileExplorer::doCopyFile(const wxString &strSrc, const wxString &strDest, int *iSrcCount, int *iDestCount)
{
    wxFileName fnameSrc = strSrc, fnameDest = strDest;
    wxString strSourceName, strSourcePath, strDestPath;

#ifdef __WXMSW__
    static const wxString strSep = uT("\\");
#else
    static const wxString strSep = uT("/");
#endif

    if (fnameSrc.FileExists()) {

        strSourceName = fnameSrc.GetFullName();

        strSourcePath = strSrc;

        strDestPath = fnameDest.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + strSourceName;
        if (::wxFileExists(strDestPath)) {
            return false;
        }

        bool bOK = ::wxCopyFile(strSourcePath, strDestPath, false);

        // check if file are well copied
        if (bOK) {
            *iSrcCount += 1;
            const wxFileOffset iFileSizeSrc = getFileSize(strSourcePath);
            const wxFileOffset iFileSizeDest = getFileSize(strDestPath);
            bOK = (iFileSizeSrc >= 0L) && (iFileSizeSrc == iFileSizeDest);
            if (bOK) {
                *iDestCount += 1;
            }
        }
        //

        return bOK;
    }
    else if (fnameSrc.DirExists()) {

        strDestPath = strDest;
        if (strDestPath.EndsWith(strSep) == false) {
            strDestPath += strSep;
        }
        strDestPath += strSourceName + strSep;

        if (wxFileName::Mkdir(strDestPath, 0777, wxPATH_MKDIR_FULL) == false) {
            return false;
        }
    }
    else {
        return false;
    }

    strSourcePath = strSrc;
    if (strSourcePath.EndsWith(strSep) == false) {
        strSourcePath += strSep;
    }

    strDestPath = strDest;
    if (strDestPath.EndsWith(strSep) == false) {
        strDestPath += strSep;
    }

    wxDir dirT(strSourcePath);
    if (dirT.IsOpened() == false) {
        return false;
    }

    wxString strSourceT, strDestT;
    bool bFlag = dirT.GetFirst(&strSourceName);
    while (bFlag) {
        strSourceT = strSourcePath + strSourceName;
        strDestT = strDestPath + strSourceName;

        if (doCopyFile(strSourceT, strDestT, iSrcCount, iDestCount) == false) {
            return false;
        }

        bFlag = dirT.GetNext(&strSourceName);
    }

    return true;
}

bool CometFileExplorer::doDeleteFile(const wxString &strFile)
{
    wxFileName fnameT = strFile;

#ifdef __WXMSW__
    static const wxString strSep = uT("\\");
#else
    static const wxString strSep = uT("/");
#endif

    if (fnameT.FileExists()) {
        return (Tunlink(LM_CSTR(strFile)) == 0);
    }
    else if (fnameT.DirExists() == false) {
        return false;
    }

    wxString strPath = strFile, strSourceName;
    if (strPath.EndsWith(strSep) == false) {
        strPath += strSep;
    }

    wxDir dirT(strPath);
    if (dirT.IsOpened() == false) {
        return false;
    }

    if ((dirT.HasFiles() == false) && (dirT.HasSubDirs() == false)) {
        return ::wxRmdir(strPath);
    }

    wxString strPathT;
    bool bFlag = dirT.GetFirst(&strSourceName);
    while (bFlag) {
        if (strSourceName.IsEmpty() || (strSourceName == uT(".")) || (strSourceName == uT(".."))) {
            bFlag = dirT.GetNext(&strSourceName);
            continue;
        }

        strPathT = strPath + strSourceName;

        if (doDeleteFile(strPathT) == false) {
            return false;
        }

        bFlag = dirT.GetNext(&strSourceName);
    }

    return true;
}