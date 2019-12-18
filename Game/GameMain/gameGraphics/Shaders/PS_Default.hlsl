// Shadow Mapping
Texture2D<float> gShadowMap			: register (t0);
SamplerComparisonState gSmpCmp		: register (s0);

// Screen-Space Ambient Occlusion
Texture2D<float4> gRandomMap		: register (t4);
Texture2D<float> gDepthMap			: register (t5);
SamplerState gSampler				: register (s1);

// ---

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

	float offset = 1.0f / (2048.0f);
	float illumination = 0.0f;

	const float shadow_bias = 0.0028f;

	const float middle_weight = 0.2f;
	const float side_weight_total = 1.0f - middle_weight;
	const float side_weight = side_weight_total / 8.0f;

	const float weigth[3][3] =
	{
		{side_weight, side_weight, side_weight},
		{side_weight, middle_weight, side_weight},
		{side_weight, side_weight, side_weight},
	};

	[unroll]
	for (int x = -1; x <= 1; x++)
	{
		[unroll]
		for (int y = -1; y <= 1; y++)
		{
			illumination += gShadowMap.SampleCmp(
				gSmpCmp,
				shadowMapUV + float2(x, y) * offset,
				depth - shadow_bias) * weigth[x + 1][y + 1];
		}
	}

	return  saturate(2.f * illumination - 1.f);
}

float3 GetViewPos(const float2 ndc_xy)
{
	float2 screen_space_position = ndc_xy;
	screen_space_position += float2(1.0f, -1.0f);
	screen_space_position /= float2(2.0f, -2.0f);
	screen_space_position *= float2(1920,  1081);

	float z = gDepthMap.Load(int3(screen_space_position.xy, 0)).r;

	float4 clip_space_position = float4(ndc_xy, z, 1.0f);

	float4 view_space_position = mul(gInvPerspective, clip_space_position);

	return view_space_position.xyz /= view_space_position.w;
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
	const float bias		= 0.1f;
	const float intensity	= 1.0f;

	const float3 diff = GetViewPos(pos_ndc + offset) - pos_view;

	const float3 v		= normalize(diff);			// Direction
	const float d		= length(diff) * scale;		// Length

	return max(0.0f, dot(normal_view, v) - bias) * intensity / (1.0f + d);
}

float CalculateSSAO(
	const float3 normal_view, 
	const float3 pos_view, 
	const float2 pos_ndc)
{
	const float sample_radius = 0.1f;

	float occlusion = 0.0f;

	const uint iterations = 4;
	const float2 vec[iterations] = {
		float2( 1,   1),
		float2(-1,   1),
		float2( 1,  -1),
		float2(-1,  -1),

		/*float2( 1, 0),
		float2(-1, 0),
		float2(0, -1),
		float2(0,  1),*/
	};

	float2 random = GetRandom(pos_ndc);
	float radius = sample_radius / pos_view.z;

	for (uint i = 0; i < iterations; i++)
	{
		float2 coord1 = reflect(vec[i], random) * radius;
		float2 coord2 = float2(
			coord1.x * 0.707f - coord1.y * 0.707f,
			coord1.x * 0.707f + coord1.y * 0.707f);

		occlusion += CalculateOcclusion(pos_ndc, coord1 * 0.25f, pos_view, normal_view);
		occlusion += CalculateOcclusion(pos_ndc, coord2 * 0.50f, pos_view, normal_view);
		occlusion += CalculateOcclusion(pos_ndc, coord1 * 0.75f, pos_view, normal_view);
		occlusion += CalculateOcclusion(pos_ndc, coord2 * 1.00f, pos_view, normal_view);
	}

	occlusion /= (float)iterations * 4.0f;

	return pow(1.0f - saturate(occlusion), 2.0f);
}

struct PSIN
{
	float4 pos			: SV_POSITION;
	float4 sunPos		: POSITION1;

	float3 color		: COLOR0;
	float3 normal		: NORMAL0;

	float3 normalViewSpace		: NORMAL1;
	float3 positionViewSpace	: POSITION2;
};

float4 main(PSIN input) : SV_TARGET0
{
	float2 pos_ndc_xy = input.pos.xy / float2(1920, 1081);
	pos_ndc_xy *= float2(2.0f, -2.0f);
	pos_ndc_xy -= float2(1.0f, -1.0f);

	const float3 sun_dir = -float3(0.5f, -1.0f, 0.5f);
	const float diffuse_shading = saturate(dot(normalize(sun_dir), normalize(input.normal)));

	float ssao = CalculateSSAO(
		normalize(input.normalViewSpace.xyz),
		input.positionViewSpace.xyz,
		pos_ndc_xy);

	float illumination = 1.0f;

	// Diffuse and shadow mapping
	illumination *= diffuse_shading;
	illumination *= diffuse_shading <= 0.05f ? 0.05f * shadow(input.sunPos.xy, input.sunPos.z) : shadow(input.sunPos.xy, input.sunPos.z);

	float3 ambient = input.color.xyz * 0.1f;
	float3 diffuse = input.color.xyz * illumination;
	
	return float4((ambient + diffuse) * ssao, 1.0f);
}