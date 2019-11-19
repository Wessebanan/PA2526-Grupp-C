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
				delete[] p_corners;
				return true;
			}
		}
	}
	delete[] p_corners;
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

CollisionInfo Sphere::GetCollisionInfo(BoundingVolume* pOther)
{
	// Check which bounding volume 'pOther' is and test.
	OBB* p_obb = dynamic_cast<OBB*>(pOther);
	if (p_obb)
	{
		return GetCollisionInfo(*(BoundingOrientedBox*)p_obb);
	}
	AABB* p_aabb = dynamic_cast<AABB*>(pOther);
	if (p_aabb)
	{
		return GetCollisionInfo(*(BoundingBox*)p_aabb);
	}
	Sphere* p_sphere = dynamic_cast<Sphere*>(pOther);
	if (p_sphere)
	{
		return GetCollisionInfo(*(Sphere*)p_sphere);
	}
	Cylinder* p_cylinder = dynamic_cast<Cylinder*>(pOther);
	if (p_cylinder)
	{
		return GetCollisionInfo(*(BoundingCylinder*)p_cylinder);
	}
	return CollisionInfo();
}

CollisionInfo Sphere::GetCollisionInfo(BoundingSphere& rSphere)
{
	return CollisionInfo();
}

CollisionInfo Sphere::GetCollisionInfo(BoundingBox& rAabb)
{
	return CollisionInfo();
}

CollisionInfo Sphere::GetCollisionInfo(BoundingOrientedBox& rObb)
{
	return CollisionInfo();
}

CollisionInfo Sphere::GetCollisionInfo(BoundingCylinder& rCylinder)
{
	return CollisionInfo();
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

CollisionInfo OBB::GetCollisionInfo(BoundingVolume* pOther)
{
	// Check which bounding volume 'pOther' is and test.
	OBB* p_obb = dynamic_cast<OBB*>(pOther);
	if (p_obb)
	{
		return GetCollisionInfo(*(BoundingOrientedBox*)p_obb);
	}
	AABB* p_aabb = dynamic_cast<AABB*>(pOther);
	if (p_aabb)
	{
		return GetCollisionInfo(*(BoundingBox*)p_aabb);
	}
	Sphere* p_sphere = dynamic_cast<Sphere*>(pOther);
	if (p_sphere)
	{
		return GetCollisionInfo(*(Sphere*)p_sphere);
	}
	Cylinder* p_cylinder = dynamic_cast<Cylinder*>(pOther);
	if (p_cylinder)
	{
		return GetCollisionInfo(*(BoundingCylinder*)p_cylinder);
	}
	return CollisionInfo();
}

CollisionInfo OBB::GetCollisionInfo(BoundingSphere& rSphere)
{
	XMFLOAT3* corners = new XMFLOAT3[CORNER_COUNT];
	GetCorners(corners);

	// Grab direction of corner closest to center of Sphere.
	float closest_distance = FLT_MAX;
	XMFLOAT3 closest_corner = XMFLOAT3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < CORNER_COUNT; i++)
	{
		float distance = CalculateDistance(rSphere.Center, corners[i]);
		if (distance < closest_distance)
		{
			closest_corner = corners[i];
			closest_distance = distance;
		}
	}
	delete[] corners;

	// Saving vector from center of aabb to closest corner of obb.
	XMVECTOR vec_center_to_closest = XMVectorSubtract(XMLoadFloat3(&closest_corner), XMLoadFloat3(&rSphere.Center));

	CollisionInfo return_info;

	// Taking normal as the normalized vector from the closest corner to sphere center.
	return_info.mNormal;
	XMStoreFloat3(&return_info.mNormal, XMVector3Normalize(vec_center_to_closest));

	// Projecting vec_center_to_closest on normal to find remainder to center
	// which is then subtracted from the radius to get the magnitude of the overlap
	// in the direction of the normal.
	XMVECTOR v_prime = XMVector3Dot(XMLoadFloat3(&return_info.mNormal), vec_center_to_closest);
	return_info.mOverlap = rSphere.Radius - fabsf(XMVectorGetX(v_prime));

	return return_info;
}

