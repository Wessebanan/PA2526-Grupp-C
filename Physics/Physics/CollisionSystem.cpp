#include "CollisionSystem.h"

ecs::systems::CollisionEventSystem::CollisionEventSystem()
{
	updateType = ecs::EventListenerOnly;
	typeFilter.addRequirement(PotentialCollisionEvent::typeID);
}

ecs::systems::CollisionEventSystem::~CollisionEventSystem()
{
}

void ecs::systems::CollisionEventSystem::readEvent(ecs::BaseEvent& _event, float _delta)
{

}