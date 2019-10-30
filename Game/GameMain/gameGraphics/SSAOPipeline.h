#pragma once
#pragma once

#include "../../Graphics/includes/Pipeline.h"
#include <DirectXMath.h>

namespace graphics
{
	struct SSAO_PIPELINE_DESC
	{
		UINT Width, Height;

		void* pRandomNormals;
		UINT ByteWidthNormals;
	};

	struct SSAO_PIPELINE_DATA
	{
	};

	class SSAOPipeline : public GraphicsPipeline
	{
	public:
		SSAOPipeline();
		virtual ~SSAOPipeline() override;

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

		ID3D11RenderTargetView* mpSSAOTarget;
		ID3D11ShaderResourceView* mpSSAOResource;

		ID3D11ShaderResourceView* mpRandomNormals;

		UINT mWidth, mHeight;
	};
}