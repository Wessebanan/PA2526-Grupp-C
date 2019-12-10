#pragma once

#include "../../Graphics/includes/Pipeline.h"
#include <DirectXMath.h>

namespace graphics
{
	struct FORWARD_RENDERING_PIPELINE_DESC
	{
		// __ In __
		UINT ClientWidth, ClientHeight;
		float Fov, NearPlane, FarPlane;
		float ClearColor[3];

		// __ Out __
		ID3D11DepthStencilView* pDepthBuffer;
		ID3D11Buffer* pViewMatrixBuffer;
		ID3D11Buffer* pProjMatrixBuffer;
		ID3D11Buffer* pInvProjMatrixBuffer;
	};

	struct FORWARD_RENDERING_PIPELINE_DATA
	{
		DirectX::XMFLOAT4X4 ViewMatrix;
	};

	class ForwardRenderingPipeline : public GraphicsPipeline
	{
	public:
		ForwardRenderingPipeline();
		virtual ~ForwardRenderingPipeline() override;

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

		ID3D11Buffer* mpMatrixBuffers[3];

		ID3D11RenderTargetView* mpRenderTarget;

		ID3D11Texture2D* mpDepthTexture;
		ID3D11DepthStencilView* mpDepthBuffer;
		ID3D11DepthStencilState* mpDepthStencilState;

		ID3D11SamplerState* mpSamplerState;

		float mClearColor[3];

		ID3D11ShaderResourceView* mpRandomNormals;
	};
}