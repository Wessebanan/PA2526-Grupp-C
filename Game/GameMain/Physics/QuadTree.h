#pragma once
#include "../Physics/PhysicsComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include <vector>

/*QuadTreeObject is mainly used in the QuadTree. It's components is used to determin what quad
of the quadtree the object should be placed in.*/
struct QuadTreeObject
{
	ecs::components::TransformComponent* pTransform;
	ecs::components::ObjectCollisionComponent* pBoundingBox;

	QuadTreeObject(ecs::components::TransformComponent* p_trans, ecs::components::ObjectCollisionComponent* p_col)
	{
		pTransform = p_trans;
		pBoundingBox = p_col;
	}
};

/*The QuadTree class is used to store QuadTreeObjects for better collsion checks. It places the objects in
layers and nodes to cull objects that are far away from eachother from collision checks.*/
class QuadTree
{
public:
	QuadTree(int level, float xMin, float xMax, float zMin, float zMax, QuadTree* pParent);
	~QuadTree();

	/*This function clears the tree from all the objects and nodes.*/
	void Clear();
	/*This function is called from the Insert function and splits the current node inte four nodes 
	if the current node already have the maximum number of objects and is not in the deepest layer.*/
	void Split();
	/*This function is called in insert and returns the index of the child node that the object fully fits in.
	If it doesn't fully fit in any of the child nodes it will return -1 and the object will be placed in the current
	node.*/
	int GetIndex(QuadTreeObject obj);
	/*This function determines which node the object should be placed in and splits the node if the number of objects 
	in it already is at maximum capacity.*/
	void Insert(QuadTreeObject obj);
	/*Returns an std::vector with every object that the given object can collide with. It also returns itself in that
	vector.*/
	void RetrieveCollisions(std::vector<QuadTreeObject>& objects, QuadTreeObject obj);
	/*Testing function used to see how many nodes and objects that exist inside the tree at a given time.*/
	void NumberOfNodesAndObjects(int& nodes, int& objects);

private:
	int mMaxLevel;
	int mMaxObjects;

	int mLevel;
	float mXBounds[2];
	float mZBounds[2];
	QuadTree* mChildren[4];
	QuadTree* mpParent;

	std::vector<QuadTreeObject> mObjects;

};