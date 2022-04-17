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
// -------------------------------------------------------------

#define LIBFILE_CORE

#define _CRT_SECURE_NO_DEPRECATE 1

#include "File.h"

#ifdef __cplusplus
extern "C" {
#endif
void SharedLibEntry(void)
{
#ifdef _WIN32
    Tprintf(uT("\nFile Library for Comet version %s build %s (C) 2010-2021  Pr. Sidi HAMADY\nhttp://www.hamady.org\n"), FILE_VERSION_STRING, FILE_BUILD_STRING);
#else
    Tprintf(uT("\nFile Library for Comet version %ls build %ls (C) 2010-2021  Pr. Sidi HAMADY\nhttp://www.hamady.org\n"), FILE_VERSION_STRING, FILE_BUILD_STRING);
#endif
    exit(0);
}
#ifdef __cplusplus
}
#endif

#define UNICODE_REPLACEMENT_CHAR (lmChar)0x0000FFFD
#define UNICODE_MAX_BMP          (lmChar)0x0000FFFF
#define UNICODE_MAX_UTF16        (lmChar)0x0010FFFF
#define UNICODE_MAX_UTF32        (lmChar)0x7FFFFFFF
#define UNICODE_MAX_LEGAL_UTF32  (lmChar)0x0010FFFF
#define UNICODE_SUR_HIGH_START   (lmChar)0xD800
#define UNICODE_SUR_HIGH_END     (lmChar)0xDBFF
#define UNICODE_SUR_LOW_START    (lmChar)0xDC00
#define UNICODE_SUR_LOW_END      (lmChar)0xDFFF

LIBFILE_API bool lf_convertUTF16to32(const char_t *pszSource, lmChar *pTarget, int iLen)
{
    size_t iSizeof = sizeof(char_t);
    if ((iSizeof == sizeof(lmChar)) || (iSizeof != sizeof(lmUShort))) {
        return true;
    }

    if ((iLen < 1) || (iLen > LM_STRSIZEL)) {
        return true;
    }

    char_t *pszSourceT = const_cast<char_t *>(pszSource);
    char_t *pszSourceEnd = const_cast<char_t *>(pszSource + iLen);
    lmChar *pTargetT = pTarget;
    lmChar cT, cTT;
    bool bRet = true;

    const int iHalfShift = 10;
    const lmChar iHalfBase = 0x0010000UL;

    while (pszSourceT < pszSourceEnd) {
        cT = static_cast<lmChar>(*(pszSourceT++));
        if ((cT >= UNICODE_SUR_HIGH_START) && (cT <= UNICODE_SUR_HIGH_END)) {
            if (pszSourceT < pszSourceEnd) {
                cTT = static_cast<lmChar>(*pszSourceT);
                if ((cTT >= UNICODE_SUR_LOW_START) && (cTT <= UNICODE_SUR_LOW_END)) {
                    cT = ((cT - UNICODE_SUR_HIGH_START) << iHalfShift) + (cTT - UNICODE_SUR_LOW_START) + iHalfBase;
                    ++pszSourceT;
                }
                else {
                    bRet = false;
                    break;
                }
            }
            else {
                break;
            }
        }
        else {
            if ((cT >= UNICODE_SUR_LOW_START) && (cT <= UNICODE_SUR_LOW_END)) {
                bRet = false;
                break;
            }
        }
        *(pTargetT++) = cT;
    }

    return bRet;
}

