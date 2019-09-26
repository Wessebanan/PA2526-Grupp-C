#pragma once
#include "Mesh.h"
#include "CollisionSystem.h"
#include "GridFunctions.h"

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
		mesh_component.mMesh.LoadFBX("../TestModel/dude.fbx");
		GroundCollisionComponent ground_collision_component;
		TransformComponent transform_component;

		ecs::Entity* ground_collision_entity = ecs.createEntity(mesh_component, transform_component, ground_collision_component);

		// Checking that there is now one entity and three components.
		EXPECT_EQ(ecs.getTotalEntityCount(), 1);
		EXPECT_EQ(ecs.getTotalComponentCount(), 3);

		// Checking that the pointer received from getEntity matches
		// the pointer to the entity.
		EXPECT_EQ(ecs.getEntity(ground_collision_entity->getID()), ground_collision_entity);
	}

	TEST(FindLowestPointOfMesh, InitGroundCollisionComponent)
	{
		ecs::EntityComponentSystem ecs;

		ecs::systems::GroundCollisionComponentInitSystem* ground_collision_component_init_system = ecs.createSystem<ecs::systems::GroundCollisionComponentInitSystem>();

		MeshComponent mesh_component;
		mesh_component.mMesh.LoadFBX("../TestModel/dude.fbx");

		// Checking that the ground collision hitbox is initialized properly.
		GroundCollisionComponent ground_collision_component;
		for (int i = 0; i < 8; i++)
		{
			EXPECT_FLOAT_EQ(ground_collision_component.mVertices[i].x, 0.0f);
			EXPECT_FLOAT_EQ(ground_collision_component.mVertices[i].y, 0.0f);
			EXPECT_FLOAT_EQ(ground_collision_component.mVertices[i].z, 0.0f);
		}

		// The entity needs a transform component as well.
		TransformComponent transform_component;

		ecs::Entity* ground_collision_entity = ecs.createEntity(mesh_component, transform_component, ground_collision_component);
		
		// Getting a pointer to the ground collision component to check its values.
		GroundCollisionComponent* p_ground_collision_component = dynamic_cast<GroundCollisionComponent*>(ecs.getComponent(GroundCollisionComponent::typeID, ground_collision_entity->getComponentID(GroundCollisionComponent::typeID)));
		
		const float abs_error = (float)pow(10, -10);		
		
		// Checking that each vertex is inside the box.
		std::vector<DirectX::XMFLOAT3> *vertices = mesh_component.mMesh.GetVertexPositionVector();
		for (int i = 0; i < vertices->size(); i++)
		{
			// Min point is at index 0 and max point at index 7.
			EXPECT_TRUE(vertices->at(i).x >= p_ground_collision_component->mVertices[0].x);
			EXPECT_TRUE(vertices->at(i).x <= p_ground_collision_component->mVertices[7].x);
			EXPECT_TRUE(vertices->at(i).y >= p_ground_collision_component->mVertices[0].y);
			EXPECT_TRUE(vertices->at(i).y <= p_ground_collision_component->mVertices[7].y);
			EXPECT_TRUE(vertices->at(i).z >= p_ground_collision_component->mVertices[0].z);
			EXPECT_TRUE(vertices->at(i).z <= p_ground_collision_component->mVertices[7].z);
		}		
	}

	TEST(GroundCollision, CreateGroundCollisionSystem)
	{
		ecs::EntityComponentSystem ecs;
		EXPECT_EQ(ecs.getTotalSystemCount(), 0);

		ecs::systems::GroundCollisionSystem* ground_collision_system = ecs.createSystem<ecs::systems::GroundCollisionSystem>();
		EXPECT_EQ(ecs.getTotalSystemCount(), 1);

		EXPECT_NE(ground_collision_system, nullptr);		
	}

	TEST(GroundCollision, CreateCompleteEntity)
	{
		ecs::EntityComponentSystem ecs;

		// Making sure there are no entities or components.
		EXPECT_EQ(ecs.getTotalEntityCount(), 0);
		EXPECT_EQ(ecs.getTotalComponentCount(), 0);

		// Creating the dude that is supposed to collide with ground.
		MeshComponent mesh_component;
		mesh_component.mMesh.LoadFBX("../TestModel/dude.fbx");
		GroundCollisionComponent ground_collision_component;
		TransformComponent transform_component;

		ecs::Entity *ground_collision_entity = ecs.createEntity(mesh_component, transform_component, ground_collision_component);

		// Checking that there is now one entity and three components.
		EXPECT_EQ(ecs.getTotalEntityCount(), 1);
		EXPECT_EQ(ecs.getTotalComponentCount(), 3);

		// Checking that the pointer received from getEntity matches
		// the pointer to the entity.
		EXPECT_EQ(ecs.getEntity(ground_collision_entity->getID()), ground_collision_entity);
		
		EXPECT_NE(ground_collision_entity, nullptr);
	}

	TEST(GroundCollision, TestCollisionHeightAdjustment)
	{
		ecs::CompTypeMemDesc ecsMemDesc[] = {
		{ TileComponent::typeID, TileComponent::size, 1000 },
		{ TransformComponent::typeID, TransformComponent::size, 1000 },
		};

		ecs::ECSDesc ecsDesc;
		ecsDesc.compTypeCount = 2;
		ecsDesc.compTypeMemDescs = ecsMemDesc;
		ecsDesc.systemLayerCount = 10;

		ecs::EntityComponentSystem ecs;
		ecs.initialize(ecsDesc);

		ecs::systems::GroundCollisionSystem* ground_collision_system = ecs.createSystem<ecs::systems::GroundCollisionSystem>();
		ecs::systems::GroundCollisionComponentInitSystem* ground_collision_component_init_system = ecs.createSystem<ecs::systems::GroundCollisionComponentInitSystem>();

		// Creating a grid for the ecs. Choosing a radius 
		// that kind of makes sense with the model size 
		// with no scaling of the dude.
		GridFunctions::CreateGrid(ecs, 12, 12, 10.0f);

		// Messing with the y-values since they are all 0 right now.
		ecs::TypeFilter filter;
		filter.addRequirement(TileComponent::typeID);
		ecs::EntityIterator it = ecs.getEntititesByFilter(filter);

		const float TILE_HEIGHT = 3.0f;

		for (int i = 0; i < it.entities.size(); i++)
		{
			ecs::Entity* tile = it.entities.at(i).entity;

			TransformComponent* transform = dynamic_cast<TransformComponent*>(ecs.getComponentFromEntity(TransformComponent::typeID, tile->getID()));
			transform->position.y = TILE_HEIGHT;
		}

		// Creating the dude that is supposed to collide with ground.
		MeshComponent mesh_component;
		mesh_component.mMesh.LoadFBX("../TestModel/dude.fbx");
		GroundCollisionComponent ground_collision_component;
		TransformComponent transform_component;
		//transform_component.position = DirectX::XMFLOAT3(10.0f, 0.0f, 10.0f);

		ecs::Entity* ground_collision_entity = ecs.createEntity(mesh_component, transform_component, ground_collision_component);

		TransformComponent* p_transform_component = dynamic_cast<TransformComponent*>(ecs.getComponentFromEntity(TransformComponent::typeID, ground_collision_entity->getID()));
		GroundCollisionComponent* p_ground_collision_component = dynamic_cast<GroundCollisionComponent*>(ecs.getComponentFromEntity(GroundCollisionComponent::typeID, ground_collision_entity->getID()));
		const float DELTA = 0.1f;

		// y position should be 0 at this point, as it is not translated.
		EXPECT_FLOAT_EQ(p_transform_component->position.y, 0.0f);
		
		// ECS update should trigger the Ground Collision System to move the model
		// above the ground.
		ecs.update(DELTA);

		// y position should now be the distance from the center of the model
		// to the base, plus the height of the tile, since the origin of
		// the model is the center of it.
		EXPECT_FLOAT_EQ(p_transform_component->position.y, TILE_HEIGHT + fabs(p_ground_collision_component->mVertices[0].y));
	}

	TEST(GroundCollision, TestCollisionHeightAdjustmentSpecificTile)
	{
		// Creating an ecsMemDesc to allow more entities and components
		// of certain types.
		ecs::CompTypeMemDesc ecsMemDesc[] = {
		{ TileComponent::typeID, TileComponent::size, 1000 },
		{ TransformComponent::typeID, TransformComponent::size, 1000 },
		};

		ecs::ECSDesc ecsDesc;
		ecsDesc.compTypeCount = 2;
		ecsDesc.compTypeMemDescs = ecsMemDesc;
		ecsDesc.systemLayerCount = 10;

		ecs::EntityComponentSystem ecs;
		ecs.initialize(ecsDesc);

		// Creating systems.
		ecs::systems::GroundCollisionSystem* ground_collision_system = ecs.createSystem<ecs::systems::GroundCollisionSystem>();
		ecs::systems::GroundCollisionComponentInitSystem* ground_collision_component_init_system = ecs.createSystem<ecs::systems::GroundCollisionComponentInitSystem>();

		// Creating a grid for the ecs. Choosing a radius 
		// that kind of makes sense with the model size 
		// with no scaling of the dude.
		GridFunctions::CreateGrid(ecs, 12, 12, 10.0f);

		// Grabbing all tiles.
		ecs::TypeFilter filter;
		filter.addRequirement(TileComponent::typeID);
		ecs::EntityIterator it = ecs.getEntititesByFilter(filter);

		const float TILE_HEIGHT = 3.0f;
		const float SPECIFIC_TILE_INDEX = 33;
		
		// Grabbing one specific tile.
		ecs::Entity* specific_tile = it.entities.at(SPECIFIC_TILE_INDEX).entity;

		// Setting that specific tile to a certain height.
		TransformComponent* specific_tile_transform = dynamic_cast<TransformComponent*>(ecs.getComponentFromEntity(TransformComponent::typeID, specific_tile->getID()));
		specific_tile_transform->position.y = TILE_HEIGHT;

		// Creating the dude that is supposed to collide with ground.
		MeshComponent mesh_component;
		mesh_component.mMesh.LoadFBX("../TestModel/dude.fbx");
		GroundCollisionComponent ground_collision_component;
		TransformComponent transform_component;

		ecs::Entity* ground_collision_entity = ecs.createEntity(mesh_component, transform_component, ground_collision_component);

		// Grabbing the components I need to look at and change.
		TransformComponent* p_transform_component = dynamic_cast<TransformComponent*>(ecs.getComponentFromEntity(TransformComponent::typeID, ground_collision_entity->getID()));
		GroundCollisionComponent* p_ground_collision_component = dynamic_cast<GroundCollisionComponent*>(ecs.getComponentFromEntity(GroundCollisionComponent::typeID, ground_collision_entity->getID()));
		
		const float DELTA = 0.1f;

		// y position should be 0 at this point, as it is not translated.
		EXPECT_FLOAT_EQ(p_transform_component->position.y, 0.0f);

		// ECS update should trigger the Ground Collision System to move the model
		// above the ground.
		ecs.update(DELTA);

		// y position should now be the distance from the center of the model
		// to the base, plus the default height of a tile (0), since the origin of
		// the model is the center of it.
		EXPECT_FLOAT_EQ(p_transform_component->position.y, 0.0f + fabs(p_ground_collision_component->mVertices[0].y));

		// Changing the position of the ground collision entity to the tile
		// where height is adjusted.
		p_transform_component->position.x = specific_tile_transform->position.x;
		p_transform_component->position.z =	specific_tile_transform->position.z;

		ecs.update(DELTA);

		// y position should now be the distance from the center of the model
		// to the base, plus the height of the specific tile.
		EXPECT_FLOAT_EQ(p_transform_component->position.y, TILE_HEIGHT + fabs(p_ground_collision_component->mVertices[0].y));
	}

} // GroundCollision