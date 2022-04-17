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


wxString CometFrame::getTextToFind(ScriptEdit *pEdit)
{
    wxString strFind = wxEmptyString;
    if (pEdit == NULL) {
        return strFind;
    }

    bool bToolbarFindO = (this->GetToolBar()->IsShown() && m_cboAnalyzer);
    bool bToolbarFind = (bToolbarFindO && m_cboAnalyzer->isFocused() && AnalyzerIsFindSelected());

    if (bToolbarFind) {
        strFind = m_cboAnalyzer->GetValue();
    }
    else {
        strFind = pEdit->GetSelectedText();
    }
    if (strFind.IsEmpty() || (strFind.Find(uT("\n")) != -1)) {
        strFind = wxEmptyString;
    }
    if (strFind == wxEmptyString) {
        strFind = getCurrentFindItem();
    }

    return strFind;
}

void CometFrame::DoFindReplace(bool bFindOnly)
{
    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = 0;

    iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        return;
    }

    pEdit->DoFindReset();

    const int iSelStartx = pEdit->GetSelectionStart();
    const int iSelEndx = pEdit->GetSelectionEnd();
    const int iLineStartx = pEdit->LineFromPosition(iSelStartx);
    const int iLineEndx = pEdit->LineFromPosition(iSelEndx);

    wxString strFind, strReplace;
    if ((m_pFindDirDlg != NULL) && (m_pFindDirDlg->IsShown())) {
        bool bToolbarFindO = (this->GetToolBar()->IsShown() && m_cboAnalyzer);
        bool bToolbarFind = (bToolbarFindO && m_cboAnalyzer->isFocused() && AnalyzerIsFindSelected());
        if ((false == bToolbarFind) && (iSelEndx > (iSelStartx + 1)) && (iLineStartx == iLineEndx)) {
            strFind = pEdit->GetSelectedText();
        }
        else {
            strFind = m_pFindDirDlg->getFind();
            if (strFind.IsEmpty()) {
                strFind = getTextToFind(pEdit);
            }
        }
        strReplace = m_pFindDirDlg->getFind();
        if (strReplace.IsEmpty()) {
            strReplace = getCurrentReplaceItem();
        }
        updateFindDirPos();
        m_pFindDirDlg->Hide();
    }
    else {
        strFind = getTextToFind(pEdit);
        strReplace = getCurrentReplaceItem();
    }

    if (m_pFindFileDlg != NULL) {

        if (m_pFindFileDlg->findOnly() == bFindOnly) {

            // Bring to front
            bool bIconized = m_pFindFileDlg->IsIconized();
            if (bIconized) {
                m_pFindFileDlg->Iconize(false);
            }
            m_pFindFileDlg->SetFocus();
            m_pFindFileDlg->Raise();
            m_pFindFileDlg->Show(true);
            //

            m_pFindFileDlg->updateItems(strFind, strReplace, iLineEndx > iLineStartx);

            return;
        }

        updateFindFilePos();
        // call Destroy directly, since at this point it is not necessary to handle EVT_CLOSE
        m_pFindFileDlg->Destroy();
        m_pFindFileDlg = NULL;
    }

    int iScreenWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    int iScreenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
    wxPoint ptInit = wxDefaultPosition;
    if ((m_SigmaGUIPrefs.iFindFilePosX >= 20) && (m_SigmaGUIPrefs.iFindFilePosX <= (iScreenWidth - 200)) && (m_SigmaGUIPrefs.iFindFilePosY >= 20) && (m_SigmaGUIPrefs.iFindFilePosY <= (iScreenHeight - 200))) {
        ptInit.x = m_SigmaGUIPrefs.iFindFilePosX;
        ptInit.y = m_SigmaGUIPrefs.iFindFilePosY;
    }

    m_pFindFileDlg = new (std::nothrow) FindFileDlg(this, strFind, (bFindOnly == false), ptInit);
    if (m_pFindFileDlg == NULL) {
        SigmaMessageBox(uT("Cannot create the Find/Replace dialog: insufficient memory"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        return;
    }

    m_pFindFileDlg->updateWhere(iLineEndx > iLineStartx);

    m_pFindFileDlg->Show(true);
}

void CometFrame::DoFindPrevNext(int idT)
{
    if ((idT == ID_FIND_PREV) || (idT == ID_FIND_NEXT)) {

        ScriptEdit *pEdit = static_cast<ScriptEdit *>(getActiveEditor());
        if (pEdit == NULL) {
            return;
        }

        wxString strFind = getTextToFind(pEdit);

        wxString strReplace = wxEmptyString;

        int iStyle = 0;
        if (idT == ID_FIND_NEXT) {

            int iFound = pEdit->DoFindReplace(strFind, true, iStyle, wxEmptyString, false, false, NULL, false, NULL, isFindMarkerEnabled());

            if (((iFound == FIND_LIMITREACHED) || (iFound == FIND_NOTFOUND)) && m_SigmaCommonPrefs.bFindCyclic) {
                pEdit->SetSelection(0, 0);
                iFound = pEdit->DoFindReplace(strFind, true, iStyle, wxEmptyString, false, false, NULL, false, NULL, isFindMarkerEnabled());
            }

            if (iFound == FIND_LIMITREACHED) {
                OutputStatusbar(uT("Find reached the end of document."), SIGMAFRAME_TIMER_SHORT);
            }
            else if (iFound == FIND_NOTFOUND) {
                wxString strT = uT("\'");
                strT += strFind;
                strT += uT("\' not found.");
                OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
            }
            else if (iFound >= FIND_ITEMFOUND) {
                int iLine = pEdit->GetCurrentLine();
                wxString strT = uT("\'");
                strT += strFind;
                strT += uT("\'");
                if (iLine >= 0) {
                    int iColumn = pEdit->GetColumn(pEdit->GetSelectionStart());
                    if (iColumn < 0) {
                        iColumn = 0;
                    }
                    strT += wxString::Format(uT(" found in line %d (column %d)."), iLine + 1, iColumn + 1);
                    updateStatusbarPos(iLine, iColumn);
                }
                OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
            }

            if ((iFound >= FIND_ITEMFOUND) && m_cboAnalyzer) {
                wxString strT = m_cboAnalyzer->GetValue();
                if (strT.IsSameAs(strFind) == false) {
                    m_cboAnalyzer->SetValue(strFind);
                }
            }
        }
        else {
            pEdit->DoFindPrev(strFind, true, iStyle, false, NULL, isFindMarkerEnabled());
        }
    }
}

int CometFrame::getFindCount(void)
{
    return m_SigmaCommonPrefs.iFindCount;
}

void CometFrame::resetFind(void)
{
    m_SigmaCommonPrefs.iFindCount = 0;
    m_SigmaCommonPrefs.iReplaceCount = 0;
    m_SigmaCommonPrefs.iDirLastIndex = 0;
    m_SigmaCommonPrefs.iDirCount = 0;
    m_SigmaCommonPrefs.szFindF[0] = uT('\0');
    m_SigmaCommonPrefs.szReplaceF[0] = uT('\0');
}

wxString CometFrame::getFindItem(int iItem)
{
    wxString strT = wxEmptyString;
    if ((iItem >= 0) && (iItem < m_SigmaCommonPrefs.iFindCount)) {
        strT = static_cast<const char_t *>(m_SigmaCommonPrefs.szFind[iItem]);
    }
    return strT;
}

wxString CometFrame::getCurrentFindItem(void)
{
    wxString strT = static_cast<const char_t *>(m_SigmaCommonPrefs.szFindF);
    return strT;
}

wxString CometFrame::getCurrentReplaceItem(void)
{
    wxString strT = static_cast<const char_t *>(m_SigmaCommonPrefs.szReplaceF);
    return strT;
}

wxString CometFrame::getCurrentDirItem(void)
{
    wxString strT = static_cast<const char_t *>(m_SigmaCommonPrefs.szDirF);
    if ((m_pExplorerTree != NULL) && (strT.IsEmpty() || (::wxDirExists(strT) == false))) {
        strT = m_pExplorerTree->doGetDir();
    }
    return strT;
}

int CometFrame::addFindItem(const wxString &strFind)
{
    if (strFind.IsEmpty()) {
        return -1;
    }

    if (m_SigmaCommonPrefs.iFindCount > 0) {
        for (int ii = 0; ii < m_SigmaCommonPrefs.iFindCount; ii++) {
            if (strFind.IsSameAs(static_cast<const char_t *>(m_SigmaCommonPrefs.szFind[ii]), true)) {
                Tstrncpy(m_SigmaCommonPrefs.szFindF, static_cast<const char_t *>(m_SigmaCommonPrefs.szFind[ii]), LM_STRSIZE - 1);
                return -1;
            }
        }
    }

    if (m_SigmaCommonPrefs.iFindCount >= FIND_MAXCOUNT) {
        --(m_SigmaCommonPrefs.iFindCount);
    }

    // shift down and copy the new item to the top
    if (m_SigmaCommonPrefs.iFindCount >= 1) {
        for (int ii = m_SigmaCommonPrefs.iFindCount; ii >= 1; ii--) {
            Tstrncpy(m_SigmaCommonPrefs.szFind[ii], static_cast<const char_t *>(m_SigmaCommonPrefs.szFind[ii - 1]), LM_STRSIZE - 1);
        }
    }
    Tstrncpy(m_SigmaCommonPrefs.szFind[0], LM_CSTR(strFind), LM_STRSIZE - 1);
    ++(m_SigmaCommonPrefs.iFindCount);
    Tstrncpy(m_SigmaCommonPrefs.szFindF, LM_CSTR(strFind), LM_STRSIZE - 1);
    DoAnalyzerUpdateList(getActiveEditor(), false);

    return 0;
}

int CometFrame::getReplaceCount(void)
{
    return m_SigmaCommonPrefs.iReplaceCount;
}

wxString CometFrame::getReplaceItem(int iItem)
{
    wxString strT = wxEmptyString;
    if ((iItem >= 0) && (iItem < m_SigmaCommonPrefs.iReplaceCount)) {
        strT = static_cast<const char_t *>(m_SigmaCommonPrefs.szReplace[iItem]);
    }
    return strT;
}

int CometFrame::addReplaceItem(const wxString &strReplace)
{
    if (strReplace.IsEmpty()) {
        return -1;
    }

    if (m_SigmaCommonPrefs.iReplaceCount > 0) {
        for (int ii = 0; ii < m_SigmaCommonPrefs.iReplaceCount; ii++) {
            if (strReplace.IsSameAs(static_cast<const char_t *>(m_SigmaCommonPrefs.szReplace[ii]))) {
                Tstrncpy(m_SigmaCommonPrefs.szReplaceF, static_cast<const char_t *>(m_SigmaCommonPrefs.szReplace[ii]), LM_STRSIZE - 1);
                return -1;
            }
        }
    }

    if (m_SigmaCommonPrefs.iReplaceCount >= FIND_MAXCOUNT) {
        --(m_SigmaCommonPrefs.iReplaceCount);
    }

    // shift down and copy the new item to the top
    if (m_SigmaCommonPrefs.iReplaceCount >= 1) {
        for (int ii = m_SigmaCommonPrefs.iReplaceCount; ii >= 1; ii--) {
            Tstrncpy(m_SigmaCommonPrefs.szReplace[ii], static_cast<const char_t *>(m_SigmaCommonPrefs.szReplace[ii - 1]), LM_STRSIZE - 1);
        }
    }
    Tstrncpy(m_SigmaCommonPrefs.szReplace[0], LM_CSTR(strReplace), LM_STRSIZE - 1);
    ++(m_SigmaCommonPrefs.iReplaceCount);
    Tstrncpy(m_SigmaCommonPrefs.szReplaceF, LM_CSTR(strReplace), LM_STRSIZE - 1);

    return 0;
}

int CometFrame::getDirCount(void)
{
    return m_SigmaCommonPrefs.iDirCount;
}

wxString CometFrame::getDirItem(int iItem)
{
    wxString strT = wxEmptyString;
    if ((iItem >= 0) && (iItem < m_SigmaCommonPrefs.iDirCount)) {
        strT = static_cast<const char_t *>(m_SigmaCommonPrefs.szDir[iItem]);
    }
    return strT;
}

int CometFrame::addDirItem(const wxString &strDir)
{
    if (strDir.IsEmpty()) {
        return -1;
    }

    if (m_SigmaCommonPrefs.iDirCount > 0) {
        for (int ii = 0; ii < m_SigmaCommonPrefs.iDirCount; ii++) {
            if (strDir.IsSameAs(static_cast<const char_t *>(m_SigmaCommonPrefs.szDir[ii]))) {
                Tstrncpy(m_SigmaCommonPrefs.szDirF, static_cast<const char_t *>(m_SigmaCommonPrefs.szDir[ii]), LM_STRSIZE - 1);
                return -1;
            }
        }
    }

    if (m_SigmaCommonPrefs.iDirCount < FIND_MAXCOUNT) {
        Tstrncpy(m_SigmaCommonPrefs.szDir[m_SigmaCommonPrefs.iDirCount], LM_CSTR(strDir), LM_STRSIZE - 1);
        m_SigmaCommonPrefs.iDirLastIndex = m_SigmaCommonPrefs.iDirCount;
        m_SigmaCommonPrefs.iDirCount += 1;
        Tstrncpy(m_SigmaCommonPrefs.szDirF, LM_CSTR(strDir), LM_STRSIZE - 1);
        return (m_SigmaCommonPrefs.iDirCount - 1);
    }

    if (m_SigmaCommonPrefs.iDirLastIndex >= (FIND_MAXCOUNT - 1)) {
        m_SigmaCommonPrefs.iDirLastIndex = 0;
    }

    Tstrncpy(m_SigmaCommonPrefs.szDir[m_SigmaCommonPrefs.iDirLastIndex], LM_CSTR(strDir), LM_STRSIZE - 1);
    m_SigmaCommonPrefs.iDirLastIndex += 1;

    Tstrncpy(m_SigmaCommonPrefs.szDirF, LM_CSTR(strDir), LM_STRSIZE - 1);

    return (m_SigmaCommonPrefs.iDirLastIndex - 1);
}
