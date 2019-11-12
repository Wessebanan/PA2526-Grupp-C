struct VSOut
{
	float4 Pos	 : SV_POSITION;
	float4 Color : COLOR;
	float2 UV	 : TEXCOORD0;
	float3 worldPos : POSITION0;
};

VSOut VS(uint VertexID: SV_VertexID)
{
	// This shader assumes an input draw of 3 vertices with no IA, no vertex buffer and no index buffer set
	// It will create a triangle that covers the entire screen, as seen in the following screenshot
	// https://i.imgur.com/jE0UlZ4.png
	VSOut output = (VSOut)0;
	output.Pos = float4(float2(((VertexID << 1) & 2) * 2.0f, (VertexID == 0) * -4.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
	return output;
}