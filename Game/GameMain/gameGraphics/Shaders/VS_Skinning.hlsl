static const int MAX_AFFECTING_BONES = 4;
static const int MAX_BONE_MATRICES = 63;

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
	float4x4 gBoneTransforms[MAX_BONE_MATRICES];
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
StructuredBuffer<float3> gVertexBlendWeight		: register (t3);
StructuredBuffer<int4>   gVertexBlendIndices	: register (t4);

struct VSOut
{
	float4 pos			: SV_POSITION;
	float4 sunPos		: POSITION1;

	float3 color		: COLOR0;
	float3 normal		: NORMAL0;

	// SSAO
	float3 normalViewSpace		: NORMAL1;
	float3 positionViewSpace	: POSITION2;
};

VSOut main(uint VertexID : VertexStart, uint InstanceID : InstanceStart)
{
	VSOut output = (VSOut)0;

	// Get Color from matrix and restore Matrix
	float4x4 world_matrix	= gMesh[InstanceID].World;
	uint color				= (uint)world_matrix[3][3];
	world_matrix[3][3]		= 1.0f;
	
	float4x4 wvCam = mul(gView, world_matrix);
	float4x4 wvpCam = mul(gPerspective, wvCam);
	float4x4 wvpSun = mul(gOrtographicsSun, mul(gViewSun, world_matrix));

	float lastWeight = 1.0f;
	float4 v = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float3 norm = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < MAX_AFFECTING_BONES - 1; ++i)
	{
		v += gVertexBlendWeight[VertexID][i] * mul(float4(gVertexPositions[VertexID], 1.0f), gMesh[InstanceID].gBoneTransforms[gVertexBlendIndices[VertexID][i]]);
		norm += (gVertexBlendWeight[VertexID][i] * mul(float4(gVertexNormals[VertexID], 1.0f), gMesh[InstanceID].gBoneTransforms[gVertexBlendIndices[VertexID][i]])).xyz;
		lastWeight -= gVertexBlendWeight[VertexID][i];
	}
	// Apply last weight
	v += lastWeight * mul(float4(gVertexPositions[VertexID], 1.0f), gMesh[InstanceID].gBoneTransforms[gVertexBlendIndices[VertexID][MAX_AFFECTING_BONES - 1]]);
	norm += (lastWeight * mul(float4(gVertexNormals[VertexID], 1.0f), gMesh[InstanceID].gBoneTransforms[gVertexBlendIndices[VertexID][MAX_AFFECTING_BONES - 1]])).xyz;
	v.w = 1.0f;

	// Output Data
	output.pos			= mul(wvpCam, v);
	output.sunPos		= mul(wvpSun, v);

	output.color.rgb	= (float3)unpack(color).rgb / 255.0f;
	output.normal		= mul(world_matrix, float4(gVertexNormals[VertexID], 0.0f)).xyz;

	output.normalViewSpace		= mul(wvCam, float4(gVertexNormals[VertexID], 0.0f)).xyz;
	output.positionViewSpace	= mul(wvCam, v).xyz;

	return output;
}
