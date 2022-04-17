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
#include "CometComboBox.h"

BEGIN_EVENT_TABLE(CometComboBox, wxComboBox)
    EVT_SET_FOCUS(CometComboBox::OnSetFocus)
    EVT_KILL_FOCUS(CometComboBox::OnKillFocus)
    EVT_KEY_UP(CometComboBox::OnKeyUp)
    EVT_MENU(wxID_CUT, CometComboBox::OnEditAction)
    EVT_MENU(wxID_COPY, CometComboBox::OnEditAction)
    EVT_MENU(wxID_PASTE, CometComboBox::OnEditAction)
    EVT_MENU(wxID_CLEAR, CometComboBox::OnEditAction)
    EVT_MENU(wxID_SELECTALL, CometComboBox::OnEditAction)
END_EVENT_TABLE()

CometComboBox::CometComboBox(wxWindow *pParent, wxWindowID idT,
                             const wxString &valueT /* = wxEmptyString*/,
                             const wxPoint &posT /* = wxDefaultPosition*/,
                             const wxSize &sizeT /* = wxDefaultSize*/,
                             int nn /* = 0*/, const wxString choicesT[] /* = NULL*/,
                             long styleT /* = 0*/,
                             const wxValidator &validatorT /* = wxDefaultValidator*/,
                             const wxString &nameT /* = wxComboBoxNameStr*/)
    : wxComboBox(pParent, idT, valueT, posT, sizeT, nn, choicesT, styleT | wxTE_PROCESS_ENTER, validatorT, nameT)
{
    m_bFocused = false;
}

void CometComboBox::OnEditAction(wxCommandEvent &tEvent)
{
    int idT = tEvent.GetId();

    switch (idT) {
        case wxID_CUT:
            if (CanCut()) {
                Cut();
            }
            break;
        case wxID_COPY:
            if (CanCopy()) {
                Copy();
            }
            break;
        case wxID_PASTE:
            if (CanPaste()) {
                Paste();
            }
            break;
        case wxID_CLEAR:
        {
            long ifrom = 0, ito = 0;
            GetSelection(&ifrom, &ito);
            Replace(ifrom, ito, wxEmptyString);
        } break;
        case wxID_SELECTALL:
            SelectAll();
            break;
        default:
            break;
    }
}

void CometComboBox::OnSetFocus(wxFocusEvent &tEvent)
{
    tEvent.Skip();

    m_bFocused = true;
}

void CometComboBox::OnKillFocus(wxFocusEvent &tEvent)
{
    tEvent.Skip();

    m_bFocused = false;
}

bool CometComboBox::DoAutoComplete(wxComboBox *pCombo, int nKeyCode)
{
    if ((nKeyCode < 32) || (nKeyCode == 127) || (nKeyCode > 254)) {
        return false;
    }

    size_t iCount = pCombo->GetCount();

    if ((pCombo->IsEnabled() == false) || (iCount < 1)) {
        return false;
    }

    long istart = -1, iend = -1;
    pCombo->GetSelection(&istart, &iend);
    if (istart < 2) {
        return false;
    }

    // autocomplete only if nothing on right
    wxString strT = pCombo->GetValue();
    if (iend < (long)(strT.Len() - 1)) {
        return false;
    }

    wxString strValue = strT.Left(istart);

    // Separator
    if (strValue.StartsWith(uT("--"))) {
        return false;
    }

    int idx = -1;
    for (size_t ii = 0; ii < iCount; ii++) {
        if (pCombo->GetString((unsigned int)ii).Left(strValue.Length()).Cmp(strValue) == 0) {
            idx = (int)(ii);
            break;
        }
    }
    if (idx >= 0) {
        pCombo->SetSelection(idx);
        pCombo->SetSelection((long)(istart), (long)(pCombo->GetValue().Length()));
        return true;
    }

    return false;
}

void CometComboBox::OnKeyUp(wxKeyEvent &tEvent)
{

    if (tEvent.AltDown() || tEvent.ControlDown() || tEvent.ShiftDown() || tEvent.MetaDown()) {
        tEvent.Skip();
        return;
    }

    if (CometComboBox::DoAutoComplete(this, tEvent.GetKeyCode()) == false) {
        tEvent.Skip();
        return;
    }
}
