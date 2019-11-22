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

/*
	Constant buffers has to be aligned. We store all height values in float4, so this
	is how we access each element per tile:

					________________________________________________________________
	Tile index		| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10| 11| 12| 13| 14| ..
					 \______ ______/ \______ ______/ \______ ______/ \______ ____
							V               V               V               V
	height float4 index		0               1               2               3

	float4 tileGroup = gHeight[ <tile index> / 4 ];
	float height = tileGroup[ <tile index> % 4];
					==
	float height = (gHeight[ <tile index> / 4 ])[ <tile index> % 4 ];
*/

struct VSOUT
{
	float4 pos			: SV_POSITION;
	float4 sunPos		: POSITION1;

	float3 color		: COLOR0;
	float3 normal		: NORMAL0;

	float3 normalViewSpace		: NORMAL1;
	float3 positionViewSpace	: POSITION2;
};

VSOUT main(uint VertexID : VertexStart)
{
	/*
		Each tile's X and Z value is read from the GPU placed 'vertex buffer',
		tiles doesn't move horisontaly during gameplay so we only upload this
		once.

		Each vertex read that tile's height value, which exist in the constant
		buffer gVertexData::gHeight[].
	*/

	VSOUT output;
	Vertex vertex = gVertices[VertexID];

	int tileIndex = floor(VertexID / 96.f);

	// To understand indexing, please read description above.
	vertex.position.y += (gHeight[floor(tileIndex / 4.f)])[tileIndex % 4];

	float4 worldPos = float4(vertex.position.xyz, 1.0f);
	float4 viewPos = mul(gView, worldPos);
	output.pos = mul(gPerspective, viewPos);
	output.sunPos = mul(gOrtographicsSun, mul(gViewSun, worldPos));

	output.color = Unpack(vertex.color) / 255.0f;
	output.normal = vertex.normal.xyz;

	output.normalViewSpace = mul((float3x3)gView, vertex.normal).xyz;
	output.positionViewSpace = viewPos.xyz;

	return output;
}