#pragma once
#include "../Physics/PhysicsComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include <vector>


struct Object
{
	ecs::components::TransformComponent* pTransform;
	ecs::components::ObjectCollisionComponent* pBoundingBox;

	Object(ecs::components::TransformComponent* p_trans, ecs::components::ObjectCollisionComponent* p_col)
	{
		pTransform = p_trans;
		pBoundingBox = p_col;
	}
};

class QuadTree
{
public:
	QuadTree(int level, float xMin, float xMax, float zMin, float zMax, QuadTree* pParent);
	~QuadTree();

	void Clear();
	void Split();
	int GetIndex(Object obj);
	void Insert(Object obj);
	void RetrieveCollisions(std::vector<Object>& objects, Object obj);
	//Testing Functions
	void NumberOfNodesAndObjects(int& nodes, int& objects);

private:
	int mMaxLevel;
	int mMaxObjects;

	int mLevel;
	float mXBounds[2];
	float mZBounds[2];
	QuadTree* mChildren[4];
	QuadTree* mpParent;

	std::vector<Object> mObjects;

};