///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/aboutdlgg.cpp
// Purpose:     implements wxGenericAboutBox() function
// Author:      Vadim Zeitlin
// Created:     2006-10-08
// RCS-ID:      $Id: aboutdlgg.cpp 58761 2009-02-08 14:55:44Z VZ $
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_ABOUTDLG

#ifndef WX_PRECOMP
    #include "wx/sizer.h"
    #include "wx/statbmp.h"
    #include "wx/stattext.h"
    #include "wx/button.h"
#endif //WX_PRECOMP

#include "wx/aboutdlg.h"
#include "wx/generic/aboutdlgg.h"

#include "wx/hyperlink.h"
#include "wx/collpane.h"

// ============================================================================
// implementation
// ============================================================================

// helper function: returns all array elements in a single comma-separated and
// newline-terminated string
static wxString AllAsString(const wxArrayString& a)
{
    wxString s;
    const size_t count = a.size();
    s.reserve(20*count);
    for ( size_t n = 0; n < count; n++ )
    {
        s << a[n] << (n == count - 1 ? _T("\n") : _T(", "));
    }

    return s;
}

// ----------------------------------------------------------------------------
// wxAboutDialogInfo
// ----------------------------------------------------------------------------

wxString wxAboutDialogInfo::GetDescriptionAndCredits() const
{
    wxString s = GetDescription();
    if ( !s.empty() )
        s << _T('\n');

    if ( HasDevelopers() )
        s << _T('\n') << _("Developed by ") << AllAsString(GetDevelopers());

    if ( HasDocWriters() )
        s << _T('\n') << _("Documentation by ") << AllAsString(GetDocWriters());

    if ( HasArtists() )
        s << _T('\n') << _("Graphics art by ") << AllAsString(GetArtists());

    if ( HasTranslators() )
        s << _T('\n') << _("Translations by ") << AllAsString(GetTranslators());

    return s;
}

wxIcon wxAboutDialogInfo::GetIcon() const
{
    wxIcon icon = m_icon;
    if ( !icon.Ok() && wxTheApp )
    {
        const wxTopLevelWindow * const
            tlw = wxDynamicCast(wxTheApp->GetTopWindow(), wxTopLevelWindow);
        if ( tlw )
            icon = tlw->GetIcon();
    }

    return icon;
}

wxString wxAboutDialogInfo::GetCopyrightToDisplay() const
{
    wxString ret = m_copyright;

#if wxUSE_UNICODE
    const wxString copyrightSign = wxString::FromUTF8("\xc2\xa9");
    ret.Replace(_T("(c)"), copyrightSign);
    ret.Replace(_T("(C)"), copyrightSign);
#endif // wxUSE_UNICODE

    return ret;
}

// ----------------------------------------------------------------------------
// wxGenericAboutDialog
// ----------------------------------------------------------------------------

