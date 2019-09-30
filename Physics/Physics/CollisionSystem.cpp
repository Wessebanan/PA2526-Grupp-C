#include "CollisionSystem.h"

// This hefty boy uses pragma regions to easily look at the system you want.

#pragma region CollisionEventSystem
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
#pragma endregion
#pragma region GroundCollisionComponentInitSystem
ecs::systems::GroundCollisionComponentInitSystem::GroundCollisionComponentInitSystem()
{
	updateType = ecs::EventListenerOnly;
	subscribeEventCreation(CreateComponentEvent::typeID);
}

ecs::systems::GroundCollisionComponentInitSystem::~GroundCollisionComponentInitSystem()
{

}

void ecs::systems::GroundCollisionComponentInitSystem::onEvent(TypeID _typeID, ecs::BaseEvent *_event)
{
	// IMPORTANT: Made temporary mesh component in order to make progress.
	CreateComponentEvent *create_component_event = dynamic_cast<CreateComponentEvent*>(_event);

	// If the component created was any other than ground collision component, do nothing.
	if (create_component_event->componentTypeID != GroundCollisionComponent::typeID)
	{
		return;
	}

	// Assumes the entity has a mesh component, transform component and ground collision component.
	// Check for ground collision component is already made.
	Entity* entity = getEntity(create_component_event->entityID);

	if (!entity->hasComponentOfType(MeshComponent::typeID))
	{
		return;
	}

	MeshComponent* mesh_component = dynamic_cast<MeshComponent*>(getComponentFromKnownEntity(MeshComponent::typeID, entity->getID()));
	
	GroundCollisionComponent* ground_collision_component = dynamic_cast<GroundCollisionComponent*>(getComponentFromKnownEntity(GroundCollisionComponent::typeID, entity->getID()));

	std::vector<DirectX::XMFLOAT3> *vertex_vector = mesh_component->mMesh.GetVertexPositionVector();

	// Getting the extreme points of the vertex group.
	DirectX::XMFLOAT3 min_point;
	DirectX::XMFLOAT3 max_point;
	PhysicsHelpers::GetExtremes(vertex_vector->data(), vertex_vector->size(), min_point, max_point);

	// Creating the OBB holding the model.
	DirectX::XMFLOAT3 vertices[8];
	PhysicsHelpers::CreateOBB(vertices, min_point, max_point);

	// Calculating the average position while setting
	// the vertices array of GroundCollisionComponent
	// for the center position.
	DirectX::XMFLOAT3 average(0, 0, 0);
	for (int i = 0; i < 8; i++)
	{
		ground_collision_component->mVertices[i] = vertices[i];
	}
	
	ground_collision_component->mCenterPos = DirectX::XMFLOAT3
	(
		(min_point.x + max_point.x) / 2.0f,
		(min_point.y + max_point.y) / 2.0f,
		(min_point.z + max_point.z) / 2.0f
	);
}
#pragma endregion
#pragma region GroundCollisionSystem
ecs::systems::GroundCollisionSystem::GroundCollisionSystem() 
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(GroundCollisionComponent::typeID);
}

ecs::systems::GroundCollisionSystem::~GroundCollisionSystem() 
{

}

