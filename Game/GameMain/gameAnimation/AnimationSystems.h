#pragma once
#include "ecsSystemIncludes.h"
#include "AnimationComponents.h"

namespace ecs {

	namespace systems {

		struct SkeletonSystem : public ECSSystem<SkeletonSystem> {
			SkeletonSystem();
			virtual ~SkeletonSystem();

			void updateEntity(FilteredEntity& entity, float delta) override;
		};
	}
}