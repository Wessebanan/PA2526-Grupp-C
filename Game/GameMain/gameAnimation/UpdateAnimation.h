#pragma once
#include "AnimationGlobals.h"

void UpdateAnimation(rendering::RenderManager* pMng, ModelLoader::Mesh* pMesh, unsigned long long int frameCount)
{
	SkinPerObjectData* model_buf = static_cast<SkinPerObjectData*>(pMng->GetTechniqueModelBuffer(rendering::RENDER_SKINNING));

	unsigned int anim_frame = frameCount % pMesh->GetSkeleton()->frameCount;
	DirectX::XMFLOAT4X4* anim_data = pMesh->GetSkeleton()->animationData;
	memcpy(&model_buf->boneMatrices, &anim_data[anim_frame * pMesh->GetSkeleton()->jointCount], pMesh->GetSkeleton()->jointCount * sizeof(DirectX::XMFLOAT4X4));
	int test = 231235;
}