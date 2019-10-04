#include "../..//includes/rendering/RenderTechnique.h"
#include "../..//includes/rendering/TransformationTechnique.h"

namespace rendering
{
	/*
		initialize	(set static data)	-(Is followed by an upload of static data)
		update		(set dynamic data)	-(Is followed by an upload of dynamic data)
		set & draw	(set state)			-(Is followed by a draw)
	*/

	const std::string gVertexShader = R"(

	struct PerObjectData
	{
		float4x4 World;
	}; 

	cbuffer gTransformation : register (b0)
	{
		PerObjectData gMesh[4];
	};

	cbuffer gCam : register (b1)
	{
		float4x4 gView;
	};

	cbuffer gPer : register (b2)
	{
		float4x4 gPerspective;
	};

	struct VSOUT
	{
		float4 pos	    : SV_POSITION;
		float2 uv		: UV0;
	};

	VSOUT main(
		float3 pos		: POSITION0, 
		float2 uv		: UV0,
		uint instance	: INDEX0)
	{
		VSOUT output;

		float4x4 wvp = mul(gPerspective, mul(gView, gMesh[instance].World));

		output.pos	= mul(wvp, float4(pos, 1.0f));
		output.uv	= uv;
	
		return output;
	}	

	)";

	const std::string gPixelShader = R"(

	struct PSIN
	{
		float4 pos	    : SV_POSITION;
		float2 uv		: UV0;
	};

	float4 main(PSIN input) : SV_TARGET
	{ 
		return float4(input.uv.xy, 1.0f, 1.0f);
	}	

	)";

	static TransformationTechniqueData* gpData = NULL;

	template<>
	void RenderTechnique::Construct<RENDER_TRANSFORMATION>(
		RENDER_TECHNIQUE_DESC& rDescription)
	{
		rDescription.ByteWidth = sizeof(TransformationTechniqueData);
		rDescription.ppData = (void**)& gpData;
	}

	template<>
	void RenderTechnique::Initialize<RENDER_TRANSFORMATION>(
		graphics::DeviceInterface* pDevice)
	{
		pDevice->CreateGraphicsPipeline(
			gVertexShader,
			gPixelShader,
			&gpData->pPipeline);
	}

	template<>
	void RenderTechnique::Update<RENDER_TRANSFORMATION>(
		graphics::RenderContext* pContext)
	{
	}

	template<>
	void RenderTechnique::Set<RENDER_TRANSFORMATION>(
		graphics::RenderContext* pContext)
	{
		pContext->SetGraphicsPipeline(gpData->pPipeline);
	}

	template<>
	void RenderTechnique::Deconstruct<RENDER_TRANSFORMATION>(
		graphics::DeviceInterface* pDevice)
	{
		pDevice->DeleteGraphicsPipeline(gpData->pPipeline);
	}

	template<>
	void* RenderTechnique::GetData<RENDER_TRANSFORMATION>()
	{
		return gpData;
	}
}