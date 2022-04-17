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
#include "CodeEdit.h"

#include <wx/file.h>     // raw file io support
#include <wx/filename.h> // filename support
#include <wx/colour.h>
#include <wx/textdlg.h>
#include <wx/stdpaths.h>

#include <wx/stc/stc.h>

BEGIN_EVENT_TABLE(CodeEdit, wxStyledTextCtrl)
    EVT_SET_FOCUS(CodeEdit::OnSetFocus)
    EVT_KILL_FOCUS(CodeEdit::OnKillFocus)
    EVT_KEY_DOWN(CodeEdit::OnKeyDown)
    EVT_LEFT_DOWN(CodeEdit::OnMouseLeftDown)
    EVT_STC_CHARADDED(wxID_ANY, CodeEdit::OnCharAdded)
    EVT_STC_DWELLSTART(wxID_ANY, CodeEdit::OnDwellStart)
    EVT_STC_DWELLEND(wxID_ANY, CodeEdit::OnDwellEnd)
    EVT_STC_UPDATEUI(wxID_ANY, CodeEdit::OnUIupdate)
    EVT_COMMAND(ID_THREAD_READ, wxEVT_COMMAND_TEXT_UPDATED, CodeEdit::OnMessageReceived)
    EVT_COMMAND(ID_THREAD_MESSAGE, wxEVT_COMMAND_TEXT_UPDATED, CodeEdit::OnMessageReceived)
    EVT_COMMAND(ID_THREAD_FINISH, wxEVT_COMMAND_TEXT_UPDATED, CodeEdit::OnMessageReceived)
END_EVENT_TABLE()

CodeEdit::CodeEdit(wxWindow *parentT, wxWindowID idT,
                   const wxPoint &posT, const wxSize &sizeT, long styleT,
                   const wxString &strName,
                   int iLexer /* = wxSTC_LEX_LUA*/)
    : wxStyledTextCtrl(parentT, idT, posT, sizeT, styleT, strName)
{
    m_iLexer = iLexer;
    m_iLexerType = LEXERTYPE_FIRST;

    m_pMutex = new wxMutex(wxMUTEX_DEFAULT);
    if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_iPlotCreatedId = 0L;
        m_bReadDone = false;
        m_strRead = uT("");
        m_bAskDone = false;
        m_iAnswer = -1;
        m_pLuaEngine = NULL;
        m_iRunningLine = -1;
        m_bGuierrSet = false;
        m_pMutex->Unlock();
    }

    m_LinenumberID = 0;
    m_MarkerID = 1;
    m_FoldingID = 2;
    m_StatusID = 3;

    m_MarkerMargin = 0;
    m_FoldingMargin = 0;
    m_StatusMargin = 0;

    m_fStartTime = 0.0;
    m_fEndTime = 0.0;

    m_strAutoCompList = wxEmptyString;

    m_bWordEnclosed = false;
    m_bPopmenuShown = false;

    m_bRunning = false;

    m_strName = uT("");

    this->SetModEventMask(wxSTC_MOD_INSERTTEXT | wxSTC_MOD_DELETETEXT);
    this->SetCodePage(wxSTC_CP_UTF8);
}

void CodeEdit::createCometLexer(void)
{
    if (PLANGPREFS_USER == NULL) {
        PLANGPREFS_USER = (UserLanguageInfo *)malloc(sizeof(UserLanguageInfo));
        memset(PLANGPREFS_USER, 0, sizeof(UserLanguageInfo));
        PLANGPREFS_USER->name = (char_t *)malloc(LM_STRSIZEN * sizeof(char_t));
        Tmemset(PLANGPREFS_USER->name, 0, LM_STRSIZEN);
        PLANGPREFS_USER->filepattern = (char_t *)malloc(LM_STRSIZEN * sizeof(char_t));
        Tmemset(PLANGPREFS_USER->filepattern, 0, LM_STRSIZEN);
        for (int ii = 0; ii < TYPECOUNT_USER; ii++) {
            PLANGPREFS_USER->styles[ii].words = (char_t *)malloc(LEXER_STRSIZE * sizeof(char_t));
            Tmemset(PLANGPREFS_USER->styles[ii].words, 0, LEXER_STRSIZE);
        }
        PLANGPREFS_USER->styles[TYPECOUNT_USER].set = -1;
        PLANGPREFS_USER->styles[TYPECOUNT_USER].type = -1;
    }
}

void CodeEdit::loadCometLexer(const char_t *name, const char_t *filepattern, const char_t *keywords)
{
    if (PLANGPREFS_USER != NULL) {
        if (ScriptEdit::supportedLexer(filepattern) != 0) {
            return;
        }
        Tstrncpy(PLANGPREFS_USER->name, name, LM_STRSIZEN - 1);
        Tstrncpy(PLANGPREFS_USER->filepattern, filepattern, LM_STRSIZEN - 1);
        wxString strKeywords(keywords);
        strKeywords.Replace(uT("\r"), uT(""), true);
        strKeywords.Replace(uT("\n"), uT(" "), true);
        strKeywords.MakeLower();
        if (strKeywords.IsEmpty() == false) {
            wxStringTokenizer tokenizerT(strKeywords, uT(" "));
            int ll = 0;
            wxString strItem;
            wxArrayString arrT;
            while (tokenizerT.HasMoreTokens() && (ll < 100)) {
                strItem = tokenizerT.GetNextToken();
                if (arrT.Index(strItem) == wxNOT_FOUND) {
                    arrT.Add(strItem);
                }
                ll += 1;
            }
            if (ll > 1) {
                strKeywords.Clear();
                ll = (int)(arrT.size());
                for (int ii = 0; ii < ll; ii++) {
                    strKeywords += arrT[ii];
                    if (ii < (ll - 1)) {
                        strKeywords += uT(" ");
                    }
                }
            }
        }

        Tstrncpy(PLANGPREFS_USER->styles[0].words, LM_CSTR(strKeywords), LEXER_STRSIZE - 1);
        PLANGPREFS_USER->lexer = wxSTC_LEX_COMET;
        PLANGPREFS_USER->styles[0].set = 0;
        PLANGPREFS_USER->styles[0].type = wxSTC_COMET_KEYWORD;
    }
}

bool CodeEdit::saveCometLexer(char_t *name, char_t *filepattern, char_t *keywords)
{
    if (PLANGPREFS_USER != NULL) {
        Tstrncpy(name, PLANGPREFS_USER->name, LM_STRSIZEN - 1);
        Tstrncpy(filepattern, PLANGPREFS_USER->filepattern, LM_STRSIZEN - 1);
        Tstrncpy(keywords, PLANGPREFS_USER->styles[0].words, LEXER_STRSIZE - 1);
        return true;
    }
    return false;
}

void CodeEdit::deleteCometLexer(void)
{
    if (PLANGPREFS_USER != NULL) {
        for (int ii = 0; ii < TYPECOUNT_USER; ii++) {
            if (PLANGPREFS_USER->styles[ii].words != NULL) {
                free(PLANGPREFS_USER->styles[ii].words);
                PLANGPREFS_USER->styles[ii].words = NULL;
            }
        }
        if (PLANGPREFS_USER->name != NULL) {
            free(PLANGPREFS_USER->name);
            PLANGPREFS_USER->name = NULL;
        }
        if (PLANGPREFS_USER->filepattern != NULL) {
            free(PLANGPREFS_USER->filepattern);
            PLANGPREFS_USER->filepattern = NULL;
        }
        free(PLANGPREFS_USER);
        PLANGPREFS_USER = NULL;
    }
}

