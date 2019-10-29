#pragma once
#include "ecsComponentIncludes.h"
#include "Fbx_Loader.h"

namespace ecs {

	namespace components {

		struct SkeletonComponent : public ECSComponent<SkeletonComponent>
		{
			ModelLoader::UniqueSkeletonData skeletonData;
		};
	}
}