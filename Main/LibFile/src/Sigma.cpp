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

LIBFILE_API void lmSigma::update(const char_t *pszMessage, int iCode, const char_t *pszSource, const char_t *pszAdvice)
{
    if (pszMessage) {
        m_iCode = iCode;
        Tstrncpy(m_szMessage, pszMessage, LM_STRSIZE - 1);
        m_szMessage[LM_STRSIZE - 1] = uT('\0');

        if (pszSource) {
            Tstrncpy(m_szSource, pszSource, LM_STRSIZEN - 1);
            m_szSource[LM_STRSIZEN - 1] = uT('\0');
        }
        else {
            Tmemset(m_szSource, 0, LM_STRSIZEN);
        }

        if (pszAdvice) {
            Tstrncpy(m_szAdvice, pszAdvice, LM_STRSIZE - 1);
            m_szAdvice[LM_STRSIZE - 1] = uT('\0');
        }
        else {
            Tmemset(m_szAdvice, 0, LM_STRSIZE);
        }

        print();
    }
    else {
        reset();
    }
}

LIBFILE_API void lmSigma::print(void)
{
    // always print errors
    lmByte iOutputTypeT = m_iOutputType;
    if ((m_iCode == LM_SIGMA_ERROR) && (((iOutputTypeT & 0xF0) != 0x10) && ((iOutputTypeT & 0x0F) != 0x01))) {
        iOutputTypeT = 0x10;
    }

    // :SEND: or :DEBG:
    const bool bSend = ((m_szMessage[0] == uT(':')) && (m_szMessage[1] == uT('S')) && (m_szMessage[2] == uT('E')) && (m_szMessage[3] == uT('N')) && (m_szMessage[4] == uT('D')) && (m_szMessage[5] == uT(':')));
    const bool bDebg = ((m_szMessage[0] == uT(':')) && (m_szMessage[1] == uT('D')) && (m_szMessage[2] == uT('E')) && (m_szMessage[3] == uT('B')) && (m_szMessage[4] == uT('G')) && (m_szMessage[5] == uT(':')));

    // always send :SEND: and :DEBG: message
    if ((iOutputTypeT & 0xF0) != 0x10) {
        if (bSend || bDebg) {
            iOutputTypeT = 0x10;
        }
    }

    if (((iOutputTypeT & 0xF0) != 0x10) && ((iOutputTypeT & 0x0F) != 0x01)) {
        return;
    }

    const int iLen = static_cast<int>(Tstrlen(static_cast<const char_t *>(m_szMessage)));
    if ((iLen < 1) || (iLen > (LM_STRSIZE - 1))) {
        return;
    }

    int iLenT = static_cast<int>(Tstrlen(static_cast<const char_t *>(m_szSource)));
    int iLenTT = static_cast<int>(Tstrlen(static_cast<const char_t *>(m_szAdvice)));

    bool bNLadded = (m_szMessage[iLen - 1] == uT('\n'));

    bool bNL = false;
    if (((iLenT > 1) || (iLenTT > 1)) && bNLadded) {
        m_szMessage[iLen - 1] = uT('\0');
        bNL = true;
    }

    char_t szExtMessage[LM_STRSIZEW];
    szExtMessage[0] = szExtMessage[LM_STRSIZEW - 1] = uT('\0');
    Tstrcpy(szExtMessage, m_szMessage);
    if (iLenT > 1) {
        Tstrcat(szExtMessage, uT(" [ "));
        Tstrcat(szExtMessage, m_szSource);
        Tstrcat(szExtMessage, uT(" ]"));
        if (iLenTT > 1) {
            Tstrcat(szExtMessage, uT(" "));
            Tstrcat(szExtMessage, m_szAdvice);
        }
        if (bNL) {
            Tstrcat(szExtMessage, uT("\n"));
        }
    }
    else if (iLenTT > 1) {
        Tstrcat(szExtMessage, uT(" [ "));
        Tstrcat(szExtMessage, m_szAdvice);
        Tstrcat(szExtMessage, uT(" ]"));
        if (bNL) {
            Tstrcat(szExtMessage, uT("\n"));
        }
    }

    // print to console or send to GUI if any
    if ((iOutputTypeT & 0xF0) == 0x10) {
        if (bSend == false) {
            Tputs(static_cast<const char_t *>(szExtMessage));
            if (bNLadded == false) {
                Tprintf(uT("\n"));
            }
        }

        if (m_pMessageSender != NULL) {
            m_pMessageSender->send(static_cast<const char_t *>(szExtMessage));
        }
    }

    // print to file
    if (((iOutputTypeT & 0x0F) == 0x01) && (m_szOutputFilename[0] != uT('\0'))) {
        // don't log special message (:SEND: or :DEBG:)
        if ((bSend == false) && (bDebg == false)) {
            // Open output file in append mode
            FILE *pf = Tfopen(static_cast<const char_t *>(m_szOutputFilename), uT("a+"));
            if (pf == NULL) {
                return;
            }
            fputws(static_cast<const char_t *>(szExtMessage), pf);
            if (bNLadded == false) {
                fputws(uT("\n"), pf);
            }
            fclose(pf);
        }
    }
}

