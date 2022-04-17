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
#include "ScriptPrint.h"

#define FILE_HEADER_COMETM    0x96
#define FILE_HEADER_MARKERS   0x10
#define FILE_HEADER_SELECTION 0x20

bool ScriptEdit::updateFilename(const wxString &filenameT, bool bSelect /* = true*/, bool bFocus /* = true*/)
{
    if (filenameT.IsEmpty()) {
        return false;
    }

    this->SetFilename(filenameT);
    wxFileName fname = this->GetFilename();
    fname.GetTimes(NULL, &m_ChangeTime, NULL);

    wxAuiNotebook *pNotebook = (wxAuiNotebook *)(GetParent());
    int indexT = pNotebook->GetPageIndex(this);
    if (wxNOT_FOUND == indexT) {
        return false;
    }
    pNotebook->SetPageText(indexT, fname.GetFullName());
    pNotebook->SetPageToolTip(indexT, fname.GetFullPath());
    if (bSelect) {
        pNotebook->SetSelection(indexT, bFocus);
    }
    SetReadOnly(lf_fileReadonly(LM_CSTR(GetFilename())));

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (pFrame) {
        pFrame->addToRecent(this->GetFilename());
    }

    return true;
}

bool ScriptEdit::DoNewFile(ScintillaPrefs &tScintillaPrefs, const wxString &strTemplate, int iLexer, int iLexerVar)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return false;
    }

    m_strDebugTrace.Empty();

    m_strFilename.Empty();
    m_ChangeTime.ResetTime();

    setModeScriptConsole(false);

    SetReadOnly(false);
    ClearAll();
    m_iBookmarkCount = 0;
    m_iBreakpointCount = 0;

    wxString strT = SCRIPT_DEFAULT_TITLE;
    switch (iLexer) {
        case wxSTC_LEX_LUA:
            strT = uT("Lua");
            break;
        case wxSTC_LEX_CPP:
            strT = (iLexerVar == ID_TEMPLATE_C) ? uT("C") : ((iLexerVar == ID_TEMPLATE_CMOD) ? uT("C module") : uT("C++"));
            break;
        case wxSTC_LEX_PYTHON:
            strT = uT("Python");
            break;
        case wxSTC_LEX_FORTRAN:
            strT = uT("Fortran");
            break;
        case wxSTC_LEX_MAKEFILE:
            strT = uT("Makefile");
            break;
        case wxSTC_LEX_LATEX:
            strT = (LEXERTYPE_FIRST == iLexerVar) ? uT("LaTeX") : uT("BibTeX");
            break;
        case wxSTC_LEX_COMET:
            strT = uT("Comet");
            break;
        case wxSTC_LEX_BATCH:
            strT = uT("Batch");
            break;
        case wxSTC_LEX_SOLIS:
            strT = uT("Solis");
            break;
        default:
            break;
    }

    initLexer(iLexer, iLexerVar);

    if ((wxSTC_LEX_LATEX == iLexer) && (false == m_ScintillaPrefs.common.wrapModeInitial) && (false == m_ScintillaPrefs.loaded)) {
        m_ScintillaPrefs.common.wrapModeInitial = true;
        DoWrapMode(m_ScintillaPrefs.common.wrapModeInitial);
    }

    if (m_bEnableCodeAnalyzer && (m_pCodeAnalyzer != NULL)) {
        m_pCodeAnalyzer->reset();
    }

    if (strTemplate.IsEmpty() == false) {

        this->SetTextRaw(LM_U8STR(strTemplate));

        this->DoFormatDoc();

#ifdef __WXMSW__
        this->ConvertEOLs(wxSTC_EOL_CRLF);
        this->SetEOLMode(wxSTC_EOL_CRLF);
#else
        this->ConvertEOLs(wxSTC_EOL_LF);
        this->SetEOLMode(wxSTC_EOL_LF);
#endif

        if (m_bEnableCodeAnalyzer && (m_pCodeAnalyzer != NULL)) {
            switch (iLexer) {
                case wxSTC_LEX_LUA:
                    m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_LUA);
                    break;
                case wxSTC_LEX_PYTHON:
                    m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_PYTHON);
                    break;
                case wxSTC_LEX_LATEX:
                    m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_LATEX);
                    break;
                case wxSTC_LEX_CPP:
                    if (m_iLexerType == LEXERTYPE_FIRST) {
                        m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_C);
                    }
                    break;
                case wxSTC_LEX_SOLIS:
                    m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_SOLIS);
                    break;
                default:
                    break;
            }
        }

        DoHighlightSyntax(m_ScintillaPrefs.common.syntaxEnable);
    }

    resetModified();

    pFrame->updateEditorStatus(this);
    updatePage(strT);
    updateStatusbar(true);

    return true;
}

int ScriptEdit::supportedLexer(const wxString &strExt, int *piLexerType /* = NULL*/)
{
    int iLexer = 0;

    if (strExt.IsSameAs(uT("lua")) || strExt.IsSameAs(uT("comet"))) {
        iLexer = wxSTC_LEX_LUA;
    }
    else if (strExt.IsSameAs(uT("c")) || strExt.IsSameAs(uT("cpp")) || strExt.IsSameAs(uT("C")) || strExt.IsSameAs(uT("cxx")) || strExt.IsSameAs(uT("cc")) || strExt.IsSameAs(uT("h")) || strExt.IsSameAs(uT("hpp")) || strExt.IsSameAs(uT("hh")) || strExt.IsSameAs(uT("hxx")) || strExt.IsSameAs(uT("inl")) || strExt.IsSameAs(uT("rc")) || strExt.IsSameAs(uT("inc"))) {
        iLexer = wxSTC_LEX_CPP;
        if (piLexerType) {
            *piLexerType = strExt.IsSameAs(uT("c")) ? LEXERTYPE_FIRST : LEXERTYPE_SECOND;
        }
    }
    else if (strExt.IsSameAs(uT("txt")) || strExt.IsSameAs(uT("dat")) || strExt.IsSameAs(uT("csv")) || strExt.IsSameAs(uT("conf")) || strExt.IsSameAs(uT("config"))) {
        iLexer = wxSTC_LEX_NULL;
    }
    else if (strExt.IsSameAs(uT("sh")) || strExt.IsSameAs(uT("csh")) || strExt.IsSameAs(uT("ksh"))) {
        iLexer = wxSTC_LEX_BASH;
    }
    else if (strExt.IsSameAs(uT("py")) || strExt.IsSameAs(uT("pyw")) || strExt.IsSameAs(uT("sage")) || strExt.IsSameAs(uT("sagews"))) {
        iLexer = wxSTC_LEX_PYTHON;
    }
    else if (strExt.IsSameAs(uT("m")) || strExt.IsSameAs(uT("mat"))) {
        iLexer = wxSTC_LEX_OCTAVE;
    }
    else if (strExt.IsSameAs(uT("f")) || strExt.IsSameAs(uT("for")) || strExt.IsSameAs(uT("f90")) || strExt.IsSameAs(uT("f95")) || strExt.IsSameAs(uT("f03")) || strExt.IsSameAs(uT("f08")) || strExt.IsSameAs(uT("f15"))) {
        iLexer = wxSTC_LEX_FORTRAN;
    }
    else if (strExt.IsSameAs(uT("tex")) || strExt.IsSameAs(uT("bib")) || strExt.IsSameAs(uT("cls")) || strExt.IsSameAs(uT("sty"))) {
        iLexer = wxSTC_LEX_LATEX;
        if (piLexerType) {
            *piLexerType = strExt.IsSameAs(uT("bib")) ? LEXERTYPE_SECOND : LEXERTYPE_FIRST;
        }
    }
    else if (strExt.IsSameAs(uT("bat"))) {
        iLexer = wxSTC_LEX_BATCH;
    }
    else if (strExt.IsSameAs(uT("solis"))) {
        iLexer = wxSTC_LEX_SOLIS;
        if (piLexerType) {
            *piLexerType = LEXERTYPE_FIRST;
        }
    }
    else if (strExt.IsSameAs(uT("cometr"))) {
        iLexer = wxSTC_LEX_SOLIS;
        if (piLexerType) {
            *piLexerType = LEXERTYPE_THIRD;
        }
    }
    else if (strExt.IsSameAs(uT("cometu"))) {
        iLexer = wxSTC_LEX_SOLIS;
        if (piLexerType) {
            *piLexerType = LEXERTYPE_FOURTH;
        }
    }

    return iLexer;
}

