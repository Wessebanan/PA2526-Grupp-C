#include "../..//includes/rendering/RenderTechnique.h"
#include "../..//includes/rendering/SceenSpaceTechnique.h"

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
		float4 Pos;
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
		float4 pos	: SV_POSITION;
	};

	VSOUT main(
		float3 pos : POSITION0, 
		uint instance : INDEX0)
	{
		VSOUT output;

		output.pos	= float4(pos.xyz + gMesh[instance].Pos.xyz, 1.0f);
	
		return output;
	}	

	)";

	const std::string gPixelShader = R"(

	struct PSIN
	{
		float4 pos	: SV_POSITION;
	};

	float4 main(PSIN input) : SV_TARGET
	{ 
		return float4(0.0f, 0.0f, 1.0f, 1.0f);
	}	

	)";

	static ScreenSpaceTechniqueData* gpData = NULL;

	template<>
	void RenderTechnique::Construct<RENDER_SCREEN_SPACE>(
		RENDER_TECHNIQUE_DESC& rDescription)
	{
		rDescription.ByteWidth = sizeof(ScreenSpaceTechniqueData);
		rDescription.ppData = (void**)& gpData;
	}

	template<>
	void RenderTechnique::Initialize<RENDER_SCREEN_SPACE>(
		graphics::DeviceInterface* pDevice)
	{
		pDevice->CreateGraphicsPipeline(
			gVertexShader,
			gPixelShader,
			&gpData->pPipeline);
	}

	template<>
	void RenderTechnique::Update<RENDER_SCREEN_SPACE>(
		graphics::RenderContext* pContext)
	{
	}

	template<>
	void RenderTechnique::Set<RENDER_SCREEN_SPACE>(
		graphics::RenderContext* pContext)
	{
		pContext->SetGraphicsPipeline(gpData->pPipeline);
	}

	template<>
	void RenderTechnique::Deconstruct<RENDER_SCREEN_SPACE>(
		graphics::DeviceInterface* pDevice)
	{
		pDevice->DeleteGraphicsPipeline(gpData->pPipeline);
	}

	template<>
	void* RenderTechnique::GetData<RENDER_SCREEN_SPACE>()
	{
		return gpData;
	}
}