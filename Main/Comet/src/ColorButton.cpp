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


#include "CometFrame.h"
#include "ColorButton.h"
#include "ColorsDlg.h"

#include <wx/dcbuffer.h>
#include <wx/colordlg.h>

BEGIN_EVENT_TABLE(ColorButton, wxPanel)
EVT_LEFT_UP(ColorButton::OnMouseUp)
EVT_ERASE_BACKGROUND(ColorButton::OnEraseBackground)
EVT_PAINT(ColorButton::OnPaint)
END_EVENT_TABLE()

ColorButton::ColorButton(wxWindow *parentT, wxWindowID idT, const wxString &strLabel /* = uT("")*/,
                         const wxColor &colorFore /* = wxColor(LM_COLOR(0x00, 0x00, 0x00))*/,
                         const wxColor &colorBack /* = wxColor(LM_COLOR(0xFF, 0xFF, 0xFF))*/,
                         const wxPoint &posT /* = wxDefaultPosition*/, const wxSize &sizeT /* = wxDefaultSize*/,
                         int indexT /* = 0*/, long styleT /* = 0*/, const wxString &nameT /* = wxEmptyString*/)
    : wxPanel(parentT, idT, posT, sizeT, styleT | wxSTATIC_BORDER, wxEmptyString)
{
    m_bBack = false;
    m_strLabel = strLabel;
    m_Color = colorFore;
    m_ColorBack = colorBack;
    m_Index = indexT;
}

void ColorButton::OnMouseUp(wxMouseEvent &tEvent)
{
    wxColourData clrData;
    clrData.SetColour(m_bBack ? m_ColorBack : m_Color);
    clrData.SetCustomColour(m_Index, m_bBack ? m_ColorBack : m_Color);
    wxColourDialog dlgColor(this, &clrData);
    if (dlgColor.ShowModal() == wxID_OK) {
        wxColourData retData = dlgColor.GetColourData();
        (m_bBack ? m_ColorBack : m_Color) = retData.GetColour();
        (static_cast<ColorsDlg *>(GetParent()))->updateStyle();
    }
    Refresh();
    tEvent.Skip();
}

void ColorButton::OnEraseBackground(wxEraseEvent &WXUNUSED(tEvent))
{
    // Do nothing, to avoid flickering
}

void ColorButton::PaintBackground(wxDC &dcT)
{
    wxBrush brOld = dcT.GetBrush();
    wxPen penOld = dcT.GetPen();

    wxRect windowRect(wxPoint(0, 0), GetClientSize());

    dcT.SetPen(*wxTRANSPARENT_PEN);
    wxBrush brushT(m_ColorBack);
    dcT.SetBrush(brushT);
    dcT.DrawRectangle(windowRect);

    dcT.SetTextForeground(m_Color);
    dcT.SetTextBackground(m_ColorBack);
    dcT.SetFont(this->GetFont());
    dcT.DrawLabel(m_strLabel, windowRect, wxALIGN_CENTER);

    dcT.SetBrush(brOld);
    dcT.SetPen(penOld);

#if defined(__WXMSW__)
    brushT.FreeResource(true);
#endif
}

void ColorButton::OnPaint(wxPaintEvent &WXUNUSED(tEvent))
{
    wxBufferedPaintDC memDC(this);

    PaintBackground(memDC);
}