int ScriptEdit::lexerFromExtension(const wxString &strExt, const wxString &strShortFilename)
{
    int iLexer = supportedLexer(strExt, &m_iLexerType);

    if (iLexer != 0) {
        return iLexer;
    }

    if (strShortFilename.IsSameAs(uT("Makefile")) || strShortFilename.IsSameAs(uT("makefile"))) {
        iLexer = wxSTC_LEX_MAKEFILE;
    }
    else if ((strExt.IsEmpty() == false) && strExt.IsSameAs(static_cast<const char_t *>(PLANGPREFS_USER ? PLANGPREFS_USER->filepattern : uT("")))) {
        iLexer = wxSTC_LEX_COMET;
    }

    return iLexer;
}

bool ScriptEdit::isExtKnown(void)
{
    wxFileName fname = this->GetFilename();
    if (fname.IsOk() == false) {
        return false;
    }
    wxString strExt = fname.GetExt();
    wxString strShortFilename = fname.GetFullName();
    int iLexer = lexerFromExtension(strExt, strShortFilename);
    return (0 != iLexer);
}

void ScriptEdit::enforceLexer(int iLexerF)
{
    if (GetLexer() == iLexerF) {
        return;
    }

    if (CodeEdit::isLexerKnown(iLexerF) == false) {
        return;
    }

    // Enforce lexer only for unknown file types
    if (isExtKnown()) {
        return;
    }

    initLexer(iLexerF, m_iLexerType);

    if ((wxSTC_LEX_LATEX == iLexerF) && (false == m_ScintillaPrefs.common.wrapModeInitial) && (false == m_ScintillaPrefs.loaded)) {
        m_ScintillaPrefs.common.wrapModeInitial = true;
        DoWrapMode(m_ScintillaPrefs.common.wrapModeInitial);
    }

    if (m_bEnableCodeAnalyzer && m_pCodeAnalyzer) {
        switch (iLexerF) {
            case wxSTC_LEX_LUA:
                m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_LUA);
                break;
            case wxSTC_LEX_PYTHON:
                m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_PYTHON);
                break;
            case wxSTC_LEX_LATEX:
                m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_LATEX);
                break;
            case wxSTC_LEX_CPP:
                if (m_iLexerType == LEXERTYPE_FIRST) {
                    m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_C);
                }
                break;
            case wxSTC_LEX_SOLIS:
                if (LEXERTYPE_FIRST == m_iLexerType) {
                    m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_SOLIS);
                }
                break;
            default:
                break;
        }
    }

    DoHighlightSyntax(m_ScintillaPrefs.common.syntaxEnable);
    m_bLexerEnforced = true;

    if ((this->GetFilename().IsEmpty()) || (::wxFileExists(this->GetFilename()) == false)) {
        wxString strT = SCRIPT_DEFAULT_TITLE;
        switch (iLexerF) {
            case wxSTC_LEX_LUA:
                strT = uT("Lua");
                break;
            case wxSTC_LEX_CPP:
                strT = (m_iLexerType == LEXERTYPE_FIRST) ? uT("C") : uT("C++");
                break;
            case wxSTC_LEX_PYTHON:
                strT = uT("Python");
                break;
            case wxSTC_LEX_FORTRAN:
                strT = uT("Fortran");
                break;
            case wxSTC_LEX_MAKEFILE:
                strT = uT("Makefile");
                break;
            case wxSTC_LEX_LATEX:
                strT = (LEXERTYPE_FIRST == m_iLexerType) ? uT("LaTeX") : uT("BibTeX");
                break;
            case wxSTC_LEX_COMET:
                strT = uT("Comet");
                break;
            case wxSTC_LEX_BATCH:
                strT = uT("Batch");
                break;
            case wxSTC_LEX_SOLIS:
                strT = (LEXERTYPE_THIRD == m_iLexerType) ? uT("Settings") : ((LEXERTYPE_FOURTH == m_iLexerType) ? uT("UserSyntax") : uT("Solis"));
                strT = uT("Solis");
                break;
            default:
                break;
        }
        updatePage(strT);
    }

    this->updateStatusbar(true);
}

bool ScriptEdit::FileLoaded(const wxString &filenameT, int maxLineIndex /* = -1*/, bool bReload /* = false*/, int iSelStart /* = 0*/, int iSelEnd /* = 0*/, bool bConverted /* = false*/, bool bSelect /* = true*/, bool bFocus /* = true*/)
{
    if (updateFilename(filenameT, bSelect, bFocus) == false) {
        return false;
    }

    wxFileName fname = this->GetFilename();

    wxString strExt = fname.GetExt();
    wxString strShortFilename = fname.GetFullName();
    int iLexer = GetLexer();
    int iLexerType = LEXERTYPE_FIRST;
    if (iLexer == wxSTC_LEX_NULL) {
        iLexer = lexerFromExtension(strExt, strShortFilename);
    }

    if (iLexer == wxSTC_LEX_CPP) {
        iLexerType = strExt.IsSameAs(uT("c")) ? LEXERTYPE_FIRST : LEXERTYPE_SECOND;
    }
    else if (iLexer == wxSTC_LEX_LATEX) {
        iLexerType = strExt.IsSameAs(uT("bib")) ? LEXERTYPE_SECOND : LEXERTYPE_FIRST;
    }
    else if (iLexer == wxSTC_LEX_SOLIS) {
        iLexerType = strExt.IsSameAs(uT("solis")) ? LEXERTYPE_FIRST : (strExt.IsSameAs(uT("cometr")) ? LEXERTYPE_THIRD : LEXERTYPE_FOURTH);
    }

    if ((iLexer == 0) && (strExt.IsSameAs(uT("cls")) || strExt.IsSameAs(uT("clo")) || strExt.IsSameAs(uT("sty")))) {
        // LaTeX class file?
        FILE *fpT = Tfopen(LM_CSTR(filenameT), uT("rt"));
        if (fpT != NULL) {
            char szLineA[LM_STRSIZET];
            memset(szLineA, 0, LM_STRSIZET * sizeof(char));
            char *pszT = fgets(szLineA, LM_STRSIZET - 1, fpT);
            if (pszT != NULL) {
                if ((szLineA[0] == '%') && (szLineA[1] == '%')) {
                    iLexer = wxSTC_LEX_LATEX;
                }
            }
            fclose(fpT);
            fpT = NULL;
        }
        //
    }

    initLexer(iLexer, iLexerType);

    if ((wxSTC_LEX_LATEX == iLexer) && (false == m_ScintillaPrefs.common.wrapModeInitial) && (false == m_ScintillaPrefs.loaded)) {
        m_ScintillaPrefs.common.wrapModeInitial = true;
        DoWrapMode(m_ScintillaPrefs.common.wrapModeInitial);
    }

    DoHighlightSyntax(m_ScintillaPrefs.common.syntaxEnable);

    calcLinenumberMargin();
    if (maxLineIndex >= 0) {
        wxString strT = GetLine(maxLineIndex);
        strT += _T("___");
        int iW = TextWidth(wxSTC_STYLE_DEFAULT, strT);
        setMaxLineWidth(iW + 20);
        SetScrollWidth(getMaxLineWidth());
        this->Refresh();
    }

    if (bReload == false) {
        if (bFocus) {
            this->SetFocus();
        }
        this->EnsureVisible(this->LineFromPosition(iSelStart));
        this->SetSelection(iSelStart, iSelEnd);
    }

    if (iLexer != wxSTC_LEX_LUA) {
        MarkerDeleteAll(SCRIPT_MASK_BREAKPOINTBIT);
    }

    if ((iLexer == wxSTC_LEX_LUA) || (iLexer == wxSTC_LEX_PYTHON) || (iLexer == wxSTC_LEX_LATEX) || (iLexer == wxSTC_LEX_CPP) || ((iLexer == wxSTC_LEX_SOLIS) && (LEXERTYPE_FIRST == m_iLexerType))) {

        if (m_bEnableCodeAnalyzer && m_pCodeAnalyzer) {

            SigmaBusyCursor waitC;
            if (this->GetLineCount() > 2048) {
                waitC.start();
            }

            m_pCodeAnalyzer->reset();
            m_pCodeAnalyzer->setFilename(this->GetFilename());
            switch (iLexer) {
                case wxSTC_LEX_LUA:
                    m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_LUA);
                    break;
                case wxSTC_LEX_PYTHON:
                    m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_PYTHON);
                    break;
                case wxSTC_LEX_LATEX:
                    m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_LATEX);
                    break;
                case wxSTC_LEX_CPP:
                    if (iLexerType == 1) {
                        m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_C);
                    }
                    break;
                case wxSTC_LEX_SOLIS:
                    if (LEXERTYPE_FIRST == m_iLexerType) {
                        m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_SOLIS);
                    }
                    break;
                default:
                    break;
            }

            if (bConverted == false) {
                m_pCodeAnalyzer->analyze();
                int iMainLine = m_pCodeAnalyzer->getMainLine();
                if (iMainLine >= 0) {
                    DoGotoLine(iMainLine, bSelect);
                }
            }
        }
    }

    return true;
}

