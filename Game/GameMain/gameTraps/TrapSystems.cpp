#include "TrapSystems.h"
#include "../Physics/PhysicsComponents.h"
#include "../Physics/PhysicsEvents.h"

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

		getComponentFromKnownEntity<HealthComponent>(id)->mHealth -= mDamage;
		//dsag
		// KNOCKBACK
		ForceImpulseEvent knockback;
		knockback.mDirection = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);//getComponentFromKnownEntity<DynamicMovementComponent>(unit_entity->getID())->mDirection;
		//XMStoreFloat3(&knockback.mDirection, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&weapon_bv->GetCenter()), XMLoadFloat3(&weapon_component->mPreviousPos))));
		knockback.mForce = mKnockback;
		knockback.mEntityID = id;
		createEvent(knockback);
	}
}
