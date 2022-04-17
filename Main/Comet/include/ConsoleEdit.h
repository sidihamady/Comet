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


#ifndef CONSOLE_EDIT_H
#define CONSOLE_EDIT_H

#include "CodeEdit.h"

#include <wx/process.h>

#define CONSOLE_HISTORY  16
#define CONSOLE_MAXLINES 256

class ConsoleEdit : public CodeEdit
{
private:
    DECLARE_EVENT_TABLE();

private:
    void *m_pThread;

    void createEngine(void);

    char_t m_szHistoryFile[LM_STRSIZE];
    char_t m_szCommand[LM_STRSIZE];

    wxString m_aCommandHistory[CONSOLE_HISTORY];
    int m_nCurCommand;
    int m_nTotalCommand;

    bool m_bError;
    bool m_bLastError;
    bool m_bNewLine;

    int m_iMLineFirst;
    int m_iLineRun;

    int m_iMaxLineWidth;

public:
    ConsoleEdit(wxWindow *pParent, wxWindowID id = wxID_ANY,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long styleT = wxNO_BORDER | wxVSCROLL | wxHSCROLL,
                const wxString &strName = wxSTCNameStr);
    virtual ~ConsoleEdit();

    bool setPrefs(const ScintillaPrefs &tPrefs, bool bInit = true);

    void restoreHistory(const char_t *pszSigmaConfigDir);
    void saveHistory(void);

    void Output(const wxString &strT);
    void initOutput(void);

    void getLine(wxString &strT, int &iLen);

    void OnThreadUpdated(wxCommandEvent &tEvent);

    void OnScriptRun(wxCommandEvent &tEvent);
    void OnEditAction(wxCommandEvent &tEvent);
    void OnConsoleAction(wxCommandEvent &tEvent);

    void OnMouseWheel(wxMouseEvent &tEvent);

    void DoEditCut(void);
    void DoEditCopy(void);
    void DoEditPaste(void);
    void DoEditRedo(void);
    void DoEditUndo(void);
    void DoEditDelete(void);
    void DoEditReset(void);

    void DoZoomIn(void);
    void DoZoomOut(void);
    void DoZoomNone(void);

    void doClear(void);
    void Reset(void);

    bool isRunning(void)
    {
        return m_bRunning;
    }
    void setRunning(bool bRunning)
    {
        m_bGuierrSet = false;
        m_bRunning = bRunning;
    }

    void AddToHistory(const char_t *pszCommand);

    int GetHistoryLength(void)
    {
        return m_nTotalCommand;
    }

    int SetCommand(const char_t *pszCommand);

    int LineIndex(int nLine = -1);

    void DoFindReset(void)
    {
        // nothing
    }

    void SetCurCommand(const wxString &strCommand);
    bool IsEditingLine(void);

    void luaRunScript(void);
    void luaStopScript(void);

    void endThread(const wxString &strT);

    void OnKeyDown(wxKeyEvent &tEvent);
    void OnChar(wxKeyEvent &tEvent);
    void OnContextMenu(wxContextMenuEvent &tEvent);
};

#endif
