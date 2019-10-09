#pragma once
#include "BoundingVolume.h"
#include "BoundingSphere.h"
#include "AABB.h"

// Not finished and likely will not be,
// but I'm not deleting until I'm sure.
class OBB : public BoundingVolume
{
public:
	OBB();
	~OBB();

	// OBB is generated as AABB, but saves vertices rather than min/max.
	// WAAAAAY too complicated to find the perfect OBB.
	// Fills the vertices array according to:
	/*
	   6-------7
	  /|      /|
	 / |     / |
	2--|----3  |
	|  4----|--5
	| /     | /
	0-------1		|: y, -: x /: z
	*/
	void Generate(XMFLOAT3* points, const unsigned int size);

	bool Intersect(const BoundingVolume* volume);

	const XMFLOAT3* GetVertices() const;

private:
	bool IntersectSphere(const BoundingVolume *volume);
	bool IntersectOBB(const BoundingVolume* volume);
	bool IntersectAABB(const BoundingVolume * volume);

	XMFLOAT3 *mpVertices;
};
