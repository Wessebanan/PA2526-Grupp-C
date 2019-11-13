#include "includes\BoundingVolume.h"

void BoundingCylinder::CreateFromTile(XMFLOAT3 position, float height, float radius)
{
	mExtentsY = height / 2.0f;
	mRadius = radius * HALF_SQRT_3;
	mCenter = XMFLOAT3(position.x, position.y - (height / 2.0f), position.z);
}

bool BoundingCylinder::Intersects(BoundingBox& aabb)
{
	// Immediately return if aabb is above or below cylinder.
	if (aabb.Center.y - aabb.Extents.y > mCenter.y + mExtentsY ||
		aabb.Center.y + aabb.Extents.y < mCenter.y - mExtentsY)
	{
		return false;
	}
	
	// Find closest x and z to cylinder center.
	float x = (std::max)(aabb.Center.x - aabb.Extents.x, (std::min)(mCenter.x, aabb.Center.x + aabb.Extents.x));
	float z = (std::max)(aabb.Center.z - aabb.Extents.z, (std::min)(mCenter.z, aabb.Center.z + aabb.Extents.z));

	XMFLOAT3 x_z_to_center = XMFLOAT3(x - mCenter.x, 0.0f, z - mCenter.z);
	float x_z_dist_to_center = PhysicsHelpers::CalculateVectorLength(x_z_to_center);

	// True if inside cylinder circle.
	return x_z_dist_to_center < mRadius;
}

bool BoundingCylinder::Intersects(BoundingSphere& sphere)
{
	// Immediately return if sphere is above or below cylinder.
	if (sphere.Center.y - sphere.Radius > mCenter.y + mExtentsY ||
		sphere.Center.y + sphere.Radius < mCenter.y - mExtentsY)
	{
		return false;
	}

	// Circle-circle intersection.
	XMFLOAT3 x_z_cylinder_center = XMFLOAT3(mCenter.x, 0.0f, mCenter.z);
	XMFLOAT3 x_z_sphere_center = XMFLOAT3(sphere.Center.x, 0.0f, sphere.Center.z);
	float x_z_center_to_center_dist = PhysicsHelpers::CalculateDistance(x_z_cylinder_center, x_z_sphere_center);

	return x_z_center_to_center_dist < sphere.Radius + mRadius;
}

bool BoundingCylinder::Intersects(BoundingOrientedBox& obb)
{
	// Only corner intersection, edges are hard af.
	XMFLOAT3* corners = new XMFLOAT3[obb.CORNER_COUNT];
	obb.GetCorners(corners);
	bool y_check = false;
	for (int i = 0; i < obb.CORNER_COUNT; i++)
	{
		XMFLOAT3 corner = corners[i];
		// If a corner is within y range, check x-z.
		if (corner.y < mCenter.y - mExtentsY ||
			corner.y > mCenter.y + mExtentsY)
		{
			XMFLOAT3 x_z_cylinder_center = XMFLOAT3(mCenter.x, 0.0f, mCenter.z);
			XMFLOAT3 x_z_corner = XMFLOAT3(corner.x, 0.0f, corner.z);
			float x_z_center_to_corner_dist = PhysicsHelpers::CalculateDistance(x_z_cylinder_center, x_z_corner);

			// If distance between corner x-z and sphere center is less than radius, intersection.
			if (x_z_center_to_corner_dist < mRadius)
			{
				return true;
			}
		}
	}
	return false;
}

bool BoundingCylinder::Intersects(BoundingCylinder& cylinder)
{
	// Return if above or below.
	if (cylinder.mCenter.y - cylinder.mExtentsY > mCenter.y + mExtentsY ||
		cylinder.mCenter.y + cylinder.mExtentsY < mCenter.y - mExtentsY)
	{
		return false;
	}

	// Circle-circle intersection.
	XMFLOAT3 x_z_this_cylinder_center = XMFLOAT3(mCenter.x, 0.0f, mCenter.z);
	XMFLOAT3 x_z_cylinder_center = XMFLOAT3(cylinder.mCenter.x, 0.0f, cylinder.mCenter.z);
	float x_z_center_to_center_dist = PhysicsHelpers::CalculateDistance(x_z_this_cylinder_center, x_z_cylinder_center);

	return x_z_center_to_center_dist < cylinder.mRadius + mRadius;
}

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
	Cylinder* cylinder = dynamic_cast<Cylinder*>(other);
	if (cylinder)
	{
		return cylinder->BoundingCylinder::Intersects(*(BoundingSphere*)this);
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
	Cylinder* cylinder = dynamic_cast<Cylinder*>(other);
	if (cylinder)
	{
		return cylinder->BoundingCylinder::Intersects(*(BoundingOrientedBox*)this);
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
	Cylinder* cylinder = dynamic_cast<Cylinder*>(other);
	if (cylinder)
	{
		return cylinder->BoundingCylinder::Intersects(*(BoundingBox*)this);
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
	// Check which bounding volume 'other' is and test.
	OBB* obb = dynamic_cast<OBB*>(other);
	if (obb)
	{
		return BoundingCylinder::Intersects(*(BoundingOrientedBox*)obb);
	}
	AABB* aabb = dynamic_cast<AABB*>(other);
	if (aabb)
	{
		return BoundingCylinder::Intersects(*(BoundingBox*)aabb);
	}
	Sphere* sphere = dynamic_cast<Sphere*>(other);
	if (sphere)
	{
		return BoundingCylinder::Intersects(*(Sphere*)sphere);
	}
	Cylinder* cylinder = dynamic_cast<Cylinder*>(other);
	if (cylinder)
	{
		return BoundingCylinder::Intersects(*cylinder);
	}
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

