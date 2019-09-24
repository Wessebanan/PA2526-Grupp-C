#pragma once

#include "D3D11Header.h"

namespace graphics
{
	class Texture2DView
	{
		friend class RenderContext;
		friend class DeviceInterface;

	public:
		Texture2DView();
		~Texture2DView();

	private:

		void CreateTexture2D(
			ID3D11Device4* pDevice4,
			const DXGI_FORMAT format,
			const UINT width,
			const UINT height,
			const UINT bindFlags);

		void CreateRenderTargetView(ID3D11Device4* pDevice4);
		void CreateShaderResourceView(ID3D11Device4* pDevice4);

		void Release();

		ID3D11Texture2D* m_pTexture2D;

		ID3D11RenderTargetView* m_pRenderTarget;
		ID3D11ShaderResourceView* m_pShaderResource;
	};
}