bool ScriptEdit::DoLoadFile(const wxString &filenameT /* = wxEmptyString*/, bool bRun /* = false*/,
                            bool bReload /* = false*/, bool bOpenRecent /* = false*/, bool bSelect /* = true*/, bool bFocus /* = true*/)
{
    if (filenameT.IsEmpty()) {
        return false;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return false;
    }

    wxFile fileT(filenameT);
    if (fileT.IsOpened() == false) {
        return false;
    }
    const wxFileOffset iFileSize = (const wxFileOffset)(fileT.Length());
    if (iFileSize < 0L) { // Should never happen
        fileT.Close();
        return false;
    }
    if (iFileSize > LF_SCRIPT_MAXCHARS) {
        fileT.Close();
        pFrame->OutputStatusbar(uT("Cannot load file: invalid size (max: 256 MB)"), SIGMAFRAME_TIMER_SHORT);
        return false;
    }
    fileT.Close();

    if (bOpenRecent) {
        if (iFileSize > LF_SCRIPT_MAXCHARS_SM) {
            // Avoid to automatically reopen big files at startup. User can always reopen them manually.
            return false;
        }
    }

    if (iFileSize < 1L) { // Load empty file
        return FileLoaded(filenameT, -1, bReload, 0, 0, false, bSelect, bFocus);
    }

    FILE *fpScript = NULL;
    const char_t *pszFilename = LM_CSTR(filenameT);

    m_bLoading = true;

    wxString strT;

    int ii = 0;

    unsigned char *pszBufferA = NULL;

    bool bConverted = false;

    try {

        wxBusyCursor waitC;

        fpScript = Tfopen(pszFilename, uT("rb"));

        if (fpScript == NULL) {
            pFrame->OutputStatusbar(uT("Cannot load file: open failed"), SIGMAFRAME_TIMER_SHORT);
            m_bLoading = false;
            return false;
        }

        const size_t iFileSizeMax = (const size_t)(iFileSize) + 1;
        pszBufferA = (unsigned char *)malloc(iFileSizeMax * sizeof(unsigned char));
        if (pszBufferA == NULL) {
            fclose(fpScript);
            fpScript = NULL;
            m_bLoading = false;
            pFrame->OutputStatusbar(uT("Cannot load file: insufficient memory"), SIGMAFRAME_TIMER_SHORT);
            return false;
        }

        pszBufferA[iFileSizeMax - 1] = 0;

        if (fread(pszBufferA, iFileSize, 1, fpScript) != 1) {
            free(pszBufferA);
            pszBufferA = NULL;
            fclose(fpScript);
            fpScript = NULL;
            pFrame->OutputStatusbar(uT("Cannot load file: read failed"), SIGMAFRAME_TIMER_SHORT);
            ClearAll();
            m_bLoading = false;
            return false;
        }

        fclose(fpScript);
        fpScript = NULL;

        // Check for Unicode Encoding
        bool bBigEndian = false;
        bool bUTF8 = false, bUTF32 = false, bUTF16 = false;
        bool bUTF8BOM = false, bUTF32BOM = false, bUTF16BOM = false;

        // Chech UTF-8 first (otherwise UTF-8 can be detected as UTF-16 or UTF-32), ...
        // ... then check for UTF-32 before UTF-16 (otherwise UTF-32 can be detected as UTF-16)
        // NB: All UTF detection algorithms should be always right!

        // UTF-8 (internal encoding format)
        if ((bUTF8 = (ScriptEdit::isUTF8(pszBufferA, iFileSize, &bUTF8BOM))) == true) {
            uint8_t *pszBufferRaw = bUTF8BOM ? (pszBufferA + 3) : pszBufferA;
            this->SetTextRaw((const char *)(pszBufferRaw));

            // Disable conversion options
            m_bUTF8done[UTF8FROM_CP1252] = true;
            m_bUTF8done[UTF8FROM_ISO8859L1] = true;
            m_bUTF8done[UTF8FROM_ISO8859L9] = true;
        }

        // UTF-32 or UTF-16
        else if (((bUTF32 = ScriptEdit::isUTF32(pszBufferA, iFileSize, &bBigEndian, &bUTF32BOM)) == true) || ((bUTF16 = ScriptEdit::isUTF16(pszBufferA, iFileSize, &bBigEndian, &bUTF16BOM)) == true)) {

            const size_t iFileSizeMaxUTF8 = bUTF32 ? ((const size_t)(iFileSize) + 1) : ((const size_t)(iFileSize << 1) + 1);
            uint8_t *pszBufferUTF8 = (uint8_t *)malloc(iFileSizeMaxUTF8 * sizeof(uint8_t));
            if (pszBufferUTF8 == NULL) {
                free(pszBufferA);
                pszBufferA = NULL;
                m_bLoading = false;
                pFrame->OutputStatusbar(uT("Cannot load file: insufficient memory"), SIGMAFRAME_TIMER_SHORT);
                return false;
            }

            pszBufferUTF8[iFileSizeMaxUTF8 - 1] = 0;

            if (bUTF32) {
                uint8_t *pszBufferRaw = bUTF32BOM ? (pszBufferA + 4) : pszBufferA;
                bool bCheck = true;
                size_t sizeRet = 0;
                ScriptEdit::convertFromWC(pszBufferUTF8, (wchar_t *)(pszBufferRaw), iFileSizeMaxUTF8 - 1, bUTF32BOM ? (iFileSize - 4) : iFileSize, &sizeRet, &bCheck);
                if ((false == bCheck) || (sizeRet < (size_t)(iFileSize / 4))) {
                    ScriptEdit::convertFromUTF32(pszBufferUTF8, pszBufferRaw, iFileSizeMaxUTF8 - 1, bUTF32BOM ? (iFileSize - 4) : iFileSize, bBigEndian);
                }
            }
            else if (bUTF16) {
                uint8_t *pszBufferRaw = bUTF16BOM ? (pszBufferA + 2) : pszBufferA;
                ScriptEdit::convertFromUTF16(pszBufferUTF8, pszBufferRaw, iFileSizeMaxUTF8 - 1, bUTF16BOM ? (iFileSize - 2) : iFileSize, bBigEndian);
            }

            this->SetTextRaw((const char *)(pszBufferUTF8));

            free(pszBufferUTF8);
            pszBufferUTF8 = NULL;

            // Disable conversion options
            m_bUTF8done[UTF8FROM_CP1252] = true;
            m_bUTF8done[UTF8FROM_ISO8859L1] = true;
            m_bUTF8done[UTF8FROM_ISO8859L9] = true;
        }

        // No UTF encoding found
        else {

            this->SetTextRaw((const char *)(pszBufferA));

            // Windows-1252? (algorithm need to be improved)
            if (ScriptEdit::isCP1252(pszBufferA, iFileSize)) {
                const size_t iFileSizeMaxUTF8 = ((const size_t)(iFileSize << 2) + 1);
                uint8_t *pszBufferUTF8 = (uint8_t *)malloc(iFileSizeMaxUTF8 * sizeof(uint8_t));
                if (pszBufferUTF8 == NULL) {
                    free(pszBufferA);
                    pszBufferA = NULL;
                    m_bLoading = false;
                    pFrame->OutputStatusbar(uT("Cannot load file: insufficient memory"), SIGMAFRAME_TIMER_SHORT);
                    return false;
                }

                bool bret = convertFromCP1252(pszBufferUTF8, iFileSizeMaxUTF8);

                if (bret) {
                    resetModified();
                    bConverted = true;
                    this->SetTextRaw((const char *)(pszBufferUTF8));
                    m_bUTF8done[UTF8FROM_CP1252] = true;
                }

                free(pszBufferUTF8);
                pszBufferUTF8 = NULL;
            }

            // Unknown format
            else {
                // Enable conversion options and warn user
                m_bUTF8done[UTF8FROM_CP1252] = false;
                m_bUTF8done[UTF8FROM_ISO8859L1] = false;
                m_bUTF8done[UTF8FROM_ISO8859L9] = false;
                wxString strMsg(uT("'"));
                wxFileName fname(filenameT);
                strMsg += fname.GetFullName();
                strMsg += uT("' unknown encoding. Use 'Edit/Convert To UTF-8' menu to convert it.");
                pFrame->OutputStatusbar(strMsg, SIGMAFRAME_TIMER_LONG);
            }
        }

        int nLinesMin = wxMin(GetLineCount(), LF_SCRIPT_MAXLINES_SM);

        /* calculate the longuest visible line and detect indentation */
        int tabCount = 0;
        int spaceCount = 0;
        const int arTabSize[4] = { 3, 4, 6, 8 };
        int arTabSizeCount[4];
        arTabSizeCount[0] = 0;
        arTabSizeCount[1] = 0;
        arTabSizeCount[2] = 0;
        arTabSizeCount[3] = 0;
        int iSp = 0, iSpm = 0;
        int maxLineIndex = 0, iLineLen = 0, iLineLenMax = 0;
        char *pszTA = (char *)pszBufferA;
        char *pszSA = pszTA;
        for (ii = 0; ii < nLinesMin; ii++) {
            pszSA = strstr(pszTA, "\n");
            if (NULL == pszSA) {
                break;
            }
            *pszSA = '\0';
            iLineLen = (int)strlen((const char *)(pszTA));
            if (iLineLen > iLineLenMax) {
                iLineLenMax = iLineLen;
                maxLineIndex = ii;
            }

            if (*pszTA == '\t') {
                ++tabCount;
            }
            else {
                iSpm = iLineLen;
                if (iSpm > 32) {
                    iSpm = 32;
                }
                iSp = 0;
                for (int ss = 0; (ss < iSpm) && ((pszTA[ss] == ' ') || (pszTA[ss] == '\t')); ss++) {
                    if (pszTA[ss] == '\t') {
                        ++tabCount;
                        break;
                    }
                    ++iSp;
                }
                if (iSp >= 3) {
                    if ((iSp % arTabSize[0]) == 0) {
                        arTabSizeCount[0] += 1;
                    }
                    else if ((iSp % arTabSize[1]) == 0) {
                        arTabSizeCount[1] += 1;
                    }
                    else if ((iSp % arTabSize[2]) == 0) {
                        arTabSizeCount[2] += 1;
                    }
                    else if ((iSp % arTabSize[3]) == 0) {
                        arTabSizeCount[3] += 1;
                    }
                    ++spaceCount;
                }
            }

            pszTA = pszSA + 1;
        }

        if (spaceCount > tabCount) {
            int ssm = 0;
            int stm = arTabSizeCount[0];
            for (int ss = 1; ss < 4; ss++) {
                if (arTabSizeCount[ss] > stm) {
                    stm = arTabSizeCount[ss];
                    ssm = ss;
                }
            }
            if (m_ScintillaPrefs.common.useTab || (m_ScintillaPrefs.common.tabSize != arTabSize[ssm])) {
                DoUseTab(false);
                DoSetTabSize(arTabSize[ssm]);
                DoIndentGuide(m_ScintillaPrefs.common.indentGuideEnable);
                DoSetLongLine(m_ScintillaPrefs.common.longLine);
            }
        }

        free(pszBufferA);
        pszBufferA = NULL;

        int iSelStart = 0, iSelEnd = 0;
        wxString strLine;

        /* load navigation info */
        if (pFrame->saveNavigation() && (iFileSize <= LF_SCRIPT_MAXCHARS_SM) && (nLinesMin > 1)) {

            char_t szFilenameMarker[LM_STRSIZE];
            Tmemset(szFilenameMarker, 0, LM_STRSIZE);
            wxStrncpy(szFilenameMarker, pszFilename, LM_STRSIZE - 1);
            int iLen = (int)wxStrlen(static_cast<const char_t *>(szFilenameMarker));
            bool bGetPar = false;
            if ((iLen > 3) && (iLen < (LM_STRSIZE - 9))) {
                for (ii = iLen - 1; ii >= 0; ii--) {
                    if (szFilenameMarker[ii] == uT('.')) {
                        szFilenameMarker[ii + 1] = uT('c');
                        szFilenameMarker[ii + 2] = uT('o');
                        szFilenameMarker[ii + 3] = uT('m');
                        szFilenameMarker[ii + 4] = uT('e');
                        szFilenameMarker[ii + 5] = uT('t');
                        szFilenameMarker[ii + 6] = uT('m');
                        szFilenameMarker[ii + 7] = uT('\0');
                        bGetPar = true;
                        break;
                    }
                }
            }

            if (bGetPar && filenameT.IsSameAs(static_cast<const char_t *>(szFilenameMarker), false)) {
                bGetPar = false;
            }

            // >> BINARY FILE with byte datatype --> non big- little-endian problem between Windows and Linux
            lmByte *pbMarkers = NULL;
            if (bGetPar) {
                pbMarkers = (lmByte *)malloc(nLinesMin * sizeof(lmByte));
            }
            if (pbMarkers == NULL) {
                bGetPar = false;
            }

            m_iBookmarkCount = 0;
            m_iBreakpointCount = 0;

            pFrame->deleteBookmark(filenameT, -1, m_Id);

            if (bGetPar) {

                FILE *fpMarker = NULL;
                if ((fpMarker = Tfopen(static_cast<const char_t *>(szFilenameMarker), uT("rb"))) != NULL) {

                    lmByte bHeader, bR, bG, bB;

                    fread(&bHeader, sizeof(lmByte), 1, fpMarker);

                    if (bHeader == FILE_HEADER_COMETM) {

                        for (int rr = 0; rr < 2; rr++) {

                            fread(&bHeader, sizeof(lmByte), 1, fpMarker);

                            if (bHeader == FILE_HEADER_MARKERS) {
                                fread(&bB, sizeof(lmByte), 1, fpMarker);
                                fread(&bG, sizeof(lmByte), 1, fpMarker);
                                fread(&bR, sizeof(lmByte), 1, fpMarker);

                                int ict = (int)LM_COLOR(bR, bG, bB);
                                if ((ict == nLinesMin) && pbMarkers) {
                                    int iret = (int)fread(pbMarkers, nLinesMin * sizeof(lmByte), 1, fpMarker);
                                    if (iret == 1) {
                                        for (ii = 0; ii < nLinesMin; ii++) {
                                            if ((pbMarkers[ii] & 0x01) == 0x01) {
                                                MarkerAdd(ii, SCRIPT_MASK_BOOKMARKBIT);
                                                if (pFrame->canAddBookmark()) {
                                                    strLine = this->GetLine(ii).Trim(false);
                                                    pFrame->addBookmark(filenameT, ii, strLine, m_Id);
                                                }
                                                m_iBookmarkCount += 1;
                                            }
                                            if ((pbMarkers[ii] & 0x02) == 0x02) {
                                                MarkerAdd(ii, SCRIPT_MASK_BREAKPOINTBIT);
                                                m_iBreakpointCount += 1;
                                            }
                                            if ((pbMarkers[ii] & 0x04) == 0x04) {
                                                ToggleFold(ii);
                                            }
                                            if ((pbMarkers[ii] & 0x08) == 0x08) {
                                                MarkerAdd(ii, SCRIPT_MASK_SAVEDBIT);
                                            }
                                            if ((pbMarkers[ii] & 0x10) == 0x10) {
                                                DoAddStatus(ii, SCRIPT_MASK_FINDBIT);
                                            }
                                        }
                                    }
                                }
                            }

                            else if (bHeader == FILE_HEADER_SELECTION) {
                                // Cursor position
                                fread(&bB, sizeof(lmByte), 1, fpMarker);
                                fread(&bG, sizeof(lmByte), 1, fpMarker);
                                fread(&bR, sizeof(lmByte), 1, fpMarker);
                                iSelStart = (int)LM_COLOR(bR, bG, bB);
                                if ((iSelStart < 0) || (iSelStart >= this->GetLength())) {
                                    iSelStart = 0;
                                }
                                fread(&bB, sizeof(lmByte), 1, fpMarker);
                                fread(&bG, sizeof(lmByte), 1, fpMarker);
                                fread(&bR, sizeof(lmByte), 1, fpMarker);
                                iSelEnd = (int)LM_COLOR(bR, bG, bB);
                                if ((iSelEnd < iSelStart) || (iSelEnd >= this->GetLength())) {
                                    iSelEnd = iSelStart;
                                }
                            }
                        }
                    }

                    fclose(fpMarker);
                    fpMarker = NULL;
                }
            }

            if (pbMarkers) {
                free(pbMarkers);
                pbMarkers = NULL;
            }
        }
        /* load navigation info */

        FileLoaded(filenameT, maxLineIndex, bReload, iSelStart, iSelEnd, bConverted, bSelect, bFocus);

        if (bRun) {
            luaRunScript();
        }
    }
    catch (...) {
        ;
    }

    // detect line-endings
    int iCRLFcount = 0;
    int iLFcount = 0;
    int iCRcount = 0;
    int iDocLength = wxMin(this->GetLength() - 1, LF_SCRIPT_MAXLINES_SM * LM_STRSIZET);
    if (iDocLength > 1) {
        char cPrev = ' ';
        char cT;
        char cNext = this->GetCharAt(0);
        for (int ii = 0; ii < iDocLength; ii++) {
            if ((iCRLFcount >= 5) || (iLFcount >= 5) || (iCRcount >= 5)) {
                break;
            }
            cT = cNext;
            cNext = this->GetCharAt(ii + 1);
            if (cT == '\r') {
                if (cNext == '\n') {
                    iCRLFcount++;
                }
                else {
                    iCRcount++;
                }
            }
            else if (cT == '\n') {
                if (cPrev != '\r') {
                    iLFcount++;
                }
            }
            cPrev = cT;
        }
    }
    int iEOL = wxSTC_EOL_CRLF;
    if ((iLFcount > iCRLFcount) && (iLFcount > iCRcount)) {
        iEOL = wxSTC_EOL_LF;
        if ((iCRLFcount > 0) || (iCRcount > 0)) {
            pFrame->OutputStatusbar(uT("Mixed line-endings found: normalized to LF"), SIGMAFRAME_TIMER_SHORT);
        }
    }
    else if ((iCRcount > iLFcount) && (iCRcount > iCRLFcount)) {
        iEOL = wxSTC_EOL_CR;
        if ((iLFcount > 0) || (iCRLFcount > 0)) {
            pFrame->OutputStatusbar(uT("Mixed line-endings found: normalized to CR"), SIGMAFRAME_TIMER_SHORT);
        }
    }
    else {
        if ((iLFcount > 0) || (iCRcount > 0)) {
            pFrame->OutputStatusbar(uT("Mixed line-endings found: normalized to CRLF"), SIGMAFRAME_TIMER_SHORT);
        }
    }
    ConvertEOLs(iEOL);
    SetEOLMode(iEOL);
    //

    if (bConverted == false) {
        resetModified();
        pFrame->DoAnalyzerUpdate(true, false);
    }

    m_bLoading = false;
    if (bConverted) {
        DoSetModified();
    }

    pFrame->updateEditorStatus(this);

    return true;
}

