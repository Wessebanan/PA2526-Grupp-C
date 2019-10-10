#include "../..//includes/rendering/RenderTechnique.h"
#include "../..//includes/rendering/TransformationTechnique.h"
#include "../..//includes/rendering/HLSLFunctions.h"

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

	cbuffer gSunCam : register (b3)
	{
		float4x4 gViewSun;
	};

	cbuffer gSunOrto : register (b4)
	{
		float4x4 gOrtographicsSun;
	};

	struct VSOUT
	{
		float4 pos			: SV_POSITION;
		float4 sunPos		: POSITION1;

		float3 color		: COLOR0;
		float3 normal		: NORMAL0;
	};

	VSOUT main(
		float3 pos		: POSITION0, 
		float3 normal	: NORMAL0,
		float2 uv		: UV0,
		uint instance	: INDEX0)
	{
		VSOUT output;

		float4x4 wvpCam = mul(gPerspective, mul(gView, gMesh[instance].World));
		float4x4 wvpSun	= mul(gOrtographicsSun, mul(gViewSun, gMesh[instance].World));

		output.pos		= mul(wvpCam, float4(pos, 1.0f));
		output.sunPos	= mul(wvpSun, float4(pos, 1.0f));

		output.normal	= mul(gMesh[instance].World, float4(normal, 0.0f)).xyz;
		output.color	= float3(1.0f, 1.0f, 1.0f);
	
		return output;
	}	

	)";

	const std::string gPixelShader = R"(

	cbuffer SunData : register (b0)
	{
		float3 gSunDirection;
		uint gSunData;
	};

	struct PSIN
	{
		float4 pos			: SV_POSITION;
		float4 sunPos		: POSITION1;

		float3 color		: COLOR0;
		float3 normal		: NORMAL0;
	};

	float4 main(PSIN input) : SV_TARGET
	{ 
		float4 sun_color = unpack(gSunData) / 256.0f;
		float illu = dot(gSunDirection, normalize(input.normal));

		float3 finalColor = input.color * sun_color.rgb * sun_color.a;

		float in_shadow = shadow(input.sunPos.xy, input.sunPos.z);
		//return float4(input.normal, 1.0f);
		return float4(finalColor.xyz * in_shadow + finalColor.xyz * 0.1f, 1.0f);
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
		std::string pixel_shader;
		pixel_shader.append(hlsl_functions::gUnpack);
		pixel_shader.append(hlsl_functions::gShadow);
		pixel_shader.append(gPixelShader);

		pDevice->CreateGraphicsPipeline(
			gVertexShader,
			pixel_shader,
			&gpData->pipeline);
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
		pContext->SetGraphicsPipeline(&gpData->pipeline);
	}

	template<>
	void RenderTechnique::Deconstruct<RENDER_TRANSFORMATION>(
		graphics::DeviceInterface* pDevice)
	{
		pDevice->DeleteGraphicsPipeline(&gpData->pipeline);
	}

	template<>
	void* RenderTechnique::GetData<RENDER_TRANSFORMATION>()
	{
		return gpData;
	}
}