#pragma once

#include <d3d11.h>


struct TileMeshConstBuf
{
	int vertex_count;
	float vertices[60*4];
};

struct LookUpTableConstBuf
{
	unsigned int oceanTileOffset;
	unsigned int oceanTileCount;
	float* xzPositions;
};

void UploadTileMeshToGPU()
{

}