LIBFILE_API lmByte *lmSigma::mallocBt(int iDim, bool bZeroMem /* = true*/)
{
    if ((iDim < 1) || (iDim > LM_MAX_POINTS)) {
        return NULL;
    }

    size_t iMemoryBlock = iDim * sizeof(lmByte);
    lmByte *pm = NULL;
    pm = static_cast<lmByte *>(malloc(iMemoryBlock));
    if (pm == NULL) {
        return NULL;
    }
    m_iUsedMemory += iMemoryBlock;
    m_iMaxUsedMemory += iMemoryBlock;
    m_MemoryBt.insert(std::pair<lmByte *, size_t>(pm, iMemoryBlock));
    if (bZeroMem) {
        memset(pm, 0, iMemoryBlock);
    }
    return pm;
}

LIBFILE_API lmByte *lmSigma::reallocBt(lmByte *ptr, int iNewDim)
{
    if ((iNewDim < 1) || (iNewDim > LM_MAX_POINTS) || (ptr == NULL)) {
        return NULL;
    }

    size_t iMemoryBlock = iNewDim * sizeof(lmByte);
    lmByte *pm = static_cast<lmByte *>(realloc(ptr, iMemoryBlock));
    if (pm == NULL) {
        return NULL;
    }

    std::map<lmByte *, size_t>::iterator it = m_MemoryBt.find(ptr);
    if (it != m_MemoryBt.end()) {
        size_t iMemoryBlock = it->second;
        m_iUsedMemory -= iMemoryBlock;
        m_MemoryBt.erase(it->first);
    }

    m_iUsedMemory += iMemoryBlock;
    m_iMaxUsedMemory += iMemoryBlock;
    m_MemoryBt.insert(std::pair<lmByte *, size_t>(pm, iMemoryBlock));

    return pm;
}

LIBFILE_API char_t *lmSigma::mallocChar(int iDim, bool bZeroMem /* = true*/)
{
    if ((iDim < 1) || (iDim > LM_MAX_POINTS)) {
        return NULL;
    }

    size_t iMemoryBlock = iDim * sizeof(char_t);
    char_t *pm = NULL;
    pm = static_cast<char_t *>(malloc(iMemoryBlock));
    if (pm == NULL) {
        return NULL;
    }
    m_iUsedMemory += iMemoryBlock;
    m_iMaxUsedMemory += iMemoryBlock;
    m_MemoryStr.insert(std::pair<char_t *, size_t>(pm, iMemoryBlock));
    if (bZeroMem) {
        memset(pm, 0, iMemoryBlock);
    }
    return pm;
}

LIBFILE_API char *lmSigma::mallocCharA(int iDim, bool bZeroMem /* = true*/)
{
    if ((iDim < 1) || (iDim > LM_MAX_POINTS)) {
        return NULL;
    }

    size_t iMemoryBlock = iDim * sizeof(char);
    char *pm = NULL;
    pm = static_cast<char *>(malloc(iMemoryBlock));
    if (pm == NULL) {
        return NULL;
    }
    m_iUsedMemory += iMemoryBlock;
    m_iMaxUsedMemory += iMemoryBlock;
    m_MemoryStrA.insert(std::pair<char *, size_t>(pm, iMemoryBlock));
    if (bZeroMem) {
        memset(pm, 0, iMemoryBlock);
    }
    return pm;
}

