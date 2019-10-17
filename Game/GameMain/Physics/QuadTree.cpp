#include "QuadTree.h"

#include <DirectXMath.h>
#include "../gameUtility/UtilityEcsFunctions.h"

QuadTree::QuadTree(int level, float xMin, float xMax, float zMin, float zMax, QuadTree* pParent)
{
	mMaxLevel = 5;
	mMaxObjects = 10;
	mLevel = level;
	mXBounds[0] = xMin;
	mXBounds[1] = xMax;
	mZBounds[0] = zMin;
	mZBounds[1] = zMax;
	for (int i = 0; i < 4; i++)
	{
		mChildren[i] = nullptr;
	}
	mpParent = pParent;
}

QuadTree::~QuadTree()
{
	Clear();
}

void QuadTree::Clear()
{
	for (int i = 0; i < mObjects.size(); i++)
	{
		mObjects[i].pBoundingBox = nullptr;
		mObjects[i].pTransform = nullptr;
	}
	mObjects.clear();
	mpParent = nullptr;
	for (int i = 0; i < 4; i++)
	{
		if (mChildren[i] != nullptr)
		{
			mChildren[i]->Clear();
			delete mChildren[i];
			mChildren[i] = nullptr;
		}
	}
}

void QuadTree::Split()
{
	float half_width = (mXBounds[1] - mXBounds[0]) / 2.0f;
	float half_height = (mZBounds[1] - mZBounds[0]) / 2.0f;

	mChildren[0] = new QuadTree(mLevel + 1, mXBounds[0], mXBounds[0] + half_width, mZBounds[0], mZBounds[0] + half_height, this);
	mChildren[1] = new QuadTree(mLevel + 1, mXBounds[0] + half_width, mXBounds[1], mZBounds[0], mZBounds[0] + half_height, this);
	mChildren[2] = new QuadTree(mLevel + 1, mXBounds[0], mXBounds[0] + half_width, mZBounds[0] + half_height, mZBounds[1], this);
	mChildren[3] = new QuadTree(mLevel + 1, mXBounds[0] + half_width, mXBounds[1], mZBounds[0] + half_height, mZBounds[1], this);
}

int QuadTree::GetIndex(QuadTreeObject obj)
{
	//If the function returns -1 the object belongs in the parent.
	int index = -1;
	//Fetch the objects max and min position in model space.
	DirectX::XMVECTOR min_position = DirectX::XMLoadFloat3(&obj.pBoundingBox->mAABB.Center);
	DirectX::XMVECTOR max_position = DirectX::XMLoadFloat3(&obj.pBoundingBox->mAABB.Center);
	DirectX::XMVECTOR extents = DirectX::XMLoadFloat3(&obj.pBoundingBox->mAABB.Extents);
	min_position = DirectX::XMVectorSubtract(min_position, extents);
	max_position = DirectX::XMVectorAdd(max_position, extents);
	DirectX::XMMATRIX world_matrix = UtilityEcsFunctions::GetWorldMatrix(*obj.pTransform);
	//Convert the max and min position to world space.
	min_position = DirectX::XMVector3Transform(min_position, world_matrix);
	max_position = DirectX::XMVector3Transform(max_position, world_matrix);
	//Store them in floats for easier access to its variables.
	DirectX::XMFLOAT3 min_pos;
	DirectX::XMFLOAT3 max_pos;
	DirectX::XMStoreFloat3(&min_pos, min_position);
	DirectX::XMStoreFloat3(&max_pos, max_position);
	//Calculate the halfway point of the childrens squares.
	float half_width = (mXBounds[1] - mXBounds[0]) / 2.0f;
	float half_height = (mZBounds[1] - mZBounds[0]) / 2.0f;
	bool left_quads = false;
	bool right_quads = false;

	//Check if the object fully fits in the left half of the square.
	if (min_pos.x > mXBounds[0] && max_pos.x < mXBounds[0] + half_width)
	{
		left_quads = true;
	}
	//Check if the object fully fits in the right half of the square
	else if (min_pos.x > mXBounds[0] + half_width && max_pos.x < mXBounds[1])
	{
		right_quads = true;
	}
	//Check if the object fully fits within the bottom quadrants
	if (min_pos.z > mZBounds[0] && max_pos.z < mZBounds[0] + half_height)
	{
		if (left_quads)
			index = 0;
		else if (right_quads)
			index = 1;
	}
	//Check if object fully fits in the top quadrants.
	else if (min_pos.z > mZBounds[0] + half_height && max_pos.z < mZBounds[1])
	{
		if (left_quads)
			index = 2;
		else if (right_quads)
			index = 3;
	}
	return index;
}

void QuadTree::Insert(QuadTreeObject obj)
{
	int index;
	if (mChildren[0] != nullptr)
	{
		index = GetIndex(obj);
		if (index != -1)
		{
			mChildren[index]->Insert(obj);
			return;
		}
	}

	mObjects.push_back(obj);
	if (mObjects.size() > mMaxObjects && mLevel < mMaxLevel)
	{
		if (mChildren[0] == nullptr)
		{
			Split();
		}
		int i = 0;
		while (i < mObjects.size())
		{
			index = GetIndex(mObjects[i]);
			if (index != -1)
			{
				mChildren[index]->Insert(mObjects[i]);
				mObjects.erase(mObjects.begin() + i);
			}
			else
			{
				i++;
			}
		}
	}
}

void QuadTree::RetrieveCollisions(std::vector<QuadTreeObject>& objects, QuadTreeObject obj)
{
	int index = GetIndex(obj);
	if (index != -1 && mChildren[0] != nullptr)
	{
		mChildren[index]->RetrieveCollisions(objects, obj);
	}

	for (int i = 0; i < mObjects.size(); i++)
	{
		objects.push_back(mObjects[i]);
	}
}

void QuadTree::NumberOfNodesAndObjects(int& nodes, int& objects)
{
	for (int i = 0; i < 4; i++)
	{
		if (mChildren[i] != nullptr)
		{
			mChildren[i]->NumberOfNodesAndObjects(nodes, objects);
		}
	}
	nodes++;
	objects += mObjects.size();
}