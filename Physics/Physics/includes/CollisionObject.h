#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "BoundingVolume.h"

enum BV_TYPE
{
	COLLISION_SPHERE,
	COLLISION_OBB,
	COLLISION_AABB,
	COLLISION_CYLINDER,
	COLLISION_ERROR
};


class CollisionObject
{
public:
	BV_TYPE mType					= COLLISION_ERROR;
	BoundingVolume* mBoundingVolume = nullptr;
	CollisionObject* mNext			= nullptr;

	~CollisionObject() { delete mNext; mNext = nullptr; };
};