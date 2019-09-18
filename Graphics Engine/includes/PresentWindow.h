#pragma once

#include "Window.h"
#include "D3D11Header.h"

namespace graphics
{
	class PresentWindow : public Window
	{
		friend class RenderContext;
		friend class DeviceInterface;
		friend class Direct2D;

	public:
		PresentWindow();
		~PresentWindow();

		void Present();

	private:

		void Initialize(
			ID3D11Device4* pDevice4,
			IDXGIFactory6* pFactory6,
			const UINT width,
			const UINT height,
			const char* pTitle);

		IDXGISwapChain4* m_pSwapChain4;
	};
}