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
#include "FindDirDlg.h"
#include "FindThread.h"

#include <wx/dir.h>

#define STATUSCOLOR_RED  wxColour(120, 0, 20)
#define STATUSCOLOR_BLUE wxColour(0, 80, 120)

BEGIN_EVENT_TABLE(DirFindReplaceComboBox, CometComboBox)
EVT_KEY_DOWN(DirFindReplaceComboBox::OnKeyDown)
END_EVENT_TABLE()

DirFindReplaceComboBox::DirFindReplaceComboBox(wxWindow *pParent, wxWindowID idT,
                                               const wxString &valueT /* = wxEmptyString*/,
                                               const wxPoint &posT /* = wxDefaultPosition*/,
                                               const wxSize &sizeT /* = wxDefaultSize*/,
                                               int nn /* = 0*/, const wxString choicesT[] /* = NULL*/,
                                               long styleT /* = 0*/,
                                               const wxValidator &validatorT /* = wxDefaultValidator*/,
                                               const wxString &nameT /* = wxComboBoxNameStr*/)
    : CometComboBox(pParent, idT, valueT, posT, sizeT, nn, choicesT, styleT | wxTE_PROCESS_ENTER, validatorT, nameT)
{
    m_bReplace = false;
}

void DirFindReplaceComboBox::OnKeyDown(wxKeyEvent &tEvent)
{
    if (tEvent.GetKeyCode() == WXK_RETURN) {
        FindDirDlg *pParentDlg = static_cast<FindDirDlg *>(GetParent());
        if (pParentDlg) {
            if (isReplace() == false) {
                pParentDlg->DoFindAll();
            }
            else {
                pParentDlg->DoReplaceAll();
            }
        }
        return;
    }

    tEvent.Skip();
}

BEGIN_EVENT_TABLE(FindDirDlg, wxDialog)
    EVT_BUTTON(ID_FIND_BROWSEDIR, FindDirDlg::OnBrowseDir)
    EVT_BUTTON(ID_FIND_FINDALL, FindDirDlg::OnFindAll)
    EVT_BUTTON(ID_FIND_REPLACEALL, FindDirDlg::OnReplaceAll)
    EVT_TIMER(TIMER_ID_FINDDIR, FindDirDlg::OnTimer)
    EVT_LIST_ITEM_RIGHT_CLICK(IDL_FIND_RESULT, FindDirDlg::OnListRightClick)
    EVT_LIST_ITEM_ACTIVATED(IDL_FIND_RESULT, FindDirDlg::OnListDoubleClick)
    EVT_MENU(ID_FIND_GOTO, FindDirDlg::OnGoto)
    EVT_MENU(ID_FIND_DELETE, FindDirDlg::OnDelete)
    EVT_MENU(ID_FIND_DELETEALL, FindDirDlg::OnDeleteAll)
    EVT_MENU(ID_SEARCH_FIND, FindDirDlg::OnMainAction)
    EVT_MENU(ID_SEARCH_REPLACE, FindDirDlg::OnMainAction)
    EVT_MENU(ID_SEARCH_FINDINFILES, FindDirDlg::OnMainAction)
    EVT_MENU(ID_SEARCH_REPLACEINFILES, FindDirDlg::OnMainAction)
    EVT_BUTTON(ID_FIND_CLOSE, FindDirDlg::OnFindClose)
    EVT_CLOSE(FindDirDlg::OnCloseWindow)
END_EVENT_TABLE()

