//#pragma once
//#include <DirectXMath.h>
//#include "CollisionFunctions.h"
//
//using namespace DirectX;
//using namespace PhysicsHelpers;
//
//class BoundingVolume
//{
//public:
//	BoundingVolume();
//	virtual ~BoundingVolume();
//
//	virtual void Generate(const XMFLOAT3* points, const unsigned int size) = 0;
//
//	// Calls private intersection functions based on which
//	// type volume has.
//	virtual bool Intersect(const BoundingVolume *volume) = 0;
//	virtual void WorldTransform(const XMMATRIX& world) = 0;
//private:
//	virtual bool IntersectSphere(const BoundingVolume *volume) = 0;
//	virtual bool IntersectOBB	(const BoundingVolume *volume) = 0;
//	virtual bool IntersectAABB	(const BoundingVolume *volume) = 0;
//};
#pragma once
#include <DirectXCollision.h>
// Base struct for unspecified DirectX bounding volumes.
using namespace DirectX;
struct BoundingVolume
{
	virtual ~BoundingVolume() = default;
	virtual bool Intersects(BoundingVolume* other) = 0;
	virtual void Transform(XMMATRIX transform) = 0;
	virtual XMFLOAT3 GetCenter() = 0;
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