#include "../..//includes/rendering/RenderTechnique.h"
#include "../..//includes/rendering/DefaultTechnique.h"
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
		float3 Pos;
		uint   color;
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
		float3 modelPos		: POSITION2;
	};

	VSOUT main(
		float3 pos		: POSITION0, 
		float3 normal	: NORMAL0,
		uint instance	: INDEX0)
	{
		VSOUT output;
	
		float4 worldPos	= float4(pos.xyz + gMesh[instance].Pos.xyz, 1.0f);

		output.pos		= mul(gPerspective, mul(gView, worldPos));
		output.sunPos	= mul(gOrtographicsSun, mul(gViewSun, worldPos));

		uint4 u			= unpack(gMesh[instance].color);
		float4 clr		= float4(u) / 256.0f;
		output.color	= clr.rgb;

		output.normal	= normal;
		output.modelPos	= pos;

		return output;
	}	

	)";

	const std::string gPixelShader = R"(

	cbuffer SunData : register (b0)
	{
		float3 gSunDirection;
		uint gSunData;
	}

	struct PSIN
	{
		float4 pos			: SV_POSITION;
		float4 sunPos		: POSITION1;

		float3 color		: COLOR0;
		float3 normal		: NORMAL0;
		float3 modelPos		: POSITION2;
	};

	float4 main(PSIN input) : SV_TARGET
	{
		float4 sun_color = unpack(gSunData) / 256.0f;
		float illu = dot(gSunDirection, normalize(input.normal));

		float in_shadow = shadow(input.sunPos.xy, input.sunPos.z);

		float3 finalColor = input.color;// * sun_color.rgb * sun_color.a;
		
		
		//float c = clamp(1.0f -  length(input.modelPos) + 0.4f, 0.2f, 1.0f);
		//if(input.modelPos.y < -0.025f)
		//	c = 1.0f;
		
		float3 ambient = input.color * (1.0f - in_shadow) * 0.2f;
		float3 diffuse = finalColor * in_shadow;

		//return float4(input.normal, 1.0f);
		return float4(ambient + diffuse, 1.0f);
	}	

	)";

	static DefaultTechniqueData* gpData	= NULL;

	template<>
	void RenderTechnique::Construct<RENDER_DEFAULT>(
		RENDER_TECHNIQUE_DESC& rDescription)
	{
		rDescription.ByteWidth	= sizeof(DefaultTechniqueData);
		rDescription.ppData		= (void**)& gpData;
	}

	template<>
	void RenderTechnique::Initialize<RENDER_DEFAULT>(
		graphics::DeviceInterface* pDevice)
	{
		std::string vertex_shader;
		vertex_shader.append(hlsl_functions::gUnpack);
		vertex_shader.append(gVertexShader);

		std::string pixel_shader;
		pixel_shader.append(hlsl_functions::gUnpack);
		pixel_shader.append(hlsl_functions::gShadow);
		pixel_shader.append(gPixelShader);

		pDevice->CreateGraphicsPipeline(
			vertex_shader,
			pixel_shader,
			&gpData->pipeline);
	}

	template<>
	void RenderTechnique::Update<RENDER_DEFAULT>(
		graphics::RenderContext* pContext)
	{
	}

	template<>
	void RenderTechnique::Set<RENDER_DEFAULT>(
		graphics::RenderContext* pContext)
	{
		pContext->SetGraphicsPipeline(&gpData->pipeline);
	}

	template<>
	void RenderTechnique::Deconstruct<RENDER_DEFAULT>(
		graphics::DeviceInterface* pDevice)
	{
		pDevice->DeleteGraphicsPipeline(&gpData->pipeline);
	}

	template<>
	void* RenderTechnique::GetData<RENDER_DEFAULT>()
	{
		return gpData;
	}
}