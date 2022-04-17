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

#include "ScriptEdit.h"
#include "EditorConfig.h"
#include "CometApp.h"

#define CONFIG_MAXLINES 2048

int EditorConfig::SECTIONCOUNT = 13;
int EditorConfig::SECTIONSIZE[] = {
    21,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    0
};

// (MAX: 32 sections)
const int EditorConfig::SECTION_ID[] = {
    -1,
    wxSTC_LUA_DEFAULT, wxSTC_LUA_WORD, wxSTC_LUA_NUMBER,
    STYLEINFO_INDEX_STRING_START, STYLEINFO_INDEX_COMMENT_START, wxSTC_LUA_PREPROCESSOR, wxSTC_LUA_IDENTIFIER,
    STYLEINFO_INDEX_LIB_START, STYLEINFO_INDEX_LINENUM, STYLEINFO_INDEX_CARET, STYLEINFO_INDEX_SELECTION,
    STYLEINFO_INDEX_WHITESPACE,
    -1
};

// (MAX: 32 sections)
char_t const *const EditorConfig::SECTION[] = {
    uT("Common"), uT("Default"),
    uT("Keyword"), uT("Number"),
    uT("String"), uT("Comment"),
    uT("Preprocessor"), uT("Identifier"),
    uT("Lib"), uT("LineNumber"),
    uT("Caret"), uT("Selection"),
    uT("Whitespace"),
    NULL
};

// Common section (MAX: 32 keys)
char_t const *const EditorConfig::KEYCOMMON[] = {
    uT("EnableSyntax"), uT("EnableFold"),
    uT("EnableIndent"), uT("DisplayEOL"),
    uT("ShowLinenum"), uT("ShowMarker"),
    uT("ShowStatus"), uT("ShowWhitespace"),
    uT("EnableCompletion"), uT("EnableCalltip"),
    uT("LineSpacing"), uT("UseTab"),
    uT("TabSize"), uT("WrapMode"),
    uT("IndentGuide"),
    uT("EnableLongLine"), uT("LongLine"),
    uT("MarkerColorModified"), uT("MarkerColorSaved"),
    uT("MarkerColorFind"), uT("MarkerColorError"),
    uT("ReloadDelay"),
    NULL
};

// Common section (MAX: 32 keys)
char_t const *const EditorConfig::KEYSTYLE[] = {
    uT("ColorFore"), uT("ColorBack"),
    uT("FontName"), uT("FontSize"),
    uT("FontStyle"), uT("LetterCase"),
    NULL
};

