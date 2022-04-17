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
#include "FindThread.h"
#include "FindDirDlg.h"

#include <wx/file.h>     // raw file io support
#include <wx/filename.h> // filename support

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <shellapi.h>
#include <windows.h>
#include <wx/msw/winundef.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#endif

// Internal function (no error check)
static int findString(const char_t *pszStr, int iStart, const char_t *pszFind, int iFindLen, bool bWord)
{
    int iLen = (int)(Tstrlen(pszStr));
    if (iStart > (iLen - iFindLen)) {
        return -1;
    }

    int iF = -1;
    const char_t *pszT = static_cast<const char_t *>(&pszStr[iStart]);
    const char_t *pszRes = static_cast<const char_t *>(Tstrstr(pszT, pszFind));
    if (pszRes) {
        iF = (int)(pszRes - pszT);
        if (iF > ((iLen - iStart) - iFindLen)) {
            iF = -1;
        }
    }

    bool bFound = (iF >= 0);

    if (bFound && bWord) {
        if (iF > 0) {
            if (wxIsalnum((int)(pszT[iF - 1]))) {
                iF = -1;
            }
        }
        if (bFound) {
            if (wxIsalnum((int)(pszT[iF + iFindLen]))) {
                iF = -1;
            }
        }
    }

    return iF;
}

// Internal function (no error check)
static int findStringAll(const char_t *pszStr, int iStart, const char_t *pszFind, int iFindLen, bool bWord)
{
    int iF = -1;
    int iFind = 0;
    while ((iF = findString(pszStr, iStart, pszFind, iFindLen, bWord)) >= 0) {
        if (iF < 0) {
            break;
        }
        iFind += 1;
        iStart += (iF + iFindLen);
    }
    return iFind;
}

// Internal function (no error check)
// File names should not contain any non-ASCII character
static int replaceFileA(const char *szFrom, const char *szTo)
{

#ifdef WIN32

    DWORD dwAttrs = GetFileAttributesA(szTo);
    if (dwAttrs == INVALID_FILE_ATTRIBUTES) {
        return -1;
    }

    if (dwAttrs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_TEMPORARY)) {
        return -1;
    }

    if (CopyFileA(szFrom, szTo, FALSE)) {
        return 0;
    }

    return -1;

#else

    int fdFrom, fdTo;
    off_t ifrom = 0, ito = 0, itosf = 0;
    struct stat stbuf;

    fdFrom = open(szFrom, O_RDONLY);
    if (fdFrom < 0) {
        return -1;
    }

    if ((fstat(fdFrom, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode))) {
        close(fdFrom);
        return -1;
    }
    ifrom = stbuf.st_size;

    fdTo = open(szTo, O_WRONLY | O_TRUNC);
    if (fdTo < 0) {
        close(fdFrom);
        return -1;
    }

    sendfile(fdTo, fdFrom, &ito, ifrom);

    itosf = ito;
    if ((fstat(fdTo, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode))) {
        close(fdFrom);
        close(fdTo);
        return -1;
    }
    ito = stbuf.st_size;

    close(fdFrom);
    close(fdTo);

    if ((ito != ifrom) || (ito != itosf)) {
        return -1;
    }

    return 0;

#endif
}

void FindThread::updateFilename(const wxString &strFilename)
{
    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    FindDirDlg *pDlg = pFrame->getFindDirDlg();
    if (pDlg == NULL) {
        return;
    }

    if (pDlg->m_pThreadMutex->TryLock() == wxMUTEX_NO_ERROR) {
        pDlg->m_strFilename = wxString(LM_CSTR(strFilename));
        pDlg->m_pThreadMutex->Unlock();
    }
    else {
        // Sleep 50 ms and retry
        Tsleep(50);
        if (pDlg->m_pThreadMutex->TryLock() == wxMUTEX_NO_ERROR) {
            pDlg->m_strFilename = wxString(LM_CSTR(strFilename));
            pDlg->m_pThreadMutex->Unlock();
        }
    }

    return;
}

