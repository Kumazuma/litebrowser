#include "wxLiteHtmlDocContainer.h"
#include <wx/dcclient.h>
#include <wx/url.h>
wxLiteHtmlDocContainer::wxLiteHtmlDocContainer(wxWindow* window)
    : m_window(window)
{

}

wxLiteHtmlDocContainer::~wxLiteHtmlDocContainer()
{
}

litehtml::uint_ptr wxLiteHtmlDocContainer::create_font(const char* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm)
{
    wxFont* font = nullptr;
    std::string_view faceNamesView(faceName);
    while (!faceNamesView.empty())
    {
        auto spliterIdx = faceNamesView.find(';');
        std::string_view faceName = faceNamesView;
        std::string_view fontFamilyStr = "";
        wxFontFamily fontFamily = wxFONTFAMILY_DEFAULT;
        if (spliterIdx != std::string_view::npos)
        {
            faceName = faceNamesView.substr(0, spliterIdx);
            faceNamesView.remove_prefix(spliterIdx + 1);
        }

        spliterIdx = faceName.find(',');
        
        if (spliterIdx != std::string_view::npos)
        {
            fontFamilyStr = faceName.substr(spliterIdx + 1);
            // Trim leading spaces
            while (!fontFamilyStr.empty() && fontFamilyStr.front() == ' ')
            {
                fontFamilyStr.remove_prefix(1);
            }

            // Trim trailing spaces
            while (!fontFamilyStr.empty() && fontFamilyStr.back() == ' ')
            {
                fontFamilyStr.remove_suffix(1);
            }

            faceName = faceName.substr(0, spliterIdx);
            // Trim leading spaces
            while (!faceName.empty() && faceName.front() == ' ')
            {
                faceName.remove_prefix(1);
            }

            // Trim trailing spaces
            while (!faceName.empty() && faceName.back() == ' ')
            {
                faceName.remove_suffix(1);
            }
        }

        if(!fontFamilyStr.empty())
        {
            if(fontFamilyStr == "serif")
            {
                fontFamily = wxFONTFAMILY_ROMAN;
            }
            else if(fontFamilyStr == "sans-serif")
            {
                fontFamily = wxFONTFAMILY_SWISS;
            }
            else if(fontFamilyStr == "monospace")
            {
                fontFamily = wxFONTFAMILY_TELETYPE;
            }
            else if(fontFamilyStr == "cursive")
            {
                fontFamily = wxFONTFAMILY_SCRIPT;
            }
            else if(fontFamilyStr == "fantasy")
            {
                fontFamily = wxFONTFAMILY_DECORATIVE;
            }
        }

        font = wxFont::New(wxSize(0, size), fontFamily,
            (italic == litehtml::font_style_italic) ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL,
            (weight >= 700) ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL,
            false, wxString::FromUTF8(faceName.data(), faceName.size()));

        if (font != nullptr && font->IsOk())
            break;
    }
    
    if(font == nullptr)
    {
        return 0;
    }
    
    if (fm != nullptr)
    {
        wxWindowDC dc(m_window);
        dc.SetFont(*font);
        auto matrix = dc.GetFontMetrics();
        fm->ascent = matrix.ascent;
        fm->descent = matrix.descent;
        fm->height = matrix.height;
        fm->x_height = matrix.ascent - matrix.internalLeading;
        if (italic == litehtml::font_style_italic || decoration)
        {
            fm->draw_spaces = true;
        }
        else
        {
            fm->draw_spaces = false;
        }
    }

    // TODO: Not implemented yet!
    return reinterpret_cast<litehtml::uint_ptr>(font);
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
    wxPoint drawPos(pos.left(), pos.top());

    dc->SetFont(*font);
    auto matrix = dc->GetFontMetrics();
    drawPos.y -= matrix.descent;
    dc->SetTextForeground(wxColour(color.red, color.green, color.blue));
    dc->DrawText(textFromUtf8, drawPos);
}

int wxLiteHtmlDocContainer::pt_to_px(int pt) const
{
    double dpi = m_window->GetDPI().x;
    return (int)((double)pt * dpi / 72.0);
}

