#include "TrapSystems.h"
#include "../Physics/PhysicsComponents.h"
#include "../Physics/PhysicsEvents.h"
#include "../gameAnimation/AnimationComponents.h"
#include "../gameAI/AIComponents.h"


/*
------------------------------------------------------------
------------------------------------------------------------
------------------------------------------------------------
------------------- UPDATE SYSTEMS -------------------------
------------------------------------------------------------
------------------------------------------------------------
------------------------------------------------------------
*/


ecs::systems::FreezingDurationSystem::FreezingDurationSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::FreezingTimerComponent::typeID);
}

ecs::systems::FreezingDurationSystem::~FreezingDurationSystem()
{
}

void ecs::systems::FreezingDurationSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	FreezingTimerComponent* p_ftimer_comp = _entityInfo.getComponent<components::FreezingTimerComponent>();

	p_ftimer_comp->mElapsedTime += _delta;

	if (p_ftimer_comp->mElapsedTime >= p_ftimer_comp->mDuration)
	{
		TypeID id = p_ftimer_comp->getEntityID();

		components::DynamicMovementComponent* p_move_comp = getComponentFromKnownEntity<DynamicMovementComponent>(id);
		components::AnimationSpeedComponent* p_ani_speed_comp = getComponentFromKnownEntity<AnimationSpeedComponent>(id);

		p_move_comp->mMaxVelocity *= 2.0f;
		p_ani_speed_comp->factor *= 2.0f;

		removeComponent(p_ftimer_comp->getEntityID(), p_ftimer_comp->getTypeID());
	}
}

/*
------------------------------------------------------------
------------------------------------------------------------
------------------------------------------------------------
------------------- EVENT READERS --------------------------
------------------------------------------------------------
------------------------------------------------------------
------------------------------------------------------------
*/




ecs::systems::FireTrapEventSystem::FireTrapEventSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::TriggerFireTrapEvent::typeID);
}

ecs::systems::FireTrapEventSystem::~FireTrapEventSystem()
{
}

void ecs::systems::FireTrapEventSystem::readEvent(BaseEvent& event, float delta)
{
	if (event.getTypeID() == ecs::events::TriggerFireTrapEvent::typeID)
	{
		TypeID id = dynamic_cast<TriggerFireTrapEvent*>(&event)->unitID;

		// Make the unit take damage
		HealthComponent* p_hp_comp = getComponentFromKnownEntity<HealthComponent>(id);
		p_hp_comp->mHealth -= mDamage;


		// Make the unit jump a litte, fire is hot and so am I
		ForceImpulseEvent knockback;
		knockback.mDirection = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		knockback.mForce = mKnockback;
		knockback.mEntityID = id;
		createEvent(knockback);

		// Check if the unit died
		if (p_hp_comp->mHealth <= 0.0f)
		{
			ecs::components::DeadComponent dead_comp;
			ecs::ECSUser::createComponent(id, dead_comp);
		}
	}
}

ecs::systems::FreezeTrapEventSystem::FreezeTrapEventSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::TriggerFreezeTrapEvent::typeID);
}

ecs::systems::FreezeTrapEventSystem::~FreezeTrapEventSystem()
{
}

void ecs::systems::FreezeTrapEventSystem::readEvent(BaseEvent& event, float delta)
{
	if (event.getTypeID() == ecs::events::TriggerFreezeTrapEvent::typeID)
	{
		TypeID id = dynamic_cast<TriggerFreezeTrapEvent*>(&event)->unitID;

		HealthComponent* p_hp_comp = getComponentFromKnownEntity<HealthComponent>(id);
		p_hp_comp->mHealth -= mDamage;

		// Check if the unit died
		if (p_hp_comp->mHealth <= 0.0f)
		{
			ecs::components::DeadComponent dead_comp;
			ecs::ECSUser::createComponent(id, dead_comp);
		}
		else
		{
			components::DynamicMovementComponent* p_move_comp = getComponentFromKnownEntity<DynamicMovementComponent>(id);
			components::AnimationSpeedComponent* p_ani_speed_comp = getComponentFromKnownEntity<AnimationSpeedComponent>(id);

			p_move_comp->mMaxVelocity *= mPower;
			p_ani_speed_comp->factor = mPower;

			FreezingTimerComponent f_comp;
			f_comp.mDuration = 3.0f;
			f_comp.mElapsedTime = 0.0f;

			createComponent(id, f_comp);


			events::ColorSwitchEvent eve;
			eve.mEntityID = id;
			eve.mTime = 3.0f;
			eve.mColor = Color;
		}
	}

}
