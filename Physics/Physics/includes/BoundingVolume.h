#pragma once
#include <DirectXMath.h>
#include "CollisionFunctions.h"

using namespace DirectX;
using namespace PhysicsHelpers;

class BoundingVolume
{
public:
	BoundingVolume();
	virtual ~BoundingVolume();

	virtual void Generate(const XMFLOAT3* points, const unsigned int size) = 0;

	// Calls private intersection functions based on which
	// type volume has.
	virtual bool Intersect(const BoundingVolume *volume) = 0;

private:
	virtual bool IntersectSphere(const BoundingVolume *volume) = 0;
	virtual bool IntersectOBB	(const BoundingVolume *volume) = 0;
	virtual bool IntersectAABB	(const BoundingVolume *volume) = 0;
};