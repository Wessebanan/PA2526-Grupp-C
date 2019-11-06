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
		(1.0f + pos.x) * 0.5f,
		(1.0f - pos.y) * 0.5f
		);

	return gShadowMap.SampleCmpLevelZero(
		gSmpCmp,
		shadowMapUV,
		depth - 0.003f);
}

struct PSIN
{
	float4 pos			: SV_POSITION;
	float4 sunPos		: POSITION1;
	float4 color		: COLOR0;
	float4 normal		: NORMAL0;
};

float4 main(PSIN input) : SV_TARGET
{
	float in_shadow = shadow(input.sunPos.xy, input.sunPos.z);

	float3 up = float3(0.0f, 1.0f, 0.0f);
	float is_up = dot(up, normalize(input.normal.xyz));

	float3 ambient = input.color.rgb * 0.1f;
	float3 diffuse = input.color.rgb * in_shadow * 0.8f * clamp((is_up > 0.9f), 0.8f, 1.0f);

	return float4(ambient + diffuse, 0.0f);
}