LIBFILE_API int *lmSigma::mallocInt(int iDim, bool bZeroMem /* = true*/)
{
    if ((iDim < 1) || (iDim > LM_MAX_POINTS) || (m_iUsedMemory >= LM_MAX_MEMORY)) {
        return NULL;
    }

    size_t iMemoryBlock = iDim * sizeof(int);
    int *pm = NULL;
    pm = static_cast<int *>(malloc(iMemoryBlock));
    if (pm == NULL) {
        return NULL;
    }
    m_iUsedMemory += iMemoryBlock;
    m_iMaxUsedMemory += iMemoryBlock;
    m_MemoryInt.insert(std::pair<int *, size_t>(pm, iMemoryBlock));
    if (bZeroMem) {
        memset(pm, 0, iMemoryBlock);
    }
    return pm;
}

LIBFILE_API double *lmSigma::mallocDbl(int iDim, bool bZeroMem /* = true*/)
{
    if ((iDim < 1) || (iDim > LM_MAX_POINTS)) {
        return NULL;
    }

    size_t iMemoryBlock = iDim * sizeof(double);
    double *pm = NULL;
    pm = static_cast<double *>(malloc(iMemoryBlock));
    if (pm == NULL) {
        return NULL;
    }
    m_iUsedMemory += iMemoryBlock;
    m_iMaxUsedMemory += iMemoryBlock;
    m_MemoryDbl.insert(std::pair<double *, size_t>(pm, iMemoryBlock));
    if (bZeroMem) {
        memset(pm, 0, iMemoryBlock);
    }
    return pm;
}

LIBFILE_API double *lmSigma::reallocDbl(double *ptr, int iNewDim)
{
    if ((iNewDim < 1) || (iNewDim > LM_MAX_POINTS) || (ptr == NULL)) {
        return NULL;
    }

    size_t iMemoryBlock = iNewDim * sizeof(double);
    double *pm = static_cast<double *>(realloc(ptr, iMemoryBlock));
    if (pm == NULL) {
        return NULL;
    }

    std::map<double *, size_t>::iterator it = m_MemoryDbl.find(ptr);
    if (it != m_MemoryDbl.end()) {
        size_t iMemoryBlock = it->second;
        m_iUsedMemory -= iMemoryBlock;
        m_MemoryDbl.erase(it->first);
    }

    m_iUsedMemory += iMemoryBlock;
    m_iMaxUsedMemory += iMemoryBlock;
    m_MemoryDbl.insert(std::pair<double *, size_t>(pm, iMemoryBlock));

    return pm;
}

LIBFILE_API double **lmSigma::mallocMat(int iRows, int iCols, bool bZeroMem /* = true*/)
{
    if ((iRows < 1) || (iRows > LM_MAX_POINTS) || (iCols < 1) || (iCols > LM_MAX_POINTS)) {
        return NULL;
    }

    size_t iMemoryBlock = iRows * sizeof(double *);
    double **pm = NULL;
    pm = static_cast<double **>(malloc(iMemoryBlock));
    if (pm == NULL) {
        return NULL;
    }
    m_iUsedMemory += iMemoryBlock;
    m_iMaxUsedMemory += iMemoryBlock;
    m_MemoryPtr.insert(std::pair<double **, size_t>(pm, iMemoryBlock));
    if (bZeroMem) {
        memset(pm, 0, iMemoryBlock);
    }

    for (int ii = 0; ii < iRows; ii++) {
        pm[ii] = mallocDbl(iCols);
        if (pm[ii] == NULL) {
            freeMat(&pm);
            return NULL;
        }
    }

    return pm;
}

