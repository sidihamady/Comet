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


#ifndef CODE_EDIT_H
#define CODE_EDIT_H

#include "Identifiers.h"

#include "../../LibLua/LuaExt/include/LuaExt.h"

#include <wx/stc/stc.h>
#include <wx/fdrepdlg.h>
#include <wx/filename.h>

#define SCRIPT_STYLE_NORMAL    0
#define SCRIPT_STYLE_BOLD      1
#define SCRIPT_STYLE_ITALIC    2
#define SCRIPT_STYLE_UNDERL    4
#define SCRIPT_STYLE_HIDDEN    8
#define SCRIPT_FOLD_COMMENT    1
#define SCRIPT_FOLD_COMPACT    2
#define SCRIPT_FOLD_PREPROC    4
#define SCRIPT_FOLD_HTML       16
#define SCRIPT_FOLD_HTMLPREP   32
#define SCRIPT_FOLD_COMMENTLUA 64
#define SCRIPT_FOLD_QUOTESLUA  128
#define SCRIPT_FLAG_WRAPMODE   16

#define PAGE_COMMON       uT("Common")
#define PAGE_LANGUAGES    uT("Languages")
#define PAGE_STYLE_TYPES  uT("Style types")
#define STYLE_TYPES_COUNT 32

#define STYLEINFO_COUNT        24
#define STYLEINFO_COUNT_SYNTAX 20

#define STYLEINFO_INDEX_COMMENT_START wxSTC_LUA_COMMENT
#define STYLEINFO_INDEX_COMMENT_END   wxSTC_LUA_COMMENTDOC
#define STYLEINFO_INDEX_STRING_START  wxSTC_LUA_STRING
#define STYLEINFO_INDEX_STRING_END    wxSTC_LUA_LITERALSTRING
#define STYLEINFO_INDEX_LIB_START     wxSTC_LUA_WORD2
#define STYLEINFO_INDEX_LIB_END       wxSTC_LUA_WORD8
#define STYLEINFO_INDEX_LINENUM       (wxSTC_LUA_WORD8 + 1)
#define STYLEINFO_INDEX_CARET         (wxSTC_LUA_WORD8 + 2)
#define STYLEINFO_INDEX_SELECTION     (wxSTC_LUA_WORD8 + 3)
#define STYLEINFO_INDEX_WHITESPACE    (wxSTC_LUA_WORD8 + 4)

#define SCRIPT_MASK_FIRSTBIT      5
#define SCRIPT_MASK_ERROR         0x00000020
#define SCRIPT_MASK_ERRORBIT      5
#define SCRIPT_MASK_DEBUG         0x00000040
#define SCRIPT_MASK_DEBUGBIT      6
#define SCRIPT_MASK_BREAKPOINT    0x00000080
#define SCRIPT_MASK_BREAKPOINTBIT 7
#define SCRIPT_MASK_FIND          0x00000100
#define SCRIPT_MASK_FINDBIT       8
#define SCRIPT_MASK_BOOKMARK      0x00000200
#define SCRIPT_MASK_BOOKMARKBIT   9
#define SCRIPT_MASK_MODIFIED      0x00000400
#define SCRIPT_MASK_MODIFIEDBIT   10
#define SCRIPT_MASK_SAVED         0x00000800
#define SCRIPT_MASK_SAVEDBIT      11
#define SCRIPT_MASK_LASTBIT       11

#define INFOBAR_WIDTH 12

#define LEXERTYPE_FIRST  0
#define LEXERTYPE_SECOND 1
#define LEXERTYPE_THIRD  2
#define LEXERTYPE_FOURTH 3

struct CommonInfo
{
    bool syntaxEnable;
    bool foldEnable;
    bool indentEnable;
    bool readOnlyInitial;
    bool overTypeInitial;
    bool wrapModeInitial;
    bool displayEOLEnable;
    bool indentGuideEnable;
    bool lineNumberEnable;
    bool markerEnable;
    bool statusEnable;
    bool longLineOnEnable;
    bool whiteSpaceEnable;
    bool autocompleteEnable;
    bool calltipEnable;
    int lineSpacing;
    int tabSize;
    bool useTab;
    int longLine;
    unsigned long marckerColorModified;
    unsigned long marckerColorSaved;
    unsigned long marckerColorFind;
    unsigned long marckerColorError;
    int reloadDelay;
};

struct LanguageInfo
{
    const char_t *name;
    const char_t *filepattern;
    int lexer;
    struct
    {
        int set;
        int type;
        const char_t *words;
    } styles[STYLE_TYPES_COUNT];
};

