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

#include <wx/filefn.h>
#include <wx/html/htmlwin.h>
#include <wx/utils.h>
#include <wx/mimetype.h>
#include <wx/clipbrd.h>
#include <wx/busyinfo.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <direct.h>
#endif

#ifndef __WXMSW__
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef __WXGTK__
#include <locale.h>
#endif

// IO
extern int luaClear(lua_State *pLua);
extern int luaRead(lua_State *pLua);
extern int luaWrite(lua_State *pLua);
extern int luaPrint(lua_State *pLua);
//

static const luaL_Reg CONSOLE_CFUNCTION_IO[] = {
    // IO
    { "clear", luaClear },
    { "cls", luaClear },
    { "io_write", luaWrite },
    { "print", luaPrint },
    { NULL, NULL }
};

static void consoleShowHelp(void)
{
#ifdef __WXMSW__
    Tprintf(uT("\n"));
#endif
    Tprintf(uT("\nComet <Programming Environment for Lua>"));
    Tprintf(uT("\nCopyright(C) 2010-2022 Pr. Sidi HAMADY"));
    Tprintf(uT("\nhttp://www.hamady.org"));
    Tprintf(uT("\nUsage: comet -run infile [-out outfile] [-show]\n"));
    Tprintf(uT("\n       comet -compile infile [-out outfile]\n"));
    lf_println();
}

