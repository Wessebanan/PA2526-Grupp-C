#pragma once
#include "CollisionObject.h"

class Collision
{
public:
	DWORD mNumObjects;
	CollisionObject* mObjects;

	Collision();
	~Collision();
	void Free();

	// Add bounding volume assigns the pointer directly to the new collision object.
	// This should be fine as the bounding volume passed to this function should already 
	// be a copy transformed to world space.
	void AddBoundingVolume(BoundingVolume* bv);
	
private:
};