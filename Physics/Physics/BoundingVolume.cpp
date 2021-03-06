#include "includes\BoundingVolume.h"

void BoundingCylinder::CreateFromPoints(BoundingCylinder& Out, size_t Count, const XMFLOAT3* pPoints, size_t Stride)
{
	// Creating min and max points to make a box.
	XMFLOAT3 min = XMFLOAT3(INFINITY, INFINITY, INFINITY);
	XMFLOAT3 max = XMFLOAT3(-INFINITY, -INFINITY, -INFINITY);

	// Looping over each vertex to see if they contain
	// the lowest or highest value in each axis.
	for (int i = 0; i < Count; i++)
	{
		XMFLOAT3 current = pPoints[i];
		if (current.x < min.x)
		{
			min.x = current.x;
		}
		else if (current.x > max.x)
		{
			max.x = current.x;
		}
		if (current.y < min.y)
		{
			min.y = current.y;
		}
		else if (current.y > max.y)
		{
			max.y = current.y;
		}
		if (current.z < min.z)
		{
			min.z = current.z;
		}
		else if (current.z > max.z)
		{
			max.z = current.z;
		}
	}
	
	// Calculating the height and radius of the cylinder.
	float height = max.y - min.y;
	float radius = 0.0f;

	float extents_x = max.x - min.x;
	float extents_z = max.z - min.z;

	if (extents_x > extents_z)
	{
		radius = extents_x / 2.0f;
	}
	else
	{
		radius = extents_z / 2.0f;
	}
	
	mExtentsY = height / 2.0f;
	mRadius = radius * HALF_SQRT_3;
	mCenter = XMFLOAT3(0.0f, -(height / 2.0f), 0.0f);
}

