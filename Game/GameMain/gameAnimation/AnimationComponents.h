#pragma once
#include "ecsComponentIncludes.h"
#include "Fbx_Loader.h"

namespace ecs {

	namespace components {

		struct SkeletonComponent : public ECSComponent<SkeletonComponent>
		{
			ModelLoader::UniqueSkeletonData skeletonData;
			float pingTimeElapsed = -1;
		};

		struct AnimationSpeedComponent : public ECSComponent<AnimationSpeedComponent>
		{
			float factor = 1.0f;
		};
	}
}