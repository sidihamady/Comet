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

#include <wx/clipbrd.h>

void CometFrame::OnUpdateFileSave(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
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

    int idT = tEvent.GetId();
    if (idT == ID_FILE_SAVE) {
        CometFrame::enableUIitem(tEvent, pEdit->isModified() || pEdit->GetFilename().IsEmpty());
    }
    else if (idT == ID_FILE_SAVEALL) {
        bool bEnable = (iPageCount >= 2);
        if (bEnable) {
            if ((pEdit->isModified() == false) && (iPageCount <= 7)) {
                int iT = 0;
                for (int ii = 0; ii < iPageCount; ii++) {
                    pEdit = getEditor(ii);
                    if (pEdit == NULL) {
                        break;
                    }
                    if (pEdit->isModified()) {
                        break;
                    }
                    iT += 1;
                }
                if (iT >= iPageCount) {
                    bEnable = false;
                }
            }
        }
        CometFrame::enableUIitem(tEvent, bEnable);
    }
}

void CometFrame::OnUpdateFileReload(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    int idT = tEvent.GetId();
    if (idT == ID_FILE_RELOAD) {
        wxFileName fname = pEdit->GetFilename();
        CometFrame::enableUIitem(tEvent, fname.FileExists());
    }
}

void CometFrame::OnUpdateRecent(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    const int idT = tEvent.GetId();

    wxMenuBar *pMenubar = this->GetMenuBar();

    if ((pMenubar == NULL) || (idT < ID_FILE_RECENT_FIRST) || (idT > ID_FILE_RECENT_LAST)) {
        m_pRecentFileMenuItem->Enable(false);
        return;
    }

    m_pRecentFileMenuItem->Enable(m_SigmaCommonPrefs.iFileRecentCount > 0);

    if (m_SigmaCommonPrefs.iFileRecentCount < 0) {
        return;
    }

    if ((idT == ID_FILE_REOPEN) || (idT == ID_FILE_RECENTCLEAR)) {
        CometFrame::enableUIitem(tEvent, m_SigmaCommonPrefs.iFileRecentCount > 0);
        return;
    }

    int ii = idT - ID_FILE_RECENT_FIRST;

    CometFrame::enableUIitem(tEvent, m_SigmaCommonPrefs.iFileRecentCount > ii);

    wxString strT = uT("Recent File");
    if (m_SigmaCommonPrefs.iFileRecentCount > ii) {
        strT = CometFrame::getLabelFromPath(m_SigmaCommonPrefs.szFileRecent[ii]);
    }

    wxMenuItem *pItem = pMenubar->FindItem(idT);
    if (pItem != NULL) {
        pItem->SetText(strT);
        pItem->SetHelp(m_SigmaCommonPrefs.szFileRecent[ii]);
#ifdef __WXMSW__
        if (idT == ID_FILE_RECENT_FIRST) {
            rebuildMenuItem(pItem, fileopen_small_xpm, fileopen_disabled_small_xpm);
        }
#endif
    }
}

void CometFrame::OnUpdateRecentDir(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    const int idT = tEvent.GetId();

    wxMenuBar *pMenubar = this->GetMenuBar();

    if ((pMenubar == NULL) || (idT < ID_DIR_RECENT_FIRST) || (idT > ID_DIR_RECENT_LAST)) {
        m_pRecentDirMenuItem->Enable(false);
        return;
    }

    m_pRecentDirMenuItem->Enable(m_SigmaCommonPrefs.iDirRecentCount > 0);

    if (m_SigmaCommonPrefs.iDirRecentCount < 0) {
        return;
    }

    if (idT == ID_DIR_RECENTCLEAR) {
        CometFrame::enableUIitem(tEvent, m_SigmaCommonPrefs.iDirRecentCount > 0);
        return;
    }

    int ii = idT - ID_DIR_RECENT_FIRST;

    CometFrame::enableUIitem(tEvent, m_SigmaCommonPrefs.iDirRecentCount > ii);

    wxString strT, strTT;
    wxFileName fname;

    strT = uT("Recent Directory");
    if (m_SigmaCommonPrefs.iDirRecentCount > ii) {
        strT = CometFrame::getLabelFromPath(m_SigmaCommonPrefs.szDirRecent[ii]);
    }

    wxMenuItem *pItem = pMenubar->FindItem(idT);
    if (pItem != NULL) {
        pItem->SetText(strT);
        pItem->SetHelp(m_SigmaCommonPrefs.szDirRecent[ii]);
#ifdef __WXMSW__
        if (idT == ID_DIR_RECENT_FIRST) {
            rebuildMenuItem(pItem, folder_small_xpm, folder_disabled_small_xpm);
        }
#endif
    }
}