void ScintillaPrefs::loadDefault(ScintillaPrefs &tScintillaPrefs)
{
    tScintillaPrefs.common.syntaxEnable = true;
    tScintillaPrefs.common.foldEnable = false;
    tScintillaPrefs.common.indentEnable = true;
    tScintillaPrefs.common.overTypeInitial = false;
    tScintillaPrefs.common.readOnlyInitial = false;
    tScintillaPrefs.common.wrapModeInitial = false;
    tScintillaPrefs.common.displayEOLEnable = false;
    tScintillaPrefs.common.indentGuideEnable = false;
    tScintillaPrefs.common.lineNumberEnable = true;
    tScintillaPrefs.common.markerEnable = true;
    tScintillaPrefs.common.statusEnable = true;
    tScintillaPrefs.common.longLineOnEnable = true;
    tScintillaPrefs.common.longLine = 120;
    tScintillaPrefs.common.whiteSpaceEnable = false;
    tScintillaPrefs.common.autocompleteEnable = true;
    tScintillaPrefs.common.calltipEnable = true;
    tScintillaPrefs.common.lineSpacing = 0;
    tScintillaPrefs.common.tabSize = 4;
    tScintillaPrefs.common.useTab = true;

    tScintillaPrefs.common.marckerColorModified = 0x00B1E6;
    tScintillaPrefs.common.marckerColorSaved = 0x009700;
    tScintillaPrefs.common.marckerColorFind = 0xC24230;
    tScintillaPrefs.common.marckerColorError = 0x0000FF;

    tScintillaPrefs.common.reloadDelay = 5;

    for (int ii = 0; ii < STYLEINFO_COUNT; ii++) {
        tScintillaPrefs.style[ii].id = CodeEdit::STYLE_LIGHT[ii].id;
        tScintillaPrefs.style[ii].idl = CodeEdit::STYLE_LIGHT[ii].idl;
        tScintillaPrefs.style[ii].foreground = CodeEdit::STYLE_LIGHT[ii].foreground;
        tScintillaPrefs.style[ii].background = CodeEdit::STYLE_LIGHT[ii].background;
        wxStrcpy(tScintillaPrefs.style[ii].fontname, CodeEdit::STYLE_LIGHT[ii].fontname);
        tScintillaPrefs.style[ii].fontsize = CodeEdit::STYLE_LIGHT[ii].fontsize;
        tScintillaPrefs.style[ii].fontstyle = CodeEdit::STYLE_LIGHT[ii].fontstyle;
        tScintillaPrefs.style[ii].lettercase = CodeEdit::STYLE_LIGHT[ii].lettercase;
    }

    tScintillaPrefs.loaded = false;
}

// Always call after initCommonPrefs to update line spacing
void CodeEdit::initCaret(const int iSpacing)
{
    m_ScintillaPrefs.common.lineSpacing = iSpacing;

    wxSize sizeT = CometApp::getTextExtent(this, uT("Qy"));
    int iH = sizeT.GetHeight();
    if (iH < 8) {
        iH = 8;
    }
    if (iH > 64) {
        iH = 64;
    }

    int iAscent = 1;
    int iDelta = iH / 8;
    switch (m_ScintillaPrefs.common.lineSpacing) {
        case 1:
            iAscent = 2 + iDelta;
            break;
        case 2:
            iAscent = 3 + (2 * iDelta);
            break;
        case 3:
            iAscent = 4 + (3 * iDelta);
            break;
        default:
            break;
    }

    SetExtraAscent(iAscent);
    SetExtraDescent(iAscent);
}

CodeEdit::~CodeEdit()
{
    if (m_pMutex) {
        delete m_pMutex;
        m_pMutex = NULL;
    }
}

void CodeEdit::DoStyleSetFont(int iStyle, unsigned long foreColor, unsigned long backColor, int iSize, const char_t *faceName, int fontStyle /* = 0*/, int letterCase /* = 0*/)
{
    StyleSetForeground(iStyle, wxColor(foreColor));
    StyleSetBackground(iStyle, wxColor(backColor));
    if ((iSize >= 4) && (iSize <= 96)) {
        StyleSetSize(iStyle, iSize);
    }
    if (faceName) {
        wxString strFontName = faceName;
        int iFontSize = iSize;
        int iFontFamily = wxMODERN;
        int iFontStyle = wxNORMAL;
        int iFontWeight = wxNORMAL;
        wxFont fontT(iFontSize, iFontFamily, iFontStyle, iFontWeight);
#ifdef __WXMSW__
        // Consolas font is not installed on Windows XP
        // replaced by an available one
        if (strFontName.CmpNoCase(uT("Consolas")) == 0) {
            OSVERSIONINFO osvi;
            ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
            osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
            GetVersionEx(&osvi);
            if (osvi.dwMajorVersion == 5) {
                strFontName = uT("Courier New");
            }
        }
#endif
        fontT.SetFaceName(strFontName);
        StyleSetFont(iStyle, fontT);
        StyleSetBold(iStyle, (fontStyle & SCRIPT_STYLE_BOLD) > 0);
        StyleSetItalic(iStyle, (fontStyle & SCRIPT_STYLE_ITALIC) > 0);
        StyleSetUnderline(iStyle, (fontStyle & SCRIPT_STYLE_UNDERL) > 0);
        StyleSetVisible(iStyle, (fontStyle & SCRIPT_STYLE_HIDDEN) == 0);
        StyleSetCase(iStyle, letterCase);
    }
}

void CodeEdit::initCommonPrefs(void)
{
    SetViewEOL(m_ScintillaPrefs.common.displayEOLEnable);
    SetEdgeMode(m_ScintillaPrefs.common.longLineOnEnable ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE);
    SetEdgeColumn(m_ScintillaPrefs.common.longLine);
    SetEdgeColour(wxColor(CodeEdit::normalizeColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_CARET].background, -40L)));
    SetViewWhiteSpace(m_ScintillaPrefs.common.whiteSpaceEnable ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE);
    SetOvertype(m_ScintillaPrefs.common.overTypeInitial);
    SetReadOnly(m_ScintillaPrefs.common.readOnlyInitial);
    SetWrapMode(m_ScintillaPrefs.common.wrapModeInitial ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE);

    SetMarginLeft(6);
    SetMarginRight(6);

    SetTabWidth(m_ScintillaPrefs.common.tabSize);
    SetIndent(m_ScintillaPrefs.common.indentEnable ? m_ScintillaPrefs.common.tabSize : 0);
    SetUseTabs(m_ScintillaPrefs.common.useTab);
    SetTabIndents(m_ScintillaPrefs.common.useTab);
    SetBackSpaceUnIndents(m_ScintillaPrefs.common.useTab);
    SetIndentationGuides(m_ScintillaPrefs.common.indentGuideEnable);

    initCaret(m_ScintillaPrefs.common.lineSpacing);
}

void CodeEdit::getPrefs(ScintillaPrefs &tPrefs)
{
    EditorConfig::copy(tPrefs, m_ScintillaPrefs);
}

