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


#include "ScriptPrint.h"

#include <wx/progdlg.h> // wxProgressDialog
#include <wx/dcclient.h>

#if wxUSE_PRINTING_ARCHITECTURE

IMPLEMENT_ABSTRACT_CLASS(ScriptPrint, wxPrintout)

// Internal memory allocation
wxPrintData *ScriptPrint::m_pPrintData = NULL;
wxPageSetupData *ScriptPrint::m_pPageSetupData = NULL;

wxPrintData *ScriptPrint::GetPrintData(void)
{
    if (m_pPrintData == NULL) {
        m_pPrintData = new (std::nothrow) wxPrintData();
        if (m_pPrintData != NULL) {
            m_pPrintData->SetPaperId(wxPAPER_A4);
        }
    }

    return m_pPrintData;
}

wxPageSetupData *ScriptPrint::GetPageSetupData(void)
{
    if (m_pPageSetupData == NULL) {
        m_pPageSetupData = new (std::nothrow) wxPageSetupData();
        if (m_pPageSetupData != NULL) {
            m_pPageSetupData->SetPaperSize(wxPAPER_A4);
            m_pPageSetupData->SetMarginTopLeft(wxPoint(20, 20));
            m_pPageSetupData->SetMarginBottomRight(wxPoint(20, 20));
        }
    }

    return m_pPageSetupData;
}

ScriptPrint::ScriptPrint()
    : wxPrintout(uT("Comet")), m_pScriptEdit(NULL)
{
}

ScriptPrint::ScriptPrint(ScriptEdit *pEdit, const wxString &strTitle)
    : wxPrintout(strTitle), m_pScriptEdit(pEdit)
{
}

ScriptPrint::~ScriptPrint()
{
    if (m_pPrintData) {
        delete m_pPrintData;
        m_pPrintData = NULL;
    }

    if (m_pPageSetupData) {
        delete m_pPageSetupData;
        m_pPageSetupData = NULL;
    }
}

bool ScriptPrint::OnBeginDocument(int startPage, int endPage)
{
    if (m_pScriptEdit == NULL) {
        return false;
    }

    return wxPrintout::OnBeginDocument(startPage, endPage);
}

void ScriptPrint::OnEndDocument()
{
    wxPrintout::OnEndDocument();
}

bool ScriptPrint::HasPage(int iPage)
{
    // Pages start at 1
    return ((iPage >= 1) && (iPage <= static_cast<int>(m_Pages.GetCount())));
}

bool ScriptPrint::OnPrintPage(int iPage)
{
    wxDC *pDC = GetDC();
    if ((m_pScriptEdit == NULL) || (HasPage(iPage) == false) || (pDC == NULL)) {
        return false;
    }

    // For every page
    DoPrintScaling(pDC);

    m_pScriptEdit->FormatRange(true, m_Pages[iPage - 1],
                               (static_cast<int>(m_Pages.GetCount()) > iPage) ? m_Pages[iPage] : m_pScriptEdit->GetLength(),
                               pDC, pDC, m_PrintRect, m_PageRect);

    return true;
}

void ScriptPrint::GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo)
{
    if ((m_pScriptEdit == NULL) || (minPage == NULL) || (maxPage == NULL) || (pageFrom == NULL) || (pageTo == NULL)) {
        return;
    }

    // initialize values
    *minPage = 0;
    *maxPage = 0;
    *pageFrom = 0;
    *pageTo = 0;

    // scale DC if possible
    wxDC *pDC = GetDC();
    if (pDC == NULL) {
        return;
    }

    DoPrintScaling(pDC);

    // count pages
    int ipos = 0, iposPrev;
    int ilength = m_pScriptEdit->GetLength();
    int ilines = m_pScriptEdit->GetLineCount();
    int ipages = 1;

    m_Pages.Clear();

    wxProgressDialog progDialog(uT("Printing"), uT("Page 1 of ?"));

    while (ipos < ilength) {

        if (*maxPage >= (int)(m_Pages.GetCount())) {
            m_Pages.Add(ipos);
        }
        else {
            m_Pages[*maxPage] = ipos;
        }

        iposPrev = ipos;
        ipos = m_pScriptEdit->FormatRange(false, ipos, ilength, pDC, pDC, m_PrintRect, m_PageRect);
        if (ipos <= iposPrev) {
            break;
        }

        *maxPage += 1;

        int currentline = m_pScriptEdit->LineFromPosition(wxMax(ipos - 1, 0));
        currentline = wxMax(currentline, 1);
        ipages = 1 + ((*maxPage) * ilines) / currentline;
        progDialog.Update((ipos * 100) / ilength, wxString::Format(uT("Page %d of %d"), *maxPage, ipages));
    }

    if (*maxPage > 0) {
        *minPage = 1;
    }

    *pageFrom = *minPage;
    *pageTo = *maxPage;
}

bool ScriptPrint::DoPrintScaling(wxDC *pDC)
{
    if (pDC == NULL) {
        return false;
    }

    wxSize pageMMSize;
    GetPageSizeMM(&pageMMSize.x, &pageMMSize.y);

    wxSize ppiScr, ppiPrn;
    GetPPIScreen(&ppiScr.x, &ppiScr.y);
    GetPPIPrinter(&ppiPrn.x, &ppiPrn.y);

    float ppiScalex = static_cast<float>(ppiPrn.x) / static_cast<float>(ppiScr.x);
    float ppiScaley = static_cast<float>(ppiPrn.y) / static_cast<float>(ppiScr.y);

    wxSize dcSize, pagePixSize;
    pDC->GetSize(&dcSize.x, &dcSize.y);
    GetPageSizePixels(&pagePixSize.x, &pagePixSize.y);

    float pagepixScalex = static_cast<float>(dcSize.x) / static_cast<float>(pagePixSize.x);
    float pagepixScaley = static_cast<float>(dcSize.y) / static_cast<float>(pagePixSize.y);

    float dcScalex = ppiScalex * pagepixScalex;
    float dcScaley = ppiScaley * pagepixScaley;

    // Pixels / mm (1 inch = 25.4 mm)
    float ppmmx = static_cast<float>(ppiScr.x) / 25.4f;
    float ppmmy = static_cast<float>(ppiScr.y) / 25.4f;

    wxSize pageT = pageMMSize;
    pageT.x = static_cast<int>(pageT.x * ppmmx);
    pageT.y = static_cast<int>(pageT.y * ppmmy);
    m_PageRect = wxRect(0, 0, pageT.x, pageT.y);

    wxPageSetupData *pPageSetupData = GetPageSetupData();
    wxPoint topLeft = (pPageSetupData != NULL) ? pPageSetupData->GetMarginTopLeft() : wxPoint(20, 20);
    wxPoint bottomRight = (pPageSetupData != NULL) ? pPageSetupData->GetMarginBottomRight() : wxPoint(20, 20);

    int itop = static_cast<int>(topLeft.y * ppmmy);
    int ibottom = static_cast<int>(bottomRight.y * ppmmy);
    int ileft = static_cast<int>(topLeft.x * ppmmx);
    int iright = static_cast<int>(bottomRight.x * ppmmx);

    m_PrintRect = wxRect(ileft, itop, pageT.x - (ileft + iright), pageT.y - (itop + ibottom));

    pDC->SetUserScale(dcScalex, dcScaley);

    return true;
}

#endif // wxUSE_PRINTING_ARCHITECTURE
