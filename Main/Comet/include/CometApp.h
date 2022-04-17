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


#ifndef SIGMAAPP_H
#define SIGMAAPP_H

#include "../../LibFile/include/File.h"

#include <wx/wxprec.h>
#include <wx/help.h>
#include <wx/generic/helpext.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <wx/stdpaths.h>
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/dcbuffer.h>
#include <wx/colordlg.h>
#include <wx/artprov.h>
#include <wx/combobox.h>
#include <wx/tglbtn.h>
#include <wx/cmdline.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/snglinst.h>

class SigmaTempFile
{
private:
    char_t m_szFilename[LM_STRSIZE];
    FILE *m_pFile;
    bool m_bWrite;
    bool m_bAutoDelete;

public:
    SigmaTempFile(const char_t *pszFilename, bool bWrite, bool bAutoDelete)
    {
        m_pFile = NULL;
        m_bWrite = bWrite;
        m_bAutoDelete = bAutoDelete;

        Tmemset(m_szFilename, 0, LM_STRSIZE);
        wxString strDir = wxStandardPaths::Get().GetTempDir();
        size_t iLen = strDir.Length();
        size_t iLenF = Tstrlen(pszFilename);
        Tstrncpy(m_szFilename, LM_CSTR(strDir), LM_STRSIZE - 1);
        if ((iLen > 3) && (iLen < (LM_STRSIZE - iLenF - 2))) {
            if ((m_szFilename[iLen - 1] != uT('\\')) && (m_szFilename[iLen - 1] != uT('/'))) { // Windows or Unix
#ifdef __WXMSW__
                m_szFilename[iLen] = uT('\\');
#else
                m_szFilename[iLen] = uT('/');
#endif
                m_szFilename[iLen + 1] = uT('\0');
            }
            Tstrcat(m_szFilename, pszFilename);
            m_pFile = Tfopen(static_cast<const char_t *>(m_szFilename), m_bWrite ? uT("w") : uT("r"));
        }
    }
    ~SigmaTempFile()
    {
        if (m_bAutoDelete) {
            this->unlink();
            return;
        }

        if (m_pFile) {
            fclose(m_pFile);
            m_pFile = NULL;
        }
    }

    bool write(const char_t *pszBuffer, int iLen)
    {
        if ((m_bWrite == false) || (m_pFile == NULL)) {
            return false;
        }
        for (int ii = 0; ii < iLen; ii++) {
            Tfputc(pszBuffer[ii], m_pFile);
        }
        return true;
    }
    bool read(char_t *pszBuffer, int iLen)
    {
        if (m_bWrite || (m_pFile == NULL)) {
            return false;
        }
        char_t cT;
        for (int ii = 0; ii < iLen; ii++) {
            cT = Tfgetc(m_pFile);
            if (cT == (char_t)WEOF) {
                break;
            }
            pszBuffer[ii] = cT;
        }
        return true;
    }
    void unlink()
    {
        if (m_pFile) {
            fclose(m_pFile);
            m_pFile = NULL;
        }
        Tunlink(static_cast<const char_t *>(m_szFilename));
    }
    bool ok() const
    {
        return (m_pFile != NULL);
    }
    const char_t *path() const
    {
        return static_cast<const char_t *>(m_szFilename);
    }
};

class CometFrame;

class CometApp : public wxApp
{
private:
    wxSingleInstanceChecker *m_pInstance;
    CometFrame *m_pMainFrame;

#ifdef __WXMSW__
    static bool FIRST_INSTANCE;
    static const WXUINT BROADCAST_MSG_ID;
    static const wxString BROADCAST_MSG_NAME;
#endif

    // Help
    char_t m_szHelpFilename[LM_STRSIZE];

    lmSigma *m_pSigma;

    wxString m_strStartScript;

    FILE *m_fpStdout;
    FILE *m_fpStderr;

public:
    static bool INITIALIZED;
    static bool COMETCONSOLE;

    CometApp();

    void reset(void);

