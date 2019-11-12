#pragma once
#include <DirectXCollision.h>
#define HALF_SQRT_3 0.866f

// Base struct for unspecified bounding volumes.
using namespace DirectX;
struct BoundingVolume
{
	virtual ~BoundingVolume() = default;
	virtual bool Intersects(BoundingVolume* other)	= 0;
	virtual void Transform(XMMATRIX transform)		= 0;
	virtual XMFLOAT3 GetCenter()					= 0;
};

//Empty struct inheriting from BoundingVolume (base class) and DirectX::BoundingSphere.
struct Sphere : public BoundingVolume, DirectX::BoundingSphere 
{
	bool Intersects(BoundingVolume* other);
	void Transform(XMMATRIX transform);
	XMFLOAT3 GetCenter();
};
//Empty struct inheriting from BoundingVolume (base class) and DirectX::BoundingOrientedBox.
struct OBB : public BoundingVolume, DirectX::BoundingOrientedBox 
{
	bool Intersects(BoundingVolume* other);
	void Transform(XMMATRIX transform);
	XMFLOAT3 GetCenter();
};
//Empty struct inheriting from BoundingVolume (base class) and DirectX::BoundingBox.
struct AABB : public BoundingVolume, DirectX::BoundingBox 
{
	bool Intersects(BoundingVolume* other);
	void Transform(XMMATRIX transform);
	XMFLOAT3 GetCenter();
};

//Axis aligned so that y is height, DO NOT ROTATE, specifically designed for tiles. 
struct Cylinder : public BoundingVolume
{
	bool Intersects(BoundingVolume* other);
	void Transform(XMMATRIX transform);
	XMFLOAT3 GetCenter();

	// Radius is center to corner
	void CreateFromTile(XMFLOAT3 position, float height, float radius = 1.0f);
private:
	XMFLOAT3 mCenter;
	float mHeight;
	float mRadius;
};