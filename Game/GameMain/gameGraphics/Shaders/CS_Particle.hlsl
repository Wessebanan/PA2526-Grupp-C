[numthreads(16, 16, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	const float delta					= 0.0f;
	const float total_life_duration		= 0.0f;
	float current_life_duration			= 0.0f;
	float scale							= 0.0f;
	float gravity						= 0.0f;

	const float life_percentage_left = current_life_duration / total_life_duration;

	float3 position		= (float3) 0.0f;
	float3 direction	= (float3) 0.0f;

	position	+= direction * life_percentage_left * delta;
	direction	+= float3(0.0f, gravity * delta, 0.0f);
}