LIBFILE_API void lmSigma::freeAll(void)
{
    if (m_bDebug) {
        char_t szT[LM_STRSIZE];
        szT[0] = szT[LM_STRSIZE - 1] = uT('\0');
        Tsnprintf(szT, LM_STRSIZE - 1, uT(":DEBG: Maximum used memory: %lu bytes\n:DEBG: Remaining used memory before last check: %lu bytes\n"),
                  (unsigned long)(m_iMaxUsedMemory), (unsigned long)(m_iUsedMemory));
        update(static_cast<const char_t *>(szT), LM_SIGMA_INFO);
    }

    if (m_MemoryBt.empty() == false) {
        std::map<lmByte *, size_t>::iterator it;
        for (it = m_MemoryBt.begin(); it != m_MemoryBt.end(); it++) {
            size_t iMemoryBlock = it->second;
            m_iUsedMemory -= iMemoryBlock;
            free(it->first);
        }
        m_MemoryBt.clear();
    }

    if (m_MemoryStr.empty() == false) {
        std::map<char_t *, size_t>::iterator it;
        for (it = m_MemoryStr.begin(); it != m_MemoryStr.end(); it++) {
            size_t iMemoryBlock = it->second;
            m_iUsedMemory -= iMemoryBlock;
            free(it->first);
        }
        m_MemoryStr.clear();
    }

    if (m_MemoryStrA.empty() == false) {
        std::map<char *, size_t>::iterator it;
        for (it = m_MemoryStrA.begin(); it != m_MemoryStrA.end(); it++) {
            size_t iMemoryBlock = it->second;
            m_iUsedMemory -= iMemoryBlock;
            free(it->first);
        }
        m_MemoryStrA.clear();
    }

    if (m_MemoryInt.empty() == false) {
        std::map<int *, size_t>::iterator it;
        for (it = m_MemoryInt.begin(); it != m_MemoryInt.end(); it++) {
            size_t iMemoryBlock = it->second;
            m_iUsedMemory -= iMemoryBlock;
            free(it->first);
        }
        m_MemoryInt.clear();
    }

    if (m_MemoryDbl.empty() == false) {
        std::map<double *, size_t>::iterator it;
        for (it = m_MemoryDbl.begin(); it != m_MemoryDbl.end(); it++) {
            size_t iMemoryBlock = it->second;
            m_iUsedMemory -= iMemoryBlock;
            free(it->first);
        }
        m_MemoryDbl.clear();
    }

    if (m_MemoryPtr.empty() == false) {
        std::map<double **, size_t>::iterator it;
        size_t iMemoryBlock = 0;
        size_t iDim = 0;
        for (it = m_MemoryPtr.begin(); it != m_MemoryPtr.end(); it++) {
            iMemoryBlock = it->second;
            iDim = iMemoryBlock / static_cast<int>(sizeof(double *));
            m_iUsedMemory -= iMemoryBlock;
            if (iDim > 0) {
                for (size_t ii = 0; ii < iDim; ii++) {
                    if ((it->first)[ii]) {
                        std::map<double *, size_t>::iterator itt = m_MemoryDbl.find((it->first)[ii]);
                        iMemoryBlock = itt->second;
                        m_iUsedMemory -= iMemoryBlock;
                        free(itt->first);
                    }
                }
                free(it->first);
            }
        }
        m_MemoryPtr.clear();
    }

    if (m_bDebug) {
        char_t szT[LM_STRSIZE];
        szT[0] = szT[LM_STRSIZE - 1] = uT('\0');
        Tsnprintf(szT, LM_STRSIZE - 1, uT(":DEBG: Remaining used memory after last check: %lu bytes\n"), (unsigned long)(m_iUsedMemory));
        update(static_cast<const char_t *>(szT), LM_SIGMA_INFO);
    }
}

LIBFILE_API void lmSigma::freeBt(lmByte **pm)
{
    if (pm && *pm) {
        std::map<lmByte *, size_t>::iterator it = m_MemoryBt.find(*pm);
        if (it != m_MemoryBt.end()) {
            size_t iMemoryBlock = it->second;
            m_iUsedMemory -= iMemoryBlock;
            free(*pm);
            *pm = NULL;
            m_MemoryBt.erase(it->first);
        }
    }
}

LIBFILE_API void lmSigma::freeStr(char_t **pm)
{
    if (pm && *pm) {
        std::map<char_t *, size_t>::iterator it = m_MemoryStr.find(*pm);
        if (it != m_MemoryStr.end()) {
            size_t iMemoryBlock = it->second;
            m_iUsedMemory -= iMemoryBlock;
            free(*pm);
            *pm = NULL;
            m_MemoryStr.erase(it->first);
        }
    }
}

