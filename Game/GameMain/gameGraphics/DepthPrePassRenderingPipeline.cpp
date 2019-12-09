#include "ForwardRenderingPipeline.h"
#include "DepthPrePassRenderingPipeline.h"

namespace graphics
{
	DepthPrePassPipeline::DepthPrePassPipeline()
	{
	}

	DepthPrePassPipeline::~DepthPrePassPipeline()
	{
	}

	HRESULT DepthPrePassPipeline::Initialize(ID3D11Device4* pDevice4, const void* pDescription)
	{
		DEPTH_PRE_PASS_PIPELINE_DESC* p_desc = (DEPTH_PRE_PASS_PIPELINE_DESC*)pDescription;

		mClientWidth = p_desc->ClientWidth;
		mClientHeight = p_desc->ClientHeight;

		mpDepthBuffer = p_desc->pDepthBuffer;
		mpBuffers[0] = p_desc->pViewMatrixBuffer;
		mpBuffers[1] = p_desc->pProjMatrixBuffer;
		mpBuffers[2] = p_desc->pInvProjMatrixBuffer;

		mpDepthBuffer->GetResource((ID3D11Resource**)&mpDepthTexture);

		{
			D3D11_TEXTURE2D_DESC texture_desc = { 0 };
			texture_desc.Width = p_desc->ClientWidth;
			texture_desc.Height = p_desc->ClientHeight;
			texture_desc.ArraySize = 1;
			texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texture_desc.CPUAccessFlags = 0;
			texture_desc.Format = DXGI_FORMAT_R32_FLOAT;
			texture_desc.SampleDesc = { 1, 0 };
			texture_desc.Usage = D3D11_USAGE_DEFAULT;

			pDevice4->CreateTexture2D(&texture_desc, NULL, &mpDepthTextureCopy);

			D3D11_SHADER_RESOURCE_VIEW_DESC shader_desc = {};
			shader_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shader_desc.Format = DXGI_FORMAT_R32_FLOAT;
			shader_desc.Texture2D.MipLevels = 1;
			shader_desc.Texture2D.MostDetailedMip = 0;

			pDevice4->CreateShaderResourceView(
				mpDepthTextureCopy,
				&shader_desc,
				&mpDepthBufferCopySRV);
		}

		return S_OK;
	}

	void DepthPrePassPipeline::Clear(ID3D11DeviceContext4* pContext4)
	{
	}

	void DepthPrePassPipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
	{
	}

	void DepthPrePassPipeline::Begin(ID3D11DeviceContext4* pContext4)
	{
		graphics::SetViewport(pContext4, 0, 0, mClientWidth, mClientHeight);
		pContext4->OMSetRenderTargets(0, NULL, mpDepthBuffer);

		pContext4->VSSetConstantBuffers(1, 2, mpBuffers);
	}

	void DepthPrePassPipeline::PreProcess(
		ID3D11DeviceContext4* pContext4, 
		ID3D11VertexShader* pVertexShader, 
		ID3D11PixelShader* pPixelShader)
	{
		pContext4->VSSetShader(pVertexShader, NULL, 0);
		pContext4->PSSetShader(NULL, NULL, 0);
	}

	void DepthPrePassPipeline::End(ID3D11DeviceContext4* pContext4)
	{
		pContext4->CopyResource(mpDepthTextureCopy, mpDepthTexture);
		pContext4->PSSetShaderResources(5, 1, &mpDepthBufferCopySRV);
		pContext4->PSSetConstantBuffers(0, 1, &mpBuffers[2]);
	}

	void DepthPrePassPipeline::Destroy()
	{
		mpDepthTextureCopy->Release();
		mpDepthBufferCopySRV->Release();
		mpDepthTexture->Release();
	}
}