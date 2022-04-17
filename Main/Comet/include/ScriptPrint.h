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


#ifndef SCRIPT_PRINT_H
#define SCRIPT_PRINT_H

#include "ScriptEdit.h"

#if wxUSE_PRINTING_ARCHITECTURE

#include <wx/print.h>    // printing support
#include <wx/printdlg.h> // wxPageSetupDialog

class ScriptPrint : public wxPrintout
{
public:
    ScriptPrint();
    ScriptPrint(ScriptEdit *pEdit, const wxString &strTitle = wxEmptyString);
    ~ScriptPrint();

    virtual bool OnBeginDocument(int startPage, int endPage);
    virtual void OnEndDocument();

    virtual bool HasPage(int iPage);
    virtual bool OnPrintPage(int iPage);
    virtual void GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo);

    static wxPrintData *GetPrintData(void);
    static wxPageSetupData *GetPageSetupData(void);

protected:
    bool DoPrintScaling(wxDC *pDC);

    ScriptEdit *m_pScriptEdit;

    wxArrayInt m_Pages; // starting position of each page
    wxRect m_PageRect;  // the dc rect of the whole page
    wxRect m_PrintRect; // the dc rect of the page - margins

    // Internal memory allocation
    static wxPrintData *m_pPrintData;
    static wxPageSetupData *m_pPageSetupData;

private:
    DECLARE_ABSTRACT_CLASS(ScriptPrint)
};

#endif // wxUSE_PRINTING_ARCHITECTURE

#endif
