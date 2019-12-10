#pragma once
#include "ecs.h"

#include "MovementSystem.h"
#include "CollisionSystem.h"
#include "FightingSystem.h"

#include "PhysicsComponents.h"
#include "PhysicsEvents.h"

// Must be called after InitMesh and InitArmy.
void InitPhysics(ecs::EntityComponentSystem& rEcs);

// Create every system needed for physics.
void CreatePhysicsSystems(ecs::EntityComponentSystem& rEcs);

// Create every necessary component for entities with unit components.
void CreatePhysicsComponentsForUnits(ecs::EntityComponentSystem& rEcs);

// LEAVING THIS FOR NOW, MAY BE UNNECCESARY.
void CreateCollisionForSceneObjects(ecs::EntityComponentSystem& rEcs);

// Creates a weapon out of a mesh and weapon type. (weapon, transform and mesh components)
ecs::Entity* CreateWeaponEntity(ecs::EntityComponentSystem& rEcs, GAME_OBJECT_TYPE weaponType, ID ownerEntity = 0);

// Set parameter direction to movement component forward and move forward.
void MoveEntity(ecs::EntityComponentSystem& rEcs, ID entityID, XMFLOAT3 direction);

// Move in direction of parameter input while keeping forward.
void MoveEntity(ecs::EntityComponentSystem& rEcs, ID entityID, MovementInputs input);

inline void InitPhysics(ecs::EntityComponentSystem& rEcs)
{
	CreatePhysicsSystems(rEcs);
	CreatePhysicsComponentsForUnits(rEcs);
}

inline void CreatePhysicsSystems(ecs::EntityComponentSystem& rEcs)
{
	// Quad Tree
	rEcs.createSystem<ecs::systems::InitQuadTreeSystem>(0);
	rEcs.createSystem<ecs::systems::FillQuadTreeSystem>(0);
	rEcs.createSystem<ecs::systems::EmptyQuadTreeSystem>(9);

	// Movement
	rEcs.createSystem<ecs::systems::DynamicMovementInitSystem>();
	rEcs.createSystem<ecs::systems::DynamicMovementSystem>(3);

	// Throw
	rEcs.createSystem<ecs::systems::ThrowEventSystem>(8);

	// Collision
	rEcs.createSystem<ecs::systems::ObjectBoundingVolumeInitSystem>();
	//rEcs.createSystem<ecs::systems::GroundCollisionComponentInitSystem>();
	rEcs.createSystem<ecs::systems::ObjectCollisionSystem>(4);
	//rEcs.createSystem<ecs::systems::GroundCollisionSystem>();

	// Fighting
	rEcs.createSystem<ecs::systems::WeaponInitSystem>();
	rEcs.createSystem<ecs::systems::DamageSystem>();
	rEcs.createSystem<ecs::systems::UnitInvincibilityTimerSystem>();

	// Color
	rEcs.createSystem<ecs::systems::UnitColorSwitchSystem>();

	// OnHit
	rEcs.createSystem<ecs::systems::WeaponOnHitSystem>();
}

inline void CreatePhysicsComponentsForUnits(ecs::EntityComponentSystem& rEcs)
{
	
}

inline void CreateCollisionForSceneObjects(ecs::EntityComponentSystem& rEcs)
{
	TypeFilter filter;
	filter.addRequirement(SceneObjectComponent::typeID);
	filter.addRequirement(TransformComponent::typeID);

	EntityIterator it = rEcs.getEntititesByFilter(filter);

	ObjectCollisionComponent collision;
	collision.mBvType = COLLISION_AABB;

	// Creating a BV for each scene object and making the tile impassable.
	for (int i = 0; i < it.entities.size(); i++)
	{
		ID current_id = it.entities.at(i).entity->getID();
		
		TransformComponent* scene_transform = static_cast<TransformComponent*>(rEcs.getComponentFromEntity(TransformComponent::typeID, current_id));
		int2 scene_tile_index = GridFunctions::GetTileFromWorldPos(scene_transform->position.x, scene_transform->position.z);

		TileData *tile_data = &GridProp::GetInstance()->mGrid[scene_tile_index.y][scene_tile_index.x];
		
		SceneObjectComponent* scene_component = static_cast<SceneObjectComponent*>(rEcs.getComponentFromEntity(SceneObjectComponent::typeID, current_id));
		collision.mObjType = scene_component->mObject;

		if (collision.mObjType == GAME_OBJECT_TYPE_FRUITTREE)
		{
			collision.mObjType = GAME_OBJECT_TYPE_TREE_TRUNK;
		}
		rEcs.createComponent<ObjectCollisionComponent>(current_id, collision);
		tile_data->isPassable = false;
		static_cast<TileComponent*>(rEcs.getComponentFromEntity(TileComponent::typeID, tile_data->Id))->impassable = true;
		
		// Scaling BV of crystal since it's very big UwU.
		if (collision.mObjType == GAME_OBJECT_TYPE_GIANTSKULL)
		{
			ObjectCollisionComponent* p_collision = static_cast<ObjectCollisionComponent*>(rEcs.getComponentFromEntity(ObjectCollisionComponent::typeID, current_id));
			p_collision->mBV->Transform(XMMatrixScaling(0.5f, 1.0f, 0.5f));
		}
		
	}
}

ecs::Entity* CreateWeaponEntity(ecs::EntityComponentSystem& rEcs, GAME_OBJECT_TYPE weaponType, ID ownerEntity)
{
	WeaponComponent		weapon_component;
	TransformComponent	weapon_transform_component;

	weapon_component.mType = weaponType;
	weapon_component.mOwnerEntity = ownerEntity;	

	switch (weaponType)
	{
	case GAME_OBJECT_TYPE_WEAPON_SWORD:
		weapon_transform_component.scale = XMFLOAT3(0.1f, 0.1f, 0.1f);
		break;
	case GAME_OBJECT_TYPE_WEAPON_FIST:
		weapon_transform_component.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		break;
	case GAME_OBJECT_TYPE_WEAPON_PROJECTILE:
		MessageBoxA(NULL, "Projectile weapon not yet implemented.", NULL, MB_YESNO);
		break;
	}
	 
	return rEcs.createEntity(weapon_transform_component, weapon_component);
}

inline void MoveEntity(ecs::EntityComponentSystem &rEcs, ID entityID, XMFLOAT3 direction)
{
	MovementInputEvent movement_event;
	movement_event.mInput = FORWARD;
	movement_event.mEntityID = entityID;

	// If direction is not, or not close to normalized, normalize.
	float dir_len = CalculateVectorLength(direction);
	if (dir_len > 1.01f)
	{
		direction.x /= dir_len;
		direction.y /= dir_len;
		direction.z /= dir_len;
	}

	rEcs.getComponentFromEntity<DynamicMovementComponent>(entityID)->mForward = direction;
	rEcs.createEvent(movement_event);
}

inline void MoveEntity(ecs::EntityComponentSystem& rEcs, ID entityID, MovementInputs input)
{
	MovementInputEvent movement_event;
	movement_event.mInput = input;
	movement_event.mEntityID = entityID;
	rEcs.createEvent(movement_event);
}

















