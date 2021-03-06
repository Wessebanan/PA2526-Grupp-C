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
	ModelLoader::Mesh dude("Physics/TestModel/dude.fbx");

	// Creating the dude that is supposed to collide with objects.
	MeshComponent mesh_component;
	mesh_component.mMesh = &dude;
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
	ModelLoader::Mesh dude("Physics/TestModel/dude.fbx");

	MeshComponent mesh_component;
	mesh_component.mMesh = &dude;
	ObjectCollisionComponent object_collision_component;

	ecs::Entity* object_collision_entity = ecs.createEntity(mesh_component, object_collision_component);

	// Getting a pointer to the ground collision component to check its values.
	ObjectCollisionComponent* p_object_collision_component = dynamic_cast<ObjectCollisionComponent*>(ecs.getComponent(ObjectCollisionComponent::typeID, object_collision_entity->getComponentID(ObjectCollisionComponent::typeID)));

	// Checking that enough vertices are inside the box.
	std::vector<DirectX::XMFLOAT3>* vertices = mesh_component.mMesh->GetVertexPositionVector();
	unsigned int n_inside = 0;
	for (int i = 0; i < vertices->size(); i++)
	{
		ContainmentType containtment_type = p_object_collision_component->mAABB.Contains(XMLoadFloat3(&vertices->at(i)));
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
	ModelLoader::Mesh dude("Physics/TestModel/dude.fbx");

	// Creating the components that the entity needs to be
	// able to collide with other entities.
	MeshComponent mesh_component;
	mesh_component.mMesh = &dude;
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
	ModelLoader::Mesh dude("Physics/TestModel/dude.fbx");

	MeshComponent mesh_component;
	mesh_component.mMesh = &dude;
	ObjectCollisionComponent object_collision_component;
	DynamicMovementComponent dynamic_movement_component;
	dynamic_movement_component.mForward = XMFLOAT3(1.0f, 0.0f, 0.0f);
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
	p_object_collision_component1->mAABB.Extents.x * 2.0f;
	p_transform_component2->position.x = p_object_collision_component1->mAABB.Extents.x * 2.0f;

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

	// Entity1 now moves into entity2 which should set the intersection bool.
	EXPECT_TRUE(p_object_collision_component1->mIntersect);

	ecs.update(DELTA);

	// Resetting the position and moving another direction.
	// (easier than trying to move out with an event because 
	// of acceleration and shit).
	TransformComponent* p_transform_component1 = dynamic_cast<TransformComponent*>(ecs.getComponent(TransformComponent::typeID, entity1->getComponentID(TransformComponent::typeID)));
	p_transform_component1->position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	movement_event.mInput = BACKWARD;
	ecs.createEvent(movement_event);

	ecs.update(DELTA);
	
	// Entity1 now moves away from entity2 which should not set the intersection bool.
	//EXPECT_FALSE(p_object_collision_component1->mIntersect);
}
#pragma endregion
#pragma region ObjectCollisionHandling
// Init/Creation is tested in ObjectCollisionCheck
TEST(ObjectCollisionHandling, HandleCollision)
{
	ecs::EntityComponentSystem ecs;

	ecs.createSystem<ecs::systems::ObjectBoundingVolumeInitSystem>();
	ecs.createSystem<ecs::systems::ObjectCollisionSystem>();
	ecs.createSystem<ecs::systems::DynamicMovementSystem>();
	ModelLoader::Mesh dude("Physics/TestModel/dude.fbx");

	MeshComponent mesh_component;
	mesh_component.mMesh = &dude;
	ObjectCollisionComponent object_collision_component;
	DynamicMovementComponent dynamic_movement_component;
	dynamic_movement_component.mForward = XMFLOAT3(1.0f, 0.0f, 0.0f);
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
	p_transform_component2->position.x = p_object_collision_component1->mAABB.Extents.x * 2.0f + 0.0001f;

	// This test is to see if entity1 collides properly with entity2.

	// Check that it currently does not intersect.
	EXPECT_FALSE(p_object_collision_component1->mIntersect);

	// Creating a movement input event to trigger 
	// a collision check.
	MovementInputEvent movement_event;
	movement_event.mEntityID = entity1->getID();
	movement_event.mInput = FORWARD;
	ecs.createEvent(movement_event);
	
	TransformComponent* p_transform_component1 = dynamic_cast<TransformComponent*>(ecs.getComponent(TransformComponent::typeID, entity1->getComponentID(TransformComponent::typeID)));

	const float DELTA = 1.0f;
	ecs.update(DELTA);
	
	// Entity1 now moves into entity2 which should set the intersection bool.
	//EXPECT_TRUE(p_object_collision_component1->mIntersect);
	//
	//ecs.update(DELTA);
	//
	//// Entity1 should now be placed outside of entity2.
	//EXPECT_FALSE(p_object_collision_component1->mIntersect);
}
#pragma endregion