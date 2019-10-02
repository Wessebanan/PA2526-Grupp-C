#pragma once
#include "CollisionSystem.h"
#include "MovementSystem.h"

#pragma region ObjectBoundingVolumes
TEST(ObjectBoundingVolumes, CreateInitSystem)
{
	ecs::EntityComponentSystem ecs;
	EXPECT_EQ(ecs.getTotalSystemCount(), 0);

	ecs::systems::ObjectBoundingVolumeInitSystem* ground_collision_system = ecs.createSystem<ecs::systems::ObjectBoundingVolumeInitSystem>();
	EXPECT_EQ(ecs.getTotalSystemCount(), 1);

	EXPECT_NE(ground_collision_system, nullptr);
}
TEST(ObjectBoundingVolumes, CreateObjectEntity)
{
	ecs::EntityComponentSystem ecs;

	// Making sure there are no entities or components.
	EXPECT_EQ(ecs.getTotalEntityCount(), 0);
	EXPECT_EQ(ecs.getTotalComponentCount(), 0);

	// Creating the dude that is supposed to collide with objects.
	MeshComponent mesh_component;
	mesh_component.mMesh.LoadFBX("../TestModel/dude.fbx");
	ObjectCollisionComponent object_collision_component;

	ecs::Entity* object_collision_entity = ecs.createEntity(mesh_component, object_collision_component);

	// Checking that there is now one entity and two components.
	EXPECT_EQ(ecs.getTotalEntityCount(), 1);
	EXPECT_EQ(ecs.getTotalComponentCount(), 2);

	// Checking that the pointer received from getEntity matches
	// the pointer to the entity.
	EXPECT_EQ(ecs.getEntity(object_collision_entity->getID()), object_collision_entity);

	EXPECT_NE(object_collision_entity, nullptr);
}
TEST(ObjectBoundingVolumes, TestBoundingVolume)
{
	ecs::EntityComponentSystem ecs;

	ecs::systems::ObjectBoundingVolumeInitSystem* ground_collision_component_init_system = ecs.createSystem<ecs::systems::ObjectBoundingVolumeInitSystem>();

	MeshComponent mesh_component;
	mesh_component.mMesh.LoadFBX("../TestModel/dude.fbx");
	ObjectCollisionComponent object_collision_component;

	ecs::Entity* object_collision_entity = ecs.createEntity(mesh_component, object_collision_component);

	// Getting a pointer to the ground collision component to check its values.
	ObjectCollisionComponent* p_object_collision_component = dynamic_cast<ObjectCollisionComponent*>(ecs.getComponent(ObjectCollisionComponent::typeID, object_collision_entity->getComponentID(ObjectCollisionComponent::typeID)));

	// Checking that each vertex is inside the box.
	std::vector<DirectX::XMFLOAT3>* vertices = mesh_component.mMesh.GetVertexPositionVector();
	for (int i = 0; i < vertices->size(); i++)
	{
		EXPECT_TRUE(vertices->at(i).x >= p_object_collision_component->mMin.x);
		EXPECT_TRUE(vertices->at(i).y >= p_object_collision_component->mMin.y);
		EXPECT_TRUE(vertices->at(i).z >= p_object_collision_component->mMin.z);
		EXPECT_TRUE(vertices->at(i).x <= p_object_collision_component->mMax.x);
		EXPECT_TRUE(vertices->at(i).y <= p_object_collision_component->mMax.y);
		EXPECT_TRUE(vertices->at(i).z <= p_object_collision_component->mMax.z);
	}

	// Test that the center of the bounding box is in the center.
	EXPECT_FLOAT_EQ((p_object_collision_component->mMin.x + p_object_collision_component->mMax.x) / 2.0f, p_object_collision_component->mCenter.x);
	EXPECT_FLOAT_EQ((p_object_collision_component->mMin.y + p_object_collision_component->mMax.y) / 2.0f, p_object_collision_component->mCenter.y);
	EXPECT_FLOAT_EQ((p_object_collision_component->mMin.z + p_object_collision_component->mMax.z) / 2.0f, p_object_collision_component->mCenter.z);
}
#pragma endregion
#pragma region ObjectCollisionCheck
TEST(ObjectCollisionCheck, CreateSystem)
{
	// Making sure that the system is created properly.
	ecs::EntityComponentSystem ecs;

	EXPECT_EQ(ecs.getTotalSystemCount(), 0);

	ecs::systems::ObjectCollisionSystem* p_object_collision_system = nullptr;
	p_object_collision_system = ecs.createSystem<ecs::systems::ObjectCollisionSystem>();

	EXPECT_EQ(ecs.getTotalSystemCount(), 1);
	EXPECT_NE(p_object_collision_system, nullptr);
}
TEST(ObjectCollisionCheck, CreateCompleteEntity)
{
	ecs::EntityComponentSystem ecs;

	// Making sure there are no entities or components.
	EXPECT_EQ(ecs.getTotalEntityCount(), 0);
	EXPECT_EQ(ecs.getTotalComponentCount(), 0);

	// Creating the components that the entity needs to be
	// able to collide with other entities.
	MeshComponent mesh_component;
	mesh_component.mMesh.LoadFBX("../TestModel/dude.fbx");
	ObjectCollisionComponent object_collision_component;
	DynamicMovementComponent dynamic_movement_component;
	TransformComponent transform_component;
	transform_component.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	ecs::ComponentList component_list;
	ecs::BaseComponent* list[] = 
	{
		&mesh_component,
		&object_collision_component,
		&dynamic_movement_component,
		&transform_component
	};
	component_list.initialInfo = list;
	component_list.componentCount = 4;
	ecs::Entity* object_collision_entity = ecs.createEntity(component_list);

	// Checking that there is now one entity and four components.
	EXPECT_EQ(ecs.getTotalEntityCount(), 1);
	EXPECT_EQ(ecs.getTotalComponentCount(), 4);

	// Checking that the pointer received from getEntity matches
	// the pointer to the entity.
	EXPECT_EQ(ecs.getEntity(object_collision_entity->getID()), object_collision_entity);

	EXPECT_NE(object_collision_entity, nullptr);
}
TEST(ObjectCollisionCheck, CheckCollision)
{
	ecs::EntityComponentSystem ecs;

	ecs.createSystem<ecs::systems::ObjectBoundingVolumeInitSystem>();
	ecs.createSystem<ecs::systems::ObjectCollisionSystem>();
	ecs.createSystem<ecs::systems::DynamicMovementSystem>();

	MeshComponent mesh_component;
	mesh_component.mMesh.LoadFBX("../TestModel/dude.fbx");
	ObjectCollisionComponent object_collision_component;
	DynamicMovementComponent dynamic_movement_component;
	TransformComponent transform_component;
	transform_component.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	ecs::ComponentList component_list;
	ecs::BaseComponent* list[] =
	{
		&mesh_component,
		&object_collision_component,
		&dynamic_movement_component,
		&transform_component
	};
	component_list.initialInfo = list;
	component_list.componentCount = 4;

	// Creating the entities that should collide.
	ecs::Entity* entity1 = ecs.createEntity(component_list);
	ecs::Entity* entity2 = ecs.createEntity(component_list);
	
	ObjectCollisionComponent* p_object_collision_component1 = dynamic_cast<ObjectCollisionComponent*>(ecs.getComponent(ObjectCollisionComponent::typeID, entity1->getComponentID(ObjectCollisionComponent::typeID)));
	TransformComponent* p_transform_component2 = dynamic_cast<TransformComponent*>(ecs.getComponent(TransformComponent::typeID, entity2->getComponentID(TransformComponent::typeID)));

	// Moving entity2 so it is right in front of entity1
	// so any forward movement should trigger a collision.
	p_transform_component2->position.x = p_object_collision_component1->mMax.x - p_object_collision_component1->mMin.x;

	// This test is to see if entity1 collides properly with entity2.

	// Check that it currently does not intersect.
	EXPECT_FALSE(p_object_collision_component1->mIntersect);

	// Creating a movement input event to trigger 
	// a collision check.
	MovementInputEvent movement_event;
	movement_event.mEntityID = entity1->getID();
	movement_event.mInput = FORWARD;
	ecs.createEvent(movement_event);

	const float DELTA = 0.1f;
	ecs.update(DELTA);

	EXPECT_TRUE(p_object_collision_component1->mIntersect);

	ecs.update(DELTA);

	// Resetting the position and moving another direction
	//EXPECT_FALSE(p_object_collision_component1->mIntersect);
}
#pragma endregion