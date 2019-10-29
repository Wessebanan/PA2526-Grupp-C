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

cbuffer gVertexData : register (b5)
{
	float4 gHeight[4096];
};

struct VSOUT
{
	/*
		TODO:
		normal
		sun pos
		View pos
		Proj pos
	*/

	float4 pos			: SV_POSITION;
	float4 sunPos		: POSITION1;

	float3 color		: COLOR0;
	float3 normal		: NORMAL0;
};

VSOUT main(uint VertexID : VertexStart)
{
	VSOUT output;
	Vertex vertex = gVertices[VertexID];
	vertex.position.y = gHeight[floor(VertexID / 96)][(VertexID / 96) % 4];
	//vertex.position.y = VertexID / 96;

	//output.pos = mul(gPerspective, mul(gView, float4(vertex.position, 1.0f)));
	//output.sunPos = mul(gOrtographicsSun, mul(gViewSun, float4(vertex.position, 1.0f)));

	////uint4 color = Unpack(vertex.color) / 255.0f;
	//output.color = vertex.color.rgb;
	////output.color = float3(1.0f, 0.f, 0.f);





	float4 worldPos = float4(vertex.position.xyz, 1.0f);

	output.pos = mul(gPerspective, mul(gView, worldPos));
	output.sunPos = mul(gOrtographicsSun, mul(gViewSun, worldPos));

	//float4 clr = unpack(gMesh[InstanceID].color) / 255.0f;
	output.color = Unpack(vertex.color) / 255.0f;
	output.normal = vertex.normal.xyz;
	//output.normal = float3(0.f, 0.f, 1.f);


	return output;
}