#pragma once

#include "../includes/Pipeline.h"
#include <DirectXMath.h>

namespace graphics
{
	struct FORWARD_RENDERING_PIPELINE_DESC
	{
		UINT ClientWidth, ClientHeight;
		float Fov, NearPlane, FarPlane;
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
		virtual void Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData) override;
		virtual void Begin(ID3D11DeviceContext4* pContext4) override;

		virtual void PreProcess(
			ID3D11DeviceContext4* pContext4,
			ID3D11VertexShader* pVertexShader,
			ID3D11PixelShader* pPixelShader) override;

		virtual void End(ID3D11DeviceContext4* pContext4) override;


	private:
		virtual void Destroy() override;

		UINT m_clientWidth, m_clientHeight;

		ID3D11Buffer* m_pMatrixBuffers[2];

		ID3D11Texture2D* m_pDepthTexture;
		ID3D11DepthStencilView* m_pDepthBuffer;

		ID3D11RenderTargetView* m_pRenderTarget;
	};
}