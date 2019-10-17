#pragma once
#include "ecs.h"
#include "MovementSystem.h"
#include "CollisionSystem.h"
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
void InitPhysics(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh **ppMmeshes);

// Create every system needed for physics.
void CreatePhysicsSystems(ecs::EntityComponentSystem& rEcs);

// Create every necessary component for entities with unit components.
void CreatePhysicsComponentsForUnits(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh *pMesh);

// Creates an entity with mesh component and collision component for an object (eg. tree).
ecs::Entity* CreateEntityForObject(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh *pMesh);

// Set parameter direction to movement component forward and move forward.
void MoveEntity(ecs::EntityComponentSystem& rEcs, ID entityID, XMFLOAT3 direction);

// Move in direction of parameter input while keeping forward.
void MoveEntity(ecs::EntityComponentSystem& rEcs, ID entityID, MovementInputs input);

inline void InitPhysics(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh** ppMeshes)
{
	CreatePhysicsSystems(rEcs);
	CreatePhysicsComponentsForUnits(rEcs, ppMeshes[Mesh::DUDE]);
}

inline void CreatePhysicsSystems(ecs::EntityComponentSystem& rEcs)
{
	// Movement
	// rEcs.createSystem<ecs::systems::StaticMovementSystem>();
	// rEcs.createSystem<ecs::systems::StaticMovementUpdateSystem>();
	rEcs.createSystem<ecs::systems::DynamicMovementSystem>();

	// Collision
	rEcs.createSystem<ecs::systems::ObjectBoundingVolumeInitSystem>();
	rEcs.createSystem<ecs::systems::ObjectCollisionSystem>();
	rEcs.createSystem<ecs::systems::GroundCollisionComponentInitSystem>();
	rEcs.createSystem<ecs::systems::GroundCollisionSystem>();
}

inline void CreatePhysicsComponentsForUnits(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh * pMesh)
{
	ecs::TypeFilter filter;
	filter.addRequirement(UnitComponent::typeID);
	ecs::EntityIterator it = rEcs.getEntititesByFilter(filter);
	
	MeshComponent mesh_component;
	mesh_component.mMesh = pMesh;
	ObjectCollisionComponent object_collision;
	GroundCollisionComponent ground_collision;
	DynamicMovementComponent movement_component;

	for (int i = 0; i < it.entities.size(); i++)
	{
		ecs::Entity* current = it.entities.at(i).entity;
		if (!current->hasComponentOfType<MeshComponent>())
		{
			rEcs.createComponent<MeshComponent>(current->getID(), mesh_component);
		}

		if (!current->hasComponentOfType<ObjectCollisionComponent>())
		{
			//rEcs.createComponent<ObjectCollisionComponent>(current->getID(), object_collision);
		}

		if (!current->hasComponentOfType<GroundCollisionComponent>())
		{
			rEcs.createComponent<GroundCollisionComponent>(current->getID(), ground_collision);
		}

		if (!current->hasComponentOfType<DynamicMovementComponent>())
		{
			rEcs.createComponent<DynamicMovementComponent>(current->getID(), movement_component);
		}
	}
}

inline ecs::Entity* CreateEntityForObject(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh* pMesh)
{
	// Create components.
	MeshComponent mesh_component;
	mesh_component.mMesh = pMesh;
	ObjectCollisionComponent collsion_component;
	TransformComponent transform_component;
	transform_component.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	// Make list.
	ecs::ComponentList component_list;
	ecs::BaseComponent* list[] =
	{
		&mesh_component,
		&transform_component,
		&collsion_component
	};
	component_list.componentCount = 3;
	component_list.initialInfo = list;

	// Create component.
	return rEcs.createEntity(component_list);
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

















