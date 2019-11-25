#pragma once
#include <DirectXCollision.h>
#include <algorithm>
#include "PhysicsHelperFunctions.h"

#define HALF_SQRT_3 0.866f

using namespace DirectX;
using namespace PhysicsHelpers;

enum BV_TYPE
{
	COLLISION_SPHERE,
	COLLISION_OBB,
	COLLISION_AABB,
	COLLISION_CYLINDER,
	COLLISION_ERROR
};

// Holds normal of collision between two bounding volumes 
// and the overlap in each direction.
struct CollisionInfo
{
	XMFLOAT3 mNormal	= XMFLOAT3(0.0f, 0.0f, 0.0f);
	float mOverlap		= -1.0f;

	CollisionInfo() 
	{
	}

	CollisionInfo(XMFLOAT3 normal, float overlap) 
		: mNormal(normal), mOverlap(overlap) 
	{
	}
};

struct BoundingCylinder
{
	XMFLOAT3 mCenter;
	float mExtentsY; // Half height.
	float mRadius;

	// Radius is center to corner
	void CreateFromPoints(BoundingCylinder &Out, size_t Count, const XMFLOAT3 *pPoints, size_t Stride);
	void CreateFromTile(XMFLOAT3 position, float radius = 1.0f);

	bool Intersects(BoundingBox& rAabb);
	bool Intersects(BoundingSphere& rSphere);
	bool Intersects(BoundingOrientedBox& rObb);
	bool Intersects(BoundingCylinder& rCylinder);
};


// Base struct for unspecified bounding volumes.
struct BoundingVolume
{
	virtual ~BoundingVolume() = default;
	virtual bool Intersects(BoundingVolume* pOther)				= 0;
	virtual void Transform(XMMATRIX transform)					= 0;
	virtual CollisionInfo GetCollisionInfo(BoundingVolume* bv)	= 0;
	virtual XMFLOAT3 GetCenter()								= 0;
	virtual XMFLOAT3 GetExtents()								= 0;
};

//Empty struct inheriting from BoundingVolume (base class) and DirectX::BoundingSphere.
struct Sphere : public BoundingVolume, BoundingSphere 
{
	bool Intersects(BoundingVolume* pOther);
	void Transform(XMMATRIX transform);
	XMFLOAT3 GetCenter();
	XMFLOAT3 GetExtents();

	CollisionInfo GetCollisionInfo(BoundingVolume* pOther);

	CollisionInfo GetCollisionInfo(BoundingSphere& rSphere);
	CollisionInfo GetCollisionInfo(BoundingBox& rAabb);
	CollisionInfo GetCollisionInfo(BoundingOrientedBox& rObb);
	CollisionInfo GetCollisionInfo(BoundingCylinder& rCylinder);
};
//Empty struct inheriting from BoundingVolume (base class) and DirectX::BoundingOrientedBox.
struct OBB : public BoundingVolume, BoundingOrientedBox 
{
	bool Intersects(BoundingVolume* pOther);
	void Transform(XMMATRIX transform);
	XMFLOAT3 GetCenter();
	XMFLOAT3 GetExtents();

	CollisionInfo GetCollisionInfo(BoundingVolume* pOther);

	CollisionInfo GetCollisionInfo(BoundingSphere& rSphere);
	CollisionInfo GetCollisionInfo(BoundingBox& rAabb);
	CollisionInfo GetCollisionInfo(BoundingOrientedBox& rObb);
	CollisionInfo GetCollisionInfo(BoundingCylinder& rCylinder);
};
//Empty struct inheriting from BoundingVolume (base class) and DirectX::BoundingBox.
struct AABB : public BoundingVolume, BoundingBox 
{
	bool Intersects(BoundingVolume* pOther);
	void Transform(XMMATRIX transform);
	XMFLOAT3 GetCenter();
	XMFLOAT3 GetExtents();

	CollisionInfo GetCollisionInfo(BoundingVolume* pOther);

	CollisionInfo GetCollisionInfo(BoundingSphere& rSphere);
	CollisionInfo GetCollisionInfo(BoundingBox& rAabb);
	CollisionInfo GetCollisionInfo(BoundingOrientedBox& rObb);
	CollisionInfo GetCollisionInfo(BoundingCylinder& rCylinder);
};

//Axis aligned so that y is height, DO NOT ROTATE, specifically designed for tiles. 
struct Cylinder : public BoundingVolume, BoundingCylinder
{
	bool Intersects(BoundingVolume* pOther);
	void Transform(XMMATRIX transform);
	XMFLOAT3 GetCenter();
	XMFLOAT3 GetExtents();

	CollisionInfo GetCollisionInfo(BoundingVolume* pOther);

	CollisionInfo GetCollisionInfo(BoundingSphere& rSphere);
	CollisionInfo GetCollisionInfo(BoundingBox& rAabb);
	CollisionInfo GetCollisionInfo(BoundingOrientedBox& rObb);
	CollisionInfo GetCollisionInfo(BoundingCylinder& rCylinder);
};