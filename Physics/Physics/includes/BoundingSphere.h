#pragma once
#include "BoundingVolume.h"
#include "OBB.h"
#include "AABB.h"

class BoundingSphere : public BoundingVolume
{
public:
	BoundingSphere();
	~BoundingSphere();
	void Generate(const XMFLOAT3* points, const unsigned int size);

	bool Intersect(const BoundingVolume* volume);
	void WorldTransform(const XMMATRIX& world);

	const XMFLOAT3* GetCenter() const;
	const float* GetRadius() const;

private:
	bool IntersectSphere(const BoundingVolume* volume);
	bool IntersectOBB(const BoundingVolume* volume);
	bool IntersectAABB(const BoundingVolume* volume);

	XMFLOAT3 mCenter;
	float mRadius;
};