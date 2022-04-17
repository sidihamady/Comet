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

#include "CometConfig.h"
#include "CometApp.h"
#include "CometFrame.h"

#include <wx/settings.h>

#define CONFIG_MAXLINES      2048
#define CONFIG_MAXSCREENSIZE 12288

int CometConfig::SECTIONCOUNT = 5;
int CometConfig::SECTIONSIZE[] = {
    2,
    26,
    20,
    23,
#if defined(__WXGTK__)
    33,
#else
    30,
#endif
    0
};
char_t const *const CometConfig::SECTION[] = {

    uT("Common"),
    uT("Find"),
    uT("Recent"),
    uT("GUI"),
    uT("Tools"),
    NULL
};

// Common section (MAX: 34 keys)
char_t const *const CometConfig::KEYCOMMON[] = {

    uT("LastDir"),
    uT("DetectChangeOutside"),
    NULL
};

// Common section (MAX: 34 keys)
char_t const *const CometConfig::KEYFIND[] = {

    uT("FindMarker"),
    uT("FindCyclic"),

    uT("Find01"), uT("Find02"),
    uT("Find03"), uT("Find04"),
    uT("Find05"), uT("Find06"),
    uT("Find07"), uT("FindF"),

    uT("Replace01"), uT("Replace02"),
    uT("Replace03"), uT("Replace04"),
    uT("Replace05"), uT("Replace06"),
    uT("Replace07"), uT("ReplaceF"),

    uT("Dir01"), uT("Dir02"),
    uT("Dir03"), uT("Dir04"),
    uT("Dir05"), uT("Dir06"),
    uT("Dir07"), uT("DirF"),
    NULL
};

// Recent files and dirs section (MAX: 34 keys)
char_t const *const CometConfig::KEYRECENT[] = {

    uT("FileRecent01"), uT("FileRecent02"),
    uT("FileRecent03"), uT("FileRecent04"),
    uT("FileRecent05"), uT("FileRecent06"),
    uT("FileRecent07"), uT("FileRecent08"),
    uT("FileRecent09"), uT("FileRecent10"),
    uT("FileRecent11"), uT("FileRecent12"),
    uT("FileRecentSel"), uT("FileOpenStatus"),
    uT("FileRecentIndex"),

    uT("DirRecent01"), uT("DirRecent02"),
    uT("DirRecent03"), uT("DirRecent04"),
    uT("DirRecent05"),
    NULL
};

// GUI section (MAX: 34 keys)
char_t const *const CometConfig::KEYGUI[] = {

    uT("FrameMaximized"),
    uT("FrameHeight"), uT("FrameWidth"),
    uT("ViewToolbar"), uT("ViewStatusbar"), uT("ViewSearchbar"),
    uT("ViewExplorer"),
    uT("ViewConsole"), uT("ViewOutput"),
    uT("ViewBookmark"),
    uT("ViewStackWinow"),
    uT("FindFilePosX"), uT("FindFilePosY"),
    uT("FindDirPosX"), uT("FindDirPosY"),
    uT("FindDirWidth"), uT("FindDirHeight"),
    uT("BookmarkPosX"), uT("BookmarkPosY"),
    uT("BookmarkWidth"), uT("BookmarkHeight"),
    uT("ReopenRecent"),
    uT("SaveNavigation"),
    uT("ViewInfobar"),
    NULL
};

// Tools section (MAX: 34 keys)
char_t const *const CometConfig::KEYTOOL[] = {

#if defined(__WXGTK__)
    uT("ToolTerm"),
    uT("ToolOpt"),
    uT("ToolShell"),
#endif
    uT("ToolLog"),
    uT("Tool01lex"),
    uT("Tool01cmd"),
    uT("Tool01out"),
    uT("Tool01cls"),
    uT("Tool02lex"),
    uT("Tool02cmd"),
    uT("Tool02out"),
    uT("Tool02cls"),
    uT("Tool03lex"),
    uT("Tool03cmd"),
    uT("Tool03out"),
    uT("Tool03cls"),
    uT("Tool04lex"),
    uT("Tool04cmd"),
    uT("Tool04out"),
    uT("Tool04cls"),
    uT("Tool05lex"),
    uT("Tool05cmd"),
    uT("Tool05out"),
    uT("Tool05cls"),
    uT("Tool06lex"),
    uT("Tool06cmd"),
    uT("Tool06out"),
    uT("Tool06cls"),
    uT("Tool07lex"),
    uT("Tool07cmd"),
    uT("Tool07out"),
    uT("Tool07cls"),
    uT("ToolSilent"),
    NULL
};

