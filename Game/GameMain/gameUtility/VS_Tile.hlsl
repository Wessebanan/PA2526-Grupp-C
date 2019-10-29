uint4 Unpack(const uint packedData)
{
	uint4 unpacked_data;

	unpacked_data.x = (packedData) >> 24;
	unpacked_data.y = (packedData & 0x00ff0000) >> 16;
	unpacked_data.z = (packedData & 0x0000ff00) >> 8;
	unpacked_data.w = (packedData & 0x000000ff);

	return unpacked_data;
}

struct Vertex
{
	float3 position;
	float3 normal;
	uint color;
};

StructuredBuffer<Vertex> gVertices : register (t5);

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

VSOUT main(uint VertexID : VertexStart)
{
	VSOUT output;
	Vertex vertex = gVertices[VertexID];

	float4 worldPos = float4(vertex.position.xyz, 1.0f);

	output.pos = mul(gPerspective, mul(gView, worldPos));
	output.sunPos = mul(gOrtographicsSun, mul(gViewSun, worldPos));

	output.color = Unpack(vertex.color) / 255.0f;
	output.normal = vertex.normal.xyz;

	return output;
}