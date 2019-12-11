#pragma once

#include "../../Graphics/includes/Pipeline.h"
#include <DirectXMath.h>

namespace graphics
{
	struct DEPTH_PRE_PASS_PIPELINE_DESC
	{
		UINT ClientWidth, ClientHeight;

		// __ In __
		ID3D11DepthStencilView* pDepthBuffer;
		ID3D11Buffer* pViewMatrixBuffer;
		ID3D11Buffer* pProjMatrixBuffer;
		ID3D11Buffer* pInvProjMatrixBuffer;
	};

	class DepthPrePassPipeline : public GraphicsPipeline
	{
	public:
		DepthPrePassPipeline();
		virtual ~DepthPrePassPipeline() override;

		virtual HRESULT Initialize(ID3D11Device4* pDevice4, const void* pDescription) override;
		virtual void Clear(ID3D11DeviceContext4* pContext4) override;
		virtual void Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData) override;
		virtual void Begin(ID3D11DeviceContext4* pContext4) override;

		virtual void PreProcess(
			ID3D11DeviceContext4* pContext4,
			ID3D11VertexShader* pVertexShader,
			ID3D11PixelShader* pPixelShader) override;

		virtual void End(ID3D11DeviceContext4* pContext4) override;


	private:
		virtual void Destroy() override;

		UINT mClientWidth, mClientHeight;

		ID3D11Texture2D* mpDepthTextureCopy;
		ID3D11ShaderResourceView* mpDepthBufferCopySRV;

		ID3D11Texture2D* mpDepthTexture;
		ID3D11DepthStencilView* mpDepthBuffer;

		ID3D11Buffer* mpBuffers[3];

		float mClearColor[3];
	};
}