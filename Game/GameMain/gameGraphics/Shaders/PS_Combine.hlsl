Texture2D<float4> gColorBuffer	: register(t1);
Texture2D<float> gOcclusionMap : register(t2);

SamplerState gSampler : register (s2);

struct PSIN
{
	float4 pos	: SV_POSITION;
	float2 uv	: TEXCOORD0;
};

float4 main(PSIN input) : SV_TARGET
{
	return 
		float4(
		gColorBuffer.Sample(gSampler, input.uv).rgb 
		- gOcclusionMap.Sample(gSampler, input.uv).rrr, 1.0f);
}