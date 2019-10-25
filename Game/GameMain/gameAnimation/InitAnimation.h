#pragma once
#include "Mesh.h"
#include "AnimationSystems.h"
#include "../MeshContainer/MeshContainer.h"
struct SkinningShaderProgramInput
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 boneMatrices[63];
};

static void UpdateAnimation(ModelLoader::Skeleton* skeleton, SkinningShaderProgramInput* skin_shader_program_input, UINT frame)
{
	for (unsigned int i = 0; i < 12; ++i)
	{
		// Avoid frame overflow
		frame = frame % skeleton->frameCount;

		memcpy(skin_shader_program_input[i].boneMatrices, &skeleton->animationData[frame * skeleton->jointCount], skeleton->jointCount * sizeof(DirectX::XMFLOAT4X4));

	}
}


static void InitAnimation(ecs::EntityComponentSystem& rECS)
{
	rECS.createSystem<ecs::systems::SkeletonSystem>();
	ecs::TypeFilter skeleton_filter;
	skeleton_filter.addRequirement(ecs::components::SkeletonComponent::typeID);
	ecs::EntityIterator skeletons = rECS.getEntititesByFilter(skeleton_filter);
	int counter = 1;
	for (ecs::FilteredEntity s : skeletons.entities)
	{	
		ModelLoader::UniqueSkeletonData* skeletonData = &s.getComponent<ecs::components::SkeletonComponent>()->skeletonData;
		skeletonData->Init(MeshContainer::GetMeshCPU(MESH_TYPE::MESH_TYPE_UNIT)->GetSkeleton());
		skeletonData->StartAnimation(ModelLoader::ANIMATION_TYPE::IDLE);
		counter++;
	}
}