bool ScriptEdit::DoSaveFile(const wxString &filenameT /* = wxEmptyString*/, bool bSelect /* = true*/)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return false;
    }

    int iLen = GetTextLength();
    if (iLen < 1) {
        pFrame->OutputStatusbar(uT("Cannot save file: empty document"), SIGMAFRAME_TIMER_SHORT);
        return false;
    }
    if (iLen > LF_SCRIPT_MAXCHARS) {
        pFrame->OutputStatusbar(uT("Cannot save file: document size > 256 MB"), SIGMAFRAME_TIMER_SHORT);
        return false;
    }

    wxString strFilename = filenameT;

    bool bAddToRecent = false;

    bool bUpdateFilename = false;

    if (strFilename.IsEmpty() == false) {
        if (autoReload()) {
            int iretx = SigmaMessageBox(uT("You have choosen to automatically reload the document.\nRevert to manual reload and save?"), uT("Comet"),
                                        wxYES | wxNO | wxICON_QUESTION | wxNO_DEFAULT, this);
            if (iretx != wxYES) {
                return false;
            }
            resetTimerReload();
            pFrame->updateEditorStatus(this);
        }
        else {
            if (DoReload(false, false)) {
                return true;
            }
        }
    }

    wxString strT;
    wxString strFullPathOld = wxEmptyString;
    wxString strDirOld = wxEmptyString;
    wxString strFilenameOld = wxEmptyString;
    wxString strExtOld = wxEmptyString;
    wxString strFullPathNew = wxEmptyString;
    wxString strDirNew = wxEmptyString;
    wxString strFilenameNew = wxEmptyString;
    wxString strExtNew = wxEmptyString;

    if (m_strFilename.IsEmpty() == false) {
        wxFileName fname = m_strFilename;
        strFullPathOld = fname.GetFullPath();
        strFilenameOld = fname.GetFullName();
        strDirOld = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
        strExtOld = fname.GetExt();
    }

    int nLines = GetLineCount();
    if (nLines < 1) {
        // Should never happen
        return false;
    }
    if (nLines > LF_SCRIPT_MAXLINES) {
        strT = uT("Cannot save document: invalid number of lines (max: 262144)");
        pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        SigmaMessageBox(strT, uT("Comet"), wxICON_ERROR | wxOK, this);
        return false;
    }

    bool bUpdateBookmark = false;

    if (strFilename.IsEmpty()) {

        bUpdateFilename = true;

        wxFileName fname;
        int filterIndex = FILE_FILTER_KNOWN;

        wxString strDir = wxEmptyString;

        wxString strExt = wxEmptyString;
        if (m_strFilename.IsEmpty() == false) {
            fname = m_strFilename;
            strFilename = fname.GetFullName();
            strDir = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
            strExt = fname.GetExt();
        }

        switch (GetLexer()) {
            case wxSTC_LEX_LUA:
                filterIndex = FILE_FILTER_LUA;
                break;
            case wxSTC_LEX_CPP:
                filterIndex = FILE_FILTER_CPP;
                break;
            case wxSTC_LEX_BASH:
                filterIndex = FILE_FILTER_BASH;
                break;
            case wxSTC_LEX_PYTHON:
                filterIndex = FILE_FILTER_PYTHON;
                break;
            case wxSTC_LEX_OCTAVE:
                filterIndex = FILE_FILTER_OCTAVE;
                break;
            case wxSTC_LEX_FORTRAN:
                filterIndex = FILE_FILTER_FORTRAN;
                break;
            case wxSTC_LEX_LATEX:
                filterIndex = FILE_FILTER_LATEX;
                break;
            case wxSTC_LEX_MAKEFILE:
                filterIndex = FILE_FILTER_MAKEFILE;
                strFilename = uT("Makefile");
                break;
            case wxSTC_LEX_BATCH:
                filterIndex = FILE_FILTER_BATCH;
                break;
            case wxSTC_LEX_SOLIS:
                filterIndex = (LEXERTYPE_FIRST == m_iLexerType) ? FILE_FILTER_SOLIS : FILE_FILTER_SETTINGS;
                break;
            case wxSTC_LEX_COMET:
                filterIndex = FILE_FILTER_COMET;
                break;
            default:
                break;
        }

        char_t szFilters[LM_STRSIZEW];
        Tmemset(szFilters, 0, LM_STRSIZEW);
        Tstrcpy(szFilters,
            uT("Lua files|*.lua;*.comet|")
            uT("C/C++ files|*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.rc|")
            uT("Text/Shell files|*.txt;*.dat;*.csv;*.log;*.conf;*.cometo;*.bat;*.sh;*.csh;*.ins;*.bst;*.patch|")
            uT("Python files|*.py;*.pyw;*.sage;*.sagews|")
            uT("Octave files|*.m;*.mat|")
            uT("Fortran files|*.f;*.for;*.f90;*.f95;*.f03;*.f08;*.f15|")
            uT("LaTeX files|*.tex;*.bib;*.cls;*.sty|")
            uT("Makefile|Makefile;makefile|"));
        Tstrcat(szFilters,
            uT("Batch files|*.bat|")
            uT("Solis files|*.solis|")
            uT("Comet settings files|*.cometr;*.cometu|"));
        Tstrcat(szFilters,
                uT("User-defined files|*."));
        Tstrcat(szFilters,
                CodeEdit::getCometLexerExt());
        Tstrcat(szFilters,
                uT("|"));
        Tstrcat(szFilters,
            uT("All supported files|*.lua;*.comet;*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.rc;")
            uT("*.txt;*.dat;*.csv;*.conf;*.cometo;*.bat;*.sh;*.csh;*.ins;*.bst;")
            uT("*.bib;*.patch;*.py;*.pyw;*.m;*.mat;*.f;*.for;*.f90;*.f95;*.f03;*.f08;*.f15;*.tex;Makefile;makefile;*.solis;*.cometr;*.cometu;*."));
        Tstrcat(szFilters,
                CodeEdit::getCometLexerExt());
        Tstrcat(szFilters,
                uT("|All files|*"));

        wxFileDialog *pDlg = new (std::nothrow) wxFileDialog(this, uT("Save file"), strDir, strFilename,
                                                             static_cast<const char_t *>(szFilters),
                                                             wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (pDlg == NULL) {
            return false;
        }
        pDlg->SetFilterIndex(filterIndex);
        if (pDlg->ShowModal() != wxID_OK) {
            pDlg->Destroy();
            return false;
        }

        strFilename = pDlg->GetPath();
        int iLen = (int)(strFilename.Length());
        if (iLen < 1) {
            pDlg->Destroy();
            return false;
        }
        pDlg->Destroy();
        bAddToRecent = true;

        filterIndex = pDlg->GetFilterIndex();

        if ((filterIndex != FILE_FILTER_MAKEFILE) && (filterIndex != FILE_FILTER_ALL)) {
            fname = strFilename;
            wxString strExt = fname.GetExt();
            if (strExt.IsEmpty()) {
                switch (filterIndex) {
                    case FILE_FILTER_LUA:
                        strFilename += uT(".lua");
                        break;
                    case FILE_FILTER_CPP:
                        strFilename += uT(".c");
                        break;
                    case FILE_FILTER_BASH:
                        strFilename += uT(".sh");
                        break;
                    case FILE_FILTER_PYTHON:
                        strFilename += uT(".py");
                        break;
                    case FILE_FILTER_OCTAVE:
                        strFilename += uT(".m");
                        break;
                    case FILE_FILTER_FORTRAN:
                        strFilename += uT(".for");
                        break;
                    case FILE_FILTER_LATEX:
                        strFilename += uT(".tex");
                        break;
                    case FILE_FILTER_BATCH:
                        strFilename += uT(".bat");
                        break;
                    case FILE_FILTER_SOLIS:
                        strFilename += uT(".solis");
                        break;
                    case FILE_FILTER_SETTINGS:
                        strFilename += (m_iLexerType == LEXERTYPE_THIRD) ? uT(".cometr") : uT(".cometu");
                        break;
                    case FILE_FILTER_COMET:
                        strFilename += uT(".");
                        strFilename += CodeEdit::getCometLexerExt();
                        break;
                    default:
                        break;
                }
            }
        }

        if (pFrame->isFileOpened(strFilename)) {
            strT = uT("'");
            strT += CometFrame::getLabelFromPath(strFilename);
            strT += uT("' already open in Comet");

            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
            SigmaMessageBox(strT, uT("Comet"), wxOK | wxICON_INFORMATION, this);
            return false;
        }

        bUpdateBookmark = true;
    }

    if (strFilename.IsEmpty() == false) {
        wxFileName fname = strFilename;
        strFullPathNew = fname.GetFullPath();
        strFilenameNew = fname.GetFullName();
        strDirNew = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
        strExtNew = fname.GetExt();
    }

    FILE *fpMarker = NULL;

    try {

        wxBusyCursor waitC;

        // Save as raw binary (not-supported characters (e.g. arabic) cause the standard Save routine failure)
        bool bSaved = false;
        FILE *fpRaw = Tfopen(LM_CSTR(strFilename), uT("wb"));
        if (fpRaw != NULL) {
            wxCharBuffer strBuffer = this->GetTextRaw();
            // GetLength and GetTextLength give the same value i.e. the number of bytes
            long iBufferSize = (long)(this->GetTextLength());

            bSaved = (fwrite((const char *)(strBuffer.data()), iBufferSize * sizeof(char), 1, fpRaw) == 1);

            fclose(fpRaw);
            fpRaw = NULL;
        }

        if (bSaved == false) {
            strT = uT("Cannot save '");
            strT += CometFrame::getLabelFromPath(this->GetFilename());
            if (lf_fileReadonly(LM_CSTR(this->GetFilename()))) {
                strT += uT("'.\nRead-only file.");
            }
            else {
                strT += uT("'.\nCheck file content or permissions.");
            }

            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
            SigmaMessageBox(strT, uT("Comet"), wxICON_ERROR | wxOK, this);
            return false;
        }

        SetSavePoint();
        //

        bool bSavePar = false;

        int ii = 0, nLinesMin = wxMin(nLines, LF_SCRIPT_MAXLINES);

        // >> BINARY FILE containing bytes --> non big- little-endian problem between Windows and Linux
        char_t szFilenameMarker[LM_STRSIZE];
        memset(szFilenameMarker, 0, LM_STRSIZE * sizeof(char_t));

        if (pFrame->saveNavigation()) {
            wxStrncpy(szFilenameMarker, LM_CSTR(strFilename), LM_STRSIZE - 1);
            int iLen = (int)wxStrlen(static_cast<const char_t *>(szFilenameMarker));
            if ((iLen > 3) && (iLen < (LM_STRSIZE - 9))) {
                for (ii = iLen - 1; ii >= 0; ii--) {
                    if (szFilenameMarker[ii] == uT('.')) {
                        szFilenameMarker[ii + 1] = uT('c');
                        szFilenameMarker[ii + 2] = uT('o');
                        szFilenameMarker[ii + 3] = uT('m');
                        szFilenameMarker[ii + 4] = uT('e');
                        szFilenameMarker[ii + 5] = uT('t');
                        szFilenameMarker[ii + 6] = uT('m');
                        szFilenameMarker[ii + 7] = uT('\0');
                        bSavePar = true;
                        break;
                    }
                }
            }
        }
        // <<

        if (strFilename.IsSameAs(static_cast<const char_t *>(szFilenameMarker), false)) {
            bSavePar = false;
        }

        // file exists? correct format?
        if (bSavePar) {
            if ((fpMarker = Tfopen(static_cast<const char_t *>(szFilenameMarker), uT("rb"))) != NULL) {
                lmByte bHeader = 0x00;
                fread(&bHeader, sizeof(lmByte), 1, fpMarker);
                if (bHeader != FILE_HEADER_COMETM) {
                    bSavePar = false;
                }
                fclose(fpMarker);
                fpMarker = NULL;
            }
        }
        //

        lmByte *pbMarkers = NULL;
        if (bSavePar) {
            pbMarkers = (lmByte *)malloc(nLinesMin * sizeof(lmByte));
            if (pbMarkers == NULL) {
                bSavePar = false;
            }
        }

        int iMarkerCount = 0;

        for (ii = 0; ii < nLinesMin; ii++) {

            DoAddStatus(ii, SCRIPT_MASK_SAVEDBIT);

            if (bSavePar && (pbMarkers != NULL)) {
                pbMarkers[ii] = 0x00;
                if (MarkerGet(ii) & SCRIPT_MASK_BOOKMARK) {
                    pbMarkers[ii] |= 0x01;
                    iMarkerCount += 1;
                }
                if (MarkerGet(ii) & SCRIPT_MASK_BREAKPOINT) {
                    pbMarkers[ii] |= 0x02;
                    iMarkerCount += 1;
                }
                if ((GetFoldLevel(ii) & wxSTC_FOLDLEVELHEADERFLAG) && (GetFoldExpanded(ii) == false)) {
                    pbMarkers[ii] |= 0x04;
                    iMarkerCount += 1;
                }
                if ((MarkerGet(ii) & SCRIPT_MASK_SAVED) != 0) {
                    pbMarkers[ii] |= 0x08;
                    iMarkerCount += 1;
                }
                if ((MarkerGet(ii) & SCRIPT_MASK_FIND) != 0) {
                    pbMarkers[ii] |= 0x10;
                    iMarkerCount += 1;
                }
            }
        }

        MarkerDeleteAll(SCRIPT_MASK_MODIFIEDBIT);

        if (bSavePar) {

            if (iMarkerCount < 1) {
                Tunlink(static_cast<const char_t *>(szFilenameMarker));
            }

            else {

                if ((fpMarker = Tfopen(static_cast<const char_t *>(szFilenameMarker), uT("wb"))) != NULL) {

                    lmByte bR, bG, bB;

                    lmByte bHeader = FILE_HEADER_COMETM;
                    fwrite(&bHeader, sizeof(lmByte), 1, fpMarker);

                    if (pbMarkers) {
                        // Markers
                        bHeader = FILE_HEADER_MARKERS;
                        fwrite(&bHeader, sizeof(lmByte), 1, fpMarker);
                        bR = LM_COLORR(nLinesMin);
                        bG = LM_COLORG(nLinesMin);
                        bB = LM_COLORB(nLinesMin);
                        fwrite(&bB, sizeof(lmByte), 1, fpMarker);
                        fwrite(&bG, sizeof(lmByte), 1, fpMarker);
                        fwrite(&bR, sizeof(lmByte), 1, fpMarker);

                        fwrite(pbMarkers, nLinesMin * sizeof(lmByte), 1, fpMarker);
                    }

                    // Cursor
                    int iSelStart = this->GetSelectionStart(),
                        iSelEnd = this->GetSelectionEnd();
                    bHeader = FILE_HEADER_SELECTION;
                    fwrite(&bHeader, sizeof(lmByte), 1, fpMarker);

                    bR = LM_COLORR(iSelStart);
                    bG = LM_COLORG(iSelStart);
                    bB = LM_COLORB(iSelStart);
                    fwrite(&bB, sizeof(lmByte), 1, fpMarker);
                    fwrite(&bG, sizeof(lmByte), 1, fpMarker);
                    fwrite(&bR, sizeof(lmByte), 1, fpMarker);

                    bR = LM_COLORR(iSelEnd);
                    bG = LM_COLORG(iSelEnd);
                    bB = LM_COLORB(iSelEnd);
                    fwrite(&bB, sizeof(lmByte), 1, fpMarker);
                    fwrite(&bG, sizeof(lmByte), 1, fpMarker);
                    fwrite(&bR, sizeof(lmByte), 1, fpMarker);

                    fclose(fpMarker);
                    fpMarker = NULL;
                }
            }
        }

        if (pbMarkers) {
            free(pbMarkers);
            pbMarkers = NULL;
        }

        m_strFilename = strFilename;

        wxFileName fname = this->GetFilename();

        if (isExtKnown() || (m_bLexerEnforced == false)) {
            wxString strExt = fname.GetExt();
            wxString strShortFilename = fname.GetFullName();
            int iLexer = lexerFromExtension(strExt, strShortFilename);
            int iLexerType = LEXERTYPE_FIRST;
            if (wxSTC_LEX_CPP == iLexer) {
                iLexerType = strExt.IsSameAs(uT("c")) ? LEXERTYPE_FIRST : LEXERTYPE_SECOND;
            }
            else if (wxSTC_LEX_LATEX == iLexer) {
                iLexerType = strExt.IsSameAs(uT("bib")) ? LEXERTYPE_SECOND : LEXERTYPE_FIRST;
            }
            else if (wxSTC_LEX_SOLIS == iLexer) {
                iLexerType = strExt.IsSameAs(uT("solis")) ? LEXERTYPE_FIRST : (strExt.IsSameAs(uT("cometr")) ? LEXERTYPE_THIRD : LEXERTYPE_FOURTH);
            }

            initLexer(iLexer, iLexerType);

            if ((wxSTC_LEX_LATEX == iLexer) && (false == m_ScintillaPrefs.common.wrapModeInitial) && (false == m_ScintillaPrefs.loaded)) {
                m_ScintillaPrefs.common.wrapModeInitial = true;
                DoWrapMode(m_ScintillaPrefs.common.wrapModeInitial);
            }

            if (m_bEnableCodeAnalyzer && m_pCodeAnalyzer) {
                switch (iLexer) {
                    case wxSTC_LEX_LUA:
                        m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_LUA);
                        break;
                    case wxSTC_LEX_PYTHON:
                        m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_PYTHON);
                        break;
                    case wxSTC_LEX_LATEX:
                        m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_LATEX);
                        break;
                    case wxSTC_LEX_CPP:
                        if (iLexerType == 1) {
                            m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_C);
                        }
                        break;
                    case wxSTC_LEX_SOLIS:
                        if (LEXERTYPE_FIRST == m_iLexerType) {
                            m_pCodeAnalyzer->setLanguage(CodeAnalyzer::LANGUAGE_SOLIS);
                        }
                        break;
                    default:
                        break;
                }
            }
            DoHighlightSyntax(m_ScintillaPrefs.common.syntaxEnable);
        }

        wxString strT = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
        if (strT.Length() > 2) {
            // ::ANSI::    NOT::UNICODE::
            char szT[LM_STRSIZE];
            memset(szT, 0, LM_STRSIZE * sizeof(char));
            strcpy(szT, LM_U8STR(strT));
            chdir(szT);
            //
        }

        SetSavePoint();

        fname.GetTimes(NULL, &m_ChangeTime, NULL);

        if (bAddToRecent) {
            pFrame->addToRecent(m_strFilename);
        }

        pFrame->updateEditorStatus(this);
        wxAuiNotebook *pNotebook = (wxAuiNotebook *)(GetParent());
        int indexT = pNotebook->GetPageIndex(this);
        pNotebook->SetPageText(indexT, fname.GetFullName());
        pNotebook->SetPageToolTip(indexT, fname.GetFullPath());
        if (bSelect) {
            pNotebook->SetSelection(indexT);
        }

        if (bUpdateFilename) {
            pFrame->updateTitle(this->GetFilename());
            strT = uT("'");
            strT += fname.GetFullName();
            strT += uT("' saved");
            pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        }

        wxString strCurrentDir = pFrame->explorerGetCurrentDir();
        if ((strFullPathNew.IsSameAs(strFullPathOld) == false) && (strCurrentDir.IsSameAs(strDirNew))) {
            pFrame->DoUpdateExplorer();
        }

        if (bUpdateBookmark) {
            pFrame->updateBookmarks(m_Id, LM_CSTR(this->GetFilename()));
        }

        return true;
    }
    catch (...) {
        if (fpMarker) {
            fclose(fpMarker);
            fpMarker = NULL;
        }

        strT = uT("Cannot save '");
        strT += CometFrame::getLabelFromPath(LM_CSTR(strFilename));
        strT += uT("': unrecoverable error");

        pFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
        SigmaMessageBox(strT, uT("Comet"), wxICON_ERROR | wxOK, this);
        return false;
    }

    return true;
}