    bool OnInit();
    int OnExit();

#ifdef __WXMSW__
    static const WXUINT getMessageId(void)
    {
        return CometApp::BROADCAST_MSG_ID;
    }
    static const wxString &getMsgFilename(void)
    {
        return CometApp::BROADCAST_MSG_NAME;
    }
    static const bool isFirstInstance(void)
    {
        return CometApp::FIRST_INSTANCE;
    }
#endif

    static wxString APPDIR;
    static wxString BINDIR;
    static wxString INCDIR;
    static wxString LIBDIR;
    static wxString DOCDIR;
    static wxString EXPDIR;
    static wxString USRDIR;
    static wxString TPLDIR;
    static wxString CNFDIR;

    static wxSize getTextExtent(wxWindow *pWnd, const wxString &strT, bool boldFont = false)
    {
        int ix, iy;
        wxFont tFont;
        if (boldFont) {
            tFont = pWnd->GetFont();
            wxFont boldFont = tFont;
            boldFont.SetWeight(wxFONTWEIGHT_BOLD);
            pWnd->SetFont(boldFont);
        }
        pWnd->GetTextExtent(strT, &ix, &iy);
        wxSize sizeT(ix, iy);
        if (boldFont) {
            pWnd->SetFont(tFont);
        }
        return sizeT;
    }

    CometFrame *getMainFrame(void)
    {
        return m_pMainFrame;
    }

    void setMainFrame(CometFrame *pMainFrame)
    {
        m_pMainFrame = pMainFrame;
    }

    wxString getStartScript(void)
    {
        return m_strStartScript;
    }

    void resetStartScript(void)
    {
        m_strStartScript = wxEmptyString;
    }

    void Output(const wxString &strT, bool bAddTime = false, bool bShowMessage = false, wxWindow *pParent = NULL);

    lmSigma *getSigma(void)
    {
        return m_pSigma;
    }

    static bool isExecutable(const wxString &pszFilename, const wxString &pszExt = wxEmptyString);

    bool GetUserDir(void);
    void DoGetAppDir(const char_t *pzDir);
    bool GetAppDir(void);
    bool GetOptions();
    bool SetOptions();

    static wxString getConfigHeader(void)
    {
        char_t szValue[LM_STRSIZEN];
        Tmemset(szValue, 0, LM_STRSIZEN);

        time_t nowT = time(NULL);
        tm *timeT = localtime(&nowT);
        Tstrftime(szValue, LM_STRSIZEN - 1, uT("%Y-%m-%d %H:%M:%S"), timeT);

        wxString strHeader = COMET_HEADER_STRING;

        // For unicode char, I need to specify %ls in the format
#ifdef __WXMSW__
        strHeader += wxString::Format(uT("\n# Comet <Version: %s Build: %s>\n# Saved: %s \n\n"), COMET_VERSION_STRING, COMET_BUILD_STRING, szValue);
#else
        strHeader += wxString::Format(uT("\n# Comet <Version: %ls Build: %ls>\n# Saved: %ls \n\n"), COMET_VERSION_STRING, COMET_BUILD_STRING, szValue);
#endif

        return strHeader;
    }

    // Utility functions extensively used by all components
    int StringToChar(wxString &strSrc, char_t *szDest, int nMax, bool bRemoveEOL = false);
    int StringReverseFind(wxString &strT, const char_t *szFind);

    void displayHelp(void);
};

DECLARE_APP(CometApp)

int SigmaMessageBox(const wxString &strMessage, const wxString &strCaption, long iStyle,
                    wxWindow *pParent,
                    const wxString &strButtonALabel = uT(""),
                    const wxString &strButtonBLabel = uT(""),
                    const wxString &strButtonCLabel = uT(""));

// Command line
#define CMDLINE_ARGCOUNT    12