CollisionInfo OBB::GetCollisionInfo(BoundingBox& rAabb)
{
	XMFLOAT3* corners = new XMFLOAT3[CORNER_COUNT];
	GetCorners(corners);

	// Grab direction of corner closest to center of AABB.
	float closest_distance = FLT_MAX;
	XMFLOAT3 closest_corner = XMFLOAT3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < CORNER_COUNT; i++)
	{
		float distance = CalculateDistance(rAabb.Center, corners[i]);
		if (distance < closest_distance)
		{
			closest_corner = corners[i];
			closest_distance = distance;
		}
	}
	delete[] corners;

	// Saving vector from center of aabb to closest corner of obb.
	XMFLOAT3 center_to_closest;
	XMStoreFloat3(&center_to_closest, XMVectorSubtract(XMLoadFloat3(&closest_corner), XMLoadFloat3(&rAabb.Center)));

	// Saving the abs values of center_to_closest to find largest component.
	XMFLOAT3 abs_center_to_closest;
	XMStoreFloat3(&abs_center_to_closest, XMVectorAbs(XMLoadFloat3(&center_to_closest)));

	CollisionInfo return_info;

	// Determining the collision normal and overlap based on largest component of center_to_closest.
	if (abs_center_to_closest.x > abs_center_to_closest.y && abs_center_to_closest.x > abs_center_to_closest.z)
	{
		int sign = Sign(center_to_closest.x);
		return_info.mOverlap = rAabb.Extents.x - abs_center_to_closest.x;
		return_info.mNormal.x = sign;
	}
	else if (abs_center_to_closest.z > abs_center_to_closest.x && abs_center_to_closest.z > abs_center_to_closest.y)
	{
		int sign = Sign(center_to_closest.z);
		return_info.mOverlap = rAabb.Extents.z - abs_center_to_closest.z;
		return_info.mNormal.z = sign;
	}
	else
	{
		int sign = Sign(center_to_closest.y);
		return_info.mOverlap = rAabb.Extents.y - abs_center_to_closest.y;
		return_info.mNormal.y = sign;
	}

	return return_info;
}

CollisionInfo OBB::GetCollisionInfo(BoundingOrientedBox& rObb)
{
	// Grabbing copies of obbs to check.
	OBB object_obb_copy = OBB(*this);
	OBB obb_copy = OBB(*(OBB*)&rObb); // lmao

	// Taking the inverse of the object obb's orientation to transform it into axis aligned.
	XMMATRIX object_obb_rotation = XMMatrixRotationQuaternion(XMLoadFloat4(&this->Orientation));
	XMMATRIX object_obb_rotation_inverse = XMMatrixInverse(nullptr, object_obb_rotation);

	// Moving object obb to origin and placing bone obb in the same relative position.
	object_obb_copy.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&obb_copy.Center, XMVectorSubtract(XMLoadFloat3(&this->Center), XMLoadFloat3(&rObb.Center)));

	// Transforming both with the inverse orientation, making object_obb_copy axis aligned.
	object_obb_copy.Transform(object_obb_rotation_inverse);
	obb_copy.Transform(object_obb_rotation_inverse);

	// Using copies to get collision info as if the object had an AABB.
	AABB temp_aabb;
	temp_aabb.Center = object_obb_copy.Center;
	temp_aabb.Extents = object_obb_copy.Center;

	// Rotating normal back according to obb orientation for true orientation of normal.
	CollisionInfo return_info = GetCollisionInfo(temp_aabb);
	XMStoreFloat3(&return_info.mNormal, XMVector3TransformNormal(XMLoadFloat3(&return_info.mNormal), object_obb_rotation));
	return return_info;
}

