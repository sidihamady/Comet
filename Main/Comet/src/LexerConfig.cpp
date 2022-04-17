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
#include "LexerConfig.h"
#include "CometApp.h"

#define CONFIG_MAXLINES 2048

int LexerConfig::SECTIONCOUNT = 1;
int LexerConfig::SECTIONSIZE[] = {
    4,
    0
};

// (MAX: 32 sections)
const int LexerConfig::SECTION_ID[] = {
    0,
    -1
};

// (MAX: 32 sections)
char_t const *const LexerConfig::SECTION[] = {
    uT("Lexer"),
    NULL
};

// Common section (MAX: 32 keys)
char_t const *const LexerConfig::KEY[] = {
    uT("Name"),
    uT("FileExt"),
    uT("Keywords"),
    uT("KeywordsB"),
    NULL
};

bool LexerConfig::load(const char_t *pszFilename)
{
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

    // file extension should be cometu
    if ((pszFilename[ilen - 7] != uT('.')) || (pszFilename[ilen - 6] != uT('c')) || (pszFilename[ilen - 5] != uT('o')) || (pszFilename[ilen - 4] != uT('m')) || (pszFilename[ilen - 3] != uT('e')) || (pszFilename[ilen - 2] != uT('t')) || (pszFilename[ilen - 1] != uT('u'))) {

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

    char_t szName[LM_STRSIZE];
    Tmemset(szName, 0, LM_STRSIZE);
    char_t szFilepattern[LM_STRSIZE];
    Tmemset(szFilepattern, 0, LM_STRSIZE);
    char_t szKeywords[LM_STRSIZE];
    Tmemset(szKeywords, 0, LM_STRSIZE);

    if (getValue(uT("Lexer"), uT("Name"), szName) && getValue(uT("Lexer"), uT("FileExt"), szFilepattern) && getValue(uT("Lexer"), uT("Keywords"), szKeywords)) {
        char_t szKeywordsAll[LEXER_STRSIZE];
        Tmemset(szKeywordsAll, 0, LEXER_STRSIZE);
        Tstrcpy(szKeywordsAll, static_cast<const char_t *>(szKeywords));
        char_t szKeywordsB[LM_STRSIZE];
        Tmemset(szKeywordsB, 0, LM_STRSIZE);
        getValue(uT("Lexer"), uT("KeywordsB"), szKeywordsB);
        if (szKeywordsB[0] != uT('\0')) {
            Tstrcat(szKeywordsAll, uT(" "));
            Tstrcat(szKeywordsAll, static_cast<const char_t *>(szKeywordsB));
        }
        CodeEdit::loadCometLexer(static_cast<const char_t *>(szName), static_cast<const char_t *>(szFilepattern),
                                 static_cast<const char_t *>(szKeywordsAll));
    }

    return true;
}

bool LexerConfig::save(const char_t *pszFilename)
{
    char_t szName[LM_STRSIZE];
    Tmemset(szName, 0, LM_STRSIZE);
    char_t szFilepattern[LM_STRSIZE];
    Tmemset(szFilepattern, 0, LM_STRSIZE);
    char_t szKeywordsAll[LEXER_STRSIZE];
    Tmemset(szKeywordsAll, 0, LEXER_STRSIZE);

    if (CodeEdit::saveCometLexer(szName, szFilepattern, szKeywordsAll) == false) {
        return 0;
    }

    return this->save(pszFilename, szName, szFilepattern, szKeywordsAll);
}

bool LexerConfig::save(const char_t *pszFilename, const char_t *pszName, const char_t *pszFilepattern, const char_t *pszKeywords)
{
    char_t szKeywords[LM_STRSIZE];
    Tmemset(szKeywords, 0, LM_STRSIZE);
    char_t szKeywordsB[LM_STRSIZE];
    Tmemset(szKeywordsB, 0, LM_STRSIZE);

    size_t iLen = Tstrlen(pszKeywords);
    if (iLen < LM_STRSIZE) {
        Tstrcpy(szKeywords, pszKeywords);
    }
    else {
        char_t *pszT = (char_t *)(Tstrstr(static_cast<const char_t *>(pszKeywords + (LM_STRSIZE - 32)), uT(" ")));
        if ((pszT != NULL) && (pszT < static_cast<const char_t *>(pszKeywords + (LM_STRSIZE - 1)))) {
            *pszT = uT('\0');
            Tstrncpy(szKeywords, static_cast<const char_t *>(pszKeywords), LM_STRSIZE - 1);
            Tstrncpy(szKeywordsB, static_cast<const char_t *>(pszT + 1), LM_STRSIZE - 1);
        }
        else {
            Tstrncpy(szKeywords, static_cast<const char_t *>(pszKeywords), LM_STRSIZE - 1);
        }
    }

    removeAll();

    int ii, jj;
    lfSection *pSection = NULL;

    for (ii = 0; LexerConfig::SECTION[ii] != NULL; ii++) {
        pSection = addSection(static_cast<const char_t *>(LexerConfig::SECTION[ii]), LexerConfig::SECTIONSIZE[ii]);
        if (pSection == NULL) {
            return false;
        }
    }

    setValue(uT("Lexer"), uT("Name"), static_cast<const char_t *>(pszName));
    setValue(uT("Lexer"), uT("FileExt"), static_cast<const char_t *>(pszFilepattern));
    setValue(uT("Lexer"), uT("Keywords"), static_cast<const char_t *>(szKeywords));
    if (szKeywordsB[0] != uT('\0')) {
        setValue(uT("Lexer"), uT("KeywordsB"), static_cast<const char_t *>(szKeywordsB));
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