void CometFrame::OnUpdateCodeFormat(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    bool bEnable = pEdit->isLexerKnown(pEdit->GetLexer()) && (pEdit->GetReadOnly() == false);

    CometFrame::enableUIitem(tEvent, bEnable);
}

void CometFrame::OnUpdateCodeSnippet(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    ScriptEdit *pEdit = getActiveEditor();
    if (pEdit == NULL) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    bool bEnable = pEdit->isLexerKnown(pEdit->GetLexer()) && (pEdit->GetReadOnly() == false) && getSnippet();

    CometFrame::enableUIitem(tEvent, bEnable);
}

void CometFrame::OnUpdateTopmost(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    if (tEvent.GetId() == ID_VIEW_TOPMOST) {
        bool bEnable = ((this->GetWindowStyle() & wxSTAY_ON_TOP) != 0);
        tEvent.Check(bEnable);
        updateTitle(wxEmptyString);
    }
}

void CometFrame::OnViewResetUI(wxCommandEvent &tEvent)
{
    if (tEvent.GetId() == ID_VIEW_RESETUI) {
        int iret = SigmaMessageBox(uT("Reset the user interface perspective?"), uT("Comet"), wxYES_NO | wxICON_QUESTION | wxNO_DEFAULT, this, uT("&Reset"), uT("&Cancel"));
        if (iret != wxYES) {
            return;
        }
        Tunlink(m_szLayoutConfigFilename);
        m_bSavePerspective = false;
        SigmaMessageBox(uT("UI perspective reset will take effect on next restart"), uT("Comet"), wxOK | wxICON_INFORMATION, this);
    }
}

void CometFrame::OnUpdateFileList(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    CometFrame::enableUIitem(tEvent, iPageCount >= 2);
    // :WARNING: Do not update menu item here to avoid flickering
}

void CometFrame::OnUpdateLexer(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
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

    bool bLexerKnown = pEdit->isExtKnown();
    tEvent.Check(CodeEdit::getLexerByIndex(tEvent.GetId() - ID_FILE_LEXER01) == pEdit->GetLexer());
    CometFrame::enableUIitem(tEvent, false == bLexerKnown);
}

void CometFrame::OnUpdateUndoRedo(wxUpdateUIEvent &tEvent)
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

    if (pEdit->isFocused()) {
        if (idT == ID_MAIN_UNDO) {
            CometFrame::enableUIitem(tEvent, pEdit->CanUndo());
        }
        else if (idT == ID_MAIN_REDO) {
            CometFrame::enableUIitem(tEvent, pEdit->CanRedo());
        }
    }

    else if (m_pEditConsole) {
        if (m_pEditConsole->isFocused()) {
            if (idT == ID_MAIN_UNDO) {
                CometFrame::enableUIitem(tEvent, m_pEditConsole->CanUndo());
            }
            else if (idT == ID_MAIN_REDO) {
                CometFrame::enableUIitem(tEvent, m_pEditConsole->CanRedo());
            }
        }
    }

    else if (m_pEditOutput) {
        if (m_pEditOutput->isFocused()) {
            if (idT == ID_MAIN_UNDO) {
                CometFrame::enableUIitem(tEvent, false);
            }
            else if (idT == ID_MAIN_REDO) {
                CometFrame::enableUIitem(tEvent, false);
            }
        }
    }
}

