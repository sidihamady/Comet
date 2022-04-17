// -------------------------------------------------------------
// LibFile
//      Copyright(C) 2010-2022 Pr. Sidi HAMADY
//      http://www.hamady.org
//      sidi@hamady.org
//
//      :STABLE:VERSION180:BUILD2104:
//
//      Released under the MIT licence (https://opensource.org/licenses/MIT)
//      See Copyright Notice in COPYRIGHT
// -----------------------------------------------------------------------------------

#define LIBFILE_CORE

#define _CRT_SECURE_NO_DEPRECATE 1

#include "File.h"

#include <errno.h>

inline static int getLine(FILE *pf, char_t *pszBuffer, int nCount)
{
    int ii;
    char_t cr;

    for (ii = 0; ii < nCount; ii++) {
        cr = fgetc(pf);

        if (feof(pf) != 0) {
            if (ii > 0) {
                if (pszBuffer[ii - 1] != uT('\n')) {
                    pszBuffer[ii] = uT('\n');
                    pszBuffer[ii + 1] = uT('\0');
                    return (ii + 1);
                }
            }
            pszBuffer[ii] = uT('\0');
            return ii;
        }
        pszBuffer[ii] = static_cast<char_t>(cr);
        if (pszBuffer[ii] == uT('\n')) {
            if (ii > 0) {
                if (pszBuffer[ii - 1] == uT('\r')) {
                    pszBuffer[ii - 1] = uT('\n');
                    pszBuffer[ii] = uT('\0');
                    return ii;
                }
            }
            pszBuffer[ii + 1] = uT('\0');
            return (ii + 1);
        }
    }

    return 0;
}

LIBFILE_API int lf_getShortFilename(char_t *szFilenameShort, const char_t *filename)
{
    int ii, nlen;
    char_t *psz = NULL;
    lmByte bret = 0;

    nlen = static_cast<int>(Tstrlen(filename));

    if ((nlen < 6) || (nlen > LM_STRSIZE)) {
        return 1;
    }

    for (ii = nlen - 1; ii > 0; ii--) {
        if ((filename[ii] == uT('\\')) || (filename[ii] == uT('/'))) {
            psz = const_cast<char_t *>(&filename[ii + 1]);
            bret = 1;
            break;
        }
    }

    if (bret == 1) {
        Tstrcpy(szFilenameShort, static_cast<const char_t *>(psz));
        return 0;
    }

    return 1;
}