LanguageInfo const *CodeEdit::getLanguageInfo(int iLexer)
{
    switch (iLexer) {
        case wxSTC_LEX_LUA:
            for (int ii = 0; ii < STYLEINFO_COUNT_SYNTAX; ii++) {
                m_ScintillaPrefs.style[ii].idl = m_ScintillaPrefs.style[ii].id;
            }
            return &LANGPREFS_LUA;

        case wxSTC_LEX_CPP:
            for (int ii = 0; ii < STYLEINFO_COUNT_SYNTAX; ii++) {
                m_ScintillaPrefs.style[ii].idl = -1;
            }
            m_ScintillaPrefs.style[wxSTC_LUA_WORD].idl = wxSTC_C_WORD;
            m_ScintillaPrefs.style[wxSTC_LUA_WORD2].idl = wxSTC_C_WORD2;
            m_ScintillaPrefs.style[wxSTC_LUA_NUMBER].idl = wxSTC_C_NUMBER;
            m_ScintillaPrefs.style[wxSTC_LUA_STRING].idl = wxSTC_C_STRING;
            m_ScintillaPrefs.style[wxSTC_LUA_LITERALSTRING].idl = wxSTC_C_VERBATIM;
            m_ScintillaPrefs.style[wxSTC_LUA_CHARACTER].idl = wxSTC_C_CHARACTER;
            m_ScintillaPrefs.style[wxSTC_LUA_OPERATOR].idl = wxSTC_C_OPERATOR;
            m_ScintillaPrefs.style[wxSTC_LUA_IDENTIFIER].idl = wxSTC_C_IDENTIFIER;
            m_ScintillaPrefs.style[wxSTC_LUA_PREPROCESSOR].idl = wxSTC_C_PREPROCESSOR;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENT].idl = wxSTC_C_COMMENT;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENTLINE].idl = wxSTC_C_COMMENTLINE;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENTDOC].idl = wxSTC_C_COMMENTDOC;
            return ((m_iLexerType == LEXERTYPE_SECOND) ? &LANGPREFS_CPP : &LANGPREFS_C);

        case wxSTC_LEX_BASH:
            for (int ii = 0; ii < STYLEINFO_COUNT_SYNTAX; ii++) {
                m_ScintillaPrefs.style[ii].idl = -1;
            }
            m_ScintillaPrefs.style[wxSTC_LUA_WORD].idl = wxSTC_SH_WORD;
            m_ScintillaPrefs.style[wxSTC_LUA_WORD2].idl = wxSTC_SH_WORD2;
            m_ScintillaPrefs.style[wxSTC_LUA_NUMBER].idl = wxSTC_SH_NUMBER;
            m_ScintillaPrefs.style[wxSTC_LUA_STRING].idl = wxSTC_SH_STRING;
            m_ScintillaPrefs.style[wxSTC_LUA_CHARACTER].idl = wxSTC_SH_CHARACTER;
            m_ScintillaPrefs.style[wxSTC_LUA_OPERATOR].idl = wxSTC_SH_OPERATOR;
            m_ScintillaPrefs.style[wxSTC_LUA_IDENTIFIER].idl = wxSTC_SH_IDENTIFIER;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENT].idl = wxSTC_SH_COMMENTLINE;
            return &LANGPREFS_BASH;

        case wxSTC_LEX_PYTHON:
            for (int ii = 0; ii < STYLEINFO_COUNT_SYNTAX; ii++) {
                m_ScintillaPrefs.style[ii].idl = -1;
            }
            m_ScintillaPrefs.style[wxSTC_LUA_WORD].idl = wxSTC_P_WORD;
            m_ScintillaPrefs.style[wxSTC_LUA_NUMBER].idl = wxSTC_P_NUMBER;
            m_ScintillaPrefs.style[wxSTC_LUA_STRING].idl = wxSTC_P_STRING;
            m_ScintillaPrefs.style[wxSTC_LUA_CHARACTER].idl = wxSTC_P_CHARACTER;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENT].idl = wxSTC_P_COMMENTLINE;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENTDOC].idl = wxSTC_P_COMMENTBLOCK;
            m_ScintillaPrefs.style[wxSTC_LUA_STRINGEOL].idl = wxSTC_P_TRIPLEDOUBLE;
            m_ScintillaPrefs.style[wxSTC_LUA_LITERALSTRING].idl = wxSTC_P_TRIPLE;
            return &LANGPREFS_PYTHON;

        case wxSTC_LEX_OCTAVE:
            for (int ii = 0; ii < STYLEINFO_COUNT_SYNTAX; ii++) {
                m_ScintillaPrefs.style[ii].idl = -1;
            }
            m_ScintillaPrefs.style[wxSTC_LUA_WORD].idl = wxSTC_MATLAB_KEYWORD;
            m_ScintillaPrefs.style[wxSTC_LUA_NUMBER].idl = wxSTC_MATLAB_NUMBER;
            m_ScintillaPrefs.style[wxSTC_LUA_STRING].idl = wxSTC_MATLAB_STRING;
            m_ScintillaPrefs.style[wxSTC_LUA_CHARACTER].idl = wxSTC_MATLAB_DOUBLEQUOTESTRING;
            m_ScintillaPrefs.style[wxSTC_LUA_OPERATOR].idl = wxSTC_MATLAB_OPERATOR;
            m_ScintillaPrefs.style[wxSTC_LUA_IDENTIFIER].idl = wxSTC_MATLAB_IDENTIFIER;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENT].idl = wxSTC_MATLAB_COMMENT;
            return &LANGPREFS_OCTAVE;

        case wxSTC_LEX_FORTRAN:
            for (int ii = 0; ii < STYLEINFO_COUNT_SYNTAX; ii++) {
                m_ScintillaPrefs.style[ii].idl = -1;
            }
            m_ScintillaPrefs.style[wxSTC_LUA_WORD].idl = wxSTC_F_WORD;
            m_ScintillaPrefs.style[wxSTC_LUA_WORD2].idl = wxSTC_F_WORD2;
            m_ScintillaPrefs.style[wxSTC_LUA_WORD3].idl = wxSTC_F_WORD3;
            m_ScintillaPrefs.style[wxSTC_LUA_NUMBER].idl = wxSTC_F_NUMBER;
            m_ScintillaPrefs.style[wxSTC_LUA_STRING].idl = wxSTC_F_STRING1;
            m_ScintillaPrefs.style[wxSTC_LUA_CHARACTER].idl = wxSTC_F_STRING2;
            m_ScintillaPrefs.style[wxSTC_LUA_STRINGEOL].idl = wxSTC_F_STRINGEOL;
            m_ScintillaPrefs.style[wxSTC_LUA_OPERATOR].idl = wxSTC_F_OPERATOR;
            m_ScintillaPrefs.style[wxSTC_LUA_IDENTIFIER].idl = wxSTC_F_IDENTIFIER;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENT].idl = wxSTC_F_COMMENT;
            m_ScintillaPrefs.style[wxSTC_LUA_PREPROCESSOR].idl = wxSTC_F_PREPROCESSOR;
            return &LANGPREFS_FORTRAN;

        case wxSTC_LEX_LATEX:
            for (int ii = 0; ii < STYLEINFO_COUNT_SYNTAX; ii++) {
                m_ScintillaPrefs.style[ii].idl = -1;
            }

            m_ScintillaPrefs.style[wxSTC_LUA_WORD].idl = wxSTC_L_COMMAND;
            m_ScintillaPrefs.style[wxSTC_LUA_PREPROCESSOR].idl = wxSTC_L_TAG;
            m_ScintillaPrefs.style[wxSTC_LUA_NUMBER].idl = wxSTC_L_MATH;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENT].idl = wxSTC_L_COMMENT;
            return &LANGPREFS_LATEX;

        case wxSTC_LEX_MAKEFILE:
            for (int ii = 0; ii < STYLEINFO_COUNT_SYNTAX; ii++) {
                m_ScintillaPrefs.style[ii].idl = -1;
            }

            m_ScintillaPrefs.style[wxSTC_LUA_WORD].idl = wxSTC_MAKE_IDENTIFIER;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENT].idl = wxSTC_MAKE_COMMENT;
            m_ScintillaPrefs.style[wxSTC_LUA_NUMBER].idl = wxSTC_MAKE_TARGET;
            m_ScintillaPrefs.style[wxSTC_LUA_PREPROCESSOR].idl = wxSTC_MAKE_PREPROCESSOR;
            m_ScintillaPrefs.style[wxSTC_LUA_STRING].idl = wxSTC_MAKE_IDEOL;
            return &LANGPREFS_MAKEFILE;

        case wxSTC_LEX_COMET:
            for (int ii = 0; ii < STYLEINFO_COUNT_SYNTAX; ii++) {
                m_ScintillaPrefs.style[ii].idl = -1;
            }
            m_ScintillaPrefs.style[wxSTC_LUA_WORD].idl = wxSTC_COMET_KEYWORD;
            m_ScintillaPrefs.style[wxSTC_LUA_NUMBER].idl = wxSTC_COMET_NUMBER;
            m_ScintillaPrefs.style[wxSTC_LUA_STRING].idl = wxSTC_COMET_STRING;
            m_ScintillaPrefs.style[wxSTC_LUA_CHARACTER].idl = wxSTC_COMET_DOUBLEQUOTESTRING;
            m_ScintillaPrefs.style[wxSTC_LUA_OPERATOR].idl = wxSTC_COMET_OPERATOR;
            m_ScintillaPrefs.style[wxSTC_LUA_IDENTIFIER].idl = wxSTC_COMET_IDENTIFIER;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENT].idl = wxSTC_COMET_COMMENT;
            return (LanguageInfo const *)(PLANGPREFS_USER);

        case wxSTC_LEX_BATCH:
            for (int ii = 0; ii < STYLEINFO_COUNT_SYNTAX; ii++) {
                m_ScintillaPrefs.style[ii].idl = -1;
            }

            m_ScintillaPrefs.style[wxSTC_LUA_WORD].idl = wxSTC_BAT_WORD;
            m_ScintillaPrefs.style[wxSTC_LUA_WORD2].idl = wxSTC_BAT_COMMAND;
            m_ScintillaPrefs.style[wxSTC_LUA_PREPROCESSOR].idl = wxSTC_BAT_HIDE;
            m_ScintillaPrefs.style[wxSTC_LUA_OPERATOR].idl = wxSTC_BAT_OPERATOR;
            m_ScintillaPrefs.style[wxSTC_LUA_PREPROCESSOR].idl = wxSTC_BAT_IDENTIFIER;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENT].idl = wxSTC_BAT_COMMENT;
            return &LANGPREFS_BATCH;

        case wxSTC_LEX_SOLIS:
            for (int ii = 0; ii < STYLEINFO_COUNT_SYNTAX; ii++) {
                m_ScintillaPrefs.style[ii].idl = -1;
            }

            m_ScintillaPrefs.style[wxSTC_LUA_WORD].idl = wxSTC_SOLIS_KEYWORD;
            m_ScintillaPrefs.style[wxSTC_LUA_WORD2].idl = wxSTC_SOLIS_KEYWORD2;
            m_ScintillaPrefs.style[wxSTC_LUA_WORD3].idl = wxSTC_SOLIS_COMMAND;
            m_ScintillaPrefs.style[wxSTC_LUA_WORD3].fontstyle = m_ScintillaPrefs.style[wxSTC_LUA_WORD].fontstyle;
            m_ScintillaPrefs.style[wxSTC_LUA_WORD3].foreground = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
            m_ScintillaPrefs.style[wxSTC_LUA_OPERATOR].idl = wxSTC_SOLIS_OPERATOR;
            m_ScintillaPrefs.style[wxSTC_LUA_OPERATOR].foreground = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
            m_ScintillaPrefs.style[wxSTC_LUA_PREPROCESSOR].idl = wxSTC_SOLIS_ERROR;
            m_ScintillaPrefs.style[wxSTC_LUA_STRING].idl = wxSTC_SOLIS_STRING;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENT].idl = wxSTC_SOLIS_COMMENT;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENTLINE].idl = wxSTC_SOLIS_COMMENTLINE;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENTLINE].fontstyle = m_ScintillaPrefs.style[wxSTC_LUA_WORD].fontstyle;
            m_ScintillaPrefs.style[wxSTC_LUA_COMMENTLINE].foreground = 0x1988DD;
            m_ScintillaPrefs.style[wxSTC_LUA_IDENTIFIER].foreground = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;

            if (m_iLexerType == LEXERTYPE_SECOND) {
                m_ScintillaPrefs.style[wxSTC_LUA_NUMBER].idl = wxSTC_SOLIS_NUMBER;
                return &LANGPREFS_SOLISOUTPUT;
            }
            else if (m_iLexerType == LEXERTYPE_THIRD) {
                return &LANGPREFS_SETTINGS;
            }
            else if (m_iLexerType == LEXERTYPE_FOURTH) {
                return &LANGPREFS_USERSYNTAX;
            }
            else {
                m_ScintillaPrefs.style[wxSTC_LUA_NUMBER].idl = wxSTC_SOLIS_NUMBER;
                return &LANGPREFS_SOLIS;
            }
            // never reached
            break;

        default:
            return NULL;
    }
}
bool CodeEdit::initLexer(int iLexer, int iLexerType)
{
    m_iLexer = iLexer;
    m_iLexerType = iLexerType;

    int ii;

    if (m_iLexer == wxSTC_LEX_NULL) {
        SetLexer(wxSTC_LEX_NULL);
        return false;
    }

    LanguageInfo const *curInfo = getLanguageInfo(m_iLexer);
    if (curInfo == NULL) {
        SetLexer(wxSTC_LEX_NULL);
        return false;
    }

    SetLexer(curInfo->lexer);

    char_t *pwords = NULL;
    int keyset = -1;
    for (ii = 0; ii < STYLE_TYPES_COUNT; ii++) {
        keyset = curInfo->styles[ii].set;
        if (keyset < 0) {
            break;
        }
        pwords = (char_t *)(curInfo->styles[ii].words);
        if (pwords == NULL) {
            break;
        }
        SetKeyWords(keyset, (const char_t *)pwords);
    }

    return true;
}