LIBFILE_API bool lf_convertUTF32to16(const lmChar *pSource, char_t *pszTarget, int iLen)
{
    size_t iSizeof = sizeof(char_t);
    if ((iSizeof == sizeof(lmChar)) || (iSizeof != sizeof(lmUShort))) {
        return true;
    }

    if ((iLen < 1) || (iLen > LM_STRSIZEL)) {
        return true;
    }

    lmChar *pSourceT = const_cast<lmChar *>(pSource);
    lmChar *pSourceEnd = const_cast<lmChar *>(pSource + iLen);
    char_t *pszTargetT = pszTarget;
    char_t *pszTargetEnd = static_cast<char_t *>(pszTarget + iLen);
    lmChar cT;

    const int iHalfShift = 10;
    const lmChar iHalfBase = 0x0010000UL;
    const lmChar iHalfMask = 0x3FFUL;

    while (pSourceT < pSourceEnd) {
        cT = *(pSourceT++);
        if (cT <= UNICODE_MAX_BMP) {
            if ((cT >= UNICODE_SUR_HIGH_START) && (cT <= UNICODE_SUR_LOW_END)) {
                break;
            }
            else {
                if (pszTargetT >= pszTargetEnd) {
                    break;
                }
                *(pszTargetT++) = static_cast<char_t>(cT);
            }
        }
        else if (cT <= UNICODE_MAX_LEGAL_UTF32) {
            if ((pszTargetT + 1) >= pszTargetEnd) {
                break;
            }
            cT -= iHalfBase;
            *(pszTargetT++) = static_cast<char_t>((cT >> iHalfShift) + UNICODE_SUR_HIGH_START);
            *(pszTargetT++) = static_cast<char_t>((cT & iHalfMask) + UNICODE_SUR_LOW_START);
        }
        else if (cT > UNICODE_MAX_LEGAL_UTF32) {
            break;
        }
    }

    return true;
}

#ifdef WIN32

#include <windows.h>

LIBFILE_API void win32ClearConsole(void)
{
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;
    HANDLE lStdHandle;
    lStdHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (!::GetConsoleScreenBufferInfo(lStdHandle, &csbi)) {
        return;
    }
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    if (!::FillConsoleOutputCharacter(lStdHandle, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten)) {
        return;
    }
    if (!::GetConsoleScreenBufferInfo(lStdHandle, &csbi)) {
        return;
    }
    if (!::FillConsoleOutputAttribute(lStdHandle, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten)) {
        return;
    }
    ::SetConsoleCursorPosition(lStdHandle, coordScreen);
}

