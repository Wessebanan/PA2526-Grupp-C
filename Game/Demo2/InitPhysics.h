#pragma once
#include "ecs.h"
#include "PhysicsComponents.h"
#include "CollisionSystem.h"
#include "MovementSystem.h"
#include "UtilityFunctions.h"
#include "Mesh.h"

// Creating systems for ecs and components for entities.
void InitGravityOnUnits(ecs::EntityComponentSystem& rEcs, ModelLoader::Mesh mesh)
{
	// Create systems.
	rEcs.createSystem<ecs::systems::GroundCollisionComponentInitSystem>();
	rEcs.createSystem<ecs::systems::GroundCollisionSystem>();
	rEcs.createSystem<ecs::systems::DynamicMovementSystem>();

	// Declare components.
	MeshComponent mesh_component;
	mesh_component.mMesh = mesh;
	GroundCollisionComponent ground_collision_component;
	DynamicMovementComponent movement_component;
	
	int army_index = 0;
	ecs::ComponentIterator it = rEcs.getAllComponentsOfType(ecs::components::ArmyComponent::typeID);
	ecs::components::ArmyComponent* army_comp;

	// Create components for entities.
	while (army_comp = (ecs::components::ArmyComponent*)it.next())
	{
		for (size_t i = 0; i < 3; i++)
		{
			ID current = army_comp->unitIDs[i];
			rEcs.createComponent<MeshComponent>(current, mesh_component);
			rEcs.createComponent<GroundCollisionComponent>(current, ground_collision_component);
			rEcs.createComponent<DynamicMovementComponent>(current, movement_component);
		}
	}	   
}