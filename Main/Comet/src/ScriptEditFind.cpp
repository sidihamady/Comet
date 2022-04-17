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

bool ScriptEdit::canSearch(wxString &strFind, wxString strReplace, bool bReplace, int *iFindLen, int *iLineCount, int *iCurLine)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return false;
    }

    *iFindLen = (int)(strFind.Length());
    if ((*iFindLen < 1) || (*iFindLen > LM_STRSIZE)) {
        return false;
    }
    if (*iFindLen > LM_STRSIZEN) {
        if (bReplace) {
            return false;
        }
        strFind = strFind.Mid(0, LM_STRSIZEN);
        *iFindLen = (int)(strFind.Length());
    }

    *iLineCount = GetLineCount();
    if ((*iLineCount < 1) || (*iLineCount > LF_SCRIPT_MAXLINES)) {
        return false;
    }
    *iCurLine = GetCurrentLine();
    if ((*iCurLine < 0) || (*iCurLine >= *iLineCount)) {
        return false;
    }

    return true;
}

void ScriptEdit::DoFindReset(void)
{
    if (m_ScintillaPrefs.common.syntaxEnable == false) {
        return;
    }

    StartStyling(0, wxSTC_INDIC2_MASK);
    SetStyling(this->GetTextLength(), 0);
    m_iFindIndicCount = 0;
}
void ScriptEdit::DoFindHighlight(int iStartPos, int iEndPos)
{
    if ((m_ScintillaPrefs.common.syntaxEnable == false) || (iStartPos >= iEndPos)) {
        return;
    }

    if ((this->GetStyleAt(iStartPos) & wxSTC_INDIC2_MASK) == 0) {
        StartStyling(iStartPos, wxSTC_INDIC2_MASK);
        SetStyling(iEndPos - iStartPos, wxSTC_INDIC2_MASK);
        m_iFindIndicCount += 1;
    }
    SetSelection(iStartPos, iEndPos);
}

