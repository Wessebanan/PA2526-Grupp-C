#pragma once
#include "CollisionSystem.h"
#include "MovementSystem.h"
#include "GridEcsFunctions.h"

// This lil' piece of jank makes tests waaay faster.
ModelLoader::Mesh dude("../TestModel/dude.fbx");

#pragma region FindLowestPointOfMesh
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
		mesh_component.mMesh = &dude;
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
		mesh_component.mMesh = &dude;

		GroundCollisionComponent ground_collision_component;

		ecs::Entity* ground_collision_entity = ecs.createEntity(mesh_component, ground_collision_component);
		
		// Getting a pointer to the ground collision component to check its values.
		GroundCollisionComponent* p_ground_collision_component = dynamic_cast<GroundCollisionComponent*>(ecs.getComponent(GroundCollisionComponent::typeID, ground_collision_entity->getComponentID(GroundCollisionComponent::typeID)));
		
		// Checking that enough vertices are inside the box.
		std::vector<DirectX::XMFLOAT3> *vertices = mesh_component.mMesh->GetVertexPositionVector();
		unsigned int n_inside = 0;
		for (int i = 0; i < vertices->size(); i++)
		{
			ContainmentType containtment_type = p_ground_collision_component->mOBB.Contains(XMLoadFloat3(&vertices->at(i)));
			if (containtment_type == ContainmentType::CONTAINS)
			{
				n_inside++;
			}
		}

		// DirectXCollision disregards a few points in making the box, for optimization probably.
		// If 99% of vertices are inside, it is safe to assume that the box has been created properly.
		EXPECT_GT((float)n_inside / (float)vertices->size(), 0.99f);
	}
