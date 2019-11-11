#pragma once

#include "../../Graphics/includes/Pipeline.h"
#include <DirectXMath.h>

namespace graphics
{
	struct OCEAN_RENDERING_PIPELINE_DESC
	{
		void* pWorldMesh;
		UINT stride;
		UINT size;
	};

	struct OCEAN_RENDERING_PIPELINE_DATA
	{
		const float* pHeightBuffer;
		UINT ByteWidth;
	};

	class OceanRenderingPipeline : public StatePipeline
	{
	public:
		OceanRenderingPipeline();
		virtual ~OceanRenderingPipeline() override;

		virtual HRESULT Initialize(ID3D11Device4* pDevice4, const void* pDescription) override;
		virtual void Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData) override;

		virtual void SetState(ID3D11DeviceContext4* pContext4) override;

	private:
		virtual void Destroy() override;

		ID3D11Buffer* mpVertexBuffer;
		ID3D11Buffer* mpDataBuffer;
		ID3D11ShaderResourceView* mpVertexBufferView;
	};
}