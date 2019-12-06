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

Texture2D<float4> gRandomMap		: register (t4);
Texture2D<float> gDepthMap			: register (t5);
SamplerState gSampler				: register (s1);

cbuffer gPer : register (b0)
{
	float4x4 gInvPerspective;
};

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

float3 GetViewPos(const float2 xy)
{
	float2 uv = (xy + float2(1.0f, -1.0f)) * float2(0.5f, -0.5f);

	float4 pos_ndc = float4(xy, gDepthMap.Sample(gSampler, uv).r, 1.0f);

	float4 pos_view = mul(gInvPerspective, pos_ndc);
	pos_view /= pos_view.w;

	return pos_view.xyz;
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
	const float2 pos_ndc,
	const float2 offset,
	const float3 pos_view,
	const float3 normal_view)
{
	const float scale		= 1.0f;
	const float bias		= 0.2f;
	const float intensity	= 1.0f;

	const float3 diff	= GetViewPos(pos_ndc + offset) - pos_view;

	const float3 v		= normalize(diff);			// Direction
	const float d		= length(diff) * scale;		// Length

	return saturate(dot(normal_view, v) - bias) * intensity / (1.0f + d);
}

float CalculateSSAO(
	const float3 normal_view, 
	const float3 pos_view, 
	const float3 pos_ndc)
{
	const float sample_radius = 0.1f;

	float occlusion = 0.0f;

	const uint iterations = 4;
	const float2 vec[iterations] = {
		float2( 1,   1),
		float2(-1,   1),
		float2( 1,  -1),
		float2(-1,  -1),
	};

	float2 random = GetRandom(pos_ndc);
	float radius = sample_radius / pos_view.z;

	for (uint i = 0; i < iterations; i++)
	{
		float2 coord1 = reflect(vec[i], random) * radius;
		float2 coord2 = float2(
			coord1.x * 0.707f - coord1.y * 0.707f,
			coord1.x * 0.707f + coord1.y * 0.707f);

		occlusion += CalculateOcclusion(pos_ndc.xy, coord1 * 0.25f, pos_view, normal_view);
		occlusion += CalculateOcclusion(pos_ndc.xy, coord2 * 0.50f, pos_view, normal_view);
		occlusion += CalculateOcclusion(pos_ndc.xy, coord1 * 0.75f, pos_view, normal_view);
		occlusion += CalculateOcclusion(pos_ndc.xy, coord2 * 1.00f, pos_view, normal_view);
	}

	occlusion /= (float)iterations * 4.0f;

	return pow(saturate(1.0f - occlusion), 1.0f);
}

struct PSIN
{
	float4 pos			: SV_POSITION;
	float4 sunPos		: POSITION1;
	
	float3 color		: COLOR0;
	float3 normal		: NORMAL0;

	float3 normalViewSpace		: NORMAL1;
	float3 positionViewSpace	: POSITION2;

	float3 pos_ndc				: POSITION3;
};

struct PSOUT
{
	float4 BackBuffer		: SV_TARGET0;
};

PSOUT main(PSIN input)
{
	PSOUT output = (PSOUT)0;

	const float3 cam_dir = -float3(0.5f, -1.0f, 0.5f);
	float illu = saturate(dot(normalize(cam_dir), normalize(input.normal)));

	float in_shadow		= shadow(input.sunPos.xy, input.sunPos.z);
	float3 finalColor	= input.color;

	float ssao = CalculateSSAO(
		normalize(input.normalViewSpace.xyz),
		input.positionViewSpace.xyz,
		input.pos_ndc.xyz);

	float3 ambient = finalColor.xyz * 0.1f;
	float3 diffuse = finalColor.xyz * illu * in_shadow;

	float sample_occlusion = CalculateOcclusion(
		input.pos_ndc.xy,
		float2(0.0f, 0.0f) * float2(1.0f / 1920.0f, 1.0f / 1080.0f),
		input.positionViewSpace.xyz,
		normalize(input.normalViewSpace.xyz));

	//output.BackBuffer = float4(sample_occlusion.xxx, 1.0f);
	//output.BackBuffer = float4(input.positionViewSpace.xyz, 1.0f);
	output.BackBuffer = float4(ssao.xxx, 1.0f);
	
	return output;
}