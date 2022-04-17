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
#include "ScriptEdit.h"


void ScriptEdit::DoAddStatus(int iLine, int iStatusBit, bool bFocus /* = true*/)
{
    if ((iLine < 0) || (iLine >= GetLineCount())) {
        return;
    }

    const int iMask = MarkerGet(iLine);

    // Delete debug markers
    if ((iMask & SCRIPT_MASK_DEBUG) != 0) {
        MarkerDelete(iLine, SCRIPT_MASK_DEBUGBIT);
    }
    if (m_iStatusLine != iLine) {
        if (m_iStatusLine >= 0) {
            if ((MarkerGet(m_iStatusLine) & SCRIPT_MASK_DEBUG) != 0) {
                MarkerDelete(m_iStatusLine, SCRIPT_MASK_DEBUGBIT);
            }
        }
        m_iStatusLine = iLine;
    }
    //

    if (SCRIPT_MASK_ERRORBIT == iStatusBit) {
        if ((iMask & SCRIPT_MASK_ERROR) == 0) {
            m_iErrLine = iLine;
            DoGotoLine(iLine, bFocus);
            MarkerAdd(iLine, iStatusBit);
        }
    }

    else if (SCRIPT_MASK_FINDBIT == iStatusBit) {
        if ((iMask & SCRIPT_MASK_FIND) == 0) {
            MarkerAdd(iLine, iStatusBit);
            m_iFindMarkerCount += 1;
        }
    }

    else if (SCRIPT_MASK_MODIFIEDBIT == iStatusBit) {
        if ((iMask & SCRIPT_MASK_MODIFIED) == 0) {
            if ((iMask & SCRIPT_MASK_SAVED) != 0) {
                MarkerDelete(iLine, SCRIPT_MASK_SAVEDBIT);
            }
            MarkerAdd(iLine, iStatusBit);
        }
    }

    else if (SCRIPT_MASK_SAVEDBIT == iStatusBit) {
        if ((iMask & SCRIPT_MASK_SAVED) == 0) {
            if ((iMask & SCRIPT_MASK_MODIFIED) != 0) {
                MarkerDelete(iLine, SCRIPT_MASK_MODIFIEDBIT);
                MarkerAdd(iLine, iStatusBit);
            }
        }
    }

    else if (SCRIPT_MASK_DEBUGBIT == iStatusBit) {
        if ((iMask & SCRIPT_MASK_DEBUG) == 0) {
            if ((iMask & SCRIPT_MASK_MODIFIED) != 0) {
                MarkerDelete(iLine, SCRIPT_MASK_MODIFIEDBIT);
            }
            DoGotoLine(iLine, bFocus);
            MarkerAdd(iLine, iStatusBit);
        }
    }
}

void ScriptEdit::setFindMarker(int iLine, const wxString &strFind, const wxString &strReplace /* = wxEmptyString*/, bool bFocus /* = true*/)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    if ((iLine < 0) || (iLine >= GetLineCount())) {
        return;
    }

    DoGotoLine(iLine, bFocus);

    int iLen = (int)(strFind.Length());
    if (iLen < 1) {
        return;
    }

    wxString strLine = GetLine(iLine);
    int iLineLen = (int)(strLine.Length());
    int iLineStart = (GetLineEndPosition(iLine) - iLineLen);

    if (iLineLen < 1) {
        return;
    }

    int findMinPos = 0, findMaxPos = 0;
    int iF = FindText(iLineStart, iLineStart + iLineLen, strFind, 0, &findMinPos, &findMaxPos);
    if ((iF >= findMinPos) && (findMaxPos > findMinPos)) {
        DoAddStatus(iLine, SCRIPT_MASK_FINDBIT);
        DoFindHighlight(findMinPos, findMaxPos);
        setFindin(true);
        pFrame->updateEditorStatus(this);
    }
    else if (strReplace.IsEmpty() == false) {
        iLen = (int)(strReplace.Length());
        if (iLen < 1) {
            return;
        }

        iF = FindText(iLineStart, iLineStart + iLineLen, strReplace, 0, &findMinPos, &findMaxPos);
        if ((iF >= findMinPos) && (findMaxPos > findMinPos)) {
            DoAddStatus(iLine, SCRIPT_MASK_FINDBIT);
            DoFindHighlight(findMinPos, findMaxPos);
            setFindin(true);
            pFrame->updateEditorStatus(this);
        }
    }
}


void ScriptEdit::DoShowMarker(bool bEnable)
{
    m_ScintillaPrefs.common.markerEnable = bEnable;

    if (m_ScintillaPrefs.common.markerEnable == false) {
        SetMarginWidth(m_MarkerID, 0);
        return;
    }

    SetMarginWidth(m_MarkerID, m_MarkerMargin);
}

