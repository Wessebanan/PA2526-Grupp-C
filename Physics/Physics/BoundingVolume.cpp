#include "includes\BoundingVolume.h"


bool Sphere::Intersects(BoundingVolume* other)
{
	// Check which bounding volume 'other' is and test.
	OBB* obb = dynamic_cast<OBB*>(other);
	if (obb)
	{
		return BoundingSphere::Intersects(*(BoundingOrientedBox*)obb);
	}
	AABB* aabb = dynamic_cast<AABB*>(other);
	if (aabb)
	{
		return BoundingSphere::Intersects(*(BoundingBox*)aabb);
	}
	Sphere* sphere = dynamic_cast<Sphere*>(other);
	if (sphere)
	{
		return BoundingSphere::Intersects(*(Sphere*)sphere);
	}
	return false;
}

void Sphere::Transform(XMMATRIX transform)
{
	BoundingSphere::Transform(*(BoundingSphere*)this, transform);
}

XMFLOAT3 Sphere::GetCenter()
{
	return Center;
}

bool OBB::Intersects(BoundingVolume* other)
{
	// Check which bounding volume 'other' is and test.
	OBB* obb = dynamic_cast<OBB*>(other);
	if (obb)
	{
		return BoundingOrientedBox::Intersects(*(BoundingOrientedBox*)obb);
	}
	AABB* aabb = dynamic_cast<AABB*>(other);
	if (aabb)
	{
		return BoundingOrientedBox::Intersects(*(BoundingBox*)aabb);
	}
	Sphere* sphere = dynamic_cast<Sphere*>(other);
	if (sphere)
	{
		return BoundingOrientedBox::Intersects(*(Sphere*)sphere);
	}
	return false;
}

void OBB::Transform(XMMATRIX transform)
{
	BoundingOrientedBox::Transform(*(BoundingOrientedBox*)this, transform);
}

XMFLOAT3 OBB::GetCenter()
{
	return Center;
}

bool AABB::Intersects(BoundingVolume* other)
{
	// Check which bounding volume 'other' is and test.
	OBB* obb = dynamic_cast<OBB*>(other);
	if (obb)
	{
		return BoundingBox::Intersects(*(BoundingOrientedBox*)obb);
	}
	AABB* aabb = dynamic_cast<AABB*>(other);
	if (aabb)
	{
		return BoundingBox::Intersects(*(BoundingBox*)aabb);
	}
	Sphere* sphere = dynamic_cast<Sphere*>(other);
	if (sphere)
	{
		return BoundingBox::Intersects(*(Sphere*)sphere);
	}
	return false;
}

void AABB::Transform(XMMATRIX transform)
{
	BoundingBox::Transform(*(BoundingBox*)this, transform);
}

XMFLOAT3 AABB::GetCenter()
{
	return Center;
}