void BoundingCylinder::CreateFromTile(XMFLOAT3 position, float radius)
{
	mExtentsY = 1.0f;
	mRadius = radius * 1.0f;
	mCenter = XMFLOAT3(0.0f,-mExtentsY,0.0f);
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

XMFLOAT3 Sphere::GetExtents()
{
	return XMFLOAT3(Radius, Radius, Radius);
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
	CollisionInfo return_info;

	// Normal will be the vector between this center and the other normalized.
	XMVECTOR this_center_to_other_center = XMVectorSubtract(XMLoadFloat3(&this->Center), XMLoadFloat3(&rSphere.Center));
	XMStoreFloat3(&return_info.mNormal, XMVector3Normalize(this_center_to_other_center));

	// Overlap will be the difference between the sum of the radii and the length of previous vector.
	float length = XMVectorGetX(XMVector3Length(this_center_to_other_center));
	float radii_sum = this->Radius + rSphere.Radius;
	return_info.mOverlap = radii_sum - length;

	// Spheres are so fn easy omg.
	return return_info;
}

CollisionInfo Sphere::GetCollisionInfo(BoundingBox& rAabb)
{
	// Calculate closest point of aabb to sphere center (sunglasses emoji).
	XMFLOAT3 closest_point = XMFLOAT3(
		(std::max)(rAabb.Center.x - rAabb.Extents.x, (std::min)(Center.x, rAabb.Center.x + rAabb.Extents.x)),
		(std::max)(rAabb.Center.y - rAabb.Extents.y, (std::min)(Center.y, rAabb.Center.y + rAabb.Extents.y)),
		(std::max)(rAabb.Center.z - rAabb.Extents.z, (std::min)(Center.z, rAabb.Center.z + rAabb.Extents.z))
	);

	// Get a vector from closest point to sphere center.
	XMFLOAT3 center_to_closest;
	XMStoreFloat3(&center_to_closest, XMVectorSubtract(XMLoadFloat3(&Center), XMLoadFloat3(&closest_point)));

	// Saving the abs values of center_to_closest to find largest component.
	XMFLOAT3 abs_center_to_closest;
	XMStoreFloat3(&abs_center_to_closest, XMVectorAbs(XMLoadFloat3(&center_to_closest)));

	CollisionInfo return_info;

	// Determining the collision normal and overlap based on largest component of center_to_closest.
	if (abs_center_to_closest.x > abs_center_to_closest.y && abs_center_to_closest.x > abs_center_to_closest.z)
	{
		return_info.mOverlap = this->Radius - abs_center_to_closest.x;
		return_info.mNormal.x = Sign(center_to_closest.x);
	}
	else if (abs_center_to_closest.z > abs_center_to_closest.x && abs_center_to_closest.z > abs_center_to_closest.y)
	{
		return_info.mOverlap = this->Radius - abs_center_to_closest.z;
		return_info.mNormal.z = Sign(center_to_closest.z);
	}
	else
	{
		return_info.mOverlap = this->Radius - abs_center_to_closest.y;
		return_info.mNormal.y = Sign(center_to_closest.y);
	}	

	return return_info;
}

CollisionInfo Sphere::GetCollisionInfo(BoundingOrientedBox& rObb)
{
	// Grabbing a copy of the obb and sphere.
	OBB obb_copy = OBB(*(OBB*)& rObb);
	Sphere sphere_copy = Sphere(*this);

	// Moving obb to origin and sphere the same relative distance.
	obb_copy.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&sphere_copy.Center, XMVectorSubtract(XMLoadFloat3(&sphere_copy.Center), XMLoadFloat3(&rObb.Center)));

	// Rotating obb by inverse of current rotation and sphere the same to make obb axis aligned.
	XMMATRIX obb_rotation = XMMatrixRotationQuaternion(XMLoadFloat4(&rObb.Orientation));
	XMMATRIX obb_rotation_inverse = XMMatrixInverse(nullptr, obb_rotation);

	obb_copy.Transform(obb_rotation_inverse);
	sphere_copy.Transform(obb_rotation_inverse);

	AABB temp_aabb;
	temp_aabb.Center = obb_copy.Center;
	temp_aabb.Extents = obb_copy.Extents;

	// Grabbing collision info for the temporary aabb (obb rotated back to axis aligned).
	CollisionInfo return_info = GetCollisionInfo(temp_aabb);

	// Rotating normal back into orientation of the obb.
	XMStoreFloat3(&return_info.mNormal, XMVector3TransformNormal(XMLoadFloat3(&return_info.mNormal), obb_rotation));

	return return_info;
}

CollisionInfo Sphere::GetCollisionInfo(BoundingCylinder& rCylinder)
{
	// Grabbing vector from center of cylinder to center of sphere.
	XMFLOAT3 cyl_center_to_sphere_center;
	XMStoreFloat3(&cyl_center_to_sphere_center, XMVectorSubtract(XMLoadFloat3(&this->Center), XMLoadFloat3(&rCylinder.mCenter)));
	
	// Saving abs value of vector components for size comparison.
	XMFLOAT3 abs_cyl_to_sphere;
	XMStoreFloat3(&abs_cyl_to_sphere, XMVectorAbs(XMLoadFloat3(&cyl_center_to_sphere_center)));

	CollisionInfo return_info;

	// If y is the largest component, collision happens on top or bottom.
	if (abs_cyl_to_sphere.y > abs_cyl_to_sphere.z && abs_cyl_to_sphere.y > abs_cyl_to_sphere.x)
	{
		return_info.mNormal.y = Sign(cyl_center_to_sphere_center.y);
		return_info.mOverlap = rCylinder.mExtentsY + this->Radius - abs_cyl_to_sphere.y;
	}
	// Otherwise around the circle.
	else
	{
		XMFLOAT3 x_z_center_to_center = cyl_center_to_sphere_center;
		x_z_center_to_center.y = 0.0f;
		XMVECTOR normalized = XMVector3Normalize(XMLoadFloat3(&x_z_center_to_center));
		XMStoreFloat3(&return_info.mNormal, normalized);
		float length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&x_z_center_to_center)));
		return_info.mOverlap = rCylinder.mRadius + this->Radius - length;
	}
	
	return return_info;
}

