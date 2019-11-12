#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "BoundingVolume.h"
#include "PhysicsHelperFunctions.h"
using namespace PhysicsHelpers;

enum BV_TYPE
{
	COLLISION_SPHERE,
	COLLISION_OBB,
	COLLISION_AABB,
	COLLISION_CYLINDER,
	COLLISION_ERROR
};

struct CollisionInfo
{
	float mOverlap;
	XMFLOAT3 mNormal;
};

class CollisionObject
{
public:
	BV_TYPE mType					= COLLISION_ERROR;
	BoundingVolume* mBoundingVolume = nullptr;
	CollisionObject* mNext			= nullptr;

	~CollisionObject();

	CollisionInfo GetCollisionInfo(OBB& obb);

private:
	// Functions that return the collision normal 
	// and overlap of the collision based on type
	// of this collision object.
	CollisionInfo GetOBBCollisionInfo(OBB& obb);
	CollisionInfo GetAABBCollisionInfo(OBB& obb);
	CollisionInfo GetSphereCollisionInfo(OBB& obb);
	CollisionInfo GetCylinderCollisionInfo(OBB& obb);
};