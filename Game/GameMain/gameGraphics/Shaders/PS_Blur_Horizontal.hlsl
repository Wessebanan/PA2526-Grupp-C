
Texture2D<float4> gNormalMap	: register (t2);
Texture2D<float> gOcclusionMap	: register (t3);

SamplerState gSampler : register (s2);

struct PSIN
{
	float4 pos	: SV_POSITION;
	float2 uv	: TEXCOORD0;
};

float GetDepth(const float2 uv)
{
	return (gNormalMap.Sample(gSampler, uv).w);
}

float Blur(
	const float2 dir, 
	const float2 uv,
	const float center_depth, 
	const float center_occlusion)
{
	float blur_radius = 5.0f;
	float weights[11] =
	{
		0.05f, 0.05f, 0.1f, 0.1f, 0.3f, 0.6f, 0.3f, 0.1f, 0.1f, 0.05f, 0.05f
	};

	float total_weight	= weights[5];
	float occlusion		= center_occlusion * weights[5];

	for (int i = -blur_radius; i <= blur_radius; ++i)
	{
		if (i == 0) continue;

		float2 tex	= i * dir * 0.003f + uv;
		float depth = GetDepth(tex);

		if (abs(depth - center_depth) <= 0.2f)
		{
			float w = weights[i + blur_radius];

			occlusion += w * gOcclusionMap.Sample(gSampler, tex).r;
			total_weight += w;
		}
	}

	return occlusion / total_weight;
}

float main(PSIN input) : SV_TARGET
{
	float center_occlusion	= gOcclusionMap.Sample(gSampler, input.uv).r;
	float center_depth		= GetDepth(input.uv);

	float blur	= Blur(
		float2(0.0f, 1.0f) * (100.0f - center_depth) / 100.0f,
		input.uv, 
		center_depth, 
		center_occlusion);

	return saturate(blur);
}