BoundingVolume* Sphere::Copy()
{
	return new Sphere(*this);
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
		return_info.mOverlap = rAabb.Extents.x - abs_center_to_closest.x;
		return_info.mNormal.x = Sign(center_to_closest.x);
	}
	else if (abs_center_to_closest.z > abs_center_to_closest.x && abs_center_to_closest.z > abs_center_to_closest.y)
	{
		return_info.mOverlap = rAabb.Extents.z - abs_center_to_closest.z;
		return_info.mNormal.z = Sign(center_to_closest.z);
	}
	else
	{
		return_info.mOverlap = rAabb.Extents.y - abs_center_to_closest.y;
		return_info.mNormal.y = Sign(center_to_closest.y);
	}

	return return_info;
}

CollisionInfo OBB::GetCollisionInfo(BoundingOrientedBox& rObb)
{
	// Grabbing copies of obbs to check.
	OBB this_obb_copy = OBB(*this);
	OBB obb_copy = OBB(*(OBB*)&rObb); // lmao

	// Taking the inverse of the object obb's orientation to transform it into axis aligned.
	XMMATRIX obb_rotation = XMMatrixRotationQuaternion(XMLoadFloat4(&rObb.Orientation));
	XMMATRIX obb_rotation_inverse = XMMatrixInverse(nullptr, obb_rotation);

	// Moving parameter obb to origin and moving this obb the same distance.
	obb_copy.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&this_obb_copy.Center, XMVectorSubtract(XMLoadFloat3(&this->Center), XMLoadFloat3(&rObb.Center)));

	// Transforming both with the inverse orientation, making parameter obb axis aligned.
	this_obb_copy.Transform(obb_rotation_inverse);
	obb_copy.Transform(obb_rotation_inverse);

	// Using copies to get collision info as if the object had an AABB.
	AABB temp_aabb;
	temp_aabb.Center = obb_copy.Center;
	temp_aabb.Extents = obb_copy.Extents;

	// Rotating normal back according to obb orientation for true orientation of normal.
	CollisionInfo return_info = this_obb_copy.GetCollisionInfo(temp_aabb);
	XMStoreFloat3(&return_info.mNormal, XMVector3TransformNormal(XMLoadFloat3(&return_info.mNormal), obb_rotation));
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

	// Saving abs of vector from center to closest to check for largest component.
	XMFLOAT3 abs_center_to_closest;
	XMStoreFloat3(&abs_center_to_closest, XMVectorAbs(XMLoadFloat3(&center_to_closest)));

	CollisionInfo return_info;

	// Normal will be +-y if y is the largest component of center_to_closest,
	// and overlap will be the difference of the half-height and the y component
	// of center_to_closest.
	if (abs_center_to_closest.y > abs_center_to_closest.x && abs_center_to_closest.y > abs_center_to_closest.z)
	{
		return_info.mNormal.y = Sign(center_to_closest.y);
		return_info.mOverlap = rCylinder.mExtentsY - fabsf(center_to_closest.y);
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

BoundingVolume* OBB::Copy()
{
	return new OBB(*this);
}

XMFLOAT3 OBB::GetCenter()
{
	return Center;
}

XMFLOAT3 OBB::GetExtents()
{
	return Extents;
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

XMFLOAT3 AABB::GetExtents()
{
	return Extents;
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
		return GetCollisionInfo(*(BoundingSphere*)p_sphere);
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
	// Calculate closest point of aabb to sphere center (sunglasses emoji).
	XMFLOAT3 closest_point = XMFLOAT3(
		(std::max)(this->Center.x - this->Extents.x, (std::min)(rSphere.Center.x, this->Center.x + this->Extents.x)),
		(std::max)(this->Center.y - this->Extents.y, (std::min)(rSphere.Center.y, this->Center.y + this->Extents.y)),
		(std::max)(this->Center.z - this->Extents.z, (std::min)(rSphere.Center.z, this->Center.z + this->Extents.z))
	);

	// Get a vector from sphere center to closest point.
	XMFLOAT3 center_to_closest;
	XMStoreFloat3(&center_to_closest, XMVectorSubtract(XMLoadFloat3(&closest_point), XMLoadFloat3(&rSphere.Center)));
	
	CollisionInfo return_info;
	
	// Normal will be the normalized direction from sphere center to closest point of aabb.
	XMStoreFloat3(&return_info.mNormal, XMVector3Normalize(XMLoadFloat3(&center_to_closest)));

	// Overlap is the diff in length between the previous vector and the radius of the sphere.
	return_info.mOverlap = rSphere.Radius - XMVectorGetX(XMVector3Length(XMLoadFloat3(&center_to_closest)));

	return return_info;
}

CollisionInfo AABB::GetCollisionInfo(BoundingBox& rAabb)
{
	// Getting vector from center of rAabb to this aabb
	XMFLOAT3 rAabb_center_to_this_center;
	XMStoreFloat3(&rAabb_center_to_this_center, XMVectorSubtract(XMLoadFloat3(&this->Center), XMLoadFloat3(&rAabb.Center)));
	
	XMFLOAT3 abs_param_to_this;
	XMStoreFloat3(&abs_param_to_this, XMVectorAbs(XMLoadFloat3(&rAabb_center_to_this_center)));

	CollisionInfo return_info;

	// Finding largest component of vector to get 
	// normal and subsequently overlap in the normal's direction.
	if (abs_param_to_this.x > abs_param_to_this.z && abs_param_to_this.x > abs_param_to_this.y)
	{
		return_info.mNormal.x = Sign(rAabb_center_to_this_center.x);
		return_info.mOverlap = this->Extents.x + rAabb.Extents.x - abs_param_to_this.x;
	}
	else if (abs_param_to_this.z > abs_param_to_this.y && abs_param_to_this.z > abs_param_to_this.x)
	{
		return_info.mNormal.z = Sign(rAabb_center_to_this_center.z);
		return_info.mOverlap = this->Extents.z + rAabb.Extents.z - abs_param_to_this.z;
	}
	else
	{
		return_info.mNormal.y = Sign(rAabb_center_to_this_center.y);
		return_info.mOverlap = this->Extents.y + rAabb.Extents.y - abs_param_to_this.y;
	}

	return return_info;
}

CollisionInfo AABB::GetCollisionInfo(BoundingOrientedBox& rObb)
{
	// This one is gonna be a lil' weird.
	CollisionInfo return_info = ((OBB*)&rObb)->GetCollisionInfo(*this);
	XMStoreFloat3(&return_info.mNormal, -XMLoadFloat3(&return_info.mNormal));

	return return_info;
}

CollisionInfo AABB::GetCollisionInfo(BoundingCylinder& rCylinder)
{
	XMVECTOR cyl_to_aabb_center = XMVectorSubtract(XMLoadFloat3(&this->Center), XMLoadFloat3(&rCylinder.mCenter));
	
	XMFLOAT3 xmf_cyl_to_aabb_center;
	XMStoreFloat3(&xmf_cyl_to_aabb_center, cyl_to_aabb_center);

	XMFLOAT3 abs_cyl_to_aabb_center;
	XMStoreFloat3(&abs_cyl_to_aabb_center, XMVectorAbs(cyl_to_aabb_center));

	CollisionInfo return_info;

	if (abs_cyl_to_aabb_center.y > abs_cyl_to_aabb_center.x && abs_cyl_to_aabb_center.y > abs_cyl_to_aabb_center.z)
	{
		return_info.mNormal.y = Sign(xmf_cyl_to_aabb_center.y);
		return_info.mOverlap = rCylinder.mExtentsY + this->Extents.y - fabsf(xmf_cyl_to_aabb_center.y);
	}
	else
	{
		// Find closest x and z to cylinder center.
		float x = (std::max)(this->Center.x - this->Extents.x, (std::min)(rCylinder.mCenter.x, this->Center.x + this->Extents.x));
		float z = (std::max)(this->Center.z - this->Extents.z, (std::min)(rCylinder.mCenter.z, this->Center.z + this->Extents.z));

		XMFLOAT3 xz_cyl_center_to_closest = XMFLOAT3(x - rCylinder.mCenter.x, 0.0f, z - rCylinder.mCenter.z);

		XMVECTOR normalized = XMVector3Normalize(XMLoadFloat3(&xz_cyl_center_to_closest));
		XMStoreFloat3(&return_info.mNormal, normalized);

		float length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&xz_cyl_center_to_closest)));
		return_info.mOverlap = rCylinder.mRadius - length;
	}
	return return_info;
}

