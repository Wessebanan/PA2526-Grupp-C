#pragma once
#include "Mesh.h"
#include "CollisionSystem.h"
namespace GroundCollision
{
	TEST(FindLowestPointOfMesh, CreateGroundCollisionComponentInitSystem)
	{
		ecs::EntityComponentSystem ecs;
		EXPECT_EQ(ecs.getTotalSystemCount(), 0);
		
		ecs::systems::GroundCollisionComponentInitSystem* ground_collision_component_init_system = ecs.createSystem<ecs::systems::GroundCollisionComponentInitSystem>();
		EXPECT_EQ(ecs.getTotalSystemCount(), 1);

		EXPECT_NE(ground_collision_component_init_system, nullptr);
	}

	TEST(FindLowestPointOfMesh, CreateGroundCollisionEntity)
	{
		ecs::EntityComponentSystem ecs;
		ecs::systems::GroundCollisionComponentInitSystem* ground_collision_component_init_system = ecs.createSystem<ecs::systems::GroundCollisionComponentInitSystem>();
		
		// Making sure there are no entities or components.
		EXPECT_EQ(ecs.getTotalEntityCount(), 0);
		EXPECT_EQ(ecs.getTotalComponentCount(), 0);

		MeshComponent mesh_component;
		EXPECT_HRESULT_SUCCEEDED(mesh_component.mMesh.LoadFBX("../TestModel/deer.fbx"));
		
		GroundCollisionComponent ground_collision_component;

		ecs::Entity* ground_collision_entity = ecs.createEntity(mesh_component, ground_collision_component);

		// Checking that there is now one entity and two components.
		EXPECT_EQ(ecs.getTotalEntityCount(), 1);
		EXPECT_EQ(ecs.getTotalComponentCount(), 2);

		// Checking that the pointer received from getEntity matches
		// the pointer to the entity.
		EXPECT_EQ(ecs.getEntity(ground_collision_entity->getID()), ground_collision_entity);
	}

	TEST(FindLowestPointOfMesh, InitGroundCollisionComponent)
	{
		ecs::EntityComponentSystem ecs;

		const float delta = 0.1f;
		
		// Initial update of ecs.
		ecs.update(delta);

		ecs::systems::GroundCollisionComponentInitSystem* ground_collision_component_init_system = ecs.createSystem<ecs::systems::GroundCollisionComponentInitSystem>();

		MeshComponent mesh_component;
		EXPECT_HRESULT_SUCCEEDED(mesh_component.mMesh.LoadFBX("../TestModel/deer.fbx"));

		ecs::Entity* ground_collision_entity = ecs.createEntity(mesh_component);
		
		ecs.update(delta);
		GroundCollisionComponent ground_collision_component;
		ecs.createComponent(ground_collision_entity->getID(), ground_collision_component);
		ecs.update(delta);
		
		// Getting a pointer to the ground collision component to check its values.
		GroundCollisionComponent* p_ground_collision_component = dynamic_cast<GroundCollisionComponent*>(ecs.getComponent(GroundCollisionComponent::typeID, ground_collision_entity->getComponentID(GroundCollisionComponent::typeID)));
		
		const float abs_error = (float)pow(10, -10);
		
		// Checking that the ground collision hitbox is initialized properly.
		for (int i = 0; i < 8; i++)
		{
			EXPECT_FLOAT_EQ(p_ground_collision_component->mVertices[i].x, 0.0f);
			EXPECT_FLOAT_EQ(p_ground_collision_component->mVertices[i].y, 0.0f);
			EXPECT_FLOAT_EQ(p_ground_collision_component->mVertices[i].z, 0.0f);
		}

		for (int i = 0; i < 8; i++)
		{
			std::cout
				<< "( " << p_ground_collision_component->mVertices[i].x
				<< ", " << p_ground_collision_component->mVertices[i].y
				<< ", " << p_ground_collision_component->mVertices[i].z
				<< " )" << std::endl;
		}
	}
} // GroundCollision