FindDirDlg::FindDirDlg(wxWindow *pParent, wxString strFind /* = wxEmptyString*/, bool bReplace /* = false*/,
                       const wxPoint &ptInit /* = FindDirDlg_POSITION*/, wxWindowID idT /* = IDD_FINDDIR*/,
                       const wxString &strTitle /* = FindDirDlg_TITLE*/, const wxSize &sizeT /* = FindDirDlg_SIZE*/,
                       long styleT /* = FindDirDlg_STYLE*/)
    : wxDialog(pParent, idT, strTitle, ptInit, sizeT, styleT)
{
    m_pThreadMutex = new wxMutex(wxMUTEX_DEFAULT);
    m_bThreadRunning = false;
    m_bThreadStop = false;

    m_arrBuffer.Clear();
    m_strFilename = wxEmptyString;

    m_strFilenamePrev = wxEmptyString;

    m_strFind = strFind;
    m_strReplace = wxEmptyString;
    m_bReplace = bReplace;
    m_strDir = wxEmptyString;
    m_iFind = 0;

    m_bReplacing = false;

    m_pTimer = NULL;

    m_cboFind = NULL;
    m_cboReplace = NULL;

    m_cboType = NULL;

    m_cboDir = NULL;

    m_btnBrowseDir = NULL;

    m_optSubDir = NULL;
    m_optCase = NULL;
    m_optWord = NULL;

    m_btnFindAll = NULL;
    m_btnReplaceAll = NULL;
    m_btnClose = NULL;

    m_txtStatus = NULL;

    m_listFind = NULL;

    m_ptInit = ptInit;

    initInterface();

    wxSize sizeM = GetSize();
    SetMinSize(sizeM);

    // Accelerators
    const int nCount = 6;
    wxAcceleratorEntry accelEntries[nCount];
    accelEntries[0].Set(wxACCEL_CTRL, (int)'F', ID_SEARCH_FIND);
    accelEntries[1].Set(wxACCEL_CTRL, (int)'H', ID_SEARCH_REPLACE);
    accelEntries[2].Set(wxACCEL_CTRL, (int)'R', ID_SEARCH_REPLACE);
    accelEntries[3].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'F', ID_SEARCH_FINDINFILES);
    accelEntries[4].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'H', ID_SEARCH_REPLACEINFILES);
    accelEntries[5].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'R', ID_SEARCH_REPLACEINFILES);
    wxAcceleratorTable accelTable(nCount, accelEntries);
    SetAcceleratorTable(accelTable);
    //
}

FindDirDlg::~FindDirDlg()
{
    if (m_pThreadMutex != NULL) {
        delete m_pThreadMutex;
        m_pThreadMutex = NULL;
    }

    if (m_pTimer != NULL) {
        m_pTimer->Stop();
        delete m_pTimer;
        m_pTimer = NULL;
    }
}

void FindDirDlg::updateLabel(const wxString &strT)
{
    m_txtStatus->SetLabel(strT);
}

bool FindDirDlg::doUpdateList(const wxString &strT)
{
    wxArrayString arrT;
    arrT.SetCount(5);
    int ii = 0, iFe = 0;
    wxString strF = strT;
    while (true) {
        iFe = strF.Find(uT("[|]"));
        if (iFe <= 0) {
            if (ii == 4) {
                arrT[ii] = strF;
                ++ii;
            }
            break;
        }
        arrT[ii] = strF.Mid(0, iFe);
        ++ii;
        if (ii >= 5) {
            break;
        }
        strF = strF.Mid(iFe + 3);
    }

    if (ii != 5) {
        return false;
    }

    if (m_iFind < FINDLIST_MAXITEMS) {
        wxListItem itemNew;
        itemNew.SetText(wxString::Format(uT("%d"), m_iFind + 1));
        itemNew.SetId(m_iFind);
        if (m_strFilenamePrev.IsSameAs(arrT[0]) == false) {
            m_strFilenamePrev = arrT[0];
        }
        wxString strTT = arrT[0];
        strTT += uT("(");
        strTT += arrT[1];
        strTT += uT(")");
        long itemIndex = m_listFind->InsertItem(itemNew);
        m_listFind->SetItem(itemIndex, 1, strTT);
        m_listFind->SetItem(itemIndex, 2, arrT[2]);
    }
    else if (m_iFind == FINDLIST_MAXITEMS) {
        wxString strTT = uT("( ... Too many results ... )");
        long itemIndex = m_listFind->InsertItem(m_iFind, strTT);
        strTT = uT("( ... )");
        m_listFind->SetItem(itemIndex, 2, strTT);
        return false;
    }

    m_iFind += 1;
    return true;
}