static int consoleRunScript(const char *pszFilename, bool bAction, const char *pszOutputFilename)
{
    if (pszFilename == NULL) {
        consoleShowHelp();
        return EXIT_FAILURE;
    }

    // run expression
    if (lf_fileExists(pszFilename) == false) {

#ifdef __WXMSW__
        try {
            lmSocket::startup();
        }
        catch (...) {
            // nothing
        }
#endif

        LuaEngine *pEngine = new (std::nothrow) LuaEngine(LUA_ENGINE_CONSOLE, CONSOLE_CFUNCTION_IO, true);
        if (pEngine == NULL) {
            printf("\n! Cannot create Lua state: insufficient memory\n");
            return EXIT_FAILURE;
        }

        bool bRet = pEngine->runScriptString(pszFilename, bAction);

        delete pEngine;
        pEngine = NULL;

#ifdef __WXMSW__
        lmSocket::shutdown();
#endif

        printf("\n");

        return (bRet) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    //

    // run file

#ifdef WIN32
    const int nBufSize = LM_STRSIZEW;
#else
    const int nBufSize = PATH_MAX;
#endif

    int iLen = (int)(strlen(pszFilename));
    if ((iLen < 5) || (iLen >= (nBufSize - 5))) {
        printf("\n! Cannot run script: invalid file\n");
        return EXIT_FAILURE;
    }

    char szFilename[nBufSize];
    szFilename[nBufSize - 1] = '\0';

    char szCurDir[nBufSize];
    szCurDir[nBufSize - 1] = '\0';

    if (getcwd(szCurDir, nBufSize * sizeof(char)) == NULL) {
        printf("\n! Cannot run script: invalid working directory\n");
        return EXIT_FAILURE;
    }
    int iLenDir = (int)(strlen((const char *)(szCurDir)));
    if ((iLenDir < 2) || (iLenDir > (nBufSize - iLen - 1))) {
        printf("\n! Cannot run script: invalid working directory\n");
        return EXIT_FAILURE;
    }
    if ((szCurDir[iLenDir - 1] != '/') && (szCurDir[iLenDir - 1] != '\\')) {
#ifdef WIN32
        szCurDir[iLenDir] = '\\';
#else
        szCurDir[iLenDir] = '/';
#endif
        szCurDir[iLenDir + 1] = '\0';
    }
    if (lf_dirExists((const char *)(szCurDir)) == 0) {
        printf("! Cannot run script: invalid working directory");
        lf_println();
        return EXIT_FAILURE;
    }

    if (lf_fileExists(pszFilename) == 0) {

        // Absolute path?
        if ((pszFilename[0] == '/') || (pszFilename[1] == ':')) {
            printf("! Cannot run script: invalid file");
            lf_println();
            return EXIT_FAILURE;
        }

        strcpy(szFilename, (const char *)(szCurDir));
        strcat(szFilename, (const char *)(pszFilename));
        if (lf_fileExists((const char *)(szFilename)) == 0) {
            printf("! Cannot run script: invalid file");
            lf_println();
            return EXIT_FAILURE;
        }
    }
    else {

        char *pszT = lf_realpath((const char *)(pszFilename), NULL);
        if (pszT == NULL) {
            printf("! Cannot run script: invalid file");
            lf_println();
            return EXIT_FAILURE;
        }
        strcpy(szFilename, (const char *)(pszT));
        free(pszT);
        pszT = NULL;
    }

    iLen = (int)(strlen((const char *)(szFilename)));
    if ((iLen < 5) || (iLen >= (nBufSize - 5))) {
        printf("! Cannot run script: invalid file");
        lf_println();
        return EXIT_FAILURE;
    }

#ifdef __WXMSW__
    try {
        lmSocket::startup();
    }
    catch (...) {
    }
#endif

    LuaEngine *pEngine = new (std::nothrow) LuaEngine(LUA_ENGINE_CONSOLE, CONSOLE_CFUNCTION_IO, true);
    if (pEngine == NULL) {
        printf("! Cannot create Lua state: insufficient memory");
        lf_println();
        return EXIT_FAILURE;
    }

    bool bRet = pEngine->runScriptFile(szFilename, iLen, (const char *)(szCurDir), bAction, pszOutputFilename);

    delete pEngine;
    pEngine = NULL;

#ifdef __WXMSW__
    lmSocket::shutdown();
#endif

    //lf_println();

    return (bRet) ? EXIT_SUCCESS : EXIT_FAILURE;
}

IMPLEMENT_APP(CometApp)

wxString CometApp::APPDIR = wxEmptyString;
wxString CometApp::BINDIR = wxEmptyString;
wxString CometApp::INCDIR = wxEmptyString;
wxString CometApp::LIBDIR = wxEmptyString;
wxString CometApp::DOCDIR = wxEmptyString;
wxString CometApp::EXPDIR = wxEmptyString;
wxString CometApp::USRDIR = wxEmptyString;
wxString CometApp::TPLDIR = wxEmptyString;
wxString CometApp::CNFDIR = wxEmptyString;

bool CometApp::INITIALIZED = false;
bool CometApp::COMETCONSOLE = false;

#ifdef __WXMSW__
bool CometApp::FIRST_INSTANCE = true;
const WXUINT CometApp::BROADCAST_MSG_ID = ::RegisterWindowMessage(LM_CSTR(wxString::Format(uT("comet_cmdline_%s.tmp"), LM_CSTR(::wxGetUserId()))));
const wxString CometApp::BROADCAST_MSG_NAME = wxString::Format(uT("comet_cmdline_%s.tmp"), LM_CSTR(::wxGetUserId()));
#endif

CometApp::CometApp() : wxApp()
{
    reset();
}

void CometApp::reset(void)
{
    m_pInstance = NULL;
    m_pMainFrame = NULL;
    m_pSigma = NULL;

#ifdef __WXMSW__
    CometApp::FIRST_INSTANCE = true;
#endif

    m_strStartScript = wxEmptyString;

    CometApp::APPDIR = wxEmptyString;
    CometApp::BINDIR = wxEmptyString;
    CometApp::INCDIR = wxEmptyString;
    CometApp::LIBDIR = wxEmptyString;
    CometApp::DOCDIR = wxEmptyString;
    CometApp::EXPDIR = wxEmptyString;
    CometApp::USRDIR = wxEmptyString;
    CometApp::CNFDIR = wxEmptyString;

    Tmemset(m_szHelpFilename, 0, LM_STRSIZE);

    m_fpStdout = NULL;
    m_fpStderr = NULL;
}

#ifdef __WXGTK__

#ifndef aT
#define aT(s) s
#endif
#ifndef achar_t
#define achar_t char
#endif

static void createDesktopEntry(const achar_t *pszName, const achar_t *pszNameLowerCase, const achar_t *pszDescription)
{
    if ((NULL == pszName) || (aT('\0') == *pszName) || (NULL == pszNameLowerCase) || (aT('\0') == *pszNameLowerCase) || (NULL == pszDescription) || (aT('\0') == *pszDescription)) {
        return;
    }

    size_t iLenA = 0;

    achar_t szDir[LM_STRSIZE];
    memset(szDir, 0, LM_STRSIZE);
    readlink(aT("/proc/self/exe"), szDir, LM_STRSIZE - 1);
    iLenA = strlen((const achar_t *)(szDir));
    if ((iLenA < 6) || (iLenA > (LM_STRSIZE - 2))) {
        return;
    }
    achar_t *pszF = (achar_t *)strstr((const achar_t *)(szDir), aT("/bin/"));
    if (NULL == pszF) {
        return;
    }
    *pszF = aT('\0');
    iLenA = strlen(szDir);
    if ((iLenA < 1) || (iLenA >= LM_STRSIZE)) {
        return;
    }

    size_t iLenN = strlen(pszName);
    if ((iLenN < 1) || (iLenN >= LM_STRSIZEN)) {
        return;
    }
    size_t iLenNL = strlen(pszNameLowerCase);
    if ((iLenNL < 1) || (iLenNL >= LM_STRSIZEN)) {
        return;
    }
    size_t iLenD = strlen(pszDescription);
    if ((iLenD < 1) || (iLenD >= LM_STRSIZEN)) {
        return;
    }

    achar_t szT[LM_STRSIZEC];
    memset(szT, 0, LM_STRSIZEC);
    snprintf(szT, LM_STRSIZEC - 1,
             aT("rm -f ~/.local/share/applications/%s.desktop"),
             pszNameLowerCase);
    int iret = system((const achar_t *)szT);
    if (0 != iret) {
        return;
    }

    snprintf(szT, LM_STRSIZEC - 1,
             aT("echo \"#!/usr/bin/env xdg-open  \" > ~/.local/share/applications/%s.desktop"),
             pszNameLowerCase);
    iret = system((const achar_t *)szT);
    if (0 != iret) {
        return;
    }

    snprintf(szT, LM_STRSIZEC - 1,
             aT("echo \"[Desktop Entry]\" >> ~/.local/share/applications/%s.desktop"),
             pszNameLowerCase);
    iret = system((const achar_t *)szT);
    if (0 != iret) {
        return;
    }

    snprintf(szT, LM_STRSIZEC - 1,
             aT("echo \"Name=%s\" >> ~/.local/share/applications/%s.desktop"),
             pszName, pszNameLowerCase);
    iret = system((const achar_t *)szT);
    if (0 != iret) {
        return;
    }

    snprintf(szT, LM_STRSIZEC - 1,
             aT("echo \"Comment=%s\" >> ~/.local/share/applications/%s.desktop"),
             pszDescription, pszNameLowerCase);
    iret = system((const achar_t *)szT);
    if (0 != iret) {
        return;
    }

    snprintf(szT, LM_STRSIZEC - 1,
             aT("echo \"Exec=%s/bin/%s %%F\" >> ~/.local/share/applications/%s.desktop"),
             (const achar_t *)szDir, pszNameLowerCase, pszNameLowerCase);
    iret = system((const achar_t *)szT);
    if (0 != iret) {
        return;
    }

    snprintf(szT, LM_STRSIZEC - 1,
             aT("echo \"StartupNotify=true\" >> ~/.local/share/applications/%s.desktop"),
             pszNameLowerCase);
    iret = system((const achar_t *)szT);
    if (0 != iret) {
        return;
    }

    snprintf(szT, LM_STRSIZEC - 1,
             aT("echo \"Terminal=false\" >> ~/.local/share/applications/%s.desktop"),
             pszNameLowerCase);
    iret = system((const achar_t *)szT);
    if (0 != iret) {
        return;
    }

    snprintf(szT, LM_STRSIZEC - 1,
             aT("echo \"Type=Application\" >> ~/.local/share/applications/%s.desktop"),
             pszNameLowerCase);
    iret = system((const achar_t *)szT);
    if (0 != iret) {
        return;
    }

    snprintf(szT, LM_STRSIZEC - 1,
             aT("echo \"Icon=%s/icons/%s.png\" >> ~/.local/share/applications/%s.desktop"),
             (const achar_t *)szDir, pszNameLowerCase, pszNameLowerCase);
    iret = system((const achar_t *)szT);
    if (0 != iret) {
        return;
    }

    snprintf(szT, LM_STRSIZEC - 1,
             aT("echo \"Categories=Science;Education;Development\" >> ~/.local/share/applications/%s.desktop"),
             pszNameLowerCase);
    iret = system((const achar_t *)szT);
    if (0 != iret) {
        return;
    }
}

#endif

bool CometApp::OnInit()
{
    CometApp::INITIALIZED = false;

    reset();

    if (!wxApp::OnInit()) {
        return false;
    }

#ifdef __WXGTK__
    setlocale(LC_ALL, "");
#endif

    // using setvbuf is necessary when the stdout is redirected and handled by another process
    setvbuf(stdout, NULL, _IONBF, 0);

#ifdef __WXGTK__
    createDesktopEntry("Comet", "comet", "Programming Environment for Lua");
#endif

    bool bShow = true;

    CometApp::COMETCONSOLE = false;

    SigmaCmdLine cmdLine;
    const int iArgc = cmdLine.getArgCount();

    if (cmdLine.isOK() && (iArgc == 2)) {
        const char_t *pszArg1 = cmdLine.getArg(1);
        if ((pszArg1 != NULL) && ((Tstricmp(pszArg1, uT("-help")) == 0) || (Tstricmp(pszArg1, uT("-h")) == 0))) {
#ifdef __WXMSW__
            // under Windows, enable std IO
            win32EnableConsole();
            //
#endif
            consoleShowHelp();
#ifdef __WXMSW__
            win32SendEnterKey();
#endif
            return false;
        }

        const char_t *pszFilename = cmdLine.getArg(1);
        if (pszFilename) {
            m_strStartScript = pszFilename;
        }
    }

    const char_t *pszRun = cmdLine.getArg(1);
    const char_t *pszInputFilename = cmdLine.getArg(2);

    bool bCompile = false;
    bool bRun = false;
    if ((iArgc >= 3) && (pszRun != NULL) && (pszInputFilename != NULL)) {
        bCompile = (Tstricmp(pszRun, uT("-compile")) == 0);
        bRun = (Tstricmp(pszRun, uT("-run")) == 0);
    }

    if (cmdLine.isOK() && (bCompile || bRun)) {

        CometApp::COMETCONSOLE = true;

        bool bOut = false;
        bShow = false;

        char_t szOutputFilename[LM_STRSIZE];
        szOutputFilename[0] = szOutputFilename[LM_STRSIZE - 1] = uT('\0');

        // execute expression or file
        if (iArgc >= 4) {
            bool bOutX = false;
            for (int ii = 3; ii < iArgc; ii++) {
                const char_t *pszT = cmdLine.getArg(ii);
                if (bShow == false) {
                    bShow = (pszT != NULL) && (Tstricmp(pszT, uT("-show")) == 0);
                }
                if (bOut == false) {
                    bOut = (pszT != NULL) && (Tstricmp(pszT, uT("-out")) == 0);
                    bOutX = bOut;
                    if (bOut) {
                        bOut = false;
                        if (ii < (iArgc - 1)) {
                            const char_t *pszTT = cmdLine.getArg(ii + 1);
                            Tstrncpy(szOutputFilename, pszTT, LM_STRSIZE - 1);
#ifdef __WXMSW__
                            // under Windows, enable std IO
                            win32EnableConsole();
                            //
#endif
                            bOut = lf_redirectConsole(pszTT, &m_fpStdout, &m_fpStderr);
                        }
                    }
                }
                if (bShow && bOutX) {
                    break;
                }
            }

            if ((bShow == false) && (bOut == false)) {
#ifdef __WXMSW__
                // under Windows, enable std IO
                win32EnableConsole();
                //
#endif
                consoleShowHelp();
#ifdef __WXMSW__
                win32SendEnterKey();
#endif
                return false;
            }
        }
        //

#ifdef __WXMSW__
        // under Windows, enable std IO
        if (bOut == false) {
            win32EnableConsole();
        }
        //
#endif

        // Convert from Unicode to ANSI
        char szFilenameA[LM_STRSIZE];
        szFilenameA[0] = szFilenameA[LM_STRSIZE - 1] = '\0';
        if (*pszInputFilename != uT('\0')) {
            wxString strT(pszInputFilename);
            strncpy(szFilenameA, LM_U8STR(strT), LM_STRSIZE - 1);
        }
        //

        if (szFilenameA[0] == '\0') {
            consoleShowHelp();
#ifdef __WXMSW__
            win32SendEnterKey();
#endif
            return false;
        }

#ifdef __WXMSW__
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        char szCT[LM_STRSIZE];
        szCT[0] = szCT[LM_STRSIZE - 1] = '\0';
        strcpy(szCT, "Comet - ");
        strncat(szCT, static_cast<const char *>(szFilenameA), LM_STRSIZE - LM_STRSIZET - 1);
        SetConsoleTitleA(static_cast<const char *>(szCT));
#endif

        char szOutputFilenameA[LM_STRSIZE];
        szOutputFilenameA[0] = szOutputFilenameA[LM_STRSIZE - 1] = '\0';
        if (szOutputFilename[0] != uT('\0')) {
            wcstombs(szOutputFilenameA, szOutputFilename, LM_STRSIZE - 1);
        }

        if (bRun) {
            consoleRunScript(static_cast<const char *>(szFilenameA), LUA_ENGINE_RUN, static_cast<const char *>(szOutputFilenameA));
        }
        else if (bCompile) {
            consoleRunScript(static_cast<const char *>(szFilenameA), LUA_ENGINE_COMPILE, static_cast<const char *>(szOutputFilenameA));
        }
    }

    if (bShow == false) {
#ifdef __WXMSW__
        if (bRun) {
            win32SendEnterKey();
        }
#endif

        // exit
        return false;
    }
    //

    CometApp::COMETCONSOLE = false;

#ifdef __WXMSW__
    m_pInstance = new (std::nothrow) wxSingleInstanceChecker();
    if (m_pInstance) {
        if (m_pInstance->Create(CometApp::BROADCAST_MSG_NAME)) {
            if (m_pInstance->IsAnotherRunning()) {
                delete m_pInstance;
                m_pInstance = NULL;
                CometApp::FIRST_INSTANCE = false;
                if (m_strStartScript.IsEmpty() == false) {
                    bool bNewWindow = false;
                    const char_t *pszNewWindow = cmdLine.getArg(2);
                    if (pszNewWindow) {
                        bNewWindow = Tstricmp(pszNewWindow, uT("--new")) == 0;
                    }
                    if (bNewWindow == false) {
                        SigmaTempFile *pFile = new (std::nothrow) SigmaTempFile(LM_CSTR(CometApp::BROADCAST_MSG_NAME), true, false);
                        if (pFile && pFile->ok()) {
                            pFile->write(m_strStartScript, (int)(m_strStartScript.Length()));
                            delete pFile;
                            pFile = NULL;
                            ::SendMessage(HWND_BROADCAST, CometApp::BROADCAST_MSG_ID, 0, 0);
                            return false;
                        }
                        if (pFile) {
                            delete pFile;
                            pFile = NULL;
                        }
                    }
                }
            }
        }
        else {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }

    try {
        lmSocket::startup();
    }
    catch (...) {
    }
#endif

    // call GetAppDir before GetUserDir
    GetAppDir();
    GetUserDir();

    m_pSigma = new (std::nothrow) lmSigma();
    if (m_pSigma == NULL) {
        if (m_pSigma != NULL) {
            delete m_pSigma;
            m_pSigma = (lmSigma *)NULL;
        }
        SigmaMessageBox(uT("Cannot start Comet: insufficient memory"), uT("Comet"), wxICON_ERROR | wxOK, NULL);
        return false;
    }

    m_pMainFrame = new CometFrame(MAINFRAME_SIZEW, MAINFRAME_SIZEH);

    // initInterface takes about one second to terminate...
    // ... nothing can be done, its wxWidgets
    m_pMainFrame->initInterface();

    m_pMainFrame->loadScript(m_strStartScript);
    m_pMainFrame->restoreConsoleHistory();
    m_pMainFrame->initConsolePrefs();

    m_pMainFrame->initOutputPrefs();
    m_pMainFrame->initToolXPrefs();
    m_pMainFrame->reopenRecent();
    m_pMainFrame->updateCodeSample();

    CometApp::INITIALIZED = true;

    SigmaGUIPrefs *SigmaGUIPrefs = m_pMainFrame->getSigmaGUIPrefs();
    if (SigmaGUIPrefs && SigmaGUIPrefs->bFrameMaximized) {
        int iWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X) - 80;
        int iHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) - 80;
        if ((iWidth >= 800) && (iHeight >= 600)) {
            m_pMainFrame->SetSize(iWidth, iHeight);
        }
    }

    SetTopWindow(m_pMainFrame);
    m_pMainFrame->Show();

    // selectRecent should be called after Show() to make the active page visible
    m_pMainFrame->selectRecent();

    if (m_pMainFrame->isStatusbarEmpty()) {
        wxString strT = uT("Comet <Programming Environment for Lua>");
        m_pMainFrame->OutputStatusbar(strT, SIGMAFRAME_TIMER_SHORT);
    }

    m_pMainFrame->initDrop();

    return true;
}

