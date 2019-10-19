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

		/*
			Initialize with maximum vertex count capacity and index count capacity
		*/
		HRESULT Initialize(
			const UINT vertexCountCapacity,
			const UINT indexCountCapacity);

		/*
			Create a mesh region to store meshes
			: Returns 0 in size if failed
		*/
		graphics::MeshRegion CreateMeshRegion(
			const UINT vertexCount, 
			const UINT indexCount);

		/*
			Sets Vertex Buffer for polling at
				gVertexPositions		: register (t0); 
				gVertexNormals			: register (t1);
				gVertexTexCoords		: register (t2);
				pVertexBlendWeights		: register (t2);
				pVertexBlendIndices		: register (t2);
		*/
		void SetVertexBuffers();

		/*
			Clear all data to enable overwrites
		*/
		void Clear();

		/*
			Upload mesh data to a region
		*/
		HRESULT UploadData(
			const MeshRegion& mesh,
			const VERTEX_DATA& vertexData,
			const void* pIndices);

		void Destroy();

	private:

		HRESULT m_UploadVertexData(
			const UINT index, 
			const UINT stride, 
			const void* pData, 
			const BufferRegion& rMeshRegion);

		HRESULT m_UploadIndexData(
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