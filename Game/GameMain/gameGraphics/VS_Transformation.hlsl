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
	float4x4 World;
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

struct VSOUT
{
	float4 pos			: SV_POSITION;
	float4 sunPos		: POSITION1;

	float3 color		: COLOR0;
	float3 normal		: NORMAL0;
};


StructuredBuffer<float3> gVertexPositions		: register (t0);
StructuredBuffer<float3> gVertexNormals			: register (t1);
StructuredBuffer<float2> gVertexTexCoords		: register (t2);
StructuredBuffer<float3> gVertexBlendWeight		: register (t3);
StructuredBuffer<uint4>  gVertexBlendIndices	: register (t4);

VSOUT main(uint VertexID : VertexStart, uint InstanceID : InstanceStart)
{
	VSOUT output;

	float3 vertex_position	= gVertexPositions[VertexID].xyz;
	float3 vertex_normal	= gVertexNormals[VertexID].xyz;

	float4x4 wvpCam = mul(gPerspective, mul(gView, gMesh[InstanceID].World));
	float4x4 wvpSun = mul(gOrtographicsSun, mul(gViewSun, gMesh[InstanceID].World));

	output.pos		= mul(wvpCam, float4(vertex_position, 1.0f));
	output.sunPos	= mul(wvpSun, float4(vertex_position, 1.0f));

	output.normal	= mul(gMesh[InstanceID].World, float4(vertex_normal, 0.0f)).xyz;
	output.color	= float3(1.0f, 1.0f, 1.0f);

	return output;
}