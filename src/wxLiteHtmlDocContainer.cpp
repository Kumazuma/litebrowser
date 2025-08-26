#include "wxLiteHtmlDocContainer.h"
#include <wx/dcclient.h>
#include <wx/url.h>
#include <wx/filesys.h>
wxLiteHtmlDocContainer::wxLiteHtmlDocContainer(wxWindow* window)
    : m_window(window)
{

}

wxLiteHtmlDocContainer::~wxLiteHtmlDocContainer()
{
}

litehtml::uint_ptr wxLiteHtmlDocContainer::create_font(const char* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm)
{
    // TODO: Not implemented yet!

}

void wxLiteHtmlDocContainer::delete_font(litehtml::uint_ptr hFont)
{
    auto font = reinterpret_cast<wxFont*>(hFont);
    delete font;
}

int wxLiteHtmlDocContainer::text_width(const char* text, litehtml::uint_ptr hFont)
{
    wxClientDC dc(m_window);
    auto textFromUtf8 = wxString::FromUTF8(text);
    auto font = reinterpret_cast<wxFont*>(hFont);
    dc.SetFont(*font);
    return dc.GetTextExtent(textFromUtf8).GetWidth();
}

void wxLiteHtmlDocContainer::draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos)
{
    auto dc = reinterpret_cast<wxDC*>(hdc);
    auto textFromUtf8 = wxString::FromUTF8(text);
    auto font = reinterpret_cast<wxFont*>(hFont);
    dc->SetFont(*font);
    dc->SetTextForeground(wxColour(color.red, color.green, color.blue));
    dc->DrawText(textFromUtf8, wxPoint(pos.x, pos.y));
}

int wxLiteHtmlDocContainer::pt_to_px(int pt) const
{
    double dpi = m_window->GetDPI().x;
    return (int)((double)pt * dpi / 72.0);
}

int wxLiteHtmlDocContainer::get_default_font_size() const
{
    return wxLiteHtmlDocContainer::pt_to_px(12);
}

const char* wxLiteHtmlDocContainer::get_default_font_name() const
{
    return "Times New Roman";
}

void wxLiteHtmlDocContainer::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker)
{
    auto dc = reinterpret_cast<wxDC*>(hdc);
    if(marker.marker_type == litehtml::list_style_type::list_style_type_circle)
    {
        dc->SetBrush(*wxBLACK_BRUSH);
        dc->DrawCircle(wxPoint(marker.pos.x + marker.pos.width / 2, marker.pos.y + marker.pos.height / 2), marker.pos.width / 4);
    }
    else if(marker.marker_type == litehtml::list_style_type::list_style_type_disc)
    {
        dc->SetPen(*wxBLACK_PEN);
        dc->SetBrush(*wxTRANSPARENT_BRUSH);
        dc->DrawCircle(wxPoint(marker.pos.x + marker.pos.width / 2, marker.pos.y + marker.pos.height / 2), marker.pos.width / 4);
    }
    else if(marker.marker_type == litehtml::list_style_type::list_style_type_square)
    {
        dc->SetBrush(*wxBLACK_BRUSH);
        dc->DrawRectangle(wxRect(marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height));
    }
    else if(marker.marker_type == litehtml::list_style_type::list_style_type_decimal)
    {
        auto font = reinterpret_cast<wxFont*>(marker.font);
        dc->SetFont(*font);
        dc->SetTextForeground(wxColour(marker.color.red, marker.color.green, marker.color.blue));
        dc->DrawText(wxString::Format("%d.", marker.index), wxPoint(marker.pos.x, marker.pos.y));
    }
}

void wxLiteHtmlDocContainer::get_image_size(const char* src, const char* baseurl, litehtml::size& sz)
{
    auto path = MakeUri(src, baseurl);
    m_fileSystem.OpenFile(path);
    wxFSFile* file = m_fileSystem.OpenFile(path);
    if(file == nullptr)
    {
        sz.width = 0;
        sz.height = 0;
        return;
    }
    wxImage image;
    if (!image.LoadFile(*file->GetStream()))
    {
        sz.width = 0;
        sz.height = 0;
        return;
    }

    sz.width = image.GetWidth();
    sz.height = image.GetHeight();
    delete file;
}

void wxLiteHtmlDocContainer::draw_background(litehtml::uint_ptr hdc, const std::vector<litehtml::background_paint>& bg)
{
    // TODO: Working on it!
}

wxString wxLiteHtmlDocContainer::MakeUri(const char* src, const char* baseurl) const
{
    wxString srcWx;
    wxString baseUrlWx;
    if(src != nullptr)
        srcWx = wxString::FromUTF8(src);

    if (baseurl != nullptr)
        baseUrlWx = wxString::FromUTF8(baseurl);

    wxURL url;
    if (srcWx.StartsWith(wxS("/")))
    {
        if(baseUrlWx.EndsWith(wxS("/")))
            baseUrlWx.RemoveLast();
    }
    else
    {
        if (!baseUrlWx.EndsWith(wxS("/")))
            baseUrlWx.Append(wxS("/"));
    }

    return baseUrlWx + srcWx;
}
