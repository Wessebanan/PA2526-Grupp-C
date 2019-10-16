#include "../..//includes/rendering/RenderTechnique.h"
#include "../..//includes/rendering/SkinningTechnique.h"
#include "../..//includes/rendering/HLSLFunctions.h"

namespace rendering
{
	/*
		initialize	(set static data)	-(Is followed by an upload of static data)
		update		(set dynamic data)	-(Is followed by an upload of dynamic data)
		set & draw	(set state)			-(Is followed by a draw)
	*/

	const std::string gVertexShader = R"(

	static const int MAX_AFFECTING_BONES = 4;
	static const int MAX_BONE_MATRICES = 63;

	struct PerObjectData
	{
		float4x4 World;
		float4x4 gBoneTransforms[MAX_BONE_MATRICES];
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

struct SKIN_VSIn
{
	float3 Pos	 : POSITION0;
	float3 Normal : NORMAL0;
	float2 UV		: UV0;
	uint instance : INDEX0;
	float3 blendWeights : BLENDWEIGHT0;
	uint4 blendIndices : BLENDINDICES0;
};

struct VSOut
{
		float4 pos			: SV_POSITION;
		float4 sunPos		: POSITION1;

		float3 color		: COLOR0;
		float3 normal		: NORMAL0;
};


VSOut main(SKIN_VSIn input)
{
	VSOut output = (VSOut)0;
	float4x4 wvpCam = mul(gPerspective, mul(gView, gMesh[input.instance].World));
	float4x4 wvpSun	= mul(gOrtographicsSun, mul(gViewSun, gMesh[input.instance].World));

	float lastWeight = 1.0f;
	float4 v = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float3 norm = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < MAX_AFFECTING_BONES - 1; ++i)
	{
		v		+= input.blendWeights[i] * mul(float4(input.Pos, 1.0f), gMesh[input.instance].gBoneTransforms[input.blendIndices[i]]);
		norm	+= (input.blendWeights[i] * mul(float4(input.Normal, 1.0f), gMesh[input.instance].gBoneTransforms[input.blendIndices[i]])).xyz;
		lastWeight -= input.blendWeights[i];
	}
	// Apply last weight
	v += lastWeight * mul(float4(input.Pos, 1.0f), gMesh[input.instance].gBoneTransforms[input.blendIndices[MAX_AFFECTING_BONES - 1]]);
	norm += (lastWeight * mul(float4(input.Normal, 1.0f), gMesh[input.instance].gBoneTransforms[input.blendIndices[MAX_AFFECTING_BONES - 1]])).xyz;
	v.w = 1.0f;

	output.pos		= mul(wvpCam, v);
	output.sunPos	= mul(wvpSun, float4(input.Pos, 1.0f));
	output.color	= float3(1.0f, 1.0f, 1.0f);
	output.normal	= mul(gMesh[input.instance].World, float4(input.Normal, 0.0f)).xyz;

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
	};

	float4 main(PSIN input) : SV_TARGET
	{
		float4 sun_color = unpack(gSunData) / 256.0f;
		float illu = dot(gSunDirection, normalize(input.normal));

		float in_shadow = shadow(input.sunPos.xy, input.sunPos.z);

		float3 finalColor = input.color * sun_color.rgb * sun_color.a;

		//return float4(input.normal, 1.0f);
		return float4(finalColor.xyz * in_shadow + finalColor.xyz * 0.1f, 1.0f);
	}	

	)";

	static SkinningTechniqueData* gpData = NULL;

	template<>
	void RenderTechnique::Construct<RENDER_SKINNING>(
		RENDER_TECHNIQUE_DESC& rDescription)
	{
		rDescription.ByteWidth = sizeof(SkinningTechniqueData);
		rDescription.ppData = (void**)&gpData;
	}

	template<>
	void RenderTechnique::Initialize<RENDER_SKINNING>(
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
	void RenderTechnique::Update<RENDER_SKINNING>(
		graphics::RenderContext* pContext)
	{
	}

	template<>
	void RenderTechnique::Set<RENDER_SKINNING>(
		graphics::RenderContext* pContext)
	{
		pContext->SetGraphicsPipeline(&gpData->pipeline);
	}

	template<>
	void RenderTechnique::Deconstruct<RENDER_SKINNING>(
		graphics::DeviceInterface* pDevice)
	{
		pDevice->DeleteGraphicsPipeline(&gpData->pipeline);
	}

	template<>
	void* RenderTechnique::GetData<RENDER_SKINNING>()
	{
		return gpData;
	}
}