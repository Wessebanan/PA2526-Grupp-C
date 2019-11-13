#pragma once
#include <DirectXCollision.h>
#include <algorithm>
#include "PhysicsHelperFunctions.h"

#define HALF_SQRT_3 0.866f

using namespace DirectX;

struct BoundingCylinder
{
	XMFLOAT3 mCenter;
	float mExtentsY; // Half height.
	float mRadius;

	// Radius is center to corner
	void CreateFromTile(XMFLOAT3 position, float height, float radius = 1.0f);

	bool Intersects(BoundingBox& aabb);
	bool Intersects(BoundingSphere& sphere);
	bool Intersects(BoundingOrientedBox& obb);
	bool Intersects(BoundingCylinder& cylinder);
};


// Base struct for unspecified bounding volumes.
struct BoundingVolume
{
	virtual ~BoundingVolume() = default;
	virtual bool Intersects(BoundingVolume* other)	= 0;
	virtual void Transform(XMMATRIX transform)		= 0;
	virtual XMFLOAT3 GetCenter()					= 0;
};

//Empty struct inheriting from BoundingVolume (base class) and DirectX::BoundingSphere.
struct Sphere : public BoundingVolume, BoundingSphere 
{
	bool Intersects(BoundingVolume* other);
	void Transform(XMMATRIX transform);
	XMFLOAT3 GetCenter();
//private:
//	bool Intersects(Cylinder& cylinder);
};
//Empty struct inheriting from BoundingVolume (base class) and DirectX::BoundingOrientedBox.
struct OBB : public BoundingVolume, BoundingOrientedBox 
{
	bool Intersects(BoundingVolume* other);
	void Transform(XMMATRIX transform);
	XMFLOAT3 GetCenter();
//private:
//	bool Intersects(Cylinder& cylinder);
};
//Empty struct inheriting from BoundingVolume (base class) and DirectX::BoundingBox.
struct AABB : public BoundingVolume, BoundingBox 
{
	bool Intersects(BoundingVolume* other);
	void Transform(XMMATRIX transform);
	XMFLOAT3 GetCenter();
//private:
//	bool Intersects(Cylinder& cylinder);
};

//Axis aligned so that y is height, DO NOT ROTATE, specifically designed for tiles. 
struct Cylinder : public BoundingVolume, BoundingCylinder
{
	bool Intersects(BoundingVolume* other);
	void Transform(XMMATRIX transform);
	XMFLOAT3 GetCenter();
};