struct UserLanguageInfo
{
    char_t *name;
    char_t *filepattern;
    int lexer;
    struct
    {
        int set;
        int type;
        char_t *words;
    } styles[STYLE_TYPES_COUNT];
};

struct StyleInfo
{
    int id;
    int idl;
    unsigned long foreground;
    unsigned long background;
    char_t fontname[LM_STRSIZES];
    int fontsize;
    int fontstyle;
    int lettercase;
};

struct StyleInfoDefault
{
    const int id;
    const int idl;
    const unsigned long foreground;
    const unsigned long background;
    const char_t *fontname;
    const int fontsize;
    const int fontstyle;
    const int lettercase;
};

struct ScintillaPrefs
{
    ScintillaPrefs()
    {
        loadDefault(*this);
    }
    static void loadDefault(ScintillaPrefs &tScintillaPrefs);
    CommonInfo common;
    StyleInfo style[STYLEINFO_COUNT];
    bool loaded;
};

#define SCINTILLA_AUTOC_MAXSUFFIX 100
struct AutoCList
{
    const int len;
    const char_t *prefix;
    const int maxi;
    const int count;
    const char_t *words;
};

struct CallTipList
{
    const int len;
    const char_t *prefix;
    const char_t *args;
};

struct DwellList
{
    const int len;
    const char_t *name;
    const char_t *info;
};

class CodeInfobar;

class CodeEdit : public wxStyledTextCtrl
{

private:
    wxString m_strAutoCompList;

    bool isSeparator(int iPos)
    {
        char_t cT = uT(' ');
        if (iPos >= 0) {
            cT = GetCharAt(iPos);
            if ((cT == uT(' ')) || (cT == uT('\t')) || (cT == uT('\r')) || (cT == uT('\n')) || (cT == uT('(')) || (cT == uT('|')) || (cT == uT('[')) || (cT == uT('\0'))) {
                cT = uT('|');
            }
        }
        return (cT == uT('|'));
    }

    void setGuierr(const wxString &strT);

protected:
    wxString m_strName;

    int m_LinenumberID;
    int m_MarkerID;
    int m_MarkerMargin;
    int m_FoldingID;
    int m_FoldingMargin;

    int m_StatusID;
    int m_StatusMargin;

    int m_iLexer;
    int m_iLexerType;

    bool m_bWordEnclosed;
    bool m_bPopmenuShown;

    static const int AUTOC_API_LUA_LEN;
    static const AutoCList AUTOC_API_LUA[];
    static const CallTipList CALLTIP_API_LUA[];
    static const DwellList DWELL_API_LUA[];

    static const int AUTOC_API_CPP_LEN;
    static const AutoCList AUTOC_API_CPP[];

    static const int AUTOC_API_C_LEN;
    static const AutoCList AUTOC_API_C[];

    static const int AUTOC_API_PYTHON_LEN;
    static const AutoCList AUTOC_API_PYTHON[];

    static const int AUTOC_API_LATEX_LEN;
    static const AutoCList AUTOC_API_LATEX[];

    static const int AUTOC_API_SOLIS_LEN;
    static const AutoCList AUTOC_API_SOLIS[];

    static const char_t *WORDLIST_LUA;
    static const char_t *WORDLIST_API_LUA;
    static const LanguageInfo LANGPREFS_LUA;

    static const char_t *WORDLIST_CPP;
    static const LanguageInfo LANGPREFS_CPP;

    static const char_t *WORDLIST_C;
    static const LanguageInfo LANGPREFS_C;

    static const char_t *WORDLIST_BASH;
    static const LanguageInfo LANGPREFS_BASH;

    static const char_t *WORDLIST_PYTHON;
    static const LanguageInfo LANGPREFS_PYTHON;

    static const char_t *WORDLIST_OCTAVE;
    static const LanguageInfo LANGPREFS_OCTAVE;

    static const char_t *WORDLIST_FORTRAN;
    static const LanguageInfo LANGPREFS_FORTRAN;

    static const char_t *WORDLIST_LATEX;
    static const LanguageInfo LANGPREFS_LATEX;

    static const char_t *WORDLIST_MAKEFILE;
    static const LanguageInfo LANGPREFS_MAKEFILE;

    static const char_t *WORDLIST_BATCH;
    static const char_t *WORDLIST_BATCH_COMMAND;
    static const LanguageInfo LANGPREFS_BATCH;

