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
	float3 pos;
	uint   color;
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

struct VSOUT
{
	float4 pos			: SV_POSITION;
	float4 color		: COLOR0;
};

VSOUT main(uint VertexID : VertexStart, uint InstanceID : InstanceStart)
{
	uint instance_id	= VertexID / 3;
	uint vertex_id		= VertexID % 3;

	VSOUT output;
	output.color		= unpack(gMesh[instance_id].color) / 255.0f;

	float4 world_pos	= float4(gMesh[instance_id].pos.xyz, 1.0f);
	float4 view_pos		= mul(gView, world_pos);

	float2 texcoord  = float2((vertex_id << 1) & 2, vertex_id & 2);
	float3 position  = float3(texcoord * float2(2, -2) + float2(-1, 1), 0);
	view_pos		+= float4(position, 0.0f) * output.color.a;

	output.pos		 = mul(gPerspective, view_pos);

	return output;
}