void EditorConfig::copy(ScintillaPrefs &tScintillaPrefsDest, const ScintillaPrefs &tScintillaPrefsSrc)
{
    if (&tScintillaPrefsDest == &tScintillaPrefsSrc) {
        return;
    }

    tScintillaPrefsDest.common.syntaxEnable = tScintillaPrefsSrc.common.syntaxEnable;
    tScintillaPrefsDest.common.foldEnable = tScintillaPrefsSrc.common.foldEnable;
    tScintillaPrefsDest.common.indentEnable = tScintillaPrefsSrc.common.indentEnable;
    tScintillaPrefsDest.common.overTypeInitial = tScintillaPrefsSrc.common.overTypeInitial;
    tScintillaPrefsDest.common.readOnlyInitial = tScintillaPrefsSrc.common.readOnlyInitial;
    tScintillaPrefsDest.common.wrapModeInitial = tScintillaPrefsSrc.common.wrapModeInitial;
    tScintillaPrefsDest.common.displayEOLEnable = tScintillaPrefsSrc.common.displayEOLEnable;
    tScintillaPrefsDest.common.indentGuideEnable = tScintillaPrefsSrc.common.indentGuideEnable;
    tScintillaPrefsDest.common.lineNumberEnable = tScintillaPrefsSrc.common.lineNumberEnable;
    tScintillaPrefsDest.common.markerEnable = tScintillaPrefsSrc.common.markerEnable;
    tScintillaPrefsDest.common.statusEnable = tScintillaPrefsSrc.common.statusEnable;
    tScintillaPrefsDest.common.longLineOnEnable = tScintillaPrefsSrc.common.longLineOnEnable;
    tScintillaPrefsDest.common.longLine = tScintillaPrefsSrc.common.longLine;
    tScintillaPrefsDest.common.whiteSpaceEnable = tScintillaPrefsSrc.common.whiteSpaceEnable;
    tScintillaPrefsDest.common.autocompleteEnable = tScintillaPrefsSrc.common.autocompleteEnable;
    tScintillaPrefsDest.common.calltipEnable = tScintillaPrefsSrc.common.calltipEnable;
    tScintillaPrefsDest.common.lineSpacing = tScintillaPrefsSrc.common.lineSpacing;
    tScintillaPrefsDest.common.useTab = tScintillaPrefsSrc.common.useTab;
    tScintillaPrefsDest.common.tabSize = tScintillaPrefsSrc.common.tabSize;

    tScintillaPrefsDest.common.marckerColorModified = tScintillaPrefsSrc.common.marckerColorModified;
    tScintillaPrefsDest.common.marckerColorSaved = tScintillaPrefsSrc.common.marckerColorSaved;
    tScintillaPrefsDest.common.marckerColorFind = tScintillaPrefsSrc.common.marckerColorFind;
    tScintillaPrefsDest.common.marckerColorError = tScintillaPrefsSrc.common.marckerColorError;

    tScintillaPrefsDest.common.reloadDelay = tScintillaPrefsSrc.common.reloadDelay;

    int ii;

    // Same background for all items, except line number
    unsigned long lBack = tScintillaPrefsSrc.style[wxSTC_LUA_DEFAULT].background;

    for (ii = 0; ii < STYLEINFO_COUNT; ii++) {
        tScintillaPrefsDest.style[ii].id = tScintillaPrefsSrc.style[ii].id;
        tScintillaPrefsDest.style[ii].idl = tScintillaPrefsSrc.style[ii].idl;
        tScintillaPrefsDest.style[ii].foreground = tScintillaPrefsSrc.style[ii].foreground;
        tScintillaPrefsDest.style[ii].background = lBack;
#if defined(__WXMSW__)
        wxStrcpy(tScintillaPrefsDest.style[ii].fontname, tScintillaPrefsSrc.style[ii].fontname);
#else
        wxStrcpy(tScintillaPrefsDest.style[ii].fontname, uT("Monospace"));
#endif
        tScintillaPrefsDest.style[ii].fontsize = tScintillaPrefsSrc.style[ii].fontsize;
        tScintillaPrefsDest.style[ii].fontstyle = tScintillaPrefsSrc.style[ii].fontstyle;
        tScintillaPrefsDest.style[ii].lettercase = tScintillaPrefsSrc.style[ii].lettercase;
    }

    // Line number and Caret
    tScintillaPrefsDest.style[STYLEINFO_INDEX_LINENUM].background = tScintillaPrefsSrc.style[STYLEINFO_INDEX_LINENUM].background;
    tScintillaPrefsDest.style[STYLEINFO_INDEX_CARET].background = tScintillaPrefsSrc.style[STYLEINFO_INDEX_CARET].background;

    // Selection and Whitespace
    tScintillaPrefsDest.style[STYLEINFO_INDEX_SELECTION].background = tScintillaPrefsSrc.style[STYLEINFO_INDEX_SELECTION].background;
    tScintillaPrefsDest.style[STYLEINFO_INDEX_WHITESPACE].foreground = tScintillaPrefsSrc.style[STYLEINFO_INDEX_WHITESPACE].foreground;

    // Operators not colorized
    tScintillaPrefsDest.style[wxSTC_LUA_OPERATOR].foreground = tScintillaPrefsSrc.style[0].foreground;

    tScintillaPrefsDest.loaded = tScintillaPrefsSrc.loaded;
}