BoundingVolume* AABB::Copy()
{
	return new AABB(*this);
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

XMFLOAT3 Cylinder::GetExtents()
{
	return XMFLOAT3(mRadius, mExtentsY, mRadius);
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
	// Get closest point of sphere edge to cylinder center.
	XMVECTOR closest_to_cyl_center = XMLoadFloat3(&rSphere.Center) + 
		rSphere.Radius / XMVectorGetX(XMVector3Length(XMLoadFloat3(&this->mCenter) - XMLoadFloat3(&rSphere.Center))) * 
		(XMLoadFloat3(&this->mCenter) - XMLoadFloat3(&rSphere.Center));


	XMVECTOR sphere_center_to_closest = closest_to_cyl_center - XMLoadFloat3(&rSphere.Center);

	CollisionInfo return_info;
	
	XMStoreFloat3(&return_info.mNormal, XMVector3Normalize(sphere_center_to_closest));

	XMFLOAT3 abs_s_to_c_center;
	XMStoreFloat3(&abs_s_to_c_center, XMVectorAbs(closest_to_cyl_center));
	
	XMVECTOR x_z_center_to_center = XMVectorSetY(closest_to_cyl_center, 0.0f);
	float x_z_length = XMVectorGetX(XMVector3Length(x_z_center_to_center));
	float y_length = abs_s_to_c_center.y;

	// If y is largest component of sphere_to_cyl_center (or normal) then check overlap in height.
	if (abs_s_to_c_center.y > abs_s_to_c_center.x && abs_s_to_c_center.y > abs_s_to_c_center.z)
	{
		return_info.mOverlap = this->mExtentsY - y_length;
	}
	// Otherwise check with radius.
	else
	{
		return_info.mOverlap = this->mRadius - x_z_length;
	}

	return return_info;
}

CollisionInfo Cylinder::GetCollisionInfo(BoundingBox& rAabb)
{
	XMFLOAT3 aabb_to_cyl_center;
	XMStoreFloat3(&aabb_to_cyl_center, XMVectorSubtract(XMLoadFloat3(&this->mCenter), XMLoadFloat3(&rAabb.Center)));

	XMFLOAT3 abs_aabb_to_cyl_center;
	XMStoreFloat3(&abs_aabb_to_cyl_center, XMVectorAbs(XMLoadFloat3(&aabb_to_cyl_center)));

	CollisionInfo return_info;

	// Getting normal and overlap by largest component of center-to-center vector.
	if (abs_aabb_to_cyl_center.y > abs_aabb_to_cyl_center.x && abs_aabb_to_cyl_center.y > abs_aabb_to_cyl_center.z)
	{
		return_info.mNormal.y = Sign(aabb_to_cyl_center.y);
		// If y specifically, check against cylinder height.
		return_info.mOverlap = rAabb.Extents.y + this->mExtentsY - aabb_to_cyl_center.y;
	}
	else
	{
		if (abs_aabb_to_cyl_center.x > abs_aabb_to_cyl_center.z)
		{
			return_info.mNormal.x = Sign(aabb_to_cyl_center.x);
		}
		else
		{
			return_info.mNormal.z = Sign(aabb_to_cyl_center.z);
		}
		// Find closest x and z to cylinder center.
		float x = (std::max)(rAabb.Center.x - rAabb.Extents.x, (std::min)(this->mCenter.x, rAabb.Center.x + rAabb.Extents.x));
		float z = (std::max)(rAabb.Center.z - rAabb.Extents.z, (std::min)(this->mCenter.z, rAabb.Center.z + rAabb.Extents.z));

		XMFLOAT3 xz_cyl_center_to_closest = XMFLOAT3(x - this->mCenter.x, 0.0f, z - this->mCenter.z);

		float length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&xz_cyl_center_to_closest)));
		return_info.mOverlap = this->mRadius - length;
	}

	return return_info;
}

