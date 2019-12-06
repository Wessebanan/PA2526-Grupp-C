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

	float3 normalViewSpace		: NORMAL1;
	float3 positionViewSpace	: POSITION2;

	float3 pos_ndc			: POSITION3;
};

VSOut main(uint VertexID : VertexStart, uint InstanceID : InstanceStart)
{
	VSOut output = (VSOut)0;

	// Get Color from matrix and restore Matrix
	float4x4 world_matrix = gMesh[InstanceID].World;
	uint color = (uint)world_matrix[3][3];
	world_matrix[3][3] = 1.0f;

	float4x4 wvpCam = mul(gPerspective, mul(gView, world_matrix));
	float4x4 wvpSun = mul(gOrtographicsSun, mul(gViewSun, world_matrix));

	float4 world_pos = float4(gVertexPositions[VertexID].xyz, 1.0f); 
	float4 view_pos = mul(gView, mul(gMesh[InstanceID].World, world_pos));

	// Output Data
	output.pos = mul(wvpCam, world_pos);
	output.sunPos = mul(wvpSun, world_pos);

	output.color.rgb = unpack(color).rgb / 255.f;

	output.normal = mul(gMesh[InstanceID].World, float4(gVertexNormals[VertexID], 0.0f)).xyz;
	
	output.pos_ndc = (output.pos.xyz / output.pos.w);

	output.normalViewSpace = mul((float3x3)gView, output.normal).xyz;
	output.positionViewSpace = view_pos.xyz;

	return output;
}
