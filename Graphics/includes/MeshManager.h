#pragma once

#include "RenderContext.h"

class Pipeline;

namespace graphics
{
	constexpr UINT VERTEX_BUFFER_COUNT = 5;

	struct VERTEX_DATA
	{
		void *pVertexPositions,
			*pVertexNormals,
			*pVertexTexCoords,
			*pVertexBlendWeights,
			*pVertexBlendIndices;
	};

	class MeshManager
	{
		friend class Pipeline;

	public:
		MeshManager();
		~MeshManager();

		HRESULT Initialize(
			const UINT vertexCountCapacity,
			const UINT indexCountCapacity);

		MeshRegion CreateMeshRegion(const UINT vertexCount, const UINT indexCount);

		void EnableVertexBuffers();
		void Destroy();

		void UploadData(
			const MeshRegion& mesh,
			const VERTEX_DATA& vertexData,
			const void* pIndices);

	private:
		void m_UploadVertexData(
			const UINT index, 
			const UINT stride, 
			const void* pData, 
			const BufferRegion& rMeshRegion);

		void m_UploadIndexData(
			const void* pData,
			const BufferRegion& rIndexRegion);

		UINT m_sizeV, m_capacityV; // Vertex Counts
		UINT m_sizeI, m_capacityI; // Index Counts

		ID3D11Buffer* m_pVertexBuffers[VERTEX_BUFFER_COUNT];
		ID3D11ShaderResourceView* m_pVertexBufferViews[VERTEX_BUFFER_COUNT];

		ID3D11Buffer* m_pIndexBuffer;

		ID3D11Device4* m_pDevice4;
		ID3D11DeviceContext4* m_pContext4;
	};
}