bool CometApp::isExecutable(const wxString &strFilename, const wxString &strExt)
{
    if (strExt.IsEmpty() == false) {
        const wxString strExeExt = uT("*.exe;*.dll;*.so;*.lib;*.exp;*.a;*.obj;*.o;*.zip;*.rar;*.tar;*.gz;*.tgz;*.com;*.cmd;*.msi;*.out;*.bin;*.scr;*.cpl;*.msc;*.msp;*.mst;*.paf;*.pif;*.pdf;");
        wxString strExtT = uT("*.");
        strExtT += strExt;
        strExtT += uT(";");
        if (strExeExt.Contains(strExtT)) {
            return true;
        }

        if (strExt.IsSameAs(uT("sh")) || strExt.IsSameAs(uT("bat")) || strExt.IsSameAs(uT("cmd")) || strExt.IsSameAs(uT("txt")) || strExt.StartsWith(uT("comet"))) {
            return false;
        }
    }

    bool bIsExe = wxIsExecutable(strFilename);
    if (bIsExe == false) {
        return false;
    }

#ifdef WIN32

    SHFILEINFO shFi;
    DWORD dwExeType = (DWORD)::SHGetFileInfo(LM_CSTR(strFilename), NULL, &shFi, sizeof(shFi), SHGFI_EXETYPE);
    return (LOWORD(dwExeType) != 0);

#else

    return false;

#endif
}

