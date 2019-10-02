#pragma once
#include "PhysicsComponents.h"
#include "UtilityComponents.h"
#include <vector>


struct Object
{
	ecs::components::TransformComponent* pTransform;
	ecs::components::ObjectCollisionComponent* pBoundingBox;
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



private:
	int mMaxLevel;
	int mMaxObjects;
	
	int mLevel;
	float mXBounds[2];
	float mZBounds[2];
	QuadTree* mChilden[4];
	QuadTree* mpParent;

	std::vector<Object> mObjects;

};