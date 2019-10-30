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
	
	float3 color		: COLOR0;
	float3 normal		: NORMAL0;

	float3 normalViewSpace		: NORMAL1;
	float3 positionViewSpace	: POSITION2;
};

struct PSOUT
{
	float4 BackBuffer		: SV_TARGET0;
	float4 NormalBuffer		: SV_TARGET1;
};

PSOUT main(PSIN input)
{
	PSOUT output = (PSOUT)0;

	const float3 cam_dir = -float3(1.2f, -0.7f, 1.0f);
	float illu = saturate(dot(cam_dir, normalize(input.normal)));

	float in_shadow		= shadow(input.sunPos.xy, input.sunPos.z);
	float3 finalColor	= input.color;

	float3 ambient = finalColor.xyz * 0.1f;
	float3 diffuse = finalColor.xyz * illu * in_shadow;

	output.BackBuffer		= float4(ambient + diffuse, input.positionViewSpace.z / 100.0f);
	output.NormalBuffer		= float4(normalize(input.normalViewSpace), 0.0f);

	return output;
}