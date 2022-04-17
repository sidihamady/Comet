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
#include "BookmarkList.h"

BEGIN_EVENT_TABLE(BookmarkList, wxListCtrl)
    EVT_LIST_COL_CLICK(IDL_BOOKMARK, BookmarkList::OnHeaderClick)
    EVT_LIST_ITEM_ACTIVATED(IDL_BOOKMARK, BookmarkList::OnDoubleClick)
    EVT_LIST_ITEM_RIGHT_CLICK(IDL_BOOKMARK, BookmarkList::OnRightClick)
    EVT_MENU(ID_BOOKMARK_GOTO, BookmarkList::OnGoto)
    EVT_MENU(ID_BOOKMARK_DELETE, BookmarkList::OnDelete)
END_EVENT_TABLE()

BookmarkList::BookmarkList(wxWindow *pParent, const wxSize &sizeT, int iW0, int iW1, int iW2)
    : wxListCtrl(pParent, IDL_BOOKMARK, wxDefaultPosition, sizeT, wxLC_REPORT | wxLC_VIRTUAL | wxNO_BORDER)
{
    wxListItem itemAt;
    itemAt.SetMask(wxLIST_MASK_TEXT);
    itemAt.SetId(0);
    itemAt.SetText(uT("Filename"));
    itemAt.SetWidth(iW0);
    InsertColumn(0, itemAt);

    wxListItem itemBt;
    itemBt.SetMask(wxLIST_MASK_TEXT);
    itemBt.SetId(1);
    itemBt.SetText(uT("Line #"));
    itemBt.SetWidth(iW1);
    InsertColumn(1, itemBt);

    wxListItem itemCt;
    itemCt.SetMask(wxLIST_MASK_TEXT);
    itemCt.SetId(2);
    itemCt.SetText(uT("Text"));
    itemCt.SetWidth(iW2);
    InsertColumn(2, itemCt);

    m_iSorted = BookmarkList::SORTDIRECTION::NONE;
}

void BookmarkList::OnHeaderClick(wxListEvent &tEvent)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    bool bAsc = (m_iSorted != BookmarkList::SORTDIRECTION::ASC);
    if (m_iSorted == BookmarkList::SORTDIRECTION::ASC) {
        m_iSorted = BookmarkList::SORTDIRECTION::DESC;
    }
    else {
        m_iSorted = BookmarkList::SORTDIRECTION::ASC;
    }

    pFrame->sortBookmark(tEvent.GetColumn(), bAsc);
}

wxString BookmarkList::OnGetItemText(long iItem, long iColumn) const
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return wxEmptyString;
    }

    bookmark_t *pBookmark = pFrame->getBookmark(iItem);

    wxString strT = wxEmptyString;

    if (pBookmark) {
        if (iColumn == 0) {
            strT = static_cast<const char_t *>(pBookmark->filename);
            wxFileName fname = strT;
            strT = fname.GetFullName();
        }
        else if (iColumn == 1) {
            strT = wxString::Format(uT("%d"), pBookmark->line + 1);
        }
        else if (iColumn == 2) {
            strT = static_cast<const char_t *>(pBookmark->info);
        }
    }

    return strT;
}

void BookmarkList::gotoItem(long iSel)
{
    if (iSel < 0) {
        iSel = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    }

    if ((iSel < 0) || (iSel >= GetItemCount())) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    bookmark_t *pBookmark = pFrame->getBookmark(iSel);
    if (pBookmark == NULL) {
        return;
    }

    wxString strFilename = static_cast<const char_t *>(pBookmark->filename);
    if (strFilename.IsEmpty() || (::wxFileExists(strFilename) == false)) {
        ScriptEdit *pEdit = pFrame->pageOpen(pBookmark->id, static_cast<long>(pBookmark->line));
        if (NULL == pEdit) {
            pFrame->deleteBookmark(strFilename, -1, 0L);
        }
        return;
    }

    pFrame->fileOpen(strFilename, false, static_cast<long>(pBookmark->line));
}

void BookmarkList::OnDoubleClick(wxListEvent &tEvent)
{
    gotoItem(tEvent.GetIndex());
}

void BookmarkList::OnRightClick(wxListEvent &tEvent)
{
    wxMenu popMenu;
    wxPoint pointT = tEvent.GetPoint();

    wxMenuItem *pItem = new wxMenuItem(&popMenu, ID_BOOKMARK_GOTO, uT("Goto"), uT("Goto bookmark"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(bookmark_next_small_xpm);
#endif
    popMenu.Append(pItem);
    popMenu.AppendSeparator();
    popMenu.Append(ID_BOOKMARK_DELETE, uT("Delete"), uT("Delete bookmark"), wxITEM_NORMAL);

    PopupMenu(&popMenu, pointT.x, pointT.y);
}

void BookmarkList::updateList(void)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    SetItemCount(pFrame->getBookmarkCount());

    RefreshItems(0, pFrame->getBookmarkCount() - 1);
}

void BookmarkList::OnGoto(wxCommandEvent &WXUNUSED(tEvent))
{
    gotoItem(-1);
}

void BookmarkList::OnDelete(wxCommandEvent &WXUNUSED(tEvent))
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    int iSel = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (iSel >= 0) {
        int iT = 0;
        while (true) {
            ++iT;
            if (pFrame->deleteBookmark(iSel) == false) {
                break;
            }
            this->DeleteItem(iSel);
            iSel = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if (iSel < 0) {
                break;
            }
        }
        pFrame->updateBookmark();
    }
}