// Darken or lighten color (Darken if iDelta < 0)
// Format: OxBBGGRR
long CodeEdit::normalizeColor(long iColor, long iDelta)
{
    return CodeEdit::normalizeColor(iColor, iDelta, iDelta, iDelta);
}

// Darken or lighten color (Darken if iDeltaX < 0)
// Format: OxBBGGRR
long CodeEdit::normalizeColor(long iColor, long iDeltaR, long iDeltaG, long iDeltaB)
{
    long iB = (iColor >> 16L) + iDeltaB;

    if (iB > 255L) {
        iB = 255L;
    }
    else if (iB < 0L) {
        iB = 0L;
    }

    long iG = ((iColor >> 8L) & 0x00FF) + iDeltaG;

    if (iG > 255L) {
        iG = 255L;
    }
    else if (iG < 0L) {
        iG = 0L;
    }

    long iR = (iColor & 0x0000FF) + iDeltaR;

    if (iR > 255L) {
        iR = 255L;
    }
    else if (iR < 0L) {
        iR = 0L;
    }

    return (iR | (iG << 8L) | (iB << 16L));
}

bool CodeEdit::setPrefs(const ScintillaPrefs &tPrefs, bool bInit /* = true*/)
{
    EditorConfig::copy(m_ScintillaPrefs, tPrefs);
    if (bInit) {
        return initPrefs();
    }
    return true;
}