LIBFILE_API void lmSigma::freeStrA(char **pm)
{
    if (pm && *pm) {
        std::map<char *, size_t>::iterator it = m_MemoryStrA.find(*pm);
        if (it != m_MemoryStrA.end()) {
            size_t iMemoryBlock = it->second;
            m_iUsedMemory -= iMemoryBlock;
            free(*pm);
            *pm = NULL;
            m_MemoryStrA.erase(it->first);
        }
    }
}

LIBFILE_API void lmSigma::freeInt(int **pm)
{
    if (pm && *pm) {
        std::map<int *, size_t>::iterator it = m_MemoryInt.find(*pm);
        if (it != m_MemoryInt.end()) {
            size_t iMemoryBlock = it->second;
            m_iUsedMemory -= iMemoryBlock;
            free(*pm);
            *pm = NULL;
            m_MemoryInt.erase(it->first);
        }
    }
}

LIBFILE_API void lmSigma::freeDbl(double **pm)
{
    if (pm && *pm) {
        std::map<double *, size_t>::iterator it = m_MemoryDbl.find(*pm);
        if (it != m_MemoryDbl.end()) {
            size_t iMemoryBlock = it->second;
            m_iUsedMemory -= iMemoryBlock;
            free(*pm);
            *pm = NULL;
            m_MemoryDbl.erase(it->first);
        }
    }
}

LIBFILE_API void lmSigma::freeMat(double ***pm)
{
    if (pm && *pm && **pm) {
        size_t iMemoryBlock = 0;
        size_t iDim = 0;
        std::map<double **, size_t>::iterator it = m_MemoryPtr.find(*pm);
        if (it != m_MemoryPtr.end()) {
            iMemoryBlock = it->second;
            iDim = iMemoryBlock / static_cast<int>(sizeof(double *));
            m_iUsedMemory -= iMemoryBlock;
            if (iDim > 0) {
                double **pmat = it->first;
                for (size_t ii = 0; ii < iDim; ii++) {
                    freeDbl(&pmat[ii]);
                }
            }
            free(*pm);
            *pm = NULL;
            m_MemoryPtr.erase(it->first);
        }
    }
}

LIBFILE_API int lm_removeSpace(char_t *szBuffer, int *nlen)
{
    int ii, nSpaces;
    char_t *psz = NULL;
    char_t szTmp[LM_STRSIZEW];

    *nlen = static_cast<int>(Tstrlen(static_cast<const char_t *>(szBuffer)));
    if ((*nlen < 1) || (*nlen > LM_STRSIZEW)) {
        return 0;
    }

    if (*nlen == 1) {
        if (isspace(szBuffer[0])) {
            szBuffer[0] = uT('\0');
            return 1;
        }
        return 0;
    }

    Tmemset(szTmp, 0, LM_STRSIZEW);

    ii = 0;
    nSpaces = 0;
    while (1) {
        if ((ii >= *nlen) || (nSpaces >= *nlen)) {
            return nSpaces;
        }
        if (szBuffer[ii] == uT('\0')) {
            return nSpaces;
        }
        if (isspace(szBuffer[ii])) {
            nSpaces += 1;
            psz = &szBuffer[ii + 1];
            Tstrcpy(szTmp, psz);
            szBuffer[ii] = uT('\0');
            Tstrcat(szBuffer, szTmp);
            *nlen -= 1;
        }
        else {
            ii += 1;
        }
    }

    return nSpaces;
}

