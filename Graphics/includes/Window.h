#pragma once

namespace graphics
{
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