bool CodeEdit::initPrefs(void)
{
    int ii;

    unsigned long lBack = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
    unsigned long lFore = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
    wxColor clrBack = wxColor(lBack);
    wxColor clrFore = wxColor(lFore);

    // Reset styles
    for (ii = 0; ii <= wxSTC_STYLE_LASTPREDEFINED; ii++) {
        DoStyleSetFont(ii, lFore, lBack,
                       m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].fontsize,
                       static_cast<const char_t *>(m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].fontname),
                       m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].fontstyle,
                       m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].lettercase);
    }

    // set visibility
    SetVisiblePolicy(wxSTC_VISIBLE_STRICT | wxSTC_VISIBLE_SLOP, 1);
    SetXCaretPolicy(wxSTC_CARET_EVEN | wxSTC_VISIBLE_STRICT | wxSTC_CARET_SLOP, 1);
    SetYCaretPolicy(wxSTC_CARET_EVEN | wxSTC_VISIBLE_STRICT | wxSTC_CARET_SLOP, 1);

    MarkerSetForeground(wxSTC_MARGIN_SYMBOL, wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].foreground));
    MarkerSetBackground(wxSTC_MARGIN_SYMBOL, wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].background));

    StyleSetForeground(wxSTC_STYLE_FOLDINGLINE, wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].foreground));

    SetSelForeground(false, wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_SELECTION].foreground));
    SetSelBackground(true, wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_SELECTION].background));

    SetWhitespaceForeground(true, wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_WHITESPACE].foreground));
    StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_WHITESPACE].foreground));

    initCommonPrefs();

    DoStyleSetFont(wxSTC_STYLE_LINENUMBER, m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].foreground,
                   m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].background,
                   m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].fontsize - 1,
                   static_cast<const char_t *>(m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].fontname),
                   m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].fontstyle,
                   m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].lettercase);

    SetLayoutCache(wxSTC_CACHE_PAGE);

    // Markers
    SetMarginMask(m_MarkerID, SCRIPT_MASK_BOOKMARK | SCRIPT_MASK_BREAKPOINT);
    MarkerDefineBitmap(SCRIPT_MASK_BOOKMARKBIT, wxBitmap(bookmark_small_xpm));
    MarkerDefineBitmap(SCRIPT_MASK_BREAKPOINTBIT, wxBitmap(breakpoint_small_xpm));
    SetMarginType(m_MarkerID, wxSTC_MARGIN_SYMBOL);
    SetMarginWidth(m_MarkerID, m_ScintillaPrefs.common.markerEnable ? m_MarkerMargin : 0);
    SetMarginSensitive(m_MarkerID, true);

    // Line Number
    SetMarginWidth(m_LinenumberID, TextWidth(wxSTC_STYLE_LINENUMBER, uT("-0000")));

    // Folding
    SetMarginType(m_FoldingID, wxSTC_MARGIN_SYMBOL);
    SetMarginMask(m_FoldingID, wxSTC_MASK_FOLDERS);
    wxColor clrBackFold = wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].foreground);
    MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS, clrBack, clrBackFold);
    MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS, clrBack, clrBackFold);
    MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE, clrBack, clrBackFold);
    MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER, clrBack, clrBackFold);
    MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED, clrBack, clrBackFold);
    MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, clrBack, clrBackFold);
    MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER, clrBack, clrBackFold);
    SetMarginWidth(m_FoldingID, m_ScintillaPrefs.common.foldEnable ? m_FoldingMargin : 0);
    SetMarginSensitive(m_FoldingID, true);

    SetProperty(uT("fold"), uT("1"));
    SetProperty(uT("fold.comment"), uT("1"));
    SetProperty(uT("fold.compact"), uT("0"));
    SetProperty(uT("fold.preprocessor"), uT("1"));
    SetProperty(uT("fold.html"), uT("1"));
    SetProperty(uT("fold.html.preprocessor"), uT("1"));
    SetProperty(uT("fold.comment.lua"), uT("1"));
    SetProperty(uT("fold.quotes.lua"), uT("1"));

    SetProperty(uT("lexer.cpp.track.preprocessor"), uT("1"));
    SetProperty(uT("lexer.cpp.update.preprocessor"), uT("1"));
    SetProperty(uT("styling.within.preprocessor"), uT("1"));

    SetFoldFlags(wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);

    SetCaretForeground(wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_CARET].foreground));
    SetCaretLineBackground(wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_CARET].background));

    AutoCompSetSeparator('|');
    AutoCompSetChooseSingle(true);
    AutoCompSetIgnoreCase(false);
    SetMouseDwellTime(500);

    initLexer(m_iLexer, m_iLexerType);
    DoHighlightSyntax(m_ScintillaPrefs.common.syntaxEnable);

    this->setMarkersColors(m_ScintillaPrefs.common.marckerColorModified,
                           m_ScintillaPrefs.common.marckerColorSaved,
                           m_ScintillaPrefs.common.marckerColorFind,
                           m_ScintillaPrefs.common.marckerColorError);

    return true;
}

void CodeEdit::setGuierr(const wxString &strT)
{
    if (m_bGuierrSet || (strT.StartsWith(uT("! ")) == false)) {
        return;
    }

    if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        if (m_pLuaEngine != NULL) {
            lua_State *pLuaState = m_pLuaEngine->getLuaState();
            if (pLuaState != NULL) {
                char szT[LM_STRSIZE];
                memset(szT, 0, LM_STRSIZE * sizeof(char));
                // Convert from Unicode to ANSI
                strncpy(szT, LM_U8STR(strT), LM_STRSIZE - 1);
                lua_pushliteral(pLuaState, "___SigmaGuierr___");
                lua_pushstring(pLuaState, static_cast<const char *>(szT));
                lua_settable(pLuaState, LUA_REGISTRYINDEX);
                // Error line number
                m_pLuaEngine->setErrLine(m_iRunningLine);
                //
                m_bGuierrSet = true;
            }
        }
        m_pMutex->Unlock();
    }
}

void CodeEdit::OnMessageReceived(wxCommandEvent &tEvent)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        tEvent.Skip();
        return;
    }

    int idT = tEvent.GetId();

    if (idT == ID_THREAD_READ) {
        this->luaDoRead(tEvent.GetInt(), tEvent.GetString());
    }

    else if (idT == ID_THREAD_MESSAGE) {
        if (tEvent.GetInt() == 0) {
            this->luaDoAlert(tEvent.GetString());
        }
        else {
            this->luaDoAsk(tEvent.GetString());
        }
    }
}

bool CodeEdit::luaDoRead(int iNum /* = 0*/, const wxString &strLabel /* = uT("")*/)
{
    if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_bReadDone = false;
        m_strRead = uT("");
        m_iAnswer = -1;
        m_pMutex->Unlock();
    }

    // >> Show Dialog
    wxString strT = strLabel;
    strT.Trim(true);
    strT.Trim(false);
    wxTextEntryDialog *pDlg = new (std::nothrow) wxTextEntryDialog(this, strT.IsEmpty() ? uT("Input:") : strT, uT("Comet"), uT(""), wxOK, wxDefaultPosition, wxSize(250, wxDefaultCoord));
    if (pDlg == NULL) {
        if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
            m_strRead = uT("");
            m_iAnswer = -1;
            m_bReadDone = true;
            m_pMutex->Unlock();
        }
        return false;
    }
    pDlg->SetMaxLength(LM_STRSIZE);
    wxPoint ptT = this->GetScreenPosition();
    wxSize sizeT = this->GetSize();
    wxSize sizeD = pDlg->GetSize();
    int nx = (sizeT.GetWidth() / 2) - (sizeD.GetWidth() / 2),
        ny = (sizeT.GetHeight() / 2) - (sizeD.GetHeight() / 2);
    if (nx < 0) {
        nx = 0;
    }
    if (ny < 0) {
        ny = 0;
    }
    ptT += wxPoint(nx, ny);
    pDlg->Move(ptT);
    pDlg->ShowModal();
    strT = pDlg->GetValue();
    pDlg->Destroy();

    if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_bReadDone = true;
        m_strRead = strT;
        m_pMutex->Unlock();
    }

    // <<

    return true;
}

bool CodeEdit::luaDoAsk(const wxString &strMessage, const wxString &strTitle /* = uT("Comet")*/)
{
    if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_bAskDone = false;
        m_iAnswer = -1;
        m_pMutex->Unlock();
    }

    int iret = SigmaMessageBox(strMessage, strTitle, wxYES_NO | wxICON_QUESTION, NULL);

    if (m_pMutex->TryLock() == wxMUTEX_NO_ERROR) {
        m_bAskDone = true;
        m_iAnswer = (iret == wxYES) ? 1 : 0;
        m_pMutex->Unlock();
    }

    return true;
}

