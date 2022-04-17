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


#ifndef FILE_EXPLORER_H
#define FILE_EXPLORER_H

#include "../../LibFile/include/File.h"

#include <wx/dirctrl.h>
#include <wx/file.h>

class CometFileExplorer : public wxGenericDirCtrl
{

private:
    wxString m_strRoot;
    wxString m_strRootLabel;
    wxTreeItemId m_RootId;
    wxTreeItemId m_SourceId;
    wxTreeItemId m_CopyId;
    bool m_bDeleteItem;
    bool m_bNewItem;
    bool m_bCutItem;

public:
    CometFileExplorer(wxWindow *parent, const wxWindowID id = wxID_ANY,
                      const wxString &rdir = wxEmptyString,
                      const wxString &dir = wxDirDialogDefaultFolderStr,
                      const wxPoint &pos = wxDefaultPosition,
                      const wxSize &size = wxDefaultSize,
                      long style = wxNO_BORDER | wxDIRCTRL_EDIT_LABELS | wxDIRCTRL_SHOW_FILTERS,
                      const wxString &filter = wxEmptyString,
                      int defaultFilter = 0,
                      const wxString &name = wxTreeCtrlNameStr) : m_strRoot(rdir), m_RootId(0), m_bDeleteItem(false), m_bNewItem(false), m_bCutItem(false)
    {
        m_strRootLabel = m_strRoot;
        Init();
        Create(parent, id, dir, pos, size, style, filter, defaultFilter, name);

        wxTreeCtrl *pTreeCtrl = GetTreeCtrl();
        pTreeCtrl->Connect(wxEVT_COMMAND_TREE_ITEM_COLLAPSING, wxTreeEventHandler(CometFileExplorer::OnCollapsing), NULL, this);
        pTreeCtrl->Connect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(CometFileExplorer::OnItemRightClick), NULL, this);
        pTreeCtrl->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(CometFileExplorer::OnItemDoubleClick), NULL, this);
        pTreeCtrl->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(CometFileExplorer::OnItemMenu), NULL, this);
        pTreeCtrl->Connect(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(CometFileExplorer::OnItemBeginRename), NULL, this);
        pTreeCtrl->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(CometFileExplorer::OnItemEndRename), NULL, this);
        pTreeCtrl->Connect(wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler(CometFileExplorer::OnItemDelete), NULL, this);
        pTreeCtrl->Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(CometFileExplorer::OnItemDragBegin), NULL, this);
        pTreeCtrl->Connect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(CometFileExplorer::OnItemDragEnd), NULL, this);
        pTreeCtrl->SetScrollPos(wxHORIZONTAL, 0);
    }

    virtual void SetupSections()
    {
        if (m_strRoot.IsEmpty()) {
            wxGenericDirCtrl::SetupSections();
        }
        else {
            m_RootId = AddSection(m_strRoot, m_strRootLabel, wxFileIconsTable::folder_open);
            ExpandPath(m_strRoot);
        }
    }

    wxString doGetCurrentDir(void) const
    {
        wxString filenameT = this->GetPath();
        if (::wxDirExists(filenameT)) {
            const wxString strSep = wxFILE_SEP_PATH;
            if (filenameT.EndsWith(strSep) == false) {
                filenameT += strSep;
            }
            return filenameT;
        }
        else if (::wxFileExists(filenameT)) {
            wxFileName fname = filenameT;
            return fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
        }
        return wxEmptyString;
    }

    wxString doGetDir(void) const
    {
        return m_strRoot;
    }
    void doSetDir(const wxString &strDir);
    void doBrowseDir(void);
    void doRefreshDir(void);
    bool doOpenFile(const wxString &strFilename);
    bool doRenameItem(void);
    void doDeleteItemPrepare(void)
    {
        m_bDeleteItem = true;
    }
    bool doDeleteItem(void);
    bool doCreateItem(bool bDir);
    bool doCutCopyItem(bool bCut);
    bool canPaste(bool bGet, wxString *pstrFilename = NULL);
    bool doCutItem(void);
    bool doCopyItem(void);
    bool doPasteItem(void);
    bool doOpenExplorer(void);
    bool doCopyPath(void);
    bool doChangeDir(void);
    bool doOpenShell(void);
    bool doViewProperties(void);

private:
    bool doCopyFile(const wxString &strSrc, const wxString &strDest, int *iSrcCount, int *iDestCount);
    bool doDeleteFile(const wxString &strDir);
    void OnCollapsing(wxTreeEvent &tEvent)
    {
        if (m_RootId == (wxTreeItemId)(tEvent.GetItem())) {
            tEvent.Veto();
        }
    }
    const wxFileOffset getFileSize(const wxString &strFilename)
    {
        wxFile fileT(strFilename);
        if (fileT.IsOpened()) {
            const wxFileOffset iFileSize = (const wxFileOffset)(fileT.Length());
            if (iFileSize < 0L) { // Should never happen
                fileT.Close();
                return -1L;
            }
            fileT.Close();
            return iFileSize;
        }
        return -1L;
    }
    void OnItemRightClick(wxTreeEvent &tEvent);
    void OnItemDoubleClick(wxTreeEvent &tEvent);
    void OnItemMenu(wxTreeEvent &tEvent);
    void OnItemBeginRename(wxTreeEvent &tEvent);
    void OnItemEndRename(wxTreeEvent &tEvent);
    void OnItemDelete(wxTreeEvent &tEvent);
    void OnItemDragBegin(wxTreeEvent &tEvent);
    void OnItemDragEnd(wxTreeEvent &tEvent);
};

#endif
