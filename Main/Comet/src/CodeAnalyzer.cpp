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
#include "CodeAnalyzer.h"

const int CodeAnalyzer::MAXCOUNT = 256;

CodeAnalyzer::CodeAnalyzer()
{
    m_pData = NULL;
    m_strFilename = wxEmptyString;
    m_iLang = CodeAnalyzer::LANGUAGE_LUA;
    m_iMainLine = -1;
}

CodeAnalyzer::~CodeAnalyzer()
{
    reset();
}

void CodeAnalyzer::reset(void)
{
    if (m_pData == NULL) {
        return;
    }

    int iCount = (int)(m_pData->size());
    if (iCount > 0) {
        for (int ii = 0; ii < iCount; ii++) {
            if ((*m_pData)[ii]) {
                delete ((*m_pData)[ii]);
                (*m_pData)[ii] = NULL;
            }
        }
        m_pData->clear();
    }
    delete m_pData;
    m_pData = NULL;
    m_iMainLine = -1;
}

int CodeAnalyzer::getIndex(const char_t *funcName, bool bComplete /* = true*/)
{
    if ((funcName == NULL) || (*funcName == uT('\0')) || (getCount() < 1)) {
        return -1;
    }
    char_t *pszF = NULL, *pszT = NULL;

    for (int ii = 0; ii < getCount(); ii++) {
        if (bComplete) {
            if (Tstrcmp(funcName, static_cast<const char_t *>(((*m_pData)[ii])->name)) == 0) {
                return ii;
            }
        }
        else {
            pszF = static_cast<char_t *>(((*m_pData)[ii])->name);
            pszT = Tstrstr(pszF, funcName);
            if (pszT == pszF) {
                return ii;
            }
        }
    }
    return -1;
}