int ScriptEdit::DoFindPrev(wxString &strFind, bool bVerbose /* = false*/, int iStyle /* = 0*/, bool bSel /* = false*/, int *iFindEnd /* = NULL*/, bool bAddMarker /* = true*/)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return FIND_PARAMERR;
    }

    if (strFind.IsSameAs(pFrame->getCurrentFindItem()) == false) {
        this->DoFindReset();
    }

    int iFindLen, iLineCount, iCurLine;

    this->setFindin(false);

    if (canSearch(strFind, wxEmptyString, false, &iFindLen, &iLineCount, &iCurLine) == false) {
        pFrame->updateEditorStatus(this);
        return FIND_PARAMERR;
    }

    wxString strline;
    int iLineLen = 0, iPos = -1;
    int iLineStart = 0, iLineEnd = 0;
    int iLine = iCurLine;

    int iFindStart = 0;

    int findMinPos = 0, findMaxPos = 0;

    bool bTextSelected = (GetSelectedText().Length() > 0);
    bool bFindSelected = false;
    if (bTextSelected) {
        if (iStyle & wxSTC_FIND_REGEXP) {
            bFindSelected = true;
        }
        else {
            if (iStyle & wxSTC_FIND_MATCHCASE) {
                bFindSelected = (GetSelectedText().Cmp(strFind) == 0);
            }
            else {
                bFindSelected = (GetSelectedText().CmpNoCase(strFind) == 0);
            }
        }
    }

    if (bSel) {
        iFindStart = GetSelectionStart();
        if (iFindEnd) {
            if (*iFindEnd <= (iFindStart + iFindLen)) {
                if (bVerbose) {
                    pFrame->OutputStatusbar(uT("Find reached the start of selection"), SIGMAFRAME_TIMER_SHORT);
                }
                pFrame->updateEditorStatus(this);
                return FIND_LIMITREACHED;
            }
            if (*iFindEnd > GetSelectionEnd()) {
                pFrame->updateEditorStatus(this);
                return FIND_PARAMERR;
            }

            iPos = FindText(*iFindEnd, iFindStart, strFind, iStyle, &findMinPos, &findMaxPos);
            if ((iPos >= iFindStart) && (iPos <= *iFindEnd)) {
                if (bAddMarker) {
                    DoAddStatus(LineFromPosition(iPos), SCRIPT_MASK_FINDBIT);
                }
                *iFindEnd = iPos;
                this->setFindin(true);
                pFrame->updateEditorStatus(this);
                pFrame->addFindItem(strFind);
                return 0;
            }
        }

        if (bVerbose) {
            pFrame->OutputStatusbar(uT("Find reached the start of selection"), SIGMAFRAME_TIMER_SHORT);
        }
        pFrame->updateEditorStatus(this);
        return FIND_LIMITREACHED;
    }

    strline = GetLine(iLine);
    iLineLen = (int)(strline.Length());

    if (iLineLen >= iFindLen) {
        iLineStart = GetLineEndPosition(iLine) - iLineLen;
        if (iFindEnd) {
            iLineEnd = *iFindEnd;
        }
        if (iLineEnd < GetSelectionEnd()) {
            iLineEnd -= 1;
        }
        iPos = FindText(iLineEnd, iLineStart, strFind, iStyle, &findMinPos, &findMaxPos);
        if ((iPos >= 0) && (iPos >= iLineStart) && (iPos <= iLineEnd) && (findMinPos < findMaxPos)) {
            if (bAddMarker) {
                DoAddStatus(iLine, SCRIPT_MASK_FINDBIT);
            }
            this->setFindin(true);
            pFrame->updateEditorStatus(this);
            DoFindHighlight(findMinPos, findMaxPos);
            pFrame->addFindItem(strFind);
            return FIND_ITEMFOUND;
        }
    }

    if (iCurLine <= 0) {
        pFrame->updateEditorStatus(this);
        if (bFindSelected) {
            if (bVerbose) {
                pFrame->OutputStatusbar(uT("Find reached the start of document"), SIGMAFRAME_TIMER_SHORT);
            }
            return FIND_LIMITREACHED;
        }
        else {
            if (bVerbose) {
                wxString strT = uT("\'");
                strT += strFind;
                strT += uT("\'");
                strT += uT(" not found in this document");
                pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
            }
            return FIND_NOTFOUND;
        }
    }

    bool bFound = false;

    for (iLine = iCurLine - 1; iLine >= 0; iLine--) {
        strline = GetLine(iLine);
        iLineLen = (int)(strline.Length());
        if (iLineLen < iFindLen) {
            continue;
        }

        iLineEnd = GetLineEndPosition(iLine);
        iLineStart = iLineEnd - iLineLen;
        findMinPos = 0;
        findMaxPos = 0;

        iPos = FindText(iLineEnd, iLineStart, strFind, iStyle, &findMinPos, &findMaxPos);
        if ((iPos < 0) || (iPos < iLineStart) || (iPos > iLineEnd) || (findMinPos >= findMaxPos)) {
            continue;
        }

        if (bAddMarker) {
            DoAddStatus(iLine, SCRIPT_MASK_FINDBIT);
        }
        DoFindHighlight(findMinPos, findMaxPos);
        bFound = true;
        break;
    }

    if (bFound == false) {

        // Text to find already selected
        if (bFindSelected) {
            this->setFindin(true);
        }

        if (bFindSelected) {
            if (bVerbose) {
                pFrame->OutputStatusbar(uT("Find reached the start of document"), SIGMAFRAME_TIMER_SHORT);
            }
            pFrame->updateEditorStatus(this);
            return FIND_LIMITREACHED;
        }

        if (bVerbose) {
            wxString strT = uT("\'");
            strT += strFind;
            strT += uT("\'");
            strT += uT(" not found in this document");
            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        }
        pFrame->updateEditorStatus(this);
        return FIND_NOTFOUND;
    }
    else {
        this->setFindin(true);
        CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
        if (pFrame) {
            pFrame->addFindItem(strFind);
        }
        DoFindHighlight(findMinPos, findMaxPos);
    }

    pFrame->updateEditorStatus(this);
    return FIND_ITEMFOUND;
}