LIBFILE_API void win32EnableConsole(void)
{
    if (::AttachConsole(ATTACH_PARENT_PROCESS) == TRUE) {
        HANDLE lStdHandle;

        lStdHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
        if ((INVALID_HANDLE_VALUE != lStdHandle) && (FILE_TYPE_CHAR == ::GetFileType(lStdHandle))) {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            if (::GetConsoleScreenBufferInfo(lStdHandle, &csbi)) {
                csbi.dwSize.Y = 1024;
                ::SetConsoleScreenBufferSize(lStdHandle, csbi.dwSize);
            }
            lStdHandle = ::CreateFile(uT("CONOUT$"), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
            if (INVALID_HANDLE_VALUE != lStdHandle) {
                ::CloseHandle(lStdHandle);
                freopen("CONOUT$", "w", stdout);
                freopen("CONOUT$", "w", stderr);
                setbuf(stdout, NULL);
            }
        }

        std::cin.clear();
        std::cout.clear();
        std::ios::sync_with_stdio();
    }
}

LIBFILE_API void win32SendEnterKey(void)
{
    // key enter only if console opened
    if (::GetConsoleWindow() == NULL) {
        printf("\n");
        return;
    }

    INPUT inputT;
    inputT.type = INPUT_KEYBOARD;
    inputT.ki.wScan = 0;
    inputT.ki.time = 0;
    inputT.ki.dwExtraInfo = 0;
    inputT.ki.wVk = 0x0D;
    inputT.ki.dwFlags = 0;
    ::SendInput(1, &inputT, sizeof(INPUT));
    inputT.ki.dwFlags = KEYEVENTF_KEYUP;
    ::SendInput(1, &inputT, sizeof(INPUT));
}

#endif

LIBFILE_API bool lf_redirectConsole(const char_t *pszFilename, FILE **fpout, FILE **fperr)
{
    if (pszFilename != NULL) {
        // Convert from Unicode to ANSI
        char szTTA[LM_STRSIZE];
        szTTA[0] = szTTA[LM_STRSIZE - 1] = '\0';
        wcstombs(szTTA, pszFilename, LM_STRSIZE - 1);
        int nLengthA = (int)strlen(static_cast<const char *>(szTTA));
        if (nLengthA >= 1) {
            *fpout = freopen(static_cast<const char *>(szTTA), "w", stdout);
            *fperr = freopen(static_cast<const char *>(szTTA), "w", stderr);
            setbuf(stdout, NULL);
            std::cin.clear();
            std::cout.clear();
            std::ios::sync_with_stdio();
            return true;
        }
        //
    }
    return false;
}

#ifdef WIN32

LIBFILE_API int lf_fileExists(const char *pszFilename)
{
    WIN32_FIND_DATAA FindFileData;
    HANDLE handleT = FindFirstFileA(pszFilename, &FindFileData);
    int ifound = (handleT != INVALID_HANDLE_VALUE) ? 1 : 0;
    if (ifound) {
        FindClose(handleT);
    }
    return ifound;
}
LIBFILE_API int lf_dirExists(const char *pszDirname)
{
    DWORD dwAttrib = GetFileAttributesA(pszDirname);
    return ((dwAttrib != INVALID_FILE_ATTRIBUTES) && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) ? 1 : 0;
}
LIBFILE_API int lf_mkdir(const char *pszDirname)
{
    if (lf_dirExists(pszDirname) == 0) {
        return (::CreateDirectoryA(static_cast<LPCSTR>(pszDirname), NULL) == TRUE) ? 1 : 0;
    }
    return 1;
}
LIBFILE_API bool lf_fileReadonly(const char_t *pszFilename)
{
    DWORD dwAttrs = GetFileAttributes(pszFilename);
    if (dwAttrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    return (dwAttrs & FILE_ATTRIBUTE_READONLY);
}
LIBFILE_API bool lf_fileReadWrite(const char_t *pszFilename)
{
    DWORD dwAttrs = GetFileAttributes(pszFilename);
    if (dwAttrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    if (dwAttrs & FILE_ATTRIBUTE_READONLY) {
        dwAttrs &= ~FILE_ATTRIBUTE_READONLY;
        SetFileAttributes(pszFilename, dwAttrs);
        dwAttrs = GetFileAttributes(pszFilename);
    }

    return ((dwAttrs & FILE_ATTRIBUTE_READONLY) == 0);
}

LIBFILE_API char *lf_realpath(const char *pszRelPath, char *pszFullPath)
{
    char *pszPath = NULL;

    if (pszFullPath == NULL) {
        pszPath = (char *)malloc(LM_STRSIZE * sizeof(char));
        if (pszPath == NULL) {
            return NULL;
        }
        pszPath[LM_STRSIZE - 1] = '\0';
    }
    else {
        pszPath = pszFullPath;
    }

    size_t iLen = ::GetFullPathNameA(pszRelPath, LM_STRSIZE - 1, pszPath, NULL);
    if ((iLen < 1) || (iLen > (LM_STRSIZE - 1))) {
        // free allocated memory
        if (pszPath != pszFullPath) {
            free(pszPath);
            pszPath = NULL;
        }
        return NULL;
    }

    if ((lf_dirExists((const char *)(pszPath)) == 0) && (lf_fileExists((const char *)(pszPath)) == 0)) {
        // free allocated memory
        if (pszPath != pszFullPath) {
            free(pszPath);
            pszPath = NULL;
        }
        return NULL;
    }

    return pszPath;
}

#else

// Linux

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

LIBFILE_API int lf_fileExists(const char *pszFilename)
{
    int fileT = open(pszFilename, O_RDONLY);
    if (fileT < 0) {
        return 0;
    }
    close(fileT);
    return 1;
}

LIBFILE_API int lf_dirExists(const char *pszDirname)
{
    DIR *dirT = opendir(pszDirname);
    if (!dirT) {
        return 0;
    }
    closedir(dirT);
    return 1;
}
LIBFILE_API int lf_mkdir(const char *pszDirname)
{
    if (lf_dirExists(pszDirname) == 0) {
        const int derr = mkdir(pszDirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (-1 == derr) {
            return 0;
        }
    }

    return 1;
}

LIBFILE_API bool lf_fileReadonly(const char_t *pszFilename)
{
    struct stat stbuf;

    int iLen = (int)Tstrlen(pszFilename);
    if ((iLen < 1) || (iLen > PATH_MAX)) {
        return false;
    }

    char *pszT = (char *)malloc((iLen + 1) * sizeof(char));
    if (pszT == NULL) {
        return false;
    }
    pszT[iLen] = '\0';

    int iret = wcstombs(pszT, pszFilename, iLen);
    if (iret != iLen) {
        free(pszT);
        pszT = NULL;
        return false;
    }
    pszT[iret] = '\0';

    stat(pszT, &stbuf);
    free(pszT);
    pszT = NULL;

    return ((stbuf.st_mode & S_IWUSR) == 0);
}
LIBFILE_API bool lf_fileReadWrite(const char_t *pszFilename)
{
    struct stat stbuf;

    int iLen = (int)Tstrlen(pszFilename);
    if ((iLen < 1) || (iLen > PATH_MAX)) {
        return false;
    }

    char *pszT = (char *)malloc((iLen + 1) * sizeof(char));
    if (pszT == NULL) {
        return false;
    }
    pszT[iLen] = '\0';

    int iret = wcstombs(pszT, pszFilename, iLen);
    if (iret != iLen) {
        free(pszT);
        pszT = NULL;
        return false;
    }
    pszT[iret] = '\0';

    stat(pszT, &stbuf);

    if ((stbuf.st_mode & S_IWUSR) == 0) {
        chmod(pszT, stbuf.st_mode | S_IWUSR);
        stat(pszT, &stbuf);
    }

    free(pszT);
    pszT = NULL;

    return (stbuf.st_mode & S_IWUSR);
}

LIBFILE_API char *lf_realpath(const char *pszRelPath, char *pszFullPath)
{
    return realpath(pszRelPath, pszFullPath);
}

#endif

#ifdef WIN32
#include <direct.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

LIBFILE_API bool lf_getCurrentDir(char *pszCurDir)
{
    if (pszCurDir == NULL) {
        return false;
    }

    pszCurDir[0] = pszCurDir[LM_STRSIZE - 1] = '\0';

    if (getcwd(pszCurDir, (LM_STRSIZE - 1) * sizeof(char)) == NULL) {
        return false;
    }
    int iLenDir = (int)(strlen((const char *)(pszCurDir)));
    if ((iLenDir < 2) || (iLenDir > (LM_STRSIZE - 1))) {
        return false;
    }
    if ((pszCurDir[iLenDir - 1] != '/') && (pszCurDir[iLenDir - 1] != '\\')) {
#ifdef WIN32
        pszCurDir[iLenDir] = '\\';
#else
        pszCurDir[iLenDir] = '/';
#endif
        pszCurDir[iLenDir + 1] = '\0';
    }
    if (lf_dirExists((const char *)(pszCurDir)) == 0) {
        return false;
    }

    return true;
}

// check if two files belong to the same directory
LIBFILE_API bool lf_comparePath(const char_t *pszPathA, const char_t *pszPathB, char_t *pszPathAA)
{
    if ((pszPathA == NULL) || (pszPathB == NULL) || (pszPathAA == NULL)) {
        return false;
    }

    const int iLenA = static_cast<const int>(Tstrlen(pszPathA));
    if ((iLenA < 5) || (iLenA >= (LM_STRSIZE - 5))) {
        return false;
    }

    const int iLenB = static_cast<const int>(Tstrlen(pszPathB));
    if ((iLenB < 5) || (iLenB >= (LM_STRSIZE - 5))) {
        return false;
    }

    char_t szFA[LM_STRSIZE];
    szFA[0] = szFA[LM_STRSIZE - 1] = uT('\0');
    char_t szFB[LM_STRSIZE];
    szFB[0] = szFB[LM_STRSIZE - 1] = uT('\0');
    Tstrcpy(szFA, pszPathA);
    Tstrcpy(szFB, pszPathB);

#ifdef WIN32
    char_t *pszFA = Tstrrchr(static_cast<char_t *>(szFA), uT('\\'));
    char_t *pszFB = Tstrrchr(static_cast<char_t *>(szFB), uT('\\'));
#else
    char_t *pszFA = Tstrrchr(static_cast<char_t *>(szFA), uT('/'));
    char_t *pszFB = Tstrrchr(static_cast<char_t *>(szFB), uT('/'));
#endif

    if ((pszFA != NULL) && (pszFB != NULL)) {
        *pszFA = uT('\0');
        *pszFB = uT('\0');
        if (Tstrcmp(static_cast<const char_t *>(szFA), static_cast<const char_t *>(szFB)) == 0) {
            Tstrcpy(pszPathAA, static_cast<const char_t *>(pszFA + 1));
            return true;
        }
    }

    Tstrcpy(pszPathAA, pszPathA);
    return false;
}
