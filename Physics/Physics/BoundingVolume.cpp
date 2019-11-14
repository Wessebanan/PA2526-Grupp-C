#include "includes\BoundingVolume.h"

void BoundingCylinder::CreateFromTile(XMFLOAT3 position, float height, float radius)
{
	mExtentsY = height / 2.0f;
	mRadius = radius * HALF_SQRT_3;
	mCenter = XMFLOAT3(position.x, position.y - (height / 2.0f), position.z);
}

bool BoundingCylinder::Intersects(BoundingBox& rAabb)
{
	// Immediately return if rAabb is above or below cylinder.
	if (rAabb.Center.y - rAabb.Extents.y >= mCenter.y + mExtentsY ||
		rAabb.Center.y + rAabb.Extents.y <= mCenter.y - mExtentsY)
	{
		return false;
	}
	
	// Find closest x and z to cylinder center.
	float x = (std::max)(rAabb.Center.x - rAabb.Extents.x, (std::min)(mCenter.x, rAabb.Center.x + rAabb.Extents.x));
	float z = (std::max)(rAabb.Center.z - rAabb.Extents.z, (std::min)(mCenter.z, rAabb.Center.z + rAabb.Extents.z));

	XMFLOAT3 x_z_to_center = XMFLOAT3(x - mCenter.x, 0.0f, z - mCenter.z);
	float x_z_dist_to_center = PhysicsHelpers::CalculateVectorLength(x_z_to_center);

	// True if inside cylinder circle.
	return x_z_dist_to_center <= mRadius;
}

bool BoundingCylinder::Intersects(BoundingSphere& rSphere)
{
	// Immediately return if rSphere is above or below cylinder.
	if (rSphere.Center.y - rSphere.Radius >= mCenter.y + mExtentsY ||
		rSphere.Center.y + rSphere.Radius <= mCenter.y - mExtentsY)
	{
		return false;
	}

	// Circle-circle intersection.
	XMFLOAT3 x_z_cylinder_center = XMFLOAT3(mCenter.x, 0.0f, mCenter.z);
	XMFLOAT3 x_z_rSphere_center = XMFLOAT3(rSphere.Center.x, 0.0f, rSphere.Center.z);
	float x_z_center_to_center_dist = PhysicsHelpers::CalculateDistance(x_z_cylinder_center, x_z_rSphere_center);

	return x_z_center_to_center_dist <= rSphere.Radius + mRadius;
}

bool BoundingCylinder::Intersects(BoundingOrientedBox& rObb)
{
	// Only corner intersection, edges are hard af.
	XMFLOAT3* p_corners = new XMFLOAT3[rObb.CORNER_COUNT];
	rObb.GetCorners(p_corners);
	bool y_check = false;
	for (int i = 0; i < rObb.CORNER_COUNT; i++)
	{
		XMFLOAT3 corner = p_corners[i];
		// If a corner is within y range, check x-z.
		if (corner.y >= mCenter.y - mExtentsY &&
			corner.y <= mCenter.y + mExtentsY)
		{
			XMFLOAT3 x_z_cylinder_center = XMFLOAT3(mCenter.x, 0.0f, mCenter.z);
			XMFLOAT3 x_z_corner = XMFLOAT3(corner.x, 0.0f, corner.z);
			float x_z_center_to_corner_dist = PhysicsHelpers::CalculateDistance(x_z_cylinder_center, x_z_corner);

			// If distance between corner x-z and sphere center is less than radius, intersection.
			if (x_z_center_to_corner_dist <= mRadius)
			{
				delete p_corners;
				return true;
			}
		}
	}
	delete p_corners;
	return false;
}

bool BoundingCylinder::Intersects(BoundingCylinder& rCylinder)
{
	// Return if above or below.
	if (rCylinder.mCenter.y - rCylinder.mExtentsY >= mCenter.y + mExtentsY ||
		rCylinder.mCenter.y + rCylinder.mExtentsY <= mCenter.y - mExtentsY)
	{
		return false;
	}

	// Circle-circle intersection.
	XMFLOAT3 x_z_this_rCylinder_center = XMFLOAT3(mCenter.x, 0.0f, mCenter.z);
	XMFLOAT3 x_z_rCylinder_center = XMFLOAT3(rCylinder.mCenter.x, 0.0f, rCylinder.mCenter.z);
	float x_z_center_to_center_dist = PhysicsHelpers::CalculateDistance(x_z_this_rCylinder_center, x_z_rCylinder_center);

	return x_z_center_to_center_dist <= rCylinder.mRadius + mRadius;
}

