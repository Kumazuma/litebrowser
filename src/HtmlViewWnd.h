#pragma once
#include <wx/wx.h>
#include "web_page.h"
#include "web_history.h"


class wxLiteHtmlWindow : public wxWindow
{
    wxDECLARE_EVENT_TABLE();
public:
    wxLiteHtmlWindow(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxASCII_STR(wxPanelNameStr));

    void Lock() { m_sync.Enter(); }
    
    void Unlock() { m_sync.Leave(); }

    void Refresh();

    void Back();

    void Forward();

    void SetCaption();

    bool IsValidPage(bool withLock = true);

    web_page* GetPage(bool withLock = true);

    void Render(bool calcTime = false, bool doRedraw = true, int calcRepeat = 1);

    void GetClientRect(litehtml::position& client) const;

    void ShowHash(std::wstring& hash);

    void UpdateHistory();

    void CalcDraw(int calcRepeat = 1);

protected:
    void OnPaint(wxPaintEvent& event);

    void OnSize(wxSizeEvent& event);

private:
    void Redraw(LPRECT rcDraw, bool update);

    void UpdateScroll();

    void UpdateCursor();

    void CreateDIB(int width, int height);

    void ScrollTo(int newLeft, int newTop);

private:
    int m_top;
    int m_left;
    int m_max_top;
    int m_max_left;
    web_history m_history;
    web_page* m_page;
    web_page* m_page_next;
    wxCriticalSection m_sync;
    simpledib::dib m_dib;

};

#define HTMLVIEWWND_CLASS	L"HTMLVIEW_WINDOW"

#define WM_IMAGE_LOADED		(WM_USER + 1000)
#define WM_PAGE_LOADED		(WM_USER + 1001)

using namespace litehtml;
class CBrowserWnd;

class CHTMLViewWnd
{
	HWND						m_hWnd;
	HINSTANCE					m_hInst;
	int							m_top;
	int							m_left;
	int							m_max_top;
	int							m_max_left;
	web_history					m_history;
	web_page*					m_page;
	web_page*					m_page_next;
	CRITICAL_SECTION			m_sync;
	simpledib::dib				m_dib;
	CBrowserWnd*				m_parent;
public:
	CHTMLViewWnd(HINSTANCE	hInst, CBrowserWnd* parent);
	virtual ~CHTMLViewWnd(void);

	void				create(int x, int y, int width, int height, HWND parent);
	void				open(LPCWSTR url, bool reload = FALSE);
	HWND				wnd()	{ return m_hWnd;	}
	void				refresh();
	void				back();
	void				forward();

	void				set_caption();
	void				lock();
	void				unlock();
	bool				is_valid_page(bool with_lock = true);
	web_page*			get_page(bool with_lock = true);

	void				render(BOOL calc_time = FALSE, BOOL do_redraw = TRUE, int calc_repeat = 1);
	void				get_client_rect(litehtml::position& client) const;
	void				show_hash(std::wstring& hash);
	void				update_history();
	void				calc_draw(int calc_repeat = 1);

protected:
	virtual void		OnCreate();
	virtual void		OnPaint(simpledib::dib* dib, LPRECT rcDraw);
	virtual void		OnSize(int width, int height);
	virtual void		OnDestroy();
	virtual void		OnVScroll(int pos, int flags);
	virtual void		OnHScroll(int pos, int flags);
	virtual void		OnMouseWheel(int delta);
	virtual void		OnKeyDown(UINT vKey);
	virtual void		OnMouseMove(int x, int y);
	virtual void		OnLButtonDown(int x, int y);
	virtual void		OnLButtonUp(int x, int y);
	virtual void		OnMouseLeave();
	virtual void		OnPageReady();
	
	void				redraw(LPRECT rcDraw, BOOL update);
	void				update_scroll();
	void				update_cursor();
	void				create_dib(int width, int height);
	void				scroll_to(int new_left, int new_top);
	

private:
	static LRESULT	CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
};

inline void CHTMLViewWnd::lock()
{
	EnterCriticalSection(&m_sync);
}

inline void CHTMLViewWnd::unlock()
{
	LeaveCriticalSection(&m_sync);
}
