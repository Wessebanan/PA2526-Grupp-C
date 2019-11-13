#include "TrapSystems.h"
#include "../Physics/PhysicsComponents.h"
#include "../Physics/PhysicsEvents.h"
#include "../gameAI/AIComponents.h"

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