class SigmaCmdLine
{
public:
    SigmaCmdLine()
    {
        int ii = 0;
        for (ii = 0; ii < CMDLINE_ARGCOUNT; ii++) {
            Tmemset(m_szCmdArg[ii], 0, LM_STRSIZE);
        }
        m_iArgCount = 0;
        char_t szCmdLine[LM_STRSIZEW];
        Tmemset(szCmdLine, 0, LM_STRSIZEW);

#ifdef __WXMSW__
        const char_t* pszCmdLine = ::GetCommandLine();
        if (pszCmdLine) {
            size_t iLen = Tstrlen(pszCmdLine);
            if ((iLen > 7) && (iLen < LM_STRSIZEW)) {
                Tstrncpy(szCmdLine, pszCmdLine, LM_STRSIZEW - 1);
                this->parse(szCmdLine, (int) (iLen));
            }
        }
#else
        pid_t pidT = getpid();
        if (pidT > 0) {
            char_t szCmdFilename[LM_STRSIZE];
            Tmemset(szCmdFilename, 0, LM_STRSIZE);
            Tsnprintf(szCmdFilename, LM_STRSIZE - 1, uT("/proc/%d/cmdline"), pidT);
            FILE *pfCmd = Tfopen((const char_t*) szCmdFilename, uT("r"));
            if (pfCmd) {
                if (Tfgets(szCmdLine, LM_STRSIZEW - 1, pfCmd) != NULL) {
                    char_t *pszArg = &szCmdLine[0];
                    int iZprev = -1;
                    for (ii = 0; ii < (LM_STRSIZEW - 1); ii++) {
                        if (szCmdLine[ii] == uT('\0')) {
                            if ((iZprev >= 0) && (ii == (iZprev + 1))) {
                                // \0\0 found --> end
                                break;
                            }
                            Tstrncpy(m_szCmdArg[m_iArgCount], pszArg, LM_STRSIZE - 1);
                            m_iArgCount++;
                            if (m_iArgCount >= CMDLINE_ARGCOUNT) {
                                break;
                            }
                            pszArg = &szCmdLine[ii + 1];
                            iZprev = ii;
                        }
                    }
                }
                fclose(pfCmd);
                pfCmd = NULL;
            }
        }
#endif
    }
    ~SigmaCmdLine()
    {

    }

    bool isOK(void)
    {
        return ((m_iArgCount >= 2) && (m_szCmdArg[0][0] != uT('\0')) && (m_szCmdArg[1][0] != uT('\0')));
    }
    const char_t* getArg(int ii) const
    {
        if ((ii < 0) || (ii >= m_iArgCount)) {
            return NULL;
        }
        return static_cast<const char_t*>(m_szCmdArg[ii]);
    }
    int getArgCount(void) const
    {
        return m_iArgCount;
    }

private:
    char_t m_szCmdArg[CMDLINE_ARGCOUNT][LM_STRSIZE];
    int m_iArgCount;

    bool isWhitespace(char_t cT)
    {
        return ((cT == uT(' ')) || (cT == uT('\t')) || (cT == uT('\r')) || (cT == uT('\n')));
    }

    void parse(char_t *pszCmdLine, int iLen)
    {
        if (pszCmdLine[0] == uT('\0')) {
            return;
        }

        bool bInQuotes = false;
        char_t* pszT = pszCmdLine;
        char_t* pszTend = pszT + iLen;
        char_t* pszArg = NULL;
        m_iArgCount = 0;

        while (true) {

            if ((*pszT == uT('\0')) || (pszT >= pszTend)) {
                break;
            }

            // Trim
            while (isWhitespace(*pszT)) {
                pszT++;
            }

            bInQuotes = (*pszT == uT('\"'));

            if (bInQuotes) {
                pszT++;
            }

            pszArg = pszT;

            if (bInQuotes) {
                while (*pszT && (*pszT != uT('\"')) && (pszT < pszTend)) {
                    pszT++;
                }
                if (*pszT && (*pszT == uT('\"'))) {
                    *pszT = uT('\0');
                    pszT++;
                }
            }
            else {
                while (*pszT && !isWhitespace(*pszT) && (pszT < pszTend)) {
                    pszT++;
                }
                if (*pszT && isWhitespace(*pszT)) {
                    *pszT = uT('\0');
                    pszT++;
                }
            }

            if (pszArg && (pszArg < pszT)) {
                Tstrncpy(m_szCmdArg[m_iArgCount], pszArg, LM_STRSIZE - 1);
                m_iArgCount++;
                if (m_iArgCount >= CMDLINE_ARGCOUNT) {
                    return;
                }
            }
        }
    }

};

#endif
