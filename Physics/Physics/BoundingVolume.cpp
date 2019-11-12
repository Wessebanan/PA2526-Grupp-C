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

bool Cylinder::Intersects(BoundingVolume* other)
{
	return false;
}

void Cylinder::Transform(XMMATRIX transform)
{ 
	// Only does translation because only tiles use this.
	XMStoreFloat3(&mCenter, XMVector3Transform(XMLoadFloat3(&mCenter), transform));
	
}

XMFLOAT3 Cylinder::GetCenter()
{
	return mCenter;
}

void Cylinder::CreateFromTile(XMFLOAT3 position, float height, float radius)
{
	mHeight = height;
	mRadius = radius * HALF_SQRT_3;
	mCenter = XMFLOAT3(position.x, position.y - (height / 2.0f), position.z);
}
