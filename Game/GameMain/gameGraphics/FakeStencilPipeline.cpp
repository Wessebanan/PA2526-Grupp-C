#include "FakeStencilPipeline.h"
#include "GraphicsInterface.h"
#include "../gameUtility/UtilityGraphics.h"

namespace graphics {

	graphics::FakeStencilPipeline::FakeStencilPipeline()
	{

	}

	graphics::FakeStencilPipeline::~FakeStencilPipeline()
	{
	}

	HRESULT graphics::FakeStencilPipeline::Initialize(ID3D11Device4* pDevice4, const void* pDescription)
	{
		HRESULT hr;
		D3D11_TEXTURE2D_DESC descOutlineBase;

		FAKE_STENCIL_PIPELINE_DESC* pDesc = (FAKE_STENCIL_PIPELINE_DESC*)pDescription;
		// Create the texture for the fake stencil, first for rendering the "stencil" and in the second
		// pass as a resource for drawing the actual outline
		descOutlineBase.Width = 1920;
		descOutlineBase.Height = 1080;
		descOutlineBase.MipLevels = 1;
		descOutlineBase.ArraySize = 1;
		descOutlineBase.Format = DXGI_FORMAT_R32_TYPELESS;
		descOutlineBase.SampleDesc.Count = 1;
		descOutlineBase.SampleDesc.Quality = 0;
		descOutlineBase.Usage = D3D11_USAGE_DEFAULT;
		descOutlineBase.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		descOutlineBase.CPUAccessFlags = 0;
		descOutlineBase.MiscFlags = 0;
		hr = pDevice4->CreateTexture2D(&descOutlineBase, NULL, &this->mpFakeStencilTexture);
		if (FAILED(hr))
		{
			MessageBox(0, "CreateTexture2D for mpFakeStencilTexture failed", 0, 0);
			return E_ABORT;
		}

		D3D11_RENDER_TARGET_VIEW_DESC outlineRTVDesc;
		D3D11_TEX2D_RTV tex2drtv;
		ZeroMemory(&tex2drtv, sizeof(D3D11_TEX2D_RTV));
		tex2drtv.MipSlice = 0;
		outlineRTVDesc.Texture2D = tex2drtv;
		outlineRTVDesc.Format = DXGI_FORMAT_R32_UINT;
		outlineRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		hr = pDevice4->CreateRenderTargetView(this->mpFakeStencilTexture, &outlineRTVDesc, &mpRenderTarget);
		if (FAILED(hr))
		{
			MessageBox(0, "CreateRenderTargetView for mpRenderTarget failed", 0, 0);
			return E_ABORT;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC outlineBasesrvDesc;
		D3D11_TEX2D_SRV tex2doutlinesrv;
		tex2doutlinesrv.MostDetailedMip = 0;
		tex2doutlinesrv.MipLevels = 1;
		outlineBasesrvDesc.Texture2D = tex2doutlinesrv;
		outlineBasesrvDesc.Format = DXGI_FORMAT_R32_UINT;
		outlineBasesrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		hr = pDevice4->CreateShaderResourceView(
			mpFakeStencilTexture,
			&outlineBasesrvDesc,
			&this->mpFakeStencilSRV
		);

		// Create all the necessary shaders

		if (FAILED(hr))
		{
			MessageBox(0, "CreateShaderResourceView for mpFakeStencilSRV failed", 0, 0);
			return E_ABORT;
		}

		hr = graphics::CreatePixelShaderFromFile(pDevice4, GetShaderFilepath("PS_Army.cso").c_str(), &mpArmyShader);
		if (FAILED(hr))
		{
			MessageBox(0, "CreatePixelShaderFromFile for PS_Army.cso failed", 0, 0);
			return E_ABORT;
		}

		hr = graphics::CreatePixelShaderFromFile(pDevice4, GetShaderFilepath("PS_Outline.cso").c_str(), &mpOutlinePS);
		if (FAILED(hr))
		{
			MessageBox(0, "CreatePixelShaderFromFile for PS_Outline.cso failed", 0, 0);
			return E_ABORT;
		}

		hr = graphics::CreateVertexShaderFromFile(pDevice4, GetShaderFilepath("VS_Outline.cso").c_str(), &mpOutlineVS);
		if (FAILED(hr))
		{
			MessageBox(0, "CreatePixelShaderFromFile for VS_Outline.cso failed", 0, 0);
			return E_ABORT;
		}

		return S_OK;
	}

	void FakeStencilPipeline::Clear(ID3D11DeviceContext4* pContext4)
	{
	}

	void graphics::FakeStencilPipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
	{
		// Nothing needs updating here :)
	}

	void graphics::FakeStencilPipeline::Begin(ID3D11DeviceContext4* pContext4)
	{
		float clear[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		// Unset the stencil SRV if it was set as it can't be bound as
		// both a render target and a SRV at the same time
		pContext4->PSSetShaderResources(6, 1, nullSRV); 
		
		pContext4->OMSetRenderTargets(1, &this->mpRenderTarget, NULL);
		pContext4->ClearRenderTargetView(this->mpRenderTarget, clear);
	}

	void graphics::FakeStencilPipeline::PreProcess(ID3D11DeviceContext4* pContext4, ID3D11VertexShader* pVertexShader, ID3D11PixelShader* pPixelShader)
	{
		pContext4->VSSetShader(pVertexShader, NULL, 0);
		pContext4->PSSetShader(this->mpArmyShader, NULL, 0);
	}

	void graphics::FakeStencilPipeline::End(ID3D11DeviceContext4* pContext4)
	{
		// Perform the outline pass here because I'm naughty like that
		// The Draw(3,0) call renders a full screen triangle in the OutlineVS
		// The Pixel shader uses the SRV from the pipeline render pass which is
		// out "fake stencil" that is used to draw outlines
		ID3D11RenderTargetView* p_back_buffer;
		internal::GetBackBuffer(&p_back_buffer);
		pContext4->OMSetRenderTargets(1, &p_back_buffer, NULL);
		pContext4->VSSetShader(this->mpOutlineVS, NULL, 0);
		pContext4->PSSetShader(this->mpOutlinePS, NULL, 0);
		pContext4->PSSetShaderResources(6, 1, &this->mpFakeStencilSRV);
		pContext4->Draw(3, 0);
		pContext4->VSSetShader(NULL, NULL, 0);
		pContext4->PSSetShader(NULL, NULL, 0);
	}

	ID3D11ShaderResourceView* FakeStencilPipeline::GetFakeStencilSRVPtr()
	{
		return this->mpFakeStencilSRV;
	}

	void graphics::FakeStencilPipeline::Destroy()
	{
	}


}
