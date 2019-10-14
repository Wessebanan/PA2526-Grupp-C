#pragma once

#include "rendering/RenderManager.h"
#include "Mesh.h"
#include "AnimationGlobals.h"

void UpdateAnimations(ModelLoader::Mesh* pMesh, rendering::RenderManager* pMng, unsigned int currentFrame)
{
	void* model_buf = pMng->GetTechniqueModelBuffer(rendering::RENDER_SKINNING);
	AnimatedModelBuffer* mbuf = static_cast<AnimatedModelBuffer*>(model_buf);
	unsigned int count = 0;
	// Frame overflow check
	int curr_frame = currentFrame % pMesh->GetSkeleton()->joints[0].mAnimationVector.size();
	for (auto a : pMesh->GetSkeleton()->joints)
	{
		DirectX::XMFLOAT4X4 newMat;
		// Convert FbxMatrix to XMFLOAT
		// VERY TEMPORARY SOLUTION
		// SHOULD NOT BE HERE FOR VERY LONG
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				newMat.m[i][j] = static_cast<float>(a.mAnimationVector[curr_frame].mOffsetMatrix.Get(i, j));
			}
		}
		mbuf[0].boneMatrices[count] = newMat;
		count++;
	}
}