

Texture2D<float> gOcclusionMap	: register (t2);
Texture2D<float> gDepthMap		: register (t5);

SamplerState gSampler : register (s2);

struct PSIN
{
	float4 pos	: SV_POSITION;
	float2 uv	: TEXCOORD0;
};

float GetDepth(const float2 uv)
{
	return (gDepthMap.Sample(gSampler, uv).r - 0.5f) * 2.0f;
}

float4 main(PSIN input) : SV_TARGET
{
	float4 weights = float4(
		0.0702702703,
		0.3162162162,
		0.3162162162,
		0.0702702703
	);

	float offsets[4] =
	{
		-3.2307692308,
		-1.3846153846,  
		 1.3846153846,
		 3.2307692308
	};

	float threshold = 0.0001f;
	float2 dir = float2(1.0f, 0.0f);

	float center_occlusion = gOcclusionMap.Sample(gSampler, input.uv).r;
	float center_depth = gDepthMap.Sample(gSampler, input.uv).r;

	float4 samples;
	samples.x = gOcclusionMap.Sample(gSampler, input.uv + offsets[0] * dir).r;
	samples.y = gOcclusionMap.Sample(gSampler, input.uv + offsets[1] * dir).r;
	samples.z = gOcclusionMap.Sample(gSampler, input.uv + offsets[2] * dir).r;
	samples.w = gOcclusionMap.Sample(gSampler, input.uv + offsets[3] * dir).r;

	float4 depth;
	depth.x = GetDepth(input.uv + offsets[0] * dir);
	depth.y = GetDepth(input.uv + offsets[1] * dir);
	depth.z = GetDepth(input.uv + offsets[2] * dir);
	depth.w = GetDepth(input.uv + offsets[3] * dir);

	float4 if_replacement = step(threshold, abs(float4(center_depth.xxxx) - depth));

	float result		= dot(if_replacement * weights, samples);
	float weigth_sum	= dot(if_replacement, weights);

	return result * (1.0f / weigth_sum);
}