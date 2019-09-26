#include "CollisionSystem.h"

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
	if (!entity->hasComponentOfType(TransformComponent::typeID))
	{
		return;
	}

	MeshComponent* mesh_component = dynamic_cast<MeshComponent*>(getComponentFromKnownEntity(MeshComponent::typeID, entity->getID()));
	
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

	// Creating the OBB holding the model.
	DirectX::XMFLOAT3 vertices[8];
	physics_helper_functions::CreateOBB(vertices, min_point, max_point);

	// Calculating the average position while setting
	// the vertices array of GroundCollisionComponent
	// for the center position.
	DirectX::XMFLOAT3 average(0, 0, 0);
	for (int i = 0; i < 8; i++)
	{
		average.x += vertices[i].x;
		average.y += vertices[i].y;
		average.z += vertices[i].z;
		ground_collision_component->mVertices[i] = vertices[i];
	}

	// Dividing by the number of vertices of a box (8) for
	// the center of the box.
	average.x /= 8;
	average.y /= 8;
	average.z /= 8;
	ground_collision_component->mCenterPos = average;

	// NOTE: The center position can be calculated more efficiently,
	// although it would not make a noticable difference.
	// current: 24 additions, 3 divisions. 
	// possible: 3 subtractions, 3 divisons, 3 additions.
	// gain: 21 additions, -3 subtractions.
	// Can fix if you want Mr. Reviewer.
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
	
}
#pragma endregion