LIBFILE_API int lm_getFormat(const char_t *pszFmt, int *piPrefix, int *piSuffix)
{
    int ii, jj, iLen = 0;
    char_t szT[16], *pz = NULL;
    lmByte bErr = 0x00;

    if ((pszFmt == NULL) || (piPrefix == NULL) || (piSuffix == NULL)) {
        return -1;
    }

    *piPrefix = 0;
    *piSuffix = 0;

    iLen = (int) Tstrlen(pszFmt);

    if ((iLen < 3) || (iLen > 16)) {
        return -1;
    }

    if (pszFmt[0] != uT('%')) {
        return -1;
    }

    jj = 0;
    Tstrcpy(szT, pszFmt);
    for (ii = 0; ii < iLen; ii++) {
        if (szT[ii] == uT('.')) {
            jj = ii;
            break;
        }
    }

    if (jj != 0) {
        if (jj == 1) {
            szT[iLen - 1] = uT('\0');
            pz = &szT[jj + 1];
            *piSuffix = static_cast<int>(lm_toDouble(pz, &bErr, NULL));
        }
        else {
            szT[jj] = uT('\0');
            pz = &szT[1];
            *piPrefix = static_cast<int>(lm_toDouble(pz, &bErr, NULL));
            szT[jj] = pszFmt[jj];
            szT[iLen - 1] = uT('\0');
            pz = &szT[jj + 1];
            *piSuffix = static_cast<int>(lm_toDouble(pz, &bErr, NULL));
        }
    }
    else {
        pz = &szT[1];
        szT[iLen - 1] = uT('\0');
        *piPrefix = static_cast<int>(lm_toDouble(pz, &bErr, NULL));
    }

    return 0;
}

LIBFILE_API int lm_reformat(char_t *szBuffer, bool bSpecChar /* = false*/)
{
    int ii, nlen, itmp, istart, ifmt;
    double fval;
    char_t szTmp[LM_STRSIZEW], *psz = NULL;

    nlen = static_cast<int>(Tstrlen(static_cast<const char_t *>(szBuffer)));
    if ((nlen < 1) || (nlen > LM_STRSIZEW)) {
        return 0;
    }
    if (nlen == 1) {
        return 1;
    }

    if (bSpecChar) {
        psz = const_cast<char_t *>(Tstrstr(static_cast<const char_t *>(szBuffer), uT("#INF")));
        if (psz) {
            szBuffer[0] = 0x221E;
            szBuffer[1] = uT('\0');
            return 0;
        }
        psz = const_cast<char_t *>(Tstrstr(static_cast<const char_t *>(szBuffer), uT("#IND")));
        if (psz) {
            szBuffer[0] = uT('?');
            szBuffer[1] = uT('\0');
            return 0;
        }
        psz = const_cast<char_t *>(Tstrstr(static_cast<const char_t *>(szBuffer), uT("#NAN")));
        if (psz) {
            szBuffer[0] = uT('?');
            szBuffer[1] = uT('\0');
            return 0;
        }
    }

    Tmemset(static_cast<char_t *>(szTmp), 0, LM_STRSIZEW);

    errno = 0;
    fval = lm_toDouble(szBuffer, NULL, NULL);
    if ((fval == -HUGE_VAL) || (fval == HUGE_VAL) || (errno == ERANGE)) {
        return 0;
    }
    if (fval == 0.0) {
        Tstrcpy(szBuffer, uT("0"));
        return 0;
    }

    ifmt = -1;
    istart = nlen - 1;
    for (ii = istart; ii >= 0; ii--) {
        if (szBuffer[ii] == uT(',')) {
            szBuffer[ii] = uT('.');
        }
    }
    for (ii = istart; ii >= 0; ii--) {
        if ((szBuffer[ii] == uT('e')) || (szBuffer[ii] == uT('E'))) { // Format %e
            if (ii >= 1) {
                ifmt = 1;
            }
            break;
        }
    }
    if (ifmt == -1) {
        for (ii = istart; ii >= 0; ii--) {
            if (szBuffer[ii] == uT('.')) { // Format %f
                ifmt = 0;
                break;
            }
        }
    }
    if (ifmt == -1) {
        return 0;
    }

    if (ifmt == 1) { // Format %e
        psz = &szBuffer[ii];
        itmp = static_cast<int>(Tstrlen(static_cast<const char_t *>(psz)));
        if (itmp == 3) {
            if (*(psz + 1) == uT('0')) { //e0x
                *(psz + 1) = *(psz + 2);
                *(psz + 2) = uT('\0');
            }
            else if (((*(psz + 1) == uT('+')) || (*(psz + 1) == uT('-'))) && (*(psz + 2) == uT('0'))) { //e+0
                *psz = uT('\0');
            }
            Tstrcpy(szTmp, static_cast<const char_t *>(psz));
        }
        else if ((itmp >= 4) && (itmp <= 5)) {
            if (((*(psz + 1) == uT('+')) || (*(psz + 1) == uT('-'))) && (*(psz + 2) == uT('0'))) {
                if (itmp == 4) {                 //e+0x
                    if (*(psz + 3) == uT('0')) { //e+00
                        *psz = uT('\0');
                    }
                    else {
                        *(psz + 2) = *(psz + 3);
                        *(psz + 3) = uT('\0');
                    }
                }
                else if (itmp == 5) {
                    if (*(psz + 3) == uT('0')) {     //e+00x
                        if (*(psz + 4) == uT('0')) { //e+000
                            *psz = uT('\0');
                        }
                        else {
                            *(psz + 2) = *(psz + 4);
                            *(psz + 3) = uT('\0');
                        }
                    }
                    else { //e+0xy
                        *(psz + 2) = *(psz + 3);
                        *(psz + 3) = *(psz + 4);
                        *(psz + 4) = uT('\0');
                    }
                }
            }
            else if ((*(psz + 1) == uT('0')) && (*(psz + 2) == uT('0'))) {
                if (itmp == 4) {                 //e00x
                    if (*(psz + 3) == uT('0')) { //e+000
                        *psz = uT('\0');
                    }
                    else {
                        *(psz + 1) = *(psz + 3);
                        *(psz + 2) = uT('\0');
                    }
                }
                else if (itmp == 5) { //e00xy
                    *(psz + 1) = *(psz + 3);
                    *(psz + 2) = *(psz + 4);
                    *(psz + 3) = uT('\0');
                }
            }
            Tstrcpy(szTmp, static_cast<const char_t *>(psz));
        }
        istart = ii - 1;
    }

    for (ii = istart; ii >= 0; ii--) {
        if (szBuffer[ii] != uT('0')) {
            break;
        }
    }
    if (szBuffer[ii] == uT('.')) {
        szBuffer[ii] = uT('\0');
    }
    else {
        szBuffer[ii + 1] = uT('\0');
    }

    if ((ifmt == 1) && (szTmp[0] != uT('\0'))) {
        Tstrcat(szBuffer, static_cast<const char_t *>(szTmp));
    }

    return static_cast<int>(Tstrlen(szBuffer));
}

