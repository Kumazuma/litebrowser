#include "globals.h"
#include "litebrowser.h"
#include "BrowserWnd.h"
#include "..\containers\windows\cairo\cairo_font.h"
#include "wxLiteHtmlDocContainer.h"
#include <wx/weakref.h>
#include <wx/dcgraph.h>
#pragma comment( lib, "gdiplus.lib" )
#pragma comment( lib, "shlwapi.lib" )

using namespace Gdiplus;


class wxLiteBrowserApp : public wxApp
{
public:
    bool OnInit() override;

private:
    wxWeakRef<wxFrame> m_frame;
    litehtml::document::ptr m_doc;

};

const char* g_html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LiteHTML Test</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
            background-color: #f4f4f4;
            line-height: 1.6;
        }
        ul {
            border: 1px solid #ccc;
        }
        h1 {
            color: #333;
        }
        p {
            line-height: 1.6;
        }
        a {
            color: #0066cc;
            text-decoration: none;
        }
        a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <h1>Welcome to LiteHTML Browser</h1>
    <p>This is a simple HTML page rendered using the LiteHTML engine within a wxWidgets application.</p>
    <p>Here are some links to test:</p>
    <ul>
        <li><a href="https://www.litehtml.com">LiteHTML Official Website</a></li>
        <li><a href="https://www.wxwidgets.org">wxWidgets Official Website</a></li>
        <li><a href="https://www.github.com">GitHub</a></li>
    </ul>
    <p>Enjoy browsing!</p>
</body>
</html>

)";

wxIMPLEMENT_APP(wxLiteBrowserApp);

bool wxLiteBrowserApp::OnInit()
{
    wxFrame* frame = new wxFrame(nullptr, wxID_ANY, "LiteHTML Browser", wxDefaultPosition, wxSize(1024, 768));
    m_frame = frame;
    frame->Show(true);
    m_doc = litehtml::document::createFromString(g_html, new wxLiteHtmlDocContainer(frame));
    frame->Bind(wxEVT_PAINT, [this](wxPaintEvent& event) {
        wxFrame* frame = m_frame;
        if(frame == nullptr)
        {
            return;
        }

        wxPaintDC dc(frame);
        
//        dc.Clear();
        if (m_doc)
        {
            wxSize size = m_frame->GetClientSize();
            m_doc->render(size.GetWidth());

            litehtml::position clip(0, 0, size.GetWidth(), size.GetHeight());

            m_doc->draw((litehtml::uint_ptr)&dc, 0, 0, &clip);
        }

        });

    frame->Refresh();
    return true;
}


CRITICAL_SECTION cairo_font::m_sync;
#if 0
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	CoInitialize(NULL);
	InitCommonControls();

	InitializeCriticalSectionAndSpinCount(&cairo_font::m_sync, 1000);

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	{
		CBrowserWnd wnd(hInstance);

		wnd.create();
		if(lpCmdLine && lpCmdLine[0])
		{
			wnd.open(lpCmdLine);
		} else
		{
			wnd.open(L"http://www.litehtml.com/");
		}

		MSG msg;

		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(gdiplusToken);

	return 0;
}

#endif