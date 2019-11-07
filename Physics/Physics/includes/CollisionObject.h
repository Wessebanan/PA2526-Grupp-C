#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "BoundingVolume.h"

enum BV_TYPE
{
	COLLISION_SPHERE,
	COLLISION_OBB,
	COLLISION_AABB,
	COLLISION_CYLINDER
};


class CollisionObject
{
public:
	BV_TYPE mType;
	BoundingVolume* mBoundingVolume;
	CollisionObject* mNext;

	CollisionObject() { mNext = nullptr; };
	~CollisionObject() { delete mNext; mNext = nullptr; };
};