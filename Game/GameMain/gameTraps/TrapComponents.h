#pragma once

#include "ecsComponentIncludes.h"
#include "../GameGlobals.h"
#include "../gameUtility/UtilityComponents.h"

#include "../Physics/PhysicsComponents.h"
#include <DirectXMath.h>

/*
	-- HOW TO ADD A NEW TRAP --																						See file:

	1. Add trap tag in GAME_OBJECT_TYPE, must come after GAME_OBJECT_TYPE_TRAP_OFFSET_TAG in the 'trap block'.		GameGlobals.h
	2. Connect the trap's GAME_OBJECT_TYPE to a specific mesh in InitMeshes().										InitGraphics.h
	3. Create a Trigger____TrapEvent in TrapEvents for the new trap.												TrapEvents.h
	4. Add Trigger____TrapEvent creation in TrapCollisionSystem::updateEntity() switch case.						TrapCollisionSystem.h
	5. Add color for the trap's GAME_OBJECT_TYPE in switch case in TrapSpawnerSystem::readEvent().					TrapSpawnerSystem.cpp
	6. Create ____TrapEventSystem that handle the trap event.														TrapSystems.h / TrapSystems.cpp
*/

namespace ecs
{
	namespace components
	{
		struct TrapComponent : public ECSComponent<TrapComponent>
		{
			float ActivationRateInSeconds;
			float CurrentTimeInSeconds;

			AABB mAABB;
			GAME_OBJECT_TYPE mObjectType;
			ID mTileID;
		};

		// The component pool for this component type will act as a queue for spawning
		// new traps in between prep phase and battle phase.
		struct TrapQueueInfoComponent : public ECSComponent<TrapQueueInfoComponent>
		{
			struct ParticleColor
			{
				char red, green, blue;
			} particleColor;

			components::TrapComponent trapCompInfo;
			components::ColorComponent colorCompInfo;
			components::TransformComponent transfCompInfo;
		};

		// Holds the time of how long the unit has been frozzen and the max time
		struct FreezingTimerComponent : public ecs::ECSComponent<FreezingTimerComponent>
		{
			float mDuration;

			float mElapsedTime = 0;
		};

		struct SpringRetractionComponent : public ecs::ECSComponent<SpringRetractionComponent>
		{
			float mDuration;

			float mElapsedTime = 0;

			float TargetOffsetY;
		};

		struct BurningComponent : public ecs::ECSComponent<BurningComponent>
		{
			float mDuration;

			float mElapsedTime = 0;

			// The damage affected by delta
			float mDamagePerSecond;

			// how often the particles should pulse
			float mPulseInterval;
			// Holds the time between pulses
			float mPulseCounter;
		};

		struct RootComponent : public ecs::ECSComponent<RootComponent>
		{
			// Root duration
			float mDuration;
			// Factor of how much longer they will be immune after getting rooted
			float mImuneFactor;

			float mElapsedTime = 0;

		};
	}
}