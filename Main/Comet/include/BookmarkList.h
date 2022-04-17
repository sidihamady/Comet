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


#ifndef BOOKMARK_LIST_H
#define BOOKMARK_LIST_H

#include <wx/listctrl.h>

class BookmarkList : public wxListCtrl
{
private:
    DECLARE_EVENT_TABLE()

    enum SORTDIRECTION
    {
        NONE,
        ASC,
        DESC
    };
    int m_iSorted;

public:
    BookmarkList(wxWindow *pParent, const wxSize &sizeT, int iW0, int iW1, int iW2);
    wxString OnGetItemText(long iItem, long iColumn) const;

    void updateList(void);
    void gotoItem(long iSel);
    void OnHeaderClick(wxListEvent &tEvent);
    void OnDoubleClick(wxListEvent &tEvent);
    void OnRightClick(wxListEvent &tEvent);
    void OnGoto(wxCommandEvent &tEvent);
    void OnDelete(wxCommandEvent &tEvent);
};

#endif