void ScriptEdit::DoShowStatus(bool bEnable)
{
    m_ScintillaPrefs.common.statusEnable = bEnable;

    if (m_ScintillaPrefs.common.statusEnable == false) {
        MarkerDeleteAll(SCRIPT_MASK_SAVEDBIT);
        SetMarginWidth(m_StatusID, 0);
        return;
    }

    SetMarginWidth(m_StatusID, m_StatusMargin);
}


void ScriptEdit::DoDeleteAllBookmarks(void)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    MarkerDeleteAll(SCRIPT_MASK_BOOKMARKBIT);

    pFrame->deleteBookmark(this->GetFilename(), -1, m_Id);

    m_iBookmarkCount = 0;
}

void ScriptEdit::DoDeleteAllMarkers(void)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    for (int ii = SCRIPT_MASK_FIRSTBIT; ii <= SCRIPT_MASK_LASTBIT; ii++) {
        MarkerDeleteAll(ii);
    }

    m_iBookmarkCount = 0;
    m_iBreakpointCount = 0;

    pFrame->deleteBookmark(this->GetFilename(), -1, m_Id);
}

void ScriptEdit::DoFindNextBookmark(void)
{
    int nPos = -1;

    nPos = GetCurrentPos();
    int nLine = LineFromPosition(nPos);
    int nFoundLine = MarkerNext(nLine + 1, SCRIPT_MASK_BOOKMARK);
    if (nFoundLine >= 0) {
        DoGotoLine(nFoundLine);
    }
}

void ScriptEdit::DoFindPrevBookmark(void)
{
    int nPos = -1;

    nPos = GetCurrentPos();
    int nLine = LineFromPosition(nPos);
    int nFoundLine = MarkerPrevious(nLine - 1, SCRIPT_MASK_BOOKMARK);
    if (nFoundLine >= 0) {
        DoGotoLine(nFoundLine);
    }
}

void ScriptEdit::DoModifyMarker(int iMarkerBit, int nPos /* = -1*/)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    if ((iMarkerBit == SCRIPT_MASK_BREAKPOINTBIT) && (this->GetLexer() != wxSTC_LEX_LUA)) {
        return;
    }

    int iLineCount = GetLineCount();
    if (iLineCount < 1) {
        return;
    }

    int iLine = -1;

    if (nPos < 0) {
        nPos = GetCurrentPos();
    }

    iLine = LineFromPosition(nPos);
    if ((iLine < 0) || (iLine >= iLineCount)) {
        return;
    }

    int iMarkerMask = (iMarkerBit == SCRIPT_MASK_BREAKPOINTBIT) ? SCRIPT_MASK_BREAKPOINT : SCRIPT_MASK_BOOKMARK;
    int *piMarkerCount = (iMarkerBit == SCRIPT_MASK_BREAKPOINTBIT) ? &m_iBreakpointCount : &m_iBookmarkCount;

    if (MarkerGet(iLine) & iMarkerMask) {
        MarkerDelete(iLine, iMarkerBit);

        if (iMarkerBit == SCRIPT_MASK_BOOKMARKBIT) {
            pFrame->deleteBookmark(GetFilename(), iLine, m_Id);
        }

        *piMarkerCount -= 1;
        if (*piMarkerCount < 0) {
            *piMarkerCount = 0;
        }
    }
    else {
        MarkerAdd(iLine, iMarkerBit);

        if (iMarkerBit == SCRIPT_MASK_BOOKMARKBIT) {
            wxString strLine = this->GetLine(iLine).Trim(false);
            pFrame->addBookmark(GetFilename(), iLine, strLine, m_Id);
        }

        *piMarkerCount += 1;
    }
}

// DoDeleteBookmark only called by CometFrame::deleteBookmark
void ScriptEdit::DoDeleteBookmark(int iLine)
{
    int iLineCount = GetLineCount();
    if (iLineCount < 1) {
        return;
    }
    if ((iLine < 0) || (iLine >= iLineCount)) {
        return;
    }

    if (MarkerGet(iLine) & SCRIPT_MASK_BOOKMARK) {
        MarkerDelete(iLine, SCRIPT_MASK_BOOKMARKBIT);
        m_iBookmarkCount -= 1;
        if (m_iBookmarkCount < 0) {
            m_iBookmarkCount = 0;
        }
    }
}

// DoDeleteBookmarkAll only called by CometFrame::deleteBookmarkAll
void ScriptEdit::DoDeleteBookmarkAll(void)
{
    int iLineCount = GetLineCount();
    if (iLineCount < 1) {
        return;
    }

    MarkerDeleteAll(SCRIPT_MASK_BOOKMARKBIT);
    m_iBookmarkCount = 0;
}
