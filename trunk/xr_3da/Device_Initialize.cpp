#include "stdafx.h"
#include "resource.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
	{
		Device.OnWM_Activate(wParam, lParam);
	}
	break;
	case WM_SETCURSOR:
		return 1;
	case WM_SYSCOMMAND:
		// Prevent moving/sizing and power loss in fullscreen mode
		switch (wParam)
		{
		case SC_MOVE:
		case SC_SIZE:
		case SC_MAXIMIZE:
		case SC_MONITORPOWER:
			return 1;
			break;
		}
		break;
	case WM_CLOSE:
		return 0;
	case WM_KEYDOWN:
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void CRenderDevice::Initialize()
{
	Log("# Initializing Engine...");
	TimerGlobal.Start			();
	TimerMM.Start				();

	// Unless a substitute hWnd has been specified, create a window to render into
    if( m_hWnd == nullptr)
    {
		const auto*	wnd_class ="_XRAY_1.5";

        // Register the windows class
		HINSTANCE hInstance = GetModuleHandle(nullptr);
        WNDCLASS wndClass = { 0, WndProc, 0, 0, hInstance,
                              LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ICON1) ),
                              LoadCursor(nullptr, IDC_ARROW ),
                              static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)),
                              nullptr, wnd_class };
        RegisterClass( &wndClass );

        // Set the window's initial style
        m_dwWindowStyle = WS_BORDER |WS_DLGFRAME;

        // Set the window's initial width
        RECT rc;
        SetRect			( &rc, 0, 0, 640, 480 );
        AdjustWindowRect( &rc, m_dwWindowStyle, FALSE );

        // Create the render window
		m_hWnd = CreateWindow( wnd_class, "S.T.A.L.K.E.R.: ESM Engine", m_dwWindowStyle,
                               /*rc.left, rc.top, */CW_USEDEFAULT, CW_USEDEFAULT,
                               (rc.right-rc.left), (rc.bottom-rc.top), nullptr,
                               nullptr, hInstance, nullptr );
    }

    // Save window properties
    m_dwWindowStyle = GetWindowLong( m_hWnd, GWL_STYLE );
    GetWindowRect	( m_hWnd, &m_rcWindowBounds );
    GetClientRect	( m_hWnd, &m_rcWindowClient );
}