void SigmaCommonPrefs::loadDefault(SigmaCommonPrefs &tSigmaCommonPrefs)
{
    Tmemset(tSigmaCommonPrefs.szLastDir, 0, LM_STRSIZE);

    for (int ii = 0; ii < RECENTFILES_MAXCOUNT; ii++) {
        Tmemset(tSigmaCommonPrefs.szFileRecent[ii], 0, LM_STRSIZE);
    }

    for (int ii = 0; ii < RECENTDIRS_MAXCOUNT; ii++) {
        Tmemset(tSigmaCommonPrefs.szDirRecent[ii], 0, LM_STRSIZE);
    }

    for (int ii = 0; ii < FIND_MAXCOUNT; ii++) {
        Tmemset(tSigmaCommonPrefs.szFind[ii], 0, LM_STRSIZE);
        Tmemset(tSigmaCommonPrefs.szReplace[ii], 0, LM_STRSIZE);
    }

    Tmemset(tSigmaCommonPrefs.szFindF, 0, LM_STRSIZE);
    Tmemset(tSigmaCommonPrefs.szReplaceF, 0, LM_STRSIZE);
    Tmemset(tSigmaCommonPrefs.szDirF, 0, LM_STRSIZE);

    tSigmaCommonPrefs.bFindMarker = false;
    tSigmaCommonPrefs.bFindCyclic = true;
    tSigmaCommonPrefs.iFindCount = 0;
    tSigmaCommonPrefs.iReplaceCount = 0;
    tSigmaCommonPrefs.iDirCount = 0;
    tSigmaCommonPrefs.iDirLastIndex = 0;

    tSigmaCommonPrefs.bDetectChangeOutside = true;
    tSigmaCommonPrefs.iFileRecentCount = 0;
    tSigmaCommonPrefs.iDirRecentCount = 0;
    tSigmaCommonPrefs.iFileRecentLastIndex = 0;
    tSigmaCommonPrefs.iFileOpenStatus = 0L;
    tSigmaCommonPrefs.iFileRecentSel = 0;
}

void SigmaGUIPrefs::loadDefault(SigmaGUIPrefs &tSigmaGUIPrefs)
{
    tSigmaGUIPrefs.bFrameMaximized = false;
    tSigmaGUIPrefs.iFrameWidth = MAINFRAME_SIZEW;
    tSigmaGUIPrefs.iFrameHeight = MAINFRAME_SIZEH;
    tSigmaGUIPrefs.bViewExplorer = false;
    tSigmaGUIPrefs.bViewBookmark = false;
    tSigmaGUIPrefs.bViewConsole = true;
    tSigmaGUIPrefs.bViewOutput = true;
    tSigmaGUIPrefs.bViewToolbar = true;
    tSigmaGUIPrefs.bViewStatusbar = true;
    tSigmaGUIPrefs.bViewSearchbar = false;

    tSigmaGUIPrefs.bReopenRecent = false;

    tSigmaGUIPrefs.bSaveNavigation = false;
    tSigmaGUIPrefs.bViewInfobar = true;

    tSigmaGUIPrefs.iFindFilePosX = 0;
    tSigmaGUIPrefs.iFindFilePosY = 0;

    tSigmaGUIPrefs.iFindDirPosX = 0;
    tSigmaGUIPrefs.iFindDirPosY = 0;
    tSigmaGUIPrefs.iFindDirWidth = 0;
    tSigmaGUIPrefs.iFindDirHeight = 0;
}

#if defined(__WXGTK__)
void SigmaToolPrefs::loadDefaultTerm(SigmaToolPrefs &tSigmaToolPrefs)
{
    Tmemset(tSigmaToolPrefs.szTerm, 0, LM_STRSIZE);
    Tmemset(tSigmaToolPrefs.szOpt, 0, LM_STRSIZEM);
    Tmemset(tSigmaToolPrefs.szShell, 0, LM_STRSIZES);
    if (::wxFileExists(uT("/usr/bin/xfce4-terminal"))) {
        Tstrcpy(tSigmaToolPrefs.szTerm, uT("/usr/bin/xfce4-terminal"));
        Tstrcpy(tSigmaToolPrefs.szOpt, uT("-T;--working-directory=;-x;-c"));
    }
    else if (::wxFileExists(uT("/usr/bin/gnome-terminal"))) {
        Tstrcpy(tSigmaToolPrefs.szTerm, uT("/usr/bin/gnome-terminal"));
        Tstrcpy(tSigmaToolPrefs.szOpt, uT("-t;--working-directory=;-x;-c"));
    }
    else if (::wxFileExists(uT("/usr/bin/konsole"))) {
        Tstrcpy(tSigmaToolPrefs.szTerm, uT("/usr/bin/konsole"));
        Tstrcpy(tSigmaToolPrefs.szOpt, uT(" ;--working-directory=;-e;-c"));
    }
    else if (::wxFileExists(uT("/usr/bin/uxterm"))) {
        Tstrcpy(tSigmaToolPrefs.szTerm, uT("/usr/bin/uxterm"));
        Tstrcpy(tSigmaToolPrefs.szOpt, uT("-T;--working-directory=;-e;-c"));
    }
    else if (::wxFileExists(uT("/usr/bin/xterm"))) {
        Tstrcpy(tSigmaToolPrefs.szTerm, uT("/usr/bin/xterm"));
        Tstrcpy(tSigmaToolPrefs.szOpt, uT("-T;--working-directory=;-e;-c"));
    }
    else {
        const char_t *arTerm[] = {
            uT("/usr/bin/x-terminal-emulator"),
            uT("/usr/bin/urxvt"),
            uT("/usr/bin/rxvt"),
            uT("/usr/bin/termit"),
            uT("/usr/bin/terminator"),
            uT("/usr/bin/eterm"),
            uT("/usr/bin/uxterm"),
            uT("/usr/bin/xterm"),
            uT("/usr/bin/gnome-terminal"),
            uT("/usr/bin/roxterm"),
            uT("/usr/bin/xfce4-terminal"),
            uT("/usr/bin/termite"),
            uT("/usr/bin/lxterminal"),
            uT("/usr/bin/mate-terminal"),
            uT("/usr/bin/terminology"),
            uT("/usr/bin/st"),
            uT("/usr/bin/qterminal"),
            uT("/usr/bin/lilyterm"),
            uT("/usr/bin/tilix"),
            uT("/usr/bin/terminix"),
            uT("/usr/bin/konsole"),
            uT("/usr/bin/kitty"),
            uT("/usr/bin/guake"),
            uT("/usr/bin/tilda"),
            uT("/usr/bin/alacritty"),
            uT("/usr/bin/hyper"),
            NULL
        };

        for (int ii = 0; arTerm[ii] != NULL; ii++) {
            if (::wxFileExists(arTerm[ii])) {
                Tstrcpy(tSigmaToolPrefs.szTerm, arTerm[ii]);
                Tstrcpy(tSigmaToolPrefs.szOpt, uT("-T;--working-directory=;-e;-c"));
                break;
            }
        }
    }
    Tstrcpy(tSigmaToolPrefs.szShell, uT("bash"));
}