void CometApp::Output(const wxString &strT, bool bAddTime /* = false*/, bool bShowMessage /* = false*/, wxWindow *pParent /* = NULL*/)
{
    m_pMainFrame->Output(strT, bAddTime, bShowMessage, pParent, false);
}

int CometApp::OnExit()
{
    // OnExit() not called if we return false

    if (m_pSigma != NULL) {
        delete m_pSigma;
        m_pSigma = NULL;
    }

#ifdef __WXMSW__
    lmSocket::shutdown();
#endif

    if (m_pInstance != NULL) {
        delete m_pInstance;
        m_pInstance = NULL;
    }

    if (m_fpStdout != NULL) {
        fclose(m_fpStdout);
        m_fpStdout = NULL;
    }
    if (m_fpStderr != NULL) {
        fclose(m_fpStderr);
        m_fpStderr = NULL;
    }

    return wxApp::OnExit();
}

void CometApp::displayHelp(void)
{
    if (::wxFileExists((const char_t *) m_szHelpFilename) == false) {
        SigmaMessageBox(uT("Cannot display help file: PDF associated application not found."), uT("Comet"), wxICON_WARNING | wxOK, NULL);
        return;
    }
#if defined(__WXMSW__)
    wxString strDir = uT("explorer.exe \"");
#else
    wxString strDir = uT("xdg-open \"");
#endif
    strDir += (const char_t *) m_szHelpFilename;
    strDir += uT("\"");
    long iRet = ::wxExecute((const char_t *) (strDir.c_str()), wxEXEC_ASYNC, NULL);
    if (iRet <= 0L) {
        SigmaMessageBox(uT("Cannot display help file. It is located in Comet/doc."), uT("Comet"), wxICON_WARNING | wxOK, NULL);
    }
}

