#pragma once

#include "../../Graphics/includes/Pipeline.h"
#include <DirectXMath.h>

namespace graphics
{
	struct FAKE_STENCIL_PIPELINE_DESC
	{
		UINT ClientWidth, ClientHeight;
		float Fov, NearPlane, FarPlane;
	};

	struct FAKE_STENCIL_PIPELINE_DATA
	{
		DirectX::XMFLOAT4X4 ViewMatrix;
		float Red, Green, Blue;
	};

	class FakeStencilPipeline : public GraphicsPipeline
	{
	public:
		FakeStencilPipeline();
		virtual ~FakeStencilPipeline() override;

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

		UINT mClientWidth, mClientHeight;

		ID3D11PixelShader* mpArmyShader;

		ID3D11Texture2D* mpFakeStencilTexture;

		ID3D11RenderTargetView* mpRenderTarget;
		ID3D11ShaderResourceView* mpFakeStencilSRV;


	};
}