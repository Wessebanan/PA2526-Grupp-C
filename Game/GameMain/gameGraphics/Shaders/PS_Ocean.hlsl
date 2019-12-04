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

	float offset = 1.0f / (1024.0f);
	float illumination = 0.0f;

	const float shadow_bias = 0.0045f;

	const float middle_weight = 0.25f;
	const float side_weight_total = 1.0f - middle_weight;
	const float side_weight = side_weight_total / 8.0f;

	const float weigth[3][3] =
	{
		{side_weight,  side_weight,   side_weight},
		{side_weight,  middle_weight, side_weight},
		{side_weight,  side_weight,   side_weight},
	};

	[unroll]
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			illumination += gShadowMap.SampleCmp(
				gSmpCmp,
				shadowMapUV + float2(x, y) * offset,
				depth - shadow_bias) * weigth[x + 1][y + 1];
		}
	}

	return saturate(2.5f * illumination - 1.5f);
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

	float3 ambient =  input.color.rgb * 0.1f;
	float3 diffuse = input.color.rgb * in_shadow * clamp((is_up > 0.9f), 0.8f, 1.0f);

	return float4(ambient + diffuse * 0.8f, 1.0f);
}