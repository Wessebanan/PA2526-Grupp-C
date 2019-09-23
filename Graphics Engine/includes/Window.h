#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class Window
{
public:
	Window();
	~Window();

	void Initialize(
		const UINT clientWidth,
		const UINT clientHeight,
		const char* pTitle);

	void Show();

	bool Update();

	bool IsOpen();

	operator const HWND() const { return m_hwnd; }

private:
	MSG m_msg;
	bool m_isOpen;

protected:
	HWND m_hwnd;
};