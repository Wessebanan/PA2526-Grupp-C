#define MORE_THAN_ONE 2

struct TileVertex
{
	float3 position;
	uint color;
};

cbuffer TileMesh : register (b0)
{
	const int gTILE_MESH_VERTEX_COUNT = 60;
	float4 gTileVertices[60]; // Might cause problem, != (constant buffer alignment)
}

cbuffer LookUpTableBuffer : register (b1)
{
	uint gOceanTileOffset;
	uint gOceanTileCount;
	float2 gFirstXZ[MORE_THAN_ONE];	 // Might cause problem, != (constant buffer alignment)
};

RWStructuredBuffer<TileVertex> gBufferOut : register(u0);

[numthreads(96, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID, uint gi : SV_GroupIndex )
{
	uint indexInTileVB = gi;
	uint indexOutVB = DTid.x * TILE_VERTEX_COUNT + gi;
	uint indexOutVB = (DTid.y * TILES_IN_X_DIM + DTid.x) * TILE_VERTEX_COUNT + gi;

	const int THREAD_COUNT = 1024;
	int current_index = DTid;
	
	int mesh_vertex_index;
	TileVertex mesh_vertex;
	int current_tile_index;
	while (current_index < gOceanTileCount)
	{
		// All vertices within one ocean tile share the same translation.
		current_tile_index = current_index % gTILE_MESH_VERTEX_COUNT;

		/*
			Fetch data
		*/

		// Fetch my vertex data
		mesh_vertex.position = gTileVertices[current_index % gTILE_MESH_VERTEX_COUNT].xyz;

		// Fetch X and Z position in world for my vertex
		float2 xzPos = gFirstXZ[current_tile_index];



		/*
			Calculate data
		*/

		mesh_vertex.position[0] = xzPos.x;
		mesh_vertex.position[2] = xzPos[1];

		// Calculate Y position, depending on current delta

		// ------ QUICK FIX START; PLEASE CHANGE TO UPDATE CORRECTLY (NOW ONLY SETS A FIXED Y POS)
		mesh_vertex.position[1] = -1.f;
		// ------ QUICK FIX END



		/*
			Place data
		*/

		// Write vertex position to output vertex buffer
		gBufferOut[current_tile_index + gOceanTileCount].position = mesh_vertex.position;



		/*
			Iterate next index, until all indices are placed
		*/

		current_index += THREAD_COUNT;
	}
}