CollisionInfo Cylinder::GetCollisionInfo(BoundingOrientedBox& rObb)
{
	// Grabbing the reverse and recomputing the normal.
	CollisionInfo return_info = ((OBB*)&rObb)->GetCollisionInfo(this);

	// Transform standard axes by orientation
	// Project center-to-center on each axis, largest result in size wins.
	XMVECTOR x, y, z;
	x = XMVectorSetX(XMVectorZero(), 1.0f);
	y = XMVectorSetY(XMVectorZero(), 1.0f);
	z = XMVectorSetZ(XMVectorZero(), 1.0f);

	XMMATRIX orientation = XMMatrixRotationQuaternion(XMLoadFloat4(&rObb.Orientation));

	XMVECTOR xt, yt, zt;
	xt = XMVector3TransformNormal(x, orientation);
	yt = XMVector3TransformNormal(y, orientation);
	zt = XMVector3TransformNormal(z, orientation);

	XMVECTOR obb_to_aabb_center = XMVectorSubtract(XMLoadFloat3(&this->mCenter), XMLoadFloat3(&rObb.Center));

	float length_xt = XMVectorGetX(XMVector3Dot(xt, obb_to_aabb_center));
	float length_yt = XMVectorGetX(XMVector3Dot(yt, obb_to_aabb_center));
	float length_zt = XMVectorGetX(XMVector3Dot(zt, obb_to_aabb_center));

	if (length_xt > length_yt && length_xt > length_zt)
	{
		XMStoreFloat3(&return_info.mNormal, xt);
	}
	else if (length_yt > length_xt && length_yt > length_zt)
	{
		XMStoreFloat3(&return_info.mNormal, yt);
	}
	else
	{
		XMStoreFloat3(&return_info.mNormal, zt);
	}

	return return_info;
}

