#pragma once

#include "GraphicsInterface.h"

namespace graphics
{
	inline void SetViewport(
		ID3D11DeviceContext4* pContext4,
		const UINT x, 
		const UINT y,
		const UINT width, 
		const UINT height)
	{
		D3D11_VIEWPORT viewport = { 0 };

		viewport.TopLeftX	= (FLOAT)x;
		viewport.TopLeftY	= (FLOAT)y;
		viewport.Width		= (FLOAT)width;
		viewport.Height		= (FLOAT)height;
		viewport.MinDepth	= 0.0f;
		viewport.MaxDepth	= 1.0f;

		pContext4->RSSetViewports(1, &viewport);
	}

	inline void UploadToDynamicBuffer(
		ID3D11DeviceContext4* pContext4,
		ID3D11Buffer* pBuffer,
		const D3D11_MAP mapType,
		const void* pData,
		const UINT byteWidth,
		const UINT offset)
	{
		D3D11_MAPPED_SUBRESOURCE map;
		pContext4->Map(pBuffer, 0, mapType, 0, &map);
		memcpy((char*)map.pData + offset, pData, byteWidth);
		pContext4->Unmap(pBuffer, 0);
	}

	inline void UploadToDefaultBuffer(
		ID3D11DeviceContext4* pContext4,
		ID3D11Buffer* pBuffer,
		const void* pData,
		const UINT byteWidth)
	{
		pContext4->UpdateSubresource(pBuffer, 0, NULL, pData, byteWidth, 0);
	}

	template <UINT N>
	inline void VSSetConstantBuffers(
		ID3D11DeviceContext4* pContext4,
		ID3D11Buffer** ppBuffers,
		const BufferRegion* pRegions,
		const UINT slot)
	{
		UINT first_constants[N];
		UINT num_constants[N];

		for (UINT i = 0; i < N; i++)
		{
			first_constants[i]	= pRegions[i].Location / 16;
			num_constants[i]	= pRegions[i].Size / 16;
		}

		pContext4->VSSetConstantBuffers1(
			slot,
			N,
			ppBuffers,
			first_constants,
			num_constants);
	}

	template <UINT N>
	inline void PSSetConstantBuffers(
		ID3D11DeviceContext4* pContext4,
		ID3D11Buffer** ppBuffers,
		const BufferRegion* pRegions,
		const UINT slot)
	{
		UINT first_constants[N];
		UINT num_constants[N];

		for (UINT i = 0; i < N; i++)
		{
			first_constants[i] = pRegions[i].Location / 16;
			num_constants[i] = pRegions[i].Size / 16;
		}

		pContext4->PSSetConstantBuffers1(
			slot,
			N,
			ppBuffers,
			first_constants,
			num_constants);
	}

	inline void DrawInstanced(
		ID3D11DeviceContext4* pContext4,
		const UINT instanceCount,
		const UINT instanceStart,
		const BufferRegion& rMeshRegion)
	{
		UINT vertexCountPerInstance = rMeshRegion.Size;
		UINT startVertexLocation = rMeshRegion.Location;

		pContext4->DrawInstanced(
			vertexCountPerInstance,
			instanceCount,
			startVertexLocation,
			instanceStart);
	}

	inline void DrawIndexedInstance(
		ID3D11DeviceContext4* pContext4,
		const UINT instanceCount,
		const UINT instanceStart,
		const BufferRegion& rMeshRegion,
		const BufferRegion& rIndexRegion)
	{
		UINT indexCountPerInstance = rIndexRegion.Size;

		UINT startIndexLocation = rIndexRegion.Location;
		UINT startVertexLocation = rMeshRegion.Location;

		pContext4->DrawIndexedInstanced(
			indexCountPerInstance,
			instanceCount,
			startIndexLocation,
			startVertexLocation,
			instanceStart);
	}

	void DrawMeshes(
		ID3D11DeviceContext4* pContext4,
		ID3D11Buffer* pDataBuffer,
		const UINT dataLocation,
		const UINT dataStride,
		const UINT numMeshes,
		const MeshRegion* pMeshes,
		const UINT* pDrawPerMesh);

	inline void Draw(
		ID3D11DeviceContext4* pContext4,
		const MeshRegion& mesh,
		const UINT count,
		const UINT start)
	{
		switch (mesh.IndexRegion.Size)
		{
			// No indexed region
		case 0:
			DrawInstanced(
				pContext4,
				count,
				start,
				mesh.VertexRegion);
			break;

			// otherwise draw with indexed region
		default:
			DrawIndexedInstance(
				pContext4,
				count,
				start,
				mesh.VertexRegion,
				mesh.IndexRegion);
			break;
		}
	}
}