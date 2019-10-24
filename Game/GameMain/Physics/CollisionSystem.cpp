#include "CollisionSystem.h"
#include "..//gameAudio/AudioECSEvents.h"
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
	ObjectCollisionComponent* p_collision	= getComponentFromKnownEntity<ObjectCollisionComponent>(p_entity->getID());
	TransformComponent* p_transform			= getComponentFromKnownEntity<TransformComponent>(p_entity->getID());
	DynamicMovementComponent* p_movement	= getComponentFromKnownEntity<DynamicMovementComponent>(p_entity->getID());

	// Grabbing a copy of AABB and transforming to world space.
	AABB aabb = p_collision->mAABB;
	XMMATRIX world_transform = UtilityEcsFunctions::GetWorldMatrix(*p_transform);
	aabb.BoundingBox::Transform(aabb, world_transform);
	XMVECTOR center = XMLoadFloat3(&aabb.Center);

	// Grabbing the entities it could collide with.
	TypeFilter filter;
	filter.addRequirement(ObjectCollisionComponent::typeID);
	filter.addRequirement(TransformComponent::typeID);
	EntityIterator it = getEntitiesByFilter(filter);

	bool intersect = false;

	for (int i = 0; i < it.entities.size(); i++)
	{
		ID current_entity_id = it.entities.at(i).entity->getID();
		// Skip yourself.
		if (current_entity_id == p_entity->getID())
		{
			continue;
		}
		// Grabbing the collision and transform component from the current entity.
		ObjectCollisionComponent* p_current_collision = getComponentFromKnownEntity<ObjectCollisionComponent>(current_entity_id);
		TransformComponent* p_current_transform = getComponentFromKnownEntity<TransformComponent>(current_entity_id);
		
		// Grabbing copy of AABB from current and transforming to world space.
		AABB current_aabb = p_current_collision->mAABB;
		XMMATRIX current_world_transform = UtilityEcsFunctions::GetWorldMatrix(*p_current_transform);
		current_aabb.BoundingBox::Transform(current_aabb, current_world_transform);

		// If the objects' bounding volumes intersect.
		if(aabb.BoundingBox::Intersects(current_aabb))
		{
			// Set the intersection bool because it may be useful.
			intersect = true;

			XMFLOAT3 velocity_pre_revert = p_movement->mVelocity;

			XMVECTOR colliding_center = XMLoadFloat3(&current_aabb.Center);
			RevertMovement(p_transform->position, p_movement->mVelocity, &aabb, &current_aabb, p_event->mDelta);

			if (getEntity(current_entity_id)->hasComponentOfType(DynamicMovementComponent::typeID))
			{
				DynamicMovementComponent* colliding_movement = getComponentFromKnownEntity<DynamicMovementComponent>(current_entity_id);
				colliding_movement->mVelocity.x += (velocity_pre_revert.x - p_movement->mVelocity.x);
				colliding_movement->mVelocity.y += (velocity_pre_revert.y - p_movement->mVelocity.y);
				colliding_movement->mVelocity.z += (velocity_pre_revert.z - p_movement->mVelocity.z);
			}

			// Transforming the aabb again since the position has changed.
			world_transform = UtilityEcsFunctions::GetWorldMatrix(*p_transform);
			aabb.BoundingBox::Transform(aabb, world_transform);
			center = XMLoadFloat3(&aabb.Center);
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
	CreateComponentEvent* create_component_event = dynamic_cast<CreateComponentEvent*>(_event);

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

	std::vector<DirectX::XMFLOAT3> *vertex_vector = mesh_component->mMesh->GetVertexPositionVector();

	ground_collision_component->mOBB.CreateFromPoints(ground_collision_component->mOBB, vertex_vector->size(), vertex_vector->data(), sizeof(XMFLOAT3));
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
	if (mppTileTransforms)
	{
		delete[] mppTileTransforms;
	}
}

void ecs::systems::GroundCollisionSystem::updateEntity(FilteredEntity& _entityInfo, float _delta) 
{
	// On first update, fill tile info members of system.
	if (!mInit)
	{
		mTiles = getEntitiesWithComponent<TileComponent>();
		
		// Grabbing sile of entityiterator to check if the grid is created.
		mTileCount = mTiles.entities.size();
		if (mTileCount != 0)
		{
			mInit = true;
			mppTileTransforms = new TransformComponent * [mTileCount];
			for (int i = 0; i < mTileCount; i++)
			{
				mppTileTransforms[i] = getComponentFromKnownEntity<TransformComponent>(mTiles.entities.at(i).entity->getID());
			}
		}
		// If there is no grid, skip ground collision.
		else
		{
			return;
		}
	}

	// Grabbing the ground collision component from the current entity.
	GroundCollisionComponent* ground_collision_component = getComponentFromKnownEntity<GroundCollisionComponent>(_entityInfo.entity->getID());

	// Checking that the ground collision entity also has a transform component.
	if (!_entityInfo.entity->hasComponentOfType<TransformComponent>())
	{
		return;
	}
	TransformComponent* p_transform_component = getComponentFromKnownEntity<TransformComponent>(_entityInfo.entity->getID());
	
	// Creating world matrix and transforming the ground collision component to world space.
	DirectX::XMMATRIX ground_collision_world = UtilityEcsFunctions::GetWorldMatrix(*p_transform_component);

	// Grabbing a copy of OBB and transforming to world space.
	BoundingOrientedBox obb = ground_collision_component->mOBB;
	obb.Transform(obb, ground_collision_world);

	// Holds the index of the closest tile.
	TypeID closest_tile_id = -1;

	// Distance to closest tile.
	float closest_distance = INFINITY;

	// For each tile, check which is closest to the ground collision component.
	// This is bad, and will likely be optimized in the future.	
	for (int i = 0; i < mTiles.entities.size(); i++)
	{		
		TypeID current_tile = mTiles.entities.at(i).entity->getID();

		TransformComponent* tile_transform = mppTileTransforms[i];
		
		XMFLOAT3 obb_center_no_height = obb.Center;
		obb_center_no_height.y = 0.0f;
		XMFLOAT3 tile_transform_position_no_height = tile_transform->position;
		tile_transform_position_no_height.y = 0.0f;

		float distance_to_tile = PhysicsHelpers::CalculateDistance(tile_transform_position_no_height, obb_center_no_height);
		// Setting the ID to the closest tile.
		if (closest_distance > distance_to_tile)
		{
			closest_distance = distance_to_tile;
			closest_tile_id = current_tile;
		}
	}
	TransformComponent* closest_tile = getComponentFromKnownEntity<TransformComponent>(closest_tile_id);

	// Getting closest tile to unit.
	//XMFLOAT3 unit_position = p_transform_component->position;
	//int2 closest_tile_index = GridFunctions::GetTileFromWorldPos(unit_position.x, unit_position.z);
	//
	//GridProp* grid_prop = GridProp::GetInstance();
	//TileData tile_data = grid_prop->mGrid[closest_tile_index.y][closest_tile_index.x];
	//
	//// Grabbing the closest tile.
	//TransformComponent* closest_tile = getComponentFromKnownEntity<TransformComponent>(tile_data.Id);
	
	// Grabbing the height (y value).
	float tile_height = closest_tile->position.y;

	// Saving this tile height as the last tile y value if it changed.
	const float ABS_ERROR = (float)pow(10, -10);
	
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
	XMFLOAT3* obb_corners = new XMFLOAT3[8];
	obb.GetCorners(obb_corners);
	for (int i = 0; i < 8; i++)
	{
		float diff = obb_corners[i].y - tile_height;
		if (diff < biggest_diff)
		{
			biggest_diff = diff;
		}
	}
	delete[] obb_corners;

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

		// TEMP GROUND HIT SOUND
		ecs::events::PlaySound sound_event;
		sound_event.audioName = AudioName::COIN_TEST_SOUND;
		sound_event.soundFlags = SoundFlags::SF_NONE;
		createEvent(sound_event);
	}

	// Break if entity does not move dynamically.
	if (!_entityInfo.entity->hasComponentOfType<DynamicMovementComponent>())
	{
		return;
	}
	DynamicMovementComponent* movement_component = getComponentFromKnownEntity<DynamicMovementComponent>(_entityInfo.entity->getID());
	// If the biggest difference is close to 0, the object is grounded.
	movement_component->mOnGround = on_ground;
	if (on_ground)
	{
		movement_component->mVelocity.y = 0.0f;
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
	std::vector<DirectX::XMFLOAT3> *vertex_list = mesh_component->mMesh->GetVertexPositionVector();

	// Grabbing the object collision component to fill it up.
	ObjectCollisionComponent* object_collision_component = getComponentFromKnownEntity<ObjectCollisionComponent>(entity->getID());
	

	object_collision_component->mAABB.CreateFromPoints(object_collision_component->mAABB, vertex_list->size(), vertex_list->data(), sizeof(XMFLOAT3));
	
	/** TODO:
	* Set ObjectCollisionComponent::mSphereCount to number of bones.
	* Allocate memory for mSphereCount number of DirectX::BoundingSphere(s).
	* For each bone, grab vertex cluster and create spheres from clusters.
	*/

}
#pragma endregion
#pragma region FillQuadTreeSystemRegion
ecs::systems::FillQuadTreeSystem::FillQuadTreeSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(components::TransformComponent::typeID);
	typeFilter.addRequirement(components::ObjectCollisionComponent::typeID);
}

ecs::systems::FillQuadTreeSystem::~FillQuadTreeSystem()
{

}

void ecs::systems::FillQuadTreeSystem::updateEntity(FilteredEntity& entity, float delta)
{
	ecs::ComponentIterator it = ecs::ECSUser::getComponentsOfType(ecs::components::QuadTreeComponent::typeID);
	ecs::components::QuadTreeComponent* p_tree = static_cast<ecs::components::QuadTreeComponent*>(it.next());
	ecs::components::TransformComponent* p_transform = entity.getComponent<ecs::components::TransformComponent>();
	ecs::components::ObjectCollisionComponent* p_collision = entity.getComponent<ecs::components::ObjectCollisionComponent>();
	static_cast<QuadTree*>(p_tree->pTree)->Insert(QuadTreeObject(p_transform, p_collision));
}
#pragma endregion