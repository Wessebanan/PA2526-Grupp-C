uint4 unpack(const uint packedData)
{
	uint4 unpacked_data;

	unpacked_data.x = (packedData) >> 24;
	unpacked_data.y = (packedData & 0x00ff0000) >> 16;
	unpacked_data.z = (packedData & 0x0000ff00) >> 8;
	unpacked_data.w = (packedData & 0x000000ff);

	return unpacked_data;
}

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
		depth - 0.003f);
}

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
	float4 sun_color = unpack(gSunData) / 255.0f;
	float illu = dot(gSunDirection, normalize(input.normal));

	float in_shadow = shadow(input.sunPos.xy, input.sunPos.z);

	float3 finalColor = input.color;

	return float4(finalColor.xyz * in_shadow + finalColor.xyz * 0.1f, 1.0f);
}