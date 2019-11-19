#pragma once

#include "../../Graphics/includes/Pipeline.h"
#include <DirectXMath.h>

namespace graphics
{
	struct OUTLINE_PIPELINE_DESC
	{
		ID3D11ShaderResourceView* fakeStencilSRV;
	};

	struct OUTLINE_PIPELINE_DATA
	{
		
	};

	class OutlinePipeline : public GraphicsPipeline
	{
	public:
		OutlinePipeline();
		virtual ~OutlinePipeline() override;

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

		ID3D11ShaderResourceView* mpFakeStencilSRV;

		ID3D11VertexShader* mpFullScreenQuadVS;
		ID3D11PixelShader* mpOutlinePS;


	};
}