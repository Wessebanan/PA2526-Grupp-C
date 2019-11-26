#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "BoundingVolume.h"
#include "PhysicsHelperFunctions.h"
using namespace PhysicsHelpers;

class CollisionObject
{
public:
	BV_TYPE mType					= COLLISION_ERROR;
	BoundingVolume* mBoundingVolume = nullptr;
	CollisionObject* mNext			= nullptr;

	~CollisionObject();
};