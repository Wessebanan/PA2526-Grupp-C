StructuredBuffer<float3> gVertexPositions	: register (t0); 
StructuredBuffer<float3> gVertexNormals		: register (t1);
StructuredBuffer<float3> gVertexTexCoords	: register (t2);

cbuffer gPerObjectData : register(b0)
{
	float4 gTransformation[2];
};

cbuffer ViewAndProjectionMatrices : register(b1)
{
	float4x4 gViewMatrix;
};

cbuffer ViewAndProjectionMatrices : register(b2)
{
	float4x4 gProjectionMatrix;
};

struct VSOUT
{
	float4 pos : SV_POSITION;
};

VSOUT main(uint VertexID : VertexStart, uint InstanceID : InstanceStart)
{
	VSOUT output;

	output.pos = float4(gVertexPositions[VertexID].xyz, 1.0f) + gTransformation[InstanceID];
	output.pos = mul(gProjectionMatrix, mul(gViewMatrix, output.pos));

	return output;
}