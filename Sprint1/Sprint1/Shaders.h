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
};
VSOUT main(
	float3 pos : POSITION0, 
	float3 normal : NORMAL0,
	float2 uv : UV0)
{
	VSOUT output;
	float4x4 wvp = mul(gPerspective, mul(gView, gWorld[0]));
	output.pos	= mul(wvp, float4(pos, 1.0f));
	output.uv	= uv;
	output.nor	= normal;
	
	return output;
}	
)";

const std::string gPixelShader = R"(
struct PSIN
{
	float4 pos	: SV_POSITION;
	float3 nor  : NORMAL;
	float2 uv	: UV0;
};
float4 main(PSIN input) : SV_TARGET
{ 
	return float4(input.nor.x, input.nor.y, input.nor.z, 1.0f);
}	
)";