#pragma once
#include "ecs.h"
#include "..//Physics/PhysicsComponents.h"
#include "..//gameUtility//UtilityComponents.h"
#include "GridProp.h"
#include "../MeshContainer/MeshContainer.h"
#include "..//gameGameLoop/GameLoopComponents.h"

#define SYSTEM(name) struct name : public ecs::ECSSystem<name>

namespace ecs
{
	namespace systems
	{
		SYSTEM(SpawnLootSystem)
		{
			SpawnLootSystem();
			~SpawnLootSystem();
		
			void act(float _delta) override;
		
		private:
			int mTargetTime;
			int mLastRoundTime;
		};
	} // systems
} // ecs
