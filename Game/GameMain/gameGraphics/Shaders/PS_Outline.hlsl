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

Texture2D<float4> StencilTexture : register(t6);
//Texture2D<uint> StencilTexture : register(t6);

#define OUTLINE_THICKNESS 2.0f // Pixel Offset

float4 PS(VSOUT input) : SV_Target0
{

	//PSOut output;
	//int stencil_sum = 0;

	//// Accumulate sum of stencil values of neighbouring pixels
	//// Decrement by 1024 if current pixel is stenciled, we don't want to draw outlines inside the objects
	//stencil_sum -= 1024 * StencilTexture.Load(int3(int2(input.Pos.x, input.Pos.y), 0));
	//stencil_sum += StencilTexture.Load(int3(int2(input.Pos.x + OUTLINE_THICKNESS, input.Pos.y + 0.0f), 0));
	//stencil_sum += StencilTexture.Load(int3(int2(input.Pos.x + 0.0f, input.Pos.y + OUTLINE_THICKNESS), 0));
	//stencil_sum += StencilTexture.Load(int3(int2(input.Pos.x - OUTLINE_THICKNESS, input.Pos.y + 0.0f), 0));
	//stencil_sum += StencilTexture.Load(int3(int2(input.Pos.x + 0.0f, input.Pos.y - OUTLINE_THICKNESS), 0));

	//// Valid stencil values are 1, 5, 26, 105 that correspond to different colours
	//// Assuming all adjacent pixels are stenciled
	//// Colors should be set to the various army colors
	//float4 final_color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	//if (stencil_sum >= 105)
	//{
	//	final_color = float4(0.0f / 255.0f, 93.0f / 255.0f, 5.0f / 255.0f, 1.0f);
	//}
	//else if (stencil_sum >= 26)
	//{
	//	final_color = float4(47.0f / 255.0f, 62.0f / 255.0f, 236.0f / 255.0f, 1.0f);
	//}
	//else if (stencil_sum >= 5)
	//{
	//	final_color = float4(74.0f / 255.0f, 1.0f / 255.0f, 117.0f / 255.0f, 1.0f);
	//}
	//else if (stencil_sum > 0)
	//{
	//	final_color = float4(117.0f / 255.0f, 1.0f / 255.0f, 1.0f / 255.0f, 1.0f);
	//}

	//
	//output.Color = final_color;
	//
	//if (stencil_sum > 0)
	//	return output;

	//// Discard pixel if we don't have to draw anything
	//discard;
	//return output;


	// Is there any color here?
	const float4 current_stencil_color = StencilTexture.Load(int3(input.Pos.xy, 0));
	if (length(current_stencil_color) == 0) discard;

	/*
		Load color data around current pixel 

		Pixels to load:
			0 1 0
			1 1 1
			0 1 0

		If their accumulated subtraction is not equal to current color multipled by 4 then we got an edge
	*/
	float4 neightbor_stencil_color[4];
	neightbor_stencil_color[0] = StencilTexture.Load(int3(int2(input.Pos.x + OUTLINE_THICKNESS, input.Pos.y + 0.0f), 0));
	neightbor_stencil_color[1] = StencilTexture.Load(int3(int2(input.Pos.x + 0.0f, input.Pos.y + OUTLINE_THICKNESS), 0));
	neightbor_stencil_color[2] = StencilTexture.Load(int3(int2(input.Pos.x - OUTLINE_THICKNESS, input.Pos.y + 0.0f), 0));
	neightbor_stencil_color[3] = StencilTexture.Load(int3(int2(input.Pos.x + 0.0f, input.Pos.y - OUTLINE_THICKNESS), 0));

	float4 current_stencil_color_multiple = current_stencil_color * 4.0f;
	current_stencil_color_multiple -= neightbor_stencil_color[0];
	current_stencil_color_multiple -= neightbor_stencil_color[1];
	current_stencil_color_multiple -= neightbor_stencil_color[2];
	current_stencil_color_multiple -= neightbor_stencil_color[3];
	

	if (abs(length(current_stencil_color_multiple)) <= 0.0001f) discard;


	return current_stencil_color;
}