bool CometApp::GetUserDir(void)
{
    int ii, iSize = LM_STRSIZE - 1;

    char_t szT[LM_STRSIZE];
    Tmemset(szT, 0, LM_STRSIZE);

    bool bFlag = false;

    wxString strDir = wxStandardPaths::Get().GetUserLocalDataDir();
    iSize = (int)(strDir.Length());
    Tstrncpy(szT, LM_CSTR(strDir), LM_STRSIZE - 1);
    if ((iSize > 6) && (iSize < (LM_STRSIZE - 30))) {
        for (ii = iSize - 1; ii > 0; ii--) {
            if ((szT[ii] == uT('\\')) || (szT[ii] == uT('/'))) { // Windows or Unix
                szT[ii + 1] = uT('\0');
                bFlag = true;
                break;
            }
        }
#if defined(__WXMSW__)
        if (bFlag) {
            Tstrcat(szT, uT("Comet\\"));
        }
        else {
            Tstrcat(szT, uT("\\Comet\\"));
        }
#else
        if (bFlag) {
            Tstrcat(szT, uT(".Comet/"));
        }
        else {
            Tstrcat(szT, uT("/.Comet/"));
        }
#endif
        if (::wxDirExists(szT) == false) {
            wxString strDir(static_cast<const char_t *>(szT));
            ::wxMkdir(strDir);
        }
        CometApp::USRDIR = wxString(static_cast<const char_t *>(szT));
        return true;
    }

    return false;
}

