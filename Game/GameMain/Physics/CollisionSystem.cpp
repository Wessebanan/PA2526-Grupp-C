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

	// Grabbing a copy of moving object's bounding volume.
	
	BoundingVolume* p_bv_copy = p_collision->mBV->Copy();

	XMMATRIX world_transform = UtilityEcsFunctions::GetWorldMatrix(*p_transform);
	p_bv_copy->Transform(world_transform);

	QuadTreeObject this_object(p_transform, p_collision);
	std::vector<QuadTreeObject> collision_list;
	QuadTreeComponent *p_quad_tree_component = static_cast<QuadTreeComponent*>(getComponentsOfType<QuadTreeComponent>().next());
	QuadTree *quad_tree = static_cast<QuadTree*>(p_quad_tree_component->pTree);

	quad_tree->RetrieveCollisions(collision_list, this_object);

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