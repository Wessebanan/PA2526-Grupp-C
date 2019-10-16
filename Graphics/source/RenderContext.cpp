
#include "../includes/RenderContext.h"

namespace graphics
{
	void DrawMeshes(
		ID3D11DeviceContext4* pContext4,
		ID3D11Buffer* pDataBuffer,
		const UINT dataLocation,
		const UINT dataStride,
		const UINT numMeshes,
		const MeshRegion* pMeshes,
		const UINT* pInstancesPerMesh)
	{
		BufferRegion bindRegion;
		bindRegion.Location = dataLocation;
		bindRegion.Size		= GPU_CONSTAND_BUFFER_BYTES_PER_BIND;

		VSSetConstantBuffers<1>(
			pContext4,
			&pDataBuffer,
			&bindRegion,
			0);

		UINT start = 0;
		UINT maxBytesPerBind = bindRegion.Size;
		UINT maxMeshes = numMeshes;
		UINT byteWidth = dataStride;

		for (UINT atMesh = 0; atMesh < maxMeshes; atMesh++)
		{
			UINT maxDrawCountPerDraw = maxBytesPerBind / byteWidth;

			// Get Mesh Related Data
			const UINT maxMeshCount = pInstancesPerMesh[atMesh];
			const MeshRegion* pMesh	= &pMeshes[atMesh];

			// Loop till every instance is drawn
			UINT toDraw = maxMeshCount;
			while (toDraw > 0)
			{
				// How many instances to draw
				UINT drawCount = toDraw;

				// if there are too many instances, draw with max possible this pass
				if (drawCount + start > maxDrawCountPerDraw)
					drawCount = maxDrawCountPerDraw - start;

				// Execute draw
				//pMesh->Draw(drawCount, start, pContext4);
				Draw(pContext4, *pMesh, drawCount, start);

				// Decrement with drawn instances
				toDraw -= drawCount;

				// set new start array index for shader to read from
				start += drawCount;

				// when end of shader array index 
				if (start >= maxDrawCountPerDraw)
				{
					// increment data pointer in shader
					bindRegion.Location += maxBytesPerBind;

					VSSetConstantBuffers<1>(
						pContext4,
						&pDataBuffer,
						&bindRegion,
						0);

					// reset shader array index 
					start = 0;
				}
			}
		}
	}
}