void CometApp::DoGetAppDir(const char_t *pzDir)
{
    CometApp::APPDIR = wxString(pzDir);
    CometApp::BINDIR = CometApp::APPDIR;
    CometApp::BINDIR += uT("bin");
    CometApp::BINDIR += wxFILE_SEP_PATH;
    CometApp::INCDIR = CometApp::APPDIR;
    CometApp::INCDIR += uT("include");
    CometApp::INCDIR += wxFILE_SEP_PATH;
    CometApp::LIBDIR = CometApp::APPDIR;
    CometApp::LIBDIR += uT("lib");
    CometApp::LIBDIR += wxFILE_SEP_PATH;
    CometApp::DOCDIR = CometApp::APPDIR;
    CometApp::DOCDIR += uT("doc");
    CometApp::DOCDIR += wxFILE_SEP_PATH;
    Tstrcpy(m_szHelpFilename, LM_CSTR(CometApp::DOCDIR));
    Tstrcat(m_szHelpFilename, uT("comet.pdf"));
    CometApp::EXPDIR = CometApp::APPDIR;
    CometApp::EXPDIR += uT("examples");
    CometApp::EXPDIR += wxFILE_SEP_PATH;
    CometApp::TPLDIR = CometApp::APPDIR;
    CometApp::TPLDIR += uT("templates");
    CometApp::TPLDIR += wxFILE_SEP_PATH;
    CometApp::CNFDIR = CometApp::APPDIR;
    CometApp::CNFDIR += uT("config");
    CometApp::CNFDIR += wxFILE_SEP_PATH;
}

bool CometApp::GetAppDir(void)
{
    int iSize = LM_STRSIZE - 1;

    char_t szT[LM_STRSIZE];
    Tmemset(szT, 0, LM_STRSIZE);

    wxString strDir = wxStandardPaths::Get().GetExecutablePath();
    iSize = (int)(strDir.Length());

    if ((iSize > 6) && (iSize < (LM_STRSIZE - 30))) {
        Tstrncpy(szT, LM_CSTR(strDir), LM_STRSIZE - 1);
        for (int ii = iSize - 1; ii > 0; ii--) {
            if ((szT[ii] == uT('\\')) || (szT[ii] == uT('/'))) { // Windows or Unix
                szT[ii + 1] = uT('\0');
                break;
            }
        }
    }
    else {
        return false;
    }

#ifdef __WXMSW__
    int iF = (int)(strDir.rfind(uT("\\bin\\")));
#else
    int iF = (int)(strDir.rfind(uT("/bin/")));
#endif
    if ((iF < 2) || (iF > (LM_STRSIZE - 12))) {
        DoGetAppDir(static_cast<const char_t *>(szT));
        return false;
    }
    szT[iF + 1] = uT('\0');

    DoGetAppDir(static_cast<const char_t *>(szT));

    return true;
}

int CometApp::StringToChar(wxString &strSrc, char_t *szDest, int nMax, bool bRemoveEOL /* = false*/)
{
    int ii, nlen = 0;

    if ((nMax < 1) || (nMax >= LM_STRSIZE)) {
        return -1;
    }

    nlen = (int)(strSrc.Length());
    if ((nlen < 0) || (nlen > nMax)) {
        return -1;
    }
    if (nlen == 0) {
        szDest[0] = uT('\0');
        return 0;
    }
    if (nlen == 1) {
        szDest[0] = (char_t)strSrc[0];
        szDest[1] = uT('\0');
        return 1;
    }

    int ilen = nlen;
    for (ii = 0; ii < nlen; ii++) {
        szDest[ii] = (char_t)strSrc[ii];
        if (bRemoveEOL && lf_isEOL(szDest[ii])) {
            ilen = ii;
            break;
        }
    }
    szDest[ilen] = uT('\0');

    return ilen;
}

int CometApp::StringReverseFind(wxString &strT, const char_t *szFind)
{
    const char_t *pszBuffer = LM_CSTR(strT);
    if (pszBuffer == NULL) {
        return -1;
    }

    char_t *pszNext = NULL, *pszBufferT, *pszBuffer0;
    int iLen = (int)Tstrlen(pszBuffer), iLenFind = (int)Tstrlen(szFind);
    int iPos = -1;

    if ((iLen < 2) || (iLen > (LM_STRSIZEL * LM_STRSIZET)) || (iLenFind < 1) || (iLenFind > LM_STRSIZE)) {
        return -1;
    }

    pszBuffer0 = (char_t *)pszBuffer;
    pszBufferT = (char_t *)pszBuffer;
    pszNext = (char_t *)Tstrstr((const char_t *)pszBufferT, szFind);
    while (pszNext) {
        iPos = pszNext - pszBuffer0;
        pszBufferT = pszNext + iLenFind;
        pszNext = (char_t *)Tstrstr((const char_t *)pszBufferT, szFind);
    }

    return iPos;
}