#endif

void SigmaToolPrefs::loadDefault(SigmaToolPrefs &tSigmaToolPrefs)
{

#if defined(__WXGTK__)
    SigmaToolPrefs::loadDefaultTerm(tSigmaToolPrefs);
#endif

    tSigmaToolPrefs.bLog = false;

    for (int ii = 0; ii < TOOLS_MAXCOUNT; ii++) {
        tSigmaToolPrefs.iLex[ii] = 0;
        Tmemset(tSigmaToolPrefs.szCmd[ii], 0, LM_STRSIZE);
        tSigmaToolPrefs.bOut[ii] = false;
        tSigmaToolPrefs.bCls[ii] = false;
    }
    tSigmaToolPrefs.bSilent = false;
#if defined(__WXMSW__)
    Tstrcpy(tSigmaToolPrefs.szCmd[0], uT("scc.exe %s.c -o %s.exe && %s.exe"));
    Tstrcpy(tSigmaToolPrefs.szCmd[1], uT("C:\\Python\\python.exe -u %s.py"));
    Tstrcpy(tSigmaToolPrefs.szCmd[2], uT("call build.bat"));
    Tstrcpy(tSigmaToolPrefs.szCmd[3], uT("call build.bat"));
    Tstrcpy(tSigmaToolPrefs.szCmd[5], uT("call build.bat"));
    Tstrcpy(tSigmaToolPrefs.szCmd[6], uT("call build.bat"));
#else
    Tstrcpy(tSigmaToolPrefs.szCmd[0], uT("./build.sh"));
    Tstrcpy(tSigmaToolPrefs.szCmd[1], uT("python -u %s.py"));
    Tstrcpy(tSigmaToolPrefs.szCmd[2], uT("./build.sh"));
    Tstrcpy(tSigmaToolPrefs.szCmd[3], uT("./build.sh"));
    Tstrcpy(tSigmaToolPrefs.szCmd[5], uT("./build.sh"));
    Tstrcpy(tSigmaToolPrefs.szCmd[6], uT("./build.sh"));
#endif
    Tstrcpy(tSigmaToolPrefs.szCmd[4], uT("make"));
    tSigmaToolPrefs.iLex[0] = wxSTC_LEX_CPP;
    tSigmaToolPrefs.iLex[1] = wxSTC_LEX_PYTHON;
    tSigmaToolPrefs.iLex[2] = wxSTC_LEX_FORTRAN;
    tSigmaToolPrefs.iLex[3] = wxSTC_LEX_LATEX;
    tSigmaToolPrefs.iLex[4] = wxSTC_LEX_MAKEFILE;
    tSigmaToolPrefs.iLex[5] = wxSTC_LEX_SOLIS;
    tSigmaToolPrefs.iLex[6] = wxSTC_LEX_COMET;
}

