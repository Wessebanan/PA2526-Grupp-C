#include "../includes/Texture2DView.h"


Texture2DView::Texture2DView()
{
}

Texture2DView::~Texture2DView()
{
}

void Texture2DView::CreateTexture2D(
	ID3D11Device4* pDevice4,
	const DXGI_FORMAT format,
	const UINT width,
	const UINT height,
	const UINT bindFlags)
{
	{
		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 0;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc = { 1, 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = bindFlags;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		pDevice4->CreateTexture2D(&desc, NULL, &m_pTexture2D);
	}
}

void Texture2DView::CreateRenderTargetView(ID3D11Device4* pDevice4)
{
	pDevice4->CreateRenderTargetView(m_pTexture2D, NULL, &m_pRenderTarget);
}

void Texture2DView::CreateShaderResourceView(ID3D11Device4* pDevice4)
{
	pDevice4->CreateShaderResourceView(m_pTexture2D, NULL, &m_pShaderResource);
}

void Texture2DView::Release()
{
	m_pTexture2D->Release();

	if(m_pRenderTarget)
		m_pRenderTarget->Release();

	if(m_pShaderResource)
		m_pShaderResource->Release();
}