LIBFILE_API int lm_reformatAuto(double fVal, char_t *szBuffer, int nCount, char_t *pszFormat /* = NULL*/, int *pFmt /* = NULL*/)
{
    if ((nCount < 1) || (nCount > LM_STRSIZEW)) {
        return 0;
    }

    double ftmp = fabs(fVal);

    if (ftmp == 0.0) {
        szBuffer[0] = uT('0');
        szBuffer[1] = uT('\0');
        if (pszFormat != NULL) {
            Tstrcpy(pszFormat, LM_FORMAT_FLONG);
            if (pFmt != NULL) {
                *pFmt = 0;
            }
        }
        return 1;
    }

    if ((ftmp > 1e-6) && (ftmp < 1e6)) {
        Tsnprintf(szBuffer, static_cast<size_t>(nCount), LM_FORMAT_FLONG, fVal);
        if (pszFormat != NULL) {
            Tstrcpy(pszFormat, LM_FORMAT_FLONG);
            if (pFmt != NULL) {
                *pFmt = 0;
            }
        }
    }
    else {
        Tsnprintf(szBuffer, static_cast<size_t>(nCount), LM_FORMAT_ELONG, fVal);
        if (pszFormat != NULL) {
            Tstrcpy(pszFormat, LM_FORMAT_ELONG);
            if (pFmt != NULL) {
                *pFmt = 1;
            }
        }
    }

    lm_reformat(szBuffer, true);

    if ((pszFormat != NULL) && (pFmt != NULL) && (fVal != 0.0)) {
        if (*pFmt == 0) {
            ftmp = lm_toDouble(szBuffer, NULL, NULL);
            if (ftmp == 0.0) {
                Tstrcpy(pszFormat, LM_FORMAT_ELONG);
                *pFmt = 1;
            }
        }
    }

    return static_cast<int>(Tstrlen(szBuffer));
}