int wxLiteHtmlDocContainer::get_default_font_size() const
{
    return wxLiteHtmlDocContainer::pt_to_px(8);
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
        dc->DrawCircle(wxPoint(marker.pos.x, marker.pos.y), marker.pos.width);
    }
    else if(marker.marker_type == litehtml::list_style_type::list_style_type_disc)
    {
        dc->SetPen(*wxBLACK_PEN);
        dc->SetBrush(*wxTRANSPARENT_BRUSH);
        dc->DrawCircle(wxPoint(marker.pos.x, marker.pos.y), marker.pos.width);
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

void wxLiteHtmlDocContainer::load_image(const char* src, const char* baseurl, bool redraw_on_ready)
{
    // TODO: Working on it!

}

void wxLiteHtmlDocContainer::draw_background(litehtml::uint_ptr hdc, const std::vector<litehtml::background_paint>& bg)
{
    auto dc = reinterpret_cast<wxDC*>(hdc);
    if (!m_clipRegion.Empty())
    {
        dc->SetClippingRegion(m_clipRegion);
    }

    auto drawBg = [dc](const litehtml::background_paint& bg)
        {
            if (bg.color == litehtml::web_color::transparent)
            {
                return;
            }

            if (bg.is_root)
            {
                
            }
        };
    // TODO: Working on it!
}

void wxLiteHtmlDocContainer::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root)
{
    auto dc = reinterpret_cast<wxDC*>(hdc);
    if(!m_clipRegion.Empty())
    {
        dc->SetClippingRegion(m_clipRegion);
    }

    // NOTE: 일단 rounded border는 지원하지 않음
    // NOTE: 추후에 최적화를 위해서는 wxPen를 캐싱했다가 재활용해야함
    auto drawBorder = [&](const litehtml::border& border, const wxPoint& start, const wxPoint& end)
        {
            if (border.width <= 0 || border.style == litehtml::border_style::border_style_none || border.style == litehtml::border_style::border_style_hidden)
                return;
           
            wxPenStyle style = wxPENSTYLE_SOLID;
            switch (border.style)
            {
                case litehtml::border_style::border_style_dashed:
                    style = wxPENSTYLE_SHORT_DASH;
                    break;

                case litehtml::border_style::border_style_dotted:
                    style = wxPENSTYLE_DOT;
                    break;
                case litehtml::border_style::border_style_double:
                    style = wxPENSTYLE_DOT_DASH;
                    break;
            }

            wxPen pen(wxColour(border.color.red, border.color.green, border.color.blue), border.width, style);
            dc->SetPen(pen);
            dc->DrawLine(start, end);
        };

    wxRect rect(draw_pos.x, draw_pos.y, draw_pos.width, draw_pos.height);
    // 반시계 방향으로 그린다.

    // Left border
    drawBorder(borders.left, rect.GetLeftTop(), rect.GetLeftBottom());

    // Bottom border
    drawBorder(borders.bottom, rect.GetLeftBottom(), rect.GetRightBottom());

    // Right border
    drawBorder(borders.right, rect.GetRightBottom(), rect.GetRightTop());

    // Top border
    drawBorder(borders.top, rect.GetRightTop(), rect.GetLeftTop());
}

void wxLiteHtmlDocContainer::set_caption(const char* caption)
{
    // 일단 아무 것도 안 할 거임
}

void wxLiteHtmlDocContainer::set_base_url(const char* base_url)
{
    // NOTE: 무슨 역할을 하는지 잘 모르겠음
}

void wxLiteHtmlDocContainer::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el)
{
    // NOTE: 무슨 역할을 하는지 잘 모르겠음
    // NOTE: 기존 litebrowser에서도 처리 안 함
}

void wxLiteHtmlDocContainer::on_anchor_click(const char* url, const litehtml::element::ptr& el)
{
    // NOTE: 앵커를 클릭했을 때 호출되는 것같은데 스크롤링을 해야 하는 듯?
}

void wxLiteHtmlDocContainer::set_cursor(const char* cursor)
{
    // TODO: Not implemented yet!
}

void wxLiteHtmlDocContainer::transform_text(litehtml::string& text, litehtml::text_transform tt)
{
    switch (tt)
    {
        case litehtml::text_transform::text_transform_none:
            // 아무 것도 안 함
            break;
        case litehtml::text_transform::text_transform_capitalize:
            if (!text.empty())
                text[0] = toupper(text[0]);

            break;
        case litehtml::text_transform::text_transform_uppercase:
            for (auto& ch : text)
            {
                ch = toupper(ch);
            }

            break;
        case litehtml::text_transform::text_transform_lowercase:
            for (auto& ch : text)
            {
                ch = tolower(ch);
            }

            break;
    }
}

void wxLiteHtmlDocContainer::import_css(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl)
{
    // TODO: Not implemented yet!
    wxString urlWx = wxString::FromUTF8(url.c_str());
}

void wxLiteHtmlDocContainer::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius)
{
    m_clips.push_back(ClipBox(pos, bdr_radius));
    UpdateClipRegion();
}

void wxLiteHtmlDocContainer::del_clip()
{
    m_clips.pop_back();
    UpdateClipRegion();
}

void wxLiteHtmlDocContainer::get_client_rect(litehtml::position& client) const
{
    auto size = m_window->GetClientSize();
    client.x = 0;
    client.y = 0;
    client.width = size.GetWidth();
    client.height = size.GetHeight();
}

std::shared_ptr<litehtml::element> wxLiteHtmlDocContainer::create_element(const char* tag_name, const litehtml::string_map& attributes, const std::shared_ptr<litehtml::document>& doc)
{

    return nullptr;
}

void wxLiteHtmlDocContainer::get_media_features(litehtml::media_features& media) const
{
    auto size = m_window->GetClientSize();

    media.type = litehtml::media_type::media_type_screen;
    media.width = size.GetWidth();
    media.height = size.GetHeight();
    media.device_width = size.GetWidth();
    media.device_height = size.GetHeight();
    media.color = 8;
    media.monochrome = 0;
    media.color_index = 256;
    media.resolution = 96;
}

void wxLiteHtmlDocContainer::get_language(litehtml::string& language, litehtml::string& culture) const
{
    language = "en";
    culture = "US";
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

void wxLiteHtmlDocContainer::UpdateClipRegion()
{
    if (m_clips.empty())
    {
        m_clipRegion.Clear();
        return;
    }
    
    auto genRegion = [](const ClipBox& box) -> wxRegion
    {
        return wxRegion(box.box.left(), box.box.top(), box.box.width, box.box.height);
    };

    wxRegion region{ genRegion(m_clips.front()) };
    auto len = m_clips.size();
    for(int i = 1; i < len; ++i)
    {
        region.Intersect(genRegion(m_clips[i]));
    }

    m_clipRegion = region;
}
