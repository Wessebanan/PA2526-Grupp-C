#include "../includes/pch.h"
#include "../includes/PresentWindow.h"

namespace graphics
{

	PresentWindow::PresentWindow()
	{
		m_pSwapChain4 = NULL;
	}

	PresentWindow::~PresentWindow()
	{
	}

	void PresentWindow::Present()
	{
		m_pSwapChain4->Present(0, 0);
	}

	HRESULT PresentWindow::Initialize(
		ID3D11Device4* pDevice4,
		IDXGIFactory6* pFactory6,
		const UINT width,
		const UINT height,
		const char* pTitle)
	{
		Window::Initialize(width, height, pTitle);

		HRESULT hr = S_OK;

		{
			IDXGISwapChain1* pTemp = NULL;

			DXGI_SWAP_CHAIN_DESC1 desc = { 0 };
			desc.Width = 0;
			desc.Height = 0;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.Stereo = FALSE;
			desc.SampleDesc = { 1, 0 };
			desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.BufferCount = 2;
			desc.Scaling = DXGI_SCALING_NONE;
			desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			desc.Flags = 0;

			hr = pFactory6->CreateSwapChainForHwnd(
				pDevice4,
				m_hwnd,
				&desc,
				NULL,
				NULL,
				&pTemp);

			if (FAILED(hr)) return hr;

			pTemp->QueryInterface(IID_PPV_ARGS(&m_pSwapChain4));
			pTemp->Release();
		}

		return S_OK;
	}
}