#ifdef wxID_HIGHEST
#define IDD_MESSAGE_BOX (wxID_HIGHEST + 1 + 9988)
#else
#define IDD_MESSAGE_BOX (6000 + 9988)
#endif

#define IDE_MESSAGE_TEXT   (IDD_MESSAGE_BOX + 1)
#define ID_MESSAGE_BUTTONA (IDD_MESSAGE_BOX + 2)
#define ID_MESSAGE_BUTTONB (IDD_MESSAGE_BOX + 3)
#define ID_MESSAGE_BUTTONC (IDD_MESSAGE_BOX + 4)

#define SigmaMessageDlg_STYLE    (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
#define SigmaMessageDlg_TITLE    uT("Comet")
#define SigmaMessageDlg_SIZE     wxDefaultSize
#define SigmaMessageDlg_POSITION wxDefaultPosition

// SigmaMessageDlg not exported
class SigmaMessageDlg : public wxDialog
{
public:
    SigmaMessageDlg(wxWindow *pParent, const wxPoint &ptInit = SigmaMessageDlg_POSITION, wxWindowID idT = IDD_MESSAGE_BOX,
                    const wxString &strTitle = SigmaMessageDlg_TITLE, const wxSize &sizeT = SigmaMessageDlg_SIZE, long styleT = SigmaMessageDlg_STYLE,
                    long dlgstyleT = wxOK,
                    const wxString &strMessage = uT(""),
                    const wxString &strButtonALabel = uT(""),
                    const wxString &strButtonBLabel = uT(""),
                    const wxString &strButtonCLabel = uT(""));
    virtual ~SigmaMessageDlg();

    void OnButtonA(wxCommandEvent &tEvent);
    void OnButtonB(wxCommandEvent &tEvent);
    void OnButtonC(wxCommandEvent &tEvent);
    void OnCloseWindow(wxCloseEvent &tEvent);

private:
    long m_iStyle;

    DECLARE_EVENT_TABLE()

    wxPoint m_ptInit;
    void initInterface(const wxString &strMessage, const wxString &strButtonALabel, const wxString &strButtonBLabel, const wxString &strButtonCLabel);
    static bool ShowToolTips();
};

BEGIN_EVENT_TABLE(SigmaMessageDlg, wxDialog)
EVT_BUTTON(ID_MESSAGE_BUTTONA, SigmaMessageDlg::OnButtonA)
EVT_BUTTON(ID_MESSAGE_BUTTONB, SigmaMessageDlg::OnButtonB)
EVT_BUTTON(ID_MESSAGE_BUTTONC, SigmaMessageDlg::OnButtonC)
EVT_CLOSE(SigmaMessageDlg::OnCloseWindow)
END_EVENT_TABLE()

SigmaMessageDlg::SigmaMessageDlg(wxWindow *pParent, const wxPoint &ptInit, wxWindowID idT,
                                 const wxString &strTitle, const wxSize &sizeT, long styleT,
                                 long dlgstyleT,
                                 const wxString &strMessage,
                                 const wxString &strButtonALabel,
                                 const wxString &strButtonBLabel,
                                 const wxString &strButtonCLabel)
    : wxDialog(pParent, idT, strTitle, ptInit, sizeT, styleT & ~wxRESIZE_BORDER)
{
    m_iStyle = dlgstyleT;

    m_ptInit = ptInit;

    initInterface(strMessage, strButtonALabel, strButtonBLabel, strButtonCLabel);

    wxSize sizeM = GetSize();
    SetMinSize(sizeM);
}

SigmaMessageDlg::~SigmaMessageDlg()
{
}

