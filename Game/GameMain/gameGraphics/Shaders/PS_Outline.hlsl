struct VSOUT
{
	float4 Pos	 : SV_POSITION;
	float4 Color : COLOR;
	float2 UV	 : TEXCOORD0;
	float3 worldPos : POSITION0;
};

struct PSOut
{
	float4 Color;
};

Texture2D<uint> StencilTexture : register(t6);

#define OUTLINE_THICKNESS 2.0f

PSOut PS(VSOUT input) : SV_Target
{

	PSOut output;
	int stencil_sum = 0;

	// Accumulate sum of stencil values of neighbouring pixels
	// Decrement by 1024 if current pixel is stenciled, we don't want to draw outlines inside the objects
	stencil_sum -= 1024 * StencilTexture.Load(int3(int2(input.Pos.x, input.Pos.y), 0));
	stencil_sum += StencilTexture.Load(int3(int2(input.Pos.x + OUTLINE_THICKNESS, input.Pos.y + 0.0f), 0));
	stencil_sum += StencilTexture.Load(int3(int2(input.Pos.x + 0.0f, input.Pos.y + OUTLINE_THICKNESS), 0));
	stencil_sum += StencilTexture.Load(int3(int2(input.Pos.x - OUTLINE_THICKNESS, input.Pos.y + 0.0f), 0));
	stencil_sum += StencilTexture.Load(int3(int2(input.Pos.x + 0.0f, input.Pos.y - OUTLINE_THICKNESS), 0));

	// Valid stencil values are 1, 5, 26, 105 that correspond to different colours
	// Assuming all adjacent pixels are stenciled
	// Colors should be set to the various army colors
	float4 final_color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	if (stencil_sum >= 105)
	{
		final_color = float4(0.0f / 255.0f, 93.0f / 255.0f, 5.0f / 255.0f, 1.0f);
	}
	else if (stencil_sum >= 26)
	{
		final_color = float4(47.0f / 255.0f, 62.0f / 255.0f, 236.0f / 255.0f, 1.0f);
	}
	else if (stencil_sum >= 5)
	{
		final_color = float4(74.0f / 255.0f, 1.0f / 255.0f, 117.0f / 255.0f, 1.0f);
	}
	else if (stencil_sum > 0)
	{
		final_color = float4(117.0f / 255.0f, 1.0f / 255.0f, 1.0f / 255.0f, 1.0f);
	}

	
	output.Color = final_color;
	
	if (stencil_sum > 0)
		return output;

	// Discard pixel if we don't have to draw anything
	discard;
	return output;
}