bool wxGenericAboutDialog::Create(const wxAboutDialogInfo& info, wxWindow *pParent)
{
    // TODO: should we use main frame as parent by default here?
    if (!wxDialog::Create(pParent, wxID_ANY, _("About ") + info.GetName(),
                           wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE & ~wxRESIZE_BORDER) )
        return false;

    m_sizerText = new wxBoxSizer(wxVERTICAL);
    wxString strName = info.GetName();
     wxString strVersion = wxEmptyString;
     // [:COMET:]:151130: separate name and version
    if ( info.HasVersion() )
         strVersion = info.GetVersion();
     wxStaticText *label = new wxStaticText(this, wxID_ANY, strName);
    wxFont fontBig(*wxNORMAL_FONT);
    fontBig.SetPointSize(fontBig.GetPointSize() + 6);
    fontBig.SetWeight(wxFONTWEIGHT_BOLD);
    label->SetFont(fontBig);

    m_sizerText->Add(label, wxSizerFlags().Centre().Border());

     if (strVersion.IsEmpty() == false) {
         AddText(strVersion);
     }

    AddText(info.GetDescription());
    AddText(info.GetCopyrightToDisplay());

    if ( info.HasWebSite() )
    {
#if wxUSE_HYPERLINKCTRL
        AddControl(new wxHyperlinkCtrl(this, wxID_ANY,
                                       info.GetWebSiteDescription(),
                                       info.GetWebSiteURL()));
#else
        AddText(info.GetWebSiteURL());
#endif // wxUSE_HYPERLINKCTRL/!wxUSE_HYPERLINKCTRL
    }

#if wxUSE_COLLPANE
    if ( info.HasLicence() )
        AddCollapsiblePane(_("License"), info.GetLicence());

    if ( info.HasDevelopers() )
        AddCollapsiblePane((info.GetDevelopers().Count() > 1) ? _("Developers") : _("Developer"),
                           AllAsString(info.GetDevelopers()));

    if ( info.HasDocWriters() )
        AddCollapsiblePane(_("Documentation writers"),
                           AllAsString(info.GetDocWriters()));

    if ( info.HasArtists() )
        AddCollapsiblePane(_("Artists"),
                           AllAsString(info.GetArtists()));

    if ( info.HasTranslators() )
        AddCollapsiblePane(_("Translators"),
                           AllAsString(info.GetTranslators()));
#endif // wxUSE_COLLPANE

    DoAddCustomControls();
     wxIcon icon = info.GetIcon();
     this->SetIcon(icon);
    wxSizer *sizerIconAndText = new wxBoxSizer(wxHORIZONTAL);

// >> [:COMET:]:130525: Comment out the following lines

//#if wxUSE_STATBMP
//    wxIcon icon = info.GetIcon();
//    if ( icon.Ok() )
//    {
//        sizerIconAndText->Add(new wxStaticBitmap(this, wxID_ANY, icon),
//                                wxSizerFlags().Border(wxRIGHT));
//    }
//#endif // wxUSE_STATBMP

// <<

    sizerIconAndText->Add(m_sizerText, wxSizerFlags(1).Expand());

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(sizerIconAndText, wxSizerFlags(1).Expand().Border());

    wxSizer *sizerBtns = CreateButtonSizer(wxOK);
    if ( sizerBtns )
    {
        sizerTop->Add(sizerBtns, wxSizerFlags().Expand().Border());
    }

    SetSizerAndFit(sizerTop);

    CentreOnScreen();

    return true;
}

void wxGenericAboutDialog::AddControl(wxWindow *win, const wxSizerFlags& flags)
{
    wxCHECK_RET( m_sizerText, _T("can only be called after Create()") );
    wxASSERT_MSG( win, _T("can't add NULL window to about dialog") );

    m_sizerText->Add(win, flags);
}

void wxGenericAboutDialog::AddControl(wxWindow *win)
{
    AddControl(win, wxSizerFlags().Border(wxDOWN).Centre());
}

void wxGenericAboutDialog::AddText(const wxString& text)
{
    if ( !text.empty() )
        AddControl(new wxStaticText(this, wxID_ANY, text));
}

void wxGenericAboutDialog::AddCollapsiblePane(const wxString& title,
                                              const wxString& text)
{
    wxCollapsiblePane *pane = new wxCollapsiblePane(this, wxID_ANY, title);
    wxStaticText *txt = new wxStaticText(pane->GetPane(), wxID_ANY, text,
                                         wxDefaultPosition, wxDefaultSize,
                                         wxALIGN_LEFT);

    // don't make the text unreasonably wide
    //static const int maxWidth = wxGetDisplaySize().x/3;
     static const int maxWidth = this->GetClientSize().x;

    txt->Wrap(maxWidth);

    // NB: all the wxCollapsiblePanes must be added with a null proportion value
    m_sizerText->Add(pane, wxSizerFlags(0).Expand().Border(wxBOTTOM));
}

// ----------------------------------------------------------------------------
// public functions
// ----------------------------------------------------------------------------

void wxGenericAboutBox(const wxAboutDialogInfo& info, wxWindow *pParent)
{
    wxGenericAboutDialog dlg(info, pParent);
    dlg.ShowModal();
}

// currently wxAboutBox is implemented natively only under these platforms, for
// the others we provide a generic fallback here
#if !defined(__WXMSW__) && !defined(__WXMAC__) && !defined(__WXGTK26__)

void wxAboutBox(const wxAboutDialogInfo& info, wxWindow *pParent)
{
    wxGenericAboutBox(info, pParent);
}

#endif // platforms without native about dialog


#endif // wxUSE_ABOUTDLG