bool CodeEdit::luaDoAlert(const wxString &strMessage, const wxString &strTitle /* = uT("Comet")*/)
{
    SigmaMessageBox(strMessage, strTitle, wxOK | wxICON_INFORMATION, NULL);
    return true;
}

void CodeEdit::DoHighlightSyntax(bool bEnable)
{
    int ii;

    m_ScintillaPrefs.common.syntaxEnable = bEnable;

    unsigned long lBack = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
    unsigned long lFore = m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].foreground;
    wxColor clrBack = wxColor(lBack);
    wxColor clrFore = wxColor(lFore);

    // Reset Syntax Styles
    for (ii = 0; ii <= STYLEINFO_COUNT_SYNTAX; ii++) {
        DoStyleSetFont(ii, lFore, lBack,
                       m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].fontsize,
                       static_cast<const char_t *>(m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].fontname),
                       m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].fontstyle,
                       m_ScintillaPrefs.style[wxSTC_LUA_DEFAULT].lettercase);
    }

    if (m_ScintillaPrefs.common.syntaxEnable && (wxSTC_LEX_NULL != m_iLexer)) {
        StyleInfo *curType = NULL;
        for (ii = 0; ii < STYLEINFO_COUNT_SYNTAX; ii++) {
            curType = &(m_ScintillaPrefs.style[ii]);
            if (curType->idl != -1) {
                DoStyleSetFont(curType->idl, curType->foreground, lBack,
                               curType->fontsize, static_cast<const char_t *>(curType->fontname),
                               curType->fontstyle, curType->lettercase);
            }
        }

        // Brace highlight
        StyleSetBackground(wxSTC_STYLE_BRACELIGHT, wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_CARET].background));
        StyleSetForeground(wxSTC_STYLE_BRACELIGHT, wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_SELECTION].foreground));
        StyleSetBackground(wxSTC_STYLE_BRACEBAD, wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_CARET].background));
        StyleSetForeground(wxSTC_STYLE_BRACEBAD, wxColor(m_ScintillaPrefs.style[STYLEINFO_INDEX_STRING_END].foreground));
        StyleSetBold(wxSTC_STYLE_BRACELIGHT, true);
        StyleSetBold(wxSTC_STYLE_BRACEBAD, true);
    }
    else {
        // Brace highlight
        StyleSetBackground(wxSTC_STYLE_BRACELIGHT, clrBack);
        StyleSetForeground(wxSTC_STYLE_BRACELIGHT, clrFore);
        StyleSetBackground(wxSTC_STYLE_BRACEBAD, clrBack);
        StyleSetForeground(wxSTC_STYLE_BRACEBAD, clrFore);
        StyleSetBold(wxSTC_STYLE_BRACELIGHT, false);
        StyleSetBold(wxSTC_STYLE_BRACEBAD, false);
    }

    // Immediately refresh colouring
    this->Colourise(0, this->GetLineEndPosition(this->GetLineCount() - 1));
}

void CodeEdit::DoEnableAutocomplete(bool bEnable)
{
    m_ScintillaPrefs.common.autocompleteEnable = bEnable;
}

void CodeEdit::DoEnableCalltip(bool bEnable)
{
    m_ScintillaPrefs.common.calltipEnable = bEnable;
}

void CodeEdit::DoCallTipCancel(void)
{
    if (AutoCompActive()) {
        AutoCompCancel();
        m_strAutoCompList = wxEmptyString;
    }
    if (CallTipActive()) {
        CallTipCancel();
    }
}

void CodeEdit::OnCharAdded(wxStyledTextEvent &WXUNUSED(tEvent))
{
    if (m_ScintillaPrefs.common.autocompleteEnable == false) {
        return;
    }

    const int nStartSel = GetSelectionStart();
    const int nEndSel = GetSelectionEnd();

    if ((nStartSel != nEndSel) || (nStartSel < 2)) {
        return;
    }

    if (this->isWordEnclosed(LineFromPosition(nStartSel), nStartSel)) {
        m_bWordEnclosed = true;
        return;
    }

    const bool bAutoCompActive = AutoCompActive();

    const AutoCList *pAutoCList;
    const int *pAutoCListLen;

    switch (m_iLexer) {

        case wxSTC_LEX_LUA:
            pAutoCList = AUTOC_API_LUA;
            pAutoCListLen = &AUTOC_API_LUA_LEN;
            break;

        case wxSTC_LEX_CPP:
            if (m_iLexerType == LEXERTYPE_FIRST) {
                pAutoCList = AUTOC_API_C;
                pAutoCListLen = &AUTOC_API_C_LEN;
            }
            else {
                pAutoCList = AUTOC_API_CPP;
                pAutoCListLen = &AUTOC_API_CPP_LEN;
            }
            break;

        case wxSTC_LEX_PYTHON:
            pAutoCList = AUTOC_API_PYTHON;
            pAutoCListLen = &AUTOC_API_PYTHON_LEN;
            break;

        case wxSTC_LEX_LATEX:
            pAutoCList = AUTOC_API_LATEX;
            pAutoCListLen = &AUTOC_API_LATEX_LEN;
            break;

        case wxSTC_LEX_SOLIS:
            if (LEXERTYPE_FIRST != m_iLexerType) {
                return;
            }
            pAutoCList = AUTOC_API_SOLIS;
            pAutoCListLen = &AUTOC_API_SOLIS_LEN;
            break;

        default:
            return;
    }

    const CallTipList *pCallTipList = CALLTIP_API_LUA;

    // Get the previous N characters and display the ad hoc autocomplete list

    bool bAutoUpdate = false, bAutoCancel = false, bAutoShown = false;
    int nStart = 0, ii = 0, jj, ll;
    wxString strT, strElem, strItem, strList;

    const int is = *pAutoCListLen;

    for (ii = 0; (pAutoCList[ii].len != 0) && (ii < is); ii++) {

        nStart = nStartSel - pAutoCList[ii].len;
        if (nStart < 0) {
            continue;
        }

        // Whole word
        if (!bAutoCompActive) {
            if (!isSeparator(nStart - 1)) {
                continue;
            }
        }
        //

        strList = pAutoCList[ii].words;

        if (!bAutoCompActive) {
            strT = GetTextRange(nStart, nEndSel);
            wxStringTokenizer tokenizerT(strList, uT("|"));
            ll = 0;
            while (tokenizerT.HasMoreTokens() && (ll < pAutoCList[ii].count)) {
                strItem = tokenizerT.GetNextToken();
                if (strItem.StartsWith(strT) && !bAutoCompActive) {
                    m_strAutoCompList = strItem;
                    AutoCompSetChooseSingle(false);
                    AutoCompShow(pAutoCList[ii].len, strList, ll);
                    bAutoShown = true;
                    break;
                }
                ll += 1;
            }
        }

        if (bAutoShown) {
            return;
        }

        if (!bAutoCompActive) {
            continue;
        }

        strT = GetTextRange(nStart, nEndSel);

        if (strT.Contains(uT("(")) == false) {
            for (jj = 1; jj < pAutoCList[ii].maxi; jj++) {

                nStart -= 1;
                if (nStart < 0) {
                    break;
                }

                // Whole word
                if (!isSeparator(nStart - 1)) {
                    continue;
                }
                //

                strT = GetTextRange(nStart, nEndSel);

                wxStringTokenizer tokenizerT(strList, uT("|"));
                ll = 0;
                bAutoUpdate = false;
                bAutoCancel = false;
                strItem = wxEmptyString;
                while (tokenizerT.HasMoreTokens() && (ll < pAutoCList[ii].count)) {
                    strItem = tokenizerT.GetNextToken();

                    if (strItem.IsSameAs(strT)) {
                        bAutoUpdate = true;
                        bAutoCancel = true;
                        break;
                    }
                    else if (strItem.StartsWith(strT)) {
                        bAutoUpdate = true;
                        bAutoCancel = false;
                        break;
                    }
                    ll += 1;
                }

                if (bAutoUpdate && (strItem.IsEmpty() == false)) {
                    if (m_strAutoCompList.IsSameAs(strItem)) {
                        AutoCompUpdate(pAutoCList[ii].len + jj, strItem, ll, false);
                    }
                    else {
                        DoCallTipCancel();
                        m_strAutoCompList = strItem;
                        AutoCompSetChooseSingle(false);
                        AutoCompShow(pAutoCList[ii].len + jj, strItem, 0);
                    }
                    bAutoShown = true;
                    break;
                }
            }
        }

        if (bAutoShown) {
            return;
        }
    }

    if (m_iLexer != wxSTC_LEX_LUA) {
        if ((bAutoShown == false) && bAutoCompActive) {
            DoCallTipCancel();
        }
        return;
    }

    //
    // The following code is only for Lua
    //

    if (this->GetCharAt(nStartSel - 1) == uT('.')) {
        DoCallTipCancel();
    }
    const bool bAutoCompActiveB = AutoCompActive();

    for (ii = is; pAutoCList[ii].len != 0; ii++) {

        nStart = nStartSel - pAutoCList[ii].len;
        if (nStart < 0) {
            continue;
        }

        // Whole word
        if (!bAutoCompActiveB) {
            if (!isSeparator(nStart - 1)) {
                continue;
            }
        }
        //

        strT = GetTextRange(nStart, nEndSel);

        strList = pAutoCList[ii].words;

        // Display the auto completion list for the first time.
        if (!bAutoCompActiveB && strT.StartsWith(pAutoCList[ii].prefix)) {
            m_strAutoCompList = strList;
            AutoCompSetChooseSingle(false);
            AutoCompShow(0, strList, 0);
            bAutoShown = true;
            break;
        }

        if (!bAutoCompActiveB) {
            continue;
        }

        if (strT.Contains(uT("(")) == false) {

            // If AutoComp already shown, update it or complete it.
            for (jj = 1; jj < pAutoCList[ii].maxi; jj++) {

                nStart -= 1;
                if (nStart < 0) {
                    break;
                }

                // Whole word
                if (!isSeparator(nStart - 1)) {
                    continue;
                }
                //

                strT = GetTextRange(nStart, nEndSel);

                // Display the auto completion list
                if (strT.StartsWith(pAutoCList[ii].prefix)) {

                    bAutoUpdate = false;
                    bAutoCancel = false;
                    ll = 0;
                    strItem = wxEmptyString;
                    wxStringTokenizer tokenizerT(strList, uT("|"));
                    while (tokenizerT.HasMoreTokens() && (ll < pAutoCList[ii].count)) {
                        strItem = tokenizerT.GetNextToken();
                        strElem = pAutoCList[ii].prefix;
                        strElem += strItem;
                        if (strElem.IsSameAs(strT)) {
                            bAutoUpdate = true;
                            bAutoCancel = true;
                            break;
                        }
                        else if (strElem.StartsWith(strT)) {
                            bAutoUpdate = true;
                            bAutoCancel = false;
                            break;
                        }
                        ll += 1;
                    }

                    if (bAutoUpdate) {
                        AutoCompUpdate(jj, strList, ll, bAutoCancel);
                        bAutoShown = true;
                    }
                    else {
                        DoCallTipCancel();
                    }

                    break;
                }
            }
        }
    }

    if (bAutoShown) {
        return;
    }

    if (this->GetCharAt(nStartSel - 1) == uT('(')) {
        DoCallTipCancel();
    }
    const bool bAutoCompActiveC = AutoCompActive();

    bool bCallTipShow = false;

    for (ii = 0; pCallTipList[ii].len != 0; ii++) {
        nStart = nStartSel - pCallTipList[ii].len;
        if (nStart < 0) {
            continue;
        }

        // Whole word
        if (!isSeparator(nStart - 1)) {
            continue;
        }
        //

        strT = GetTextRange(nStart, nEndSel);
        // Display the call tips
        if (strT == pCallTipList[ii].prefix) {
            if (bAutoCompActiveC) {
                DoCallTipCancel();
            }
            strList = pCallTipList[ii].args;
            CallTipShow(nEndSel, strList);
            bCallTipShow = true;
            break;
        }
    }

    if (bCallTipShow == false) {
        DoCallTipCancel();
        return;
    }
}

