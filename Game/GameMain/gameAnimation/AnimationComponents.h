#pragma once
#include "ecsComponentIncludes.h"
#include "Fbx_Loader.h"
#include "../gameRagdoll/Ragdoll.h"

namespace ecs {

	namespace components {

		struct SkeletonComponent : public ECSComponent<SkeletonComponent>
		{
			ModelLoader::UniqueSkeletonData skeletonData;
			Ragdoll ragdoll;
			bool ragdollEnabled = true;
			Collision* ragdollCollisionObject = nullptr;
			float pingTimeElapsed = -1;
		};

		struct AnimationSpeedComponent : public ECSComponent<AnimationSpeedComponent>
		{
			float factor = 1.0f;
		};
	}
}