void FindThread::updateBuffer(const wxString &strBuffer, int iBufferLen)
{
    if (iBufferLen < 1) {
        return;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return;
    }

    FindDirDlg *pDlg = pFrame->getFindDirDlg();
    if (pDlg == NULL) {
        return;
    }

    if (pDlg->m_pThreadMutex->TryLock() == wxMUTEX_NO_ERROR) {
        pDlg->m_arrBuffer.Add(LM_CSTR(strBuffer));
        pDlg->m_pThreadMutex->Unlock();
    }
    else {
        // Sleep 50 ms and retry
        Tsleep(50);
        if (pDlg->m_pThreadMutex->TryLock() == wxMUTEX_NO_ERROR) {
            pDlg->m_arrBuffer.Add(LM_CSTR(strBuffer));
            pDlg->m_pThreadMutex->Unlock();
        }
    }

    return;
}

bool FindThread::doFindInFile(const wxString &strFilename)
{
    // SEARCH
    FILE *fp = NULL;
    FILE *fpRe = NULL;
    int iLine;
    int jj;
    char_t cT;

    int iFind = m_iFind;

    fp = Tfopen(LM_CSTR(strFilename), uT("rt"));
    if (fp == NULL) {
        return true;
    }

    wxFileName fname = strFilename;
    wxString strExtRaw = fname.GetExt();

    if (CometApp::isExecutable(strFilename, strExtRaw)) {
        fclose(fp);
        fp = NULL;
        return true;
    }

    if (m_strType.IsSameAs(uT("Makefile")) || m_strType.IsSameAs(uT("makefile"))) {
        if ((fname.GetFullName().IsSameAs(uT("Makefile")) == false) && (fname.GetFullName().IsSameAs(uT("makefile")) == false)) {
            fclose(fp);
            fp = NULL;
            updateFilename(uT("Searching..."));
            return true;
        }
    }
    else if (strExtRaw.IsEmpty()) {
        if (m_strType.Contains(uT("*.*")) == false) {
            fclose(fp);
            fp = NULL;
            updateFilename(uT("Searching..."));
            return true;
        }
    }
    else {
        wxString strExt = uT("*.");
        strExt += strExtRaw;
        strExt += uT(";");
        if (m_strType.Contains(strExt) == false) {
            strExt = uT("*.");
            strExt += strExtRaw;
            strExt += uT(")");
            if (m_strType.Contains(strExt) == false) {
                if (m_strType.Contains(uT("*.*")) == false) {
                    fclose(fp);
                    fp = NULL;
                    updateFilename(uT("Searching..."));
                    return true;
                }
            }
        }
    }

    wxString strRelFilename = strFilename.Mid(m_iDirLen);
    int iRelFilenameLen = (int)(strRelFilename.Length());
    updateFilename(strRelFilename);

    char_t *pszT = NULL;
    if (m_bReplace) {
        pszT = Ttmpnam(m_szFilenameRe);
        if (pszT == NULL) {
            fclose(fp);
            fp = NULL;
            return false;
        }
        fpRe = Tfopen((const char_t *)m_szFilenameRe, uT("w"));
        if (fpRe == NULL) {
            fclose(fp);
            fp = NULL;
            return false;
        }
    }

    wxString strBuffer = wxEmptyString;
    int iBufferLen = 0;

    int iLineLen, iFL, iF, ll;

    m_szLine[0] = m_szLine[FIND_MAXLINESIZE - 1] = uT('\0');
    m_szLineCase[0] = m_szLineCase[FIND_MAXLINESIZE - 1] = uT('\0');
    bool bFound = false;
    char_t *pszLine = NULL;
    const int iReplace = m_iReplace;
    size_t iFX = 0, iRX = 0;
    bool bReplace = m_bReplace;
#ifdef _WIN32
    int iLFnum = 0;
    int iCRLFnum = 0;
#endif

    for (iLine = 1; iLine <= LF_SCRIPT_MAXLINES; iLine++) {

        m_szLine[0] = uT('\0');
        if (Tfgets(m_szLine, FIND_MAXLINESIZE - 1, fp) == NULL) {
            break;
        }

        if (uT('\0') == m_szLine[0]) {
            bReplace = false;
            break;
        }

        iLineLen = (int)(Tstrlen(static_cast<const char_t *>(m_szLine)));

#ifdef _WIN32
        // Detect inconsistent end of line: fgets normalise EOL to \n. If \r\n detected, it means that the previous line was \r ended.
        if ((iLineLen >= 2) && (uT('\r') == m_szLine[iLineLen - 2]) && (uT('\n') == m_szLine[iLineLen - 1])) {
            ++iCRLFnum;
        }
        else if ((iLineLen >= 2) && (uT('\r') != m_szLine[iLineLen - 2]) && (uT('\n') == m_szLine[iLineLen - 1])) {
            ++iLFnum;
        }
        else if ((1 == iLineLen) && (uT('\n') == m_szLine[iLineLen - 1])) {
            ++iLFnum;
        }
        if ((iCRLFnum > 0) && (iLFnum > iCRLFnum)) {
            iLine += iCRLFnum;
            iCRLFnum = 0;
        }
#else
        // Detect inconsistent end of line: \r\r\n read as one line.
        if ((iLineLen >= 3) && (uT('\r') == m_szLine[iLineLen - 3]) && (uT('\r') == m_szLine[iLineLen - 2]) && (uT('\n') == m_szLine[iLineLen - 1])) {
            ++iLine;
        }
#endif

        if (((1 == iLineLen) && ((uT('\r') == m_szLine[0]) || (uT('\n') == m_szLine[0]))) || ((2 == iLineLen) && ((uT('\r') == m_szLine[0]) && (uT('\n') == m_szLine[1]))) || (iLineLen < m_iFindLen)) {
            if (bReplace && (fpRe != NULL)) {
                Tfputs(static_cast<const char_t *>(m_szLine), fpRe);
            }
            continue;
        }

        // Detect incomplete line (larger than the buffer size FIND_MAXLINESIZE) and disable replacement, if needed.
        if (m_szLine[iLineLen - 1] != LM_NEWLINE_CHAR) {
            if (bReplace && (feof(fp) == false)) {
                bReplace = false; // Do not modify file if incomplete line detected
            }
            break;
        }

        if (m_bCase == false) {
            for (ll = 0; ll < iLineLen; ll++) {
                m_szLineCase[ll] = (char_t)wxTolower((int)(m_szLine[ll]));
            }
            m_szLineCase[iLineLen] = uT('\0');
            pszLine = (char_t *)(m_szLineCase);
        }
        else {
            pszLine = (char_t *)(m_szLine);
        }

        iF = findString(static_cast<const char_t *>(pszLine), 0, static_cast<const char_t *>(m_pszFind), m_iFindLen, m_bWord);
        bFound = (iF >= 0);

        if (bFound) {

            strBuffer = strRelFilename;
            iBufferLen = iRelFilenameLen;
            strBuffer += uT("[|]");
            iBufferLen += 3;
            strBuffer += wxString::Format(uT("%d[|]"), iLine);
            iBufferLen += 9;
            iFL = iF + FIND_MAXLENGTH - 1;
            if (iFL >= iLineLen) {
                iFL = iLineLen - 1;
            }
            if ((iFL - iF) <= m_iFindLen) {
                strBuffer += m_strFind;
                iBufferLen += m_iFindLen;
            }
            else {
                cT = m_szLine[iFL];
                m_szLine[iFL] = uT('\0');
                strBuffer += static_cast<const char_t *>(&m_szLine[iF]);
                iBufferLen += iFL;
                m_szLine[iFL] = cT;
            }

            strBuffer += uT("[|]None[|]None");
            iBufferLen += 16;

            updateBuffer(strBuffer, iBufferLen);

            if (bReplace && (fpRe != NULL)) {
                strBuffer = static_cast<const char_t *>(m_szLine);
                // if case insensitive search, Replace returns without modifying line
                if ((jj = (int)(strBuffer.Replace(static_cast<const char_t *>(m_szFind), static_cast<const char_t *>(m_szReplace), true))) > 0) {
                    m_iFind += jj;
                    m_iReplace += jj;
                    iFX += jj, iRX += jj;
                }
                else {
                    jj = findStringAll(static_cast<const char_t *>(pszLine), 0, static_cast<const char_t *>(m_pszFind), m_iFindLen, m_bWord);
                    m_iFind += jj;
                    iFX += jj;
                }
                Tfputs(LM_CSTR(strBuffer), fpRe);
            }
            else {
                jj = findStringAll(static_cast<const char_t *>(pszLine), 0, static_cast<const char_t *>(m_pszFind), m_iFindLen, m_bWord);
                m_iFind += jj;
                iFX += jj;
            }
        }
        else {
            if (bReplace && (fpRe != NULL)) {
                Tfputs(static_cast<const char_t *>(m_szLine), fpRe);
            }
        }
    }

    fclose(fp);
    fp = NULL;

    // sanity check for replacement
    if (bReplace && ((iFX != iRX) || (m_iReplace <= iReplace))) {
        bReplace = false; // Do not modify file if something got wrong
    }

    if (m_iFind > iFind) {
        m_iFileCount += 1;
    }

    m_iFileTotal += 1;

    if (fpRe != NULL) {

        fclose(fpRe);
        fpRe = NULL;

        if (bReplace) {
            strncpy(m_szFilenameA, LM_U8STR(strFilename), LM_STRSIZE - 1);
            wxString strFilenameRe = static_cast<const char_t *>(m_szFilenameRe);
            strcpy(m_szFilenameReA, LM_U8STR(strFilenameRe));
            if (replaceFileA((const char *)(m_szFilenameReA), (const char *)(m_szFilenameA)) != 0) {
                bReplace = false;
                Tunlink(static_cast<const char_t *>(m_szFilenameRe));
                return false;
            }
        }

        Tunlink(static_cast<const char_t *>(m_szFilenameRe));
    }
    //

    return true;
}

