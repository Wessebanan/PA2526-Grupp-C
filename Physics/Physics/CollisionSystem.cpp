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
	updateType = ecs::EventReader;
	typeFilter.addRequirement(CreateComponentEvent::typeID);
}

ecs::systems::GroundCollisionComponentInitSystem::~GroundCollisionComponentInitSystem()
{

}

void ecs::systems::GroundCollisionComponentInitSystem::readEvent(ecs::BaseEvent& _event, float _delta)
{
	// IMPORTANT: Made temporary mesh component in order to make progress.
	CreateComponentEvent create_component_event = dynamic_cast<CreateComponentEvent&>(_event);

	// If the component created was any other than ground collision component, do nothing.
	if (create_component_event.componentTypeID != GroundCollisionComponent::typeID)
	{
		return;
	}

	// Assumes the entity has a mesh component as well as a ground collision component.
	Entity* entity = getEntity(create_component_event.entityID);
	MeshComponent* mesh_component = dynamic_cast<MeshComponent*>(getComponentFromKnownEntity(MeshComponent::typeID, entity->getID()));

	// Do nothing if there is no mesh component.
	if (!mesh_component)
	{
		return;
	}
	GroundCollisionComponent* ground_collision_component = dynamic_cast<GroundCollisionComponent*>(getComponentFromKnownEntity(GroundCollisionComponent::typeID, entity->getID()));

	std::vector<DirectX::XMFLOAT3> *vertex_vector = mesh_component->mMesh.GetVertexPositionVector();
	
	// Creating min and max points to make a box.
	DirectX::XMFLOAT3 min_point(INFINITY, INFINITY, INFINITY);
	DirectX::XMFLOAT3 max_point(-INFINITY, -INFINITY, -INFINITY);

	// Looping over each vertex to see if they contain
	// the lowest or highest value in each axis.
	for (int i = 0; i < vertex_vector->size(); i++)
	{
		DirectX::XMFLOAT3 current = vertex_vector->at(i);
		if (current.x < min_point.x)
		{
			min_point.x = current.x;
		}
		else if (current.x > max_point.x)
		{
			max_point.x = current.x;
		}
		if (current.y < min_point.y)
		{
			min_point.y = current.y;
		}
		else if (current.y > max_point.y)
		{
			max_point.y = current.y;
		}
		if (current.z < min_point.z)
		{
			min_point.z = current.z;
		}
		else if (current.z > max_point.z)
		{
			max_point.z = current.z;
		}
	}

	// Creating the vertices for the box with the min and max points.
	DirectX::XMFLOAT3 vertices[] =
	{
		// Near z:	2 3
		//			0 1
		min_point,
		DirectX::XMFLOAT3(max_point.x, min_point.y, min_point.z),
		DirectX::XMFLOAT3(min_point.x, max_point.y, min_point.z),
		DirectX::XMFLOAT3(max_point.x, max_point.y, min_point.z),

		// Far z:	6 7
		//			4 5
		DirectX::XMFLOAT3(min_point.x, min_point.y, max_point.z),
		DirectX::XMFLOAT3(max_point.x, min_point.y, max_point.z),
		DirectX::XMFLOAT3(min_point.x, max_point.y, max_point.z),
		max_point
	};
	
	for (int i = 0; i < 8; i++)
	{
		ground_collision_component->mVertices[i] = vertices[i];
	}
}