void FindDirDlg::updateTypeList(void)
{
    if (m_cboType == NULL) {
        return;
    }

    wxArrayString arTypes;
    arTypes.Add(uT("Lua (*.lua;*.comet)"));
    arTypes.Add(uT("C/C++ (*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.rc)"));
    arTypes.Add(uT("Text/Shell (*.txt;*.dat;*.csv;*.log;*.conf;*.bat;*.sh;*.csh;*.ins;*.bst;*.patch)"));
    arTypes.Add(uT("Python (*.py;*.pyw;*.sage;*.sagews)"));
    arTypes.Add(uT("Fortran (*.f;*.for;*.f90;*.f95;*.f03;*.f08;*.f15)"));
    arTypes.Add(uT("Octave (*.m;*.mat)"));
    arTypes.Add(uT("LaTeX files (*.tex;*.bib;*.cls;*.sty)"));
    arTypes.Add(uT("Makefile"));
    arTypes.Add(uT("Solis files (*.solis)"));
    arTypes.Add(uT("Comet settings files (*.cometr;*.cometu)"));
    arTypes.Add(uT("All (*.*)"));

    m_cboType->Clear();
    for (size_t ii = 0; ii < arTypes.GetCount(); ii++) {
        m_cboType->AppendString(arTypes[ii]);
    }
    m_cboType->SetSelection(0);
}

void FindDirDlg::updateDirList(const wxString &strDir)
{
    if (m_cboDir == NULL) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    int ii;

    m_cboDir->Clear();
    if (pFrame->getDirCount() > 0) {
        for (ii = 0; ii < pFrame->getDirCount(); ii++) {
            m_cboDir->AppendString(pFrame->getDirItem(ii));
        }
    }

    m_cboDir->SetValue(strDir);
    m_cboDir->SelectAll();
}

void FindDirDlg::updateFindList(const wxString &strFind)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    int ii;

    m_cboFind->Clear();
    if (pFrame->getFindCount() > 0) {
        for (ii = 0; ii < pFrame->getFindCount(); ii++) {
            m_cboFind->AppendString(pFrame->getFindItem(ii));
        }
    }

    m_cboFind->SetValue(strFind);
    m_cboFind->SelectAll();
}

void FindDirDlg::updateReplaceList(const wxString &strReplace)
{
    if (m_bReplace == false) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    int ii;

    m_cboReplace->Clear();
    if (pFrame->getReplaceCount() > 0) {
        for (ii = 0; ii < pFrame->getReplaceCount(); ii++) {
            m_cboReplace->AppendString(pFrame->getReplaceItem(ii));
        }
        m_cboReplace->SetValue(strReplace);
        m_cboReplace->SelectAll();
    }
}

void FindDirDlg::updateItems(const wxString &strFind, const wxString &strReplace, const wxString &strDir)
{
    if (isRunning()) {
        return;
    }

    updateDirList(strDir);
    updateFindList(strFind);
    updateReplaceList(strReplace);
}