#ifdef WIN32

void FindThread::doFindInDir(const char_t *pszDir)
{
    WIN32_FIND_DATA fdFile;
    HANDLE hFile = INVALID_HANDLE_VALUE, hFileT = INVALID_HANDLE_VALUE;
    static FILETIME ft;

    char_t szPath[PATH_MAXLEN];
    int iLen;

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());

    if ((NULL == pFrame) || m_bStop) {
        return;
    }

    FindDirDlg *pDlg = pFrame->getFindDirDlg();
    if (pDlg == NULL) {
        return;
    }

    wxString strResult, strFilename, strExtRaw;
    wxFileName fname;

    Tsprintf(szPath, uT("%s\\*.*"), pszDir);

    if ((hFile = FindFirstFile(szPath, &fdFile)) == INVALID_HANDLE_VALUE) {
        m_bStop = true;
        return;
    }

    do {

        if (pDlg->m_pThreadMutex->TryLock() == wxMUTEX_NO_ERROR) {
            m_bStop = pDlg->m_bThreadStop;
            pDlg->m_pThreadMutex->Unlock();
        }

        if (m_bStop) {
            break;
        }

        if ((Tstrcmp(fdFile.cFileName, uT(".")) == 0) || (Tstrcmp(fdFile.cFileName, uT("..")) == 0)) {
            continue;
        }

        iLen = Tsnprintf(szPath, PATH_MAXLEN, uT("%s\\%s"), pszDir, fdFile.cFileName);

        if (iLen >= PATH_MAXLEN) {
            continue;
        }

        if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (m_bSubDir == false) {
                continue;
            }
            this->doFindInDir(static_cast<const char_t *>(szPath));
        }
        else {
            strFilename = wxString(static_cast<const char_t *>(szPath));

            if (doFindInFile(strFilename) == false) {
                m_bStop = true;
                break;
            }
        }
    } while (FindNextFile(hFile, &fdFile));

    FindClose(hFile);
    hFile = INVALID_HANDLE_VALUE;
}