void CodeEdit::updateStatusbar(bool bUpdateLexer /* = false*/)
{
    int nPos = GetSelectionStart();
    int nLine = LineFromPosition(nPos);
    int nCol = GetColumn(nPos);

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    if (bUpdateLexer) {
        wxString strLexer = isModeScriptConsole() ? m_strName : getLexerName();
        pFrame->updateStatusbarLexer(strLexer);
    }
    pFrame->updateStatusbarPos(nLine, nCol);
    pFrame->updateStatusbarIns(GetOvertype() == 0);
    pFrame->updateStatusbarEOL((this->GetLineCount() > 1) ? GetEOLMode() : -1);
}

void CodeEdit::OnSetFocus(wxFocusEvent &tEvent)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    tEvent.Skip();

    if ((pFrame->isOpeningFile() == false) && (pFrame->isClosingAll() == false)) {
        updateStatusbar(true);
    }

    m_bFocused = true;
}

void CodeEdit::OnKillFocus(wxFocusEvent &tEvent)
{
    DoCallTipCancel();

    tEvent.Skip();

    m_bFocused = false;
}

void CodeEdit::OnKeyDown(wxKeyEvent &tEvent)
{
    wxStyledTextCtrl::OnKeyDown(tEvent);

    updateStatusbar();
}

void CodeEdit::OnMouseLeftDown(wxMouseEvent &tEvent)
{
    wxStyledTextCtrl::OnMouseLeftDown(tEvent);

    updateStatusbar();
}

void CodeEdit::calcStatusMargin(void)
{
    m_StatusMargin = TextWidth(wxSTC_STYLE_DEFAULT, uT(">")) + 2;
    SetMarginWidth(m_StatusID, m_StatusMargin);
}

bool CodeEdit::isWordEnclosed(int iLine, int iPos)
{
    // Return true if enclosed in string on in comment

    int ll;
    int iLineEnd = GetLineEndPosition(iLine);
    int iLineStart = 0;
    if (iLine > 0) {
        iLineStart = (iLineEnd - (int)(GetLine(iLine).Length()));
    }
    if ((iLineStart < 0) || (iLineStart >= iLineEnd)) {
        return true;
    }
    if ((iPos <= iLineStart) || (iPos > iLineEnd)) {
        return false;
    }

    char_t cNext;

    bool bL = false, bR = false;

    int iLexer = this->GetLexer();

    // Comment?
    int iQuoteS = 0, iQuoteD = 0;
    for (ll = iLineStart; ll <= iPos; ll++) {
        cNext = (char_t)GetCharAt(ll);
        if ((uT(' ') == cNext) || (uT('\t') == cNext) || (uT('\r') == cNext) || (uT('\n') == cNext)) {
            continue;
        }
        if (uT('\'') == cNext) {
            iQuoteS += 1;
            continue;
        }
        if (uT('"') == cNext) {
            iQuoteD += 1;
            continue;
        }
        if (((iQuoteS % 2) == 0) && ((iQuoteD % 2) == 0)) {
            if (iLexer == wxSTC_LEX_LUA) {
                if ((uT('-') == cNext) && (ll < iPos)) {
                    cNext = (char_t)GetCharAt(ll + 1);
                    if (uT('-') == cNext) {
                        return true;
                    }
                }
            }
            else if ((iLexer == wxSTC_LEX_PYTHON) || (iLexer == wxSTC_LEX_BASH) || (iLexer == wxSTC_LEX_MAKEFILE) || (iLexer == wxSTC_LEX_COMET)) {
                if (uT('#') == cNext) {
                    return true;
                }
            }
            else if (iLexer == wxSTC_LEX_CPP) {
                if ((uT('/') == cNext) && (ll < iPos)) {
                    cNext = (char_t)GetCharAt(ll + 1);
                    if ((uT('/') == cNext) || (uT('*') == cNext)) {
                        return true;
                    }
                }
            }
            else if ((iLexer == wxSTC_LEX_LATEX) || (iLexer == wxSTC_LEX_OCTAVE)) {
                if (uT('%') == cNext) {
                    return true;
                }
            }
            else if (iLexer == wxSTC_LEX_FORTRAN) {
                if (uT('!') == cNext) {
                    return true;
                }
            }
        }
    }
    //

    // Open quotes
    if (((iQuoteS % 2) == 1) || ((iQuoteD % 2) == 1)) {
        return true;
    }
    //

    // In string?
    for (ll = iPos; ll >= iLineStart; ll--) {
        cNext = (char_t)GetCharAt(ll);
        if ((uT('\"') == cNext) || (uT('\'') == cNext)) {
            bL = true;
            break;
        }
    }
    for (ll = iPos; ll <= iLineEnd; ll++) {
        cNext = (char_t)GetCharAt(ll);
        if ((uT('\"') == cNext) || (uT('\'') == cNext)) {
            bR = true;
            break;
        }
    }
    //

    return (bL && bR);
}

