#pragma once
#include "ecs.h"

#include "MovementSystem.h"
#include "CollisionSystem.h"
#include "FightingSystem.h"

#include "PhysicsComponents.h"
#include "PhysicsEvents.h"

namespace Mesh
{
	enum MESHES
	{
		DUDE,
		TILE,
		N_MESHES,
	};
}

// Must be called after InitMesh and InitArmy.
void InitPhysics(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh *ppMmeshes);

// Create every system needed for physics.
void CreatePhysicsSystems(ecs::EntityComponentSystem& rEcs);

// Create every necessary component for entities with unit components.
void CreatePhysicsComponentsForUnits(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh *pMesh);

// LEAVING THIS FOR NOW, MAY BE UNNECCESARY.
void CreateCollisionForSceneObjects(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh* pMesh);

// Set parameter direction to movement component forward and move forward.
void MoveEntity(ecs::EntityComponentSystem& rEcs, ID entityID, XMFLOAT3 direction);

// Move in direction of parameter input while keeping forward.
void MoveEntity(ecs::EntityComponentSystem& rEcs, ID entityID, MovementInputs input);

inline void InitPhysics(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh* ppMeshes)
{
	CreatePhysicsSystems(rEcs);
	CreatePhysicsComponentsForUnits(rEcs, ppMeshes);
}

inline void CreatePhysicsSystems(ecs::EntityComponentSystem& rEcs)
{
	// Movement
	rEcs.createSystem<ecs::systems::DynamicMovementInitSystem>();
	rEcs.createSystem<ecs::systems::DynamicMovementSystem>();

	// Collision
	rEcs.createSystem<ecs::systems::ObjectBoundingVolumeInitSystem>();
	rEcs.createSystem<ecs::systems::GroundCollisionComponentInitSystem>();
	rEcs.createSystem<ecs::systems::ObjectCollisionSystem>();
	rEcs.createSystem<ecs::systems::GroundCollisionSystem>();

	// Fighting
	rEcs.createSystem<ecs::systems::WeaponInitSystem>();
	rEcs.createSystem<ecs::systems::DamageSystem>();
}

inline void CreatePhysicsComponentsForUnits(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh * pMesh)
{
	
}

inline void CreateCollisionForSceneObjects(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh* pMesh)
{
	TypeFilter filter;
	filter.addRequirement(SceneObjectComponent::typeID);

	EntityIterator it = rEcs.getEntititesByFilter(filter);
	for (int i = 0; i < it.entities.size(); i++)
	{
		ID current_id = it.entities.at(i).entity->getID();
		TransformComponent* scene_transform = static_cast<TransformComponent*>(rEcs.getComponent(TransformComponent::typeID, current_id));
		int2 scene_tile_index = GridFunctions::GetTileFromWorldPos(scene_transform->position.x, scene_transform->position.z);

		TileData tile_data = GridProp::GetInstance()->mGrid[scene_tile_index.y][scene_tile_index.x];
		// If tile is made not passable by scene object, add collision to scene object.
		if (!tile_data.isPassable)
		{
			SceneObjectComponent* scene_component = static_cast<SceneObjectComponent*>(rEcs.getComponent(SceneObjectComponent::typeID, current_id));
		}
	}
	// TODO : Get scene objects and add object collision components to them.
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

