    static const char_t *WORDLIST_SOLIS_KEYWORD;
    static const char_t *WORDLIST_SOLIS_KEYWORD2;
    static const LanguageInfo LANGPREFS_SOLIS;

    static const char_t *WORDLIST_SOLISOUTPUT_KEYWORD;
    static const char_t *WORDLIST_SOLISOUTPUT_KEYWORD2;
    static const LanguageInfo LANGPREFS_SOLISOUTPUT;

    static const char_t *WORDLIST_SETTINGS_KEYWORD;
    static const char_t *WORDLIST_SETTINGS_KEYWORD2;
    static const LanguageInfo LANGPREFS_SETTINGS;

    static const char_t *WORDLIST_USERSYNTAX_KEYWORD;
    static const char_t *WORDLIST_USERSYNTAX_KEYWORD2;
    static const LanguageInfo LANGPREFS_USERSYNTAX;

    static const int TYPECOUNT_USER;
    static UserLanguageInfo *PLANGPREFS_USER;

    LanguageInfo const *getLanguageInfo(int iLexer);

    ScintillaPrefs m_ScintillaPrefs;

    double m_fStartTime; // Script start time (millisecond)
    double m_fEndTime;   // Script end time (millisecond)

    bool m_bRunning;

    bool m_bFocused;

    int m_iRunningLine;

    DECLARE_EVENT_TABLE()

public:
    CodeEdit(wxWindow *pParent, wxWindowID idT = ID_WINDOW_SCRIPT,
             const wxPoint &posT = wxDefaultPosition,
             const wxSize &sizeT = wxDefaultSize,
             long styleT = wxNO_BORDER | wxVSCROLL | wxHSCROLL,
             const wxString &strName = wxSTCNameStr,
             int iLexer = wxSTC_LEX_LUA);

    ~CodeEdit();

    static const StyleInfoDefault STYLE_LIGHT[];
    static const StyleInfoDefault STYLE_DARK[];
    static const int KNOWN_LEXER[];
    static const char_t *KNOWN_LEXER_DESCRIPTION[];
    static bool isLexerKnown(int iLexer);
    static int getLexerByIndex(int ll);

    static void createCometLexer(void);
    static void loadCometLexer(const char_t *name, const char_t *filepattern, const char_t *keywords);
    static bool saveCometLexer(char_t *name, char_t *filepattern, char_t *keywords);
    static void deleteCometLexer(void);
    static const char_t *getCometLexerExt(void)
    {
        if (PLANGPREFS_USER != NULL) {
            return static_cast<const char_t *>(PLANGPREFS_USER->filepattern);
        }
        return NULL;
    }

    // >> Communication between threads
    wxMutex *m_pMutex;
    long m_iPlotCreatedId;
    bool m_bReadDone;
    bool m_bAskDone;
    long m_iPlotCount;
    long m_bPlotCreated;
    wxString m_strRead;
    int m_iAnswer;
    LuaEngine *m_pLuaEngine;
    bool m_bGuierrSet;
    // <<

    void setInfobar(wxWindow *pInfobar)
    {
        m_pInfobar = pInfobar;
    }
    wxWindow *getInfobar(void)
    {
        return m_pInfobar;
    }

    bool luaDoRead(int iNum = 0, const wxString &strLabel = uT(""));
    bool luaDoAsk(const wxString &strMessage, const wxString &strTitle = uT("Comet"));
    bool luaDoAlert(const wxString &strMessage, const wxString &strTitle = uT("Comet"));

    void OnMessageReceived(wxCommandEvent &tEvent);

    static long normalizeColor(long iColor, long iDelta);
    static long normalizeColor(long iColor, long iDeltaR, long iDeltaG, long iDeltaB);

    void initCaret(const int iSpacing);
    void initCommonPrefs(void);
    bool setPrefs(const ScintillaPrefs &tPrefs, bool bInit = true);
    void getPrefs(ScintillaPrefs &tPrefs);
    bool initPrefs(void);
    bool initLexer(int iLexer, int iLexerType);

    int getLexerType(void)
    {
        return m_iLexerType;
    }