bool Sphere::Intersects(BoundingVolume* pOther)
{
	// Check which bounding volume 'pOther' is and test.
	OBB* p_obb = dynamic_cast<OBB*>(pOther);
	if (p_obb)
	{
		return BoundingSphere::Intersects(*(BoundingOrientedBox*)p_obb);
	}
	AABB* p_aabb = dynamic_cast<AABB*>(pOther);
	if (p_aabb)
	{
		return BoundingSphere::Intersects(*(BoundingBox*)p_aabb);
	}
	Sphere* p_sphere = dynamic_cast<Sphere*>(pOther);
	if (p_sphere)
	{
		return BoundingSphere::Intersects(*(Sphere*)p_sphere);
	}
	Cylinder* p_cylinder = dynamic_cast<Cylinder*>(pOther);
	if (p_cylinder)
	{
		return p_cylinder->BoundingCylinder::Intersects(*(BoundingSphere*)this);
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

bool OBB::Intersects(BoundingVolume* pOther)
{
	// Check which bounding volume 'pOther' is and test.
	OBB* p_obb = dynamic_cast<OBB*>(pOther);
	if (p_obb)
	{
		return BoundingOrientedBox::Intersects(*(BoundingOrientedBox*)p_obb);
	}
	AABB* p_aabb = dynamic_cast<AABB*>(pOther);
	if (p_aabb)
	{
		return BoundingOrientedBox::Intersects(*(BoundingBox*)p_aabb);
	}
	Sphere* p_sphere = dynamic_cast<Sphere*>(pOther);
	if (p_sphere)
	{
		return BoundingOrientedBox::Intersects(*(Sphere*)p_sphere);
	}
	Cylinder* p_cylinder = dynamic_cast<Cylinder*>(pOther);
	if (p_cylinder)
	{
		return p_cylinder->BoundingCylinder::Intersects(*(BoundingOrientedBox*)this);
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

bool AABB::Intersects(BoundingVolume* pOther)
{
	// Check which bounding volume 'pOther' is and test.
	OBB* p_obb = dynamic_cast<OBB*>(pOther);
	if (p_obb)
	{
		return BoundingBox::Intersects(*(BoundingOrientedBox*)p_obb);
	}
	AABB* p_aabb = dynamic_cast<AABB*>(pOther);
	if (p_aabb)
	{
		return BoundingBox::Intersects(*(BoundingBox*)p_aabb);
	}
	Sphere* p_sphere = dynamic_cast<Sphere*>(pOther);
	if (p_sphere)
	{
		return BoundingBox::Intersects(*(Sphere*)p_sphere);
	}
	Cylinder* p_cylinder = dynamic_cast<Cylinder*>(pOther);
	if (p_cylinder)
	{
		return p_cylinder->BoundingCylinder::Intersects(*(BoundingBox*)this);
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

bool Cylinder::Intersects(BoundingVolume* pOther)
{
	// Check which bounding volume 'pOther' is and test.
	OBB* p_obb = dynamic_cast<OBB*>(pOther);
	if (p_obb)
	{
		return BoundingCylinder::Intersects(*(BoundingOrientedBox*)p_obb);
	}
	AABB* p_aabb = dynamic_cast<AABB*>(pOther);
	if (p_aabb)
	{
		return BoundingCylinder::Intersects(*(BoundingBox*)p_aabb);
	}
	Sphere* p_sphere = dynamic_cast<Sphere*>(pOther);
	if (p_sphere)
	{
		return BoundingCylinder::Intersects(*(Sphere*)p_sphere);
	}
	Cylinder* p_cylinder = dynamic_cast<Cylinder*>(pOther);
	if (p_cylinder)
	{
		return BoundingCylinder::Intersects(*p_cylinder);
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

