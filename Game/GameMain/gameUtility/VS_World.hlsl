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
	uint color;
};

StructuredBuffer<Vertex> gVertices : register (t0);


struct VSOUT
{
	/*
		TODO:
		normal
		sun pos
		View pos
		Proj pos
	*/

	float4 position		: SV_POSITION;
	float3 color		: COLOR0;
};

VSOUT main(uint VertexID : VertexStart)
{
	VSOUT output;
	Vertex vertex = gVertices[VertexID];

	uint4 color = Unpack(vertex.color) / 255.0f;
	output.color = color.rgb;
	output.position = float4(vertex.position, 1.0f);

	return output;
}