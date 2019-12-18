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

		//mpDepthBuffer = p_desc->pDepthBuffer;
		mpBuffers[0] = p_desc->pViewMatrixBuffer;
		mpBuffers[1] = p_desc->pProjMatrixBuffer;
		mpBuffers[2] = p_desc->pInvProjMatrixBuffer;

		p_desc->pDepthBuffer->GetResource((ID3D11Resource**)&mpDepthTexture);

		{
			// Sample depth for SSAO
			D3D11_SHADER_RESOURCE_VIEW_DESC shader_desc = {};
			shader_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shader_desc.Format = DXGI_FORMAT_R32_FLOAT;
			shader_desc.Texture2D.MipLevels = 1;
			shader_desc.Texture2D.MostDetailedMip = 0;

			pDevice4->CreateShaderResourceView(
				mpDepthTexture,
				&shader_desc,
				&mpDepthBufferCopySRV);

			// To create depth buffer to later be used for read only
			D3D11_DEPTH_STENCIL_VIEW_DESC depth_desc = {};
			depth_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depth_desc.Format = DXGI_FORMAT_D32_FLOAT;
			depth_desc.Texture2D.MipSlice = 0;
			depth_desc.Flags = 0;

			pDevice4->CreateDepthStencilView(
				mpDepthTexture, 
				&depth_desc,
				&mpDepthBuffer);
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
		pContext4->ClearDepthStencilView(mpDepthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);

		ID3D11ShaderResourceView* p_null_view = NULL;
		pContext4->PSSetShaderResources(5, 1, &p_null_view);

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
		//pContext4->CopyResource(mpDepthTextureCopy, mpDepthTexture);
		pContext4->OMSetRenderTargets(0, NULL, NULL);
		pContext4->PSSetShaderResources(5, 1, &mpDepthBufferCopySRV);
		pContext4->PSSetConstantBuffers(0, 1, &mpBuffers[2]);
	}

	void DepthPrePassPipeline::Destroy()
	{
		//mpDepthTextureCopy->Release();
		mpDepthBufferCopySRV->Release();
		mpDepthTexture->Release();
	}
}