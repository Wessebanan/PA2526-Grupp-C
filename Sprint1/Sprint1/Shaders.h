#pragma once
#include <string>

const std::string gVertexShader = R"(
cbuffer gTransformation : register (b0)
{
	float4x4 gWorld[3];
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
	float3 nor  : NORMAL;
	float2 uv	: UV0;
	float4 worldPos : POSITION;
	uint instance : INDEX0;
};
VSOUT main(
	float3 pos : POSITION0, 
	float3 normal : NORMAL0,
	float2 uv : UV0,
	uint instance : INDEX0)
{
	VSOUT output;
	float4x4 wvp = mul(gPerspective, mul(gView, gWorld[instance]));
	output.pos	= mul(wvp, float4(pos, 1.0f));
	output.uv	= uv;
	output.nor	= normal;
	output.worldPos  = mul(gWorld[0], float4(pos, 1.0f));
	output.instance = instance;
	return output;
}	
)";

const std::string gPixelShader = R"(
struct PSIN
{
	float4 pos	: SV_POSITION;
	float3 nor  : NORMAL;
	float2 uv	: UV0;
	float4 worldPos : POSITION;
	uint instance : INDEX0;
};
float4 main(PSIN input) : SV_TARGET
{ 
	float4 finalColor = float4(input.nor.x, input.nor.y, input.nor.z, 1.0f);
	if(finalColor.z <= -0.93f) {
		finalColor = float4(140.0f/255.0f, 125.0f/255.0f, 112.0f/255.0f, 1.0f) * (-((float)input.instance - 1024) / 712.0f);
		if(input.instance % 7 <= 1 || input.instance % 7 >= 6)
		{
			finalColor = float4(0.486f, 0.988f, 0.0f, 1.0f) * (-((float)input.instance - 1024) / 712.0f);
		}
		if(input.instance % 19 == 0)
		{
			finalColor = float4(0.0f, 0.467f, 0.745f, 1.0f);
		}
	}	
	else if (finalColor.z <= -0.50f) {
		finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		finalColor = float4(140.0f/455.0f * input.nor.x, 125.0f/455.0f * input.nor.y, 112.0f/455.0f * input.nor.z, 1.0f);
	}
	return finalColor;
}
)";