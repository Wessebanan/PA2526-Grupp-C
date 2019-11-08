#include "Collision.h"

Collision::Collision()
{
	mNumObjects = 0;
	mObjects = nullptr;
}

Collision::~Collision()
{
	Free();
}

void Collision::Free()
{
	delete mObjects;
	mObjects = nullptr;
	mNumObjects = 0;
}

void Collision::AddBoundingVolume(BoundingVolume* bv)
{
	AABB* aabb = dynamic_cast<AABB*>(bv);
	if (aabb)
	{
		CollisionObject *object = new CollisionObject();
		object->mType = COLLISION_AABB;
		object->mBoundingVolume = aabb;
		object->mNext = mObjects;
		mObjects = object;
		mNumObjects++;
	}
	OBB* obb = dynamic_cast<OBB*>(bv);
	if (obb)
	{
		CollisionObject* object = new CollisionObject();
		object->mType = COLLISION_OBB;
		object->mBoundingVolume = obb;
		object->mNext = mObjects;
		mObjects = object;
		mNumObjects++;
	}
	Sphere* sphere = dynamic_cast<Sphere*>(bv);
	if (sphere)
	{
		CollisionObject* object = new CollisionObject();
		object->mType = COLLISION_SPHERE;
		object->mBoundingVolume = sphere;
		object->mNext = mObjects;
		mObjects = object;
		mNumObjects++;
	}
	Cylinder* cylinder = dynamic_cast<Cylinder*>(bv);
	if (cylinder)
	{
		CollisionObject* object = new CollisionObject();
		object->mType = COLLISION_CYLINDER;
		object->mBoundingVolume = cylinder;
		object->mNext = mObjects;
		mObjects = object;
		mNumObjects++;
	}
}
