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

void CreateCollisionForSceneObjects(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh* pMesh);

// Creates a weapon out of a mesh and weapon type. (weapon, transform and mesh components)
ecs::Entity* CreateWeaponEntity(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh* pMesh, WEAPON_TYPE weaponType);

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
	// rEcs.createSystem<ecs::systems::StaticMovementSystem>();
	// rEcs.createSystem<ecs::systems::StaticMovementUpdateSystem>();
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
	ecs::TypeFilter filter;
	filter.addRequirement(UnitComponent::typeID);
	ecs::EntityIterator it = rEcs.getEntititesByFilter(filter);
	
	MeshComponent mesh_component;
	mesh_component.mMesh = pMesh;
	ObjectCollisionComponent object_collision;
	GroundCollisionComponent ground_collision;
	DynamicMovementComponent movement_component;
	HealthComponent health_component;
	EquipmentComponent equipment_component;
	


	for (int i = 0; i < it.entities.size(); i++)
	{
		ecs::Entity* current = it.entities.at(i).entity;
		if (!current->hasComponentOfType<MeshComponent>())
		{
			rEcs.createComponent<MeshComponent>(current->getID(), mesh_component);
		}

		if (!current->hasComponentOfType<ObjectCollisionComponent>())
		{
			rEcs.createComponent<ObjectCollisionComponent>(current->getID(), object_collision);
		}

		if (!current->hasComponentOfType<GroundCollisionComponent>())
		{
			rEcs.createComponent<GroundCollisionComponent>(current->getID(), ground_collision);
		}

		if (!current->hasComponentOfType<DynamicMovementComponent>())
		{
			rEcs.createComponent<DynamicMovementComponent>(current->getID(), movement_component);
		}

		if (!current->hasComponentOfType<HealthComponent>())
		{
			rEcs.createComponent<HealthComponent>(current->getID(), health_component);
		}

		// Initializing with fist weapons.
		if (!current->hasComponentOfType<EquipmentComponent>())
		{
			// Setting melee range here (arm length) hoping that any unit mesh is either facing x or z on load.
			ObjectCollisionComponent* p_object_collision = dynamic_cast<ObjectCollisionComponent*>(rEcs.getComponent(ObjectCollisionComponent::typeID, current->getComponentID(ObjectCollisionComponent::typeID)));
			XMFLOAT3 extents = p_object_collision->mAABB.Extents;
			equipment_component.mMeleeRange = extents.x > extents.z ? extents.x : extents.z;
			TransformComponent* p_transform = dynamic_cast<TransformComponent*>(rEcs.getComponent(TransformComponent::typeID, current->getComponentID(TransformComponent::typeID)));
			
			// Assume uniform scale (pain otherwise).
			equipment_component.mMeleeRange *= p_transform->scale.x;
			
			// Set attack range to melee range since fist adds no range.
			equipment_component.mAttackRange = equipment_component.mMeleeRange;

			ecs::Entity* weapon_entity = CreateWeaponEntity(rEcs, nullptr, FIST);
			equipment_component.mEquippedWeapon = weapon_entity->getID();
			rEcs.createComponent<EquipmentComponent>(current->getID(), equipment_component);
		}
	}
}

inline void CreateCollisionForSceneObjects(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh* pMesh)
{
	// TODO : Get scene objects and add object collision components to them.
}

inline ecs::Entity* CreateWeaponEntity(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh* pMesh, WEAPON_TYPE weaponType)
{
	WeaponComponent		weapon_component;
	TransformComponent	weapon_transform_component;
	MeshComponent		weapon_mesh_component;

	weapon_transform_component.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	weapon_component.mType = weaponType;
	switch (weaponType)
	{
	case SWORD:
	{
		ModelLoader::Mesh sword("Physics/TestModel/sword.fbx");
		weapon_mesh_component.mMesh = pMesh;
		break;
	}
	case FIST:
		weapon_mesh_component.mMesh = pMesh;
		break;
	case PROJECTILE:
		MessageBoxA(NULL, "Projectile weapon not yet implemented.", NULL, MB_YESNO);
		break;
	}
	 
	return rEcs.createEntity(weapon_mesh_component, weapon_transform_component, weapon_component);
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

















