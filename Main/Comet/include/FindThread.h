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


#ifndef THREAD_FIND_H
#define THREAD_FIND_H

#include <wx/wx.h>
#include <wx/thread.h>

#define PATH_MAXLEN        256
#define FIND_MAXLENGTH     (LM_STRSIZE / 2)
#define FIND_MAXLINESIZE   (LM_STRSIZEL)
#define FIND_MAXBUFFERSIZE (LM_STRSIZE * FINDLIST_MAXITEMS)

class FindThread : public wxThread
{
private:
    wxString m_strFind;
    bool m_bReplace;
    wxString m_strReplace;
    wxString m_strType;
    wxString m_strDirf;
    bool m_bSubDir;
    bool m_bWord;
    bool m_bCase;

    int m_iFind;
    int m_iReplace;
    int m_iFileCount;
    int m_iFileTotal;

    char_t m_szFilenameRe[LM_STRSIZE];
    char m_szFilenameA[LM_STRSIZE];
    char m_szFilenameReA[LM_STRSIZE];

    int m_iDirLen;

    char_t *m_pszFind;
    char_t m_szFind[FIND_MAXLENGTH];
    char_t m_szFindCase[FIND_MAXLENGTH];
    int m_iFindLen;
    char_t m_szReplace[FIND_MAXLENGTH];
    int m_iReplaceLen;
    char_t m_szLine[FIND_MAXLINESIZE];
    char_t m_szLineCase[FIND_MAXLINESIZE];
    int m_iCount;

    bool m_bStop;

    inline void updateBuffer(const wxString &strBuffer, int iBufferLen);
    inline void updateFilename(const wxString &strFilename);

#ifdef WIN32
    void doFindInDir(const char_t *pszDir);
#else
    void doFindInDir(const char *pszDir);
#endif
    bool doFindInFile(const wxString &strFilename);

public:
    FindThread() : wxThread(wxTHREAD_DETACHED)
    {
        m_strFind = wxEmptyString;
        m_strReplace = wxEmptyString;
        m_bReplace = false;
        m_strType = wxEmptyString;
        m_strDirf = wxEmptyString;
        m_bSubDir = false;
        m_bWord = false;
        m_bCase = false;

        m_iFind = 0;
        m_iReplace = 0;
        m_iFileCount = 0;
        m_iFileTotal = 0;

        m_iDirLen = 0;

        m_iCount = 0;
        Tmemset(m_szLine, 0, FIND_MAXLINESIZE);
        Tmemset(m_szLineCase, 0, FIND_MAXLINESIZE);

        m_pszFind = NULL;
        Tmemset(m_szFind, 0, FIND_MAXLENGTH);
        Tmemset(m_szFindCase, 0, FIND_MAXLENGTH);
        m_iFindLen = 0;
        Tmemset(m_szReplace, 0, FIND_MAXLENGTH);
        m_iReplaceLen = 0;

        memset(m_szFilenameA, 0, LM_STRSIZE * sizeof(char));
        memset(m_szFilenameReA, 0, LM_STRSIZE * sizeof(char));
        memset(m_szFilenameRe, 0, LM_STRSIZE * sizeof(char_t));

        m_bStop = false;
    }

    wxThreadError Create(const wxString &strFind, bool bReplace, const wxString &strReplace,
                         const wxString &strType, const wxString &strDirf, bool bSubDir, bool bWord, bool bCase);

protected:
    virtual ExitCode Entry();
    virtual void OnExit();
};

#endif