#else

void FindThread::doFindInDir(const char *pszDir)
{
    DIR *dirT;
    char szPath[PATH_MAXLEN];
    int iLen;
    struct dirent *entryT;
    struct dirent entryR;

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());

    if ((NULL == pFrame) || m_bStop) {
        return;
    }

    FindDirDlg *pDlg = pFrame->getFindDirDlg();
    if (pDlg == NULL) {
        return;
    }

    dirT = opendir(pszDir);

    if (!dirT) {
        m_bStop = true;
        return;
    }

    wxString strFilename;

    while (true) {

        int ires = readdir_r(dirT, &entryR, &entryT);
        if ((ires != 0) || (entryT == NULL)) {
            break;
        }

        if (pDlg->m_pThreadMutex->TryLock() == wxMUTEX_NO_ERROR) {
            m_bStop = pDlg->m_bThreadStop;
            pDlg->m_pThreadMutex->Unlock();
        }

        if (m_bStop) {
            break;
        }

        const char *entryname;

        entryname = entryT->d_name;

        if ((strcmp(entryname, "..") == 0) || (strcmp(entryname, ".") == 0)) {
            continue;
        }

        iLen = snprintf(szPath, PATH_MAXLEN - 1, "%s/%s", pszDir, entryname);
        if (iLen >= PATH_MAXLEN) {
            continue;
        }

        if (entryT->d_type & DT_DIR) {

            if (m_bSubDir == false) {
                continue;
            }

            this->doFindInDir(szPath);
        }
        else if (entryT->d_type & DT_REG) {

            if (access(szPath, F_OK) != 0) {
                continue;
            }

            strFilename = LM_U8TOWC(szPath);

            if (doFindInFile(strFilename) == false) {
                m_bStop = true;
                break;
            }
        }
    }

    closedir(dirT);
}

