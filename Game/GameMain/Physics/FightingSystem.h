#pragma once
#include "ecs.h"

#include "PhysicsComponents.h"
#include "PhysicsEvents.h"
#include "PhysicsHelperFunctions.h"
#include "..//gameUtility/UtilityComponents.h"
#include "..//gameUtility/UtilityEcsFunctions.h"
#include "..//gameAnimation/AnimationComponents.h"
#include "..//gameAI/AIComponents.h"
#include "..//gameAudio/AudioECSEvents.h"
#include "..//gameAudio/AudioGlobals.h"
#include <unordered_map>

#define SYSTEM(name) struct name : public ecs::ECSSystem<name>

namespace ecs
{
	namespace systems
	{
		/** Weapon Init System:
		* Triggers when a weapon component is created
		* and initializes provided the entity has a mesh 
		* component and a transform component before. Use
		* "CreateWeaponEntity" from Physics/InitPhysics.h
		* to make sure the weapon component is created properly.
		*/
		SYSTEM(WeaponInitSystem)
		{
			WeaponInitSystem();
			~WeaponInitSystem();
			void onEvent(TypeID _typeID, ecs::BaseEvent * _event) override;
		};


		/** Damage System:
		* Checks collision of each weapon with each entity that has a 
		* health component, applies damage to each collided entity based
		* on weapon properties and velocity.
		*/
		SYSTEM(DamageSystem)
		{
			DamageSystem();
			~DamageSystem();
			void updateEntity(FilteredEntity & _entityInfo, float _delta) override;
		};

		SYSTEM(UnitColorSwitchSystem)
		{
			UnitColorSwitchSystem();
			~UnitColorSwitchSystem();
			void onEvent(TypeID _typeID, ecs::BaseEvent * _event) override;
			void updateEntity(FilteredEntity & _entity, float _delta) override;
		private:			
			std::unordered_map<ID, float> mTimers;
		};

		/** Unit Invincibility Timer System:
		* Handles invincibility components given to units when damage is taken 
		* by deducting frame time from them and removing them when the time is up.
		*/
		SYSTEM(UnitInvincibilityTimerSystem)
		{
			UnitInvincibilityTimerSystem();
			~UnitInvincibilityTimerSystem();			
			void updateEntity(FilteredEntity & _entity, float _delta) override;		
		};

		SYSTEM(WeaponOnHitSystem)
		{
			WeaponOnHitSystem();
			~WeaponOnHitSystem();

			void readEvent(BaseEvent & _event, float _delta) override;
		};

		SYSTEM(HealthCheckSystem)
		{
			HealthCheckSystem();
			~HealthCheckSystem();

			void updateEntity(FilteredEntity& _entity, float _delta) override;

		private:
			unordered_map<ID, float> mHealths;
		};
	} // systems
} // ecs