// Call tips
bool CodeEdit::DoDwellStart(int iPos)
{
    if (m_bPopmenuShown) {
        m_bPopmenuShown = false;
        return false;
    }

    if (iPos < 0) {
        return false;
    }
    int iLine = LineFromPosition(iPos);
    if ((iLine < 0) || (iLine >= GetLineCount())) {
        return false;
    }

    if (this->isWordEnclosed(iLine, iPos)) {
        m_bWordEnclosed = true;
        return false;
    }

    const DwellList *pDwellList = DWELL_API_LUA;
    if (pDwellList[0].len == 0) {
        return false;
    }

    int iLenW;
    int iStart;
    char_t cStart, cNext;
    wxString strT, strInfo;
    bool bFLag = false, bN = false;

    int iLineEnd = GetLineEndPosition(iLine);
    int iLineStart = 0;
    if (iLine > 0) {
        iLineStart = (iLineEnd - (int)(GetLine(iLine).Length()));
    }
    if ((iLineStart < 0) || (iLineStart >= iLineEnd)) {
        return false;
    }

    int ii = 0, ll;
    while (pDwellList[ii].len != 0) {

        iLenW = pDwellList[ii].len;
        cStart = pDwellList[ii].name[0];
        cNext = pDwellList[ii].name[1];
        iStart = wxMax(iLineStart, iPos - iLenW);

        bFLag = false;

        for (ll = iStart; ll < (iStart + iLenW); ll++) {
            bN = true;
            if (ll < (iStart + iLenW - 1)) {
                bN = (cNext == (char_t)GetCharAt(ll + 1));
            }
            if (bN && (cStart == (char_t)GetCharAt(ll))) {
                iStart = ll;
                bFLag = true;
                break;
            }
        }

        if ((bFLag == false) || (iPos < iStart) || (iPos > (iStart + iLenW))) {
            ii += 1;
            continue;
        }

        strT = GetTextRange(iStart, iStart + iLenW);

        // Display the call tips
        if (isSeparator(iStart - 1) && isSeparator(iStart + iLenW) && strT.IsSameAs(pDwellList[ii].name)) {

            // If enclosed in string on in commentary dont show call tip
            bool bL = false, bR = false;
            if ((iPos < iLineEnd) && (iPos > iLineStart)) {
                for (ll = iPos; ll >= iLineStart; ll--) {
                    cNext = (char_t)GetCharAt(ll);
                    if ((uT('\"') == cNext) || (uT('\'') == cNext)) {
                        bL = true;
                        break;
                    }
                    if ((uT('-') == cNext) && (ll > iLineStart)) {
                        cNext = (char_t)GetCharAt(ll - 1);
                        if (uT('-') == cNext) {
                            bL = true;
                            bR = true;
                            break;
                        }
                    }
                }
                if (bR == false) {
                    for (ll = iPos; ll <= iLineEnd; ll++) {
                        cNext = (char_t)GetCharAt(ll);
                        if ((uT('\"') == cNext) || (uT('\'') == cNext)) {
                            bR = true;
                            break;
                        }
                    }
                }
            }

            if (bL && bR) {
                return false;
            }
            //

            strInfo = pDwellList[ii].name;
            strInfo += uT("\n");
            strInfo += pDwellList[ii].info;
            CallTipShow(iPos, strInfo);
            return true;
        }
        ii += 1;
    }

    return false;
}

void CodeEdit::OnDwellStart(wxStyledTextEvent &tEvent)
{
    if ((m_ScintillaPrefs.common.calltipEnable == false) || ((m_iLexer != wxSTC_LEX_LUA) && (m_iLexer != wxSTC_LEX_PYTHON))) {
        return;
    }

    if (GetSTCFocus() == false) {
        DoCallTipCancel();
        return;
    }

    DoDwellStart(tEvent.GetPosition());
}

void CodeEdit::OnDwellEnd(wxStyledTextEvent &WXUNUSED(tEvent))
{
    // Cancel any outstanding call tip
    if (CallTipActive()) {
        CallTipCancel();
    }
}

void CodeEdit::OnUIupdate(wxStyledTextEvent &WXUNUSED(tEvent))
{
    if ((m_ScintillaPrefs.common.syntaxEnable == false) || (wxSTC_LEX_NULL == m_iLexer)) {
        return;
    }

    int iPos = GetCurrentPos();
    char cT = static_cast<char>(GetCharAt(iPos));
    int iPos1 = wxSTC_INVALID_POSITION;

    if ((cT == '(') || (cT == '{') || (cT == '[')) {
        iPos1 = iPos;
    }

    if (iPos1 != wxSTC_INVALID_POSITION) {
        int iPos2 = BraceMatch(iPos1);
        if (iPos2 != wxSTC_INVALID_POSITION) {
            if (iPos2 > (iPos1 + 1)) {
                BraceHighlight(iPos1, iPos2);
            }
            else {
                BraceBadLight(wxSTC_INVALID_POSITION);
            }
        }
        else {
            BraceBadLight(iPos1);
        }
    }
    else {
        BraceBadLight(wxSTC_INVALID_POSITION);
    }
}

bool CodeEdit::BasicCanSearch(wxString &strFind, int *iFindLen, int *iLineCount, int *iCurLine)
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

int CodeEdit::BasicFind(wxString &strFind, int iStyle /* = 0*/, int *iFindPos /* = NULL*/, bool bPrev /* = false*/)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return FIND_PARAMERR;
    }

    int iFindLen, iLineCount, iCurLine;

    if (BasicCanSearch(strFind, &iFindLen, &iLineCount, &iCurLine) == false) {
        return FIND_PARAMERR;
    }

    const int iSelStart = GetSelectionStart();
    const int iSelEnd = GetSelectionEnd();

    int minPos = 0, maxPos = 0;
    int findMinPos = 0, findMaxPos = 0;

    if (bPrev) {
        minPos = iSelStart;
        maxPos = 0;
    }
    else {
        minPos = iSelEnd;
        maxPos = GetLineEndPosition(iLineCount - 1);
    }
    int iPos = FindText(minPos, maxPos, strFind, iStyle, &findMinPos, &findMaxPos);
    if ((iPos >= 0) && (findMinPos < findMaxPos)) {
        SetSelection(findMinPos, findMaxPos);
        if (iFindPos != NULL) {
            *iFindPos = iPos;
        }
        return (FIND_ITEMFOUND + 1);
    }

    return FIND_NOTFOUND;
}