#endif

wxThreadError FindThread::Create(const wxString &strFind, bool bReplace, const wxString &strReplace,
                                 const wxString &strType, const wxString &strDirf, bool bSubDir, bool bWord, bool bCase)
{
    m_strFind = strFind;
    m_strReplace = strReplace;
    m_bReplace = bReplace;
    m_strType = strType;
    m_strDirf = strDirf;
    m_bSubDir = bSubDir;
    m_bWord = bWord;
    m_bCase = bCase;

    m_iFind = 0;
    m_iReplace = 0;
    m_iFileCount = 0;
    m_iFileTotal = 0;

    m_iDirLen = (int)(m_strDirf.Length());

#if defined(__WXMSW__)
    if (m_strDirf.EndsWith(uT("\\"))) {
        m_strDirf.RemoveLast();
    }
#else
    if (m_strDirf.EndsWith(uT("/"))) {
        m_strDirf.RemoveLast();
    }
#endif

    m_iCount = 0;

    m_pszFind = NULL;
    Tstrncpy(m_szFind, LM_CSTR(m_strFind), FIND_MAXLENGTH - 1);
    m_iFindLen = (int)(Tstrlen(static_cast<const char_t *>(m_szFind)));
    if ((m_iFindLen > 2) && (m_bCase == false)) {
        for (int ll = 0; ll < m_iFindLen; ll++) {
            m_szFindCase[ll] = (char_t)wxTolower((int)(m_szFind[ll]));
        }
        m_szFindCase[m_iFindLen] = uT('\0');
        m_pszFind = (char_t *)(m_szFindCase);
    }
    else {
        m_pszFind = (char_t *)(m_szFind);
    }

    if (m_bReplace) {
        if (m_strReplace.IsSameAs(m_strFind)) {
            m_bReplace = false;
        }
        else {
            Tstrncpy(m_szReplace, LM_CSTR(m_strReplace), FIND_MAXLENGTH - 1);
            m_iReplaceLen = (int)(Tstrlen(static_cast<const char_t *>(m_szReplace)));
        }
    }

    m_bStop = false;

    return wxThread::Create();
}

wxThread::ExitCode FindThread::Entry()
{
    int iLen = (int)(m_strFind.Length());
    if ((iLen < 2) || (iLen > FIND_MAXLENGTH)) {
        return 0;
    }
    iLen = (int)(m_strType.Length());
    if ((iLen < 3) || (iLen > FIND_MAXLENGTH)) {
        return 0;
    }
    iLen = (int)(m_strDirf.Length());
    if ((iLen < 2) || (iLen > LM_STRSIZE) || (::wxDirExists(m_strDirf) == false)) {
        return 0;
    }

    m_bStop = false;

#ifdef WIN32
    this->doFindInDir(LM_CSTR(m_strDirf));
#else
    this->doFindInDir(LM_U8STR(m_strDirf));
#endif

    return 0;
}

void FindThread::OnExit()
{
    // Send the last message
    wxString strBuffer = wxString::Format(uT("Finished[|]%d[|]%d[|]%d[|]%d"), m_iFind, m_iReplace, m_iFileCount, m_iFileTotal);
    int iBufferLen = (int)(strBuffer.Length());
    updateBuffer(strBuffer, iBufferLen);
}