void SigmaMessageDlg::initInterface(const wxString &strMessage, const wxString &strButtonALabel, const wxString &strButtonBLabel, const wxString &strButtonCLabel)
{
    SetHelpText(GetTitle());
    if (SigmaMessageDlg::ShowToolTips()) {
        SetToolTip(GetTitle());
    }

    wxBoxSizer *pSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pSizer);

    wxBoxSizer *pSizerMessage = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *pSizerButtons = new wxBoxSizer(wxHORIZONTAL);

    int ix, iy;
    wxSize sizeText(-1, -1);

    this->GetTextExtent(uT("<Do Not Delete>"), &ix, &iy);
    wxSize sizeButton(ix, iy + 10);

    wxString strMessageT = uT(' ');
    strMessageT += strMessage;
    strMessageT.Replace(uT("\n"), uT(" \n "), true);
    strMessageT += uT(' ');

    wxStaticText *txtMessage = new wxStaticText(this, wxID_ANY, strMessageT, wxDefaultPosition, sizeText, wxALIGN_LEFT);

    wxArtID iconT = wxART_INFORMATION;

    if (m_iStyle & wxICON_QUESTION) {
        iconT = wxART_QUESTION;
    }
    else if (m_iStyle & wxICON_WARNING) {
        iconT = wxART_WARNING;
    }
    else if (m_iStyle & wxICON_ERROR) {
        iconT = wxART_ERROR;
    }

    wxBitmap bitmapT = wxArtProvider::GetBitmap(iconT);
    wxSize sizeBitmap(bitmapT.GetWidth() + 8, bitmapT.GetHeight() + 8);

    pSizerMessage->AddSpacer(8);
    pSizerMessage->Add(new wxStaticBitmap(this, wxID_ANY, bitmapT, wxDefaultPosition, sizeBitmap, 0), 0, wxALL, 0);
    pSizerMessage->AddSpacer(8);
    pSizerMessage->Add(txtMessage, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
    pSizerMessage->AddSpacer(8);

    wxButton *pButtonA = NULL, *pButtonB = NULL, *pButtonC = NULL;
    if (m_iStyle & wxYES_NO) {
        if (m_iStyle & wxCANCEL) {
            // Three buttons
            pButtonA = new wxButton(this, ID_MESSAGE_BUTTONA, strButtonALabel.IsEmpty() ? uT("&Yes") : strButtonALabel, wxDefaultPosition, sizeButton, 0);
            pButtonB = new wxButton(this, ID_MESSAGE_BUTTONB, strButtonBLabel.IsEmpty() ? uT("&No") : strButtonBLabel, wxDefaultPosition, sizeButton, 0);
            pButtonC = new wxButton(this, ID_MESSAGE_BUTTONC, strButtonCLabel.IsEmpty() ? uT("&Cancel") : strButtonCLabel, wxDefaultPosition, sizeButton, 0);
        }
        else {
            // Two buttons
            pButtonA = new wxButton(this, ID_MESSAGE_BUTTONA, strButtonALabel.IsEmpty() ? uT("&Yes") : strButtonALabel, wxDefaultPosition, sizeButton, 0);
            pButtonB = new wxButton(this, ID_MESSAGE_BUTTONB, strButtonBLabel.IsEmpty() ? uT("&No") : strButtonBLabel, wxDefaultPosition, sizeButton, 0);
        }

        if (m_iStyle & wxNO_DEFAULT) {
            pButtonB->SetDefault();
            pButtonB->SetFocus();
        }
        else {
            pButtonA->SetDefault();
            pButtonA->SetFocus();
        }
    }
    else {
        // One button
        pButtonA = new wxButton(this, ID_MESSAGE_BUTTONA, strButtonALabel.IsEmpty() ? uT("&OK") : strButtonALabel, wxDefaultPosition, sizeButton, 0);
        pButtonA->SetDefault();
    }

    pSizerButtons->AddSpacer(8);
    pSizerButtons->Add(pButtonA, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
    pSizerButtons->AddSpacer(8);
    if (pButtonB) {
        pSizerButtons->Add(pButtonB, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
        pSizerButtons->AddSpacer(8);
    }
    if (pButtonC) {
        pSizerButtons->Add(pButtonC, 0, wxEXPAND | wxRIGHT | wxLEFT, 0);
        pSizerButtons->AddSpacer(8);
    }

    pSizer->AddSpacer(8);
    pSizer->Add(pSizerMessage, 0, wxALIGN_LEFT | wxLEFT | wxBOTTOM, 0);
    pSizer->AddSpacer(16);
    pSizer->Add(pSizerButtons, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 0);
    pSizer->AddSpacer(8);

    pSizer->Fit(this);

    Move(m_ptInit);
}

void SigmaMessageDlg::OnButtonA(wxCommandEvent &tEvent)
{
    int iret = wxCANCEL;

    if (m_iStyle & wxYES_NO) {
        iret = wxYES;
    }

    EndModal(iret);
}

void SigmaMessageDlg::OnButtonB(wxCommandEvent &tEvent)
{
    int iret = wxCANCEL;

    if (m_iStyle & wxYES_NO) {
        iret = wxNO;
    }

    EndModal(iret);
}

void SigmaMessageDlg::OnButtonC(wxCommandEvent &tEvent)
{
    EndModal(wxCANCEL);
}

void SigmaMessageDlg::OnCloseWindow(wxCloseEvent &tEvent)
{
    EndModal(wxCANCEL);
}

bool SigmaMessageDlg::ShowToolTips()
{
    return false;
}

int SigmaMessageBox(const wxString &strMessage, const wxString &strCaption, long iStyle,
                    wxWindow *pParent,
                    const wxString &strButtonALabel,
                    const wxString &strButtonBLabel,
                    const wxString &strButtonCLabel)
{
    SigmaMessageDlg *pDlg = new (std::nothrow) SigmaMessageDlg(pParent, SigmaMessageDlg_POSITION, IDD_MESSAGE_BOX,
                                                               strCaption, SigmaMessageDlg_SIZE, SigmaMessageDlg_STYLE,
                                                               iStyle, strMessage, strButtonALabel, strButtonBLabel, strButtonCLabel);

    if (pDlg == NULL) {
        return -1;
    }

    int iret = pDlg->ShowModal();
    pDlg->Destroy();

    return iret;
}