bool CodeAnalyzer::analyzeLine(char_t *pszLine, int iLen, int *piId, int *piLine, int *piBalanceFunc, int *piBalanceLoop)
{
    if (m_pData == NULL) {
        m_pData = new (std::nothrow) std::vector<AnalyzerElement *>;
    }

    if (m_pData == NULL) {
        return false;
    }

    if (iLen < 0) {
        iLen = lm_trim(pszLine);
    }

    // Empty line?
    if ((iLen < 2) || (pszLine[0] == uT('\0')) || (pszLine[1] == uT('\0')) || (pszLine[2] == uT('\0'))) {
        *piLine += 1;
        return true;
    }

    if ((m_iLang != CodeAnalyzer::LANGUAGE_LATEX) && (m_iLang != CodeAnalyzer::LANGUAGE_SOLIS) && !Tisalpha(pszLine[0])) {
        *piLine += 1;
        return true;
    }

    if ((m_iLang == CodeAnalyzer::LANGUAGE_LATEX) && (pszLine[0] != uT('\\'))) {
        *piLine += 1;
        return true;
    }

    if ((m_iLang == CodeAnalyzer::LANGUAGE_SOLIS) && (pszLine[0] != uT('['))) {
        *piLine += 1;
        return true;
    }

    char_t *pszT = NULL, *pszF = NULL, *pszStart = NULL, *pszEnd = NULL, cT;

    AnalyzerElement::SCOPE funcScope = AnalyzerElement::SCOPE_GLOBAL;

    int funcLine;
    char_t funcName[ANALYZER_NAMESIZE];
    Tmemset(funcName, 0, ANALYZER_NAMESIZE);

    int iT = 0;

    if (m_iLang == CodeAnalyzer::LANGUAGE_PYTHON) {

        if (iLen <= 7) {
            *piLine += 1;
            return true;
        }

        if ((pszLine[0] == uT('d')) && (pszLine[1] == uT('e')) && (pszLine[2] == uT('f')) && CodeAnalyzer::isSeparator(pszLine[3]) && (pszLine[iLen - 1] == uT(':'))) {

            pszT = &pszLine[3];
            while (CodeAnalyzer::isSeparator(*pszT)) {
                pszT += 1;
            }

            if (pszT && *pszT && (Tisalpha(*pszT) || ((*pszT) == uT('_')))) {

                pszStart = pszT;
                while (Tisalnum(*pszT) || ((*pszT) == uT('_'))) {
                    pszT += 1;
                }

                if (pszT && *pszT && ((*pszT) == uT('('))) {
                    pszT = Tstrstr(pszT + 1, uT(")"));
                    if (pszT && *pszT) {
                        pszT += 1;
                        if (CodeAnalyzer::isSeparator(*pszT)) {
                            while (CodeAnalyzer::isSeparator(*pszT)) {
                                pszT += 1;
                            }
                        }
                        if (pszT && *pszT && ((*pszT) == uT(':'))) {
                            *pszT = uT('\0');
                            Tstrncpy(funcName, static_cast<const char_t *>(pszStart), ANALYZER_NAMESIZE - 1);
                        }
                    }
                }
            }
        }

        if (funcName[0] != uT('\0')) {

            *piId += 1;
            funcLine = *piLine;

            AnalyzerElement *pFunc = new (std::nothrow) AnalyzerElement(*piId, funcScope, AnalyzerElement::ETYPE_FUNC, funcLine, static_cast<const char_t *>(funcName));
            if (pFunc == NULL) {
                *piLine += 1;
                return false;
            }

            m_pData->push_back(pFunc);
        }

        *piLine += 1;
        return true;
    }

    else if (m_iLang == CodeAnalyzer::LANGUAGE_LATEX) {

        if (iLen <= 10) {
            *piLine += 1;
            return true;
        }

        // Section or Subsection?
        if ((pszLine[0] != uT('\\')) || (pszLine[1] != uT('s')) || (pszLine[iLen - 1] != uT('}'))) {
            *piLine += 1;
            return true;
        }

        iT = 0;

        const int secsize[3] = { 8, 11, 14 };
        const int iZ = (iLen > secsize[2]) ? secsize[2] : iLen;
        cT = pszLine[iZ];
        pszLine[iZ] = uT('\0');

        pszT = Tstrstr(pszLine, uT("\\section"));
        if (pszT == NULL) {
            pszT = Tstrstr(pszLine, uT("\\subsection"));
            if (pszT == NULL) {
                pszT = Tstrstr(pszLine, uT("\\subsubsection"));
                if (pszT != NULL) {
                    iT = secsize[2];
                }
            }
            else {
                iT = secsize[1];
            }
        }
        else {
            iT = secsize[0];
        }

        pszLine[iZ] = cT;

        if (pszT != pszLine) {
            *piLine += 1;
            return true;
        }

        pszT += iT;

        if ((*pszT != uT('{')) && (*pszT != uT('['))) {
            *piLine += 1;
            return true;
        }
        char_t cQstart = *pszT;
        char_t cQend = uT('}');
        if (cQstart == uT('[')) {
            cQend = uT(']');
        }

        pszT += 1;
        pszStart = pszT;
        while (*pszT != cQend) {
            pszT += 1;
        }
        if (pszT && (*pszT == cQend)) {
            if ((pszT - pszStart) > 0) {
                *pszT = uT('\0');
                Tstrncpy(funcName, static_cast<const char_t *>(pszStart), ANALYZER_NAMESIZE - 1);
                *piId += 1;
                funcLine = *piLine;
                AnalyzerElement *pFunc = new (std::nothrow) AnalyzerElement(*piId, funcScope, AnalyzerElement::ETYPE_FUNC, funcLine, static_cast<const char_t *>(funcName));
                if (pFunc != NULL) {
                    m_pData->push_back(pFunc);
                }
            }
        }

        *piLine += 1;
        return true;
    }

    else if (m_iLang == CodeAnalyzer::LANGUAGE_C) {

        if (iLen <= 10) {
            *piLine += 1;
            return true;
        }

        // Comment, Preproc, ...?
        if (!Tisspace(pszLine[0]) && !Tisalpha(pszLine[0])) {
            *piLine += 1;
            return true;
        }

        if (pszLine[iLen - 1] == uT(';')) {
            *piLine += 1;
            return true;
        }

        const char_t *szItems[] = { uT("while"), uT("catch"), uT("else"), uT("switch"), uT("throw"),
                                    uT("if"), uT("for"), uT("return"), uT("sizeof"), uT("struct"), uT("union"),
                                    NULL };
        size_t iRet = 0;
        for (int ii = 0; szItems[ii] != NULL; ii++) {
            iRet = Tstartwith(static_cast<const char_t *>(pszLine), szItems[ii]);
            if ((iRet > 0) && !Tisalnum(pszLine[iRet]) && (pszLine[iRet] != uT('_'))) {
                *piLine += 1;
                return true;
            }
        }

        bool bF = false;
        m_iMainLine = -1;
        int iB = 0, iF = 0;
        for (int ii = (iLen - 1); ii >= 5; ii--) {
            if (pszLine[ii] == uT('(')) {
                iF = ii - 1;
                for (int jj = iF; jj >= 0; jj--) {
                    if (!Tisspace(pszLine[jj])) {
                        iF = jj;
                        break;
                    }
                }
                if (iF < 4) {
                    break;
                }
                for (int jj = iF; jj >= 0; jj--) {
                    if (Tisspace(pszLine[jj])) {
                        iB = jj + 1;
                        break;
                    }
                }
                if (iB <= 1) {
                    break;
                }

                bF = true;

                for (int jj = iB - 1; jj >= 0; jj--) {
                    if (!Tisspace(pszLine[jj]) && !Tisalnum(pszLine[jj]) && (pszLine[jj] != uT('_')) && (pszLine[jj] != uT('*')) && (pszLine[jj] != uT('&')) && (pszLine[jj] != uT('\t'))) {
                        bF = false;
                        break;
                    }
                }

                if (bF) {
                    for (int jj = ii + 1; jj < iLen; jj++) {
                        if (!Tisspace(pszLine[jj]) && !Tisalnum(pszLine[jj]) && (pszLine[jj] != uT('_')) && (pszLine[jj] != uT('\t')) && (pszLine[jj] != uT('\r')) && (pszLine[jj] != uT('\n')) && (pszLine[jj] != uT(',')) && (pszLine[jj] != uT('*')) && (pszLine[jj] != uT('&')) && (pszLine[jj] != uT('[')) && (pszLine[jj] != uT(']')) && (pszLine[jj] != uT(')'))) {
                            bF = false;
                            break;
                        }
                    }
                }

                if (bF && (pszLine[iB] == uT('m')) && (pszLine[iB + 1] == uT('a')) && (pszLine[iB + 2] == uT('i')) && (pszLine[iB + 3] == uT('n'))) {
                    m_iMainLine = *piLine;
                }

                break;
            }
        }

        if ((bF == false) || (iB >= (iF - 1))) {
            *piLine += 1;
            return true;
        }

        pszT = Tstrstr(pszLine + iF + 1, uT(")"));
        char_t cc = 0;
        if (pszT != NULL) {
            cc = *(pszT + 1);
            *(pszT + 1) = uT('\0');
            Tstrncpy(funcName, static_cast<const char_t *>(pszLine), ANALYZER_NAMESIZE - 1);
            *(pszT + 1) = cc;
        }
        else {
            const int ii = (iLen > ANALYZER_NAMESIZE) ? (ANALYZER_NAMESIZE - 1) : (iLen - 1);
            const int is = iF + 2;
            for (int jj = is; jj <= ii; jj++) {
                if (!Tisspace(pszLine[jj]) && !Tisalnum(pszLine[jj]) && (pszLine[jj] != uT('_')) && (pszLine[jj] != uT(','))) {
                    break;
                }
                if (pszLine[jj] == uT(',')) {
                    iF = jj;
                }
            }
            cc = *(pszLine + iF + 1);
            *(pszLine + iF + 1) = uT('\0');
            Tstrncpy(funcName, static_cast<const char_t *>(pszLine), iF + 2);
            *(pszLine + iF + 1) = cc;
        }

        *piId += 1;
        funcLine = *piLine;

        AnalyzerElement *pFunc = new (std::nothrow) AnalyzerElement(*piId, funcScope, AnalyzerElement::ETYPE_FUNC, funcLine, static_cast<const char_t *>(funcName));
        if (pFunc == NULL) {
            *piLine += 1;
            return false;
        }

        m_pData->push_back(pFunc);
        *piLine += 1;
        return true;
    }

    else if (m_iLang == CodeAnalyzer::LANGUAGE_SOLIS) {

        if (iLen <= 6) {
            *piLine += 1;
            return true;
        }

        // Section?
        if (pszLine[0] != uT('[')) {
            *piLine += 1;
            return true;
        }

        iT = 0;

        pszT = Tstrstr(pszLine, uT("]"));
        if ((pszT == NULL) || ((int)(pszT - pszLine) < 6) || ((int)(pszT - pszLine) > 32)) {
            *piLine += 1;
            return true;
        }
        for (char_t *pzi = pszLine + 1; pzi < pszT; pzi++) {
            if (!Tisalpha(*pzi)) {
                *piLine += 1;
                return true;
            }
        }

        cT = *pszT;
        *pszT = uT('\0');
        Tstrncpy(funcName, static_cast<const char_t *>(pszLine + 1), ANALYZER_NAMESIZE - 1);
        *pszT = cT;

        if (funcName[0] != uT('\0')) {

            *piId += 1;
            funcLine = *piLine;

            AnalyzerElement *pFunc = new (std::nothrow) AnalyzerElement(*piId, funcScope, AnalyzerElement::ETYPE_FUNC, funcLine, static_cast<const char_t *>(funcName));
            if (pFunc == NULL) {
                *piLine += 1;
                return false;
            }

            m_pData->push_back(pFunc);
        }

        *piLine += 1;
        return true;
    }

    // >> Lua

    if (iLen == 3) {
        if (Tstrcmp(pszLine, uT("end")) == 0) {
            if (*piBalanceLoop > 0) {
                *piBalanceLoop -= 1;
            }
            else if (*piBalanceFunc > 0) {
                *piBalanceFunc -= 1;
            }
        }
        *piLine += 1;
        return true;
    }

    // if, for, while, repeat
    iT = 0;
    if ((pszT = Tstrstr(pszLine, uT("if"))) != NULL) {
        iT = 2;
    }
    else if ((pszT = Tstrstr(pszLine, uT("for"))) != NULL) {
        iT = 3;
    }
    else if ((pszT = Tstrstr(pszLine, uT("while"))) != NULL) {
        iT = 5;
    }
    else if ((pszT = Tstrstr(pszLine, uT("repeat"))) != NULL) {
        iT = 6;
    }

    if (iT > 0) {
        if (CodeAnalyzer::isSeparator(*(pszT + iT))) {
            *piBalanceLoop += 1;
            pszT = Tstrstr(pszT + iT, uT("end"));
            if (pszT) {
                if ((pszT == (pszLine + iLen - 3)) && CodeAnalyzer::isSeparator(*(pszT - 1))) {
                    *piBalanceLoop -= 1;
                }
            }
        }
        *piLine += 1;
        return true;
    }

    if ((pszT = Tstrstr(pszLine, uT("until"))) != NULL) {
        iT = 5;
        if (CodeAnalyzer::isSeparator(*(pszT + iT))) {
            *piBalanceLoop -= 1;
        }
        *piLine += 1;
        return true;
    }

    if ((*piBalanceFunc < 0) || (*piBalanceLoop < 0)) {
        return false;
    }

    // function f()
    if (iLen < 12) {
        *piLine += 1;
        return true;
    }

    funcName[0] = uT('\0');
    funcScope = AnalyzerElement::SCOPE_GLOBAL;

    pszT = Tstrstr(pszLine, uT("local"));
    if (pszT) {
        if (CodeAnalyzer::isSeparator(*(pszT + 5))) {
            pszT += 5;
            funcScope = AnalyzerElement::SCOPE_LOCAL;
        }
        else {
            *piLine += 1;
            return true;
        }
    }
    else {
        pszT = pszLine;
    }

    pszT = Tstrstr(pszT, uT("function"));
    if (pszT) {

        if ((*(pszT + 8)) == uT('(')) {

            // :BEGIN: name = function(arg)
            pszStart = pszT;
            pszT += 9;
            pszT = Tstrstr(pszT, uT(")"));
            if (pszT == NULL) {
                *piLine += 1;
                return true;
            }
            pszEnd = pszT + 1;
            pszT = pszLine;
            while (Tisalnum(*pszT) || ((*pszT) == uT('_')) || ((*pszT) == uT('.')) || ((*pszT) == uT(':'))) {
                pszT += 1;
            }
            if (pszT && *pszT && (pszT < (pszEnd - 10))) {
                while (CodeAnalyzer::isSeparator(*pszT)) {
                    pszT += 1;
                }
                if ((*pszT) == uT('=')) {
                    pszT += 1;
                    while (CodeAnalyzer::isSeparator(*pszT)) {
                        pszT += 1;
                    }
                    if (pszT == pszStart) {
                        cT = *pszEnd;
                        *pszEnd = uT('\0');
                        Tstrncpy(funcName, static_cast<const char_t *>(pszLine), ANALYZER_NAMESIZE - 1);
                        *pszEnd = cT;
                    }
                }
            }
            // :END: name = function(arg)
        }
        else if (CodeAnalyzer::isSeparator(*(pszT + 8))) {

            pszT += 8;
            while (CodeAnalyzer::isSeparator(*pszT)) {
                pszT += 1;
            }

            if ((*pszT) == uT('(')) {

                // :BEGIN: name = function(arg)
                pszStart = pszT;
                pszT += 9;
                pszT = Tstrstr(pszT, uT(")"));
                if (pszT == NULL) {
                    *piLine += 1;
                    return true;
                }
                pszEnd = pszT + 1;
                pszT = pszLine;
                while (Tisalnum(*pszT) || ((*pszT) == uT('_')) || ((*pszT) == uT('.')) || ((*pszT) == uT(':'))) {
                    pszT += 1;
                }
                if (pszT && *pszT && (pszT < (pszEnd - 10))) {
                    while (CodeAnalyzer::isSeparator(*pszT)) {
                        pszT += 1;
                    }
                    if ((*pszT) == uT('=')) {
                        pszT += 1;
                        while (CodeAnalyzer::isSeparator(*pszT)) {
                            pszT += 1;
                        }
                        if (pszT == pszStart) {
                            cT = *pszEnd;
                            *pszEnd = uT('\0');
                            Tstrncpy(funcName, static_cast<const char_t *>(pszLine), ANALYZER_NAMESIZE - 1);
                            *pszEnd = cT;
                        }
                    }
                }
                // :END: name = function(arg)
            }
            else if (pszT && *pszT && (Tisalpha(*pszT) || ((*pszT) == uT('_')))) {
                pszF = pszT;
                while (Tisalnum(*pszT) || ((*pszT) == uT('_')) || ((*pszT) == uT('.')) || ((*pszT) == uT(':'))) {
                    pszT += 1;
                }
                if (pszT && *pszT) {
                    if (*pszT == uT('(')) {
                        while (*pszT != uT(')')) {
                            pszT += 1;
                        }
                        if (*pszT == uT(')')) {
                            pszT += 1;
                        }
                        cT = *pszT;
                        *pszT = uT('\0');
                        Tstrncpy(funcName, static_cast<const char_t *>(pszF), ANALYZER_NAMESIZE - 1);
                        *pszT = cT;
                    }
                    else {
                        if (CodeAnalyzer::isSeparator(*pszT) == false) {
                            *piLine += 1;
                            return true;
                        }
                        while (CodeAnalyzer::isSeparator(*pszT)) {
                            pszT += 1;
                        }
                        if (*pszT == uT('(')) {
                            while (*pszT != uT(')')) {
                                pszT += 1;
                            }
                            if (*pszT == uT(')')) {
                                pszT += 1;
                            }
                            cT = *pszT;
                            *pszT = uT('\0');
                            Tstrncpy(funcName, static_cast<const char_t *>(pszF), ANALYZER_NAMESIZE - 1);
                            *pszT = cT;
                        }
                    }
                }
            }
        }
    }

    if (funcName[0] == uT('\0')) {
        *piLine += 1;
        return true;
    }

    *piId += 1;
    funcLine = *piLine;

    AnalyzerElement *pFunc = new (std::nothrow) AnalyzerElement(*piId, funcScope, AnalyzerElement::ETYPE_FUNC, funcLine, static_cast<const char_t *>(funcName));
    if (pFunc == NULL) {
        *piLine += 1;
        return false;
    }

    m_pData->push_back(pFunc);

    *piBalanceFunc += 1;

    pszT = Tstrstr(pszLine, uT("end"));
    if (pszT) {
        if ((pszT == (pszLine + iLen - 3)) && CodeAnalyzer::isSeparator(*(pszT - 1))) {
            *piBalanceFunc -= 1;
        }
    }

    *piLine += 1;
    return true;
}

