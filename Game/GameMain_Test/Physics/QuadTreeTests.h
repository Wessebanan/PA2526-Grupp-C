#pragma once
#include "ecs.h"
#include "../Physics/QuadTree.h"
#include "../../AI/includes/GridProp.h"

#pragma region QuadTreeTests
TEST(QuadTreeTests, AddObjectsToTree)
{
	//Calculate some values we need for the test.
	float pi = 3.1415f;
	float tilecenter_to_side = cos(30 * pi / 180) * TILE_RADIUS;
	float arena_min_x = -TILE_RADIUS;
	float arena_max_x = (TILE_RADIUS * 2) * ARENA_COLUMNS - TILE_RADIUS;
	float arena_min_z = - tilecenter_to_side;
	float arena_max_z = (tilecenter_to_side * 2) * ARENA_ROWS - TILE_RADIUS;
	
	//Create the quadtree with its min and max values.
	QuadTree tree(0, arena_min_x, arena_max_x, arena_min_z, arena_max_z, nullptr);
	//Create ten objects that should be placed in the bottom left quad 
	ecs::components::TransformComponent transform_comp;
	transform_comp.position = DirectX::XMFLOAT3(arena_min_x + TILE_RADIUS * 2, 10.0f, arena_min_z + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp;
	obj_col_comp.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	QuadTreeObject obj(&transform_comp, &obj_col_comp);
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}
	//Create ten objects that should be placed in the bottom right quad 
	ecs::components::TransformComponent transform_comp2;
	transform_comp2.position = DirectX::XMFLOAT3((arena_max_x - arena_min_x) / 2.0f + TILE_RADIUS * 2, 10.0f, arena_min_z + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp2;
	obj_col_comp2.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp2.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp2;
	obj.pTransform = &transform_comp2;
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}
	//Create ten objects that should be placed in the top left quad 
	ecs::components::TransformComponent transform_comp3;
	transform_comp3.position = DirectX::XMFLOAT3(arena_min_x + TILE_RADIUS * 2, 10.0f, (arena_max_z - arena_min_z) / 2.0f + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp3;
	obj_col_comp3.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp3.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp3;
	obj.pTransform = &transform_comp3;
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}
	//Create ten objects that should be placed in the top right quad 
	ecs::components::TransformComponent transform_comp4;
	transform_comp4.position = DirectX::XMFLOAT3((arena_max_x - arena_min_x) / 2.0f + TILE_RADIUS * 2, 10.0f, (arena_max_z - arena_min_z) / 2.0f + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp4;
	obj_col_comp4.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp4.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp4;
	obj.pTransform = &transform_comp4;
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}
	//Create one more object for the top right quad that should make it split into another layer
	ecs::components::TransformComponent transform_comp5;
	transform_comp5.position = DirectX::XMFLOAT3(arena_max_x - TILE_RADIUS * 2, 10.0f, (arena_max_z - arena_min_z) / 2.0f + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp5;
	obj_col_comp5.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp5.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp5;
	obj.pTransform = &transform_comp5;
	tree.Insert(obj);
	
	//Check to see that the number of nodes and objects in the tree is what we expect it to be.
	int nodes = 0;
	int objects = 0;
	tree.NumberOfNodesAndObjects(nodes, objects);
	EXPECT_EQ(nodes, 9);
	EXPECT_EQ(objects, 41);
	tree.Clear();

}

TEST(QuadTreeTests, ClearTree)
{
	//Calculate some values we need for the test.
	float pi = 3.1415f;
	float tilecenter_to_side = cos(30 * pi / 180) * TILE_RADIUS;
	float arena_min_x = -TILE_RADIUS;
	float arena_max_x = (TILE_RADIUS * 2) * ARENA_COLUMNS - TILE_RADIUS;
	float arena_min_z = -tilecenter_to_side;
	float arena_max_z = (tilecenter_to_side * 2) * ARENA_ROWS - TILE_RADIUS;

	//Create the quadtree with its min and max values.
	QuadTree tree(0, arena_min_x, arena_max_x, arena_min_z, arena_max_z, nullptr);
	//Create ten objects that should be placed in the bottom left quad 
	ecs::components::TransformComponent transform_comp;
	transform_comp.position = DirectX::XMFLOAT3(arena_min_x + TILE_RADIUS * 2, 10.0f, arena_min_z + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp;
	obj_col_comp.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	QuadTreeObject obj(&transform_comp, &obj_col_comp);
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}
	//Create ten objects that should be placed in the bottom right quad 
	ecs::components::TransformComponent transform_comp2;
	transform_comp2.position = DirectX::XMFLOAT3((arena_max_x - arena_min_x) / 2.0f + TILE_RADIUS * 2, 10.0f, arena_min_z + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp2;
	obj_col_comp2.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp2.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp2;
	obj.pTransform = &transform_comp2;
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}
	//Create ten objects that should be placed in the top left quad 
	ecs::components::TransformComponent transform_comp3;
	transform_comp3.position = DirectX::XMFLOAT3(arena_min_x + TILE_RADIUS * 2, 10.0f, (arena_max_z - arena_min_z) / 2.0f + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp3;
	obj_col_comp3.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp3.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp3;
	obj.pTransform = &transform_comp3;
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}
	//Create ten objects that should be placed in the top right quad 
	ecs::components::TransformComponent transform_comp4;
	transform_comp4.position = DirectX::XMFLOAT3((arena_max_x - arena_min_x) / 2.0f + TILE_RADIUS * 2, 10.0f, (arena_max_z - arena_min_z) / 2.0f + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp4;
	obj_col_comp4.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp4.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp4;
	obj.pTransform = &transform_comp4;
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}
	//Create one more object for the top right quad that should make it split into another layer
	ecs::components::TransformComponent transform_comp5;
	transform_comp5.position = DirectX::XMFLOAT3(arena_max_x - TILE_RADIUS * 2, 10.0f, (arena_max_z - arena_min_z) / 2.0f + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp5;
	obj_col_comp5.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp5.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp5;
	obj.pTransform = &transform_comp5;
	tree.Insert(obj);

	//Check to see that the number of nodes and objects in the tree is what we expect it to be.
	int nodes = 0;
	int objects = 0;
	tree.NumberOfNodesAndObjects(nodes, objects);
	EXPECT_EQ(nodes, 9);
	EXPECT_EQ(objects, 41);
	
	//Clear the tree and check again.
	tree.Clear();
	nodes = 0; 
	objects = 0;
	tree.NumberOfNodesAndObjects(nodes, objects);
	EXPECT_EQ(nodes, 1);
	EXPECT_EQ(objects, 0);
}

TEST(QuadTreeTests, GeTCollisionList)
{
	//Calculate some values we need for the test.
	float pi = 3.1415f;
	float tilecenter_to_side = cos(30 * pi / 180) * TILE_RADIUS;
	float arena_min_x = -TILE_RADIUS;
	float arena_max_x = (TILE_RADIUS * 2) * ARENA_COLUMNS - TILE_RADIUS;
	float arena_min_z = -tilecenter_to_side;
	float arena_max_z = (tilecenter_to_side * 2) * ARENA_ROWS - TILE_RADIUS;

	//Create the quadtree with its min and max values.
	QuadTree tree(0, arena_min_x, arena_max_x, arena_min_z, arena_max_z, nullptr);
	//Create ten objects that should be placed in the bottom left quad 
	ecs::components::TransformComponent transform_comp;
	transform_comp.position = DirectX::XMFLOAT3(arena_min_x + TILE_RADIUS * 2, 10.0f, arena_min_z + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp;
	obj_col_comp.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	QuadTreeObject obj(&transform_comp, &obj_col_comp);
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}
	//Create ten objects that should be placed in the bottom right quad 
	ecs::components::TransformComponent transform_comp2;
	transform_comp2.position = DirectX::XMFLOAT3((arena_max_x - arena_min_x) / 2.0f + TILE_RADIUS * 2, 10.0f, arena_min_z + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp2;
	obj_col_comp2.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp2.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp2;
	obj.pTransform = &transform_comp2;
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}
	//Create ten objects that should be placed in the top left quad 
	ecs::components::TransformComponent transform_comp3;
	transform_comp3.position = DirectX::XMFLOAT3(arena_min_x + TILE_RADIUS * 2, 10.0f, (arena_max_z - arena_min_z) / 2.0f + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp3;
	obj_col_comp3.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp3.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp3;
	obj.pTransform = &transform_comp3;
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}
	//Create ten objects that should be placed in the top right quad 
	ecs::components::TransformComponent transform_comp4;
	transform_comp4.position = DirectX::XMFLOAT3((arena_max_x - arena_min_x) / 2.0f + TILE_RADIUS * 2, 10.0f, (arena_max_z - arena_min_z) / 2.0f + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp4;
	obj_col_comp4.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp4.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp4;
	obj.pTransform = &transform_comp4;
	for (int i = 0; i < 10; i++)
	{
		tree.Insert(obj);
	}
	//Create one more object for the top right quad that should make it split into another layer
	ecs::components::TransformComponent transform_comp5;
	transform_comp5.position = DirectX::XMFLOAT3(arena_max_x - TILE_RADIUS * 2, 10.0f, (arena_max_z - arena_min_z) / 2.0f + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp5;
	obj_col_comp5.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp5.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp5;
	obj.pTransform = &transform_comp5;
	tree.Insert(obj);

	//Create a vector which will fetch the list off possible collision.
	std::vector<QuadTreeObject> object_vector;
	obj.pBoundingBox = &obj_col_comp4;
	obj.pTransform = &transform_comp4;
	tree.RetrieveCollisions(object_vector, obj);
	//Check to see that the number of possible collisions is what we expect.
	EXPECT_EQ(object_vector.size(), 10);
	tree.Clear();
	object_vector.clear();
}

TEST(QuadTreeTests, FillQuadTreeSystem)
{
	//Calculate some values we need for the test.
	float pi = 3.1415f;
	float tilecenter_to_side = cos(30 * pi / 180) * TILE_RADIUS;
	float arena_min_x = -TILE_RADIUS;
	float arena_max_x = (TILE_RADIUS * 2) * ARENA_COLUMNS - TILE_RADIUS;
	float arena_min_z = -tilecenter_to_side;
	float arena_max_z = (tilecenter_to_side * 2) * ARENA_ROWS - TILE_RADIUS;
	//Create the quadtree with its min and max values.
	QuadTree tree(0, arena_min_x, arena_max_x, arena_min_z, arena_max_z, nullptr);
	ecs::components::QuadTreeComponent my_tree;
	my_tree.pTree = &tree;
	//Create the ecs
	ecs::EntityComponentSystem my_ecs;
	my_ecs.createEntity(my_tree);
	my_ecs.createSystem<ecs::systems::FillQuadTreeSystem>();
	//Create ten objects that should be placed in the bottom left quad 
	ecs::components::TransformComponent transform_comp;
	transform_comp.position = DirectX::XMFLOAT3(arena_min_x + TILE_RADIUS * 2, 10.0f, arena_min_z + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp;
	obj_col_comp.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	QuadTreeObject obj(&transform_comp, &obj_col_comp);
	for (int i = 0; i < 10; i++)
	{
		my_ecs.createEntity(transform_comp, obj_col_comp);
	}
	//Create ten objects that should be placed in the bottom right quad 
	//Create ten objects that should be placed in the bottom right quad 
	ecs::components::TransformComponent transform_comp2;
	transform_comp2.position = DirectX::XMFLOAT3((arena_max_x - arena_min_x) / 2.0f + TILE_RADIUS * 2, 10.0f, arena_min_z + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp2;
	obj_col_comp2.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp2.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp2;
	obj.pTransform = &transform_comp2;
	for (int i = 0; i < 10; i++)
	{
		my_ecs.createEntity(transform_comp2, obj_col_comp2);
	}
	//Create ten objects that should be placed in the top left quad 
	//Create ten objects that should be placed in the top left quad 
	ecs::components::TransformComponent transform_comp3;
	transform_comp3.position = DirectX::XMFLOAT3(arena_min_x + TILE_RADIUS * 2, 10.0f, (arena_max_z - arena_min_z) / 2.0f + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp3;
	obj_col_comp3.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp3.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp3;
	obj.pTransform = &transform_comp3;
	for (int i = 0; i < 10; i++)
	{
		my_ecs.createEntity(transform_comp3, obj_col_comp3);
	}
	//Create ten objects that should be placed in the top right quad 
	ecs::components::TransformComponent transform_comp4;
	transform_comp4.position = DirectX::XMFLOAT3((arena_max_x - arena_min_x) / 2.0f + TILE_RADIUS * 2, 10.0f, (arena_max_z - arena_min_z) / 2.0f + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp4;
	obj_col_comp4.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp4.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp4;
	obj.pTransform = &transform_comp4;
	for (int i = 0; i < 10; i++)
	{
		my_ecs.createEntity(transform_comp4, obj_col_comp4);
	}
	//Create one more object for the top right quad that should make it split into another layer
	ecs::components::TransformComponent transform_comp5;
	transform_comp5.position = DirectX::XMFLOAT3(arena_max_x - TILE_RADIUS * 2, 10.0f, (arena_max_z - arena_min_z) / 2.0f + TILE_RADIUS * 2);
	ecs::components::ObjectCollisionComponent obj_col_comp5;
	obj_col_comp5.mAABB.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	obj_col_comp5.mAABB.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	obj.pBoundingBox = &obj_col_comp5;
	obj.pTransform = &transform_comp5;
	my_ecs.createEntity(transform_comp5, obj_col_comp5);

	my_ecs.update(0.1f);
	//Check to see that the number of nodes and objects in the tree is what we expect it to be.
	int nodes = 0;
	int objects = 0;
	tree.NumberOfNodesAndObjects(nodes, objects);
	EXPECT_EQ(nodes, 9);
	EXPECT_EQ(objects, 41);
	//Clear the tree
	tree.Clear();
	//Check to see that the number of nodes and objects in the tree is what we expect it to be.
	nodes = 0;
	objects = 0;
	tree.NumberOfNodesAndObjects(nodes, objects);
	EXPECT_EQ(nodes, 1);
	EXPECT_EQ(objects, 0);
	//Update again
	my_ecs.update(0.1f);
	//Check to see that the number of nodes and objects in the tree is what we expect it to be.
	nodes = 0;
	objects = 0;
	tree.NumberOfNodesAndObjects(nodes, objects);
	EXPECT_EQ(nodes, 9);
	EXPECT_EQ(objects, 41);
}
#pragma endregion