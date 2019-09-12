#pragma once

#include "D3D11Header.h"

struct MeshRegion
{
	UINT
		VertexCount,
		VertexLocation;
};

class MeshHeap : public GraphicsInterface
{
	friend class RenderContext;
	friend class DeviceInterface;

public:
	MeshHeap();
	~MeshHeap();

	void AppendVertexData(
		const UINT count,
		const void* pVertices,
		const void* pNormals,
		const void* pUVs,
		MeshRegion* pRegion);

private:

	void Initialize(ID3D11Device4* pDevice4, const UINT elements);
	void Release();

	D3D11_PRIMITIVE_TOPOLOGY m_topology;
	ID3D11Buffer* m_pVertexBuffers[3];

	UINT m_offsets[3], m_strides[3];

	UINT m_vertexCount[10];

	UINT m_count;

	char m_vertices[1024];
	char* m_pVerticesAt;

	char m_uv[1024];
	char* m_pUVAt;

	UINT m_currentLocation;
};
