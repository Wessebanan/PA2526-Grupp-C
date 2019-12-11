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
	ID entity_id = p_event->mEntityID;
	Entity* p_entity = getEntity(entity_id);
	// Any entity creating a potential collision event should have these three components, 
	// if not it would break in this function, so return.
	if (!p_entity->hasComponentOfType<ObjectCollisionComponent>() || 
		!p_entity->hasComponentOfType<TransformComponent>() || 
		!p_entity->hasComponentOfType<DynamicMovementComponent>())
	{
		return;
	}

	// Grabbing the entity's object collision component 
	// and transform component.
	ObjectCollisionComponent*	p_collision	= getComponentFromKnownEntity<ObjectCollisionComponent>(entity_id);
	TransformComponent*			p_transform	= getComponentFromKnownEntity<TransformComponent>(entity_id);
	DynamicMovementComponent*	p_movement	= getComponentFromKnownEntity<DynamicMovementComponent>(entity_id);
	SkeletonComponent*			p_skeleton	= getComponentFromKnownEntity<SkeletonComponent>(entity_id);

	// Grabbing a copy of moving object's bounding volume.
	
	BoundingVolume* p_bv_copy = p_collision->mBV->Copy();

	XMMATRIX world_transform = UtilityEcsFunctions::GetWorldMatrix(*p_transform);
	p_bv_copy->Transform(world_transform);

	QuadTreeObject this_object(p_transform, p_collision);
	std::vector<QuadTreeObject> collision_list;
	QuadTreeComponent *p_quad_tree_component = static_cast<QuadTreeComponent*>(getComponentsOfType<QuadTreeComponent>().next());
	QuadTree *quad_tree = static_cast<QuadTree*>(p_quad_tree_component->pTree);

	quad_tree->RetrieveCollisions(collision_list, this_object);

	if (p_skeleton)
	{
		if (p_skeleton->ragdollEnabled)
		{
			if (p_skeleton->ragdollCollisionObject)
			{
				// Remove the object from the previous frame
				delete p_skeleton->ragdollCollisionObject;
			}
			p_skeleton->ragdollCollisionObject = new Collision();
			for (auto a : collision_list)
			{
				p_skeleton->ragdollCollisionObject->AddBoundingVolume(a.pBoundingBox->mBV);
			}

		}
	}

	bool intersect = false;
	bool on_ground = false;

	std::vector<CollisionInfo> collisions;

	for (int i = 0; i < collision_list.size(); i++)
	{
		ID current_entity_id = collision_list.at(i).pTransform->getEntityID();
		QuadTreeObject current_qt_object = collision_list.at(i);
		
		// Skip yourself.
		if (current_entity_id == entity_id)
		{
			continue;
		}
		
		// Grabbing the collision and transform component from the current entity.
		ObjectCollisionComponent* p_current_collision	= current_qt_object.pBoundingBox;
		TransformComponent* p_current_transform			= current_qt_object.pTransform;
		XMMATRIX current_world_transform = UtilityEcsFunctions::GetWorldMatrix(*p_current_transform);
		
		// Grabbing copy of BV from current and transforming to world space.
		// And checking intersection.
		CollisionInfo info;

		BoundingVolume* p_current_copy = p_current_collision->mBV->Copy();
		p_current_copy->Transform(current_world_transform);
		if (p_bv_copy->Intersects(p_current_copy))
		{
			info = p_bv_copy->GetCollisionInfo(p_current_copy);
		}
		delete p_current_copy;
	
		// If the objects' bounding volumes intersect.
		if(info.mOverlap > 0.0f)
		{
			// Set the intersection bool because it may be useful. :)
			intersect = true;
			
			// Dot < 0.0f means moving towards collided object.
			XMVECTOR velocity_direction = XMVector3Normalize(XMLoadFloat3(&p_movement->mVelocity));
			float dot = XMVectorGetX(XMVector3Dot(velocity_direction, XMLoadFloat3(&info.mNormal)));

			// If the collided object is a tile.
			bool is_tile = getEntity(current_entity_id)->hasComponentOfType<TileComponent>();
			if (is_tile)
			{
				TileComponent* p_tile_comp = getComponentFromKnownEntity<TileComponent>(current_entity_id);
				
				// If the unit hit water, he gonna die so no more collision haha.
				if (p_tile_comp->tileType == WATER)
				{
					delete p_bv_copy;
					return;
				}			
			}

			// If tile, always resolve, otherwise resolve if moving towards object.
			if (dot < 0.0f || is_tile)
			{
				XMFLOAT3 resolution_movement = XMFLOAT3
				(
					info.mNormal.x * info.mOverlap,
					info.mNormal.y * info.mOverlap,
					info.mNormal.z * info.mOverlap
				);

				// Reverting movement.
				p_transform->position.x += resolution_movement.x;
				p_transform->position.y += resolution_movement.y;
				p_transform->position.z += resolution_movement.z;

				// Translating moving BV to new position.
				XMMATRIX resolution_translation = XMMatrixTranslation(resolution_movement.x, resolution_movement.y, resolution_movement.z);
				p_bv_copy->Transform(resolution_translation);

				// If collided object has movement, give it some of colliding units velocity.
				DynamicMovementComponent* p_current_movement = getComponentFromKnownEntity<DynamicMovementComponent>(current_entity_id);
				if (p_current_movement)
				{
					p_current_movement->mVelocity.x += 0.2f * p_movement->mVelocity.x;
					p_current_movement->mVelocity.y += 0.2f * p_movement->mVelocity.y;
					p_current_movement->mVelocity.z += 0.2f * p_movement->mVelocity.z;
				}
				
				// Find largest component of collision normal and set velocity in that direction to -0.5 of current (BOUNCE).
				XMFLOAT3 abs_normal;
				XMStoreFloat3(&abs_normal, XMVectorAbs(XMLoadFloat3(&info.mNormal)));

				if (abs_normal.x > abs_normal.y && abs_normal.x > abs_normal.z)
				{
					p_movement->mVelocity.x *= -0.5f;
				}
				else if (abs_normal.y > abs_normal.z)
				{
					p_movement->mVelocity.y *= -0.5f;
				}
				else
				{
					p_movement->mVelocity.z *= -0.5f;
				}
			}

			// If normal is +y, the object is on ground, unless it is a unit.
			if (info.mNormal.y > 0.99f)
			{
				// If on top of unit, move away from unit instead.
				if (getEntity(current_entity_id)->hasComponentOfType<UnitComponent>())
				{
					info.mNormal.y = 0.0f;
					info.mNormal.z = 1.0f;
				}
				else
				{
					p_movement->mLastTileY = p_current_transform->position.y;
					on_ground = true;
				}
			}
			
		}
	}
	p_movement->mOnGround = on_ground;
	p_collision->mIntersect = intersect;
	delete p_bv_copy;
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

	GroundCollisionComponent* ground_collision_component = getComponentFromKnownEntity<GroundCollisionComponent>(create_component_event->entityID);

	std::vector<DirectX::XMFLOAT3> *vertex_vector = MeshContainer::GetMeshCPU(GAME_OBJECT_TYPES::GAME_OBJECT_TYPE_UNIT)->GetVertexPositionVector();

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
	float closest_distance = FLT_MAX;

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
	
	// Grabbing the height of the closest tile (y value).
	TransformComponent* closest_tile = getComponentFromKnownEntity<TransformComponent>(closest_tile_id);

	// Sanity check in case the id did not correspond to a valid tile.
	float tile_height = 0.0f;
	if (closest_tile)
	{
		tile_height = closest_tile->position.y;
	}

	TileComponent* closest_tile_comp = getComponentFromKnownEntity<TileComponent>(closest_tile_id);
	
	// Sanity check in case there was no tile component. Default to water.
	TileType type = WATER;
	if (closest_tile_comp)
	{
		type = closest_tile_comp->tileType;
	}

	//Check if units is to far away from the map. If so we set the y they will fall to to -2.0f.
	if (closest_distance > 1.5f || type == WATER)
	{
		tile_height = -2.0f;
	}

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

	// Grabbing the object collision component to fill it up.
	ObjectCollisionComponent* object_collision_component = getComponent<ObjectCollisionComponent>(create_component_event->componentID);

	ModelLoader::Mesh* p_mesh = MeshContainer::GetMeshCPU(object_collision_component->mObjType);

	if (!p_mesh)
	{
		MessageBoxA(nullptr, "No mesh for BV init.", "STOOPID", MB_YESNO);
		return;
	}

	std::vector<DirectX::XMFLOAT3> *vertex_list = p_mesh->GetVertexPositionVector();
	
	// Creating boundingvolume for object with given type around given mesh.
	switch (object_collision_component->mBvType)
	{
	case COLLISION_AABB:
	{
		object_collision_component->mBV = new AABB;
		AABB* p_aabb = static_cast<AABB*>(object_collision_component->mBV);
		p_aabb->CreateFromPoints(*p_aabb, vertex_list->size(), vertex_list->data(), sizeof(XMFLOAT3));
		break;
	}
	case COLLISION_CYLINDER:
	{
		object_collision_component->mBV = new Cylinder;
		Cylinder* p_cylinder = static_cast<Cylinder*>(object_collision_component->mBV);
		XMFLOAT3 position = getComponentFromKnownEntity<TransformComponent>(create_component_event->entityID)->position;
		p_cylinder->CreateFromTile(position);
		//p_cylinder->CreateFromPoints(*p_cylinder, vertex_list->size(), vertex_list->data(), sizeof(XMFLOAT3));
		break;
	}
	case COLLISION_OBB:
	{
		object_collision_component->mBV = new OBB;
		OBB* p_obb = static_cast<OBB*>(object_collision_component->mBV);
		p_obb->CreateFromPoints(*p_obb, vertex_list->size(), vertex_list->data(), sizeof(XMFLOAT3));
		break;
	}
	case COLLISION_SPHERE:
	{
		object_collision_component->mBV = new Sphere;
		Sphere* p_sphere = static_cast<Sphere*>(object_collision_component->mBV);
		p_sphere->CreateFromPoints(*p_sphere, vertex_list->size(), vertex_list->data(), sizeof(XMFLOAT3));
		break;
	}
	// No BV? Get outta here.
	default:
		MessageBoxA(nullptr, "Tried to init a BV without type.", "STOOPID", MB_YESNO);
		return;
	}

	// Unit special case since they need to be able to fight.
	if (object_collision_component->mObjType == GAME_OBJECT_TYPE_UNIT)
	{
		object_collision_component->mBV->Transform(XMMatrixScaling(0.3f, 1.0f, 0.9f));
	}
}
#pragma endregion
#pragma region FillQuadTreeSystem
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
	ecs::components::QuadTreeComponent* p_tree = static_cast<ecs::components::QuadTreeComponent*>
		(ecs::ECSUser::getComponentsOfType(ecs::components::QuadTreeComponent::typeID).next());
	if (!p_tree)
	{
		return;
	}
	ecs::components::TransformComponent* p_transform = entity.getComponent<ecs::components::TransformComponent>();
	ecs::components::ObjectCollisionComponent* p_collision = entity.getComponent<ecs::components::ObjectCollisionComponent>();
	if (p_collision->mBV)
	{
		QuadTree* quad_tree = static_cast<QuadTree*>(p_tree->pTree);
		quad_tree->Insert(QuadTreeObject(p_transform, p_collision));
	}
}
#pragma endregion
#pragma region EmptyQuadTreeSystem
ecs::systems::EmptyQuadTreeSystem::EmptyQuadTreeSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(QuadTreeComponent::typeID);
}
ecs::systems::EmptyQuadTreeSystem::~EmptyQuadTreeSystem()
{

}
void ecs::systems::EmptyQuadTreeSystem::updateEntity(FilteredEntity& entity, float delta)
{
	QuadTreeComponent* p_quad_tree_component = entity.getComponent<QuadTreeComponent>();
	QuadTree* p_quad_tree = static_cast<QuadTree*>(p_quad_tree_component->pTree);
	p_quad_tree->Clear();
}
#pragma endregion
#pragma region InitQuadTreeSystem
ecs::systems::InitQuadTreeSystem::InitQuadTreeSystem()
{
	updateType = EventListenerOnly;
	subscribeEventCreation(CreateComponentEvent::typeID);
}
ecs::systems::InitQuadTreeSystem::~InitQuadTreeSystem()
{
	ComponentIterator it = getComponentsOfType<QuadTreeComponent>();
	while (QuadTreeComponent * quad_tree_component = static_cast<QuadTreeComponent*>(it.next()))
	{
		delete static_cast<QuadTree*>(quad_tree_component->pTree);
	}
}
void ecs::systems::InitQuadTreeSystem::onEvent(TypeID _typeId, ecs::BaseEvent* _event)
{
	CreateComponentEvent* create_component_event = static_cast<CreateComponentEvent*>(_event);
	if (create_component_event->componentTypeID != QuadTreeComponent::typeID)
	{
		return;
	}

	QuadTreeComponent* quad_tree = getComponentFromKnownEntity<QuadTreeComponent>(create_component_event->entityID);
	quad_tree->pTree = new QuadTree(
		0,
		-TILE_RADIUS,
		(TILE_RADIUS * 2.0f) * GridProp::GetInstance()->GetSize().x - TILE_RADIUS,
		cos(30.0f * PI / 180.0f) * TILE_RADIUS,
		(cos(30.0f * PI / 180.0f) * TILE_RADIUS * 2.0f) * GridProp::GetInstance()->GetSize().y - TILE_RADIUS,
		nullptr);
}
#pragma endregion