void ecs::systems::GroundCollisionSystem::updateEntity(FilteredEntity& _entityInfo, float _delta) 
{
	// Grabbing all entities with a tile component (tiles).
	EntityIterator it = getEntitiesWithComponent<TileComponent>();

	// Grabbing the ground collision component from the current entity.
	GroundCollisionComponent* ground_collision_component = getComponentFromKnownEntity<GroundCollisionComponent>(_entityInfo.entity->getID());
	//DirectX::XMFLOAT3 center_position = p_ground_collision_component->mCenterPos;

	// Checking that the ground collision entity also has a transform component.
	if (!_entityInfo.entity->hasComponentOfType<TransformComponent>())
	{
		return;
	}
	TransformComponent* ground_collision_transform = getComponentFromKnownEntity<TransformComponent>(_entityInfo.entity->getID());
	
	// Creating world matrix and transforming the ground collision component center position to world space.
	DirectX::XMMATRIX ground_collision_world = UtilityFunctions::GetWorldMatrix(*ground_collision_transform);
	DirectX::XMVECTOR ground_collision_center_position = DirectX::XMLoadFloat3(&ground_collision_component->mCenterPos);
	ground_collision_center_position = DirectX::XMVector3Transform(ground_collision_center_position, ground_collision_world);


	// Holds the index of the closest tile.
	TypeID closest_tile_id = -1;

	// Distance to closest tile.
	float closest_distance = INFINITY;

	// For each tile, check which is closest to the ground collision component.
	// This is bad, and will likely be optimized in the future.
	
	for (int i = 0; i < it.entities.size(); i++)
	{		
		TypeID current_tile = it.entities.at(i).entity->getID();

		// Check that the tile has a transform component before trying to use it.
		if (!it.entities.at(i).entity->hasComponentOfType<TransformComponent>())
		{
			return;
		}
		TransformComponent* tile_transform = getComponentFromKnownEntity<TransformComponent>(current_tile);

		// Using XMVECTOR bs to get distance to tile. May change this in the future but just something that works for now.
		DirectX::XMVECTOR tile_position = DirectX::XMLoadFloat3(&tile_transform->position);
		DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(tile_position, ground_collision_center_position);
		DirectX::XMVECTOR diff_length = DirectX::XMVector3Length(diff);
		float distance_to_tile = 0.0f;
		DirectX::XMStoreFloat(&distance_to_tile, diff_length);
		
		// Setting the ID to the closest tile.
		if (closest_distance > distance_to_tile)
		{
			closest_distance = distance_to_tile;
			closest_tile_id = current_tile;
		}
	}

	// Grabbing the closest tile.
	TransformComponent* closest_tile = getComponentFromKnownEntity<TransformComponent>(closest_tile_id);

	// Grabbing the height (y value).
	float tile_height = closest_tile->position.y;

	// Find the vertex with highest negative difference to tile height.
	// This is for movement out of the ground.
	float biggest_diff = INFINITY;
	for (int i = 0; i < 8; i++)
	{
		// Transforming the current vertex to world space and grabbing the height.
		DirectX::XMVECTOR current_vertex = DirectX::XMLoadFloat3(&ground_collision_component->mVertices[i]);
		current_vertex = DirectX::XMVector3Transform(current_vertex, ground_collision_world);
		float vertex_height = DirectX::XMVectorGetY(current_vertex);
		
		// Finding biggest difference between vertex and tile height.
		float diff = vertex_height - tile_height;
		if (diff < biggest_diff)
		{
			biggest_diff = diff;
		}
	}
	
	// If the biggest difference is negative, move the object above ground again.
	if (biggest_diff < 0.0f)
	{
		// This should place the ground collision component so that no
		// vertex intersects the ground level.
		ground_collision_transform->position.y += -biggest_diff;
	}
}
#pragma endregion
#pragma region ObjectBoundingVolumeInitSystem
ecs::systems::ObjectBoundingVolumeInitSystem::ObjectBoundingVolumeInitSystem()
{
	updateType = ecs::EventListenerOnly;
	subscribeEventCreation(CreateComponentEvent::typeID);
}

ecs::systems::ObjectBoundingVolumeInitSystem::~ObjectBoundingVolumeInitSystem()
{

}

void ecs::systems::ObjectBoundingVolumeInitSystem::onEvent(TypeID _typeID, ecs::BaseEvent* _event)
{
	CreateComponentEvent* create_component_event = dynamic_cast<CreateComponentEvent*>(_event);
	
	// If the component created was any other than object collision component, do nothing.
	if (create_component_event->componentTypeID != ObjectCollisionComponent::typeID)
	{
		return;
	}

	Entity* entity = getEntity(create_component_event->entityID);
	
	// Checking that there is a mesh to compute a bounding volume from.
	if (!entity->hasComponentOfType(MeshComponent::typeID))
	{
		return;
	}

	// Grabbing the mesh component and in turn the vertex list.
	MeshComponent* mesh_component = getComponentFromKnownEntity<MeshComponent>(entity->getID());
	std::vector<DirectX::XMFLOAT3> *vertex_list = mesh_component->mMesh.GetVertexPositionVector();

	// Getting the extreme values of the vertex list.
	DirectX::XMFLOAT3 min_point, max_point;
	PhysicsHelpers::GetExtremes(vertex_list->data(), vertex_list->size(), min_point, max_point);

	// Grabbing the object collision component to fill it up.
	ObjectCollisionComponent* object_collision_component = getComponentFromKnownEntity<ObjectCollisionComponent>(entity->getID());

	object_collision_component->mMin = min_point;
	object_collision_component->mMax = max_point;

	// min + max / 2 gives us the center.
	object_collision_component->mCenter = DirectX::XMFLOAT3
	(
		(min_point.x + max_point.x) / 2.0f,
		(min_point.y + max_point.y) / 2.0f,
		(min_point.z + max_point.z) / 2.0f
	);

}
#pragma endregion