#pragma once
#include "ecsSystemIncludes.h"
#include "TrapEvents.h"
#include "TrapComponents.h"
#include <vector>


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
	namespace systems
	{
		// ----------- UPDATE SYSTEMS ---------------

		// Updates the timer on frozzen durations
		class FreezingDurationSystem : public ecs::ECSSystem<FreezingDurationSystem>
		{
		public:
			FreezingDurationSystem();
			~FreezingDurationSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};

		// Updates Tiels that are beeing retracted
		class SpringRetractionSystem : public ecs::ECSSystem<SpringRetractionSystem>
		{
		public:
			SpringRetractionSystem();
			~SpringRetractionSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};

		// ----------- EVENTREADERS -----------------

		class GenericTrapEventSystem : public ecs::ECSSystem<GenericTrapEventSystem>
		{
		public:
			GenericTrapEventSystem();
			~GenericTrapEventSystem();
			void readEvent(BaseEvent& event, float delta) override;
		};

		class FireTrapEventSystem : public ecs::ECSSystem<FireTrapEventSystem>
		{
		public:
			FireTrapEventSystem();
			~FireTrapEventSystem();
			void readEvent(BaseEvent& event, float delta) override;
		private:

			const float mDamage = 20.0f;
			const float mKnockbackAcc = 3.0f;
		};

		class FreezeTrapEventSystem : public ecs::ECSSystem<FreezeTrapEventSystem>
		{
		public:
			FreezeTrapEventSystem();
			~FreezeTrapEventSystem();
			void readEvent(BaseEvent& event, float delta) override;
		private:

			const float mDamage = 10.0f;
			const float mPower = 0.5f; // REMEMBER TO SYNC WITH FREEZING DURATION STSTEM
		};


		class SpringTrapEventSystem : public ecs::ECSSystem<SpringTrapEventSystem>
		{
		public:
			SpringTrapEventSystem();
			~SpringTrapEventSystem();
			void readEvent(BaseEvent& event, float delta) override;
		private:

			const float mKnockbackAcc = 5.0f;
		};

		class SpikeTrapEventSystem : public ecs::ECSSystem<SpikeTrapEventSystem>
		{
		public:
			SpikeTrapEventSystem();
			~SpikeTrapEventSystem();
			void readEvent(BaseEvent& event, float delta) override;
		private:
			const float mKnockbackAcc = 2.0f;
			const float mDamage = 20.0f;
		};
	}
}