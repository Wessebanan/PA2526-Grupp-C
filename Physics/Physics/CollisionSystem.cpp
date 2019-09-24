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

ecs::systems::GroundCollisionComponentInitSystem::GroundCollisionComponentInitSystem()
{
	updateType = ecs::EventListenerOnly;
	typeFilter.addRequirement(GroundCollisionComponentCreateEvent::typeID);
}

ecs::systems::GroundCollisionComponentInitSystem::~GroundCollisionComponentInitSystem()
{

}

void ecs::systems::GroundCollisionComponentInitSystem::readEvent(ecs::BaseEvent& _event, float _delta)
{
	// IMPORTANT: Made temporary mesh component in order to make progress.
	GroundCollisionComponentCreateEvent ground_collision_component_create_event = dynamic_cast<GroundCollisionComponentCreateEvent&>(_event);

	// Assumes the entity has a mesh component as well as a ground collision component.
	Entity* entity = getEntity(ground_collision_component_create_event.mEntityID);
	MeshComponent* mesh_component = dynamic_cast<MeshComponent*>(getComponentFromKnownEntity(MeshComponent::typeID, entity->getID()));
	GroundCollisionComponent* ground_collision_component = dynamic_cast<GroundCollisionComponent*>(getComponentFromKnownEntity(GroundCollisionComponent::typeID, entity->getID()));

	std::vector<DirectX::XMFLOAT3> *vertex_vector = mesh_component->mMesh.GetVertexPositionVector();
	
}