uint4 unpack(const uint packedData)
{
	uint4 unpacked_data;

	unpacked_data.x = (packedData) >> 24;
	unpacked_data.y = (packedData & 0x00ff0000) >> 16;
	unpacked_data.z = (packedData & 0x0000ff00) >> 8;
	unpacked_data.w = (packedData & 0x000000ff);

	return unpacked_data;
}

struct PerObjectData
{
	float3 Pos;
	uint   color;
};

cbuffer gTransformation : register (b0)
{
	PerObjectData gMesh[4];
};

cbuffer gCam : register (b1)
{
	float4x4 gView;
};

cbuffer gPer : register (b2)
{
	float4x4 gPerspective;
};

cbuffer gSunCam : register (b3)
{
	float4x4 gViewSun;
};

cbuffer gSunOrto : register (b4)
{
	float4x4 gOrtographicsSun;
};

StructuredBuffer<float3> gVertexPositions		: register (t0);
StructuredBuffer<float3> gVertexNormals			: register (t1);
StructuredBuffer<float2> gVertexTexCoords		: register (t2);

struct VSOUT
{
	float4 pos			: SV_POSITION;
	float4 sunPos		: POSITION1;

	float3 color		: COLOR0;
	float3 normal		: NORMAL0;

	// SSAO
	float3 normalViewSpace		: NORMAL1;
	float3 positionViewSpace	: POSITION2;
};

VSOUT main(uint VertexID : VertexStart, uint InstanceID : InstanceStart)
{
	VSOUT output;

	float4 worldPos = float4(gVertexPositions[VertexID].xyz + gMesh[InstanceID].Pos.xyz, 1.0f);
	float4 viewPos = mul(gView, worldPos);

	output.pos		= mul(gPerspective, viewPos);
	output.sunPos	= mul(gOrtographicsSun, mul(gViewSun, worldPos));

	float4 clr		= unpack(gMesh[InstanceID].color) / 255.0f;
	output.color	= clr.rgb;
	output.normal	= gVertexNormals[VertexID];

	output.normalViewSpace		= mul(gView, float4(gVertexNormals[VertexID], 0.0f)).xyz;
	output.positionViewSpace	= viewPos.xyz;

	return output;
}