#pragma once
#include "CollisionSystem.h"

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