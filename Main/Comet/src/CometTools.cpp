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

const int CometFrame::TOOL_LEX[] = {
    wxSTC_LEX_CPP,
    wxSTC_LEX_PYTHON,
    wxSTC_LEX_FORTRAN,
    wxSTC_LEX_LATEX,
    wxSTC_LEX_MAKEFILE,
    wxSTC_LEX_SOLIS,
    wxSTC_LEX_COMET,
    -1
};
const char_t *CometFrame::TOOL_EXT[] = {
    uT("C/C++ (*.c;*.cpp;*.cxx;*.cc)"),
    uT("Python (*.py)"),
    uT("Fortran (*.f;*.for)"),
    uT("LaTeX (*.tex;*.bib;*.cls;*.sty)"),
    uT("Makefile"),
    uT("Solis (*.solis)"),
    uT("User-defined"),
    uT("-1")
};
bool CometFrame::isToolSet(int iLexer)
{
    for (int ii = 0;; ii++) {
        if (-1 == CometFrame::TOOL_LEX[ii]) {
            return false;
        }
        if (CometFrame::TOOL_LEX[ii] == iLexer) {
            return true;
        }
    }
    return false;
}
SigmaToolPrefs CometFrame::TOOL_CONF;
const wxString CometFrame::getToolCmd(int iLexer, const wxString &strExt, const wxString &strShortFilename, bool *pbOut, bool *pbSilent, bool *pbCls)
{
    if (iLexer == wxSTC_LEX_COMET) {
        *pbOut = CometFrame::TOOL_CONF.bOut[TOOLS_MAXCOUNT - 1];
        *pbSilent = CometFrame::TOOL_CONF.bSilent;
        *pbCls = CometFrame::TOOL_CONF.bCls[TOOLS_MAXCOUNT - 1];
        return CometFrame::TOOL_CONF.szCmd[TOOLS_MAXCOUNT - 1];
    }

    for (int ii = 0;; ii++) {
        if (-1 == CometFrame::TOOL_LEX[ii]) {
            return wxEmptyString;
        }
        if (iLexer == CometFrame::TOOL_LEX[ii]) {
            wxString strExtT = uT("*.");
            strExtT += strExt;
            if (Tstrstr(CometFrame::TOOL_EXT[ii], LM_CSTR(strExtT)) != NULL) {
                *pbOut = CometFrame::TOOL_CONF.bOut[ii];
                *pbSilent = CometFrame::TOOL_CONF.bSilent;
                *pbCls = CometFrame::TOOL_CONF.bCls[ii];
                return CometFrame::TOOL_CONF.szCmd[ii];
            }
            else if (strShortFilename.IsSameAs(CometFrame::TOOL_EXT[ii])) {
                *pbOut = CometFrame::TOOL_CONF.bOut[ii];
                *pbSilent = CometFrame::TOOL_CONF.bSilent;
                *pbCls = CometFrame::TOOL_CONF.bCls[ii];
                return CometFrame::TOOL_CONF.szCmd[ii];
            }
            *pbOut = false;
            return wxEmptyString;
        }
    }
    *pbOut = false;
    return wxEmptyString;
}
const bool CometFrame::getToolOut(int iLexer)
{
    for (int ii = 0;; ii++) {
        if (-1 == CometFrame::TOOL_LEX[ii]) {
            return false;
        }
        if (iLexer == CometFrame::TOOL_LEX[ii]) {
            return CometFrame::TOOL_CONF.bOut[ii];
        }
    }
    return false;
}
const bool CometFrame::getToolSilent(int iLexer)
{
    return CometFrame::TOOL_CONF.bSilent;
}
