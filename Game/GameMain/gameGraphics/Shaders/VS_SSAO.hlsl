
StructuredBuffer<float3> gVertexPositions		: register (t0);
StructuredBuffer<float3> gVertexNormals			: register (t1);
StructuredBuffer<float2> gVertexTexCoords		: register (t2);
StructuredBuffer<float3> gVertexBlendWeight		: register (t3);
StructuredBuffer<int4>   gVertexBlendIndices	: register (t4);



struct VSOUT
{
	float4 pos	: SV_POSITION;
	float2 uv	: TEXCOORD0;
};

VSOUT main(
	uint VertexID : VertexStart, 
	uint InstanceID : InstanceStart)
{
	VSOUT output = (VSOUT)0;

	output.pos	= float4(gVertexPositions[VertexID], 1.0f);
	output.uv	= gVertexTexCoords[VertexID];

	return output;
}