CollisionInfo Cylinder::GetCollisionInfo(BoundingCylinder& rCylinder)
{
	XMFLOAT3 other_to_this_center;
	XMStoreFloat3(&other_to_this_center, XMVectorSubtract(XMLoadFloat3(&this->mCenter), XMLoadFloat3(&rCylinder.mCenter)));

	XMFLOAT3 abs_other_to_this_center;
	XMStoreFloat3(&abs_other_to_this_center, XMVectorAbs(XMLoadFloat3(&other_to_this_center)));

	CollisionInfo return_info;

	float length_center_to_center = XMVectorGetX(XMVector3Length(XMLoadFloat3(&other_to_this_center)));

	// If y is the largest component, get overlap and normal in y.
	if (abs_other_to_this_center.y > abs_other_to_this_center.x && abs_other_to_this_center.y > abs_other_to_this_center.z)
	{
		return_info.mNormal.y = Sign(other_to_this_center.y);
		return_info.mOverlap = this->mExtentsY + rCylinder.mExtentsY - abs_other_to_this_center.y;
	}
	else
	{
		XMVECTOR xz_center_to_center = XMVectorSetY(XMLoadFloat3(&other_to_this_center), 0.0f);
		float xz_length = XMVectorGetX(XMVector3Length(xz_center_to_center));

		XMStoreFloat3(&return_info.mNormal, XMVector3Normalize(xz_center_to_center));
		return_info.mOverlap = this->mRadius + rCylinder.mRadius - xz_length;
	}

	return return_info;
}

BoundingVolume* Cylinder::Copy()
{
	return new Cylinder(*this);
}