bool CodeAnalyzer::analyze(void)
{
    if (::wxFileExists(m_strFilename) == false) {
        return false;
    }

    reset();

    m_pData = new (std::nothrow) std::vector<AnalyzerElement *>;
    if (m_pData == NULL) {
        return false;
    }

    FILE *fp = Tfopen(LM_CSTR(m_strFilename), uT("rt"));
    if (fp == NULL) {
        return false;
    }

    char szLineA[LM_STRSIZEL];
    memset(szLineA, 0, LM_STRSIZEL * sizeof(char));

    char_t szLine[LM_STRSIZEL];
    Tmemset(szLine, 0, LM_STRSIZEL);

    wxString strLine;

    int iLine = 0, iLen, iId = 0, iBalanceFunc = 0, iBalanceLoop = 0, iBalanceComment = 0;

    while (fgets(szLineA, LM_STRSIZEL - 1, fp) != NULL) {

        // Convert from UTF-8
        strLine = wxString::FromUTF8((const char *)(szLineA));
        Tstrcpy(szLine, LM_CSTR(strLine));

        iLen = lm_trim(szLine);

        if (m_iLang == CodeAnalyzer::LANGUAGE_LUA) {
            if (iLen >= 4) {
                // Comment block?
                if ((szLine[0] == uT('-')) && (szLine[1] == uT('-')) && (szLine[2] == uT('[')) && (szLine[3] == uT('['))) {
                    iBalanceComment += 1;
                    iLine += 1;
                    continue;
                }
                else if ((szLine[0] == uT('-')) && (szLine[1] == uT('-')) && (szLine[2] == uT(']')) && (szLine[3] == uT(']'))) {
                    iBalanceComment -= 1;
                    iLine += 1;
                    continue;
                }
                else if ((szLine[iLen - 2] == uT(']')) && (szLine[iLen - 1] == uT(']'))) {
                    iBalanceComment -= 1;
                    iLine += 1;
                    continue;
                }
            }
            else if (iLen >= 2) {
                // Comment block?
                if ((szLine[iLen - 2] == uT(']')) && (szLine[iLen - 1] == uT(']'))) {
                    iBalanceComment -= 1;
                    iLine += 1;
                    continue;
                }
            }

            if (iBalanceComment > 0) {
                iLine += 1;
                continue;
            }
        }

        if (analyzeLine(szLine, iLen, &iId, &iLine, &iBalanceFunc, &iBalanceLoop) == false) {
            fclose(fp);
            fp = NULL;
            return false;
        }

        if (getCount() >= CodeAnalyzer::MAXCOUNT) {
            fclose(fp);
            fp = NULL;
            return true;
        }
    }

    fclose(fp);
    fp = NULL;

    return true;
}