int ScriptEdit::DoFindReplace(wxString &strFind, bool bVerbose /* = false*/, int iStyle /* = 0*/,
                              wxString strReplace /* = wxEmptyString*/, bool bReplace /* = false*/, bool bAll /* = false*/,
                              int *iCount /* = NULL*/, bool bSel /* = false*/, int *iFindStart /* = NULL*/, bool bAddMarker /* = true*/)
{
    m_iLinePrev = -1;
    m_bLinePrev = false;

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return FIND_PARAMERR;
    }

    SigmaBusyCursor waitC;
    if (bAll) {
        waitC.start();
    }

    if (bAll) {
        MarkerDeleteAll(SCRIPT_MASK_FINDBIT);
    }

    if (bAll || (strFind.IsSameAs(pFrame->getCurrentFindItem()) == false)) {
        this->DoFindReset();
    }

    this->setFindin(false);

    int iFindLen;
    int iFindStartLine, iFindEndLine, iCurLine;
    int iFindStartPos, iFindEndPos;

    if (bReplace) {
        if (strReplace.IsSameAs(strFind)) {
            return FIND_PARAMERR;
        }
    }

    if (canSearch(strFind, strReplace, bReplace, &iFindLen, &iFindEndLine, &iCurLine) == false) {
        pFrame->updateEditorStatus(this);
        return FIND_PARAMERR;
    }
    iFindEndLine -= 1;

    const int iSelStartx = GetSelectionStart();
    const int iSelEndx = GetSelectionEnd();
    const int iFirstLinex = GetFirstVisibleLine();

    const int iMoveFwd = ((iSelEndx - iSelStartx) > 0) ? (iSelEndx - iSelStartx) : 1;

    wxString strline;
    int iLineLen = 0, iPos = -1;
    int iReplaceLen = bReplace ? (int)(strReplace.Length()) : 0;

    BeginUndoAction();

    bool bTextSelected = (GetSelectedText().Length() > 0);
    bool bFindSelected = false;
    if (bTextSelected) {
        if (iStyle & wxSTC_FIND_REGEXP) {
            bFindSelected = true;
        }
        else {
            if (iStyle & wxSTC_FIND_MATCHCASE) {
                bFindSelected = (GetSelectedText().Cmp(strFind) == 0);
            }
            else {
                bFindSelected = (GetSelectedText().CmpNoCase(strFind) == 0);
            }
        }
    }

    int iSelDelta = 0;

    bool bReplaceSelected = false;
    if ((bAll == false) && (GetSelectedText().Length() > 0)) {
        if (iStyle & wxSTC_FIND_REGEXP) {
            bReplaceSelected = true;
        }
        else {
            if (iStyle & wxSTC_FIND_MATCHCASE) {
                bReplaceSelected = (GetSelectedText().Cmp(strReplace) == 0);
            }
            else {
                bReplaceSelected = (GetSelectedText().CmpNoCase(strReplace) == 0);
            }
        }
    }

    // Search in the whole document ...
    if (bAll && (bSel == false)) {
        if (iCount) {
            *iCount = 0;
        }
        SetSelection(0, 0);
        iFindStartLine = 0;
        iFindStartPos = 0;
        iFindEndPos = GetLineEndPosition(iFindEndLine);
        bFindSelected = false;
        bReplaceSelected = false;
    }

    // ... or in the selected text ...
    else if (bSel) {
        iFindStartLine = LineFromPosition(iSelStartx);
        iFindEndLine = LineFromPosition(iSelEndx);
        iFindStartPos = iFindStart ? *iFindStart : iSelStartx;
        iFindEndPos = iSelEndx;
    }

    // ... or from the saved current position
    else {
        iFindStartLine = iCurLine;
        iFindStartPos = iFindStart ? *iFindStart : iSelStartx;
        iFindEndPos = GetLineEndPosition(iFindEndLine);
        if (bFindSelected || bReplaceSelected) {
            iFindStartPos += iMoveFwd;
            if (iFindStart) {
                *iFindStart = iFindStartPos;
            }
        }
    }

    // Text to find already selected
    if (bFindSelected) {
        if (bAddMarker) {
            DoAddStatus(iCurLine, SCRIPT_MASK_FINDBIT);
        }
        if (bReplace) {
            iPos = GetSelectionStart();
            ReplaceSelection(strReplace);
            iSelDelta += (iReplaceLen - iFindLen);
            pFrame->addFindItem(strFind);
            // Replace the selected found text, and continue searching
            if (iReplaceLen > iMoveFwd) {
                // Mandatory to move forward
                iFindStartPos += (iReplaceLen - iMoveFwd);
                if (iFindStart) {
                    *iFindStart = iFindStartPos;
                }
            }
        }
        else {
            DoFindHighlight(iSelStartx, iSelEndx);
        }
        this->setFindin(true);
    }

    int findMinPos = 0, findMaxPos = 0;
    int findDelta = iReplaceLen - iFindLen;

    if (bSel) {
        if (iFindStartPos < iSelStartx) {
            pFrame->updateEditorStatus(this);
            m_iLinePrev = -1;
            EndUndoAction();
            return 0;
        }
        if ((iFindEndPos - iFindStartPos) < iFindLen) {
            if (bVerbose) {
                pFrame->OutputStatusbar(uT("Find reached the end of selection"), SIGMAFRAME_TIMER_SHORT);
            }
            pFrame->updateEditorStatus(this);
            m_iLinePrev = -1;
            EndUndoAction();
            return FIND_LIMITREACHED;
        }
    }

    if ((bAll == false) && (bSel == false)) {
        iPos = FindText(iFindStartPos, iFindEndPos, strFind, iStyle, &findMinPos, &findMaxPos);
        if ((iPos >= 0) && (iPos >= iFindStartPos) && (findMinPos < findMaxPos)) {
            if (bAddMarker) {
                DoAddStatus(LineFromPosition(iPos), SCRIPT_MASK_FINDBIT);
            }
            pFrame->addFindItem(strFind);
            this->setFindin(true);
            pFrame->updateEditorStatus(this);
            DoFindHighlight(findMinPos, findMaxPos);
            m_iLinePrev = -1;
            EndUndoAction();
            return (FIND_ITEMFOUND + 1);
        }
        else {

            if (bFindSelected) {
                if (bVerbose) {
                    pFrame->OutputStatusbar(uT("Find reached the end of document"), SIGMAFRAME_TIMER_SHORT);
                }
                pFrame->updateEditorStatus(this);
                m_iLinePrev = -1;

                EndUndoAction();
                return FIND_LIMITREACHED;
            }

            if (bVerbose) {
                wxString strT = uT("\'");
                strT += strFind;
                strT += uT("\'");
                strT += uT(" not found in this document");
                pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
            }
            pFrame->updateEditorStatus(this);
            m_iLinePrev = -1;

            EndUndoAction();
            return FIND_NOTFOUND;
        }
    }

    m_bLinePrev = true;
    int iFound = 0, iFoundByLine = 0, iLineStartPos, iLineEndPos;

    for (int iLine = iFindStartLine; iLine <= iFindEndLine; iLine++) {

        strline = GetLine(iLine);
        iLineLen = (int)(strline.Length());
        if (iLineLen < iFindLen) {
            continue;
        }
        iLineEndPos = GetLineEndPosition(iLine);
        iLineStartPos = iLineEndPos - iLineLen;

        if (bSel) {
            if (iLineStartPos < iFindStartPos) {
                iLineStartPos = iFindStartPos;
            }
            if (iLineEndPos > iFindEndPos) {
                iLineEndPos = iFindEndPos;
            }

            if ((iLineEndPos - iLineStartPos) < iFindLen) {
                continue;
            }
        }

        if (bAll == false) {

            findMinPos = findMaxPos = 0;
            iPos = FindText(iLineStartPos, iLineEndPos, strFind, iStyle, &findMinPos, &findMaxPos);
            if ((iPos < 0) || (iPos < iLineStartPos) || (findMinPos >= findMaxPos)) {
                continue;
            }

            if (bAddMarker) {
                DoAddStatus(iLine, SCRIPT_MASK_FINDBIT);
            }
            if (bReplace) {
                SetSelection(findMinPos, findMaxPos);
                if (this->GetSelectedText().IsSameAs(strFind)) {
                    ReplaceSelection(strReplace);
                }
                findMaxPos = iPos + iReplaceLen;
                iSelDelta += findDelta;
            }
            else {
                DoFindHighlight(findMinPos, findMaxPos);
            }
            if (iFindStart) {
                *iFindStart = findMaxPos;
            }
            iFound += 1;
            this->setFindin(true);
            break;
        }

        else {
            // Find all occurences in the line
            iFoundByLine = 0;
            int iFwd = 1;
            while (true) {
                iPos = FindText(iLineStartPos, iLineEndPos, strFind, iStyle, &findMinPos, &findMaxPos);
                if ((iPos < 0) || (findMinPos >= findMaxPos)) {
                    break;
                }
                if (bSel && ((findMinPos < iSelStartx) || (findMaxPos > iFindEndPos))) {
                    break;
                }

                iFwd = 1;
                if (bReplace) {
                    SetSelection(findMinPos, findMaxPos);
                    if (this->GetSelectedText().IsSameAs(strFind)) {
                        ReplaceSelection(strReplace);
                    }
                    iFwd = iReplaceLen;
                    iSelDelta += findDelta;
                    iLineEndPos += findDelta;
                    iFindEndPos += findDelta;
                }
                else if (iFoundByLine == 0) {
                    if (bAddMarker) {
                        DoAddStatus(iLine, SCRIPT_MASK_FINDBIT);
                    }
                    DoFindHighlight(findMinPos, findMaxPos);
                }
                else {
                    DoFindHighlight(findMinPos, findMaxPos);
                }
                iFoundByLine += 1;
                iLineStartPos = findMinPos + iFwd;
            }
            iFound += iFoundByLine;
            if (iCount) {
                *iCount += iFoundByLine;
            }
            this->setFindin(iFoundByLine > 0);
        }
    }

    if (bSel) {
        // Restore selection
        iFindEndPos = iSelEndx;
        if (bReplace) {
            iFindEndPos = iSelEndx + iSelDelta;
            if (iFindEndPos < iSelStartx) {
                iFindEndPos = iSelStartx;
            }
        }

        SetSelection(iSelStartx, iFindEndPos);
        const int iFirstLinexNew = GetFirstVisibleLine();
        if (iFirstLinex != iFirstLinexNew) {
            this->LineScroll(0, iFirstLinex - iFirstLinexNew);
        }
    }

    if (iFound < 1) {

        if (bFindSelected) {
            if (bVerbose) {
                pFrame->OutputStatusbar(uT("Find reached the end of document"), SIGMAFRAME_TIMER_SHORT);
            }
            pFrame->updateEditorStatus(this);
            m_iLinePrev = -1;
            m_bLinePrev = false;

            if (bSel == false) {
                // restore selected text
                SetSelection(iSelStartx, iSelEndx);
                const int iFirstLinexNew = GetFirstVisibleLine();
                if (iFirstLinex != iFirstLinexNew) {
                    this->LineScroll(0, iFirstLinex - iFirstLinexNew);
                }
            }

            EndUndoAction();
            return FIND_LIMITREACHED;
        }

        if (bVerbose) {
            wxString strT = uT("\'");
            strT += strFind;
            strT += uT("\'");
            strT += uT(" not found in this document");
            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        }
        pFrame->updateEditorStatus(this);
        m_iLinePrev = -1;
        m_bLinePrev = false;

        if (bSel == false) {
            // restore selected text
            SetSelection(iSelStartx, iSelEndx);
            const int iFirstLinexNew = GetFirstVisibleLine();
            if (iFirstLinex != iFirstLinexNew) {
                this->LineScroll(0, iFirstLinex - iFirstLinexNew);
            }
        }

        EndUndoAction();
        return FIND_NOTFOUND;
    }
    else {
        pFrame->addFindItem(strFind);
        if (bReplace && (strReplace.IsEmpty() == false)) {
            pFrame->addReplaceItem(strReplace);
        }
        if (bAll && bVerbose) {
            wxString strT = uT("\'");
            strT += strFind;
            strT += uT("\'");
            strT += wxString::Format((iFound > 1) ? uT(" found %d times in ") : uT(" found %d time in "), iFound);
            strT += GetFilename();
            strT += uT(".");
            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        }
        this->setFindin(true);
    }

    pFrame->updateEditorStatus(this);

    m_iLinePrev = -1;
    m_bLinePrev = false;

    EndUndoAction();
    return (FIND_ITEMFOUND + iFound);
}

