#pragma once
#include <wx/wx.h>
#include <litehtml.h>
#include <wx/filesys.h>

class wxLiteHtmlDocContainer : public litehtml::document_container
{
public:
    struct ClipBox
    {
        typedef std::vector<ClipBox> vector;
        litehtml::position	box;
        litehtml::border_radiuses radius;

        ClipBox(const litehtml::position& vBox, const litehtml::border_radiuses& vRad)
        {
            box = vBox;
            radius = vRad;
        }

        ClipBox(const ClipBox& val)
        {
            box = val.box;
            radius = val.radius;
        }
        ClipBox& operator=(const ClipBox& val)
        {
            box = val.box;
            radius = val.radius;
            return *this;
        }
    };

    explicit wxLiteHtmlDocContainer(wxWindow* window);

    virtual ~wxLiteHtmlDocContainer();

    litehtml::uint_ptr create_font(const char* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm) override;

    void delete_font(litehtml::uint_ptr hFont) override;

    int text_width(const char* text, litehtml::uint_ptr hFont) override;

    void draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) override;

    int pt_to_px(int pt) const override;

    int get_default_font_size() const override;
    
    const char* get_default_font_name() const override;

    void draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) override;

    void get_image_size(const char* src, const char* baseurl, litehtml::size& sz) override;
    
    void load_image(const char* src, const char* baseurl, bool redraw_on_ready) override;

    void draw_background(litehtml::uint_ptr hdc, const std::vector<litehtml::background_paint>& bg) override;

    void draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) override;

    void set_caption(const char* caption) override;

    void set_base_url(const char* base_url) override;
    
    void link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) override;

    void on_anchor_click(const char* url, const litehtml::element::ptr& el) override;

    void set_cursor(const char* cursor) override;
    
    void transform_text(litehtml::string& text, litehtml::text_transform tt) override;

    void import_css(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl) override;
    
    void set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius) override;
    
    void del_clip() override;
    
    void get_client_rect(litehtml::position& client) const override;

    std::shared_ptr<litehtml::element>	create_element(const char* tag_name,
        const litehtml::string_map& attributes,
        const std::shared_ptr<litehtml::document>& doc) override;

    void get_media_features(litehtml::media_features& media) const override;

    void get_language(litehtml::string& language, litehtml::string& culture) const override;

    litehtml::string resolve_color(const litehtml::string& color) const override { return ""; }

private:
    wxString MakeUri(const char* src, const char* baseurl) const;

    void UpdateClipRegion();

private:
    ClipBox::vector m_clips;
    wxFileSystem m_fileSystem;
    wxWindow* m_window;
    wxRegion m_clipRegion;
};
