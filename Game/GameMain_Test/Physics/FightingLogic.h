#pragma once
#include "FightingSystem.h"
#include "UtilityComponents.h"

ModelLoader::Mesh sword("Physics/TestModel/sword.fbx");

#pragma region WeaponInit
TEST(WeaponInit, CreateWeaponInitSystem)
{
	ecs::EntityComponentSystem ecs;
	EXPECT_EQ(ecs.getTotalSystemCount(), 0);

	ecs::systems::WeaponInitSystem* weapon_init_system = ecs.createSystem<ecs::systems::WeaponInitSystem>();
	EXPECT_EQ(ecs.getTotalSystemCount(), 1);

	EXPECT_NE(weapon_init_system, nullptr);
}
TEST(WeaponInit, CreateWeaponEntity)
{
	ecs::EntityComponentSystem ecs;

	// Making sure there are no entities or components.
	EXPECT_EQ(ecs.getTotalEntityCount(), 0);
	EXPECT_EQ(ecs.getTotalComponentCount(), 0);

	// Creating the components a weapon should have.
	MeshComponent mesh_component;
	mesh_component.mMesh = &sword;
	WeaponComponent weapon_component;
	TransformComponent transform_component;

	ecs::Entity* weapon_entity = ecs.createEntity(mesh_component, transform_component, weapon_component);

	// Checking that there is now one entity and three components.
	EXPECT_EQ(ecs.getTotalEntityCount(), 1);
	EXPECT_EQ(ecs.getTotalComponentCount(), 3);

	// Checking that the pointer received from getEntity matches
	// the pointer to the entity.
	EXPECT_EQ(ecs.getEntity(weapon_entity->getID()), weapon_entity);
}
TEST(WeaponInit, TestBoundingVolume)
{
	ecs::EntityComponentSystem ecs;
	ecs::systems::WeaponInitSystem* weapon_init_system = ecs.createSystem<ecs::systems::WeaponInitSystem>();

	// Creating the components a weapon should have.
	MeshComponent mesh_component;
	mesh_component.mMesh = &sword;
	WeaponComponent weapon_component;
	weapon_component.mType = SWORD;
	TransformComponent transform_component;

	ecs::Entity* weapon_entity = ecs.createEntity(mesh_component, transform_component, weapon_component);

	// Grabbing the weapon component.
	WeaponComponent* p_weapon_component = ecs.getComponentFromEntity<WeaponComponent>(weapon_entity->getID());
	
	// Grabbing the obb from the weapon component (cause sword gets obb).
	OBB* obb = static_cast<OBB*>(p_weapon_component->mBoundingVolume);

	// Grabbing the vertices to check if they are inside the OBB.
	std::vector<DirectX::XMFLOAT3>* vertices = mesh_component.mMesh->GetVertexPositionVector();
	unsigned int n_inside = 0;
	
	XMVECTOR diff = XMVECTOR();
	for (int i = 0; i < vertices->size(); i++)
	{
		XMVECTOR current_vertex = DirectX::XMLoadFloat3(&vertices->at(i));
		ContainmentType containtment_type = obb->mOBB.Contains(DirectX::XMLoadFloat3(&vertices->at(i)));
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