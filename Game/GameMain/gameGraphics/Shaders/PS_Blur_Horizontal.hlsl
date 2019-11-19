
Texture2D<float4> gNormalMap	: register (t2);
Texture2D<float> gOcclusionMap	: register (t3);

SamplerState gSampler : register (s2);

struct PSIN
{
	float4 pos	: SV_POSITION;
	float2 uv	: TEXCOORD0;
};

float4 GetNormalAndDepth(const float2 uv)
{
	const float4 data = gNormalMap.SampleLevel(gSampler, uv, 0.0f);

	return float4(normalize(data.xyz), data.w);
}

#define TEXEL_WIDTH		(1.0f / (960.0f))
#define TEXEL_HEIGHT	(1.0f / (540.0f))

float Blur(
	const float2 dir, 
	const float2 uv,
	const float4 center_nor_depth, 
	const float center_occlusion)
{
	int blur_radius = 5;
	float weights[11] =
	{
		0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f
	};

	float total_weight	= weights[5];
	float occlusion		= center_occlusion * weights[5];

	[unroll]
	for (int i = -blur_radius; i <= blur_radius; i++)
	{
		if (i == 0) continue;

		const float2 tex = i * dir + uv;
		const float4 nor_depth	= GetNormalAndDepth(tex);

		if (abs(nor_depth.w - center_nor_depth.w) <= 0.1f &&
			dot(center_nor_depth.xyz, nor_depth.xyz) > 0.5f)
		{
			const float w = weights[i + blur_radius];

			occlusion += w * gOcclusionMap.Sample(gSampler, tex, 0.0f).r;
			total_weight += w;
		}
	}

	return occlusion / total_weight;
}

float main(PSIN input) : SV_TARGET
{
	float center_occlusion		= gOcclusionMap.SampleLevel(gSampler, input.uv, 0.0f).r;
	float4 center_nor_depth		= GetNormalAndDepth(input.uv);

	float blur	= Blur(
		float2(TEXEL_WIDTH, 0.0f) /** (50.0f - center_nor_depth.w) / 50.0f*/,
		input.uv, 
		center_nor_depth,
		center_occlusion);

	return saturate(blur);
}