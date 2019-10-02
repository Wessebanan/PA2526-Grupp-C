#include "CollisionSystem.h"

// This hefty boy uses pragma regions to easily look at the system you want.

#pragma region ObjectCollisionSystem
ecs::systems::ObjectCollisionSystem::ObjectCollisionSystem()
{
	updateType = ecs::EventListenerOnly;
	subscribeEventCreation(PotentialCollisionEvent::typeID);
}

ecs::systems::ObjectCollisionSystem::~ObjectCollisionSystem()
{
}

void ecs::systems::ObjectCollisionSystem::onEvent(TypeID _typeID, ecs::BaseEvent* _event)
{
	PotentialCollisionEvent* p_event = dynamic_cast<PotentialCollisionEvent*>(_event);
	
	// Grabbing the entity that moved.
	Entity* p_entity = getEntity(p_event->mEntityID);

	// Grabbing the entity's object collision component 
	// and transform component.
	ObjectCollisionComponent* p_collision = getComponentFromKnownEntity<ObjectCollisionComponent>(p_entity->getID());
	TransformComponent* p_transform = getComponentFromKnownEntity<TransformComponent>(p_entity->getID());

	// Applying the world transform to the collision component min and max.
	DirectX::XMVECTOR collision_min = DirectX::XMLoadFloat3(&p_collision->mMin);
	DirectX::XMVECTOR collision_max = DirectX::XMLoadFloat3(&p_collision->mMax);

	collision_min = DirectX::XMVector3Transform(collision_min, UtilityFunctions::GetWorldMatrix(*p_transform));
	collision_max = DirectX::XMVector3Transform(collision_max, UtilityFunctions::GetWorldMatrix(*p_transform));

	// Grabbing the entities it could collide with.
	TypeFilter filter;
	filter.addRequirement(ObjectCollisionComponent::typeID);
	EntityIterator it = getEntitiesByFilter(filter);

	bool intersect = false;

	for (int i = 0; i < it.entities.size(); i++)
	{
		// Grabbing the collision and transform component from the current entity.
		ObjectCollisionComponent* p_current_collision = getComponentFromKnownEntity<ObjectCollisionComponent>(it.entities.at(i).entity->getID());
		TransformComponent* p_current_transform = getComponentFromKnownEntity<TransformComponent>(it.entities.at(i).entity->getID());

		// Applying the world transform to the current collision component min and max.
		DirectX::XMVECTOR current_collision_min = DirectX::XMLoadFloat3(&p_current_collision->mMin);;
		DirectX::XMVECTOR current_collision_max = DirectX::XMLoadFloat3(&p_current_collision->mMax);;

		current_collision_min = DirectX::XMVector3Transform(current_collision_min, UtilityFunctions::GetWorldMatrix(*p_current_transform));
		current_collision_max = DirectX::XMVector3Transform(current_collision_max, UtilityFunctions::GetWorldMatrix(*p_current_transform));

		// If the objects' bounding volumes intersect.
		if (PhysicsHelpers::AABBIntersect(collision_min, collision_max, current_collision_min, current_collision_max))
		{
			// Set the intersection bool and stop checking
			// because any collision means the movement should revert.
			intersect = true;
			break;
		}
	}
	p_collision->mIntersect = intersect;
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
	TransformComponent* p_transform_component = getComponentFromKnownEntity<TransformComponent>(_entityInfo.entity->getID());
	
	// Creating world matrix and transforming the ground collision component center position to world space.
	DirectX::XMMATRIX ground_collision_world = UtilityFunctions::GetWorldMatrix(*p_transform_component);
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
		
		float distance_to_tile = PhysicsHelpers::CalculateDistance(tile_position, ground_collision_center_position);
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

	// Saving this tile height as the last tile y value if it changed.
	const float ABS_ERROR = pow(10, -10);
	
	// If the height of the nearest tile changed, update and move on.
	if (fabs(tile_height - ground_collision_component->mLastTileY) > ABS_ERROR)
	{
		ground_collision_component->mLastTileY = tile_height;
	}
	// If neither tile height nor y position changed, skip the rest.
	else if (fabs(p_transform_component->position.y - ground_collision_component->mLastY) < ABS_ERROR)
	{
		return;
	}

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

	bool on_ground = false;
	// If the biggest difference is negative, move the object above ground again.
	if (biggest_diff < 0.0f)
	{
		// This should place the ground collision component so that no
		// vertex intersects the ground level.
		p_transform_component->position.y += -biggest_diff;

		// Saving this adjusted height as the last y value.
		ground_collision_component->mLastY = p_transform_component->position.y;
		
		// If the object moved, it is now on the ground level.
		on_ground = true;
	}

	// Break if entity does not move dynamically.
	if (!_entityInfo.entity->hasComponentOfType<DynamicMovementComponent>())
	{
		return;
	}
	DynamicMovementComponent* movement_component = getComponentFromKnownEntity<DynamicMovementComponent>(_entityInfo.entity->getID());
	movement_component->mOnGround = on_ground;
	// If the biggest difference is close to 0, the object is grounded.
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