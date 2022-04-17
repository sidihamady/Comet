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


#ifndef SIGMA_COMBOBOX_H
#define SIGMA_COMBOBOX_H

#include "Identifiers.h"

class CometComboBox : public wxComboBox
{
private:
    bool m_bFocused;

protected:
    DECLARE_EVENT_TABLE()

public:
    CometComboBox(wxWindow *pParent, wxWindowID idT,
                  const wxString &valueT = wxEmptyString,
                  const wxPoint &posT = wxDefaultPosition,
                  const wxSize &sizeT = wxDefaultSize,
                  int nn = 0, const wxString choicesT[] = NULL,
                  long styleT = 0,
                  const wxValidator &validatorT = wxDefaultValidator,
                  const wxString &nameT = wxComboBoxNameStr);

    void OnSetFocus(wxFocusEvent &tEvent);
    void OnKillFocus(wxFocusEvent &tEvent);
    void OnKeyUp(wxKeyEvent &tEvent);
    void OnEditAction(wxCommandEvent &tEvent);

    static bool DoAutoComplete(wxComboBox *pCombo, int nKeyCode);

    bool isFocused(void)
    {
#ifdef __WXMSW__
        return m_bFocused;
#else
        return (m_bFocused || this->hasFocus());
#endif
    }
};

#endif