void CometFrame::OnUpdateCopyPaste(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    int idT = tEvent.GetId();

    if ((idT >= wxID_CUT) && (idT <= wxID_SELECTALL) && m_cboAnalyzer && m_cboAnalyzer->isFocused() && m_cboAnalyzer->IsEditable()) {
        CometFrame::enableUIitem(tEvent, true);
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    ScriptEdit *pEdit = this->getActiveEditor();
    if (pEdit == NULL) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    int iStart = 0, iEnd = 0;

    if (pEdit->isFocused() == false) {
        // Output Window (Read-only)
        if (m_pEditOutput && (idT == wxID_COPY)) {
            if (m_pEditOutput->isFocused()) {
                m_pEditOutput->GetSelection(&iStart, &iEnd);
                CometFrame::enableUIitem(tEvent, iEnd > iStart);
                return;
            }
        }
        //
    }

    if ((idT == wxID_CUT) || (idT == wxID_COPY) || (idT == wxID_CLEAR)) {
        if (pEdit->isFocused()) {
            if (pEdit->GetReadOnly() && (idT != wxID_COPY)) {
                CometFrame::enableUIitem(tEvent, false);
                return;
            }
            pEdit->GetSelection(&iStart, &iEnd);
        }
        else if (m_pEditConsole) {
            if (m_pEditConsole->isFocused()) {
                m_pEditConsole->GetSelection(&iStart, &iEnd);
            }
        }
        CometFrame::enableUIitem(tEvent, (idT == wxID_CLEAR) || (iEnd > iStart));
    }
    else if (idT == wxID_PASTE) {
        if (pEdit->GetReadOnly()) {
            CometFrame::enableUIitem(tEvent, false);
        }
        else {
            // small performance penalty on Linux if Paste enabled too often
            // update only every 2000 milliseconds
            if (canUpdateUI(2000.0) == false) {
                tEvent.Skip();
                return;
            }
            bool bEnable = false;
            if (wxTheClipboard && wxTheClipboard->Open()) {
                bEnable = wxTheClipboard->IsSupported(wxDF_TEXT);
                wxTheClipboard->Close();
            }
            CometFrame::enableUIitem(tEvent, bEnable);
        }
    }
}

#ifdef __WXMSW__

// rebuildMenuItem necessary since wxWidgets 2.8.12 hide the menu bitmap after using SetText
wxMenuItem *CometFrame::rebuildMenuItem(wxMenuItem *pMenuItem, const wxBitmap &tBmp /* = wxNullBitmap*/, const wxBitmap &tBmpD /* = wxNullBitmap*/)
{
    wxMenu *pMenu = pMenuItem->GetMenu();
    wxMenuItemList items = pMenu->GetMenuItems();
    int posT = items.IndexOf(pMenuItem);

    wxMenuItem *pMenuItemNew = new (std::nothrow) wxMenuItem(pMenu, pMenuItem->GetId(),
#if wxCHECK_VERSION(3, 0, 0)
                                                             pMenuItem->GetItemLabel(),
#else
                                                             pMenuItem->GetText(),
#endif
                                                             pMenuItem->GetHelp(), pMenuItem->GetKind(),
                                                             pMenuItem->GetSubMenu());

    if (pMenuItemNew == NULL) {
        return pMenuItem;
    }

    pMenuItemNew->SetBitmap(tBmp.IsOk() ? tBmp : pMenuItem->GetBitmap());
    pMenuItemNew->SetDisabledBitmap(tBmpD.IsOk() ? tBmpD : pMenuItem->GetDisabledBitmap());
    pMenuItemNew->SetFont(pMenuItem->GetFont());

#if wxUSE_OWNER_DRAWN
    if (pMenuItem->IsOwnerDrawn()) {
        pMenuItemNew->SetOwnerDrawn(true);
        pMenuItemNew->SetMarginWidth(pMenuItem->GetMarginWidth());
        pMenuItemNew->SetDisabledBitmap(pMenuItem->GetDisabledBitmap());
        if (pMenuItem->IsCheckable()) {
            pMenuItemNew->SetCheckable(true);
            pMenuItemNew->SetBitmaps(tBmp.IsOk() ? tBmp : pMenuItem->GetBitmap(true), tBmpD.IsOk() ? tBmpD : pMenuItem->GetBitmap(false));
        }
    }
#endif

    // delete the old menu item
    pMenu->Destroy(pMenuItem);

    // insert the new menu item
    pMenu->Insert(posT, pMenuItemNew);

    return pMenuItemNew;
}

#endif

void CometFrame::OnUpdateExample(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    if (m_pExamplesMenuItem == NULL) {
        return;
    }

    CodeSample *pExample = getExample();
    if (pExample == NULL) {
        CometFrame::enableUIitem(tEvent, false);
        m_pExamplesMenuItem->Enable(false);
        return;
    }

    int idx = tEvent.GetId() - ID_SCRIPT_EXAMPLE_FIRST;
    if (idx < 0) {
        CometFrame::enableUIitem(tEvent, false);
        m_pExamplesMenuItem->Enable(false);
        return;
    }

    m_pExamplesMenuItem->Enable(true);

    if (idx >= pExample->getCount()) {
        wxString strT = wxString::Format(uT("Example #%d"), idx + 1);
        tEvent.SetText(strT);
        CometFrame::enableUIitem(tEvent, false);
        return;
    }

    tEvent.SetText(pExample->getTitle(idx));
    CometFrame::enableUIitem(tEvent, true);
}

void CometFrame::OnUpdateSnippet(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    const int idT = tEvent.GetId();

    wxMenuBar *pMenubar = this->GetMenuBar();
    ScriptEdit *pEdit = getActiveEditor();
    if ((pMenubar == NULL) || (idT < ID_SCRIPT_SNIPPET_FIRST) || (idT > ID_SCRIPT_SNIPPET_LASTMENU) || (pEdit == NULL)) {
        m_pSnippetsMenuItem->Enable(false);
        return;
    }

    CodeSample *pSnippet = getSnippet();
    // the menu contains always the same number of code snippets for all lexers, to avoid performance penalty of recreating it.
    if (pEdit->GetReadOnly() || (pSnippet == NULL) || (pSnippet->getCount() < (ID_SCRIPT_SNIPPET_LASTMENU - ID_SCRIPT_SNIPPET_FIRST + 1))) {
        m_pSnippetsMenuItem->Enable(false);
        return;
    }

    m_pSnippetsMenuItem->Enable(true);

    int ii = idT - ID_SCRIPT_SNIPPET_FIRST;

    const char_t *pszT = pSnippet->getTitle(ii);
    if (NULL == pszT) {
        m_pSnippetsMenuItem->Enable(false);
        return;
    }

    wxString strT(pszT);

    wxMenuItem *pItem = pMenubar->FindItem(idT);
    if (pItem != NULL) {
        pItem->SetText(strT);
        pItem->SetHelp(strT);
#ifdef __WXMSW__
        if (idT == ID_SCRIPT_SNIPPET_FIRST) {
            // rebuildMenuItem necessary since wxWidgets 2.8.12 hide the menu bitmap after using SetText
            rebuildMenuItem(pItem, code_small_xpm, code_small_xpm);
        }
#endif
    }
}

void CometFrame::enableUIitem(wxUpdateUIEvent &tEvent, bool bEnable)
{
    tEvent.Enable(bEnable);
}

void CometFrame::OnUpdateMainAction(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    int idT = tEvent.GetId();
    if (idT == ID_SEARCH_RESETMARKERS) {
        ScriptEdit *pEdit = getActiveEditor();
        if (pEdit == NULL) {
            return;
        }
        CometFrame::enableUIitem(tEvent, (pEdit->getFindMarkerCount() > 0) || (pEdit->getFindIndicCount() > 0));
    }
    else if (idT == ID_FILE_PIN) {
        ScriptEdit *pEdit = getActiveEditor();
        if (pEdit) {
            bool bPinned = pEdit->isPinned();
            wxString strT(bPinned ? uT("Unpin Document") : uT("Pin Document"));
            tEvent.SetText(strT);
        }
    }
    else if ((idT == ID_FILE_RESETMARKERS) || (idT == ID_FILE_RESETMARKERSB)) {
        ScriptEdit *pEdit = getActiveEditor();
        if (pEdit == NULL) {
            return;
        }
        CometFrame::enableUIitem(tEvent, pEdit->containsMarkers());
    }
}

void CometFrame::OnUpdateModifyAction(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    bool bEnable = (pEdit->GetReadOnly() == false);

    bool bIsModified = (pEdit->isModified());
    bool bEnableConv = (bIsModified == false) && (pEdit->GetFilename().IsEmpty() == false) && ((pEdit->isFromCP1252done() == false) || (pEdit->isFromISO8859L1done() == false) || (pEdit->isFromISO8859L9done() == false));

    if (bEnable == false) {
        CometFrame::enableUIitem(tEvent, bEnable);
    }
    else {
        int idT = tEvent.GetId();
        if ((idT == ID_SCRIPT_UTF8FROM_CP1252) || (idT == ID_MAIN_UTF8FROM_CP1252)) {
            CometFrame::enableUIitem(tEvent, (bIsModified == false) && (pEdit->isFromCP1252done() == false));
        }
        else if ((idT == ID_SCRIPT_UTF8FROM_ISO8859L1) || (idT == ID_MAIN_UTF8FROM_ISO8859L1)) {
            CometFrame::enableUIitem(tEvent, (bIsModified == false) && (pEdit->isFromISO8859L1done() == false));
        }
        else if ((idT == ID_SCRIPT_UTF8FROM_ISO8859L9) || (idT == ID_MAIN_UTF8FROM_ISO8859L9)) {
            CometFrame::enableUIitem(tEvent, (bIsModified == false) && (pEdit->isFromISO8859L9done() == false));
        }
    }

    m_pFormatMenuItem->Enable(bEnable);
    m_pDateMenuItem->Enable(bEnable);
    m_pEOLMenuItem->Enable(bEnable);
    m_pUTF8MenuItem->Enable(bEnable && bEnableConv);

    return;
}

void CometFrame::OnUpdateEditorAction(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    if (pEdit->GetReadOnly()) {
        CometFrame::enableUIitem(tEvent, false);
        return;
    }

    int iLexer = pEdit->GetLexer();
    bool bKnownLexer = pEdit->isLexerKnown(iLexer);
    bool bSaved = (pEdit->GetFilename().IsEmpty() == false);

    int idT = tEvent.GetId();

    if (idT == ID_SCRIPT_FORMATDOC) {
        CometFrame::enableUIitem(tEvent, bKnownLexer && (pEdit->GetLineCount() > 1));
        return;
    }

    if ((pEdit->GetSelectionEnd() <= pEdit->GetSelectionStart())) {
        if ((idT == ID_SCRIPT_COMMENTSELECTION) || (idT == ID_SCRIPT_UNCOMMENTSELECTION)) {
            CometFrame::enableUIitem(tEvent, bKnownLexer || (false == bSaved));
            return;
        }
        CometFrame::enableUIitem(tEvent, false);
        return;
    }

    if ((idT == ID_SCRIPT_UPPERCASE) || (idT == ID_SCRIPT_LOWERCASE)) {
        CometFrame::enableUIitem(tEvent, true);
        return;
    }

    CometFrame::enableUIitem(tEvent, bKnownLexer || (false == bSaved));
}

void CometFrame::OnUpdateEditorOptions(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    int idT = tEvent.GetId();

    if (idT == ID_SCRIPT_SYNTAX) {
        tEvent.Check(m_ScintillaPrefs.common.syntaxEnable);
    }
    else if (idT == ID_SCRIPT_AUTOCOMPLETE) {
        tEvent.Check(m_ScintillaPrefs.common.autocompleteEnable);
    }
    else if (idT == ID_SCRIPT_CALLTIP) {
        tEvent.Check(m_ScintillaPrefs.common.calltipEnable);
    }
    else if (idT == ID_SCRIPT_LINENUMBERS) {
        tEvent.Check(m_ScintillaPrefs.common.lineNumberEnable);
    }
    else if (idT == ID_SCRIPT_MARGINMARKERS) {
        tEvent.Check(m_ScintillaPrefs.common.markerEnable);
    }
    else if (idT == ID_SCRIPT_MARGINSTATUS) {
        tEvent.Check(m_ScintillaPrefs.common.statusEnable);
    }
    else if (idT == ID_SCRIPT_FOLDENABLE) {
        tEvent.Check(m_ScintillaPrefs.common.foldEnable);
    }
    else if (idT == ID_SCRIPT_WRAPMODE) {
        tEvent.Check(m_ScintillaPrefs.common.wrapModeInitial);
    }
    else if (idT == ID_SCRIPT_SAVENAVIGATION) {
        tEvent.Check(m_SigmaGUIPrefs.bSaveNavigation);
    }
    else if (idT == ID_SCRIPT_VIEWINFOBAR) {
        tEvent.Check(m_SigmaGUIPrefs.bViewInfobar);
    }
    else if (idT == ID_SCRIPT_DISPLAYEOL) {
        tEvent.Check(m_ScintillaPrefs.common.displayEOLEnable);
    }
    else if (idT == ID_SCRIPT_WHITESPACE) {
        tEvent.Check(m_ScintillaPrefs.common.whiteSpaceEnable);
    }
    else if (idT == ID_SCRIPT_DETECTCHANGE) {
        tEvent.Check(m_SigmaCommonPrefs.bDetectChangeOutside);
    }
    else if (idT == ID_VIEW_EXPLORER) {
        if (m_pFilesysPanel != NULL) {
            wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pFilesysPanel);
            tEvent.Check(paneT.IsShown() && m_pExplorerTree);
        }
        else {
            tEvent.Check(false);
        }
    }
    else if (idT == ID_VIEW_BOOKMARK) {
        if (m_pFilesysPanel != NULL) {
            wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pFilesysPanel);
            tEvent.Check(paneT.IsShown() && m_pListBookmark);
        }
        else {
            tEvent.Check(false);
        }
    }
    else if (idT == ID_VIEW_CONSOLE) {
        if (m_pToolPanel != NULL) {
            wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pToolPanel);
            tEvent.Check(paneT.IsShown() && m_pEditConsole);
        }
        else {
            tEvent.Check(false);
        }
    }
    else if (idT == ID_VIEW_OUTPUT) {
        if (m_pToolPanel != NULL) {
            wxAuiPaneInfo &paneT = m_auiManager.GetPane(m_pToolPanel);
            tEvent.Check(paneT.IsShown() && m_pEditOutput);
        }
        else {
            tEvent.Check(false);
        }
    }
    else if (idT == ID_VIEW_TOOLBAR) {
        m_SigmaGUIPrefs.bViewToolbar = this->GetToolBar()->IsShown();
        tEvent.Check(m_SigmaGUIPrefs.bViewToolbar);
    }
    else if (idT == ID_VIEW_STATUSBAR) {
        m_SigmaGUIPrefs.bViewStatusbar = GetStatusBar()->IsShown();
        tEvent.Check(m_SigmaGUIPrefs.bViewStatusbar);
    }
    else if (idT == ID_VIEW_SEARCHBAR) {
        m_SigmaGUIPrefs.bViewSearchbar = (m_cboAnalyzer != NULL);
        tEvent.Check(m_SigmaGUIPrefs.bViewSearchbar);
    }
    else {
        int iLexer = pEdit->GetLexer();
        if ((iLexer != wxSTC_LEX_LUA) && (pEdit->GetFilename().IsEmpty() == false)) {
            if (idT == ID_SCRIPT_SYNTAX) {
                if (iLexer == wxSTC_LEX_NULL) {
                    CometFrame::enableUIitem(tEvent, false);
                    return;
                }
                CometFrame::enableUIitem(tEvent, true);
            }
            else if ((idT == ID_SCRIPT_AUTOCOMPLETE) || (idT == ID_SCRIPT_CALLTIP)) {
                CometFrame::enableUIitem(tEvent, false);
                return;
            }
            else {
                CometFrame::enableUIitem(tEvent, true);
            }
        }
        else {
            CometFrame::enableUIitem(tEvent, true);
        }
    }
}

