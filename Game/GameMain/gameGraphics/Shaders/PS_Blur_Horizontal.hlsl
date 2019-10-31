
Texture2D<float4> gColorBuffer	: register (t1);
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
	return (gColorBuffer.Sample(gSampler, uv).w);

	//return (gDepthMap.Sample(gSampler, uv).r);
}

float Blur(
	const float2 dir, 
	const float2 uv,
	const float center_depth, 
	const float center_occlusion)
{
	// Good settings
	//float4 weights = float4(
	//	0.1f,
	//	0.2f,
	//	0.2f,
	//	0.1f
	//);

	//float offsets[4] =
	//{
	//	-0.004f,
	//	-0.002f,
	//	 0.002f,
	//	 0.004f
	//};

	//float4 threshold = (float4)0.008f;

	// --- END ---

	//float4 threshold = (float4)0.0008f;

	float4 weights = float4(
		0.12f,
		0.22f,
		0.22f,
		0.12f
		);

	float offsets[4] =
	{
		-0.003f,
		-0.001f,
		 0.001f,
		 0.003f
	};

	float4 threshold = (float4)0.001f;

	float4 samples;
	samples.x = gOcclusionMap.Sample(gSampler, uv + offsets[0] * dir).r;
	samples.y = gOcclusionMap.Sample(gSampler, uv + offsets[1] * dir).r;
	samples.z = gOcclusionMap.Sample(gSampler, uv + offsets[2] * dir).r;
	samples.w = gOcclusionMap.Sample(gSampler, uv + offsets[3] * dir).r;

	float4 depth;
	depth.x = GetDepth(uv + offsets[0] * dir);
	depth.y = GetDepth(uv + offsets[1] * dir);
	depth.z = GetDepth(uv + offsets[2] * dir);
	depth.w = GetDepth(uv + offsets[3] * dir);


	float4 distance			= abs((float4)center_depth - depth);
	float4 if_replacement	= step(distance, threshold);

	float result		= dot(if_replacement * weights, samples);
	float weight_sum	= dot(if_replacement, weights);

	return result * weight_sum;
	//return saturate(length(if_replacement));
}

float main(PSIN input) : SV_TARGET
{
	float center_occlusion	= gOcclusionMap.Sample(gSampler, input.uv).r;
	float center_depth		= GetDepth(input.uv);

	float blur_horizontal	= Blur(
		float2(0.0f, 1.0f) * (100.0f - center_depth) / 100.0f, 
		input.uv, 
		center_depth, 
		center_occlusion);
	
	float blur_vertical		= Blur(
		float2(1.0f, 0.0f) * (100.0f - center_depth) / 100.0f, 
		input.uv, 
		center_depth, 
		center_occlusion);

	return saturate(blur_horizontal + blur_vertical);
}