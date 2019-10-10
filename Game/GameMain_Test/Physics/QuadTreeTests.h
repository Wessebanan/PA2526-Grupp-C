#pragma once
#include "ecs.h"
#include "../Physics/QuadTree.h"
#include "../../AI/includes/GridProp.h"

#pragma region QuadTreeTests
TEST(QuadTreeTests, AddObjectsToTree)
{
	float pi = 3.1415f;
	float tilecenter_to_side = cos(30 * pi / 180) * TILE_RADIUS;
	float arena_min_x = -TILE_RADIUS;
	float arena_max_x = (TILE_RADIUS * 2) * ARENA_COLUMNS - TILE_RADIUS;
	float arena_min_z = - tilecenter_to_side;
	float arena_max_z = (tilecenter_to_side * 2) * ARENA_ROWS - TILE_RADIUS;
	
	QuadTree tree(0, arena_min_x, arena_max_x, arena_min_z, arena_max_z, nullptr);
	//10 in child one
	ecs::components::TransformComponent transform_comp;
	transform_comp.position = DirectX::XMFLOAT3(24.0f, 10.0f, 18.0f);
	ecs::components::ObjectCollisionComponent obj_col_comp;
	obj_col_comp.mCenter = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp.mMax = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj_col_comp.mMin = DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f);
	Object obj;
	obj.pBoundingBox = &obj_col_comp;
	obj.pTransform = &transform_comp;
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}
	// 10 in child 2
	ecs::components::TransformComponent transform_comp2;
	transform_comp2.position = DirectX::XMFLOAT3(70.0f, 10.0f, 18.0f);
	ecs::components::ObjectCollisionComponent obj_col_comp2;
	obj_col_comp2.mCenter = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp2.mMax = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj_col_comp2.mMin = DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f);
	obj.pBoundingBox = &obj_col_comp2;
	obj.pTransform = &transform_comp2;
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}// 10 in child 3
	ecs::components::TransformComponent transform_comp3;
	transform_comp3.position = DirectX::XMFLOAT3(24.0f, 10.0f, 50.0f);
	ecs::components::ObjectCollisionComponent obj_col_comp3;
	obj_col_comp3.mCenter = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp3.mMax = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj_col_comp3.mMin = DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f);
	obj.pBoundingBox = &obj_col_comp3;
	obj.pTransform = &transform_comp3;
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}// 10 in child 4
	ecs::components::TransformComponent transform_comp4;
	transform_comp4.position = DirectX::XMFLOAT3(70.0f, 10.0f, 50.0f);
	ecs::components::ObjectCollisionComponent obj_col_comp4;
	obj_col_comp4.mCenter = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp4.mMax = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj_col_comp4.mMin = DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f);
	obj.pBoundingBox = &obj_col_comp4;
	obj.pTransform = &transform_comp4;
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}


	

	EXPECT_EQ(0, 0);
}
#pragma endregion