LIBFILE_API double lm_toDouble(const char_t *szInput, lmByte *pError /* = NULL*/, char_t **pszEnd /* = NULL*/)
{
    double fVal = 0.0;

    errno = 0;
    if (pError) {
        *pError = 0;
    }

    if (szInput[0] == uT('\0')) {
        if (pszEnd) {
            *pszEnd = const_cast<char_t *> (szInput);
        }
        return 0.0;
    }

    if (szInput[1] == uT('\0')) {
        if (pszEnd) {
            *pszEnd = const_cast<char_t *>(szInput) + 1;
        }
        return static_cast<double>(szInput[0] - uT('0'));
    }

    fVal = static_cast<double>(Tstrtod(szInput, pszEnd));

    if (pError) {
        if ((errno == ERANGE) || (fVal == -HUGE_VAL) || (fVal == HUGE_VAL)) {
            *pError = 1;
        }
    }

    return fVal;
}

LIBFILE_API double lm_toDoubleA(const char *szInput, lmByte *pError /* = NULL*/, char **pszEnd /* = NULL*/)
{
    double fVal = 0.0;

    errno = 0;
    if (pError) {
        *pError = 0;
    }

    if (szInput[0] == ('\0')) {
        if (pszEnd) {
            *pszEnd = const_cast<char *> (szInput);
        }
        return 0.0;
    }

    if (strlen(szInput) == 1) {
        if (pszEnd) {
            *pszEnd = const_cast<char *>(szInput) + 1;
        }
        return static_cast<double>(szInput[0] - ('0'));
    }

    fVal = static_cast<double>(strtod(szInput, pszEnd));

    if (pError) {
        if ((errno == ERANGE) || (fVal == -HUGE_VAL) || (fVal == HUGE_VAL)) {
            *pError = 1;
        }
    }

    return fVal;
}

LIBFILE_API double lm_round(double *fVal, bool bUp /* = true*/)
{
    double ft, fa, fb;

    if (*fVal == 0.0) {
        return *fVal;
    }

    // Calculate nPlaces: if log10(x) < 0 --> nPlaces = (int) ceil(fabs(log10(x)))
    int nPlaces = 0;
    double fPlaces = fabs(*fVal);
    if (fPlaces > DBL_EPSILON) {
        fPlaces = log10(fPlaces);
        if (fPlaces < 0.0) {
            nPlaces = (int)ceil(fabs(fPlaces));
            if (nPlaces > 12) {
                nPlaces = 12;
            }
        }
    }

    ft = *fVal;
    if (ft < 0.0) {
        ft = fabs(ft);
    }
    fa = static_cast<double>(pow(10.0, static_cast<double>(nPlaces)));
    fb = ft * fa;
    fb = static_cast<double>(bUp ? ceil(fb) : floor(fb));
    ft = (fb / fa);

    *fVal = (*fVal >= 0.0) ? ft : -ft;

    return *fVal;
}

LIBFILE_API double lm_round(long double *fVal, bool bUp /* = true*/)
{
    long double ft, fa, fb;

    if (*fVal == 0.0) {
        return *fVal;
    }

    // Calculate nPlaces: if log10(x) < 0 --> nPlaces = (int) ceil(fabs(log10(x)))
    int nPlaces = 0;
    double fPlaces = fabs(*fVal);
    if (fPlaces > DBL_EPSILON) {
        fPlaces = log10(fPlaces);
        if (fPlaces < 0.0) {
            nPlaces = (int)ceil(fabs(fPlaces));
            if (nPlaces > 12) {
                nPlaces = 12;
            }
        }
    }

    ft = *fVal;
    if (ft < 0.0) {
        ft = fabs(ft);
    }
    fa = static_cast<long double>(pow(10.0, static_cast<long double>(nPlaces)));
    fb = ft * fa;
    fb = static_cast<long double>(bUp ? ceil(fb) : floor(fb));
    ft = (fb / fa);

    *fVal = (*fVal >= 0.0) ? ft : -ft;

    return *fVal;
}