bool CometConfig::load(const char_t *pszFilename, SigmaCommonPrefs &tSigmaCommonPrefs, SigmaGUIPrefs &tSigmaGUIPrefs, SigmaToolPrefs &tSigmaToolPrefs)
{
    if (pszFilename == NULL) {
        SigmaCommonPrefs::loadDefault(tSigmaCommonPrefs);
        SigmaGUIPrefs::loadDefault(tSigmaGUIPrefs);
        SigmaToolPrefs::loadDefault(tSigmaToolPrefs);
        return false;
    }

    SigmaCommonPrefs::loadDefault(tSigmaCommonPrefs);
    SigmaGUIPrefs::loadDefault(tSigmaGUIPrefs);
    SigmaToolPrefs::loadDefault(tSigmaToolPrefs);

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

    wxString strT;

    if (getValue(uT("Common"), uT("LastDir"), szTmp)) {
        strT = (const char_t *)szTmp;
        // Add dir, even if does not exist on the current machine
        wxStrncpy(tSigmaCommonPrefs.szLastDir, (const char_t *)szTmp, LM_STRSIZE - 1);
    }

    if (getValue(uT("Common"), uT("DetectChangeOutside"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaCommonPrefs.bDetectChangeOutside = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaCommonPrefs.bDetectChangeOutside = true;
        }
    }

    char_t szTT[LM_STRSIZES];
    memset(szTT, 0, LM_STRSIZES * sizeof(char_t));
    int iT = 0;

    wxString strTA;
    bool bFlag = false;

    tSigmaCommonPrefs.iFileRecentCount = 0;
    for (int ii = 1; ii <= RECENTFILES_MAXCOUNT; ii++) {
        Tsnprintf(szTT, LM_STRSIZES - 1, uT("FileRecent%02d"), ii);
        if (getValue(uT("Recent"), static_cast<const char_t *>(szTT), szTmp)) {
            strT = (const char_t *)szTmp;
            bFlag = false;
            // Add file, even if does not exist on the current machine
            if (tSigmaCommonPrefs.iFileRecentCount > 0) {
                for (int jj = 0; jj < tSigmaCommonPrefs.iFileRecentCount; jj++) {
                    strTA = wxString(static_cast<const char_t *>(tSigmaCommonPrefs.szFileRecent[jj]));
                    if (strTA.IsSameAs(strT)) {
                        bFlag = true;
                        break;
                    }
                }
            }
            if (bFlag == false) {
                wxStrncpy(tSigmaCommonPrefs.szFileRecent[tSigmaCommonPrefs.iFileRecentCount], (const char_t *)szTmp, LM_STRSIZE - 1);
                tSigmaCommonPrefs.iFileRecentCount += 1;
            }
        }
    }

    if (tSigmaCommonPrefs.iFileRecentCount > 0) {
        if (tSigmaCommonPrefs.iFileRecentCount < RECENTFILES_MAXCOUNT) {
            tSigmaCommonPrefs.iFileRecentLastIndex = tSigmaCommonPrefs.iFileRecentCount;
        }
        else {
            if (getValue(uT("Recent"), uT("FileRecentIndex"), szTmp)) {
                iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
                if ((iT >= 0) && (iT <= tSigmaCommonPrefs.iFileRecentCount)) {
                    tSigmaCommonPrefs.iFileRecentLastIndex = iT;
                }
            }
            if (tSigmaCommonPrefs.iFileRecentLastIndex >= RECENTFILES_MAXCOUNT) {
                tSigmaCommonPrefs.iFileRecentLastIndex = 0;
            }
        }
        if (getValue(uT("Recent"), uT("FileOpenStatus"), szTmp)) {
            tSigmaCommonPrefs.iFileOpenStatus = (unsigned long)(wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10));
        }
        if (getValue(uT("Recent"), uT("FileRecentSel"), szTmp)) {
            iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
            if ((iT >= 0) && (iT < tSigmaCommonPrefs.iFileRecentCount)) {
                tSigmaCommonPrefs.iFileRecentSel = iT;
            }
        }
    }

    const wxString strSep = wxFILE_SEP_PATH;

    tSigmaCommonPrefs.iDirRecentCount = 0;
    for (int ii = 1; ii <= RECENTDIRS_MAXCOUNT; ii++) {
        Tsnprintf(szTT, LM_STRSIZES - 1, uT("DirRecent%02d"), ii);
        if (getValue(uT("Recent"), static_cast<const char_t *>(szTT), szTmp)) {
            strT = (const char_t *)szTmp;
            if (strT.EndsWith(strSep) == false) {
                strT += strSep;
            }

            bFlag = false;
            // Add dir, even if does not exist on the current machine
            if (tSigmaCommonPrefs.iDirRecentCount > 0) {
                for (int jj = 0; jj < tSigmaCommonPrefs.iDirRecentCount; jj++) {
                    strTA = wxString(static_cast<const char_t *>(tSigmaCommonPrefs.szDirRecent[jj]));
                    if (strTA.EndsWith(strSep) == false) {
                        strTA += strSep;
                    }
                    if (strTA.IsSameAs(strT)) {
                        bFlag = true;
                        break;
                    }
                }
            }
            if (bFlag == false) {
                wxStrncpy(tSigmaCommonPrefs.szDirRecent[tSigmaCommonPrefs.iDirRecentCount], (const char_t *)szTmp, LM_STRSIZE - 1);
                tSigmaCommonPrefs.iDirRecentCount += 1;
            }
        }
    }

    if (getValue(uT("Find"), uT("FindMarker"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaCommonPrefs.bFindMarker = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaCommonPrefs.bFindMarker = true;
        }
    }

    if (getValue(uT("Find"), uT("FindCyclic"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaCommonPrefs.bFindCyclic = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaCommonPrefs.bFindCyclic = true;
        }
    }

    tSigmaCommonPrefs.iFindCount = 0;
    tSigmaCommonPrefs.iReplaceCount = 0;
    tSigmaCommonPrefs.iDirCount = 0;
    for (int ii = 1; ii <= FIND_MAXCOUNT; ii++) {
        Tsnprintf(szTT, LM_STRSIZES - 1, uT("Find%02d"), ii);
        if (getValue(uT("Find"), static_cast<const char_t *>(szTT), szTmp)) {
            wxStrncpy(tSigmaCommonPrefs.szFind[tSigmaCommonPrefs.iFindCount], (const char_t *)szTmp, LM_STRSIZE - 1);
            tSigmaCommonPrefs.iFindCount += 1;
        }

        Tsnprintf(szTT, LM_STRSIZES - 1, uT("Replace%02d"), ii);
        if (getValue(uT("Find"), static_cast<const char_t *>(szTT), szTmp)) {
            wxStrncpy(tSigmaCommonPrefs.szReplace[tSigmaCommonPrefs.iReplaceCount], (const char_t *)szTmp, LM_STRSIZE - 1);
            tSigmaCommonPrefs.iReplaceCount += 1;
        }

        Tsnprintf(szTT, LM_STRSIZES - 1, uT("Dir%02d"), ii);
        if (getValue(uT("Find"), static_cast<const char_t *>(szTT), szTmp)) {
            if (::wxDirExists((const char_t *)szTmp)) {
                wxStrncpy(tSigmaCommonPrefs.szDir[tSigmaCommonPrefs.iDirCount], (const char_t *)szTmp, LM_STRSIZE - 1);
                tSigmaCommonPrefs.iDirCount += 1;
            }
        }
    }
    if (tSigmaCommonPrefs.iFindCount > 0) {
        if (getValue(uT("Find"), uT("FindF"), szTmp)) {
            wxStrncpy(tSigmaCommonPrefs.szFindF, (const char_t *)szTmp, LM_STRSIZE - 1);
        }
    }

    if (tSigmaCommonPrefs.iReplaceCount > 0) {
        if (getValue(uT("Find"), uT("ReplaceF"), szTmp)) {
            wxStrncpy(tSigmaCommonPrefs.szReplaceF, (const char_t *)szTmp, LM_STRSIZE - 1);
        }
    }

    if (tSigmaCommonPrefs.iDirCount > 0) {
        tSigmaCommonPrefs.iDirLastIndex = tSigmaCommonPrefs.iDirCount - 1;
        if (getValue(uT("Find"), uT("DirF"), szTmp)) {
            if (::wxDirExists((const char_t *)szTmp)) {
                wxStrncpy(tSigmaCommonPrefs.szDirF, (const char_t *)szTmp, LM_STRSIZE - 1);
            }
        }
    }

    int iScreenWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    if ((iScreenWidth < 400) || (iScreenWidth > CONFIG_MAXSCREENSIZE)) {
        iScreenWidth = CONFIG_MAXSCREENSIZE;
    }
    int iScreenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
    if ((iScreenHeight < 400) || (iScreenHeight > CONFIG_MAXSCREENSIZE)) {
        iScreenHeight = CONFIG_MAXSCREENSIZE;
    }

    if (getValue(uT("GUI"), uT("FrameMaximized"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bFrameMaximized = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bFrameMaximized = true;
        }
    }

    iT = 0;
    if (getValue(uT("GUI"), uT("FrameWidth"), szTmp)) {
        iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
        if ((iT >= 640) && (iT <= iScreenWidth)) {
            tSigmaGUIPrefs.iFrameWidth = iT;
        }
    }
    if (getValue(uT("GUI"), uT("FrameHeight"), szTmp)) {
        iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
        if ((iT >= 480) && (iT <= iScreenHeight)) {
            tSigmaGUIPrefs.iFrameHeight = iT;
        }
    }

    if (getValue(uT("GUI"), uT("FindFilePosX"), szTmp)) {
        iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
        if ((iT >= 20) && (iT <= (iScreenWidth - 200))) {
            tSigmaGUIPrefs.iFindFilePosX = iT;
        }
    }

    if (getValue(uT("GUI"), uT("FindFilePosY"), szTmp)) {
        iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
        if ((iT >= 20) && (iT <= (iScreenHeight - 400))) {
            tSigmaGUIPrefs.iFindFilePosY = iT;
        }
    }

    if (getValue(uT("GUI"), uT("FindDirPosX"), szTmp)) {
        iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
        if ((iT >= 20) && (iT <= (iScreenWidth - 200))) {
            tSigmaGUIPrefs.iFindDirPosX = iT;
        }
    }
    if (getValue(uT("GUI"), uT("FindDirPosY"), szTmp)) {
        iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
        if ((iT >= 20) && (iT <= (iScreenHeight - 400))) {
            tSigmaGUIPrefs.iFindDirPosY = iT;
        }
    }
    if (getValue(uT("GUI"), uT("FindDirWidth"), szTmp)) {
        iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
        if ((iT >= 200) && (iT <= iScreenWidth)) {
            tSigmaGUIPrefs.iFindDirWidth = iT;
        }
    }
    if (getValue(uT("GUI"), uT("FindDirHeight"), szTmp)) {
        iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
        if ((iT >= 400) && (iT <= iScreenHeight)) {
            tSigmaGUIPrefs.iFindDirHeight = iT;
        }
    }

    if (getValue(uT("GUI"), uT("ViewExplorer"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewExplorer = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewExplorer = true;
        }
    }

    if (getValue(uT("GUI"), uT("ViewConsole"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewConsole = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewConsole = true;
        }
    }

    if (getValue(uT("GUI"), uT("ViewOutput"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewOutput = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewOutput = true;
        }
    }

    if (getValue(uT("GUI"), uT("ViewBookmark"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewBookmark = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewBookmark = true;
        }
    }

    if (getValue(uT("GUI"), uT("ReopenRecent"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bReopenRecent = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bReopenRecent = true;
        }
    }

    if (getValue(uT("GUI"), uT("SaveNavigation"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bSaveNavigation = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bSaveNavigation = true;
        }
    }

    if (getValue(uT("GUI"), uT("ViewInfobar"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewInfobar = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewInfobar = true;
        }
    }

    if (getValue(uT("GUI"), uT("ViewToolbar"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewToolbar = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewToolbar = true;
        }
    }

    if (getValue(uT("GUI"), uT("ViewStatusbar"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewStatusbar = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewStatusbar = true;
        }
    }

    if (getValue(uT("GUI"), uT("ViewSearchbar"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewSearchbar = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaGUIPrefs.bViewSearchbar = true;
        }
    }

#if defined(__WXGTK__)
    if (getValue(uT("Tools"), uT("ToolTerm"), szTmp)) {
        wxStrncpy(tSigmaToolPrefs.szTerm, (const char_t *)szTmp, LM_STRSIZE - 1);
    }
    if (getValue(uT("Tools"), uT("ToolOpt"), szTmp)) {
        wxStrncpy(tSigmaToolPrefs.szOpt, (const char_t *)szTmp, LM_STRSIZEM - 1);
    }
    if (getValue(uT("Tools"), uT("ToolShell"), szTmp)) {
        wxStrncpy(tSigmaToolPrefs.szShell, (const char_t *)szTmp, LM_STRSIZES - 1);
    }
    if ((::wxFileExists((const char_t *)(tSigmaToolPrefs.szTerm)) == false) || (Tstrlen((const char_t *)(tSigmaToolPrefs.szOpt)) < 2) || (Tstrlen((const char_t *)(tSigmaToolPrefs.szShell)) < 2)) {
        SigmaToolPrefs::loadDefaultTerm(tSigmaToolPrefs);
    }
#endif

    if (getValue(uT("Tools"), uT("ToolLog"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaToolPrefs.bLog = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaToolPrefs.bLog = true;
        }
    }

    for (int ii = 1; ii <= TOOLS_MAXCOUNT; ii++) {
        Tsnprintf(szTT, LM_STRSIZES - 1, uT("Tool%02dlex"), ii);
        if (getValue(uT("Tools"), static_cast<const char_t *>(szTT), szTmp)) {
            iT = (int)wxStrtol((const char_t *)szTmp, (char_t **)NULL, 10);
            if (CometFrame::isToolSet(iT)) {
                Tsnprintf(szTT, LM_STRSIZES - 1, uT("Tool%02dcmd"), ii);
                if (getValue(uT("Tools"), static_cast<const char_t *>(szTT), szTmp)) {
                    wxStrncpy(tSigmaToolPrefs.szCmd[ii - 1], (const char_t *)szTmp, LM_STRSIZE - 1);
                }
                Tsnprintf(szTT, LM_STRSIZES - 1, uT("Tool%02dout"), ii);
                if (getValue(uT("Tools"), static_cast<const char_t *>(szTT), szTmp)) {
                    if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
                        tSigmaToolPrefs.bOut[ii - 1] = false;
                    }
                    else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
                        tSigmaToolPrefs.bOut[ii - 1] = true;
                    }
                }
                Tsnprintf(szTT, LM_STRSIZES - 1, uT("Tool%02dcls"), ii);
                if (getValue(uT("Tools"), static_cast<const char_t *>(szTT), szTmp)) {
                    if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
                        tSigmaToolPrefs.bCls[ii - 1] = false;
                    }
                    else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
                        tSigmaToolPrefs.bCls[ii - 1] = true;
                    }
                }
            }
        }
    }

    if (getValue(uT("Tools"), uT("ToolSilent"), szTmp)) {
        if (wxStrcmp(uT("0"), (const char_t *)szTmp) == 0) {
            tSigmaToolPrefs.bSilent = false;
        }
        else if (wxStrcmp(uT("1"), (const char_t *)szTmp) == 0) {
            tSigmaToolPrefs.bSilent = true;
        }
    }

    return true;
}

bool CometConfig::save(const char_t *pszFilename, const SigmaCommonPrefs &tSigmaCommonPrefs, const SigmaGUIPrefs &tSigmaGUIPrefs, const SigmaToolPrefs &tSigmaToolPrefs)
{
    removeAll();

    int ii, jj;
    lfSection *pSection = NULL;

    for (ii = 0; CometConfig::SECTION[ii] != NULL; ii++) {
        pSection = addSection(static_cast<const char_t *>(CometConfig::SECTION[ii]), CometConfig::SECTIONSIZE[ii]);
        if (pSection == NULL) {
            return false;
        }
    }

    setValue(uT("Common"), uT("LastDir"), static_cast<const char_t *>(tSigmaCommonPrefs.szLastDir));
    setValue(uT("Common"), uT("DetectChangeOutside"), tSigmaCommonPrefs.bDetectChangeOutside ? uT("1") : uT("0"));

    char_t szTT[LM_STRSIZES];
    memset(szTT, 0, LM_STRSIZES * sizeof(char_t));
    char_t szValue[LM_STRSIZEN];
    Tmemset(szValue, 0, LM_STRSIZEN);

    if (tSigmaCommonPrefs.iFileRecentCount > 0) {

        for (ii = 1; ii <= wxMin(tSigmaCommonPrefs.iFileRecentCount, RECENTFILES_MAXCOUNT); ii++) {
            Tsnprintf(szTT, LM_STRSIZES - 1, uT("FileRecent%02d"), ii);
            setValue(uT("Recent"), static_cast<const char_t *>(szTT), static_cast<const char_t *>(tSigmaCommonPrefs.szFileRecent[ii - 1]));
        }
        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tSigmaCommonPrefs.iFileRecentLastIndex);
        setValue(uT("Recent"), uT("FileRecentIndex"), (const char_t *)szValue);
        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%lu"), tSigmaCommonPrefs.iFileOpenStatus);
        setValue(uT("Recent"), uT("FileOpenStatus"), (const char_t *)szValue);
        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tSigmaCommonPrefs.iFileRecentSel);
        setValue(uT("Recent"), uT("FileRecentSel"), (const char_t *)szValue);
    }

    if (tSigmaCommonPrefs.iDirRecentCount > 0) {

        for (ii = 1; ii <= wxMin(tSigmaCommonPrefs.iDirRecentCount, RECENTDIRS_MAXCOUNT); ii++) {
            Tsnprintf(szTT, LM_STRSIZES - 1, uT("DirRecent%02d"), ii);
            setValue(uT("Recent"), static_cast<const char_t *>(szTT), static_cast<const char_t *>(tSigmaCommonPrefs.szDirRecent[ii - 1]));
        }
    }

    setValue(uT("Find"), uT("FindMarker"), tSigmaCommonPrefs.bFindMarker ? uT("1") : uT("0"));

    setValue(uT("Find"), uT("FindCyclic"), tSigmaCommonPrefs.bFindCyclic ? uT("1") : uT("0"));

    if (tSigmaCommonPrefs.iFindCount > 0) {

        for (ii = 1; ii <= wxMin(tSigmaCommonPrefs.iFindCount, FIND_MAXCOUNT); ii++) {
            Tsnprintf(szTT, LM_STRSIZES - 1, uT("Find%02d"), ii);
            setValue(uT("Find"), static_cast<const char_t *>(szTT), static_cast<const char_t *>(tSigmaCommonPrefs.szFind[ii - 1]));
        }
        setValue(uT("Find"), uT("FindF"), static_cast<const char_t *>(tSigmaCommonPrefs.szFindF));
    }

    if (tSigmaCommonPrefs.iReplaceCount > 0) {

        for (ii = 1; ii <= wxMin(tSigmaCommonPrefs.iReplaceCount, FIND_MAXCOUNT); ii++) {
            Tsnprintf(szTT, LM_STRSIZES - 1, uT("Replace%02d"), ii);
            setValue(uT("Find"), static_cast<const char_t *>(szTT), static_cast<const char_t *>(tSigmaCommonPrefs.szReplace[ii - 1]));
        }
        setValue(uT("Find"), uT("ReplaceF"), static_cast<const char_t *>(tSigmaCommonPrefs.szReplaceF));
    }

    if (tSigmaCommonPrefs.iDirCount > 0) {

        for (ii = 1; ii <= wxMin(tSigmaCommonPrefs.iDirCount, FIND_MAXCOUNT); ii++) {
            Tsnprintf(szTT, LM_STRSIZES - 1, uT("Dir%02d"), ii);
            setValue(uT("Find"), static_cast<const char_t *>(szTT), static_cast<const char_t *>(tSigmaCommonPrefs.szDir[ii - 1]));
        }
        setValue(uT("Find"), uT("DirF"), static_cast<const char_t *>(tSigmaCommonPrefs.szDirF));
    }

    setValue(uT("GUI"), uT("FrameMaximized"), tSigmaGUIPrefs.bFrameMaximized ? uT("1") : uT("0"));
    Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tSigmaGUIPrefs.iFrameWidth);
    setValue(uT("GUI"), uT("FrameWidth"), (const char_t *)szValue);
    Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tSigmaGUIPrefs.iFrameHeight);
    setValue(uT("GUI"), uT("FrameHeight"), (const char_t *)szValue);

    if ((tSigmaGUIPrefs.iFindFilePosX > 0) && (tSigmaGUIPrefs.iFindFilePosY > 0)) {
        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tSigmaGUIPrefs.iFindFilePosX);
        setValue(uT("GUI"), uT("FindFilePosX"), (const char_t *)szValue);
        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tSigmaGUIPrefs.iFindFilePosY);
        setValue(uT("GUI"), uT("FindFilePosY"), (const char_t *)szValue);
    }

    if ((tSigmaGUIPrefs.iFindDirPosX > 0) && (tSigmaGUIPrefs.iFindDirPosY > 0) && (tSigmaGUIPrefs.iFindDirWidth > 0) && (tSigmaGUIPrefs.iFindDirHeight > 0)) {
        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tSigmaGUIPrefs.iFindDirPosX);
        setValue(uT("GUI"), uT("FindDirPosX"), (const char_t *)szValue);
        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tSigmaGUIPrefs.iFindDirPosY);
        setValue(uT("GUI"), uT("FindDirPosY"), (const char_t *)szValue);
        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tSigmaGUIPrefs.iFindDirWidth);
        setValue(uT("GUI"), uT("FindDirWidth"), (const char_t *)szValue);
        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tSigmaGUIPrefs.iFindDirHeight);
        setValue(uT("GUI"), uT("FindDirHeight"), (const char_t *)szValue);
    }

    setValue(uT("GUI"), uT("ViewExplorer"), tSigmaGUIPrefs.bViewExplorer ? uT("1") : uT("0"));
    setValue(uT("GUI"), uT("ViewConsole"), tSigmaGUIPrefs.bViewConsole ? uT("1") : uT("0"));
    setValue(uT("GUI"), uT("ViewOutput"), tSigmaGUIPrefs.bViewOutput ? uT("1") : uT("0"));
    setValue(uT("GUI"), uT("ViewBookmark"), tSigmaGUIPrefs.bViewBookmark ? uT("1") : uT("0"));
    setValue(uT("GUI"), uT("ViewToolbar"), tSigmaGUIPrefs.bViewToolbar ? uT("1") : uT("0"));
    setValue(uT("GUI"), uT("ViewStatusbar"), tSigmaGUIPrefs.bViewStatusbar ? uT("1") : uT("0"));
    setValue(uT("GUI"), uT("ViewSearchbar"), tSigmaGUIPrefs.bViewSearchbar ? uT("1") : uT("0"));

    setValue(uT("GUI"), uT("ReopenRecent"), tSigmaGUIPrefs.bReopenRecent ? uT("1") : uT("0"));
    setValue(uT("GUI"), uT("SaveNavigation"), tSigmaGUIPrefs.bSaveNavigation ? uT("1") : uT("0"));
    setValue(uT("GUI"), uT("ViewInfobar"), tSigmaGUIPrefs.bViewInfobar ? uT("1") : uT("0"));

#if defined(__WXGTK__)
    setValue(uT("Tools"), uT("ToolTerm"), static_cast<const char_t *>(tSigmaToolPrefs.szTerm));
    setValue(uT("Tools"), uT("ToolOpt"), static_cast<const char_t *>(tSigmaToolPrefs.szOpt));
    setValue(uT("Tools"), uT("ToolShell"), static_cast<const char_t *>(tSigmaToolPrefs.szShell));
#endif

    setValue(uT("Tools"), uT("ToolLog"), tSigmaToolPrefs.bLog ? uT("1") : uT("0"));

    for (ii = 1; ii <= TOOLS_MAXCOUNT; ii++) {
        Tsnprintf(szTT, LM_STRSIZES - 1, uT("Tool%02dlex"), ii);
        Tsnprintf(szValue, LM_STRSIZEN - 1, uT("%d"), tSigmaToolPrefs.iLex[ii - 1]);
        setValue(uT("Tools"), static_cast<const char_t *>(szTT), static_cast<const char_t *>(szValue));
        Tsnprintf(szTT, LM_STRSIZES - 1, uT("Tool%02dcmd"), ii);
        setValue(uT("Tools"), static_cast<const char_t *>(szTT), static_cast<const char_t *>(tSigmaToolPrefs.szCmd[ii - 1]));
        Tsnprintf(szTT, LM_STRSIZES - 1, uT("Tool%02dout"), ii);
        setValue(uT("Tools"), static_cast<const char_t *>(szTT), tSigmaToolPrefs.bOut[ii - 1] ? uT("1") : uT("0"));
        Tsnprintf(szTT, LM_STRSIZES - 1, uT("Tool%02dcls"), ii);
        setValue(uT("Tools"), static_cast<const char_t *>(szTT), tSigmaToolPrefs.bCls[ii - 1] ? uT("1") : uT("0"));
    }
    setValue(uT("Tools"), uT("ToolSilent"), tSigmaToolPrefs.bSilent ? uT("1") : uT("0"));

    FILE *fp = NULL;

    if ((fp = Tfopen(pszFilename, uT("w"))) == NULL) {
        return false;
    }

    wxString strHeader = CometApp::getConfigHeader();
    Tfputs(LM_CSTR(strHeader), fp);

#ifdef __WXMSW__
    const char_t *pszFormatSection = uT("[%s]\n");
    const char_t *pszFormatItem = uT("   %s=%s\n");
#else
    // For unicode char, I need to specify %ls in the format (for Linux)
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
