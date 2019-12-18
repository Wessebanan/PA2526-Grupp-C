uint4 unpack(const uint packedData)
{
	uint4 unpacked_data;

	unpacked_data.x = (packedData) >> 24;
	unpacked_data.y = (packedData & 0x00ff0000) >> 16;
	unpacked_data.z = (packedData & 0x0000ff00) >> 8;
	unpacked_data.w = (packedData & 0x000000ff);

	return unpacked_data;
}

struct PSIN
{
	float4 pos			: SV_POSITION;
	float4 sunPos		: POSITION1;

	float4 color		: COLOR0;
	float3 normal		: NORMAL0;

	float3 normalViewSpace		: NORMAL1;
	float3 positionViewSpace	: POSITION2;

	uint   fakeStencilValue		: FAKESTENCIL;
};

float4 main(PSIN input) : SV_TARGET0
{
	return input.color;
}