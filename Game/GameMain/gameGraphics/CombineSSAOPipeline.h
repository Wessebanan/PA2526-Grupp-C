#pragma once

#pragma once
#pragma once

#include "../../Graphics/includes/Pipeline.h"
#include <DirectXMath.h>

namespace graphics
{
	struct COMBINE_PIPELINE_DESC
	{
		UINT Width, Height;
	};

	struct COMBINE_PIPELINE_DATA
	{
	};

	class CombinePipeline : public GraphicsPipeline
	{
	public:
		CombinePipeline();
		virtual ~CombinePipeline() override;

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

		ID3D11RenderTargetView* mpBackBuffer;
		UINT mWidth, mHeight;
	};
}