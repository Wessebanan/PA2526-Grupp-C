#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace graphics
{
	struct CLIENT_INT2
	{
		int x, y;
	};

	inline CLIENT_INT2 GetWorkAreaResolution()
	{
		RECT r;
		SystemParametersInfoA(SPI_GETWORKAREA, 0, &r, 0);

		return {
			r.right - r.left,
			r.bottom - r.top
		};
	}

	inline CLIENT_INT2 GetDisplayResolution()
	{
		return {
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN) + 1
		};
	}

	inline CLIENT_INT2 GetClientResolution(const HWND hWnd)
	{
		RECT r;
		GetClientRect(hWnd, &r);

		return {
			r.right - r.left,
			r.bottom - r.top
		};
	}

	inline UINT HasFocus(const HWND hWnd)
	{
		return GetActiveWindow() == hWnd ? TRUE : FALSE;
	}

	enum class WINDOW_STYLE
	{
		BORDER,
		BORDERLESS,
	};

	class Window
	{
	public:
		Window();
		~Window();

		HRESULT Initialize(
			const UINT clientWidth,
			const UINT clientHeight,
			const char* pTitle,
			const WINDOW_STYLE style);

		void Open();
		bool Close();

		/*
			Returns:
					- true if message queue is non-empty
					- false if message queue is empty

		*/
		bool Update();

		bool IsOpen();

		operator const HWND() const { return m_hwnd; }

	private:
		MSG m_msg;
		bool m_isOpen;
		HWND m_hwnd;

		UINT m_clientWidth, m_clientHeight;

	};
}