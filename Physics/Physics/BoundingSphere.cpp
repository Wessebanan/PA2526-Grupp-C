#include "BoundingSphere.h"

BoundingSphere::BoundingSphere()
{
	mCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mRadius = 0.0f;
}

BoundingSphere::~BoundingSphere()
{
}

void BoundingSphere::Generate(const XMFLOAT3* points, const unsigned int size)
{
	XMFLOAT3 min, max;
	GetExtremes(points, size, min, max);
	// min + max / 2 gives us the center.
	mCenter = DirectX::XMFLOAT3
	(
		(min.x + max.x) / 2.0f,
		(min.y + max.y) / 2.0f,
		(min.z + max.z) / 2.0f
	);

	// Find the biggest distance to a point in the group
	// to get the radius of the smallest enclosing sphere.
	float biggest_distance = 0.0f;
	for (int i = 0; i < size; i++)
	{
		float distance = PhysicsHelpers::CalculateDistance(mCenter, points[i]);
		if (biggest_distance < distance)
		{
			biggest_distance = distance;
		}
	}
	mRadius = biggest_distance;
}

bool BoundingSphere::Intersect(const BoundingVolume* volume)
{
	const OBB* obb = dynamic_cast<const OBB*>(volume);
	if (obb)
	{
		return IntersectOBB(volume);
	}
	const AABB* aabb = dynamic_cast<const AABB*>(volume);
	if (aabb)
	{
		return IntersectAABB(volume);
	}
	const BoundingSphere* sphere = dynamic_cast<const BoundingSphere*>(volume);
	if (sphere)
	{
		return IntersectSphere(volume);
	}
}

void BoundingSphere::WorldTransform(const XMMATRIX& world)
{
	XMVECTOR center = XMLoadFloat3(&mCenter);
	center = XMVector3Transform(center, world);
	XMStoreFloat3(&mCenter, center);
	
}

bool BoundingSphere::IntersectSphere(const BoundingVolume* volume)
{
	// If the distance between the center points is greater than the sum of the radii, 
	// the spheres intersect, with the difference being the overlap in the direction of
	// the vector between the centers.
	const BoundingSphere* sphere = dynamic_cast<const BoundingSphere*>(volume);
	return CalculateDistance(mCenter, *sphere->GetCenter()) < (mRadius + *sphere->GetRadius());
}

bool BoundingSphere::IntersectOBB(const BoundingVolume* volume)
{
	return false;
}

bool BoundingSphere::IntersectAABB(const BoundingVolume* volume)
{
	const AABB* aabb = dynamic_cast<const AABB*>(volume);

	const XMFLOAT3 box_min = *aabb->GetMin();
	const XMFLOAT3 box_max = *aabb->GetMax();

	float dist_sq = Square(mRadius);

	// Solution from stackoverflow, don't question it.
	if (mCenter.x < box_min.x)
	{
		dist_sq -= Square(mCenter.x - box_min.x);
	}
	else if (mCenter.x > box_max.x)
	{
		dist_sq -= Square(mCenter.x - box_max.x);
	}
	if (mCenter.y < box_min.y)
	{
		dist_sq -= Square(mCenter.y - box_min.y);
	}
	else if (mCenter.y > box_max.y)
	{
		dist_sq -= Square(mCenter.y - box_max.y);
	}
	if (mCenter.z < box_min.z)
	{
		dist_sq -= Square(mCenter.z - box_min.z);
	}
	else if (mCenter.z > box_max.z)
	{
		dist_sq -= Square(mCenter.z - box_max.z);
	}
	return dist_sq > 0.0f;
}

const XMFLOAT3* BoundingSphere::GetCenter() const
{
	return &mCenter;
}

const float* BoundingSphere::GetRadius() const
{
	return &mRadius;
}