bool ScriptEdit::isModified(void)
{
    return (GetModify() && !GetReadOnly() && (GetLength() > 1));
}

void ScriptEdit::DoSetModified(int iLine /* = -1*/)
{
    if (m_bLoading) {
        return;
    }

    if (iLine < 0) {
        iLine = GetCurrentLine();
    }

    // Avoid multiple status update for the same line
    if (m_bLinePrev) {
        if (m_iLinePrev == iLine) {
            return;
        }
        m_iLinePrev = iLine;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    bool bModified = isModified();
    wxMenuBar *pMenubar = pFrame->GetMenuBar();
    if (pMenubar) {
        wxMenuItem *pItem = pMenubar->FindItem(ID_FILE_SAVE);
        if (pItem) {
            pItem->Enable(bModified);
        }
    }

    wxAuiNotebook *pNotebook = (wxAuiNotebook *)(GetParent());
    int indexT = pNotebook->GetPageIndex(this);
    wxString strT = pNotebook->GetPageText(indexT);
    bool bFlag = strT.EndsWith(uT("*"));
    if (bModified && !bFlag) {
        strT += uT("*");
        pNotebook->SetPageText(indexT, strT);
    }
    else if (!bModified && bFlag) {
        strT = strT.Mid(0, strT.Length() - 1);
        pNotebook->SetPageText(indexT, strT);
    }

    pFrame->updateTitle(this->GetFilename());
    pFrame->updateEditorStatus(this);

    calcLinenumberMargin();

    if (m_ScintillaPrefs.common.statusEnable && (GetMarginWidth(m_StatusID) > 0)) {
        if ((GetModEventMask() & wxSTC_MOD_CHANGEMARKER) == 0) {
            // SetModEventMask SHOULD NOT contain wxSTC_MOD_CHANGEMARKER
            if (bModified) {
                DoAddStatus(iLine, SCRIPT_MASK_MODIFIEDBIT);
            }
            else {
                MarkerDeleteAll(SCRIPT_MASK_MODIFIEDBIT);
            }
        }
    }

    if (bModified && m_pInfobar) {
        m_pInfobar->Refresh();
    }
}

void ScriptEdit::DoSetModifiedLean(int iLine)
{
    if (m_bLoading) {
        return;
    }

    if (iLine < 0) {
        return;
    }

    if (m_ScintillaPrefs.common.statusEnable && (GetMarginWidth(m_StatusID) > 0)) {
        if ((GetModEventMask() & wxSTC_MOD_CHANGEMARKER) == 0) {
            // SetModEventMask SHOULD NOT contain wxSTC_MOD_CHANGEMARKER
            if (isModified()) {
                DoAddStatus(iLine, SCRIPT_MASK_MODIFIEDBIT);
            }
            else {
                MarkerDeleteAll(SCRIPT_MASK_MODIFIEDBIT);
            }
        }
    }
}

void ScriptEdit::OnModified(wxStyledTextEvent &tEvent)
{
    if (m_bLoading) {
        return;
    }

    DoSetModified(LineFromPosition(tEvent.GetPosition()));
}

void ScriptEdit::DoPrint(void)
{
    wxPrintData *printData = ScriptPrint::GetPrintData();
    if (printData == NULL) {
        CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
        if (NULL == pFrame) {
            // should never happen
            return;
        }
        pFrame->OutputStatusbar(uT("Cannot print document: insufficient memory"), SIGMAFRAME_TIMER_SHORT);
        SigmaMessageBox(uT("Cannot print document: insufficient memory"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        return;
    }

    wxPrintDialogData printDialogData(*printData);
    wxPrinter printerT(&printDialogData);
    ScriptPrint printT(this);

    if (!printerT.Print(this, &printT, true)) {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR) {
            return;
        }
    }

    *printData = printerT.GetPrintDialogData().GetPrintData();
    return;
}

bool ScriptEdit::ShowPrintPreviewDialog(void)
{
    wxPrintData *printData = ScriptPrint::GetPrintData();
    if (printData == NULL) {
        CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
        if (NULL == pFrame) {
            // should never happen
            return false;
        }
        pFrame->OutputStatusbar(uT("Cannot print document: insufficient memory"), SIGMAFRAME_TIMER_SHORT);
        SigmaMessageBox(uT("Cannot print document: insufficient memory"), uT("Comet"), wxOK | wxICON_EXCLAMATION, this);
        return false;
    }

    wxPrintDialogData printDialogData(*printData);
    wxPrintPreview *previewT = new (std::nothrow) wxPrintPreview(new ScriptPrint(this), new ScriptPrint(this), &printDialogData);
    if (previewT == NULL) {
        return false;
    }
    if (previewT->Ok() == false) {
        delete previewT;
        return false;
    }

    wxPreviewFrame *framePreview = new wxPreviewFrame(previewT, this->GetParent()->GetParent(), uT("Print Preview"));

    wxRect rect = ::wxGetClientDisplayRect();
    rect.Intersect(wxRect(rect.x, rect.y, 800, 600));
    rect.Deflate(20, 20);
    framePreview->SetSize(rect);

    framePreview->Centre(wxBOTH);
    framePreview->Initialize();
    framePreview->Show(true);
    wxGetApp().SetTopWindow(framePreview);

    return true;
}

void ScriptEdit::initDrop(void)
{
    DragAcceptFiles(true);
}

void ScriptEdit::OnDropFiles(wxDropFilesEvent &tEvent)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    int iNum = tEvent.GetNumberOfFiles();
    if (iNum > 0) {
        if (iNum > CometFrame::MAX_DROPCOUNT) {
            iNum = CometFrame::MAX_DROPCOUNT;
        }
        wxArrayString arrT;
        for (int ii = 0; ii < iNum; ii++) {
            arrT.Add(tEvent.GetFiles()[ii]);
        }
        pFrame->dropFiles(arrT);
    }
}