CollisionInfo OBB::GetCollisionInfo(BoundingCylinder& rCylinder)
{
	XMFLOAT3* p_corners = new XMFLOAT3[CORNER_COUNT];
	GetCorners(p_corners);
	float closest_distance = FLT_MAX;
	XMFLOAT3 closest_corner = XMFLOAT3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < CORNER_COUNT; i++)
	{
		float distance = CalculateDistance(rCylinder.mCenter, p_corners[i]);
		if (distance < closest_distance)
		{
			closest_corner = p_corners[i];
			closest_distance = distance;
		}
	}
	delete[] p_corners;

	// Saving vector from center of cylinder to closest corner of obb.
	XMFLOAT3 center_to_closest;
	XMStoreFloat3(&center_to_closest, XMVectorSubtract(XMLoadFloat3(&closest_corner), XMLoadFloat3(&rCylinder.mCenter)));

	CollisionInfo return_info;

	// Normal will be y if y is the largest component of center_to_closest,
	// and overlap will be the difference of the half-height and the y component
	// of center_to_closest.
	if (center_to_closest.y > center_to_closest.x && center_to_closest.y > center_to_closest.z)
	{
		return_info.mNormal.y = 1.0f;
		return_info.mOverlap = rCylinder.mExtentsY - center_to_closest.y;
	}
	// Otherwise somewhere around the circle (center_to_closest in x-z only).
	else
	{
		XMFLOAT3 x_z_center_to_closest = center_to_closest;
		x_z_center_to_closest.y = 0.0f;
		XMVECTOR normalized = XMVector3Normalize(XMLoadFloat3(&x_z_center_to_closest));
		XMStoreFloat3(&return_info.mNormal, normalized);
		float length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&x_z_center_to_closest)));
		return_info.mOverlap = rCylinder.mRadius - length;
	}
	return return_info;
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

CollisionInfo AABB::GetCollisionInfo(BoundingVolume* pOther)
{
	// Check which bounding volume 'pOther' is and test.
	OBB* p_obb = dynamic_cast<OBB*>(pOther);
	if (p_obb)
	{
		return GetCollisionInfo(*(BoundingOrientedBox*)p_obb);
	}
	AABB* p_aabb = dynamic_cast<AABB*>(pOther);
	if (p_aabb)
	{
		return GetCollisionInfo(*(BoundingBox*)p_aabb);
	}
	Sphere* p_sphere = dynamic_cast<Sphere*>(pOther);
	if (p_sphere)
	{
		return GetCollisionInfo(*(Sphere*)p_sphere);
	}
	Cylinder* p_cylinder = dynamic_cast<Cylinder*>(pOther);
	if (p_cylinder)
	{
		return GetCollisionInfo(*(BoundingCylinder*)p_cylinder);
	}
	return CollisionInfo();
}

CollisionInfo AABB::GetCollisionInfo(BoundingSphere& rSphere)
{
	return CollisionInfo();
}

CollisionInfo AABB::GetCollisionInfo(BoundingBox& rAabb)
{
	return CollisionInfo();
}

CollisionInfo AABB::GetCollisionInfo(BoundingOrientedBox& rObb)
{
	return CollisionInfo();
}

CollisionInfo AABB::GetCollisionInfo(BoundingCylinder& rCylinder)
{
	return CollisionInfo();
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

CollisionInfo Cylinder::GetCollisionInfo(BoundingVolume* pOther)
{
	// Check which bounding volume 'pOther' is and test.
	OBB* p_obb = dynamic_cast<OBB*>(pOther);
	if (p_obb)
	{
		return GetCollisionInfo(*(BoundingOrientedBox*)p_obb);
	}
	AABB* p_aabb = dynamic_cast<AABB*>(pOther);
	if (p_aabb)
	{
		return GetCollisionInfo(*(BoundingBox*)p_aabb);
	}
	Sphere* p_sphere = dynamic_cast<Sphere*>(pOther);
	if (p_sphere)
	{
		return GetCollisionInfo(*(Sphere*)p_sphere);
	}
	Cylinder* p_cylinder = dynamic_cast<Cylinder*>(pOther);
	if (p_cylinder)
	{
		return GetCollisionInfo(*(BoundingCylinder*)p_cylinder);
	}
	return CollisionInfo();
}

CollisionInfo Cylinder::GetCollisionInfo(BoundingSphere& rSphere)
{
	return CollisionInfo();
}

CollisionInfo Cylinder::GetCollisionInfo(BoundingBox& rAabb)
{
	return CollisionInfo();
}

CollisionInfo Cylinder::GetCollisionInfo(BoundingOrientedBox& rObb)
{
	return CollisionInfo();
}

CollisionInfo Cylinder::GetCollisionInfo(BoundingCylinder& rCylinder)
{
	return CollisionInfo();
}