    wxString getLexerName(void)
    {
        if (isModeScriptConsole()) {
            return m_strName;
        }

        wxString strLexer(uT(""));

        switch (this->GetLexer()) {
            case wxSTC_LEX_LUA:
                strLexer = wxString(uT("Lua"));
                break;
            case wxSTC_LEX_CPP:
                strLexer = wxString(uT("C/C++"));
                break;
            case wxSTC_LEX_BASH:
                strLexer = wxString(uT("Bash"));
                break;
            case wxSTC_LEX_PYTHON:
                strLexer = wxString(uT("Python"));
                break;
            case wxSTC_LEX_OCTAVE:
                strLexer = wxString(uT("Octave"));
                break;
            case wxSTC_LEX_FORTRAN:
                strLexer = wxString(uT("Fortran"));
                break;
            case wxSTC_LEX_LATEX:
                strLexer = wxString((LEXERTYPE_FIRST == m_iLexerType) ? uT("LaTeX") : uT("BibTeX"));
                break;
            case wxSTC_LEX_MAKEFILE:
                strLexer = wxString(uT("Makefile"));
                break;
            case wxSTC_LEX_COMET:
                strLexer = wxString(uT("Comet"));
                break;
            case wxSTC_LEX_BATCH:
                strLexer = wxString(uT("Batch"));
                break;
            case wxSTC_LEX_SOLIS:
                strLexer = wxString((LEXERTYPE_THIRD == m_iLexerType) ? uT("Settings") : ((LEXERTYPE_FOURTH == m_iLexerType) ? uT("UserSyntax") : ((LEXERTYPE_SECOND == m_iLexerType) ? uT("Output") : uT("Solis"))));
                break;
            default:
                break;
        }

        return strLexer;
    }

    bool isWrapEnabled(void)
    {
        return m_ScintillaPrefs.common.wrapModeInitial;
    }

    virtual void DoFindReset(void) = 0;

    bool BasicCanSearch(wxString &strFind, int *iFindLen, int *iLineCount, int *iCurLine);
    int BasicFind(wxString &strFind, int iStyle = 0, int *iFindPos = NULL, bool bPrev = false);

    void DoStyleSetFont(int styleIndex, unsigned long foreColor, unsigned long backColor, int iSize, const char_t *faceName, int fontStyle = 0, int letterCase = 0);

    bool GetUserSettings(const wxString &strFilename);

    void DoCallTipCancel(void);
    void DoHighlightSyntax(bool bEnable);
    void DoEnableAutocomplete(bool bEnable);
    void DoEnableCalltip(bool bEnable);
    void DoScriptAction(int idT);

    int LinenumberID(void) { return m_LinenumberID; }
    int LinenumberMargin(void) { return GetMarginWidth(m_LinenumberID); }
    int MarkerID(void) { return m_MarkerID; }
    int MarkerMargin(void) { return m_MarkerMargin; }
    int FoldingID(void) { return m_FoldingID; }
    int FoldingMargin(void) { return m_FoldingMargin; }

    int StatusID(void) { return m_StatusID; }
    int StatusMargin(void) { return m_StatusMargin; }
    void calcStatusMargin(void);

    bool isFocused(void) { return m_bFocused; }

    bool DoDwellStart(int iPos);
    bool isWordEnclosed(int iLine, int iPos);

    void updateStatusbar(bool bUpdateLexer = false);

    void OnSetFocus(wxFocusEvent &tEvent);
    void OnKillFocus(wxFocusEvent &tEvent);
    void OnKeyDown(wxKeyEvent &tEvent);
    void OnMouseLeftDown(wxMouseEvent &tEvent);
    void OnCharAdded(wxStyledTextEvent &tEvent);
    void OnDwellStart(wxStyledTextEvent &tEvent);
    void OnDwellEnd(wxStyledTextEvent &tEvent);
    void OnUIupdate(wxStyledTextEvent &tEvent);
};

class SigmaBusyCursor
{
private:
    bool m_bStarted;

public:
    SigmaBusyCursor()
    {
        m_bStarted = false;
    }
    ~SigmaBusyCursor()
    {
        if (m_bStarted) {
            ::wxEndBusyCursor();
            m_bStarted = false;
        }
    }

    void start(void)
    {
        m_bStarted = true;
        ::wxBeginBusyCursor(wxHOURGLASS_CURSOR);
    }
};

inline wxString SigmaTrim(const wxString &strT)
{
    wxString strX = strT;

    char_t szT[LM_STRSIZEL];
    szT[LM_STRSIZEL - 1] = uT('\0');
    Tstrncpy(szT, LM_CSTR(strT), LM_STRSIZEL - 1);

    int iLen = lm_trim(szT);

    if (iLen > 0) {
        strX = wxString(static_cast<const char_t*>(szT));
    }

    return strX;
}

#endif
