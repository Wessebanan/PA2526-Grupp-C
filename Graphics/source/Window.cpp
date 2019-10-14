#include "../includes/Window.h"

#include <iostream>

namespace graphics
{
	constexpr DWORD ws_borderless = (WS_POPUPWINDOW);
	constexpr DWORD ws_border = (WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION);

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	Window::Window()
	{
		m_hwnd = NULL;
		m_msg = {};
		m_isOpen = false;
	}

	Window::~Window()
	{
	}

	HRESULT Window::Initialize(
		const UINT clientWidth,
		const UINT clientHeight,
		const char* pTitle,
		const WINDOW_STYLE style)
	{

		m_clientWidth	= clientWidth;
		m_clientHeight	= clientHeight;

		// Register the window class.
		const char* CLASS_NAME = pTitle;

		WNDCLASS wincl;
		if (!GetClassInfo(NULL, CLASS_NAME, &wincl))
		{
			WNDCLASS wc = { };

			wc.lpfnWndProc		= WindowProc;
			wc.hInstance		= NULL;
			wc.lpszClassName	= CLASS_NAME;

			if (!RegisterClass(&wc)) return S_FALSE;
		}

		int x = 0;
		int y = 0;
		DWORD ws_style = 0;
		switch (style)
		{
		case WINDOW_STYLE::BORDER:
			x = CW_USEDEFAULT;
			y = CW_USEDEFAULT;
			ws_style = ws_border;
			break;

		case WINDOW_STYLE::BORDERLESS:
			ws_style = ws_borderless;
			x = -1;
			y = -1;
			break;
		}

		RECT rect = { 0, 0, (LONG)m_clientWidth, (LONG)m_clientHeight };
		AdjustWindowRect(&rect, ws_style, false);
		m_clientWidth	= rect.right - rect.left;
		m_clientHeight	= rect.bottom - rect.top;

		// Create the window.

		m_hwnd = CreateWindowEx(
			0,                              // Optional window styles.
			CLASS_NAME,                     // Window class
			pTitle,							// Window text
			ws_style,						// Window style

			// Size and position
			x,
			y,
			m_clientWidth,
			m_clientHeight,

			NULL,       // Parent window    
			NULL,       // Menu
			NULL,		// Instance handle
			NULL        // Additional application data
		);

		return m_hwnd ? S_OK : S_FALSE;
	}

	void Window::Open()
	{
		ShowWindow(m_hwnd, SW_NORMAL);
		m_isOpen = true;
	}

	bool Window::Close()
	{
		return DestroyWindow(m_hwnd);
	}

	bool Window::Update()
	{
		if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
		{
			m_isOpen = WM_QUIT != m_msg.message;

			TranslateMessage(&m_msg);
			DispatchMessage(&m_msg);

			return true;
		}

		return false;
	}

	bool Window::IsOpen()
	{
		return m_isOpen;
	}
}