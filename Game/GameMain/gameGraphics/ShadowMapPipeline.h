#pragma once
#pragma once

#include "../../Graphics/includes/Pipeline.h"
#include <DirectXMath.h>

namespace graphics
{
	struct SHADOW_MAP_PIPELINE_DESC
	{
		UINT PixelsWidth;
		float Width, Height, NearPlane, FarPlane;
	};

	struct SHADOW_MAP_PIPELINE_DATA
	{
		DirectX::XMFLOAT4X4 ViewMatrix;
	};

	class ShadowMapPipeline : public GraphicsPipeline
	{
	public:
		ShadowMapPipeline();
		virtual ~ShadowMapPipeline() override;

		virtual HRESULT Initialize(ID3D11Device4* pDevice4, const void* pDescription) override;
		virtual void Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData) override;
		virtual void Begin(ID3D11DeviceContext4* pContext4) override;

		virtual void PreExecute(
			ID3D11DeviceContext4* pContext4,
			ID3D11VertexShader* pVertexShader,
			ID3D11PixelShader* pPixelShader) override;

		virtual void End(ID3D11DeviceContext4* pContext4, RenderManagerData* pData) override;


	private:
		virtual void Destroy() override;

		UINT m_pixelsWidth;

		ID3D11Buffer* m_pMatrixBuffers[2];

		ID3D11Texture2D* m_pDepthTexture;
		ID3D11DepthStencilView* m_pDepthBuffer;
		ID3D11ShaderResourceView* m_pDepthResource;

		ID3D11SamplerState* m_pSamplerState;
	};
}