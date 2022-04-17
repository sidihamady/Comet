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


#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H

class ColorButton : public wxPanel
{
private:
    wxString m_strLabel;
    wxColor m_Color;
    wxColor m_ColorBack;
    bool m_bBack;
    int m_Index;

    DECLARE_EVENT_TABLE()

public:
    ColorButton(wxWindow *parentT, wxWindowID idT, const wxString &strLabel = uT(""),
                const wxColor &colorFore = wxColor(LM_COLOR(0xFF, 0xFF, 0xFF)),
                const wxColor &colorBack = wxColor(LM_COLOR(0xFF, 0xFF, 0xFF)),
                const wxPoint &posT = wxDefaultPosition, const wxSize &sizeT = wxDefaultSize,
                int indexT = 0, long styleT = 0, const wxString &nameT = wxEmptyString);

    wxColor GetColorFore(void)
    {
        return m_Color;
    }
    wxColor GetColorBack(void)
    {
        return m_ColorBack;
    }
    void SetColorFore(wxColor tColor)
    {
        m_Color = tColor;
    }
    void SetColorBack(wxColor tColorBack)
    {
        m_ColorBack = tColorBack;
    }
    void SetBack(bool bBack = true)
    {
        m_bBack = bBack;
    }

    void OnMouseUp(wxMouseEvent &tEvent);
    void OnEraseBackground(wxEraseEvent &tEvent);
    void PaintBackground(wxDC &tDC);
    void OnPaint(wxPaintEvent &tEvent);
};

#endif