void CometFrame::OnUpdateBreakpoint(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    int iLexer = pEdit->GetLexer();
    if (iLexer == wxSTC_LEX_LUA) {
        int idT = tEvent.GetId();
        if (idT == ID_SCRIPT_BREAKPOINT_DELALL) {
            CometFrame::enableUIitem(tEvent, pEdit->getBreakpointCount() > 0);
        }
        else {
            CometFrame::enableUIitem(tEvent, true);
        }
    }
    else {
        CometFrame::enableUIitem(tEvent, false);
    }
}

void CometFrame::DoUpdateStartButton(void)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    if (NULL == m_pNotebookMain) {
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

    wxToolBar *pToolBar = this->GetToolBar();
    if ((NULL == pToolBar) || (false == pToolBar->IsShown())) {
        return;
    }

    int idT = ID_SCRIPT_START;

    int iLexer = pEdit->GetLexer();

    bool bToolSet = CometFrame::isToolSet(iLexer);
    // for batch or bash files, the tool is the file itself
    bool bBatch = ((wxSTC_LEX_BATCH == iLexer) || (wxSTC_LEX_BASH == iLexer));

    if (bToolSet || bBatch) {
        if (idT == ID_SCRIPT_START) {
            pToolBar->EnableTool(idT, pEdit->processIsAlive() == false);
        }
        else {
            pToolBar->EnableTool(idT, false);
        }
        return;
    }

    if (iLexer != wxSTC_LEX_LUA) {
        pToolBar->EnableTool(idT, false);
        return;
    }

    if (idT == ID_SCRIPT_START) {
        pToolBar->EnableTool(idT, (pEdit->isRunning() == false) || (pEdit->isDebugging() == true));
    }
    else if (idT == ID_SCRIPT_STEPINTO) {
        pToolBar->ToggleTool(idT, pEdit->stepInto());
        pToolBar->EnableTool(idT, true);
    }
    else if (idT == ID_SCRIPT_STOP) {
        pToolBar->EnableTool(idT, pEdit->isDebugging());
    }
    else {
        pToolBar->EnableTool(idT, true);
    }
}

