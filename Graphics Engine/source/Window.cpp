#include "../includes/pch.h"
#include "../includes/Window.h"

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

void Window::Initialize(
	const UINT clientWidth,
	const UINT clientHeight,
	const char* pTitle)
{
	// Register the window class.
	const char CLASS_NAME[] = "D3D11 Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = NULL;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	RECT rect = { 0, 0, (LONG)clientWidth, (LONG)clientHeight };
	AdjustWindowRect(&rect, (WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION), false);

	// Create the window.

	m_hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		pTitle,							// Window text
		(WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION),					// Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,

		NULL,       // Parent window    
		NULL,       // Menu
		NULL,		// Instance handle
		NULL        // Additional application data
	);
}

void Window::Show()
{
	ShowWindow(m_hwnd, SW_NORMAL);
	m_isOpen = true;
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