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


#ifndef SIGMA_CONFIG_H
#define SIGMA_CONFIG_H

#include "../../LibFile/include/File.h"

#include <wx/filename.h>

#define RECENTFILES_MAXCOUNT 12 // The absolute highest value is 32 (sizeof(unsigned int))
#define RECENTDIRS_MAXCOUNT  5
#define FIND_MAXCOUNT        7

struct SigmaCommonPrefs
{
    SigmaCommonPrefs()
    {
        loadDefault(*this);
    }

    static void loadDefault(SigmaCommonPrefs &tSigmaCommonPrefs);

    char_t szLastDir[LM_STRSIZE];

    bool bDetectChangeOutside;

    int iFileRecentLastIndex;
    int iFileRecentCount;
    char_t szFileRecent[RECENTFILES_MAXCOUNT][LM_STRSIZE];
    unsigned long iFileOpenStatus;
    int iFileRecentSel;

    int iDirRecentCount;
    char_t szDirRecent[RECENTDIRS_MAXCOUNT][LM_STRSIZE];

    bool bFindMarker;
    bool bFindCyclic;

    int iFindCount;
    char_t szFind[FIND_MAXCOUNT][LM_STRSIZE];
    char_t szFindF[LM_STRSIZE];

    int iReplaceCount;
    char_t szReplace[FIND_MAXCOUNT][LM_STRSIZE];
    char_t szReplaceF[LM_STRSIZE];

    int iDirLastIndex;
    int iDirCount;
    char_t szDir[FIND_MAXCOUNT][LM_STRSIZE];
    char_t szDirF[LM_STRSIZE];
};

struct SigmaGUIPrefs
{
    SigmaGUIPrefs()
    {
        loadDefault(*this);
    }

    static void loadDefault(SigmaGUIPrefs &tSigmaGUIPrefs);

    static void copy(SigmaGUIPrefs &tFrom, SigmaGUIPrefs &tTo)
    {
        tTo.iFrameWidth = tFrom.iFrameWidth;
        tTo.iFrameHeight = tFrom.iFrameHeight;
        tTo.bFrameMaximized = tFrom.bFrameMaximized;

        tTo.iFindDirPosX = tFrom.iFindDirPosX;
        tTo.iFindDirPosY = tFrom.iFindDirPosY;
        tTo.iFindDirWidth = tFrom.iFindDirWidth;
        tTo.iFindDirHeight = tFrom.iFindDirHeight;

        tTo.iFindFilePosX = tFrom.iFindFilePosX;
        tTo.iFindFilePosY = tFrom.iFindFilePosY;

        tTo.bViewExplorer = tFrom.bViewExplorer;
        tTo.bViewConsole = tFrom.bViewConsole;
        tTo.bViewOutput = tFrom.bViewOutput;
        tTo.bViewBookmark = tFrom.bViewBookmark;
        tTo.bViewToolbar = tFrom.bViewToolbar;
        tTo.bViewStatusbar = tFrom.bViewStatusbar;
        tTo.bViewSearchbar = tFrom.bViewSearchbar;

        tTo.bReopenRecent = tFrom.bReopenRecent;

        tTo.bSaveNavigation = tFrom.bSaveNavigation;
        tTo.bViewInfobar = tFrom.bViewInfobar;
    }

    int iFrameWidth;
    int iFrameHeight;
    bool bFrameMaximized;

    int iFindDirPosX;
    int iFindDirPosY;
    int iFindDirWidth;
    int iFindDirHeight;

    int iFindFilePosX;
    int iFindFilePosY;

    bool bViewExplorer;
    bool bViewConsole;
    bool bViewOutput;
    bool bViewBookmark;
    bool bViewToolbar;
    bool bViewStatusbar;
    bool bViewSearchbar;

    bool bReopenRecent;

    bool bSaveNavigation;
    bool bViewInfobar;
};

#define TOOLS_MAXCOUNT 7
struct SigmaToolPrefs
{
    SigmaToolPrefs()
    {
        loadDefault(*this);
    }

    static void loadDefault(SigmaToolPrefs &tSigmaToolPrefs);

    bool bLog;
    int iLex[TOOLS_MAXCOUNT];
    char_t szCmd[TOOLS_MAXCOUNT][LM_STRSIZE];
    bool bOut[TOOLS_MAXCOUNT];
    bool bCls[TOOLS_MAXCOUNT];
    bool bSilent;
#if defined(__WXGTK__)
    static void loadDefaultTerm(SigmaToolPrefs &tSigmaToolPrefs);
    char_t szTerm[LM_STRSIZE];
    char_t szShell[LM_STRSIZES];
    char_t szOpt[LM_STRSIZEM];
#endif
};

/*    -------------------------------------------------------------
    CometConfig
    Managing Preferences
    ------------------------------------------------------------- */
class CometConfig : public lfConfig
{

private:
    // disable the copy constructor
    CometConfig(const CometConfig &);
    CometConfig &operator=(CometConfig const &);

public:
    // Constructors
    explicit CometConfig(int iMaxSectionCount) : lfConfig(iMaxSectionCount)
    {
    }

    int sectionIndex(const char_t *pszSection)
    {
        if (pszSection) {
            for (int ii = 0; CometConfig::SECTION[ii] != NULL; ii++) {
                if (Tstricmp(pszSection, CometConfig::SECTION[ii]) == 0) {
                    return ii;
                }
            }
        }
        return -1;
    }
    int sectionRecordCount(int ii)
    {
        if ((ii >= 0) && (ii < this->SECTIONCOUNT)) {
            return this->SECTIONSIZE[ii];
        }
        return 0;
    }
    int keyIndex(const char_t *pszSection, const char_t *pszKey)
    {
        if (pszSection && pszKey) {
            int ii, ss;
            if ((ss = sectionIndex(pszSection)) < 0) {
                return -1;
            }
            char_t **psz = NULL;
            if (ss == 0) {
                psz = (char_t **)(CometConfig::KEYCOMMON);
            }
            else if (ss == 1) {
                psz = (char_t **)(CometConfig::KEYFIND);
            }
            else if (ss == 2) {
                psz = (char_t **)(CometConfig::KEYRECENT);
            }
            else if (ss == 3) {
                psz = (char_t **)(CometConfig::KEYGUI);
            }
            else if (ss == 4) {
                psz = (char_t **)(CometConfig::KEYTOOL);
            }
            if (psz == NULL) {
                return -1;
            }
            for (ii = 0; psz[ii] != NULL; ii++) {
                if (Tstricmp(pszKey, psz[ii]) == 0) {
                    return ii;
                }
            }
        }
        return -1;
    }

    static int SECTIONCOUNT;
    static int SECTIONSIZE[];
    static char_t const *const SECTION[];
    static char_t const *const KEYCOMMON[];
    static char_t const *const KEYFIND[];
    static char_t const *const KEYRECENT[];
    static char_t const *const KEYGUI[];
    static char_t const *const KEYTOOL[];

    bool load(const char_t *pszFilename, SigmaCommonPrefs &tSigmaCommonPrefs, SigmaGUIPrefs &tSigmaGUIPrefs, SigmaToolPrefs &tSigmaToolPrefs);
    bool save(const char_t *pszFilename, const SigmaCommonPrefs &tSigmaCommonPrefs, const SigmaGUIPrefs &tSigmaGUIPrefs, const SigmaToolPrefs &tSigmaToolPrefs);
};

#endif
