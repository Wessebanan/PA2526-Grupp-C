#pragma once
#include "FightingSystem.h"
#include "UtilityComponents.h"


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
	ModelLoader::Mesh sword("Physics/TestModel/sword.fbx");

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
	ModelLoader::Mesh sword("Physics/TestModel/sword.fbx");

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
		ContainmentType containtment_type = obb->Contains(DirectX::XMLoadFloat3(&vertices->at(i)));
		if (containtment_type == ContainmentType::CONTAINS)
		{
			n_inside++;
		}		
	}

	// DirectXCollision disregards a few points in making the box, for optimization probably.
	// If 99% of vertices are inside, it is safe to assume that the box has been created properly.
	EXPECT_GT((float)n_inside / (float)vertices->size(), 0.99f);

	delete p_weapon_component->mBoundingVolume;
	
}
#pragma endregion
#pragma region DamageDealing
TEST(DamageDealing, CreateDamageSystem)
{
	ecs::EntityComponentSystem ecs;
	EXPECT_EQ(ecs.getTotalSystemCount(), 0);

	ecs::systems::DamageSystem* damage_system = ecs.createSystem<ecs::systems::DamageSystem>();
	EXPECT_EQ(ecs.getTotalSystemCount(), 1);

	EXPECT_NE(damage_system, nullptr);
}
TEST(DamageDealing, CreateOwnedWeapon)
{
	ecs::EntityComponentSystem ecs;
	
	// Creating the components that the entity needs to be
	// able to collide with other entities.
	MeshComponent unit_mesh_component;
	ObjectCollisionComponent unit_collision_component;
	DynamicMovementComponent unit_movement_component;
	TransformComponent unit_transform_component;

	ecs::ComponentList component_list;
	ecs::BaseComponent* list[] =
	{
		&unit_mesh_component,
		&unit_collision_component,
		&unit_movement_component,
		&unit_transform_component
	};
	component_list.initialInfo = list;
	component_list.componentCount = 4;
	ecs::Entity* unit_entity = ecs.createEntity(component_list);

	// Checking that there is now one entity and four components.
	EXPECT_EQ(ecs.getTotalEntityCount(), 1);
	EXPECT_EQ(ecs.getTotalComponentCount(), 4);

	WeaponComponent weapon_component;
	weapon_component.mOwnerEntity = unit_entity->getID();
	TransformComponent weapon_transform_component;
	MeshComponent weapon_mesh_component;

	ecs::Entity* weapon_entity = ecs.createEntity(weapon_mesh_component, weapon_transform_component, weapon_component);

	// Checking that there is now two entities and seven components.
	EXPECT_EQ(ecs.getTotalEntityCount(), 2);
	EXPECT_EQ(ecs.getTotalComponentCount(), 7);

	WeaponComponent *p_weapon_component = ecs.getComponentFromEntity<WeaponComponent>(weapon_entity->getID());

	// Checking that the unit entity owns the weapon entity.
	EXPECT_EQ(unit_entity, ecs.getEntity(p_weapon_component->mOwnerEntity));
}

#pragma endregion
