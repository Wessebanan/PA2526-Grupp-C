#pragma once

#include <string>

namespace hlsl_functions
{
	/*
		input: 
			packedData 
				HO - (uint8 | uint8 | uint8 | uint8) - LO

		output:
			uint4
				x: HO bit
				w: LO bit
	*/
	const char* gUnpack = R"(
		uint4 unpack(const uint packedData)
		{
			uint4 unpacked_data;
	
			unpacked_data.x = (packedData) >> 24;
			unpacked_data.y = (packedData & 0x00ff0000) >> 16;
			unpacked_data.z = (packedData & 0x0000ff00) >>  8;
			unpacked_data.w = (packedData & 0x000000ff);

			return unpacked_data;
		}
	)";

	/*
	
		input:
			float2 pos
			float depth

		output:
			float shadow : 
				0 -> In Shadow
				1 -> In Light
	
	*/
	const char* gShadow = R"(

	Texture2D<float> gShadowMap			: register (t0);
	SamplerComparisonState gSmpCmp		: register (s0);

	float shadow(const float2 pos, const float depth)
	{
		float2 shadowMapUV = float2(
			(pos.x + 1.0f) * 0.5f,
			(1.0f - pos.y) * 0.5f
		);

		return gShadowMap.SampleCmpLevelZero(
					gSmpCmp, 
					shadowMapUV, 
					depth);
	}

	)";

}