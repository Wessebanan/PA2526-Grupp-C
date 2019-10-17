static const int MAX_AFFECTING_BONES = 4;
static const int MAX_BONE_MATRICES = 63;
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
};
VSOut main(uint VertexID : VertexStart, uint InstanceID : InstanceStart)
{
	VSOut output = (VSOut)0;
	float4x4 wvpCam = mul(gPerspective, mul(gView, gMesh[InstanceID].World));
	float4x4 wvpSun = mul(gOrtographicsSun, mul(gViewSun, gMesh[InstanceID].World));
	float lastWeight = 1.0f;
	float4 v = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float3 norm = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < MAX_AFFECTING_BONES - 1; ++i)
	{
		v += gVertexBlendIndices[VertexID][i] * mul(float4(gVertexPositions[VertexID], 1.0f), gMesh[InstanceID].gBoneTransforms[gVertexBlendIndices[VertexID][i]]);
		norm += (gVertexBlendIndices[VertexID][i] * mul(float4(gVertexNormals[VertexID], 1.0f), gMesh[InstanceID].gBoneTransforms[gVertexBlendIndices[VertexID][i]])).xyz;
		lastWeight -= gVertexBlendIndices[VertexID][i];
	}
	// Apply last weight
	v += lastWeight * mul(float4(gVertexPositions[VertexID], 1.0f), gMesh[InstanceID].gBoneTransforms[gVertexBlendIndices[VertexID][MAX_AFFECTING_BONES - 1]]);
	norm += (lastWeight * mul(float4(gVertexNormals[VertexID], 1.0f), gMesh[InstanceID].gBoneTransforms[gVertexBlendIndices[VertexID][MAX_AFFECTING_BONES - 1]])).xyz;
	v.w = 1.0f;
	output.pos = mul(wvpCam, v);
	output.sunPos = mul(wvpSun, float4(gVertexPositions[VertexID], 1.0f));
	output.color = float3(1.0f, 1.0f, 1.0f);
	output.normal = mul(gMesh[InstanceID].World, float4(gVertexNormals[VertexID], 0.0f)).xyz;
	return output;
}
