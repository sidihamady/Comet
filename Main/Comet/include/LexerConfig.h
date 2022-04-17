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


#ifndef LEXER_CONFIG_H
#define LEXER_CONFIG_H

#include "../../LibFile/include/File.h"

#define LEXER_STRSIZE (LM_STRSIZE << 1)

// LexerConfig: Managing Lexer Preferences

class LexerConfig : public lfConfig
{
private:
    // disable the copy constructor
    LexerConfig(const LexerConfig &);
    LexerConfig &operator=(LexerConfig const &);

public:
    // Constructors
    explicit LexerConfig(int iMaxSectionCount) : lfConfig(iMaxSectionCount)
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
                psz = (char_t **)(this->KEY);
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
    static char_t const *const KEY[];

    bool load(const char_t *pszFilename);
    bool save(const char_t *pszFilename);
    bool save(const char_t *pszFilename, const char_t *pszName, const char_t *pszFilepattern, const char_t *pszKeywords);
};

#endif