void FindDirDlg::initInterface()
{
    SetHelpText(m_bReplace ? FindDirDlg_TITLEX : FindDirDlg_TITLE);
    if (FindDirDlg::ShowToolTips()) {
        SetToolTip(m_bReplace ? FindDirDlg_TITLEX : FindDirDlg_TITLE);
    }
    SetTitle(m_bReplace ? FindDirDlg_TITLEX : FindDirDlg_TITLE);

    SetIcon((char **)find_files_small_xpm);

    wxSize sizeBtn = CometApp::getTextExtent(this, uT(">Weplace Ally<"));
    const int iH = sizeBtn.GetHeight();
    const int iW = sizeBtn.GetWidth();
    sizeBtn.SetHeight(iH + 10);

    wxSize sizeBtnSm = CometApp::getTextExtent(this, uT("123456"));
    sizeBtnSm.SetHeight(iH + 10);
    const int iWs = sizeBtnSm.GetWidth();

    wxSize sizeText;
    sizeText.SetHeight(iH);
    sizeText.SetWidth((iW * 4) + 12);

    wxSize sizeList;
    sizeList.SetHeight((iH + 8) * 8);
    sizeList.SetWidth((iW * 4) + 12);

    wxSize sizeOpt = sizeText;
    sizeOpt.SetWidth(sizeText.GetWidth() - 8 - iWs);

    wxSize sizeOptCase = sizeText;
    wxSize sizeOptWord = sizeText;
    if (m_bReplace) {
        sizeOptCase.SetWidth(sizeText.GetWidth() - 8 - iW - 8 - iW);
    }
    else {
        sizeOptCase.SetWidth(sizeText.GetWidth() - 8 - iW);
    }
    sizeOptWord.SetWidth(sizeText.GetWidth() - 8 - iW);

    wxSize sizeEdit = sizeText;
    sizeEdit.SetHeight(iH + 10);

    m_cboFind = new DirFindReplaceComboBox(this, IDE_FIND_WHAT, uT(""), wxDefaultPosition, sizeEdit, 0, 0);
    m_cboFind->setReplace(false);

    if (m_bReplace) {
        m_cboReplace = new DirFindReplaceComboBox(this, IDE_FIND_REPLACE, uT(""), wxDefaultPosition, sizeEdit, 0, 0);
        m_cboReplace->setReplace(true);
    }

    m_cboType = new wxComboBox(this, IDE_FIND_TYPE, uT(""), wxDefaultPosition, sizeEdit, 0, wxCB_READONLY);
    updateTypeList();

    m_cboDir = new CometComboBox(this, IDE_FIND_DIR, uT(""), wxDefaultPosition, sizeEdit, 0, 0);

    m_optSubDir = new wxCheckBox(this, IDK_FIND_SUBDIR, uT("Include &Sub-directories"), wxDefaultPosition, sizeOpt);
    m_btnBrowseDir = new wxButton(this, ID_FIND_BROWSEDIR, uT("..."), wxDefaultPosition, sizeBtnSm, 0);
    m_optCase = new wxCheckBox(this, IDK_FIND_CASE, uT("Match cas&e"), wxDefaultPosition, sizeOptCase);
    m_optWord = new wxCheckBox(this, IDK_FIND_WORD, uT("Match whole &word"), wxDefaultPosition, sizeOptWord);

    m_btnFindAll = new wxButton(this, ID_FIND_FINDALL, uT("&Find All"), wxDefaultPosition, sizeBtn, 0);
    if (m_bReplace) {
        m_btnReplaceAll = new wxButton(this, ID_FIND_REPLACEALL, uT("&Replace All"), wxDefaultPosition, sizeBtn, 0);
    }
    m_btnClose = new wxButton(this, ID_FIND_CLOSE, uT("&Close"), wxDefaultPosition, sizeBtn, 0);

    m_txtStatus = new wxStaticText(this, wxID_ANY, uT(""), wxDefaultPosition, sizeText, 0);
    m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);

    m_listFind = new wxListCtrl(this, IDL_FIND_RESULT, wxDefaultPosition, sizeList, wxLC_REPORT | wxLC_SINGLE_SEL);

    int iWl = sizeList.GetWidth();
    wxListItem itemAt;
    long maskT = itemAt.GetMask();
    maskT |= (wxLIST_MASK_TEXT | wxLIST_MASK_FORMAT);
    itemAt.SetMask(maskT);
    itemAt.SetId(0);
    itemAt.SetText(uT("Id"));
    itemAt.SetWidth(iWs);
    itemAt.SetAlign(wxLIST_FORMAT_RIGHT);
    m_listFind->InsertColumn(0, itemAt);

    wxListItem itemBt;
    maskT = itemBt.GetMask();
    maskT |= (wxLIST_MASK_TEXT | wxLIST_MASK_FORMAT);
    itemBt.SetMask(maskT);
    itemBt.SetId(1);
    itemBt.SetText(uT("Filename"));
    itemBt.SetWidth((iWl / 2) - iWs);
    itemBt.SetAlign(wxLIST_FORMAT_LEFT);
    m_listFind->InsertColumn(1, itemBt);

    wxListItem itemCt;
    maskT = itemCt.GetMask();
    maskT |= (wxLIST_MASK_TEXT | wxLIST_MASK_FORMAT);
    itemCt.SetMask(maskT);
    itemCt.SetId(2);
    itemCt.SetText(uT("Text"));
    itemCt.SetWidth((iWl / 2) - iWs);
    itemCt.SetAlign(wxLIST_FORMAT_LEFT);
    m_listFind->InsertColumn(2, itemCt);

    wxBoxSizer *pSizerMain = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *pSizerTop = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *pSizerSubdir = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerCase = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerWord = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer *pSizerStatus = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer *pSizerBottom = new wxBoxSizer(wxVERTICAL);

    pSizerTop->AddSpacer(8);
    pSizerTop->Add(new wxStaticText(this, wxID_ANY, uT("Find what:"), wxDefaultPosition, sizeText, 0), 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerTop->AddSpacer(4);
    pSizerTop->Add(m_cboFind, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);

    if (m_bReplace) {
        pSizerTop->AddSpacer(8);
        pSizerTop->Add(new wxStaticText(this, wxID_ANY, uT("Replace with:"), wxDefaultPosition, sizeText, 0), 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
        pSizerTop->AddSpacer(4);
        pSizerTop->Add(m_cboReplace, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    }

    pSizerTop->AddSpacer(8);
    pSizerTop->Add(new wxStaticText(this, wxID_ANY, uT("File type(s):"), wxDefaultPosition, sizeText, 0), 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerTop->AddSpacer(4);
    pSizerTop->Add(m_cboType, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);

    pSizerTop->AddSpacer(8);
    pSizerTop->Add(new wxStaticText(this, wxID_ANY, uT("Look in:"), wxDefaultPosition, sizeText, 0), 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerTop->AddSpacer(4);
    pSizerTop->Add(m_cboDir, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);

    pSizerSubdir->Add(m_optSubDir, 2, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerSubdir->AddSpacer(8);
    pSizerSubdir->Add(m_btnBrowseDir, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);

    pSizerTop->AddSpacer(4);
    pSizerTop->Add(pSizerSubdir, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);

    pSizerCase->Add(m_optCase, 2, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerCase->AddSpacer(8);
    if (m_bReplace) {
        pSizerCase->Add(m_btnReplaceAll, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
        pSizerCase->AddSpacer(4);
    }
    pSizerCase->Add(m_btnFindAll, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);

    pSizerTop->AddSpacer(4);
    pSizerTop->Add(pSizerCase, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);

    pSizerWord->Add(m_optWord, 2, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerWord->AddSpacer(8);
    pSizerWord->Add(m_btnClose, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);

    pSizerTop->AddSpacer(4);
    pSizerTop->Add(pSizerWord, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);

    pSizerStatus->AddSpacer(8);
    pSizerStatus->Add(m_txtStatus, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerStatus->AddSpacer(8);

    pSizerBottom->AddSpacer(8);
    pSizerBottom->Add(pSizerStatus, 0, wxEXPAND | wxALL, 0);

    pSizerBottom->Add(m_listFind, 2, wxEXPAND | wxALL, 8);

    pSizerMain->Add(pSizerTop, 0, wxEXPAND | wxRIGHT | wxLEFT, 8);
    pSizerMain->Add(pSizerBottom, 2, wxEXPAND | wxUP | wxDOWN, 0);

    this->SetSizerAndFit(pSizerMain);

    Move(m_ptInit);
}

void FindDirDlg::OnBrowseDir(wxCommandEvent &tEvent)
{
    wxDirDialog *dirDlg = new (std::nothrow) wxDirDialog(this, uT("Choose a directory"), m_strDir, 0, wxDefaultPosition);
    if (dirDlg && (dirDlg->ShowModal() == wxID_OK)) {
        m_strDir = dirDlg->GetPath();
        m_cboDir->SetValue(m_strDir);
    }
}

void FindDirDlg::DoFindReplace(bool bReplace /* = false*/)
{
    if (bReplace && (m_bReplace == false)) {
        return;
    }

    bool bRunning = isRunning();

    if (bRunning) {
        stopIt(true);
        return;
    }

    if (setRunning(true) == false) {
        return;
    }
    stopIt(false);

    // for static combobox (without edit ctrl) do not use GetValue()
    int iSel = m_cboType->GetSelection();
    if (iSel == wxNOT_FOUND) {
        iSel = 0;
    }
    m_cboType->SetSelection(iSel);
    const wxString strType = m_cboType->GetString(iSel);

    wxString strFind = m_cboFind->GetValue(),
             strReplace = wxEmptyString,
             strDir = m_cboDir->GetValue();

    // Warn in case of 'Replace All'
    if (bReplace) {
        strReplace = m_cboReplace->GetValue();
        if (strReplace.IsSameAs(strFind)) {
            m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
            updateLabel(uT("Replacement string is identical to the input string"));
            setRunning(false);
            stopIt(false);
            return;
        }

        int iret = SigmaMessageBox(uT("You are about to replace one string with another in all directory files.\nReplacements in files not currently open cannot be undone.\nContinue anyway ?"),
                                   uT("Replace All"), wxYES_NO | wxICON_QUESTION | wxNO_DEFAULT, this, uT("Replace All"), uT("Cancel"));
        if (iret != wxYES) {
            m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
            updateLabel(uT("Replace All cancelled"));
            setRunning(false);
            stopIt(false);
            return;
        }
    }

    FindThread *pThread = NULL;
    try {
        pThread = new FindThread();
    }
    catch (...) {
        pThread = NULL;
    }
    if (pThread == NULL) {
        m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
        updateLabel(uT("! Insufficient memory"));
        setRunning(false);
        stopIt(false);
        return;
    }

    if (strFind.IsEmpty() || strType.IsEmpty() || strDir.IsEmpty()) {
        setRunning(false);
        stopIt(false);
        return;
    }

    updateLabel(uT("Searching..."));

    wxThreadError errT = pThread->Create(strFind, bReplace, strReplace, strType, strDir,
                                         m_optSubDir->IsChecked(), m_optWord->IsChecked(), m_optCase->IsChecked());

    if (errT != wxTHREAD_NO_ERROR) {
        m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
        if (errT == wxTHREAD_NO_RESOURCE) {
            updateLabel(uT("! Insufficient memory"));
        }
        else if (errT == wxTHREAD_RUNNING) {
            updateLabel(uT("! Already running"));
        }
        else {
            updateLabel(uT("! Cannot search"));
        }
        setRunning(false);
        stopIt(false);
        return;
    }

    m_strDir = strDir;
#if defined(__WXMSW__)
    if (m_strDir.EndsWith(uT("\\")) == false) {
        m_strDir += uT('\\');
    }
#else
    if (m_strDir.EndsWith(uT("/")) == false) {
        m_strDir += uT('/');
    }
#endif
    m_iFind = 0;
    m_strFind = strFind;
    m_strReplace = strReplace;

    m_listFind->DeleteAllItems();
    m_txtStatus->SetForegroundColour(STATUSCOLOR_BLUE);

    errT = pThread->Run();

    if (errT != wxTHREAD_NO_ERROR) {
        m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
        if (errT == wxTHREAD_NO_RESOURCE) {
            updateLabel(uT("! Insufficient memory"));
        }
        else if (errT == wxTHREAD_RUNNING) {
            updateLabel(uT("! Already running"));
        }
        else {
            updateLabel(uT("! Cannot search"));
        }
        setRunning(false);
        stopIt(false);
        return;
    }
}

void FindDirDlg::DoFindAll(void)
{
    m_bReplacing = false;
    DoFindReplace(false);
}

void FindDirDlg::DoReplaceAll(void)
{
    m_bReplacing = true;
    DoFindReplace(true);
}

void FindDirDlg::OnFindAll(wxCommandEvent &tEvent)
{
    DoFindAll();
}

void FindDirDlg::OnReplaceAll(wxCommandEvent &tEvent)
{
    DoReplaceAll();
}

void FindDirDlg::updateList(void)
{
    bool bFind = false;
    wxArrayString arrBuffer;
    wxString strFilename = wxEmptyString;

    arrBuffer.Clear();
    int iCount = 0;

    if (m_pThreadMutex->TryLock() == wxMUTEX_NO_ERROR) {
        strFilename = wxString(LM_CSTR(m_strFilename));
        iCount = (int)(m_arrBuffer.Count());
        if (iCount > 0) {
            for (int ii = 0; ii < iCount; ii++) {
                arrBuffer.Add(LM_CSTR(m_arrBuffer[ii]));
            }
            m_arrBuffer.Clear();
            bFind = true;
        }
        m_pThreadMutex->Unlock();
        updateLabel(strFilename);
    }

    if ((bFind == false) || (iCount < 1)) {
        return;
    }

    if ((iCount < 1) && (arrBuffer[0].StartsWith(uT("Finished[|]")) == true)) {
        endTask(arrBuffer[0]);
        return;
    }

    for (int ii = 0; ii < iCount; ii++) {
        if (arrBuffer[ii].StartsWith(uT("Finished[|]")) == true) {
            endTask(arrBuffer[ii]);
            return;
        }
        if (doUpdateList(arrBuffer[ii]) == false) {
            break;
        }
    }
}

void FindDirDlg::endTask(const wxString &strT)
{

    // Bring to front
    if (this->IsIconized() || (this->IsShown() == false)) {
        this->Iconize(false);
        this->SetFocus();
        this->Raise();
        this->Show(true);
    }
    //

    wxArrayString arrT;
    arrT.SetCount(5);
    int ii = 0, iFe = 0;
    wxString strF = strT;
    while (true) {
        iFe = strF.Find(uT("[|]"));
        if (iFe <= 0) {
            if (ii == 4) {
                arrT[ii] = strF;
                ++ii;
            }
            break;
        }
        arrT[ii] = strF.Mid(0, iFe);
        ++ii;
        if (ii >= 5) {
            break;
        }
        strF = strF.Mid(iFe + 3);
    }

    wxString strTT = wxEmptyString;

    if ((ii != 5) || (m_iFind < 1)) {
        if (m_iFind < 1) {
            strTT = uT("Item not found.");
            if (m_optSubDir->IsChecked() == false) {
                wxDir dirT(m_strDir);
                if (dirT.HasSubDirs()) {
                    strTT += uT(" NB: 'Include Sub-directories' unchecked.");
                }
            }
            m_txtStatus->SetForegroundColour(STATUSCOLOR_RED);
        }
        else {
            strTT = wxString::Format((m_iFind > 1) ? uT("%d items found.") : uT("%d item found."), m_iFind);
        }
    }

    else {
        wxColour clrT = STATUSCOLOR_BLUE;

        strTT = arrT[1];
        strTT += (m_iFind > 1) ? uT(" items found") : uT(" item found");
        if (m_bReplacing) {
            strTT += uT(" and ");
            strTT += arrT[2];
            strTT += uT(" replaced");
            if (arrT[2].IsSameAs(arrT[1]) == false) {
                clrT = STATUSCOLOR_RED;
            }
        }
        strTT += uT(" in ");
        strTT += arrT[3];
        long iFC = 2L;
        arrT[3].ToLong(&iFC);
        strTT += ((iFC > 1) ? uT(" files (Searched: ") : uT(" file (searched: "));
        strTT += arrT[4];
        strTT += uT(").");

        m_txtStatus->SetForegroundColour(clrT);

        CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
        if (pFrame) {
            if (pFrame->addDirItem(m_strDir) >= 0) {
                updateDirList(m_strDir);
            }

            pFrame->addFindItem(m_strFind);
            updateFindList(m_strFind);

            if (m_bReplacing && (m_strReplace.IsEmpty() == false)) {
                if (pFrame->addReplaceItem(m_strReplace) >= 0) {
                    updateReplaceList(m_strReplace);
                }
            }
        }
    }

    updateLabel(strTT);

    setRunning(false);
    stopIt(false);
    return;
}

bool FindDirDlg::setRunning(bool bRunning)
{
    bool bRunningX = (bRunning == false);

    if (m_pThreadMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_bThreadRunning = bRunning;
        bRunningX = m_bThreadRunning;
        m_arrBuffer.Clear();
        m_strFilename.Empty();
        m_pThreadMutex->Unlock();
    }

    if (m_bReplacing) {
        m_btnFindAll->Enable(bRunning == false);
        m_btnReplaceAll->SetLabel(bRunningX ? uT("Stop") : uT("&Replace All"));
    }
    else {
        m_btnFindAll->SetLabel(bRunningX ? uT("Stop") : uT("&Find All"));
        if (m_bReplace) {
            m_btnReplaceAll->Enable(bRunning == false);
        }
    }
    if (bRunningX == false) {
        m_bReplacing = false;
    }

    if (m_pTimer != NULL) {
        m_pTimer->Stop();
        delete m_pTimer;
        m_pTimer = NULL;
    }

    if (bRunningX) {
        try {
            m_pTimer = new wxTimer(this, TIMER_ID_FINDDIR);
        }
        catch (...) {
            m_pTimer = NULL;
        }
        if (m_pTimer) {
            m_pTimer->Start(160, false);
        }
    }

    return bRunningX;
}

bool FindDirDlg::isRunning(void)
{
    bool bRunning = false;

    if (m_pThreadMutex->TryLock() == wxMUTEX_NO_ERROR) {
        bRunning = m_bThreadRunning;
        m_pThreadMutex->Unlock();
    }

    return bRunning;
}

bool FindDirDlg::stopIt(bool bStopIt)
{
    bool bStopItX = (bStopIt == false);

    if (m_pThreadMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_bThreadStop = bStopIt;
        bStopItX = m_bThreadStop;
        m_pThreadMutex->Unlock();
    }

    return bStopItX;
}

void FindDirDlg::OnTimer(wxTimerEvent &tEvent)
{
    tEvent.Skip();

    if ((m_pTimer == NULL) || (tEvent.GetId() != TIMER_ID_FINDDIR)) {
        return;
    }

    updateList();
}

void FindDirDlg::gotoItem(long iSel)
{
    if (iSel < 0) {
        iSel = m_listFind->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    }

    if ((iSel < 0) || (iSel >= m_listFind->GetItemCount())) {
        return;
    }

    wxListItem rowInfo;
    rowInfo.m_itemId = iSel;
    rowInfo.m_col = 1;
    rowInfo.m_mask = wxLIST_MASK_TEXT;
    if (m_listFind->GetItem(rowInfo) == false) {
        return;
    }

    int iF = rowInfo.m_text.Find(uT('('), true);
    wxString strF = rowInfo.m_text.Mid(0, iF);
    wxString strL = rowInfo.m_text.Mid(iF + 1);
    wxString filenameT = m_strDir;
    filenameT += strF;

    if (::wxFileExists(filenameT) == false) {
        return;
    }

    strL = strL.Mid(0, strL.Length() - 1);

    long iSelLine = -1L;
    if (strL.ToLong(&iSelLine, 10) == false) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame) {
        pFrame->fileOpen(filenameT, false, iSelLine - 1, m_strFind, m_strReplace, false);
    }
}

void FindDirDlg::OnGoto(wxCommandEvent &WXUNUSED(tEvent))
{
    gotoItem(-1);
}

void FindDirDlg::OnDelete(wxCommandEvent &WXUNUSED(tEvent))
{
    if (this->isRunning()) {
        return;
    }

    long iSel = m_listFind->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (iSel >= 0L) {
        m_listFind->DeleteItem(iSel);
    }
}

void FindDirDlg::OnDeleteAll(wxCommandEvent &WXUNUSED(tEvent))
{
    if (this->isRunning()) {
        return;
    }

    m_listFind->DeleteAllItems();
}

void FindDirDlg::OnMainAction(wxCommandEvent &tEvent)
{
    if (((tEvent.GetId() == ID_SEARCH_FINDINFILES) && findOnly()) || ((tEvent.GetId() == ID_SEARCH_REPLACEINFILES) && (findOnly() == false))) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }
    pFrame->DoMainAction(tEvent.GetId());
}

void FindDirDlg::OnListRightClick(wxListEvent &tEvent)
{
    if (this->isRunning()) {
        tEvent.Veto();
        return;
    }

    wxMenu popMenu;

    wxPoint pointT = wxGetMousePosition();
    pointT = this->ScreenToClient(pointT);

    wxMenuItem *pItem = new wxMenuItem(&popMenu, ID_FIND_GOTO, uT("Goto"), uT("Goto found item"), wxITEM_NORMAL);
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    pItem->SetBitmap(find_next_small_xpm);
#endif
    popMenu.Append(pItem);
    popMenu.AppendSeparator();
    popMenu.Append(ID_FIND_DELETE, uT("Delete"), uT("Delete item"), wxITEM_NORMAL);
    popMenu.Append(ID_FIND_DELETEALL, uT("Delete All"), uT("Delete all items"), wxITEM_NORMAL);

    PopupMenu(&popMenu, pointT.x, pointT.y);
}

void FindDirDlg::OnListDoubleClick(wxListEvent &tEvent)
{
    gotoItem(tEvent.GetIndex());
}

void FindDirDlg::OnFindClose(wxCommandEvent &WXUNUSED(tEvent))
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame != NULL) {
        pFrame->updateFindDirPos();
    }
    this->Hide();
}

void FindDirDlg::OnCloseWindow(wxCloseEvent &WXUNUSED(tEvent))
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame != NULL) {
        pFrame->updateFindDirPos();
    }
    this->Hide();
}

bool FindDirDlg::ShowToolTips()
{
    return false;
}