void CometFrame::OnUpdateStartStop(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    int idT = tEvent.GetId();
    int iLexer = pEdit->GetLexer();

    bool bToolSet = CometFrame::isToolSet(iLexer);
    // for batch or bash files, the tool is the file itself
    bool bBatch = ((wxSTC_LEX_BATCH == iLexer) || (wxSTC_LEX_BASH == iLexer));

    if (bToolSet || bBatch) {
        if (idT == ID_SCRIPT_START) {
            CometFrame::enableUIitem(tEvent, pEdit->processIsAlive() == false);
        }
        else {
            CometFrame::enableUIitem(tEvent, false);
        }
        return;
    }

    if (iLexer != wxSTC_LEX_LUA) {
        CometFrame::enableUIitem(tEvent, false);
        return;
    }

    if (idT == ID_SCRIPT_START) {
        CometFrame::enableUIitem(tEvent, (pEdit->isRunning() == false) || (pEdit->isDebugging() == true));
    }
    else if (idT == ID_SCRIPT_STEPINTO) {
        tEvent.Check(pEdit->stepInto());
        CometFrame::enableUIitem(tEvent, true);
    }
    else if (idT == ID_SCRIPT_STOP) {
        CometFrame::enableUIitem(tEvent, pEdit->isDebugging());
    }
    else {
        CometFrame::enableUIitem(tEvent, true);
    }
}

