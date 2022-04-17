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


#ifndef EDITOR_CONFIG_H
#define EDITOR_CONFIG_H

#include "../../LibFile/include/File.h"

// EditorConfig: Managing Preferences

class EditorConfig : public lfConfig
{
private:
    // disable the copy constructor
    EditorConfig(const EditorConfig &);
    EditorConfig &operator=(EditorConfig const &);

public:
    // Constructors
    explicit EditorConfig(int iMaxSectionCount) : lfConfig(iMaxSectionCount)
    {
    }

    int sectionIndex(const char_t *pszSection)
    {
        if (pszSection) {
            for (int ii = 0; this->SECTION[ii] != NULL; ii++) {
                if (Tstricmp(pszSection, this->SECTION[ii]) == 0) {
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
                psz = (char_t **)KEYCOMMON;
            }
            else {
                psz = (char_t **)KEYSTYLE;
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
    static const int SECTION_ID[];
    static char_t const *const SECTION[];
    static char_t const *const KEYCOMMON[];
    static char_t const *const KEYSTYLE[];

    static void copy(ScintillaPrefs &tScintillaPrefsDest, const ScintillaPrefs &tScintillaPrefsSrc);
    bool load(const char_t *pszFilename, ScintillaPrefs &tScintillaPrefs);
    bool save(const char_t *pszFilename, const ScintillaPrefs &tScintillaPrefs);
};

#endif
