

Texture2D<float4> gNormalMap		: register (t2);
Texture2D<float4> gRandomMap		: register (t4);

SamplerState gSampler				: register (s1);

float GetDepth(const float2 uv)
{
	return gNormalMap.Sample(gSampler, uv).w;
}

float3 GetNormal(const float2 uv)
{
	return normalize(gNormalMap.Sample(gSampler, uv).xyz);
}

float3 ViewPosFromDepth(const float depth, const float2 uv)
{
	const float2 xy = uv * float2(2.0f, -2.0f) - float2(1.0f, -1.0f);

	const float aspect	= 1920.0f / 1080.0f;
	const float fovy	= 3.14f / 2.0f;
	const float far_z	= 100.0f;

	float2 half_size;
	half_size.y			= far_z * tan(0.5f * fovy);
	half_size.x			= aspect * half_size.y;

	float3 frustrum_pos = float3(xy * half_size, far_z);

	return float3((depth / far_z) * frustrum_pos);
}

float2 GetRandom(const float2 uv)
{
	const float2 screen_size = float2(1920, 1080);
	const float2 random_size = float2(64, 64);

	return normalize(
		gRandomMap.Sample(
			gSampler, 
			screen_size * uv / random_size).xy * 2.0f - 1.0f);
}

float CalculateOcclusion(
	const float2 tcoord,
	const float2 uv,
	const float3 pos,
	const float3 normal)
{
	const float scale		= 0.20f;
	const float bias		= 0.25f;
	const float intensity	= 1.20f;

	const float3 occlusion_position = ViewPosFromDepth(
		GetDepth(tcoord + uv), 
		tcoord + uv);


	const float3 diff	= occlusion_position - pos;
	const float3 v		= normalize(diff);
	const float d		= length(diff) * scale;

	return 
		saturate(dot(normal, v) - bias) 
		* (1.0f / (1.0f + d)) * intensity;
}

struct PSIN
{
	float4 pos	: SV_POSITION;
	float2 uv	: TEXCOORD0;
};

float main(PSIN input) : SV_TARGET
{
	const float sample_radius = 0.1f;

	float occlusion = 0.0f;

	const float2 vec[4] = {
		float2( 1,  0),
		float2(-1,  0),
		float2( 0,  1),
		float2( 0, -1),
	};

	float3 pos		= ViewPosFromDepth(GetDepth(input.uv), input.uv);
	float3 normal	= GetNormal(input.uv);

	float2 random	= GetRandom(input.uv);
	float radius	= sample_radius / pos.z;

	const uint iterations = 1;
	for (uint i = 0; i < iterations; i++)
	{
		float2 coord1 = reflect(vec[i], random) * radius;
		float2 coord2 = float2(
			coord1.x * 0.707f - coord1.y * 0.707f,
			coord1.x * 0.707f + coord1.y * 0.707f);

		occlusion += CalculateOcclusion(input.uv, coord1 * 0.25f, pos, normal);
		occlusion += CalculateOcclusion(input.uv, coord2 * 0.50f, pos, normal);
		occlusion += CalculateOcclusion(input.uv, coord1 * 0.75f, pos, normal);
		occlusion += CalculateOcclusion(input.uv, coord2 * 1.00f, pos, normal);
	}

	occlusion /= (float)iterations * 4.0f;
	
	return saturate(pow(occlusion, 4.0f) * 100.0f);
}