bool EditorConfig::load(const char_t *pszFilename, ScintillaPrefs &tScintillaPrefs)
{
    tScintillaPrefs.loaded = false;

    ScintillaPrefs::loadDefault(tScintillaPrefs);

    if (pszFilename == NULL) {
        return false;
    }

    // load from file

    FILE *fp = NULL;
    char_t szCurrentSection[LM_STRSIZE];
    char_t szKey[LM_STRSIZE];
    char_t szValue[LM_STRSIZE];
    char_t szBuffer[LM_STRSIZE];
    char_t *pszDest = NULL, *pszT = NULL;
    int indexT, iLine = 0, ilen = 0;

    ilen = (int)Tstrlen(pszFilename);
    if ((ilen < 9) || (ilen > (LM_STRSIZE - 9))) {
        return false;
    }

    // file extension should be cometr
    if ((pszFilename[ilen - 7] != uT('.')) || (pszFilename[ilen - 6] != uT('c')) || (pszFilename[ilen - 5] != uT('o')) || (pszFilename[ilen - 4] != uT('m')) || (pszFilename[ilen - 3] != uT('e')) || (pszFilename[ilen - 2] != uT('t')) || (pszFilename[ilen - 1] != uT('r'))) {

        return false;
    }
    //

    Tmemset(szCurrentSection, 0, LM_STRSIZE);

    if ((fp = Tfopen((const char_t *)pszFilename, uT("rt"))) == NULL) {
        return false;
    }

    if (Tfgets(szBuffer, LM_STRSIZE, fp) == NULL) {
        fclose(fp);
        fp = NULL;
        return false;
    }
    ilen = trim(szBuffer);
    // Check the header
    if ((ilen < 3) || (ilen >= LM_STRSIZE) || (Tstrcmp(static_cast<const char_t *>(szBuffer), COMET_HEADER_STRING) != 0)) {
        fclose(fp);
        fp = NULL;
        return false;
    }

    removeAll();

    int iSectionCount = 0, iSectionIndex = 0, iLen = 0;
    lfSection *pCurSection = NULL;

    while (Tfgets(szBuffer, LM_STRSIZE - 1, fp) != NULL) {

        ilen = trim(szBuffer);
        if ((ilen < 3) || (ilen >= LM_STRSIZE)) {
            iLine += 1;
            if (iLine > CONFIG_MAXLINES) {
                break;
            }
            continue;
        }

        pszT = szBuffer;
        while (lf_isWhiteChar(*pszT)) {
            pszT += 1;
        }
        iLen = (int)wxStrlen((const char_t *)pszT);
        if (iLen < 3) {
            continue;
        }

        switch (*pszT) {

            case uT('['): // section;
                pszDest = (char_t *)Tstrrchr((const char_t *)pszT, uT(']'));
                if (pszDest == NULL) {
                    break;
                }
                indexT = pszDest - pszT;

                if (*(pszDest + 1) != uT('\0')) {
                    break;
                }

                Tmemcpy(szCurrentSection, pszT + 1, (indexT - 1));
                szCurrentSection[indexT - 1] = uT('\0');
                iSectionIndex = sectionIndex((const char_t *)szCurrentSection);
                if (iSectionIndex < 0) {
                    break;
                }

                pCurSection = addSection((const char_t *)szCurrentSection, sectionRecordCount(iSectionIndex));
                if (pCurSection == NULL) {
                    break;
                }

                iSectionCount += 1;
                break;

            case uT('#'): // comment
            case uT(';'):
                break;

            default: // section content
                if (pCurSection == NULL) {
                    break;
                }

                pszDest = (char_t *)Tstrrchr((const char_t *)pszT, uT('='));
                if (pszDest == NULL) {
                    break;
                }
                indexT = pszDest - pszT;
                if (indexT < 1) {
                    break;
                }
                Tmemcpy(szKey, pszT, indexT);
                szKey[indexT] = uT('\0');
                Tmemcpy(szValue, static_cast<const char_t *>(pszT + indexT + 1), (Tstrlen(pszT) - indexT));

                if (keyIndex(static_cast<const char_t *>(pCurSection->getName()), (const char_t *)szKey) < 0) {
                    break;
                }

                if (setValue(static_cast<const char_t *>(pCurSection->getName()), (const char_t *)szKey, (const char_t *)szValue) == false) {
                    break;
                }

                break;
        }
        iLine += 1;
        if ((iLine > CONFIG_MAXLINES) || (iSectionCount > m_iMaxSectionCount)) {
            break;
        }
    }

    fclose(fp);
    fp = NULL;
    //

    if ((m_iSectionCount < 1) || (m_iSectionCount > m_iMaxSectionCount)) {
        return false;
    }

    char_t szTmp[LM_STRSIZE];
    memset(szTmp, 0, LM_STRSIZE * sizeof(char_t));

    if (getValue(uT("Common"), uT("EnableSyntax"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.syntaxEnable = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.syntaxEnable = true;
        }
    }

    if (getValue(uT("Common"), uT("EnableFold"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.foldEnable = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.foldEnable = true;
        }
    }

    if (getValue(uT("Common"), uT("WrapMode"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.wrapModeInitial = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.wrapModeInitial = true;
        }
    }

    if (getValue(uT("Common"), uT("EnableIndent"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.indentEnable = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.indentEnable = true;
        }
    }

    if (getValue(uT("Common"), uT("DisplayEOL"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.displayEOLEnable = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.displayEOLEnable = true;
        }
    }

    if (getValue(uT("Common"), uT("ShowLinenum"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.lineNumberEnable = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.lineNumberEnable = true;
        }
    }

    if (getValue(uT("Common"), uT("ShowMarker"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.markerEnable = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.markerEnable = true;
        }
    }

    if (getValue(uT("Common"), uT("ShowStatus"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.statusEnable = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.statusEnable = true;
        }
    }

    if (getValue(uT("Common"), uT("ShowWhitespace"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.whiteSpaceEnable = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.whiteSpaceEnable = true;
        }
    }

    if (getValue(uT("Common"), uT("EnableCompletion"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.autocompleteEnable = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.autocompleteEnable = true;
        }
    }

    if (getValue(uT("Common"), uT("EnableCalltip"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.calltipEnable = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.calltipEnable = true;
        }
    }

    if (getValue(uT("Common"), uT("LineSpacing"), szTmp)) {
        if (wxStrcmp(uT("1.00"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.lineSpacing = 0;
        }
        else if (wxStrcmp(uT("1.25"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.lineSpacing = 1;
        }
        else if (wxStrcmp(uT("1.50"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.lineSpacing = 2;
        }
        else if (wxStrcmp(uT("1.75"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.lineSpacing = 3;
        }
    }

    if (getValue(uT("Common"), uT("UseTab"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.useTab = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.useTab = true;
        }
    }

    if (getValue(uT("Common"), uT("IndentGuide"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.indentGuideEnable = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.indentGuideEnable = true;
        }
    }

    if (getValue(uT("Common"), uT("EnableLongLine"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.longLineOnEnable = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.longLineOnEnable = true;
        }
    }

    int iT;

    if (getValue(uT("Common"), uT("LongLine"), szTmp)) {
        iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
        if ((iT >= 10) && (iT <= 1024)) {
            tScintillaPrefs.common.longLine = iT;
        }
    }

    if (getValue(uT("Common"), uT("TabSize"), szTmp)) {
        if (wxStrcmp(uT("3"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.tabSize = 3;
        }
        else if (wxStrcmp(uT("4"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.tabSize = 4;
        }
        else if (wxStrcmp(uT("6"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.tabSize = 6;
        }
        else if (wxStrcmp(uT("8"), (const char_t *)szTmp) == 0) {
            tScintillaPrefs.common.tabSize = 8;
        }
    }

    if (getValue(uT("Common"), uT("ReloadDelay"), szTmp)) {
        iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
        if ((iT >= 0) && (iT <= 28800)) {
            if (iT < 3) {
                iT = 3;
            }
            else if (iT > 3600) {
                iT = 3600;
            }
            tScintillaPrefs.common.reloadDelay = iT;
        }
    }

    if (getValue(uT("Common"), uT("MarkerColorModified"), szTmp)) {
        iT = (int)wxStrtoul((const char_t *)szTmp, (char_t **)NULL, 16);
        tScintillaPrefs.common.marckerColorModified = iT;
    }
    if (getValue(uT("Common"), uT("MarkerColorSaved"), szTmp)) {
        iT = (int)wxStrtoul((const char_t *)szTmp, (char_t **)NULL, 16);
        tScintillaPrefs.common.marckerColorSaved = iT;
    }
    if (getValue(uT("Common"), uT("MarkerColorFind"), szTmp)) {
        iT = (int)wxStrtoul((const char_t *)szTmp, (char_t **)NULL, 16);
        tScintillaPrefs.common.marckerColorFind = iT;
    }
    if (getValue(uT("Common"), uT("MarkerColorError"), szTmp)) {
        iT = (int)wxStrtoul((const char_t *)szTmp, (char_t **)NULL, 16);
        tScintillaPrefs.common.marckerColorError = iT;
    }

    char_t *pszV = NULL;
    unsigned long lT = 0L;
    int idT = 0;
    int iErr = 0;

    int ii;

    if (getValue(uT("Default"), uT("ColorFore"), szTmp)) {
        lT = wxStrtoul((const char_t *)szTmp, (char_t **)NULL, 16);
        tScintillaPrefs.style[0].foreground = lT;
    }
    if (getValue(uT("Default"), uT("ColorBack"), szTmp)) {
        lT = wxStrtoul((const char_t *)szTmp, (char_t **)NULL, 16);
        tScintillaPrefs.style[0].foreground = lT;
    }

    if (getValue(uT("Default"), uT("FontName"), szTmp)) {
        wxStrncpy(tScintillaPrefs.style[idT].fontname, (const char_t *)szTmp, LM_STRSIZES - 1);
    }

    if (getValue(uT("Default"), uT("FontSize"), szTmp)) {
        iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
        tScintillaPrefs.style[0].fontsize = iT;
        if ((iT < 6) || (iT > 96)) {
            ScintillaPrefs::loadDefault(tScintillaPrefs);
            return false;
        }
    }
    if (getValue(uT("Default"), uT("FontStyle"), szTmp)) {
        iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
        tScintillaPrefs.style[0].fontstyle = iT;
    }
    if (getValue(uT("Default"), uT("LetterCase"), szTmp)) {
        iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
        tScintillaPrefs.style[0].lettercase = iT;
    }

    for (ii = 1; EditorConfig::SECTION[ii] != NULL; ii++) {

        idT = EditorConfig::SECTION_ID[ii];
        if (idT < 0) {
            break;
        }

        tScintillaPrefs.style[idT].id = idT;

        pszV = (char_t *)(EditorConfig::SECTION[ii]);

        if (getValue((const char_t *)pszV, uT("ColorFore"), szTmp)) {
            lT = wxStrtoul((const char_t *)szTmp, (char_t **)NULL, 16);
            tScintillaPrefs.style[idT].foreground = lT;
        }
        if (getValue((const char_t *)pszV, uT("ColorBack"), szTmp)) {
            lT = wxStrtoul((const char_t *)szTmp, (char_t **)NULL, 16);
            tScintillaPrefs.style[idT].background = lT;
        }

        if (getValue((const char_t *)pszV, uT("FontName"), szTmp)) {
            wxStrncpy(tScintillaPrefs.style[idT].fontname, (const char_t *)szTmp, LM_STRSIZES - 1);
        }
        iT = 0;
        if (getValue((const char_t *)pszV, uT("FontSize"), szTmp)) {
            iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
            tScintillaPrefs.style[idT].fontsize = iT;
            if ((iT < 6) || (iT > 96)) {
                iErr += 1;
            }
        }
        if (getValue((const char_t *)pszV, uT("FontStyle"), szTmp)) {
            iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
            tScintillaPrefs.style[idT].fontstyle = iT;
        }
        if (getValue((const char_t *)pszV, uT("LetterCase"), szTmp)) {
            iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
            tScintillaPrefs.style[idT].lettercase = iT;
        }
    }

    iT = tScintillaPrefs.style[STYLEINFO_INDEX_COMMENT_START].foreground;
    for (int ii = STYLEINFO_INDEX_COMMENT_START; ii <= STYLEINFO_INDEX_COMMENT_END; ii++) {
        tScintillaPrefs.style[ii].foreground = iT;
    }

    iT = tScintillaPrefs.style[STYLEINFO_INDEX_STRING_START].foreground;
    for (int ii = STYLEINFO_INDEX_STRING_START; ii <= STYLEINFO_INDEX_STRING_END; ii++) {
        tScintillaPrefs.style[ii].foreground = iT;
    }

    iT = tScintillaPrefs.style[STYLEINFO_INDEX_LIB_START].foreground;
    for (int ii = STYLEINFO_INDEX_LIB_START; ii <= STYLEINFO_INDEX_LIB_END; ii++) {
        tScintillaPrefs.style[ii].foreground = iT;
    }

    for (int ii = wxSTC_LUA_DEFAULT + 1; ii < STYLEINFO_COUNT_SYNTAX; ii++) {
        wxStrcpy(tScintillaPrefs.style[ii].fontname, tScintillaPrefs.style[wxSTC_LUA_DEFAULT].fontname);
        tScintillaPrefs.style[ii].fontsize = tScintillaPrefs.style[wxSTC_LUA_DEFAULT].fontsize;
        tScintillaPrefs.style[ii].background = tScintillaPrefs.style[wxSTC_LUA_DEFAULT].background;
    }

    tScintillaPrefs.loaded = true;
    return true;
}

bool EditorConfig::save(const char_t *pszFilename, const ScintillaPrefs &tScintillaPrefs)
{
    removeAll();

    int ii, jj;
    lfSection *pSection = NULL;

    for (ii = 0; EditorConfig::SECTION[ii] != NULL; ii++) {
        pSection = addSection(static_cast<const char_t *>(EditorConfig::SECTION[ii]), EditorConfig::SECTIONSIZE[ii]);
        if (pSection == NULL) {
            return false;
        }
    }

    setValue(uT("Common"), uT("EnableSyntax"), tScintillaPrefs.common.syntaxEnable ? uT("1") : uT("0"));
    setValue(uT("Common"), uT("EnableFold"), tScintillaPrefs.common.foldEnable ? uT("1") : uT("0"));
    setValue(uT("Common"), uT("WrapMode"), tScintillaPrefs.common.wrapModeInitial ? uT("1") : uT("0"));
    setValue(uT("Common"), uT("EnableIndent"), tScintillaPrefs.common.indentEnable ? uT("1") : uT("0"));
    setValue(uT("Common"), uT("DisplayEOL"), tScintillaPrefs.common.displayEOLEnable ? uT("1") : uT("0"));
    setValue(uT("Common"), uT("ShowLinenum"), tScintillaPrefs.common.lineNumberEnable ? uT("1") : uT("0"));
    setValue(uT("Common"), uT("ShowMarker"), tScintillaPrefs.common.markerEnable ? uT("1") : uT("0"));
    setValue(uT("Common"), uT("ShowStatus"), tScintillaPrefs.common.statusEnable ? uT("1") : uT("0"));
    setValue(uT("Common"), uT("ShowWhitespace"), tScintillaPrefs.common.whiteSpaceEnable ? uT("1") : uT("0"));
    setValue(uT("Common"), uT("EnableCompletion"), tScintillaPrefs.common.autocompleteEnable ? uT("1") : uT("0"));
    setValue(uT("Common"), uT("EnableCalltip"), tScintillaPrefs.common.calltipEnable ? uT("1") : uT("0"));

    if (tScintillaPrefs.common.lineSpacing == 0) {
        setValue(uT("Common"), uT("LineSpacing"), uT("1.00"));
    }
    else if (tScintillaPrefs.common.lineSpacing == 1) {
        setValue(uT("Common"), uT("LineSpacing"), uT("1.25"));
    }
    else if (tScintillaPrefs.common.lineSpacing == 2) {
        setValue(uT("Common"), uT("LineSpacing"), uT("1.50"));
    }
    else if (tScintillaPrefs.common.lineSpacing == 3) {
        setValue(uT("Common"), uT("LineSpacing"), uT("1.75"));
    }
    setValue(uT("Common"), uT("UseTab"), tScintillaPrefs.common.useTab ? uT("1") : uT("0"));
    setValue(uT("Common"), uT("IndentGuide"), tScintillaPrefs.common.indentGuideEnable ? uT("1") : uT("0"));
    if (tScintillaPrefs.common.tabSize == 3) {
        setValue(uT("Common"), uT("TabSize"), uT("3"));
    }
    else if (tScintillaPrefs.common.tabSize == 4) {
        setValue(uT("Common"), uT("TabSize"), uT("4"));
    }
    else if (tScintillaPrefs.common.tabSize == 6) {
        setValue(uT("Common"), uT("TabSize"), uT("6"));
    }
    else if (tScintillaPrefs.common.tabSize == 8) {
        setValue(uT("Common"), uT("TabSize"), uT("8"));
    }

    char_t szValue[LM_STRSIZEN];
    Tmemset(szValue, 0, LM_STRSIZEN);

    Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tScintillaPrefs.common.reloadDelay);
    setValue(uT("Common"), uT("ReloadDelay"), (const char_t *)szValue);

    setValue(uT("Common"), uT("EnableLongLine"), tScintillaPrefs.common.longLineOnEnable ? uT("1") : uT("0"));
    Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tScintillaPrefs.common.longLine);
    setValue(uT("Common"), uT("LongLine"), (const char_t *)szValue);

    Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%06lX"), tScintillaPrefs.common.marckerColorModified);
    setValue(uT("Common"), uT("MarkerColorModified"), (const char_t *)szValue);
    Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%06lX"), tScintillaPrefs.common.marckerColorSaved);
    setValue(uT("Common"), uT("MarkerColorSaved"), (const char_t *)szValue);
    Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%06lX"), tScintillaPrefs.common.marckerColorFind);
    setValue(uT("Common"), uT("MarkerColorFind"), (const char_t *)szValue);
    Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%06lX"), tScintillaPrefs.common.marckerColorError);
    setValue(uT("Common"), uT("MarkerColorError"), (const char_t *)szValue);

    char_t *pszT = NULL;
    int idT;

    for (ii = 1; EditorConfig::SECTION[ii] != NULL; ii++) {

        pszT = (char_t *)(EditorConfig::SECTION[ii]);
        idT = EditorConfig::SECTION_ID[ii];

        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%06lX"), tScintillaPrefs.style[idT].foreground);
        setValue((const char_t *)pszT, uT("ColorFore"), (const char_t *)szValue);

        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%06lX"), tScintillaPrefs.style[idT].background);
        setValue((const char_t *)pszT, uT("ColorBack"), (const char_t *)szValue);

        setValue((const char_t *)pszT, uT("FontName"), static_cast<const char_t *>(tScintillaPrefs.style[idT].fontname));

        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tScintillaPrefs.style[idT].fontsize);
        setValue((const char_t *)pszT, uT("FontSize"), (const char_t *)szValue);

        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tScintillaPrefs.style[idT].fontstyle);
        setValue((const char_t *)pszT, uT("FontStyle"), (const char_t *)szValue);

        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tScintillaPrefs.style[idT].lettercase);
        setValue((const char_t *)pszT, uT("LetterCase"), (const char_t *)szValue);
    }

    FILE *fp = NULL;

    if ((fp = Tfopen(pszFilename, uT("w"))) == NULL) {
        return false;
    }

    wxString strHeader = CometApp::getConfigHeader();
    Tfputs(LM_CSTR(strHeader), fp);

// For unicode char, I need to specify %ls in the format
#ifdef __WXMSW__
    const char_t *pszFormatSection = uT("[%s]\n");
    const char_t *pszFormatItem = uT("   %s=%s\n");
#else
    const char_t *pszFormatSection = uT("[%ls]\n");
    const char_t *pszFormatItem = uT("   %ls=%ls\n");
#endif

    for (ii = 0; ii < getSectionCount(); ii++) {
        pSection = getSection(ii);
        if (pSection == NULL) {
            fclose(fp);
            fp = NULL;
            return false;
        }
        Tfprintf(fp, pszFormatSection, static_cast<const char_t *>(pSection->getName()));
        for (jj = 0; jj < pSection->getRecordCount(); jj++) {
            Tfprintf(fp, pszFormatItem, static_cast<const char_t *>(pSection->getRecord(jj)->getKey()), static_cast<const char_t *>(pSection->getRecord(jj)->getValue()));
        }
        Tfprintf(fp, uT("\n"));
    }

    fclose(fp);
    fp = NULL;
    return true;
}
