struct PSIN
{
	float4 pos			: SV_POSITION;
	float4 color		: COLOR0;
};

float4 main(PSIN input) : SV_TARGET
{
	return float4(input.color.rgb, 1.0f);
}