

Texture2D<float4> gNormalMap		: register (t2);
Texture2D<float4> gPositionMap		: register (t3);

Texture2D<float4> gRandomMap		: register(t4);

SamplerState gSampler				: register (s1);

cbuffer gCam : register (b0)
{
	float4x4 gView;
};

cbuffer gPer : register (b1)
{
	float4x4 gPerspective;
};

float3 GetPosition(const float2 uv)
{
	return gPositionMap.Sample(gSampler, uv).xyz;
}

float3 GetNormal(const float2 uv)
{
	return normalize(gNormalMap.Sample(gSampler, uv).xyz);
}

float4 GetNormalAndDepth(const float2 uv)
{
	float4 data = gNormalMap.Sample(gSampler, uv).xyzw;
	float3 normal = data.xyz;

	data.xyz = normalize(normal).xyz;

	return data;
}

float3 WorldPosFromDepth(const float depth, const float2 uv)
{/*
	const float3 screen_pos =
		float3(
			uv * float2(2.0f, -2.0f) - float2(1.0f, -1.0f),
			1.0f - depth);

	float4 world_pos = mul(float4(screen_pos, 1.0f), transpose(gPerspective));
	world_pos /= world_pos.w;

	return world_pos.xyz;*/

	const float4 clip_space_position =
		float4(
			uv * float2(2.0f, -2.0f) - float2(1.0f, -1.0f),
			1.0f - depth, 1.0f);

	float4 view_space_position = mul(clip_space_position, transpose(gPerspective));

	view_space_position /= view_space_position.w;

	//float4 world_space_position = mul(view_space_position, transpose(gView));

	return view_space_position.xyz;
}

float2 GetRandom(const float2 uv)
{
	const float2 screen_size = float2(1920, 1080);
	const float2 random_size = float2(64, 64);

	return normalize(gRandomMap.Sample(gSampler, screen_size * uv / random_size).xy * 2.0f - 1.0f);
}

float CalculateOcclusion(
	const float2 tcoord,
	const float2 uv,
	const float3 pos,
	const float3 normal)
{
	const float scale = 0.2f;
	const float bias = 0.2f;
	const float intensity = 0.5f;

	const float3 diff	= GetPosition(tcoord + uv) - pos;
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
	const float sample_radius = 0.2f;

	float occlusion = 0.0f;

	const float2 vec[4] = {
		float2( 1,  0),
		float2(-1,  0),
		float2( 0,  1),
		float2( 0, -1),
	};

	float3 pos	= GetPosition(input.uv).xyz;
	float3 normal	= GetNormal(input.uv).xyz;

	/*float4 data		= GetNormalAndDepth(input.uv);
	float3 pos		= WorldPosFromDepth(data.w, input.uv);
	float3 normal	= data.xyz;

	return data.wwww;*/

	float2 random	= GetRandom(input.uv).xy;
	float radius	= sample_radius / pos.z;

	const uint iterations = 4;
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
	
	return occlusion;
}