void CometFrame::OnUpdateKillTool(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    CometFrame::enableUIitem(tEvent, pEdit->processIsAlive());
}

void CometFrame::OnUpdateScriptClose(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iId = tEvent.GetId();

    if (ID_FILE_CLOSEALLBUTPINNED == iId) {
        int iPinned = 0;
        ScriptEdit *pEdit;
        for (int ii = 0; ii < iPageCount; ii++) {
            pEdit = getEditor(ii);
            if (NULL == pEdit) {
                break;
            }
            if (pEdit->isPinned()) {
                ++iPinned;
            }
        }
        CometFrame::enableUIitem(tEvent, (iPinned > 0) && (iPinned < iPageCount));
    }
    else {
        CometFrame::enableUIitem(tEvent, (ID_FILE_CLOSE == iId) ? (iPageCount >= 1) : (iPageCount >= 2));
    }
}

void CometFrame::OnUpdateScriptReopenLast(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    CometFrame::enableUIitem(tEvent, (tEvent.GetId() == ID_FILE_REOPENLAST) ? (::wxFileExists(static_cast<const char_t *>(m_szLastClosed))) : true);
}

void CometFrame::OnUpdateScriptPath(wxUpdateUIEvent &tEvent)
{
    if (false == CometApp::INITIALIZED) {
        return;
    }

    const int iPageCount = (const int)(m_pNotebookMain->GetPageCount());

    int iSel = m_pNotebookMain->GetSelection();
    if ((iSel < 0) || (iSel >= iPageCount)) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    ScriptEdit *pEdit = getEditor(iSel);
    if (pEdit == NULL) {
        // do not execute Skip before UI action such as Enable or Check
        tEvent.Skip();
        return;
    }

    int idT = tEvent.GetId();

    if (idT == ID_FILE_NEWWINDOW) {
        CometFrame::enableUIitem(tEvent, (pEdit->GetFilename().IsEmpty() == false) && (iPageCount > 1));
    }
    else {
        if (idT == ID_FILE_READONLY) {
            tEvent.Check(pEdit->GetReadOnly());
        }
        CometFrame::enableUIitem(tEvent, pEdit->GetFilename().IsEmpty() == false);
    }
}