#pragma endregion
#pragma region GroundCollision
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
		mesh_component.mMesh = &dude;
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
		{ ColorComponent::typeID, ColorComponent::size, 1000 },
		{ TransformComponent::typeID, TransformComponent::size, 1000 },
		};

		ecs::ECSDesc ecsDesc;
		ecsDesc.compTypeCount = 3;
		ecsDesc.compTypeMemDescs = ecsMemDesc;
		ecsDesc.systemLayerCount = 10;

		ecs::EntityComponentSystem ecs;
		ecs.initialize(ecsDesc);

		ecs::systems::GroundCollisionSystem* ground_collision_system = ecs.createSystem<ecs::systems::GroundCollisionSystem>();
		ecs::systems::GroundCollisionComponentInitSystem* ground_collision_component_init_system = ecs.createSystem<ecs::systems::GroundCollisionComponentInitSystem>();

		// Creating a grid for the ecs. Choosing a radius 
		// that kind of makes sense with the model size 
		// with no scaling of the dude.
		GridEcsFunctions::CreateGrid(ecs, 12, 12, 10.0f);

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
		mesh_component.mMesh = &dude;
		GroundCollisionComponent ground_collision_component;
		TransformComponent transform_component;
		transform_component.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

		ecs::Entity* ground_collision_entity = ecs.createEntity(mesh_component, transform_component, ground_collision_component);

		TransformComponent* p_transform_component = dynamic_cast<TransformComponent*>(ecs.getComponentFromEntity(TransformComponent::typeID, ground_collision_entity->getID()));
		GroundCollisionComponent* p_ground_collision_component = dynamic_cast<GroundCollisionComponent*>(ecs.getComponentFromEntity(GroundCollisionComponent::typeID, ground_collision_entity->getID()));
		const float DELTA = 0.1f;

		// y position should be 0 at this point, as it is not translated.
		EXPECT_FLOAT_EQ(p_transform_component->position.y, 0.0f);
		
		// ECS update should trigger the Ground Collision System to move the model
		// above the ground.
		ecs.update(DELTA);

		BoundingOrientedBox obb = p_ground_collision_component->mOBB;
		obb.Transform(obb, UtilityEcsFunctions::GetWorldMatrix(*p_transform_component));

		XMFLOAT3* corners = new XMFLOAT3[8];
		obb.GetCorners(corners);
		const float ABS_ERROR = pow(10.0f, -6.0f);
		for (int i = 0; i < 8; i++)
		{			
			EXPECT_GE(corners[i].y - TILE_HEIGHT, 0.0f - ABS_ERROR);
		}
		delete[] corners;
	}

	TEST(GroundCollision, TestCollisionHeightAdjustmentSpecificTile)
	{
		// Creating an ecsMemDesc to allow more entities and components
		// of certain types.
		ecs::CompTypeMemDesc ecsMemDesc[] = {
		{ TileComponent::typeID, TileComponent::size, 1000 },
		{ ColorComponent::typeID, ColorComponent::size, 1000 },
		{ TransformComponent::typeID, TransformComponent::size, 1000 },
		};


		ecs::ECSDesc ecsDesc;
		ecsDesc.compTypeCount = 3;
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
		GridEcsFunctions::CreateGrid(ecs, 12, 12, 10.0f);

		// Grabbing all tiles.
		ecs::TypeFilter filter;
		filter.addRequirement(TileComponent::typeID);
		ecs::EntityIterator it = ecs.getEntititesByFilter(filter);

		const float TILE_HEIGHT = 3.0f;
		const int SPECIFIC_TILE_INDEX = 33;
		
		// Grabbing one specific tile.
		ecs::Entity* specific_tile = it.entities.at(SPECIFIC_TILE_INDEX).entity;

		// Setting that specific tile to a certain height.
		TransformComponent* specific_tile_transform = dynamic_cast<TransformComponent*>(ecs.getComponentFromEntity(TransformComponent::typeID, specific_tile->getID()));
		specific_tile_transform->position.y = TILE_HEIGHT;

		// Creating the dude that is supposed to collide with ground.
		MeshComponent mesh_component;
		mesh_component.mMesh = &dude;
		GroundCollisionComponent ground_collision_component;
		TransformComponent transform_component;
		transform_component.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

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

		BoundingOrientedBox obb = p_ground_collision_component->mOBB;
		obb.Transform(obb, UtilityEcsFunctions::GetWorldMatrix(*p_transform_component));

		XMFLOAT3* corners = new XMFLOAT3[8];
		obb.GetCorners(corners);
		const float ABS_ERROR = pow(10.0f, -6.0f);
		for (int i = 0; i < 8; i++)
		{
			EXPECT_GE(corners[i].y, 0.0f - ABS_ERROR);
		}
		// Changing the position of the ground collision entity to the tile
		// where height is adjusted.
		p_transform_component->position.x = specific_tile_transform->position.x;
		p_transform_component->position.z =	specific_tile_transform->position.z;

		ecs.update(DELTA);

		obb = p_ground_collision_component->mOBB;
		obb.Transform(obb, UtilityEcsFunctions::GetWorldMatrix(*p_transform_component));

		obb.GetCorners(corners);
		for (int i = 0; i < 8; i++)
		{
			EXPECT_GE(corners[i].y - TILE_HEIGHT, 0.0f - ABS_ERROR);
		}

		delete[] corners;
	}
	
	TEST(GroundCollision, RemoveGravityOnGround)
	{
		// Creating an ecsMemDesc to allow more entities and components
		// of certain types.
		ecs::CompTypeMemDesc ecsMemDesc[] = {
		{ TileComponent::typeID, TileComponent::size, 1000 },
		{ ColorComponent::typeID, ColorComponent::size, 1000 },
		{ TransformComponent::typeID, TransformComponent::size, 1000 },
		};


		ecs::ECSDesc ecsDesc;
		ecsDesc.compTypeCount = 3;
		ecsDesc.compTypeMemDescs = ecsMemDesc;
		ecsDesc.systemLayerCount = 10;

		ecs::EntityComponentSystem ecs;
		ecs.initialize(ecsDesc);

		// Creating systems.
		ecs::systems::GroundCollisionSystem* ground_collision_system = ecs.createSystem<ecs::systems::GroundCollisionSystem>(2);
		ecs::systems::GroundCollisionComponentInitSystem* ground_collision_component_init_system = ecs.createSystem<ecs::systems::GroundCollisionComponentInitSystem>();
		ecs::systems::DynamicMovementSystem* dynamic_movement_system = ecs.createSystem<ecs::systems::DynamicMovementSystem>(1);

		// Creating a grid for the ecs. Choosing a radius 
		// that kind of makes sense with the model size 
		// with no scaling of the dude.
		GridEcsFunctions::CreateGrid(ecs, 12, 12, 10.0f);

		// Creating the dude that is supposed to collide with ground.
		MeshComponent mesh_component;
		mesh_component.mMesh = &dude;
		GroundCollisionComponent ground_collision_component;
		TransformComponent transform_component;
		DynamicMovementComponent movement_component;
		const float START_POSITION = 15.0f;
		transform_component.position.y = START_POSITION;
		transform_component.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

		ecs::Entity* entity = ecs.createEntity(mesh_component, transform_component, ground_collision_component, movement_component);

		// Grabbing relevant components.
		DynamicMovementComponent* p_movement = dynamic_cast<DynamicMovementComponent*>(ecs.getComponentFromEntity(DynamicMovementComponent::typeID, entity->getID()));
		TransformComponent* p_transform = dynamic_cast<TransformComponent*>(ecs.getComponentFromEntity(TransformComponent::typeID, entity->getID()));
		GroundCollisionComponent* p_ground_collision = dynamic_cast<GroundCollisionComponent*>(ecs.getComponentFromEntity(GroundCollisionComponent::typeID, entity->getID()));

		const float DELTA = 1.0f;

		// Should start off as not on the ground.
		EXPECT_FALSE(p_movement->mOnGround);

		// y position should be 10 at this point, as it is not translated.
		EXPECT_FLOAT_EQ(p_transform->position.y, START_POSITION);

		ecs.update(DELTA);

		// Updating in 1 second intervals should set the y velocity to the acceleration of -gravity.
		// (v = a*t = -GRAVITY*1)
		EXPECT_FLOAT_EQ(p_movement->mVelocity.y, -p_movement->mGravity);

		// And position to 0.18 (p = p_0 + v*t = 10 - GRAVITY*1).
		EXPECT_FLOAT_EQ(p_transform->position.y, START_POSITION - p_movement->mGravity);

		// And we're still not on ground level.
		EXPECT_FALSE(p_movement->mOnGround);
		
		ecs.update(DELTA);

		BoundingOrientedBox obb = p_ground_collision->mOBB;
		obb.Transform(obb, UtilityEcsFunctions::GetWorldMatrix(*p_transform));
		XMFLOAT3* corners = new XMFLOAT3[8];
		obb.GetCorners(corners);
		const float ABS_ERROR = pow(10.0f, -6.0f);
		for (int i = 0; i < 8; i++)
		{
			EXPECT_GE(corners[i].y, 0.0f - ABS_ERROR);
		}
		EXPECT_FLOAT_EQ(p_movement->mVelocity.y, 0.0f);
		EXPECT_TRUE(p_movement->mOnGround);

		// Grabbing all tiles.
		ecs::TypeFilter filter;
		filter.addRequirement(TileComponent::typeID);
		ecs::EntityIterator it = ecs.getEntititesByFilter(filter);

		const float TILE_HEIGHT = 3.0f;
		const int SPECIFIC_TILE_INDEX = 33;

		// Grabbing one specific tile.
		ecs::Entity* specific_tile = it.entities.at(SPECIFIC_TILE_INDEX).entity;

		// Setting that specific tile to a certain height.
		TransformComponent* specific_tile_transform = dynamic_cast<TransformComponent*>(ecs.getComponentFromEntity(TransformComponent::typeID, specific_tile->getID()));
		specific_tile_transform->position.y = TILE_HEIGHT;

		// Changing the position of the ground collision entity to the tile
		// where height is adjusted.
		p_transform->position.x = specific_tile_transform->position.x;
		p_transform->position.z = specific_tile_transform->position.z;

		ecs.update(DELTA);
		
		// This update expects the ground collision to work even
		// if the entity is "on ground" since it moved horizontally
		// into a tile that is higher.
		obb = p_ground_collision->mOBB;
		obb.Transform(obb, UtilityEcsFunctions::GetWorldMatrix(*p_transform));
		obb.GetCorners(corners);

		for (int i = 0; i < 8; i++)
		{
			EXPECT_GE(corners[i].y, 0.0f - ABS_ERROR);
		}
		EXPECT_FLOAT_EQ(p_movement->mVelocity.y, 0.0f);
		EXPECT_TRUE(p_movement->mOnGround);
		delete[] corners;
	}
#pragma endregion