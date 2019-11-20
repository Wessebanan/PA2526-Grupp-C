Texture2D<float3> gColorBuffer	: register(t1);
Texture2D<float> gOcclusionMap	: register(t3);

SamplerState gSampler : register (s2);

struct PSIN
{
	float4 pos	: SV_POSITION;
	float2 uv	: TEXCOORD0;
};

float4 main(PSIN input) : SV_TARGET
{
	const float3 color		= gColorBuffer.Sample(gSampler, input.uv).rgb;
	const float3 occlusion	= pow((1.0f - gOcclusionMap.Sample(gSampler, input.uv).rrr), 4.0f);

	return float4(occlusion * color, 1.0f);
}